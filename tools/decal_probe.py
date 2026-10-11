"""Compare recovered decal behavior against the contributor's retail executable.

Requires the optional tools/_decal_probe/requirements.txt dependencies. Builds
nothing, uploads nothing, and writes only the requested JSON report.
"""

import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import runpy
import sys
import tempfile

from inventory import check_retail
from xbe import ROOT, retail_xbe_path

SUITES = {
    "decal": "decal_projection_integration_probe.py",
    "decal-textures": "decal_projection_integration_probe.py",
    "physical": "physical_geometry_probe.py",
    "portals": "cluster_portal_probe.py",
    "buffer": "decal_buffer_probe.py",
}
FULL_PATH = [
    "--real-commit",
    "--real-spatial",
    "--real-records",
    "--real-pool",
    "--real-bsp",
    "--real-buffer",
    "--real-lighting",
    "--real-physical",
]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--suite", choices=SUITES, default="decal")
    parser.add_argument("--retail", type=Path, default=Path(retail_xbe_path()))
    parser.add_argument("--image", type=Path, default=Path(ROOT) / "build/halo2.exe")
    parser.add_argument(
        "--map", dest="link_map", type=Path, default=Path(ROOT) / "build/halo2.map"
    )
    parser.add_argument(
        "--output", type=Path, default=Path(ROOT) / "build/decal-probe.json"
    )
    args = parser.parse_args(argv)
    inputs = [args.retail.resolve(), args.image.resolve(), args.link_map.resolve()]
    if args.output.resolve() in inputs:
        parser.error("--output must not overwrite an input file")
    report = {"passed": False, "suite": args.suite, "status": "started"}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    try:
        if not __debug__:
            raise RuntimeError(
                "Python optimization disables assertions; run without -O or PYTHONOPTIMIZE."
            )
        for path in inputs:
            if not path.is_file():
                raise FileNotFoundError(f"Missing input: {path}")
        check_retail(str(inputs[0]))
        for dependency in ("unicorn", "capstone"):
            if importlib.util.find_spec(dependency) is None:
                raise RuntimeError(
                    f"Missing {dependency}; install tools/_decal_probe/requirements.txt"
                )
        import unicorn
        import capstone

        report.update(
            retail_sha256=hashlib.sha256(inputs[0].read_bytes()).hexdigest(),
            image_sha256=hashlib.sha256(inputs[1].read_bytes()).hexdigest(),
            map_sha256=hashlib.sha256(inputs[2].read_bytes()).hexdigest(),
            unicorn_version=unicorn.__version__,
            capstone_version=capstone.__version__,
        )
        os.environ["RETAIL_XBE"] = str(inputs[0])
        os.environ["DECOMP_PROBE_IMAGE"] = str(inputs[1])
        os.environ["DECOMP_PROBE_MAP"] = str(inputs[2])
        fixtures = Path(__file__).resolve().parent / "_decal_probe"
        sys.path.insert(0, str(fixtures))
        script = fixtures / SUITES[args.suite]
        sys.argv = [str(script)] + (
            FULL_PATH if args.suite in ("decal", "decal-textures") else []
        )
        if args.suite == "decal-textures":
            sys.argv.append("--real-textures")
        with tempfile.TemporaryDirectory(prefix="halo2-decal-probe-") as temp:
            os.environ["DECOMP_PROBE_REPORT_DIR"] = temp
            runpy.run_path(str(script), run_name="__main__")
            results = list(Path(temp).glob("*.json"))
            if len(results) != 1:
                raise RuntimeError(f"Expected one result, found {len(results)}")
            result = json.loads(results[0].read_text())
            if result.get("passed") is not True:
                raise RuntimeError("Differential comparison failed")
            report.update(result)
            report["status"] = "complete"
    except (Exception, SystemExit) as error:
        report.update(
            passed=False, status="failed", error=f"{type(error).__name__}: {error}"
        )
        print(report["error"], file=sys.stderr)
    finally:
        args.output.write_text(json.dumps(report, indent=2) + "\n")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
