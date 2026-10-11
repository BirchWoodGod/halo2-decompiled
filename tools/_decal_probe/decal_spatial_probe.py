"""Actual decal spatial registration against retail with cluster search and
reference allocation mocked. Covers list limits, original-cluster skipping,
zero radius, absent clusters and allocation failure; no actual BSP search.
"""

import hashlib, itertools, json, sys
from pathlib import Path
import decal_preparation_probe as p
from unicorn import x86_const as X

POOL = p.DATA + 0x7000
RECORDS = p.DATA + 0x20000
GLOBALS = p.DATA + 0x30000
real_reference = "--real-reference" in sys.argv
real_pool = "--real-pool" in sys.argv
real_physical = "--real-physical" in sys.argv
real_bsp = "--real-bsp" in sys.argv
record_base = 0 if "--real-records" in sys.argv else 1
calls = {p.r: [], p.c: []}
allocations = {p.r: 0, p.c: 0}
targets = {
    p.symbol("function_14a5b0"): 0x14A5B0,
    p.symbol("function_17d100"): 0x17D100,
    p.symbol("record_pool_allocate"): 0x16B840,
}


def hook(u, address, size, unused):
    a = address if u is p.r else targets.get(address)
    sp = p.reg(u, "ESP")
    if a == 0x14A5B0:
        cluster, point, radius, capacity, out = p.get(u, sp + 4, "IIfII")
        if real_bsp:
            calls[u].append(("query", cluster & 65535, radius, capacity))
            return
        assert cluster & 65535 == 7 and point == p.PLACE + 8 and capacity == 512
        calls[u].append(("query", cluster & 65535, radius, capacity))
        for i in range(min(max(returned, 0), 512)):
            p.put(u, out + 2 * i, "h", 7 if i % 3 == 0 else (20 + i) % 512)
        p.ret(u, returned, 20)
    elif a == 0x17D100:
        cluster, datum = p.get(u, sp + 4, "2I")
        assert 0x4321 <= datum >> 16 < 0x4340 if real_pool else datum >> 16 == 0x4321
        calls[u].append(("allocate", cluster, datum))
        if real_reference:
            return
        n = allocations[u]
        allocations[u] += 1
        p.ret(u, -1 if n == fail_at else 0x43210001 + n, 8)
    elif a == 0x16B840 and real_reference:
        if real_physical and p.reg(u, "EDX") != POOL:
            return
        assert p.reg(u, "EDX") == POOL
        n = allocations[u]
        allocations[u] += 1
        if real_pool:
            return
        p.ret(u, -1 if n == fail_at else 0x43210000 + record_base + n, 0)


for u in (p.r, p.c):
    u.hook_add(p.U.UC_HOOK_CODE, hook)
returned = 3
fail_at = -1
if __name__ == "__main__":
    coverage = {
        "cases": 0,
        "queries": 0,
        "allocation_attempts": 0,
        "changed_next_fields": 0,
    }
    for radius_squared, cluster, returned, fail_at, source_flags in itertools.product(
        (0.0, 0.0001, 4.0, 1000000.0),
        (-1, 7),
        (-1, 0, 1, 2, 10, 512, 513, 32767),
        (-1, 0, 1, 100),
        (0, 1, 2, 3) if real_reference else (0,),
    ):
        results = []
        for u, entry in ((p.r, 0x180B60), (p.c, p.symbol("function_180b60"))):
            p.initialize(u, bytes(0x8000))
            u.mem_write(RECORDS, bytes([0xCD]) * 0x8100)
            u.mem_write(GLOBALS, bytes([0xFF]) * 0x3800 + bytes(0x500))
            p.put(u, RECORDS + 2, "BB", source_flags, 2)
            p.put(u, 0x4EA94C if u is p.r else p.symbol("g_4ea94c"), "I", GLOBALS)
            p.put(u, p.PLACE + 8, "3f", 1, 2, 3)
            p.put(u, p.PLACE + 0x20, "h", cluster)
            p.put(u, POOL + 0x44, "I", RECORDS)
            for va, name, value, fmt in [
                (0x4EA950, "g_4ea950", POOL, "I"),
                (0x4E7414, "g_4e7414", 123, "I"),
                (0x4E7411, "g_4e7411", 0, "B"),
            ]:
                p.put(u, va if u is p.r else p.symbol(name), fmt, value)
            calls[u] = []
            allocations[u] = 0
            p.put(u, p.STACK, "IIfI", p.STOP, p.PLACE, radius_squared, 0x43210000)
            u.emu_start(entry, p.STOP, count=100000)
            assert p.reg(u, "EIP") == p.STOP and p.reg(u, "ESP") == p.STACK + 16
            counter = p.get(u, 0x4E7414 if u is p.r else p.symbol("g_4e7414"), "I")[0]
            flag = p.get(u, 0x4E7411 if u is p.r else p.symbol("g_4e7411"), "B")[0]
            results.append(
                (
                    bytes(u.mem_read(RECORDS, 0x8100)),
                    calls[u],
                    counter,
                    flag,
                    bytes(u.mem_read(GLOBALS, 0x3C2C)),
                )
            )
        assert results[0] == results[1], (
            radius_squared,
            cluster,
            returned,
            fail_at,
            results[0][1:],
            results[1][1:],
        )
        coverage["cases"] += 1
        coverage["queries"] += sum(x[0] == "query" for x in calls[p.r])
        coverage["allocation_attempts"] += allocations[p.r]
        coverage["changed_next_fields"] += sum(
            p.get(p.r, RECORDS + i * 64 + 60, "I")[0] != 0xCDCDCDCD for i in range(513)
        )
    report = {
        "passed": True,
        "coverage": coverage,
        "real_reference": real_reference,
        "image_sha256": hashlib.sha256(p.compiled.data).hexdigest(),
        "retail_sha256": hashlib.sha256(p.retail.data).hexdigest(),
        "scope": (
            __doc__.replace(
                "reference allocation mocked",
                "record-pool allocation mocked; actual reference copying and cell linking run",
            )
            if real_reference
            else __doc__
        ),
    }
    p.report_path(__file__).with_suffix(
        ".real-reference.json" if real_reference else ".json"
    ).write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))
