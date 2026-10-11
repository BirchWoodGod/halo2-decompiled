# Decal differential probes

These independent CC0 fixtures compare retail machine code with a compiled
recovery using Unicorn. No game, compiler, SDK or executable data is included.
Supply your own lawfully usable retail executable and build the contribution
with the project's normal toolchain first.

```sh
python -m pip install -r tools/_decal_probe/requirements.txt
python tools/decal_probe.py --retail /path/to/default.xbe
```

The default suite runs the 648-case synthetic decal path, with actual physical
and record allocation, projection/clipping, commit/packing, portal traversal,
record linking, geometry-buffer lookup and vertex-cache command generation.
No reached game routine is mocked in this configuration. Synthetic state
controls tag chaining; resource/backing memory and command storage are fixtures.
Lighting runs without a lightmap, texture availability is bypassed, allocator
callbacks are absent and command storage does not refill. **This is not a GPU,
real-map, asset-streaming or gameplay test.**

Use `--suite decal-textures` to enable texture availability checks across
1,944 cases: the same geometry combinations crossed with a frame hit, a raw
texture, and a resident level-zero cache entry. Actual cache lookup executes,
including bitmap texture/frame updates and physical-cache timestamps. Full
bitmap/cache memory and helper traces are compared, with independent expected
pointer, frame, timestamp and lookup-count checks. Unexpected fallback is an
error; streaming, missing textures, shared textures and GPU sampling are not
covered. Enabling this check also forwards a zero flag to decal commit, as
retail does; the default suite retains its original nonzero flag.

Other suites are `--suite physical`, `--suite portals` and `--suite buffer`.
The runner accepts `--image`, `--map`, and `--output`; defaults are
`build/halo2.exe`, `build/halo2.map`, and `build/decal-probe.json`. The image and
map must come from the same build. `RETAIL_XBE` supplies the default retail
path, otherwise `orig/default.xbe` is used. Paths work from any current directory.
Reports include input hashes, emulator versions, coverage and scope. A failed
run replaces any old success report and exits nonzero. Do not run with Python
optimization enabled: these fixtures use assertions for ABI and memory checks.

The fixture modules are implementation details; use the runner. They retain
local investigation modes, but the runner exposes only the supported suites
above and contains no diagnostic argument-intervention hook. Calling conventions
were independently read from retail and generated instructions. LTCG changes
can invalidate an ABI assumption: investigate an assertion failure before
classifying it as a game-logic regression. Emulation compares the supplied
images; it does not prove every input or turn unmatched source into a byte match.
