# PocketOS Test Plan

No physical test is marked PASS without an actual result, board revision, date and evidence.

## Software release integrity

| ID | Check | Expected result |
|---|---|---|
| SW-01 | CI source identity | Build run head SHA equals branch HEAD and release tag commit |
| SW-02 | Target profile | ESP32-S3 N16R8, 16 MB Flash, 8 MB OPI PSRAM |
| SW-03 | Image headers | Bootloader and application: DIO / 16 MB / 80 MHz |
| SW-04 | Partition table | Exact app3M_fat9M_16MB entries and offsets |
| SW-05 | Manifest | Commit, board, memory, partition scheme, addresses, sizes and SHA-256 values are correct |
| SW-06 | SHA256SUMS | Covers bootloader, partitions, boot_app0, application and manifest.json |
| SW-07 | ZIP archive | Contains byte-identical files from the verified package |
| SW-08 | Web Flasher | Deployed from the same CI package; checks manifest and binary hashes before write |
| SW-09 | Flash safety | Uses eraseAll: false; no full-chip erase or eFuse write |

## RC6 SD package catalogue tests

| ID | Test | Expected result |
|---|---|---|
| SDAPP-01 | No SD card or no `/apps` directory | Clear empty/unavailable status; no crash |
| SDAPP-02 | Valid content manifest | Package appears with correct name and version |
| SDAPP-03 | Missing required field / malformed line | Package is rejected and counted invalid |
| SDAPP-04 | Duplicate or unknown key | Package is rejected |
| SDAPP-05 | Folder name differs from manifest ID | Package is rejected |
| SDAPP-06 | Manifest larger than 768 bytes | Package is rejected without unbounded allocation |
| SDAPP-07 | More than 32 package folders | Scan inspects no more than 32 package directories and accepts at most 16 valid packages |
| SDAPP-08 | Unsupported `type` or executable file present | No code is loaded or executed; unsupported package is rejected |
| SDAPP-09 | Rescan and detail navigation | List refreshes and details return correctly |

## Physical hardware tests

| ID | Test | Expected result | Status |
|---|---|---|---|
| HW-01 | Cold boot | ROM reaches second-stage bootloader; no repeating TG0WDT_SYS_RST | NOT RUN |
| HW-02 | E-Ink | GDEM0397T81 initializes and refreshes the 800 × 480 panel | NOT RUN |
| HW-03 | Side OLED | OLED initializes and status indicators update | NOT RUN |
| HW-04 | Keyboard / I²C | CardKB2 input is reliable; RTC, SHTC3 and PMIC are handled | NOT RUN |
| HW-05 | SD_MMC | Card mounts; file listing, read and write work | NOT RUN |
| HW-06 | Audio | ES8311 output and recording functions behave correctly | NOT RUN |
| HW-07 | Wi-Fi | Station connection, HTTP/TLS and file exchange behave correctly | NOT RUN |
| HW-08 | Power | Battery/charging values are plausible; no unexpected shutdown | NOT RUN |
| HW-09 | Stability | App switching, memory headroom and repeated restarts work | PARTIAL — user confirmed RC4 basic operation; RC5 app-view telemetry still needs on-device review |

## Controlled first-flash precautions

1. Back up the SD card and important internal Flash data.
2. Use only the v0.1.1-rc.2 package whose manifest commit matches the release tag and branch build.
3. Use Chrome or Edge with Web Serial over HTTPS and a stable USB data cable.
4. Do not disconnect USB or power while flashing.
5. Do not use a full-chip erase or eFuse operation.
6. After reboot, capture the serial log and verify the boot sequence before testing peripherals.

The partition map changes from the old 4 MiB single-app/SPIFFS reference archive to app3M_fat9M_16MB; old SPIFFS contents may become inaccessible.

## Test record format

- Result: PASS / FAIL / BLOCKED / NOT RUN
- Firmware release and commit:
- Board model/revision:
- Date:
- Steps and observed result:
- Serial log, photo or other evidence:
