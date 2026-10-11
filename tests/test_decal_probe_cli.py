"""Failure reporting must not leave a stale success or overwrite an input."""

import json
from pathlib import Path
import subprocess
import sys

RUNNER = Path(__file__).resolve().parents[1] / "tools/decal_probe.py"


def run_probe(tmp_path, *args, optimized=False):
    return subprocess.run(
        [sys.executable, *(["-O"] if optimized else []), str(RUNNER), *map(str, args)],
        cwd=tmp_path,
        capture_output=True,
        text=True,
    )


def test_missing_input_replaces_stale_success(tmp_path):
    output = tmp_path / "result.json"
    output.write_text('{"passed": true}')
    result = run_probe(
        tmp_path, "--retail", tmp_path / "missing.xbe", "--output", output
    )
    assert result.returncode == 1
    report = json.loads(output.read_text())
    assert report["passed"] is False and report["status"] == "failed"
    assert "Missing input" in report["error"]


def test_wrong_retail_rejected_before_emulation(tmp_path):
    paths = [tmp_path / name for name in ("retail.xbe", "build.exe", "build.map")]
    for path in paths:
        path.write_bytes(b"not a retail executable")
    output = tmp_path / "result.json"
    result = run_probe(
        tmp_path,
        "--retail",
        paths[0],
        "--image",
        paths[1],
        "--map",
        paths[2],
        "--output",
        output,
    )
    assert result.returncode == 1
    report = json.loads(output.read_text())
    assert report["passed"] is False
    assert "not the retail XBE" in report["error"]


def test_output_cannot_overwrite_input(tmp_path):
    retail = tmp_path / "original.xbe"
    retail.write_bytes(b"preserve this input")
    result = run_probe(tmp_path, "--retail", retail, "--output", retail)
    assert result.returncode != 0
    assert retail.read_bytes() == b"preserve this input"


def test_optimized_python_is_rejected(tmp_path):
    output = tmp_path / "result.json"
    output.write_text('{"passed": true}')
    result = run_probe(tmp_path, "--output", output, optimized=True)
    assert result.returncode == 1
    report = json.loads(output.read_text())
    assert report["passed"] is False
    assert "optimization disables assertions" in report["error"]
