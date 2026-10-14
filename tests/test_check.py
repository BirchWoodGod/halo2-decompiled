import json

import pytest

from build import Marked
from check import (Identity, check_function, check_unique_markers, compare, extract, masked_offsets, parse_addresses,
                   relative_ok, resolve, StandinCalls, write_report)
from linkmap import LinkMap

MAP = """ Preferred load address is 00400000
 0001:00000000 00000100H .text                   CODE
 0001:00000000       ?f@@YIXH@Z                 00401000 f   a.obj
 0001:00000040       ?f@@YIXM@Z                 00401040 f   a.obj
 0001:00000080       ?g@@YIXXZ                  00401080 f   a.obj
"""


def marked(name):
    return Marked('src/a.cpp', 0x1000, name, 'void', [])


def test_resolve_symbol():
    assert resolve(LinkMap(MAP), marked('g')).va == 0x401080


def test_resolve_vcall_thunk_by_its_vtable_offset():
    linkmap = LinkMap(MAP + ' 0001:000000c0       ??_9@$BFM@AE               004010c0 f i a.obj\n')
    assert resolve(linkmap, Marked('src/a.cpp', 0x234c5f, "`vcall'{0x5c}", '', [], kind='vcall')).va == 0x4010c0


def test_resolve_symbol_ambiguous():
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(MAP), marked('f'))
    assert '?f@@YIXH@Z' in str(e.value) and '?f@@YIXM@Z' in str(e.value)


def test_resolve_symbol_missing_is_none():
    # folded into an identical function or dropped: reported as todo, not fatal
    assert resolve(LinkMap(MAP), marked('h')) is None


def test_extract_strips_fill_after_retail_length():
    assert extract(bytes.fromhex('c3cccccc'), bytes.fromhex('c3')) == bytes.fromhex('c3')


def test_extract_keeps_real_cc_bytes():
    assert extract(bytes.fromhex('ebcccccc'), bytes.fromhex('ebcc')) == bytes.fromhex('ebcc')


def test_compare_masks_only_listed_fields():
    ours = bytes.fromhex('a1' '00204000' 'c3')
    theirs = bytes.fromhex('a1' '88e75500' 'c3')
    assert compare(ours, theirs, masked_offsets(0x401000, 6, {0x401001})) is None
    assert compare(ours, theirs, set()) == 1


def test_compare_unequal_lengths():
    assert compare(bytes.fromhex('33c0c3'), bytes.fromhex('33c0'), set()) == 2


def test_relative_fields_must_leave_in_retail():
    call_out = bytes.fromhex('e8' '10000000' 'c3')     # call va+0x15, outside a 6-byte function
    jump_self = bytes.fromhex('e9' 'fbffffff' 'c3')    # jmp va, inside
    assert relative_ok(call_out, 0x401000, call_out, 0x1000, 1)
    assert not relative_ok(call_out, 0x401000, jump_self, 0x1000, 1)


LO, HI = 0x10000, 0x600000


def run(full, start, theirs, theirs_va=0x1000, absolute=(), relative=()):
    return check_function(full, start, theirs, theirs_va, set(absolute), set(relative), LO, HI)


def test_absolute_field_must_be_an_address_in_retail():
    ours = bytes.fromhex('b8' '00204000' 'c3')
    status, first, _, _ = run(ours, 0x401000, bytes.fromhex('b8' '2a000000' 'c3'), absolute={0x401001})
    assert first == 1 and status == 'near'
    assert run(ours, 0x401000, bytes.fromhex('b8' '00204000' 'c3'), absolute={0x401001})[:2] == ('matched', None)


def test_field_straddling_the_start_is_not_masked():
    status, first, _, _ = run(bytes.fromhex('c3'), 0x401000, bytes.fromhex('90'), relative={0x400ffd})
    assert first == 0 and status == 'near'


def test_disjoint_field_is_ignored():
    assert run(bytes.fromhex('c3'), 0x401000, bytes.fromhex('c3'), relative={0x400ff0})[1] is None


def test_failed_relative_field_checked_even_when_bytes_equal():
    jump_self = bytes.fromhex('e9' 'fbffffff' 'c3')
    jump_out = bytes.fromhex('e9' '10000000' 'c3')
    status, first, _, _ = run(jump_out, 0x401000, jump_self, 0x1000, relative={0x401001})
    assert first == 1 and status == 'near'


