"""Compare retail and compiled preparation with explicit downstream helper mocks."""

import hashlib, json, os, random, struct, sys
from functools import cache
from pathlib import Path
import unicorn as U
from unicorn import x86_const as X

CHECKOUT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(CHECKOUT / "tools"))
from xbe import Xbe, retail_xbe_path
from pe import Pe
from linkmap import LinkMap, plain_name
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
import re

retail = Xbe(retail_xbe_path())
compiled = Pe(os.environ.get("DECOMP_PROBE_IMAGE", str(CHECKOUT / "build/halo2.exe")))
link = LinkMap.read(
    os.environ.get("DECOMP_PROBE_MAP", str(CHECKOUT / "build/halo2.map"))
)


def report_path(source):
    return Path(os.environ["DECOMP_PROBE_REPORT_DIR"]) / Path(source).name


@cache
def symbol(name):
    hits = [s.va for s in link.symbols if plain_name(s.name) == name]
    assert len(hits) == 1, (name, hits)
    return hits[0]


DATA = 0x1000000
STACK = 0x1108000
STOP = 0x1208000
TAG = DATA
DEF = DATA + 0x100
PLACE = DATA + 0x300
DIRECTION = DATA + 0x400
TRANSFORM = DATA + 0x500
PREP = DATA + 0x600
STATE = DATA + 0x800
RANDOM = DATA + 0x900
TIME = DATA + 0xA00
GROUP = DATA + 0xB00
BITMAP = DATA + 0xC00
SEQUENCE = DATA + 0x1000
FRAME = DATA + 0x2000
GLOBALS = DATA + 0x3000
rng = random.Random(0x17EF10)


def make(reader):
    u = U.Uc(U.UC_ARCH_X86, U.UC_MODE_32)
    u.mem_map(0x10000, 0x1200000)
    for s in reader.sections:
        if s.rsize:
            u.mem_write(s.va, reader.data[s.raw : s.raw + s.rsize])
    return u


r = make(retail)
c = make(compiled)


def put(u, a, fmt, *values):
    u.mem_write(a, struct.pack("<" + fmt, *values))


def get(u, a, fmt):
    return struct.unpack("<" + fmt, u.mem_read(a, struct.calcsize("<" + fmt)))


def reg(u, name):
    return u.reg_read(getattr(X, "UC_X86_REG_" + name))


def ret(u, value, pop):
    sp = reg(u, "ESP")
    target = get(u, sp, "I")[0]
    u.reg_write(X.UC_X86_REG_EAX, value & 0xFFFFFFFF)
    u.reg_write(X.UC_X86_REG_ESP, sp + 4 + pop)
    u.reg_write(X.UC_X86_REG_EIP, target)


helpers = {
    0x142390: "function_142390",
    0x17CB00: "function_17cb00",
    0x12CCF0: "texture_cache_bitmap_get_texture",
    0x12CE00: "function_12ce00",
    0x17D9A0: "function_17d9a0",
}
# Compiled helper conventions are filled from current linked instructions below.
compiled_helpers = {symbol(name): address for address, name in helpers.items()}
trace = {r: [], c: []}
real_geometry = "--real-geometry" in sys.argv
real_textures = "--real-textures" in sys.argv


def hook(u, address, size, unused):
    retail_address = address if u is r else compiled_helpers.get(address)
    if retail_address not in helpers:
        return
    sp = reg(u, "ESP")
    trace[u].append(retail_address)
    if real_textures and retail_address in (0x12CCF0, 0x12CE00):
        assert retail_address != 0x12CE00, (
            "Unexpected texture fallback in resident fixture"
        )
        return
    if real_geometry and retail_address in (0x142390, 0x17CB00, 0x17D9A0):
        return
    if retail_address == 0x142390:
        if u is r:
            plane, out = reg(u, "ESI"), reg(u, "EDI")
            pop = 0
        else:
            plane, out = reg(u, "ESI"), reg(u, "EDI")
            pop = 0
        assert plane == PLACE + 0x28
        put(u, out, "13f", 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0)
        ret(u, 0, pop)
    elif retail_address == 0x17CB00:
        if u is r:
            definition, uv = reg(u, "EDI"), reg(u, "EAX")
            seq, frame, scale, bounds = get(u, sp + 4, "IIfI")
            pop = 16
        else:
            definition, uv = reg(u, "EDI"), reg(u, "EAX")
            seq, frame, scale, bounds = get(u, sp + 4, "IIfI")
            pop = 16
        assert definition == DEF and frame == 0
        if sprite_success:
            put(u, uv, "4f", 0.1, 0.9, 0.2, 0.8)
            put(u, bounds, "4f", -scale, scale, -scale * 0.5, scale * 0.5)
        ret(u, sprite_success, pop)
    elif retail_address == 0x12CCF0:
        # Retail uses ESI bitmap, EDX flags, XMM1 bias.
        if u is r:
            bitmap, flags, pop = reg(u, "ESI"), reg(u, "EDX"), 0
        else:
            bitmap, flags, pop = reg(u, "ESI"), reg(u, "EDX"), 0
        assert bitmap == BITMAP and flags == 2
        ret(u, 0x123000 if lookup_success else 0, pop)
    elif retail_address == 0x12CE00:
        if u is r:
            bitmap, flags = get(u, sp + 4, "II")
            pop = 8
        else:
            bitmap, flags = reg(u, "ECX"), reg(u, "EDX")
            pop = 4
            assert get(u, sp + 4, "f")[0] == 0
        assert bitmap == BITMAP and flags == 2
        ret(u, 0x123000 if fallback_success else 0, pop)
    else:
        if u is r:
            matrix = reg(u, "EBX")
            bounds, out = get(u, sp + 4, "II")
            pop = 8
        else:
            matrix = reg(u, "EDX")
            bounds, out = get(u, sp + 4, "II")
            pop = 8
        assert (matrix, bounds, out) == (PREP + 0xA8, PREP + 0xDC, PREP)
        # Mirror inputs into prefix to expose incorrect ordering or buffer pointers.
        u.mem_write(
            out,
            bytes(u.mem_read(matrix, 0x34))
            + bytes(u.mem_read(bounds, 16))
            + bytes(0x48),
        )
        ret(u, 0, pop)


