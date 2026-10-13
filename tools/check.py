"""Builds the project (tools/build.py) and compares every function marked
"// @retail 0x..." with the retail XBE, byte for byte apart from address
fields. Our extent comes from the linker map, retail's from
config/functions.csv. The masked fields are exactly those our linker filled
in: base relocations (absolute) and the relative fields /MAPINFO:FIXUPS lists.
Each masked field must also be the same kind of field in retail: an absolute
one holds an address inside the retail image, a relative one leaves the
function.

Writes build/report.json, updates the status and source columns of
config/functions.csv, and prints a summary.

--fast checks the listed functions on a reduced image instead (see
tools/reduced.py): one image holding each function's own source, the sources
of its callees in retail all the way down, those of the functions and globals
its body uses by name, and what the last full build (build/halo2.exe and
build/halo2.map) says pins their calling conventions and which library
objects it takes. Only those sources are compiled, and the link leaves out
the rest of the program, so a check takes seconds, not minutes.
The comparison and its output are the full check's. A CAVEAT line follows a
function when the reduced image may build it differently from the full one.
--fast writes build/report-fast.json and nothing else: the csv and
build/report.json only ever come from a full build, which stays the judge of
what is kept. The image is linked as build/fast/reduced.img, never as an .exe,
and is reused when nothing it was linked from has changed. --jobs compiles
that many sources at once (default 2).

    python tools/check.py [<retail va> ...] [--no-build]
    python tools/check.py --fast <retail va> ... [--jobs N]
"""
import argparse
import json
import os
import re
import sys
import time

from capstone import CS_ARCH_X86, CS_MODE_32, Cs

import build
import reduced
from inventory import check_retail, read_rows, write_rows
from linkmap import plain_name
from xbe import FUNCTIONS_CSV, ROOT, Xbe, retail_xbe_path

NEAR = 2  # differing instructions, at most, for "near"


# A stand-in in another source that copy-constructs a derived class emits the
# base's implicit copy constructor, with the marked constructor's plain name.
COPY_CONSTRUCTOR = '@@QAE@ABV0@@Z'


# MSVC's codes for the scalar parameter types, by the names the sources use.
SCALAR_CODES = {
    'char': 'D', 'signed char': 'C', 'unsigned char': 'E', 'byte': 'E', 'bool': '_N',
    'short': 'F', 'unsigned short': 'G', 'word': 'G', 'int': 'H', 'unsigned int': 'I', 'unsigned': 'I',
    'long': 'J', 'unsigned long': 'K', 'dword': 'K', 'float': 'M', 'real': 'M', 'double': 'N',
}


def _constructor_parameters(decorated):
    """The parameter codes of a decorated constructor ('??0c@@QAE@JG@Z' -> 'JG'), or None."""
    m = re.match(r'^\?\?0.*?@@Q[A-D]E@(.*)@Z$', decorated)
    return m.group(1) if m else None


def find_marked(linkmap, marked):
    """The image symbols with a marked function's plain name. A marked
    constructor that is not itself a copy constructor never means the class's
    copy constructor, so that one is left out when it makes the name ambiguous.
    Overloaded constructors whose parameters are all scalars are told apart by
    their decorated parameter codes."""
    hits = linkmap.find(marked.name)
    own_class = marked.cls.rpartition('::')[2]
    is_copy = (len(marked.params) == 1 and marked.params[0].endswith('&')
               and re.search(rf'\b{re.escape(own_class)}\b', marked.params[0]))
    if len(hits) > 1 and marked.kind == 'constructor' and not is_copy:
        hits = [h for h in hits if h.name != f'??0{own_class}{COPY_CONSTRUCTOR}'] or hits
    if len(hits) > 1 and marked.kind == 'constructor' and marked.params:
        codes = [SCALAR_CODES.get(' '.join(p.split())) for p in marked.params]
        if all(codes):
            hits = [h for h in hits if _constructor_parameters(h.name) == ''.join(codes)] or hits
    return hits


def resolve(linkmap, marked, standin_calls=None):
    """The image symbol of a marked function, or None when the linker left it
    out (folded into an identical function, or unreferenced)."""
    hits = find_overload(linkmap, marked, standin_calls)
    if not hits:
        return None
    if len(hits) > 1:
        raise SystemExit(f'{marked.path}: {marked.name} is ambiguous: ' + ', '.join(h.name for h in hits))
    return hits[0]