def test_failed_field_counts_as_differing_instruction():
    # two nops differ from retail; with the failed jmp that is three instructions
    ours = bytes.fromhex('e9' '10000000' '90' '90' 'c3')
    theirs = bytes.fromhex('e9' 'fbffffff' '91' '91' 'c3')
    assert run(ours, 0x401000, theirs, relative={0x401001})[0] == 'todo'
    same_jmp = theirs[:5] + ours[5:]
    assert run(same_jmp, 0x401000, theirs)[0] == 'near'


def test_incomplete_disassembly_is_todo():
    # 0f is not decodable on its own
    assert run(bytes.fromhex('0f'), 0x401000, bytes.fromhex('c3'))[0] == 'todo'


def test_ours_longer_with_fill_compares_retail_length():
    assert run(bytes.fromhex('ebcc' 'cccc'), 0x401000, bytes.fromhex('ebcc'))[:2] == ('matched', None)


@pytest.mark.parametrize('opcode, fixups', [('b8', 'absolute'), ('e8', 'relative')])
def test_field_beyond_retail_length_is_not_masked(opcode, fixups):
    ours = bytes.fromhex(opcode + '00204000' + 'c3')
    status, first, _, _ = run(ours, 0x401000, bytes.fromhex(opcode + '90' + 'c3'), **{fixups: {0x401001}})
    assert first == 1 and status == 'todo'


def test_jump_table_entries_must_map_to_the_same_offsets():
    # jmp [eax*4+table]; three case bodies (nop, nop, ret); table of three absolute entries
    def func(base, order):
        body = bytes.fromhex('ff2485' + (base + 12).to_bytes(4, 'little').hex() + '90' '90' 'c3')
        # code is 10 bytes (0..9), table at 12? pad two bytes so the table sits at offset 12
        return body + bytes.fromhex('9090') + b''.join((base + o).to_bytes(4, 'little') for o in order)
    ours = func(0x401000, (7, 8, 9))
    fields = {0x401000 + 12, 0x401000 + 16, 0x401000 + 20, 0x401000 + 3}
    same = func(0x1000, (7, 8, 9))
    assert run(ours, 0x401000, same, absolute=fields)[:2] == ('matched', None)
    swapped = func(0x1000, (7, 9, 8))
    assert run(ours, 0x401000, swapped, absolute=fields)[1] == 16


def _jump_table_function(base, order, case=b'\x90'):
    """jmp [eax*4+table]; three case bodies (case, nop, ret); two bytes of fill; a table of three absolute
    entries at offset 12, pointing at the case bodies in order."""
    body = bytes.fromhex('ff2485') + (base + 12).to_bytes(4, 'little') + case + bytes.fromhex('90' 'c3')
    return body + bytes.fromhex('9090') + b''.join((base + o).to_bytes(4, 'little') for o in order)


JUMP_TABLE_FIELDS = {0x401000 + 3, 0x401000 + 12, 0x401000 + 16, 0x401000 + 20}


def _differing_lines(lines):
    """The differing lines of a listing, as (ours, retail) texts."""
    return [tuple(side.strip() for side in line[5:].split('|')) for line in lines if line.startswith('  !!')]


def test_jump_table_entries_that_differ_only_by_layout_do_not_count():
    # the entries hold different addresses (the functions sit at different places) but the same offsets; only
    # the changed case body differs, and the table is compared as dwords, not decoded as instructions
    ours = _jump_table_function(0x401000, (7, 8, 9))
    theirs = _jump_table_function(0x1000, (7, 8, 9), case=b'\x91')
    status, first, lines, _ = run(ours, 0x401000, theirs, absolute=JUMP_TABLE_FIELDS)
    assert first == 7 and status == 'near'
    assert _differing_lines(lines) == [('nop', 'xchg ecx, eax')]
    assert sum(line[5:].startswith('dd ') for line in lines) == 3


def test_jump_table_entries_that_differ_count_once_each():
    ours = _jump_table_function(0x401000, (7, 8, 9))
    theirs = _jump_table_function(0x1000, (7, 9, 8), case=b'\x91')
    status, first, lines, _ = run(ours, 0x401000, theirs, absolute=JUMP_TABLE_FIELDS)
    assert first == 7 and status == 'todo'
    assert _differing_lines(lines) == [('nop', 'xchg ecx, eax'), ('dd 0x401008', 'dd 0x1009'),
                                       ('dd 0x401009', 'dd 0x1008')]


def test_self_recursive_call_matches_when_retail_calls_itself():
    ours = bytes.fromhex('e8' 'fbffffff' 'c3')      # call own start
    assert run(ours, 0x401000, ours, relative={0x401001})[:2] == ('matched', None)


