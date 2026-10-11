"""Actual traversal/preparation/projection against synthetic one-, two-, and three-face meshes.

Final decal commit is mocked; texture availability is bypassed by the caller flag.
Does not test GPU submission, actual game assets, or complete gameplay.
"""

import hashlib, itertools, json, math, struct, sys
from pathlib import Path
import decal_preparation_probe as p
from unicorn import x86_const as X

diagnose_arguments = False
single_face = "--single-face" in sys.argv
bent = "--bent" in sys.argv
three_faces = "--three-faces" in sys.argv
assert not three_faces or bent
real_textures = "--real-textures" in sys.argv
real_commit = "--real-commit" in sys.argv
real_spatial = "--real-spatial" in sys.argv
assert not real_spatial or real_commit
real_records = "--real-records" in sys.argv
assert not real_records or real_spatial
real_pool = "--real-pool" in sys.argv
assert not real_pool or real_records
real_bsp = "--real-bsp" in sys.argv
assert not real_bsp or real_pool
real_buffer = "--real-buffer" in sys.argv
real_lighting = "--real-lighting" in sys.argv
assert not (real_buffer or real_lighting) or real_bsp
real_physical = "--real-physical" in sys.argv
assert not real_physical or (real_buffer and real_lighting)
if real_bsp:
    import cluster_portal_probe as portal_probe
pool_capacity = 2 if "--small-pool" in sys.argv else 16 if real_bsp else 10
if real_commit:
    import decal_commit_probe as commit_probe
p.real_geometry = True
p.lookup_success = True
p.fallback_success = True
r, c = p.r, p.c
MESH = p.DATA + 0x7400
PLANE = MESH + 0x100
FACE = MESH + 0x140
EDGES = MESH + 0x180
VERTICES = MESH + 0x200
TEXTURE_CACHE = p.DATA + 0x80000
texture_coverage = {route: 0 for route in ("frame", "raw", "resident")}
captures = {r: [], c: []}
commits = {r: 0, c: 0}
commit_targets = {r: 0x180D80, c: p.symbol("function_180d80")}


def commit(u, address, size, unused):
    if address != commit_targets[u]:
        return
    sp = p.reg(u, "ESP")
    transform, tag, placement, flag, output, state = p.get(u, sp + 4, "6I")
    prep = p.reg(u, "ECX")
    assert (
        transform == 0
        and tag == 0x12340000
        and placement == p.PLACE
        and flag & 255 == (0 if real_textures else 1)
    )
    vertices = p.get(u, output + 0x5000, "h")[0]
    polygons = p.get(u, output + 0x5802, "h")[0]
    assert 0 <= vertices <= 1024 and 0 <= polygons <= 1024
    captures[u].append(
        (
            bytes(u.mem_read(prep, 0x110)),
            bytes(u.mem_read(state, 28)),
            vertices,
            polygons,
            bytes(u.mem_read(output, 20 * vertices)),
            bytes(u.mem_read(output + 0x5002, 2 * polygons)),
        )
    )
    commits[u] += 1
    # Preserve the preparation choices across subsequent links.
    if not real_commit:
        p.put(u, state, "B", 1)
    if real_commit:
        p.put(u, p.DEF + 0xC, "I", 0x12340000 if commits[u] < links else 0xFFFFFFFF)
    else:
        p.ret(u, 0x12340000 if commits[u] < links else -1, 24)


for u in (r, c):
    u.hook_add(p.U.UC_HOOK_CODE, commit)


