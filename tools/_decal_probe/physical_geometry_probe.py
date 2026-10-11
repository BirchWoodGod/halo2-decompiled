"""Actual physical block allocation/deletion and record-pool helpers against retail.
Synthetic arenas with no busy/delete callbacks; varies policy, capacity, size,
limits and time so allocation, exhaustion, reuse and eviction can execute.
"""

import hashlib, itertools, json, random
from pathlib import Path
import decal_preparation_probe as p
from unicorn import x86_const as X

MANAGER = p.DATA + 0x7300
POOL = p.DATA + 0x60000
BLOCKS = POOL + 0x100
BITMAP = POOL + 0x280
DELETE = next(
    s.va for s in p.link.symbols if s.name == "?block_delete@s_physical_object@@QAEXJ@Z"
)


def setup(u, pages=32, shift=6, policy=0):
    u.mem_write(MANAGER, bytes(0x70))
    u.mem_write(POOL, bytes(0x300))
    p.put(u, MANAGER + 0x2C, "6I", policy, pages, shift, 1, 0xFFFFFFFF, 0xFFFFFFFF)
    p.put(u, MANAGER + 0x44, "8I", *([0x7FFFFFFF] * 8))
    p.put(u, MANAGER + 0x64, "I", POOL)
    p.put(u, POOL + 0x20, "2I", 8, 24)
    p.put(u, POOL + 0x29, "B", 1)
    p.put(u, POOL + 0x40, "H", 0x8000)
    p.put(u, POOL + 0x44, "2I", BLOCKS, BITMAP)


if __name__ == "__main__":
    rng = random.Random(0x13D370)
    coverage = {
        "sequences": 0,
        "allocations": 0,
        "explicit_deletions": 0,
        "failed_allocations": 0,
        "evicted_blocks": 0,
    }
    for pages, policy in itertools.product((2, 8, 32), (0, 1, 2)):
        for u in (p.r, p.c):
            p.initialize(u, bytes(0x8000))
            setup(u, pages, 6, policy)
        for step in range(128):
            live = [
                (p.get(p.r, BLOCKS + i * 24, "H")[0] << 16) | i
                for i in range(8)
                if p.get(p.r, BITMAP, "I")[0] & (1 << i)
            ]
            delete = bool(live) and step % 7 == 6
            handle = rng.choice(live) if delete else 0
            size = rng.choice((1, 63, 64, 65, 128, 256, 512))
            kind = step % 3
            results = []
            for u in (p.r, p.c):
                p.put(u, MANAGER + 0x38, "I", 1 + step // 4)
                p.put(u, MANAGER + 0x44, "8I", *([0x7FFFFFFF] * 8))
                u.reg_write(X.UC_X86_REG_ESP, p.STACK)
                if delete:
                    p.put(u, p.STACK, "I", p.STOP)
                    u.reg_write(X.UC_X86_REG_EDI, MANAGER)
                    u.reg_write(X.UC_X86_REG_EBX, handle)
                    entry = 0x13D830 if u is p.r else DELETE
                else:
                    p.put(u, p.STACK, "4I", p.STOP, MANAGER, size, kind)
                    entry = 0x13D370 if u is p.r else p.symbol("function_13d370")
                u.emu_start(entry, p.STOP, count=100000)
                assert p.reg(u, "EIP") == p.STOP and p.reg(u, "ESP") == p.STACK + (
                    4 if delete else 16
                )
                results.append(
                    (
                        None if delete else p.reg(u, "EAX"),
                        bytes(u.mem_read(MANAGER, 0x70)),
                        bytes(u.mem_read(POOL, 0x300)),
                    )
                )
            if results[0] != results[1]:
                print(
                    "FAIL",
                    pages,
                    policy,
                    step,
                    delete,
                    size,
                    kind,
                    results[0][0],
                    results[1][0],
                )
                for k in (1, 2):
                    print(
                        k,
                        [
                            (hex(i), a, b)
                            for i, (a, b) in enumerate(
                                zip(results[0][k], results[1][k])
                            )
                            if a != b
                        ][:20],
                    )
                raise SystemExit(1)
            coverage["explicit_deletions" if delete else "allocations"] += 1
            if not delete:
                coverage["failed_allocations"] += int(results[0][0] == 0xFFFFFFFF)
                after = {
                    (p.get(p.r, BLOCKS + i * 24, "H")[0] << 16) | i
                    for i in range(8)
                    if p.get(p.r, BITMAP, "I")[0] & (1 << i)
                }
                coverage["evicted_blocks"] += len(set(live) - after)
        coverage["sequences"] += 1
    report = {
        "passed": True,
        "coverage": coverage,
        "image_sha256": hashlib.sha256(p.compiled.data).hexdigest(),
        "scope": __doc__,
    }
    p.report_path(__file__).with_suffix(".json").write_text(
        json.dumps(report, indent=2) + "\n"
    )
    print(json.dumps(report, indent=2))