def test_call_elsewhere_inside_retail_is_a_difference():
    ours = bytes.fromhex('e8' 'fbffffff' 'c3')
    theirs = bytes.fromhex('e8' '00000000' 'c3')    # calls its own ret
    assert run(ours, 0x401000, theirs, relative={0x401001})[1] == 1


def test_parse_addresses_rejects_non_hex():
    assert parse_addresses(['163ba0', '0x259d0']) == {0x163ba0, 0x259d0}
    with pytest.raises(SystemExit) as e:
        parse_addresses(['function_163ba0'])
    assert 'not a hexadecimal' in str(e.value)


def test_duplicate_retail_markers_are_rejected():
    a, b = Marked('src/a.cpp', 0x1000, 'f', 'void', []), Marked('src/b.cpp', 0x1000, 'g', 'void', [])
    check_unique_markers([a, Marked('src/a.cpp', 0x2000, 'h', 'void', [])])
    with pytest.raises(SystemExit) as e:
        check_unique_markers([a, b])
    assert 'duplicate @retail 0x1000' in str(e.value)


def test_check_reads_standard_markers_like_any_other(tmp_path):
    """check.py takes its markers from build.marked_sources, so a "standard"
    marker is checked, and counts toward duplicates, like a plain one."""
    import build
    (tmp_path / 'src').mkdir()
    (tmp_path / 'src' / 'a.cpp').write_text('// @retail 0x1000 standard\nlong __stdcall f(long a)\n{\n}\n')
    (tmp_path / 'src' / 'b.cpp').write_text('// @retail 0x1000\nlong g(long a)\n{\n}\n')
    found = build.marked_sources(str(tmp_path))
    assert [(m.path, m.retail, m.name, m.standard) for m in found] == [
        ('src/a.cpp', 0x1000, 'f', True), ('src/b.cpp', 0x1000, 'g', False)]
    assert build.standin_names(found[:1]) == {('src/a.cpp', 0x1000): 'standin_a_0'}
    with pytest.raises(SystemExit, match='duplicate @retail 0x1000'):
        check_unique_markers(found)


def test_filtered_run_merges_into_the_report(tmp_path):
    path = str(tmp_path / 'report.json')
    write_report(path, {'a': 1, 'b': 1}, merge=False)
    write_report(path, {'b': 2}, merge=True)
    assert json.load(open(path)) == {'a': 1, 'b': 2}
    write_report(path, {'c': 3}, merge=False)
    assert json.load(open(path)) == {'c': 3}


IDENT_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000000       ?caller@@YAXXZ             00401000 f   a.obj
 0001:00000100       ?function_163ba0@@YIXPAK@Z 00401100 f   a.obj
 0001:00000200       _strncmp                   00401200 f   libcmt:strncmp.obj