coverage = {
    "cases": 0,
    "commits": 0,
    "polygons": 0,
    "vertices": 0,
    "successful_chains": 0,
    "empty_chains": 0,
    "multi_face_cases": 0,
    "multi_face_polygons": 0,
    "secondary_quad_calls": 0,
    "secondary_cases": 0,
    "multi_face_secondary_cases": 0,
    "registered_records": 0,
    "spatial_queries": 0,
    "pool_allocations": 0,
}
for case, (
    size,
    offset,
    radius,
    flags,
    links,
    winding,
    two_faces,
    angle_degrees,
    texture_route,
) in enumerate(
    itertools.product(
        (0.25, 1.0, 4.0),
        (0.0, 0.5, 2.0),
        (0.25, 1.0, 2.0),
        (0, 8, 0x28),
        (1, 3),
        (1, -1),
        (True,) if bent else (False,) if single_face else (False, True),
        (-80, -41, -39, -15, 15, 39, 41, 80, 111) if bent else (0,),
        ("frame", "raw", "resident") if real_textures else ("bypass",),
    )
):
    u = r
    u.mem_write(p.DATA, bytes(0x8000))
    p.put(u, p.TAG + 8, "I", p.DEF)
    p.put(u, p.TAG + 24, "I", p.GROUP)
    p.put(u, p.DEF, "HH", flags, 0)
    p.put(u, p.DEF + 0x10, "3f", radius, radius, 1)
    p.put(u, p.DEF + 0x8C, "I", 1)
    p.put(u, p.DEF + 0xA4, "f", 2)
    p.put(u, p.PLACE + 8, "3f", offset, 0, 0)
    p.put(u, p.PLACE + 0x20, "h", 0 if real_bsp else 7)
    p.put(u, p.PLACE + 0x28, "4f", 0, 0, 1, 0)
    p.put(u, p.PLACE + 0x50, "i", 0)
    p.put(u, p.DIRECTION, "3f", 0.25, 0.5, -1)
    p.put(u, p.RANDOM + 4, "I", 0x13572468 + case)
    p.put(u, p.GROUP + 0x3C, "4I", 0, p.SEQUENCE, 1, p.BITMAP)
    p.put(u, p.BITMAP + 4, "hh", 64, 64)
    p.put(u, MESH + 0xC, "I", PLANE)
    p.put(u, MESH + 0x28, "6I", 1, FACE, 4, EDGES, 4, VERTICES)
    p.put(u, PLANE, "4f", 0, 0, 1, 0)
    p.put(u, FACE, "hH", 0, 0)
    if two_faces:
        p.put(u, MESH + 0x28, "6I", 2, FACE, 7, EDGES, 6, VERTICES)
        p.put(u, FACE + 8, "hH", 1 if bent else 0, 4)
        angle = math.radians(angle_degrees)
        if bent:
            p.put(u, PLANE + 16, "4f", -math.sin(angle), 0, math.cos(angle), 0)
        points = [
            (-size, -size),
            (0, -size),
            (0, size),
            (-size, size),
            (size, -size),
            (size, size),
        ]
        for i, (x, y) in enumerate(points):
            p.put(
                u,
                VERTICES + i * 16,
                "3f",
                x * math.cos(angle) if x > 0 else x,
                y * winding,
                x * math.sin(angle) if x > 0 else 0,
            )
        topology = [
            (0, 1, 1, 0, 0, -1),
            (1, 2, 2, 4, 0, 1),
            (2, 3, 3, 0, 0, -1),
            (3, 0, 0, 0, 0, -1),
            (1, 4, 5, 0, 1, -1),
            (4, 5, 6, 0, 1, -1),
            (5, 2, 1, 0, 1, -1),
        ]
        if three_faces:
            p.put(u, MESH + 0x28, "6I", 3, FACE, 10, EDGES, 8, VERTICES)
            p.put(u, FACE + 8, "hH", 1, 5)
            p.put(u, FACE + 16, "hH", 1, 7)
            points = [
                (-size, -size),
                (0, -size),
                (0, 0),
                (0, size),
                (-size, size),
                (size, -size),
                (size, 0),
                (size, size),
            ]
            for i, (x, y) in enumerate(points):
                p.put(
                    u,
                    VERTICES + i * 16,
                    "3f",
                    x * math.cos(angle) if x > 0 else x,
                    y * winding,
                    x * math.sin(angle) if x > 0 else 0,
                )
            topology = [
                (0, 1, 1, 0, 0, -1),
                (1, 2, 2, 5, 0, 1),
                (2, 3, 3, 7, 0, 2),
                (3, 4, 4, 0, 0, -1),
                (4, 0, 0, 0, 0, -1),
                (1, 5, 6, 0, 1, -1),
                (5, 6, 7, 0, 1, -1),
                (6, 2, 1, 9, 1, 2),
                (7, 3, 2, 0, 2, -1),
                (6, 7, 8, 0, 2, -1),
            ]
        for i, edge in enumerate(topology):
            p.put(u, EDGES + i * 12, "4H2h", *edge)
    else:
        points = [(-size, -size), (size, -size), (size, size), (-size, size)]
        if winding < 0:
            points.reverse()
        for i, (x, y) in enumerate(points):
            p.put(u, VERTICES + i * 16, "3f", x, y, 0)
            p.put(
                u,
                EDGES + i * 12,
                "4H2h",
                i,
                (i + 1) % 4,
                (i + 1) % 4,
                (i + 3) % 4,
                0,
                -1,
            )
    payload = bytes(u.mem_read(p.DATA, 0x8000))
    results = []
    for u, entry in ((r, 0x17EE20), (c, p.symbol("function_17ee20"))):
        p.initialize(u, payload)
        u.mem_write(p.STACK - 0x20000, bytes([0xCD]) * 0x20000)
        captures[u] = []
        commits[u] = 0
        if real_commit:
            commit_probe.trace[u] = []
            u.mem_write(commit_probe.PACKED, bytes([0xCD]) * 0x4000)
            p.put(u, commit_probe.POOL + 0x44, "I", commit_probe.RECORD)
            if real_buffer:
                commit_probe.buffer_probe.setup(u)
            if real_physical:
                commit_probe.physical_probe.setup(u, 64, 6, 0)
            if real_lighting:
                fallback = p.DATA + 0x61000
                p.put(u, fallback, "I3f", 0, 0.2, 0.6, 0.8)
                for va, name, value in [
                    (0x4686D4, "g_4686d4", fallback),
                    (0x4E0344, "g_4e0344", 0),
                ]:
                    p.put(u, va if u is r else p.symbol(name), "I", value)
            if real_bsp:
                u.mem_write(commit_probe.RECORD, bytes(0x400))
                portal_probe.setup(u, case % 3)
            if real_pool:
                p.put(u, commit_probe.POOL + 0x20, "2I", pool_capacity, 64)
                p.put(u, commit_probe.POOL + 0x29, "B", 1)
                p.put(u, commit_probe.POOL + 0x34, "3IH", 0, 0, 0, 0x4321)
                p.put(u, commit_probe.POOL + 0x48, "I", commit_probe.POOL + 0x80)
            if real_spatial:
                u.mem_write(
                    0x4C8798 if u is r else p.symbol("g_4c8798"), bytes([0xFF]) * 32
                )
                commit_probe.spatial.calls[u] = []
                commit_probe.spatial.allocations[u] = 0
                u.mem_write(p.GLOBALS, bytes([0xFF]) * 0x3800)
                p.put(u, p.GLOBALS + 0x3800, "3i", -1, 0, 0)
            for va, name, value in [
                (0x509448, "g_509448", commit_probe.ALLOC),
                (0x4EA950, "g_4ea950", commit_probe.POOL),
            ]:
                p.put(u, va if u is r else p.symbol(name), "I", value)
        if real_textures:
            cache = TEXTURE_CACHE
            u.mem_write(cache, bytes(0x1000))
            for va, name, value, fmt in [
                (0x468841, "g_468841", 0, "B"),
                (0x4E647B, "g_4e647b", 0, "B"),
                (0x4E6454, "g_4e6454", cache + 0x100, "I"),
                (0x4E6464, "g_4e6464", cache + 0x400, "I"),
            ]:
                p.put(u, va if u is r else p.symbol(name), fmt, value)
            p.put(u, cache + 0x144, "I", cache + 0x200)
            p.put(u, cache + 0x203, "B", 1)
            p.put(u, cache + 0x438, "I", 0x1234)
            p.put(u, cache + 0x464, "I", cache + 0x500)
            p.put(u, cache + 0x544, "I", cache + 0x600)
            p.put(u, p.BITMAP + 0xE, "H", 0x200 if texture_route == "resident" else 0)
            p.put(u, p.BITMAP + 0x28, "3i", 0, -1, -1)
            p.put(
                u,
                p.BITMAP + 0x50,
                "I",
                0 if texture_route == "resident" else cache + 0x900,
            )
            p.put(u, p.BITMAP + 0x58, "f", 1)
            p.put(u, p.BITMAP + 0x70, "i", 6 if texture_route == "frame" else 0)
        p.put(
            u,
            p.STACK,
            "8I",
            p.STOP,
            MESH,
            0,
            p.DIRECTION,
            0x3F800000,
            0 if real_textures else 1,
            0xFFFFFFFF,
            0,
        )
        u.reg_write(X.UC_X86_REG_ECX, 0x12340000)
        u.reg_write(X.UC_X86_REG_EBX, p.PLACE)
        u.emu_start(entry, p.STOP, count=1000000)
        assert p.reg(u, "EIP") == p.STOP and p.reg(u, "ESP") == p.STACK + 32
        results.append(
            (
                p.reg(u, "EAX") & 255,
                captures[u],
                bytes(u.mem_read(p.RANDOM, 8)),
                (
                    bytes(u.mem_read(commit_probe.RECORD, 64)),
                    bytes(u.mem_read(commit_probe.PACKED, 0x4000)),
                    commit_probe.trace[u],
                    bytes(u.mem_read(p.STATE, 28)),
                    (
                        bytes(u.mem_read(commit_probe.buffer_probe.BLOCK_POOL, 0x600)),
                        bytes(u.mem_read(commit_probe.ALLOC, 0x70)),
                    )
                    if real_buffer
                    else None,
                    (
                        bytes(
                            u.mem_read(
                                commit_probe.RECORD, 0x400 if real_bsp else 0x280
                            )
                        ),
                        bytes(u.mem_read(p.GLOBALS, 0x3C2C)),
                        commit_probe.spatial.calls[u],
                        bytes(
                            u.mem_read(0x4C8798 if u is r else p.symbol("g_4c8798"), 32)
                        ),
                        bytes(u.mem_read(commit_probe.POOL, 0x84)),
                        (
                            bytes(
                                u.mem_read(
                                    0x4E7418 if u is r else p.symbol("g_4e7418"), 2048
                                )
                            ),
                            p.get(u, 0x4E7414 if u is r else p.symbol("g_4e7414"), "I")[
                                0
                            ],
                        )
                        if real_bsp
                        else None,
                    )
                    if real_spatial
                    else None,
                )
                if real_commit
                else None,
            )
        )
        if real_textures:
            lookups = p.trace[u].count(0x12CCF0)
            expected_lookups = (
                0
                if texture_route == "frame"
                else commits[u]
                if texture_route == "raw"
                else 1
            )
            assert lookups == expected_lookups, (
                texture_route,
                lookups,
                expected_lookups,
            )
            assert p.get(u, p.BITMAP + 0x50, "I")[0] == TEXTURE_CACHE + (
                0x214 if texture_route == "resident" else 0x900
            )
            if texture_route == "resident":
                assert p.get(u, p.BITMAP + 0x70, "i")[0] == 7
                assert p.get(u, TEXTURE_CACHE + 0x614, "I")[0] == 0x1234
            results[-1] += (
                bytes(u.mem_read(p.BITMAP, 0x74)),
                bytes(u.mem_read(TEXTURE_CACHE, 0x1000)),
                p.trace[u][:],
            )
    if real_textures:
        texture_coverage[texture_route] += 1
    if results[0] != results[1]:
        print(
            "FAIL",
            case,
            size,
            offset,
            radius,
            flags,
            links,
            winding,
            two_faces,
            angle_degrees,
        )
        for label, res in zip(("retail", "compiled"), results):
            print(
                label,
                "return",
                res[0],
                "counts",
                [(x[2], x[3]) for x in res[1]],
                "seed",
                res[2].hex(),
            )
        for i, (a, b) in enumerate(zip(results[0][1], results[1][1])):
            for j, (aa, bb) in enumerate(zip(a, b)):
                if aa != bb:
                    print(
                        "commit",
                        i,
                        "field",
                        j,
                        "diff",
                        [
                            (k, aa[k], bb[k])
                            for k in range(min(len(aa), len(bb)))
                            if aa[k] != bb[k]
                        ][:30]
                        if isinstance(aa, bytes)
                        else (aa, bb),
                    )
        failure = {
            "passed": False,
            "case": case,
            "size": size,
            "offset": offset,
            "radius": radius,
            "flags": flags,
            "links": links,
            "winding": winding,
            "two_faces": two_faces,
            "three_faces": three_faces,
            "angle_degrees": angle_degrees,
            "retail_counts": [(x[2], x[3]) for x in results[0][1]],
            "compiled_counts": [(x[2], x[3]) for x in results[1][1]],
            "diagnostic_argument_swap": diagnose_arguments,
            "image_sha256": hashlib.sha256(p.compiled.data).hexdigest(),
        }
        p.report_path(__file__).with_suffix(
            ".real-commit.failure.json"
            if real_commit
            else ".three-faces.failure.json"
            if three_faces
            else ".bent.failure.json"
            if bent
            else ".failure.json"
        ).write_text(json.dumps(failure, indent=2) + "\n")
        raise SystemExit(1)
    secondary_calls = p.trace[r].count(0x17D9A0) - commits[r]
    assert secondary_calls >= 0
    assert p.trace[r] == p.trace[c], ("helper trace", case, p.trace[r], p.trace[c])
    coverage["secondary_quad_calls"] += secondary_calls
    coverage["secondary_cases"] += int(secondary_calls > 0)
    coverage["multi_face_secondary_cases"] += int(
        three_faces and secondary_calls > 0 and any(x[3] >= 3 for x in captures[r])
    )
    if real_spatial:
        coverage["spatial_queries"] += sum(
            x[0] == "query" for x in commit_probe.spatial.calls[r]
        )
        coverage["pool_allocations"] += commit_probe.spatial.allocations[r]
    if real_commit:
        coverage["registered_records"] += sum(
            x[0] == "register" for x in commit_probe.trace[r]
        )
    coverage["cases"] += 1
    coverage["commits"] += len(captures[r])
    coverage["polygons"] += sum(x[3] for x in captures[r])
    coverage["vertices"] += sum(x[2] for x in captures[r])
    coverage["successful_chains" if results[0][0] else "empty_chains"] += 1
    if two_faces:
        coverage["multi_face_cases"] += 1
        coverage["multi_face_polygons"] += sum(x[3] for x in captures[r])
