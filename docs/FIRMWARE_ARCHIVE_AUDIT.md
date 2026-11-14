# Firmware ZIP audit

> Scope: static inspection of the user-supplied `firmware.zip`. This report does not establish that the archive was built from this branch or that it boots on the target board.

## Package contents

| File | Size (bytes) | SHA-256 |
|---|---:|---|
| `bootloader.bin` | 19,984 | `b68c1ed33c43a4290375213f24d608c125e31e22880d16d4a5dcba89e083716d` |
| `partitions.bin` | 3,072 | `aaae2888c5a6a348004b5b436f47abb25ae32e72d9003902955a998eda723edd` |
| `boot_app0.bin` | 8,192 | `f94c5d786a7a8fab06ac5d10e33bf37711a6697636dc037559ea19cc410a17f0` |
| `firmware.bin` | 1,705,856 | `600fefc715ba7ba48c1c1cafbe32a64cb8103c74e8187107c2549d471d14829f` |
| `FLASH-OFFSETS.txt` | 138 | `4d17011b1ea2d87a32d1b26fd9cf7e6b58c806ea9619c486865bba01c5d0e92d` |

## Declared flash map

The supplied offset file specifies:

| Address | File |
|---:|---|
| `0x0000` | `bootloader.bin` |
| `0x8000` | `partitions.bin` |
| `0xE000` | `boot_app0.bin` |
| `0x10000` | `firmware.bin` |

These offsets are recorded as archive metadata, not independently verified for the physical board.

## Partition table decoded from `partitions.bin`

| Name | Offset | Size |
|---|---:|---:|
| nvs | `0x9000` | `0x5000` (20 KiB) |
| otadata | `0xE000` | `0x2000` (8 KiB) |
| app0 | `0x10000` | `0x300000` (3 MiB) |
| spiffs | `0x310000` | `0xE0000` (896 KiB) |
| coredump | `0x3F0000` | `0x10000` (64 KiB) |

The partition layout ends at `0x400000` (4 MiB). The 1,705,856-byte application image fits inside the 3 MiB app0 slot.

## Image header finding requiring resolution

Both `bootloader.bin` and `firmware.bin` start with ESP image magic `0xE9`. Their image headers contain:

- SPI mode: `2` (DIO)
- Flash-size field: `4` (16 MiB according to Espressif's ESP32 image format)
- Flash-frequency field: `0xF` (80 MHz)

This declared 16 MiB image-header setting differs from the 4 MiB address range described by the partition table. A 16 MiB physical flash could contain this 4 MiB partition layout, but a 4 MiB device would not match the image-header declaration. The actual chip capacity must be read from the target device or identified from its module marking before flashing.

## Provenance and limits

The ZIP contains prebuilt binaries and offsets, not source files, a dependency manifest or a build log. No source commit identifier is embedded in the package metadata examined here. Therefore the archive cannot currently be proven to correspond to any particular Git commit in this repository.

The reported PSRAM capacity and mode also cannot be established from this partition table alone. Do not infer PSRAM configuration from the 4 MiB Flash partition layout.

## Next verification steps

1. Identify the exact Waveshare ESP32-S3 board/module revision and flash chip marking.
2. Read the connected device's flash ID/capacity and PSRAM information.
3. Export the complete Schematic.io source and pin the Arduino-ESP32 core, libraries and board settings.
4. Rebuild from a clean checkout and compare the resulting artifacts and hashes.
5. Only then finalize browser flashing instructions and publish a pre-release artifact.