class StandinCalls:
    """What each marked function's stand-in (tools/build.py) calls or jumps to
    directly, read from the built image. A stand-in calls exactly the overload
    its marker names, whatever its parameter types."""

    def __init__(self, linkmap, image, marked):
        self.linkmap, self.image = linkmap, image
        self.names = build.standin_names(marked)
        self.md = Cs(CS_ARCH_X86, CS_MODE_32)

    def __call__(self, marked):
        """The direct call and jump targets of marked's stand-in (empty without one)."""
        symbols = self.linkmap.find(self.names.get((marked.path, marked.retail), ''))
        if len(symbols) != 1:
            return set()
        start, end = self.linkmap.extent(symbols[0])
        code = self.image.read(start, end - start)
        return {int(op_str, 16) for _, _, mnemonic, op_str in self.md.disasm_lite(code, start)
                if mnemonic in ('call', 'jmp') and op_str.startswith('0x')}


def find_overload(linkmap, marked, standin_calls=None):
    """find_marked's symbols; when they are overloads that its rules leave
    ambiguous (a pointer or class parameter, a method or a plain function),
    the one the marker's stand-in calls, given standin_calls (a StandinCalls)."""
    hits = find_marked(linkmap, marked)
    if len(hits) > 1 and standin_calls is not None:
        called = standin_calls(marked)
        hits = [h for h in hits if h.va in called] or hits
    return hits


def extract(full, theirs):
    """The code to compare: exactly retail's length when the rest of our
    extent is fill, otherwise the whole extent minus trailing fill."""
    if len(full) >= len(theirs) and set(full[len(theirs):]) <= {0xCC}:
        return full[:len(theirs)]
    return full.rstrip(b'\xcc')


def field_starts(va, size, fixups):
    """Offsets of the 4-byte fields lying wholly inside [va, va+size)."""
    return {f - va for f in fixups if va <= f and f + 4 <= va + size}


def masked_offsets(va, size, fixups):
    return {k + i for k in field_starts(va, size, fixups) for i in range(4)}


def straddling_offsets(va, size, fixups):
    """Offsets covered by fields that cross an edge of the function."""
    offsets = set()
    for f in fixups:
        if f < va + size and f + 4 > va and not (va <= f and f + 4 <= va + size):
            offsets.update(k for k in range(f - va, f - va + 4) if 0 <= k < size)
    return offsets


def _dword(data, k, signed=False):
    return int.from_bytes(data[k:k + 4], 'little', signed=signed)


def relative_ok(ours, start, theirs, theirs_va, k):
    """Whether the relative field at offset k agrees with retail. A target
    inside our function must be inside retail's at the same offset; any other
    must leave retail's function."""
    mine = start + k + 4 + _dword(ours, k, signed=True)
    target = theirs_va + k + 4 + _dword(theirs, k, signed=True)
    if start <= mine < start + len(ours):
        return target - theirs_va == mine - start
    return not theirs_va <= (target & 0xFFFFFFFF) < theirs_va + len(theirs)


def same_callee(ours, start, theirs, theirs_va, k, identity):
    """Whether the relative field at offset k, if it leaves the function, reaches the same function as retail."""
    mine = (start + k + 4 + _dword(ours, k, signed=True)) & 0xFFFFFFFF
    target = (theirs_va + k + 4 + _dword(theirs, k, signed=True)) & 0xFFFFFFFF
    if identity is None or start <= mine < start + len(ours):
        return True
    return identity.ok(mine, target)


def absolute_ok(ours, start, theirs, theirs_va, k, lo, hi):
    """Whether the absolute field at offset k agrees with retail. When either
    side points inside its own function (a jump table entry) both must point
    at the same offset; otherwise retail's must be an address inside [lo, hi)."""
    mine, value = _dword(ours, k), _dword(theirs, k)
    if start <= mine < start + len(ours) or theirs_va <= value < theirs_va + len(theirs):
        return value - theirs_va == mine - start
    return lo <= value < hi


def compare(ours, theirs, masked):
    for k in range(min(len(ours), len(theirs))):
        if k not in masked and ours[k] != theirs[k]:
            return k
    return None if len(ours) == len(theirs) else min(len(ours), len(theirs))