assert coverage["polygons"] > 0, coverage
if bent:
    assert coverage["secondary_quad_calls"] > 0, coverage
if three_faces:
    assert coverage["multi_face_secondary_cases"] > 0, coverage
scope = __doc__
if real_commit:
    mocks = (
        [
            "geometry allocation",
            "buffer lookup",
            "lighting",
            "BSP cluster search",
            "record-pool allocation",
        ]
        if real_spatial
        else [
            "geometry allocation",
            "initial record creation",
            "buffer lookup",
            "lighting",
            "spatial registration",
        ]
    )
    if real_physical:
        mocks.remove("geometry allocation")
    if real_buffer:
        mocks.remove("buffer lookup")
    if real_lighting:
        mocks.remove("lighting")
    if real_bsp:
        mocks.remove("BSP cluster search")
    if real_pool:
        mocks.remove("record-pool allocation")
    if real_spatial and not real_records:
        mocks.append("initial record creation")
    scope = (
        "Actual traversal, preparation, projection and commit execute"
        + (
            ", including spatial registration, reference copying and cell linking"
            if real_spatial
            else ""
        )
        + (" and initial record creation" if real_records else "")
        + ". Controlled dependencies: "
        + (", ".join(mocks) if mocks else "none at game-routine boundaries")
        + ". "
        + (
            "Actual buffer lookup emits vertex-cache commands into synthetic nonexhausted command storage; GPU execution is not tested. "
            if real_buffer
            else ""
        )
        + (
            "Actual lighting wrapper runs with no lightmap; full lightmap sampling is not tested. "
            if real_lighting
            else ""
        )
        + (
            "Texture availability executes frame-hit, raw-texture and resident-cache paths; fallback/streaming are excluded. "
            if real_textures
            else "Texture availability is bypassed. "
        )
        + "Compares generated/packed buffers, records, state, seed, helper traces and per-registration snapshots. No GPU, asset streaming or gameplay proof."
    )
