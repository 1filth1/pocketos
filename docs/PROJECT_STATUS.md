# PocketOS — Project Status

**Version:** 0.1  
**Stage:** Experimental / pre-release preparation  
**Target:** Waveshare ESP32-S3 E-Ink, 800 × 480  
**Development environment:** Schematic.io

## Current snapshot

PocketOS is an experimental single-firmware embedded system. The repository contains the main firmware entry point, UI/display support and a collection of built-in application modules.

This page separates source-code presence from verified device behavior. A module existing in the repository does not by itself prove that its feature is complete or stable on hardware.

## Release readiness

| Area | Status | Evidence required |
|---|---|---|
| Project overview | Documented | README and release notes |
| Source structure | Present | Review against current Schematic.io export |
| Build environment | Not yet pinned | Exact board/core/compiler configuration |
| Dependencies | Not yet pinned | Library names and versions |
| Flash / PSRAM | Not yet verified | Board settings and memory report |
| Partition layout | Not yet verified | Exact partition table and offsets |
| Firmware archive CI check | **PASS** — GitHub Actions run #5 on commit `758a91f4e073a8a200df3edf9f4f2f78c1a6290e` | [Run #5](https://github.com/1filth1/pocketos/actions/runs/43568349097); archive metadata validator only |
| Firmware artifacts | Pending | Reproducible binary outputs and hashes |
| Flashing guide | Pending | Verified offsets and recovery procedure |
| Hardware validation | Pending | Test report from target device |

## Validation checklist

**Latest CI evidence (2026-11-14):** GitHub Actions run [#5](https://github.com/1filth1/pocketos/actions/runs/43568349097) completed with `success` for commit `758a91f4e073a8a200df3edf9f4f2f78c1a6290e`. It runs only `scripts/validate_firmware_archive.py`; it does **not** compile the firmware or test hardware.


### Build and toolchain
- [x] Add a static validator for the checked-in firmware ZIP and a GitHub Actions workflow; run #5 completed successfully.
- [ ] Export the complete project from Schematic.io.
- [ ] Identify the exact ESP32-S3 board target and Arduino/ESP-IDF core version.
- [ ] Record compiler and framework versions.
- [ ] Inventory all external libraries and pin their versions.
- [ ] Confirm USB CDC, bootloader and upload settings.
- [ ] Confirm Flash size, PSRAM mode and partition layout.
- [ ] Perform a clean build from a fresh checkout.

### Hardware
- [ ] Confirm E-Ink initialization and full refresh.
- [ ] Verify keyboard input and I²C peripherals.
- [ ] Verify SD_MMC mount, read, write and error handling.
- [ ] Verify RTC, SHTC3 and AXP2101 readings.
- [ ] Verify Wi-Fi and file exchange.
- [ ] Check memory headroom and stability during application switching.
- [ ] Test restart, power loss and SD-card removal behavior.
- [ ] Record power consumption and display refresh behavior.

### Release package
- [ ] Publish only binaries produced by a verified build.
- [ ] Add a reproducible source compilation job; the current workflow checks archive metadata only.
- [ ] Record SHA-256 checksums.
- [ ] Document exact flashing offsets for the exact board configuration.
- [ ] Include recovery and troubleshooting instructions.
- [ ] Keep the first public build marked as pre-release until hardware tests pass.

## Known architectural limitations

- Built-in applications are compiled into one firmware and share an address space.
- Process isolation is not implemented.
- Arbitrary executable applications cannot currently be installed and launched from SD.
- Feature availability can depend on external peripherals and network access.

## Branch policy

Release preparation takes place in `release/pocketos-1.0`. The default `main` branch is kept separate until the changes are reviewed and explicitly approved for merge.

## Reporting test results

When reporting a failure, include:
1. Board model and hardware revision.
2. Firmware commit or build identifier.
3. Steps to reproduce.
4. Expected and actual behavior.
5. Serial logs or photos, with personal data removed.


## Repository governance

- License: MIT for original project materials; third-party components remain subject to their own licenses.
- Release procedure: [docs/RELEASE_PROCESS.md](RELEASE_PROCESS.md) describes the verification gates required before publishing a firmware release.
- Engineering references: [architecture](ARCHITECTURE.md), [hardware notes](HARDWARE.md), and [test plan](TESTING.md).