def _text(ins):
    return f'{ins.mnemonic} {ins.op_str}' if ins else ''


def differing_instructions(ours, ours_va, theirs, theirs_va, masked, forced=frozenset()):
    """Returns (count, lines, complete). complete is False when the
    disassembly does not cover every byte of both sides."""
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    a = list(md.disasm(ours, ours_va))
    b = list(md.disasm(theirs, theirs_va))
    complete = sum(i.size for i in a) == len(ours) and sum(i.size for i in b) == len(theirs)
    lines, count = [], 0
    for k in range(max(len(a), len(b))):
        x, y = (a[k] if k < len(a) else None), (b[k] if k < len(b) else None)
        same = x is not None and y is not None and x.size == y.size and all(
            (x.address - ours_va + i) not in forced and
            ((x.address - ours_va + i) in masked or x.bytes[i] == y.bytes[i]) for i in range(x.size))
        count += not same
        lines.append(f'  {"  " if same else "!!"} '
                     f'{_text(x):<44} | {_text(y)}')
    return count, lines, complete


# Library routines exported under two names; the linker resolves both to one function
ALIASES = {'chkstk': 'alloca_probe', 'memmove': 'memcpy'}

# MSVC's special names for the helpers the compiler generates itself, and the
# plain name a source in src/ gives the helper when it recreates retail's copy.
COMPILER_HELPERS = {'??_H': 'vector_constructor_iterator', '??_I': 'vector_destructor_iterator',
                    '??_L': 'eh_vector_constructor_iterator', '??_M': 'eh_vector_destructor_iterator'}


def compiler_helper(decorated):
    """The plain name src/ uses for the compiler helper a symbol is ('??_H@...'), else None."""
    return COMPILER_HELPERS.get(decorated[:4]) if decorated.startswith('??_') else None


class Identity:
    """Whether a call or jump to another function reaches the same function in
    our image and in retail. Our target is a map symbol, retail's a row of
    config/functions.csv; markers (@retail and @stub) tie the two together.
    A retail row's name, where known, comes from a library signature."""

    def __init__(self, linkmap, rows, markers, standin_calls=None):
        self.linkmap, self.rows = linkmap, rows
        self.address_of = {}  # our symbol's name -> the retail address its marker gives
        self.claimed = {m.retail: m.name for m in markers}  # retail address -> its marker's plain name
        for m in markers:
            hits = find_overload(linkmap, m, standin_calls)
            if len(hits) == 1:
                self.address_of[hits[0].name] = m.retail

    def ok(self, ours, theirs):
        # several names when the linker folded identical functions into one
        symbols = self.linkmap.symbols_at(ours)
        unmarked = [s for s in symbols if s.name not in self.address_of]
        if any(self.address_of[s.name] == theirs for s in symbols if s.name in self.address_of):
            return True
        if symbols and not unmarked:
            return False
        if theirs in self.claimed:
            # the compiler's own copy of a helper (??_H, ...) that src/ marks under its plain name
            # matches; otherwise src/ says another function is retail's target
            return any(compiler_helper(s.name) == self.claimed[theirs] for s in unmarked)
        name = self.rows.get(theirs, {}).get('name')
        if unmarked and name:
            theirs_name = plain_name(name)
            theirs_name = ALIASES.get(theirs_name, theirs_name)
            return any(ALIASES.get(n, n) == theirs_name for n in (plain_name(s.name) for s in unmarked))
        return True


def check_function(full, start, theirs, theirs_va, absolute, relative, lo, hi, identity=None):
    """Compares one function. Returns (status, first difference or None,
    disassembly lines, length of our code). identity, if given, also checks
    that each call out of the function reaches the same function as retail."""
    ours = extract(full, theirs)
    # a field counts only when it lies wholly inside both extents
    size = min(len(ours), len(theirs))
    fixups = [*absolute, *relative]
    failed = {k for k in field_starts(start, size, absolute)
              if not absolute_ok(ours, start, theirs, theirs_va, k, lo, hi)}
    failed |= {k for k in field_starts(start, size, relative)
               if not relative_ok(ours, start, theirs, theirs_va, k)
               or not same_callee(ours, start, theirs, theirs_va, k, identity)}
    forced = straddling_offsets(start, size, fixups)
    for k in failed:
        forced.update(range(k, k + 4))
    masked = masked_offsets(start, size, fixups) - forced
    first = compare(ours, theirs, masked)
    offsets = [k for k in forced if k < size]
    if first is not None:
        offsets.append(first)
    if not offsets:
        return 'matched', None, [], len(ours)
    count, lines, complete = differing_instructions(ours, start, theirs, theirs_va, masked, forced)
    near = complete and len(ours) == len(theirs) and count <= NEAR
    return ('near' if near else 'todo'), min(offsets), lines, len(ours)