"""
CALL_START = 0x401000
CALLEE = Marked('src/a.cpp', 0x2100, 'function_163ba0', 'void', ['unsigned long *'])


def call_to(base, target):
    """A function at base that calls target, then returns."""
    return bytes.fromhex('e8') + ((target - (base + 5)) & 0xFFFFFFFF).to_bytes(4, 'little') + bytes.fromhex('c3')


def run_call(retail_target, markers=(CALLEE,), rows=None, our_target=0x401100):
    identity = Identity(LinkMap(IDENT_MAP), rows or {}, list(markers))
    return check_function(call_to(CALL_START, our_target), CALL_START, call_to(0x1000, retail_target), 0x1000,
                          set(), {CALL_START + 1}, LO, HI, identity)


def test_call_to_a_marked_callee_matches_when_retail_targets_its_address():
    assert run_call(0x2100)[:2] == ('matched', None)


def test_call_to_a_marked_callee_with_the_wrong_address_differs_at_the_call_field():
    assert run_call(0x2200)[1] == 1


def test_call_to_a_marked_retail_function_must_reach_its_symbol():
    # retail calls 0x2100, which src/ claims; ours calls something else
    assert run_call(0x2100, our_target=0x401200)[1] == 1


def test_call_to_a_library_function_matches_by_plain_name():
    rows = {0x2200: {'name': '_strncmp'}}
    assert run_call(0x2200, markers=(), rows=rows, our_target=0x401200)[:2] == ('matched', None)


def test_call_to_a_library_function_with_another_name_differs():
    rows = {0x2200: {'name': '_strcmp'}}
    assert run_call(0x2200, markers=(), rows=rows, our_target=0x401200)[1] == 1


def test_call_to_an_aliased_library_function_matches():
    # libcmt exports the stack probe as both _chkstk and __alloca_probe
    alias_map = IDENT_MAP.replace('_strncmp                  ', '__chkstk                  ')
    identity = Identity(LinkMap(alias_map), {0x2200: {'name': '__alloca_probe'}}, [])
    result = check_function(call_to(CALL_START, 0x401200), CALL_START, call_to(0x1000, 0x2200), 0x1000,
                            set(), {CALL_START + 1}, LO, HI, identity)
    assert result[:2] == ('matched', None)


def test_call_to_memmove_matches_retail_memcpy():
    # libcmt's memcpy.obj and memmove.obj hold the same code; retail's copy is named _memcpy
    alias_map = IDENT_MAP.replace('_strncmp                  ', '_memmove                  ')
    identity = Identity(LinkMap(alias_map), {0x2200: {'name': '_memcpy'}}, [])
    result = check_function(call_to(CALL_START, 0x401200), CALL_START, call_to(0x1000, 0x2200), 0x1000,
                            set(), {CALL_START + 1}, LO, HI, identity)
    assert result[:2] == ('matched', None)


@pytest.mark.parametrize('twin_first', [False, True])
def test_call_to_a_folded_function_matches_through_any_of_its_names(twin_first):
    # the linker folded two identical functions into one body; the map lists both at its address
    twin = ' 0001:00000100       ?twin@@YIXPAK@Z            00401100 f   b.obj\n'
    lines = IDENT_MAP.splitlines(keepends=True)
    lines.insert(3 if twin_first else 4, twin)
    markers = [CALLEE, Marked('src/b.cpp', 0x2300, 'twin', 'void', ['unsigned long *'])]
    identity = Identity(LinkMap(''.join(lines)), {}, markers)

    def result(retail_target):
        return check_function(call_to(CALL_START, 0x401100), CALL_START, call_to(0x1000, retail_target), 0x1000,
                              set(), {CALL_START + 1}, LO, HI, identity)[:2]
    assert result(0x2100) == ('matched', None)
    assert result(0x2300) == ('matched', None)
    assert result(0x2200)[1] == 1


def test_call_between_unnamed_functions_is_accepted():
    assert run_call(0x2200, markers=(), rows={0x2200: {'name': ''}}, our_target=0x401200)[:2] == ('matched', None)



CTOR_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000000       ?caller@@YAXXZ             00401000 f   a.obj
 0001:00000100       ??0c_foo@@QAE@XZ           00401100 f   a.obj
 0001:00000200       ??0c_foo@@QAE@ABV0@@Z      00401200 f i b.obj
"""
FOO = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', [], cls='c_foo', kind='constructor')


def test_constructor_marker_ignores_the_implicit_copy_constructor():
    # another source's stand-in emits c_foo's implicit copy constructor, with the same plain name
    assert resolve(LinkMap(CTOR_MAP), FOO).va == 0x401100


def test_copy_constructor_marker_stays_ambiguous():
    copy = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', ['const c_foo &'], cls='c_foo', kind='constructor')
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(CTOR_MAP), copy)
    assert 'ambiguous' in str(e.value)


OVERLOAD_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000000       ??0c_foo@@QAE@G@Z          00401000 f   a.obj
 0001:00000100       ??0c_foo@@QAE@J@Z          00401100 f   a.obj
 0001:00000200       ??0c_foo@@QAE@PAX@Z        00401200 f   a.obj