for u in (r, c):
    u.hook_add(U.UC_HOOK_CODE, hook)


def initialize(u, payload):
    u.mem_write(DATA, payload)
    u.mem_write(STACK - 0x1000, bytes(0x2000))
    for rr in ("EAX", "EBX", "ECX", "EDX", "EBP", "ESI", "EDI"):
        u.reg_write(getattr(X, "UC_X86_REG_" + rr), 0)
    u.reg_write(X.UC_X86_REG_ESP, STACK)
    u.reg_write(X.UC_X86_REG_EFLAGS, 2)
    u.reg_write(X.UC_X86_REG_MXCSR, 0x1F80)
    u.reg_write(X.UC_X86_REG_FPCW, 0x37F)
    for address, value, name in [
        (0x4E3B44, TAG, "g_4e3b44"),
        (0x4E7408, RANDOM, "g_4e7408"),
        (0x510C54, TIME, "g_510c54"),
        (0x4EA94C, GLOBALS, "g_4ea94c"),
    ]:
        put(u, address if u is r else symbol(name), "I", value)
    for address, value, name in [
        (0x46DD4C, 3, "g_46dd4c"),
        (0x485AA0, 123, "g_485aa0"),
        (0x4E6488, 5, "g_4e6488"),
    ]:
        put(u, address if u is r else symbol(name), "I", value)
    put(u, 0x510C80 if u is r else symbol("g_510c80"), "3f", 7, 8, 9)
    trace[u] = []