report = {
    "passed": True,
    "diagnostic_argument_swap": diagnose_arguments,
    "coverage": coverage,
    "real_textures": real_textures,
    "texture_coverage": texture_coverage if real_textures else None,
    "real_physical": real_physical,
    "real_buffer": real_buffer,
    "real_lighting": real_lighting,
    "real_bsp": real_bsp,
    "real_pool": real_pool,
    "pool_capacity": pool_capacity if real_pool else None,
    "real_records": real_records,
    "real_spatial": real_spatial,
    "real_commit": real_commit,
    "three_faces": three_faces,
    "angles_degrees": [-80, -41, -39, -15, 15, 39, 41, 80, 111] if bent else [0],
    "image_sha256": hashlib.sha256(p.compiled.data).hexdigest(),
    "retail_sha256": hashlib.sha256(p.retail.data).hexdigest(),
    "scope": scope,
}
p.report_path(__file__).with_suffix(
    ".real-physical.json"
    if real_physical
    else ".real-buffer-lighting.json"
    if real_buffer and real_lighting
    else ".real-bsp.json"
    if real_bsp
    else ".real-pool-small.json"
    if real_pool and pool_capacity == 2
    else ".real-pool.json"
    if real_pool
    else ".real-records.json"
    if real_records
    else ".real-spatial.json"
    if real_spatial
    else ".real-commit-three-faces.json"
    if real_commit and three_faces
    else ".real-commit.json"
    if real_commit
    else ".diagnostic.json"
    if diagnose_arguments
    else ".three-faces.json"
    if three_faces
    else ".bent.json"
    if bent
    else ".single-face.json"
    if single_face
    else ".json"
).write_text(json.dumps(report, indent=2) + "\n")
print(json.dumps(report, indent=2))
