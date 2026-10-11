"""Actual sphere/portal cluster traversal against retail on synthetic portal graphs.
Executes portal-plane, sphere and polygon intersection helpers without mocks.
Compares returned count, bounded output, visited markers and fixture memory.
"""

import hashlib, itertools, json, math
from pathlib import Path
import decal_preparation_probe as p
from unicorn import x86_const as X

BASE = p.DATA + 0x40000
BSP = BASE
COLLISION = BASE + 0x100
PLANES = BASE + 0x200
CLUSTERS = BASE + 0x400
PORTALS = BASE + 0x800
INDICES = BASE + 0xB00
VERTICES = BASE + 0xC00
OUT = BASE + 0x1100
GRAPHS = [[(0, 1), (1, 2)], [(0, 1), (1, 2), (2, 0)], [(0, 1), (0, 2), (1, 3), (2, 3)]]


def setup(u, graph):
    u.mem_write(BASE, bytes(0x1200))
    p.put(u, BSP + 0x18, "I", COLLISION)
    p.put(u, BSP + 0x60, "I", PORTALS)
    p.put(u, BSP + 0xA0, "I", CLUSTERS)
    p.put(u, COLLISION + 0xC, "I", PLANES)
    lists = [[] for _ in range(4)]
    for i, (a, b) in enumerate(GRAPHS[graph]):
        x = i * 0.5
        v = VERTICES + i * 48
        p.put(u, PLANES + i * 16, "4f", 1, 0, 0, x)
        p.put(u, PORTALS + i * 36, "hhI4fIII", a, b, i, x, 0, 0, math.sqrt(2), 0, 4, v)
        for j, (y, z) in enumerate([(-1, -1), (1, -1), (1, 1), (-1, 1)]):
            p.put(u, v + j * 12, "3f", x, y, z)
        lists[a].append(i)
        lists[b].append(i)
    for i, links in enumerate(lists):
        p.put(u, CLUSTERS + i * 0xB0 + 0x8C, "2I", len(links), INDICES + i * 16)
        for j, link in enumerate(links):
            p.put(u, INDICES + i * 16 + j * 2, "h", link)
    for va, name, value in [
        (0x4E0348, "g_4e0348", BSP),
        (0x4E033C, "g_4e033c", COLLISION),
        (0x4E7414, "g_4e7414", 1),
    ]:
        p.put(u, va if u is p.r else p.symbol(name), "I", value)
    u.mem_write(0x4E7418 if u is p.r else p.symbol("g_4e7418"), bytes(512 * 4))


if __name__ == "__main__":
    coverage = {
        "cases": 0,
        "reached_clusters": 0,
        "multi_cluster_cases": 0,
        "truncated_cases": 0,
    }
    for graph, x, y, radius, capacity in itertools.product(
        range(3), (-2.0, 0.0, 0.5, 2.0), (0.0, 2.0), (0.0, 0.25, 1.0, 3.0), (0, 1, 2, 4)
    ):
        results = []
        for u, entry in ((p.r, 0x14A5B0), (p.c, p.symbol("function_14a5b0"))):
            p.initialize(u, bytes(0x8000))
            setup(u, graph)
            p.put(u, p.PLACE, "3f", x, y, 0)
            u.mem_write(OUT, bytes([0xCD]) * 64)
            p.put(u, p.STACK, "IIIfII", p.STOP, 0, p.PLACE, radius, capacity, OUT)
            u.emu_start(entry, p.STOP, count=100000)
            assert p.reg(u, "EIP") == p.STOP and p.reg(u, "ESP") == p.STACK + 24
            count = p.reg(u, "EAX") & 65535
            results.append(
                (
                    count,
                    bytes(u.mem_read(BASE, 0x1200)),
                    bytes(
                        u.mem_read(0x4E7418 if u is p.r else p.symbol("g_4e7418"), 2048)
                    ),
                )
            )
        if results[0] != results[1]:
            print(
                "FAIL",
                graph,
                x,
                y,
                radius,
                capacity,
                "count",
                results[0][0],
                results[1][0],
            )
            raise SystemExit(1)
        assert bytes(
            p.r.mem_read(
                OUT + min(capacity, results[0][0]) * 2,
                64 - min(capacity, results[0][0]) * 2,
            )
        ) == bytes([0xCD]) * (64 - min(capacity, results[0][0]) * 2)
        coverage["cases"] += 1
        coverage["reached_clusters"] += results[0][0]
        coverage["multi_cluster_cases"] += int(results[0][0] > 1)
        coverage["truncated_cases"] += int(results[0][0] > capacity)
    assert coverage["multi_cluster_cases"] > 0
    report = {
        "passed": True,
        "coverage": coverage,
        "image_sha256": hashlib.sha256(p.compiled.data).hexdigest(),
        "retail_sha256": hashlib.sha256(p.retail.data).hexdigest(),
        "scope": __doc__,
    }
    p.report_path(__file__).with_suffix(".json").write_text(
        json.dumps(report, indent=2) + "\n"
    )
    print(json.dumps(report, indent=2))
