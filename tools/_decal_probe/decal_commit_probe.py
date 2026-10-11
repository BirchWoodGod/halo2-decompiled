"""Retail comparison of actual decal commit, packing, color and transform logic.
Allocator, record creation, buffer lookup, lighting and spatial registration are
mocked at independently disassembled boundaries. No GPU or gameplay proof.
"""

import hashlib, json, random, struct, sys
from pathlib import Path
import decal_preparation_probe as p
from unicorn import x86_const as X

OUTPUT = p.DATA + 0x1000
real_spatial = "--real-spatial" in sys.argv
real_records = "--real-records" in sys.argv
real_buffer = "--real-buffer" in sys.argv
real_physical = "--real-physical" in sys.argv
if real_physical:
    import physical_geometry_probe as physical_probe
real_lighting = "--real-lighting" in sys.argv
if real_buffer:
    import decal_buffer_probe as buffer_probe
if real_spatial:
    import decal_spatial_probe as spatial

    spatial.real_reference = True
POOL = p.DATA + 0x7000
RECORD = p.DATA + (0x20000 if "--real-bsp" in sys.argv else 0x7100)
ALLOC = p.DATA + 0x7300
PACKED = 0x81008000 if real_physical else 0x81009000 if real_buffer else p.DATA + 0x9000
if real_buffer:
    for u in (p.r, p.c):
        u.mem_map(0x81000000, 0x20000)
addresses = {
    0x13D370: "function_13d370",
    0x17CFA0: "function_17cfa0",
    0x439D0: "function_439d0",
    0xD47D0: "function_d47d0",
    0x180B60: "function_180b60",
}
compiled = {p.symbol(name): a for a, name in addresses.items()}
delete = next(
    s.va for s in p.link.symbols if s.name == "?block_delete@s_physical_object@@QAEXJ@Z"
)
compiled[delete] = 0x13D830
trace = {p.r: [], p.c: []}


def hook(u, address, size, unused):
    a = address if u is p.r else compiled.get(address)
    if a not in (*addresses, 0x13D830):
        return
    sp = p.reg(u, "ESP")
    if a == 0x13D370:
        allocator, count, flags = p.get(u, sp + 4, "3I")
        assert allocator == ALLOC and flags == 1
        trace[u].append(("allocate", count, flags))
        if real_physical:
            return
        p.ret(u, -1 if failure == 1 else 0x12340002, 12)
    elif a == 0x17CFA0:
        if u is p.r:
            previous, block = p.reg(u, "EAX"), p.reg(u, "ECX")
            cluster, kind, flag = p.get(u, sp + 4, "3I")
            pop = 12
        else:
            block = p.reg(u, "EAX")
            previous, cluster, kind, flag = p.get(u, sp + 4, "4I")
            pop = 16
        assert previous == 0xFFFFFFFF and (real_physical or block == 0x12340002)
        trace[u].append(("record", cluster & 65535, kind & 65535, flag & 255))
        if real_records:
            return
        p.ret(u, -1 if failure == 2 else 0x43210000, pop)
    elif a == 0x439D0:
        assert real_physical or p.reg(u, "EAX") == 0x12340002
        trace[u].append(("buffer",))
        if real_buffer:
            return
        p.ret(u, 0 if failure == 3 else PACKED, 0)
    elif a == 0xD47D0:
        query = p.get(u, sp + 4, "I")[0]
        assert query == p.PLACE
        if real_lighting:
            trace[u].append(("lighting",))
            return
        out = p.reg(u, "EAX")
        p.put(u, out, "3f", 0.2, 0.6, 0.8)
        trace[u].append(("lighting",))
        p.ret(u, lighting, 4)
    elif a == 0x180B60:
        query, radius_bits, record = p.get(u, sp + 4, "3I")
        assert query == p.PLACE and 0x4321 <= record >> 16 < 0x4340
        trace[u].append(
            (
                "register",
                radius_bits,
                record,
                hashlib.sha256(
                    bytes(u.mem_read(RECORD + (record & 65535) * 64, 64))
                    + bytes(u.mem_read(PACKED, 0x4000))
                ).hexdigest(),
            )
        )
        if not real_spatial:
            p.ret(u, 0, 12)
    else:
        if real_physical:
            trace[u].append(("delete", p.reg(u, "EBX")))
            return
        assert p.reg(u, "EDI") == ALLOC and p.reg(u, "EBX") == 0x12340002
        trace[u].append(("delete",))
        p.ret(u, 0, 0)


for u in (p.r, p.c):
    u.hook_add(p.U.UC_HOOK_CODE, hook)
