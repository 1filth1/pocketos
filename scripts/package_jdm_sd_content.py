#!/usr/bin/env python3
"""Build a copy-to-SD content pack for JDM Garage Empire from the canonical sprite source."""
import hashlib
import json
import pathlib
import re
import sys
import zipfile

source = pathlib.Path(sys.argv[1])
out = pathlib.Path(sys.argv[2])
archive = pathlib.Path(sys.argv[3])
text = source.read_text(encoding="utf-8")
match = re.search(r"carSprites\s*\[10\]\s*\[3120\]\s*PROGMEM\s*=\s*\{", text)
if not match:
    raise SystemExit("Could not find canonical carSprites[10][3120] PROGMEM array")

# Parse the ten nested initializers without evaluating C/C++ source.
pos = match.end() - 1
depth = 0
groups = []
start = None
for i in range(pos, len(text)):
    ch = text[i]
    if ch == "{":
        depth += 1
        if depth == 2:
            start = i + 1
    elif ch == "}":
        if depth == 2 and start is not None:
            groups.append(text[start:i])
            start = None
        depth -= 1
        if depth == 0:
            break

if len(groups) != 10:
    raise SystemExit(f"Expected 10 car sprites, parsed {len(groups)}")
root = out / "Games" / "JDM_Garage_Empire"
assets = root / "assets"
assets.mkdir(parents=True, exist_ok=True)
files = []
for index, group in enumerate(groups):
    values = [int(v) for v in re.findall(r"\b\d+\b", group)]
    if len(values) != 3120 or any(v < 0 or v > 255 for v in values):
        raise SystemExit(f"Sprite {index}: expected 3120 bytes, got {len(values)}")
    data = bytes(values)
    name = f"car_shaded_v2_{index}.mono"
    (assets / name).write_bytes(data)
    files.append({
        "file": f"assets/{name}",
        "width": 240,
        "height": 104,
        "bits_per_pixel": 1,
        "bytes_per_row": 30,
        "byte_order": "MSB-first",
        "sha256": hashlib.sha256(data).hexdigest(),
        "size": len(data),
    })

manifest = {
    "format": "PocketOS JDM Garage Empire SD content",
    "content_version": "1.0.0",
    "game": "JDM Garage Empire",
    "asset_format": "raw monochrome bitmap compatible with GxEPD2 drawBitmap",
    "pixel_semantics": "1 = black pixel, 0 = white pixel",
    "install_to": "/",
    "assets": files,
}
(root / "assets-manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
(out / "README.txt").write_text(
    "JDM GARAGE EMPIRE — SD CONTENT PACK 1.0.0\n"
    "==========================================\n\n"
    "Copy the included Games folder to the root of the PocketOS SD card.\n"
    "The resulting game artwork paths are:\n"
    "/Games/JDM_Garage_Empire/assets/car_shaded_v2_0.mono ... car_shaded_v2_9.mono\n\n"
    "Each car image is a 240x104, 1-bit, MSB-first bitmap (3,120 bytes).\n"
    "The game can also seed these files from built-in firmware artwork when no SD pack is present.\n"
    "Existing files are never overwritten automatically. Back up the SD card before installing.\n",
    encoding="utf-8",
)
archive.parent.mkdir(parents=True, exist_ok=True)
with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED) as z:
    for path in sorted(out.rglob("*")):
        if path.is_file():
            z.write(path, path.relative_to(out).as_posix())
print(f"Created {archive} with {len(files)} car sprites")
