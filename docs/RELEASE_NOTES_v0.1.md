# PocketOS v0.1

Initial reproducible firmware release for the Waveshare ESP32-S3 ePaper target.

## Build target

- Arduino-ESP32 3.3.12
- ESP32-S3, 16 MB Flash, 8 MB OPI PSRAM
- 800 × 480 E-Ink
- Flash layout: `app3M_fat9M_16MB`
- GxEPD2 1.6.9 and the pinned libraries listed in [BUILD.md](BUILD.md)

## Install

Use the browser installer at:

`https://1filth1.github.io/pocketos/`

Open it in Chrome or Edge on a desktop computer, connect the board over a data-capable USB cable, select its serial port, and follow the installer. The installer verifies SHA-256 hashes before writing the bootloader, partition table, boot_app0 and application image.

The release assets also include the individual binary images, manifest and SHA256SUMS for manual recovery.

## Important

- This release is compiled in GitHub Actions for the documented board profile.
- Physical validation is still required for the actual board revision, E-Ink, side OLED, SD_MMC, I2C devices, audio, Wi-Fi and power management.
- Changing the partition table can make data stored under the previous layout inaccessible. Back up anything important before flashing.
- Do not disconnect USB or remove power while flashing.