failure = 0
lighting = True
if __name__ == "__main__":
    rng = random.Random(0x180D80)
    coverage = {
        "cases": 0,
        "success": 0,
        "failure": 0,
        "transformed": 0,
        "bytes_allocated": 0,
        "cleanup": 0,
        "lighting": 0,
    }
    mismatches = []
    for case in range(1024):
        failure = case % 8 if case % 8 < 4 else 0
        lighting = bool(case % 3)
        reuse = bool(case % 2)
        polygons = (
            [3 + (case % 7), 3 + ((case // 7) % 7)] if case % 3 else [3 + (case % 7)]
        )
        vertex_count = sum(polygons)
        transform = p.TRANSFORM if case % 4 else 0
        u = p.r
        p.initialize(u, bytes(0x8000))
        p.put(u, p.TAG + 8, "I", p.DEF)
        p.put(u, p.DEF, "B", (case % 4) * 2 + (case % 2))
        p.put(u, p.DEF + 4, "h", case % 3)
        p.put(u, p.DEF + 0xC, "I", 0xFFFFFFFF if case % 5 else 0x12340001)
        p.put(u, p.DEF + 0x1C, "6f", 0.1, 0.2, 0.3, 0.8, 0.7, 0.6)
        p.put(u, p.DEF + 0x34, "4f", 0.25, 2, 0.5, 3)
        p.put(u, p.PLACE + 8, "3f", 1, 2, 3)
        p.put(u, p.PLACE + 0x20, "h", -1 if case % 19 == 0 else 7)
        p.put(u, p.PREP + 0x8C, "6f", 0.1, 0.3 if case % 2 else 1, 0.2, 0.4, 0.3, 0.5)
        p.put(u, p.PREP + 0xEC, "4f", -0.1, 0.9, 0.2, 1.2)
        p.put(u, p.PREP + 0x100, "B", case % 8)
        p.put(u, p.STATE, "B", reuse)
        p.put(u, p.STATE + 8, "3f", 0.2, 0.3, 0.4)
        p.put(u, p.RANDOM + 4, "I", rng.getrandbits(32))
        p.put(u, p.TIME + 8, "i", 123)
        p.put(
            u,
            p.TRANSFORM,
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
        for i in range(vertex_count):
            p.put(
                u,
                OUTPUT + i * 20,
                "5f",
                *[rng.uniform(-2, 2) for _ in range(3)],
                rng.uniform(-0.5, 1.5),
                rng.uniform(-0.5, 1.5),
            )
        p.put(u, OUTPUT + 0x5000, "h", 0 if case % 23 == 0 else vertex_count)
        p.put(u, OUTPUT + 0x5802, "h", 0 if case % 29 == 0 else len(polygons))
        for i, n in enumerate(polygons):
            p.put(u, OUTPUT + 0x5002 + i * 2, "h", n)
        p.put(u, POOL + 0x44, "I", RECORD)
        payload = bytes(u.mem_read(p.DATA, 0x8000))
        results = []
        for u, entry in [(p.r, 0x180D80), (p.c, p.symbol("function_180d80"))]:
            p.initialize(u, payload)
            u.mem_write(PACKED, bytes([0xCD]) * 0x4000)
            u.mem_write(p.STACK - 0x20000, bytes([0xCD]) * 0x20000)
            for retail, name, value in [
                (0x509448, "g_509448", ALLOC),
                (0x4EA950, "g_4ea950", POOL),
            ]:
                p.put(u, retail if u is p.r else p.symbol(name), "I", value)
            trace[u] = []
            p.put(
                u,
                p.STACK,
                "7I",
                p.STOP,
                transform,
                0x12340000,
                p.PLACE,
                1,
                OUTPUT,
                p.STATE,
            )
            u.reg_write(X.UC_X86_REG_ECX, p.PREP)
            u.emu_start(entry, p.STOP, count=200000)
            assert p.reg(u, "EIP") == p.STOP and p.reg(u, "ESP") == p.STACK + 28
            results.append(
                (
                    p.reg(u, "EAX"),
                    bytes(u.mem_read(p.DATA, 0x8000)),
                    bytes(u.mem_read(PACKED, 0x4000)),
                    trace[u],
                )
            )
        if results[0] != results[1]:
            details = {
                "case": case,
                "failure_stage": failure,
                "return_diff": results[0][0] != results[1][0],
                "memory_diff": results[0][1] != results[1][1],
                "packed_diff": results[0][2] != results[1][2],
                "trace_diff": results[0][3] != results[1][3],
            }
            mismatches.append(details)
            if len(mismatches) <= 3:
                print(
                    "FAIL",
                    details,
                    "retail trace",
                    results[0][3],
                    "compiled trace",
                    results[1][3],
                )
                for field in (1, 2):
                    print(
                        "field",
                        field,
                        "diffs",
                        [
                            (hex(i), a, b)
                            for i, (a, b) in enumerate(
                                zip(results[0][field], results[1][field])
                            )
                            if a != b
                        ][:12],
                    )
            if "--characterize" not in sys.argv:
                break
        success = any(x[0] == "register" for x in trace[p.r])
        coverage["cases"] += 1
        coverage["success" if success else "failure"] += 1
        coverage["transformed"] += int(success and bool(transform))
        coverage["bytes_allocated"] += sum(
            x[1] for x in trace[p.r] if x[0] == "allocate"
        )
        coverage["cleanup"] += sum(x[0] == "delete" for x in trace[p.r])
        coverage["lighting"] += sum(x[0] == "lighting" for x in trace[p.r])
    report = {
        "passed": not mismatches,
        "mismatches": mismatches,
        "coverage": coverage,
        "image_sha256": hashlib.sha256(p.compiled.data).hexdigest(),
        "retail_sha256": hashlib.sha256(p.retail.data).hexdigest(),
        "scope": __doc__,
    }
    p.report_path(__file__).with_suffix(
        ".failure.json" if mismatches else ".json"
    ).write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({k: v for k, v in report.items() if k != "mismatches"}, indent=2))
    print(
        "mismatch counts",
        {
            k: sum(m[k] for m in mismatches)
            for k in ["return_diff", "memory_diff", "packed_diff", "trace_diff"]
        },
    )
    raise SystemExit(bool(mismatches))
