#!/usr/bin/env python3
"""Build and verify a same-commit PocketOS firmware/Web Flasher release bundle."""
import hashlib
import json
import os
import pathlib
import re
import shutil
import struct
import sys
import zipfile

build = pathlib.Path(sys.argv[1])
out = pathlib.Path(sys.argv[2])
boot_app0 = pathlib.Path(sys.argv[3]) if len(sys.argv) > 3 else None
out.mkdir(parents=True, exist_ok=True)

EXPECTED_PARTITIONS = [
    ("nvs",      0x01, 0x02, 0x00009000, 0x00005000, 0),
    ("otadata",  0x01, 0x00, 0x0000E000, 0x00002000, 0),
    ("app0",     0x00, 0x10, 0x00010000, 0x00300000, 0),
    ("app1",     0x00, 0x11, 0x00310000, 0x00300000, 0),
    ("ffat",     0x01, 0x81, 0x00610000, 0x009E0000, 0),
    ("coredump", 0x01, 0x03, 0x00FF0000, 0x00010000, 0),
]
EXPECTED_FLASH_FILES = [
    ("bootloader", "bootloader.bin", 0x0000, "Bootloader"),
    ("partitions", "partitions.bin", 0x8000, "Partition table"),
    ("boot_app0", "boot_app0.bin", 0xE000, "OTA data initializer"),
    ("application", "application.bin", 0x10000, "PocketOS application"),
]

def one(pattern):
    matches = sorted(build.glob(pattern))
    if len(matches) != 1:
        available = sorted(p.name for p in build.glob("*.bin"))
        raise SystemExit(
            f"Expected exactly one {pattern}, found: {[p.name for p in matches]}. "
            f"Build directory binaries: {available}"
        )
    return matches[0]

def sha256(data):
    return hashlib.sha256(data).hexdigest()

def decode_partition_table(data):
    entries = []
    found_md5_marker = False
    for pos in range(0, len(data) - 31, 32):
        magic = struct.unpack_from("<H", data, pos)[0]
        if magic == 0xEBEB:
            found_md5_marker = True
            break
        if magic == 0xFFFF:
            break
        if magic != 0x50AA:
            raise SystemExit(f"Invalid partition-table magic 0x{magic:04x} at 0x{pos:x}")
        part_type, subtype = struct.unpack_from("<BB", data, pos + 2)
        offset, size = struct.unpack_from("<II", data, pos + 4)
        label = data[pos + 12:pos + 28].split(b"\0", 1)[0].decode("ascii")
        flags = struct.unpack_from("<I", data, pos + 28)[0]
        entries.append((label, part_type, subtype, offset, size, flags))
    if not found_md5_marker:
        raise SystemExit("Partition table has no ESP-IDF MD5 marker")
    return entries

def verify_image_header(path):
    image = path.read_bytes()
    if len(image) < 4 or image[0] != 0xE9:
        raise SystemExit(f"{path.name}: invalid ESP image header")
    mode, size_freq = image[2], image[3]
    size_code, freq_code = size_freq >> 4, size_freq & 0x0F
    if (mode, size_code, freq_code) != (2, 4, 0x0F):
        raise SystemExit(
            f"{path.name}: expected DIO / 16MB / 80MHz image header; "
            f"got mode={mode}, size_code={size_code}, freq_code={freq_code}"
        )

if boot_app0 is None or not boot_app0.is_file() or boot_app0.stat().st_size == 0:
    raise SystemExit("A non-empty boot_app0.bin from the installed Arduino-ESP32 3.3.12 core is required")

github_sha = os.environ.get("GITHUB_SHA", "")
if not re.fullmatch(r"[0-9a-f]{40}", github_sha):
    raise SystemExit(f"GITHUB_SHA is missing or invalid: {github_sha!r}")