_within = reduced.fields_within  # the part of sorted fixups in [low, high)


def parse_addresses(texts):
    try:
        return {int(a, 16) for a in texts}
    except ValueError:
        bad = next(a for a in texts if not re.fullmatch(r'(0[xX])?[0-9a-fA-F]+', a))
        raise SystemExit(f'{bad!r} is not a hexadecimal retail address')


def check_unique_markers(marked):
    seen = {}
    for m in marked:
        if m.retail in seen:
            raise SystemExit(f'duplicate @retail {m.retail:#x}: {seen[m.retail].path} {seen[m.retail].name} '
                             f'and {m.path} {m.name}')
        seen[m.retail] = m


def write_report(path, report, merge):
    """Writes the report; a filtered run (merge) updates the entries it checked
    and keeps the rest of an existing report."""
    if merge and os.path.exists(path):
        with open(path, encoding='utf-8') as f:
            report = {**json.load(f), **report}
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8') as f:
        json.dump(report, f, indent=1, sort_keys=True)


def check_one(m, row, ours, retail, identity, standin_calls):
    """Compares the marked function m (row, its config/functions.csv row) with
    retail in a built image (ours, a reduced.Image), and prints MATCH, DIFF
    with a listing, or MISSING. Returns (its report entry, its symbol in the
    image or None)."""
    symbol = resolve(ours.linkmap, m, standin_calls)
    if symbol is None:
        print(f'MISSING {m.retail:08x} {m.name}: not in the image '
              '(the linker folded it into an identical function, or nothing references it)')
        return dict(name=m.name, source=m.path, status='todo', size=int(row['size']), ours=0, first_difference=0), None
    start, end = ours.linkmap.extent(symbol)
    theirs = retail.read(m.retail, int(row['size']))
    window = (start - 3, end)  # every fixup that touches the function
    status, first, lines, length = check_function(
        ours.image.read(start, end - start), start, theirs, m.retail,
        _within(ours.absolute, *window), _within(ours.relative, *window),
        retail.base, max(s.va + s.vsize for s in retail.sections), identity)
    if first is not None:
        print(f'DIFF   {m.retail:08x} {m.name} at +{first:#x} ({length} bytes, retail {len(theirs)})')
        print('\n'.join(lines))
    else:
        print(f'MATCH  {m.retail:08x} {m.name}')
    return dict(name=m.name, source=m.path, status=status, size=len(theirs), ours=length, first_difference=first), symbol


FAST_IMAGE = os.path.join('build', 'fast', 'reduced.img')
FAST_PARSED = os.path.join('build', 'fast', 'parsed.pickle')  # build.load_parse_cache
FAST_REPORT = os.path.join('build', 'report-fast.json')
FAST_JOBS = 2  # sources --fast compiles at once
NO_FULL_BUILD = ('no full build (build/halo2.exe and build/halo2.map), so the reduced image leaves out the library '
                 'objects and the address-taking sources one would add; run python tools/check.py once')


def same_code(a, a_symbol, b, b_symbol):
    """Whether two images (reduced.Image) hold the same code for the two
    symbols: the same length and the same bytes, apart from address fields."""
    x, x_masked = a.code(a_symbol)
    y, y_masked = b.code(b_symbol)
    return compare(x, y, x_masked | y_masked) is None


