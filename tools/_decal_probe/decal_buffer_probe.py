"""Actual geometry-buffer lookup and vertex-cache command generation.
Uses synthetic resource, block metadata and a nonexhausted command buffer.
Does not execute GPU commands or test command-buffer refill.
"""

import hashlib, itertools, json, re
from pathlib import Path
import decal_preparation_probe as p
from unicorn import x86_const as X

MANAGER = p.DATA + 0x7300
BLOCK_POOL = p.DATA + 0x60000
BLOCKS = BLOCK_POOL + 0x100
RESOURCE = BLOCK_POOL + 0x300
DEVICE = BLOCK_POOL + 0x400
COMMANDS = BLOCK_POOL + 0x500
code = p.Cs(p.CS_ARCH_X86, p.CS_MODE_32).disasm(
    p.compiled.read(p.symbol("function_439d0"), 100), p.symbol("function_439d0")
)
loads = [
    int(m[1], 16)
    for i in code
    if i.mnemonic == "mov"
    and (m := re.fullmatch(r"edi, dword ptr \[(0x[0-9a-f]+)\]", i.op_str))
]
assert len(loads) == 1
COMPILED_DEVICE_GLOBAL = loads[0]


def setup(u, index=2, offset=4, shift=10, clock=12345):
    u.mem_write(BLOCK_POOL, bytes(0x600))
    p.put(u, MANAGER + 0x34, "2I", shift, clock)
    p.put(u, MANAGER + 0x64, "I", BLOCK_POOL)
    p.put(u, BLOCK_POOL + 0x44, "I", BLOCKS)
    p.put(u, BLOCKS + index * 24 + 8, "I", offset)
    p.put(u, RESOURCE + 4, "I", 0x1008000)
    p.put(u, DEVICE, "2I", COMMANDS, COMMANDS + 0x100)
    for va, name, value in [
        (0x509448, "g_509448", MANAGER),
        (0x509444, "g_509444", RESOURCE),
    ]:
        p.put(u, va if u is p.r else p.symbol(name), "I", value)
    p.put(u, 0x407488 if u is p.r else COMPILED_DEVICE_GLOBAL, "I", DEVICE)


if __name__ == "__main__":
    cases = 0
    for index, offset, shift, clock in itertools.product(
        (0, 2), (0, 1, 17), (0, 6, 10), (0, 12345, 0x7FFFFFFF)
    ):
        results = []
        for u, entry in ((p.r, 0x439D0), (p.c, p.symbol("function_439d0"))):
            p.initialize(u, bytes(0x8000))
            setup(u, index, offset, shift, clock)
            p.put(u, p.STACK, "I", p.STOP)
            u.reg_write(X.UC_X86_REG_EAX, 0x12340000 + index)
            u.emu_start(entry, p.STOP, count=1000)
            assert p.reg(u, "EIP") == p.STOP and p.reg(u, "ESP") == p.STACK + 4
            results.append((p.reg(u, "EAX"), bytes(u.mem_read(BLOCK_POOL, 0x600))))
            assert p.reg(u, "EAX") == 0x81008000 + (offset << shift)
            assert (
                p.get(u, COMMANDS, "2I") == (0x41710, 0)
                and p.get(u, DEVICE, "I")[0] == COMMANDS + 8
            )
            assert p.get(u, BLOCKS + index * 24 + 20, "I")[0] == clock
        assert results[0] == results[1]
        cases += 1
    report = {
        "passed": True,
        "cases": cases,
        "image_sha256": hashlib.sha256(p.compiled.data).hexdigest(),
        "scope": __doc__,
    }
    p.report_path(__file__).with_suffix(".json").write_text(
        json.dumps(report, indent=2) + "\n"
    )
    print(json.dumps(report, indent=2))
