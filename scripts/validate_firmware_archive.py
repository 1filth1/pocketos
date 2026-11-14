#!/usr/bin/env python3
"""Static integrity checks for the checked-in ESP32 firmware archive.

This does NOT compile the source, identify the physical flash chip, or validate
firmware on hardware. It only checks that the archive and its own metadata agree.
"""
from __future__ import annotations
import hashlib
import re
import struct
import sys
import zipfile
from pathlib import Path

ARCHIVE = Path(sys.argv[1] if len(sys.argv) > 1 else "firmware.zip")
REQUIRED = {"FLASH-OFFSETS.txt", "bootloader.bin", "partitions.bin", "boot_app0.bin", "firmware.bin"}
EXPECTED_OFFSETS = {
    "bootloader.bin": 0x0000,
    "partitions.bin": 0x8000,
    "boot_app0.bin": 0xE000,
    "firmware.bin": 0x10000,
}
PARTITION_MAGIC = 0x50AA
MD5_MAGIC = 0xEBEB


def fail(message: str) -> None:
    raise SystemExit("FAIL: " + message)


def image_flash_config(data: bytes, name: str) -> tuple[int, int]:
    if len(data) < 4 or data[0] != 0xE9:
        fail(f"{name}: invalid ESP image magic/header")
    flash_size_code = data[3] >> 4
    flash_freq_code = data[3] & 0x0F
    flash_sizes = {0: 1, 1: 2, 2: 4, 3: 8, 4: 16, 5: 32, 6: 64, 7: 128}
    flash_freqs = {0: "40MHz", 1: "26MHz", 2: "20MHz", 0xF: "80MHz"}
    if flash_size_code not in flash_sizes or flash_freq_code not in flash_freqs:
        fail(f"{name}: unsupported flash config byte 0x{data[3]:02x}")
    return flash_sizes[flash_size_code], flash_freq_code


def parse_partitions(data: bytes):
    rows = []
    for pos in range(0, len(data) - 31, 32):
        magic = struct.unpack_from("<H", data, pos)[0]
        if magic == MD5_MAGIC:
            break
        if magic == 0xFFFF:
            break
        if magic != PARTITION_MAGIC:
            fail(f"partition table: unexpected magic 0x{magic:04x} at 0x{pos:x}")
        ptype, subtype, offset, size = struct.unpack_from("<BBII", data, pos + 2)
        label = data[pos + 12:pos + 28].split(b"\0", 1)[0].decode("ascii", "replace")
        if size == 0 or offset + size > 0x1000000:
            fail(f"partition {label}: invalid range")
        rows.append((label, ptype, subtype, offset, size))
    if not rows:
        fail("partition table is empty or invalid")
    return rows


def main() -> None:
    if not ARCHIVE.is_file():
        fail(f"archive not found: {ARCHIVE}")
    with zipfile.ZipFile(ARCHIVE) as zf:
        names = set(zf.namelist())
        missing = REQUIRED - names
        if missing:
            fail("missing archive members: " + ", ".join(sorted(missing)))
        files = {name: zf.read(name) for name in REQUIRED}

    offsets_text = files["FLASH-OFFSETS.txt"].decode("utf-8", "replace")
    for filename, expected in EXPECTED_OFFSETS.items():
        matches = []
        for line in offsets_text.splitlines():
            if filename not in line:
                continue
            addresses = re.findall(r"0x[0-9a-fA-F]+", line)
            if any(int(address, 16) == expected for address in addresses):
                matches.append(line)
        if not matches:
            fail(f"FLASH-OFFSETS.txt does not document {filename} at 0x{expected:x}")

    boot_size, boot_freq = image_flash_config(files["bootloader.bin"], "bootloader.bin")
    app_size, app_freq = image_flash_config(files["firmware.bin"], "firmware.bin")
    if (boot_size, boot_freq) != (app_size, app_freq):
        fail("bootloader and application image flash settings differ")

    partitions = parse_partitions(files["partitions.bin"])
    ordered = sorted(partitions, key=lambda p: p[3])
    for previous, current in zip(ordered, ordered[1:]):
        if previous[3] + previous[4] > current[3]:
            fail(f"partition overlap: {previous[0]} and {current[0]}")
    app = next((p for p in partitions if p[0] == "app0"), None)
    if app is None:
        fail("factory app partition 'app0' not found")
    if len(files["firmware.bin"]) > app[4]:
        fail("firmware.bin is larger than the app0 partition")
    if max(p[3] + p[4] for p in partitions) > boot_size * 1024 * 1024:
        fail("partition table exceeds the flash capacity encoded in the image header")

    print("PASS: firmware archive structure and offsets are internally consistent")
    print(f"Image header: {boot_size} MB flash, { {0: '40MHz', 1: '26MHz', 2: '20MHz', 15: '80MHz'}[boot_freq] }")
    print(f"Firmware: {len(files['firmware.bin'])} bytes; app0 capacity: {app[4]} bytes")
    print(f"Partitions checked: {len(partitions)}")
    for name in sorted(REQUIRED):
        print(f"SHA256 {name}: {hashlib.sha256(files[name]).hexdigest()}")


if __name__ == "__main__":
    main()
