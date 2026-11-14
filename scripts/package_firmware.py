#!/usr/bin/env python3
"""Package the exact Arduino-ESP32 build into a Web Flasher manifest."""
import hashlib, json, os, pathlib, shutil, sys, zipfile

build = pathlib.Path(sys.argv[1])
out = pathlib.Path(sys.argv[2])
out.mkdir(parents=True, exist_ok=True)

def one(pattern):
    matches = sorted(build.glob(pattern))
    if len(matches) != 1:
        raise SystemExit(f"Expected exactly one {pattern}, found: {[p.name for p in matches]}")
    return matches[0]

files = [
    ("bootloader", one("*.bootloader.bin"), 0x0000, "Bootloader"),
    ("partitions", one("*.partitions.bin"), 0x8000, "Partition table"),
    ("boot_app0", one("boot_app0.bin"), 0xE000, "OTA data initializer"),
    ("application", one("pocketos.ino.bin"), 0x10000, "PocketOS application"),
]
manifest_files = []
checksums = []
for key, src, address, label in files:
    dest = out / f"{key}.bin"
    shutil.copyfile(src, dest)
    digest = hashlib.sha256(dest.read_bytes()).hexdigest()
    manifest_files.append({"id": key, "label": label, "file": dest.name,
                           "address": address, "size": dest.stat().st_size,
                           "sha256": digest})
    checksums.append(f"{digest}  {dest.name}")

manifest = {
    "version": os.environ.get("RELEASE_VERSION", "PocketOS CI build"),
    "commit": os.environ["GITHUB_SHA"],
    "core": "3.3.12", "flash": "16MB", "psram": "8MB OPI",
    "baudrate": 460800, "flash_files": manifest_files
}
(out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
(out / "SHA256SUMS").write_text("\n".join(checksums) + "\n")
with zipfile.ZipFile(out / "PocketOS-firmware.zip", "w", zipfile.ZIP_DEFLATED) as z:
    for p in sorted(out.glob("*.bin")):
        z.write(p, p.name)
    z.write(out / "manifest.json", "manifest.json")
    z.write(out / "SHA256SUMS", "SHA256SUMS")
print(json.dumps(manifest, indent=2))