"""


@pytest.mark.parametrize('param, va', [('word', 0x401000), ('unsigned short', 0x401000), ('long', 0x401100)])
def test_overloaded_constructors_are_told_apart_by_scalar_parameters(param, va):
    marked = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', [param], cls='c_foo', kind='constructor')
    assert resolve(LinkMap(OVERLOAD_MAP), marked).va == va


def test_overloaded_constructor_with_a_pointer_parameter_stays_ambiguous():
    marked = Marked('src/a.cpp', 0x2100, 'c_foo::c_foo', '', ['void *'], cls='c_foo', kind='constructor')
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(OVERLOAD_MAP), marked)
    assert 'ambiguous' in str(e.value)


def test_call_to_a_marked_constructor_ignores_its_implicit_copy_constructor():
    identity = Identity(LinkMap(CTOR_MAP), {}, [FOO])

    def result(our_target):
        return check_function(call_to(CALL_START, our_target), CALL_START, call_to(0x1000, 0x2100), 0x1000,
                              set(), {CALL_START + 1}, LO, HI, identity)[:2]
    assert result(0x401100) == ('matched', None)
    assert result(0x401200)[1] == 1  # the copy constructor is not the marked one


VECTOR_CONSTRUCTOR = '??_H@YGXPAXIHP6EPAX0@Z@Z'
HELPER_MAP = IDENT_MAP.replace('_strncmp                  ', VECTOR_CONSTRUCTOR)
ITERATOR = Marked('src/b.cpp', 0x2200, 'vector_constructor_iterator', 'void', [])


def call_helper(marker=ITERATOR):
    """Ours calls the compiler's ??_H; retail calls 0x2200, which src/ claims with marker."""
    identity = Identity(LinkMap(HELPER_MAP), {0x2200: {'name': ''}}, [marker])
    return check_function(call_to(CALL_START, 0x401200), CALL_START, call_to(0x1000, 0x2200), 0x1000,
                          set(), {CALL_START + 1}, LO, HI, identity)


def test_call_to_the_compilers_helper_matches_the_marker_with_its_plain_name():
    # src/ marks retail's ??_H as vector_constructor_iterator; the compiler still emits and calls its own ??_H
    assert call_helper()[:2] == ('matched', None)


def test_call_to_a_claimed_function_differs_unless_it_is_the_same_compiler_helper():
    rows = {0x2200: {'name': '_strncmp'}}
    assert run_call(0x2200, markers=(ITERATOR,), rows=rows, our_target=0x401200)[1] == 1
    other = Marked('src/b.cpp', 0x2200, 'vector_destructor_iterator', 'void', [])
    assert call_helper(other)[1] == 1  # a different helper

# Overloads with pointer parameters: find_marked's rules leave them ambiguous,
# and each marker's stand-in calls its own overload.
STANDIN_MAP = """ Preferred load address is 00400000
 0001:00000000 00000300H .text                   CODE
 0001:00000100       ??0c_text@@QAE@PBD@Z       00401100 f   ui.obj
 0001:00000140       ??0c_text@@QAE@PB_W@Z      00401140 f   ui.obj
 0001:00000200       ?standin_ui_0@@YIXXZ       00401200 f   ui.obj
 0001:00000220       ?standin_ui_1@@YIXXZ       00401220 f   ui.obj
 0001:00000240       ?standin_ui_2@@YIXXZ       00401240 f   ui.obj
"""
WIDE = Marked('src/ui.cpp', 0x2bac52, 'c_text::c_text', '', ['const wchar_t *'], cls='c_text', kind='constructor')
NARROW = Marked('src/ui.cpp', 0x2bac6a, 'c_text::c_text', '', ['const char *'], cls='c_text', kind='constructor')
CALLER = Marked('src/ui.cpp', 0x2baeb1, 'make_texts', 'void', [])


class FakeImage:
    """Our image: stand-in k is the k-th marker of src/ui.cpp, and calls that marker's overload."""

    def __init__(self, code):
        self.code = code

    def read(self, va, size):
        return self.code.get(va, b'').ljust(size, b'\xcc')[:size]


def standin_calls():
    image = FakeImage({0x401200: call_to(0x401200, 0x401140), 0x401220: call_to(0x401220, 0x401100)})
    return StandinCalls(LinkMap(STANDIN_MAP), image, [WIDE, NARROW, CALLER])


def test_overloads_are_told_apart_by_what_their_stand_ins_call():
    linkmap, calls = LinkMap(STANDIN_MAP), standin_calls()
    assert resolve(linkmap, WIDE, calls).name == '??0c_text@@QAE@PB_W@Z'
    assert resolve(linkmap, NARROW, calls).name == '??0c_text@@QAE@PBD@Z'


def test_pointer_overloads_without_stand_in_calls_stay_ambiguous():
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(STANDIN_MAP), WIDE)
    assert 'ambiguous' in str(e.value)


def test_calls_to_overloads_reach_the_marked_one():
    # retail's caller calls the wide constructor (0x2bac52); ours must call that overload, not the narrow one
    identity = Identity(LinkMap(STANDIN_MAP), {}, [WIDE, NARROW], standin_calls())

    def result(our_target):
        return check_function(call_to(CALL_START, our_target), CALL_START, call_to(0x1000, 0x2bac52), 0x1000,
                              set(), {CALL_START + 1}, LO, HI, identity)[:2]
    assert result(0x401140) == ('matched', None)
    assert result(0x401100)[1] == 1