files = []
for key, filename, address, label in EXPECTED_FLASH_FILES:
    if key == "bootloader":
        src = one("*.bootloader.bin")
    elif key == "partitions":
        src = one("*.partitions.bin")
    elif key == "application":
        src = one("pocketos.ino.bin")
    else:
        src = boot_app0
    dest = out / filename
    shutil.copyfile(src, dest)
    data = dest.read_bytes()
    files.append({
        "id": key,
        "label": label,
        "file": filename,
        "address": address,
        "size": len(data),
        "sha256": sha256(data),
    })

actual_partitions = decode_partition_table((out / "partitions.bin").read_bytes())
if actual_partitions != EXPECTED_PARTITIONS:
    raise SystemExit(
        "Generated partition table does not match app3M_fat9M_16MB. "
        f"Expected {EXPECTED_PARTITIONS!r}; got {actual_partitions!r}"
    )

verify_image_header(out / "bootloader.bin")
verify_image_header(out / "application.bin")

manifest = {
    "version": os.environ.get("RELEASE_VERSION", "PocketOS CI build"),
    "commit": github_sha,
    "board": "Waveshare ESP32-S3-ePaper-3.97 (N16R8)",
    "core": "3.3.12",
    "flash": "16MB",
    "psram": "8MB OPI",
    "flash_mode": "dio",
    "flash_freq": "80m",
    "partition_scheme": "app3M_fat9M_16MB",
    "partition_layout": "app3M_fat9M_16MB (3 MiB APP / 9.875 MiB FATFS)",
    "baudrate": 460800,
    "flash_files": files,
}
manifest_path = out / "manifest.json"
manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

checksums = [f"{item['sha256']}  {item['file']}" for item in files]
checksums.append(f"{sha256(manifest_path.read_bytes())}  manifest.json")
sums_path = out / "SHA256SUMS"
sums_path.write_text("\n".join(checksums) + "\n", encoding="ascii")

sum_map = {}
for line in sums_path.read_text(encoding="ascii").splitlines():
    match = re.fullmatch(r"([0-9a-f]{64})  ([A-Za-z0-9._-]+)", line)
    if not match or match.group(2) in sum_map:
        raise SystemExit(f"Malformed or duplicate SHA256SUMS line: {line!r}")
    sum_map[match.group(2)] = match.group(1)
expected_sum_names = {item["file"] for item in files} | {"manifest.json"}
if set(sum_map) != expected_sum_names:
    raise SystemExit(f"SHA256SUMS file set mismatch: {sorted(sum_map)}")
for filename, expected_hash in sum_map.items():
    actual_hash = sha256((out / filename).read_bytes())
    if actual_hash != expected_hash:
        raise SystemExit(f"SHA256SUMS mismatch for {filename}")
for item in manifest["flash_files"]:
    data = (out / item["file"]).read_bytes()
    if len(data) != item["size"] or sha256(data) != item["sha256"]:
        raise SystemExit(f"Manifest size/SHA mismatch for {item['file']}")
if sum_map["manifest.json"] != sha256(manifest_path.read_bytes()):
    raise SystemExit("SHA256SUMS does not authenticate manifest.json")

archive = out / "PocketOS-firmware.zip"
archive_names = expected_sum_names | {"SHA256SUMS"}
with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED) as z:
    for filename in sorted(archive_names):
        z.write(out / filename, filename)

with zipfile.ZipFile(archive) as z:
    if set(z.namelist()) != archive_names:
        raise SystemExit("Firmware ZIP contents do not match the verified release bundle")
    for filename in archive_names:
        if z.read(filename) != (out / filename).read_bytes():
            raise SystemExit(f"Firmware ZIP content mismatch for {filename}")

# Each image must fit entirely inside the reserved region before the next address.
region_limits = {
    "bootloader.bin": 0x8000,
    "partitions.bin": 0x6000,
    "boot_app0.bin": 0x2000,
    "application.bin": 0x300000,
}
for filename, limit in region_limits.items():
    if (out / filename).stat().st_size > limit:
        raise SystemExit(f"{filename} exceeds its reserved flash region ({limit} bytes)")

print(json.dumps(manifest, indent=2))
print("Verified: N16R8 profile, DIO/16MB/80MHz headers, app3M_fat9M_16MB partitions, manifest and SHA256SUMS.")
