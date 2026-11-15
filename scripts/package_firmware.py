#!/usr/bin/env python3
"""Package the exact Arduino-ESP32 build into a Web Flasher manifest."""
import hashlib, json, os, pathlib, shutil, sys, zipfile

build = pathlib.Path(sys.argv[1])
out = pathlib.Path(sys.argv[2])
boot_app0 = pathlib.Path(sys.argv[3]) if len(sys.argv) > 3 else None
out.mkdir(parents=True, exist_ok=True)

def one(pattern):
    matches = sorted(build.glob(pattern))
    if len(matches) != 1:
        available = sorted(p.name for p in build.glob("*.bin"))
        raise SystemExit(
            f"Expected exactly one {pattern}, found: {[p.name for p in matches]}. "
            f"Build directory binaries: {available}"
        )
    return matches[0]

if boot_app0 is None or not boot_app0.is_file():
    raise SystemExit(
        "boot_app0.bin must be supplied as the third argument from the installed "
        "Arduino-ESP32 core (tools/partitions/boot_app0.bin)."
    )
if boot_app0.stat().st_size == 0:
    raise SystemExit(f"boot_app0.bin is empty: {boot_app0}")

files = [
    ("bootloader", one("*.bootloader.bin"), 0x0000, "Bootloader"),
    ("partitions", one("*.partitions.bin"), 0x8000, "Partition table"),
    ("boot_app0", boot_app0, 0xE000, "OTA data initializer"),
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

# The user-tested reference archive uses DIO/80 MHz and this exact partition table.
# Failing closed here prevents CI from publishing a package that silently drifts.
REFERENCE_PARTITIONS_SHA256 = "aaae2888c5a6a348004b5b436f47abb25ae32e72d9003902955a998eda723edd"
partition_hash = hashlib.sha256((out / "partitions.bin").read_bytes()).hexdigest()
if partition_hash != REFERENCE_PARTITIONS_SHA256:
    raise SystemExit(
        "Generated partition table does not match the user-tested reference archive: "
        f"{partition_hash}"
    )

for image_name in ("bootloader.bin", "application.bin"):
    image = (out / image_name).read_bytes()
    if len(image) < 4 or image[0] != 0xE9:
        raise SystemExit(f"{image_name}: invalid ESP image header")
    mode, size_freq = image[2], image[3]
    if mode != 2 or (size_freq >> 4) != 4 or (size_freq & 0x0F) != 0x0F:
        raise SystemExit(
            f"{image_name}: expected DIO / 16MB / 80MHz image header; "
            f"got mode={mode}, size_code={size_freq >> 4}, freq_code={size_freq & 0x0F}"
        )

manifest = {
    "version": os.environ.get("RELEASE_VERSION", "PocketOS CI build"),
    "commit": os.environ["GITHUB_SHA"],
    "core": "3.3.12", "flash": "16MB", "psram": "8MB OPI",
    "flash_mode": "dio", "flash_freq": "80m",
    "partition_layout": "firmware.zip reference (4 MiB mapped / 16 MiB physical)",
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