standin = symbol("standin_unknown_17d9a0_7")
code = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(compiled.read(standin, 0x60), standin))
loads = [
    int(m.group(1), 16)
    for i in code
    if (m := re.search(r", (?:dword|byte) ptr \[(0x[0-9a-f]+)\]", i.op_str))
]
args_symbol = min(loads)
assert sorted(loads) == list(range(args_symbol, args_symbol + 160, 16))
stores = [
    int(m.group(1), 16)
    for i in code
    if (m := re.search(r"byte ptr \[(0x[0-9a-f]+)\], al", i.op_str))
]
assert len(stores) == 1
result = stores[0]
if __name__ == "__main__":
    passed = 0
    coverage = {
        "success": 0,
        "failure": 0,
        "plane_basis": 0,
        "sprite_bounds": 0,
        "lookup": 0,
        "fallback": 0,
        "quad": 0,
        "cache_changed": 0,
    }
    for case in range(2048):
        flags = [0, 8, 0x28, 0x100, 0x108, 0x128][case % 6]
        reuse = rng.randrange(2)
        density = case % 5
        skip_texture = bool(rng.randrange(2))
        sprite_success = bool(rng.randrange(2))
        lookup_success = bool(rng.randrange(2))
        fallback_success = bool(rng.randrange(2))
        bitmap_type = 3 if rng.randrange(2) else 0
        tag = 0x12340000
        u = r
        u.mem_write(DATA, bytes(0x8000))
        put(u, TAG + 8, "I", DEF)
        put(u, TAG + 24, "I", GROUP)
        put(u, DEF, "H", flags)
        put(u, DEF + 6, "H", density)
        put(u, DEF + 0x10, "3f", 0.25, 1.5, 0 if case % 3 == 0 else 1.25)
        put(u, DEF + 0x38, "f", 0 if case % 3 == 1 else 2)
        put(u, DEF + 0x8C, "I", 1)
        put(u, DEF + 0xA4, "f", 2)
        put(u, PLACE + 8, "3f", 1, 2, 3)
        normal = (
            [rng.uniform(-1, 1) for _ in range(3)]
            if case % 4
            else [(1.0 if k == case % 3 else 0.0) for k in range(3)]
        )
        put(u, PLACE + 0x28, "4f", *normal, 1.25)
        put(u, DIRECTION, "3f", *[rng.uniform(-1, 1) for _ in range(3)])
        put(
            u,
            TRANSFORM,
            "13f",
            [0.5, 1, 2][case % 3],
            0.6,
            0.8,
            0,
            -0.8,
            0.6,
            0,
            0,
            0,
            1,
            10,
            20,
            30,
        )
        put(u, STATE, "B", reuse)
        put(u, STATE + 4, "5f", 0.3, 0.4, 0.5, 0.6, 1.2)
        put(u, STATE + 24, "i", case % 7 - 2)
        put(u, RANDOM + 4, "I", rng.getrandbits(32))
        put(u, TIME, "B", 1)
        put(u, TIME + 4, "fi", 0.5, 10)
        put(u, GROUP, "H", bitmap_type)
        put(u, GROUP + 0x3C, "4I", [0, 1, 4][case % 3], SEQUENCE, 1, BITMAP)
        put(u, BITMAP + 4, "hh", 64, 32)
        put(u, BITMAP + 0x50, "I", 0x123000 if rng.randrange(2) else 0)
        put(u, BITMAP + 0x70, "I", rng.randrange(4, 8))
        for i in range(4):
            put(u, SEQUENCE + i * 0x3C + 0x34, "II", 1, FRAME)
        put(u, FRAME, "h", 0)
        put(u, FRAME + 8, "6f", 0.1, 0.9, 0.2, 0.8, 0.3, 0.4)
        for i in range(24):
            a = GLOBALS + 0x380C + i * 0x2C
            put(
                u,
                a,
                "I3ff3fIfI",
                0 if case % 3 == 0 else (tag if i % 3 else 0x12340001),
                1,
                2,
                3,
                0.5,
                0,
                0,
                0,
                0,
                [0, 4, 5, 6][(case + i) % 4],
                100 - i,
            )
        payload = bytes(u.mem_read(DATA, 0x8000))
        transform = TRANSFORM if case % 4 == 0 else 0
        args = [
            transform,
            tag,
            PLACE,
            DIRECTION,
            0 if case % 4 == 0 else 0x3F800000,
            int(skip_texture),
            0xFFFFFFFF if case % 4 else 2,
            int(case % 7 == 0),
            PREP,
            STATE,
        ]
        outputs = []
        for u in (r, c):
            initialize(u, payload)
            if u is r:
                put(u, STACK, "10I", STOP, *args[1:])
                u.reg_write(X.UC_X86_REG_ECX, transform)
                u.emu_start(0x17EF10, STOP, count=30000)
                value = reg(u, "EAX") & 255
                assert reg(u, "ESP") == STACK + 40
            else:
                for i, value in enumerate(args):
                    put(u, args_symbol + i * 16, "I", value)
                put(u, STACK, "I", STOP)
                u.emu_start(standin, STOP, count=30000)
                value = get(u, result, "B")[0]
                assert reg(u, "ESP") == STACK + 4
            assert reg(u, "EIP") == STOP
            outputs.append((value, bytes(u.mem_read(DATA, 0x8000)), trace[u]))
        if outputs[0] != outputs[1]:
            a, b = outputs
            diffs = [
                (hex(i), a[1][i], b[1][i])
                for i in range(len(a[1]))
                if a[1][i] != b[1][i]
            ]
            print(
                "FAIL",
                case,
                flags,
                reuse,
                density,
                "return",
                a[0],
                b[0],
                "trace",
                a[2],
                b[2],
                "diffs",
                diffs[:80],
            )
            sys.exit(1)
        passed += 1
        coverage["success" if outputs[0][0] else "failure"] += 1
        for address, key in [
            (0x142390, "plane_basis"),
            (0x17CB00, "sprite_bounds"),
            (0x12CCF0, "lookup"),
            (0x12CE00, "fallback"),
            (0x17D9A0, "quad"),
        ]:
            coverage[key] += outputs[0][2].count(address)
        coverage["cache_changed"] += (
            outputs[0][1][0x680C:0x6C2C] != payload[0x680C:0x6C2C]
        )
    report = {
        "passed": True,
        "cases": passed,
        "image_sha256": hashlib.sha256(compiled.data).hexdigest(),
        "retail_sha256": hashlib.sha256(retail.data).hexdigest(),
        "real_geometry": real_geometry,
        "coverage": coverage,
        "scope": "Actual retail and compiled preparation, full synthetic input/output memory, return and helper order. Texture lookup/fallback are mocked at independently inspected ABI boundaries. Plane basis, sprite bounds and quad preparation execute actual helper instructions when real_geometry is true; otherwise they are mocked. No GPU/gameplay claim.",
    }
    report_path(__file__).with_suffix(
        ".real-geometry.json" if real_geometry else ".json"
    ).write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))
