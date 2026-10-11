"""Compare actual retail and compiled texture-header construction.

Includes linear pitch, swizzled dimensions and both hardware-format rows.
No helper replacements; synthetic bitmap records, no GPU or disk execution.
Requires optional unicorn>=2.1.3,<3 and a normal project build.
"""

import argparse
import hashlib
import itertools
import json
import struct
import sys
from pathlib import Path
from inventory import check_retail
from xbe import ROOT, Xbe, retail_xbe_path
from pe import Pe
from linkmap import LinkMap, plain_name


def compare(args):
    import unicorn as U
    from unicorn import x86_const as X

    retail = Xbe(str(args.retail))
    image = Pe(str(args.image))
    link = LinkMap.read(str(args.link_map))

    def symbol(name):
        hits = [s.va for s in link.symbols if plain_name(s.name) == name]
        assert len(hits) == 1, (name, hits)
        return hits[0]

    def machine(reader):
        u = U.Uc(U.UC_ARCH_X86, U.UC_MODE_32)
        u.mem_map(0x10000, 0x1200000)
        for s in reader.sections:
            if s.rsize:
                u.mem_write(s.va, reader.data[s.raw : s.raw + s.rsize])
        return u

    r, c = machine(retail), machine(image)
    DATA = 0x1000000
    OUT = DATA + 0x200
    STACK = 0x1108000
    STOP = 0x1208000
    formats = struct.unpack("<48i", retail.read(0x450768, 192))
    assert image.read(symbol("g_450768"), 192) == retail.read(0x450768, 192), (
        "Hardware format table differs from retail"
    )
    assert image.read(symbol("g_453550"), 48) == retail.read(0x453550, 48), (
        "Bits-per-pixel table differs from retail"
    )
    coverage = {"cases": 0, "linear": 0, "swizzled": 0, "alternate_format": 0}
    failures = []
    for (
        linear,
        format_index,
        kind,
        width,
        level,
        mipmaps,
        alternate,
    ) in itertools.product(
        range(2), range(24), range(3), (8, 64), range(3), (0, 3), range(2)
    ):
        if formats[linear * 24 + format_index] < 0:
            continue
        flags = 0x200 | (0x10 if linear else 0) | (0x20 if alternate else 0)
        payload = bytearray([0xCD] * 0x400)
        struct.pack_into(
            "<hhbbhhH", payload, 4, width, width, 4, 0, kind, format_index, flags
        )
        struct.pack_into("<h", payload, 0x14, mipmaps)
        addresses = (0x81001000, 0x82002000, 0x83003000)
        struct.pack_into("<3I", payload, 0x40, *addresses)
        results = []
        for u, entry in [(r, 0x12D6D0), (c, symbol("function_12d6d0"))]:
            u.mem_write(DATA, bytes(payload))
            u.mem_write(STACK - 0x100, bytes(0x200))
            u.mem_write(STACK, struct.pack("<3I", STOP, level, OUT))
            for name in ("EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP"):
                u.reg_write(getattr(X, "UC_X86_REG_" + name), 0)
            u.reg_write(X.UC_X86_REG_EAX, DATA)
            u.reg_write(X.UC_X86_REG_ESP, STACK)
            u.reg_write(X.UC_X86_REG_EFLAGS, 2)
            u.reg_write(X.UC_X86_REG_MXCSR, 0x1F80)
            u.reg_write(X.UC_X86_REG_FPCW, 0x37F)
            u.emu_start(entry, STOP, count=10000)
            assert (
                u.reg_read(X.UC_X86_REG_EIP) == STOP
                and u.reg_read(X.UC_X86_REG_ESP) == STACK + 12
            )
            data = bytes(u.mem_read(DATA, 0x400))
            assert data[:0x200] == payload[:0x200] and data[0x214:] == payload[0x214:]
            common, pointer, lock, format_word, size = struct.unpack(
                "<5I", data[0x200:0x214]
            )
            assert (
                common == 0x40001
                and pointer == (addresses[level] & 0xFFFFFFF)
                and lock == 0
            )
            results.append(data)
        if results[0] != results[1]:
            failures.append(
                {
                    "linear": linear,
                    "format_index": format_index,
                    "kind": kind,
                    "width": width,
                    "level": level,
                    "mipmaps": mipmaps,
                    "alternate": alternate,
                    "retail": results[0][0x200:0x214].hex(),
                    "compiled": results[1][0x200:0x214].hex(),
                }
            )
        coverage["cases"] += 1
        coverage["linear" if linear else "swizzled"] += 1
        coverage["alternate_format"] += bool(alternate and format_index in (10, 11))
    report = {
        "passed": not failures,
        "coverage": coverage,
        "mismatches": len(failures),
        "examples": failures[:10],
        "retail_sha256": hashlib.sha256(retail.data).hexdigest(),
        "image_sha256": hashlib.sha256(image.data).hexdigest(),
        "scope": __doc__,
        "map_sha256": hashlib.sha256(args.link_map.read_bytes()).hexdigest(),
        "unicorn_version": U.__version__,
    }
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--retail", type=Path, default=Path(retail_xbe_path()))
    parser.add_argument("--image", type=Path, default=Path(ROOT) / "build/halo2.exe")
    parser.add_argument(
        "--map", dest="link_map", type=Path, default=Path(ROOT) / "build/halo2.map"
    )
    parser.add_argument(
        "--output", type=Path, default=Path(ROOT) / "build/texture-headers.json"
    )
    args = parser.parse_args()
    if args.output.resolve() in {
        p.resolve() for p in (args.retail, args.image, args.link_map)
    }:
        parser.error("--output must not overwrite an input")
    report = {"passed": False, "status": "started"}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    try:
        if not __debug__:
            raise RuntimeError(
                "Run without -O or PYTHONOPTIMIZE; assertions validate memory and ABI."
            )
        check_retail(str(args.retail))
        report.update(compare(args))
        report["status"] = "complete"
    except Exception as error:
        report.update(
            passed=False, status="failed", error=f"{type(error).__name__}: {error}"
        )
        print(report["error"], file=sys.stderr)
    finally:
        args.output.write_text(json.dumps(report, indent=2) + "\n")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