def fast_caveats(ours, symbol, full, identity):
    """What may make a reduced image (ours, a reduced.Image) build the function
    at symbol differently from a full build: calls and references to globals
    that the reduced image leaves out (unresolved: the full build may inline
    such a function, or build differently around such a global), and code that
    differs from the last full build (full, a reduced.FullBuild, or None) for
    the function itself or a function it calls, where their sources and those
    of their own callees went into that build unchanged."""
    direct, calls = ours.callees(symbol)
    unresolved = ours.unresolved_globals(symbol)
    notes = []
    if calls or unresolved:
        what = [f'{n} {kind}{"" if n == 1 else "s"}' for n, kind in ((calls, 'call'), (unresolved, 'global reference'))
                if n]
        notes.append(f'{" and ".join(what)} reach{"es" if calls + unresolved == 1 else ""} what the reduced image '
                     'leaves out (unresolved); the full build may inline such a function, or build differently '
                     'around such a global')
    if full is None:
        return notes
    for s in [symbol, *direct]:
        twin = full.counterpart(s)
        if (twin is None or reduced.library_object(s.object)
                or not all(map(full.unchanged, [s, *(direct if s is symbol else ours.callees(s)[0])]))
                or same_code(ours, s, full, twin)):
            continue
        if s is symbol:
            notes.append('it builds differently here than in the last full build of the same source; '
                         'trust the full check for it')
        else:
            va = identity.address_of.get(s.name)
            name = f'{va:#x}' if va is not None else plain_name(s.name)
            notes.append(f'its callee {name} builds differently here than in the last full build '
                         '(link-time code generation outside the reduced image); trust the full check for it')
    return notes


def fast_check(wanted, retail, jobs, root=ROOT, started=None):
    """--fast: checks the functions marked at the retail addresses in wanted on
    one reduced image (see the module docstring), prints the full check's lines
    for each, with CAVEAT lines, and writes build/report-fast.json. Returns the
    exit status: 1 when one differs. started: when the run started
    (time.perf_counter), for the time line."""
    started = time.perf_counter() if started is None else started
    build.load_parse_cache(os.path.join(root, FAST_PARSED))
    rows = read_rows(os.path.join(root, 'config', 'functions.csv'))
    marked, stubs = build.marked_sources(root), build.stub_sources(root)
    markers = marked + stubs
    check_unique_markers(markers)
    unmarked = wanted - {m.retail for m in marked}
    if unmarked:
        print('no @retail marker in src/ for: ' + ', '.join(f'{a:#x}' for a in sorted(unmarked)))
        return 1
    targets = [m for m in marked if m.retail in wanted]
    for m in targets:
        if m.retail not in rows:
            raise SystemExit(f'{m.path}: @retail {m.retail:#x} is not a function start in config/functions.csv')
    units = set().union(*(reduced.reduced_units(m.retail, rows, markers) for m in targets))
    caveats = {m.retail: [] for m in targets}
    full, include = reduced.FullBuild.load(root), ()
    if full is None:
        print(f'CAVEAT {NO_FULL_BUILD}')
    else:
        include = reduced.library_symbols(full.linkmap)
        full_calls = StandinCalls(full.linkmap, full.image, marked)
        found = []
        for m in targets:
            hits = find_overload(full.linkmap, m, full_calls)
            if len(hits) == 1:
                found.append(hits[0])
            else:
                caveats[m.retail].append('not in the last full build, so the sources that take its address '
                                         'there are unknown')
        sources = build.source_objects(root)
        units |= reduced.pinning_units(full.linkmap, full.image, found, sources)
        units |= reduced.named_units(reduced.bodies(targets, root), full.linkmap, sources)
    timings = {}
    image_path = os.path.join(root, FAST_IMAGE)
    map_path = build.build(root, units=units, image=image_path, include=include, jobs=jobs, reuse=True,
                           timings=timings)
    built = time.perf_counter()
    build.save_parse_cache(os.path.join(root, FAST_PARSED))

    ours = reduced.Image.read(map_path, image_path)
    standin_calls = StandinCalls(ours.linkmap, ours.image, marked)
    identity = Identity(ours.linkmap, rows, markers, standin_calls)
    report, failed = {}, 0
    for m in targets:
        entry, symbol = check_one(m, rows[m.retail], ours, retail, identity, standin_calls)
        failed += entry['first_difference'] is not None
        notes = caveats[m.retail] + (fast_caveats(ours, symbol, full, identity) if symbol else [])
        for note in notes:
            print(f'CAVEAT {m.retail:08x} {m.name}: {note}')
        report[f'{m.retail:08x}'] = dict(entry, caveats=([NO_FULL_BUILD] if full is None else []) + notes)
    write_report(os.path.join(root, FAST_REPORT), report, merge=False)
    done = time.perf_counter()

    print(f'fast check: {len(targets) - failed} of {len(targets)} match on a reduced image of {len(units)} sources; '
          'only the full check (python tools/check.py) updates config/functions.csv')
    link, compiled = timings['link'], timings['compiled']
    print(f'time {done - started:.1f}s: prepare {built - started - timings["compile"] - (link or 0):.1f}s, '
          f'compile {compiled} source{"" if compiled == 1 else "s"} {timings["compile"]:.1f}s, '
          f'link {"kept from the last run" if link is None else f"{link:.1f}s"}, compare {done - built:.1f}s')
    return 1 if failed else 0


