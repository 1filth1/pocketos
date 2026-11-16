# PocketOS — Project Status

**Candidate:** v0.1.2-rc.6 (RC5 user-reported flash success; RC6 under development)  
**Stage:** RC5 build and flash reported by user; RC6 catalogue implementation awaiting CI and hardware validation  
**Target:** Waveshare ESP32-S3-ePaper-3.97 (N16R8), E-Ink 800 × 480  
**Build:** Arduino-ESP32 3.3.12, 16 MB Flash, 8 MB OPI PSRAM, DIO / 80 MHz, app3M_fat9M_16MB

## Current candidate

- [GitHub prerelease v0.1.2-rc.5](https://github.com/1filth1/pocketos/releases/tag/v0.1.2-rc.5)
- [PocketOS Web Flasher](https://1filth1.github.io/pocketos/)
- [Firmware build workflow](https://github.com/1filth1/pocketos/actions/workflows/firmware-build.yml)

The release workflow creates the tag and attaches assets only after the firmware compile, package validation and GitHub Pages deployment succeed. manifest.json records the exact source commit; SHA256SUMS covers the four binary images and the manifest. The Pages deployment is copied from that same CI package.

## RC6 SD package catalogue

- Implemented on the RC6 feature branch: bounded, read-only manifest catalogue under `/apps`.
- No SD executable loading, package installation, extraction or deletion is included.
- Build and hardware validation are pending until the RC6 pull request checks complete and the candidate is published.

## User-reported hardware smoke test

- [x] User reports that v0.1.2-rc.5 compiled and flashed on the target Waveshare N16R8 board.
- [x] Earlier user-confirmed baseline: v0.1.2-rc.4 works on the target board.
- [ ] Record detailed results for each peripheral and subsystem in [TESTING.md](TESTING.md).

This is an initial smoke test only. It does not establish that every peripheral, long-running workload or recovery path is verified.

## Boot-loop investigation

The earlier failed candidate repeatedly reported TG0WDT_SYS_RST, SPI_FAST_FLASH_BOOT, mode:QIO and ets_loader.c 78 while the ROM was loading the second-stage bootloader. This happens before Arduino setup(); the early Serial trace commit was not a fix. The user-confirmed working Schematic.io archive declares DIO / 80 MHz. The current candidate therefore uses DIO / 80 MHz. See the firmware archive audit for evidence and limits.

## Exact partition map

The candidate uses the requested Arduino-ESP32 app3M_fat9M_16MB scheme:

- NVS: 0x9000 / 0x5000
- OTA data: 0xE000 / 0x2000
- app0: 0x10000 / 0x300000
- app1: 0x310000 / 0x300000
- FATFS: 0x610000 / 0x9E0000
- coredump: 0xFF0000 / 0x10000

This differs from the old user-supplied 4 MiB single-app/SPIFFS archive. Back up the SD card and important internal Flash data before testing; old SPIFFS data may become inaccessible under the new layout. The flasher does not perform a full-chip erase and does not write eFuses.

## Software release checks

- [x] Pinned Arduino-ESP32 core and library versions.
- [x] Explicit N16R8 FQBN: 16 MB Flash, OPI PSRAM, DIO, 240 MHz CPU, USB CDC.
- [x] Partition binary decoded and checked against app3M_fat9M_16MB.
- [x] Bootloader and application image headers checked for DIO / 16 MB / 80 MHz.
- [x] Manifest includes commit SHA, board profile, addresses, sizes and binary hashes.
- [x] SHA256SUMS includes all four binary images and manifest.json.
- [x] Web Flasher verifies manifest, checksum list, addresses, sizes and binary hashes before writing.
- [x] Flash operation keeps eraseAll: false; no eFuse operation is present.
- [x] CI build, package validation, release publication and Pages deployment passed for the published candidate.
- [ ] Detailed physical validation of all hardware subsystems.

## Hardware validation — partial

The user has confirmed RC3 and RC4 basic operation and reports that RC5 compiled and flashed. RC5 runtime behavior has not yet been separately confirmed. The following detailed checks remain to be recorded on the actual device:

- [ ] Cold boot without a watchdog/reset loop; confirm ROM reaches the second-stage bootloader.
- [ ] E-Ink initialization and refresh.
- [ ] Side OLED initialization and status display.
- [ ] CardKB2 keyboard and I²C bus (RTC, SHTC3, PMIC).
- [ ] SD_MMC mount, read and write.
- [ ] Audio output and recorder.
- [ ] Wi-Fi connection and file exchange.
- [ ] Battery/charging and power-management behavior.
- [ ] Application switching, memory headroom and repeated restarts.

A successful CI build, matching package hashes and deployed Web Flasher authorize a controlled test only; they are not proof of complete hardware compatibility. The physical device is tested by the user, not accessible to the assistant.