def main():
    # `@file` reads arguments one per line, so long address lists stay off the
    # command line (antivirus heuristics flag long runs of hex arguments)
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0], fromfile_prefix_chars='@')
    ap.add_argument('addresses', nargs='*')
    ap.add_argument('--no-build', action='store_true')
    ap.add_argument('--fast', action='store_true',
                    help='check the listed functions on a reduced image; writes only build/report-fast.json')
    ap.add_argument('--jobs', type=int, default=None, help=f'with --fast: sources compiled at once (default {FAST_JOBS})')
    args = ap.parse_args()
    started = time.perf_counter()
    if args.fast and not args.addresses:
        ap.error('--fast checks the functions it is given: list their retail addresses')
    if args.fast and args.no_build:
        ap.error('--fast builds its own reduced image; --no-build is for the full check')
    if args.jobs is not None and (not args.fast or args.jobs < 1):
        ap.error('--jobs takes a positive count, and needs --fast')
    retail_path = retail_xbe_path()
    if not os.path.exists(retail_path):
        raise SystemExit(f'retail XBE not found at {retail_path}: put it at orig/default.xbe or set RETAIL_XBE')
    check_retail(retail_path)
    wanted = parse_addresses(args.addresses)
    if args.fast:
        sys.exit(fast_check(wanted, Xbe(retail_path), args.jobs or FAST_JOBS, started=started))
    map_path = os.path.join(ROOT, 'build', build.MAP_NAME)
    exe_path = os.path.join(ROOT, 'build', build.EXE_NAME)
    if args.no_build:
        if not os.path.exists(map_path) or not os.path.exists(exe_path):
            raise SystemExit('--no-build needs a previous build (build/halo2.exe and build/halo2.map); run without it first')
    else:
        map_path = build.build()

    ours = reduced.Image.read(map_path, exe_path)
    retail = Xbe(retail_path)
    rows = read_rows(FUNCTIONS_CSV)

    marked = build.marked_sources()
    stubs = build.stub_sources()
    check_unique_markers(marked + stubs)
    standin_calls = StandinCalls(ours.linkmap, ours.image, marked)
    identity = Identity(ours.linkmap, rows, marked + stubs, standin_calls)
    unmarked = wanted - {m.retail for m in marked}
    if unmarked:
        print('no @retail marker in src/ for: ' + ', '.join(f'{a:#x}' for a in sorted(unmarked)))
        sys.exit(1)
    report, failed, claimed = {}, 0, set()
    for m in marked:
        if wanted and m.retail not in wanted:
            continue
        row = rows.get(m.retail)
        if row is None:
            raise SystemExit(f'{m.path}: @retail {m.retail:#x} is not a function start in config/functions.csv')
        entry, _ = check_one(m, row, ours, retail, identity, standin_calls)
        claimed.add(m.retail)
        failed += entry['first_difference'] is not None
        row['status'], row['source'] = entry['status'], m.path
        report[f'{m.retail:08x}'] = entry
    if not wanted:
        for va, r in rows.items():
            if r['source'] and va not in claimed:
                r['source'], r['status'] = '', 'todo'
    write_rows(FUNCTIONS_CSV, list(rows.values()))
    write_report(os.path.join(ROOT, 'build', 'report.json'), report, merge=bool(wanted))

    def summary(label, subset):
        done = [r for r in subset if r['status'] == 'matched']
        total, matched = sum(int(r['size']) for r in subset), sum(int(r['size']) for r in done)
        print(f'matched {len(done)} of {len(subset)} {label} '
              f'({matched} of {total} bytes, {100 * matched / max(total, 1):.2f}%)')

    summary('game functions', [r for r in rows.values() if r['owner'] == 'game'])
    summary('functions in scope',
            [r for r in rows.values() if r['owner'] != 'eh' and not r['owner'].startswith('third:')])
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()
