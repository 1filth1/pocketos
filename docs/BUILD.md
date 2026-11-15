# Reproducible build — Waveshare N16R8

## Target board

- Board: Waveshare ESP32-S3-ePaper-3.97 (N16R8), ESP32-S3R8
- Flash: 16 MB
- PSRAM: 8 MB OPI
- Main display: GDEM0397T81, 800 × 480 E-Ink
- CPU: 240 MHz
- USB CDC on boot: enabled
- Arduino board definition: ESP32S3 Dev Module (esp32:esp32:esp32s3)

## Pinned build profile

CI uses Arduino-ESP32 **3.3.12** and this FQBN:

~~~text
esp32:esp32:esp32s3:CDCOnBoot=cdc,CPUFreq=240,FlashMode=dio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi
~~~

**Flash mode is deliberately DIO / 80 MHz.** The earlier candidate emitted a ROM boot line with mode:QIO and repeatedly reset at ets_loader.c 78 before the second-stage bootloader or Arduino setup(). The user-supplied firmware archive that boots on this exact N16R8 board declares DIO / 80 MHz. Espressif documents that some ESP32-S3 devices cannot boot in QIO and recommends DIO when flash reads fail. The candidate therefore keeps the known-good DIO mode; this is a software-level diagnosis, not a claim that the new candidate has passed a physical test.

## Partition layout

The selected layout is exactly app3M_fat9M_16MB from Arduino-ESP32 3.3.12. The repository partitions.csv mirrors that scheme and the packaging script decodes the generated partitions.bin, rejecting any unexpected entry, offset or size.

| Name | Type / subtype | Offset | Size |
|---|---|---:|---:|
| nvs | data / nvs | 0x9000 | 0x5000 |
| otadata | data / ota | 0xE000 | 0x2000 |
| app0 | app / ota_0 | 0x10000 | 0x300000 |
| app1 | app / ota_1 | 0x310000 | 0x300000 |
| ffat | data / fat | 0x610000 | 0x9E0000 |
| coredump | data / coredump | 0xFF0000 | 0x10000 |

The previous user-supplied archive used a different single-app 4 MiB map with SPIFFS. The candidate intentionally uses the requested 16 MB app3M_fat9M_16MB layout. Back up the SD card and important internal Flash data first; old SPIFFS data may become inaccessible under the new map. PocketOS currently uses external SD_MMC for user files and does not mount the internal FFAT partition.

## Pinned Arduino libraries

- GxEPD2 **1.6.9**
- Adafruit GFX Library **1.13.0**
- Adafruit BusIO **1.17.4**
- U8g2_for_Adafruit_GFX **1.8.0**
- Adafruit SSD1306 **2.5.17**
- JPEGDEC **1.8.5**
- bb_spi_lcd **2.9.10**
- TinyGPSPlus **1.0.3**

The GDEM0397T81 display driver is supplied by GxEPD2 1.6.9. The ES8311 driver is vendored from the Waveshare ESP32-S3 ePaper 3.97 Arduino audio example. The QR encoder is vendored under src/ with its upstream MIT license.

## Packaging and release checks

scripts/package_firmware.py fails closed unless:

1. The generated partition table exactly matches app3M_fat9M_16MB.
2. Bootloader and application headers declare DIO / 16 MB / 80 MHz.
3. The package contains bootloader, partition table, boot_app0 and application from the same CI run.
4. manifest.json records the exact Git commit, board, memory profile, partition scheme, addresses, sizes and SHA-256 hashes.
5. SHA256SUMS authenticates all four binaries and manifest.json.
6. The ZIP archive contains all four binaries, manifest.json and SHA256SUMS, verified byte-for-byte.

The Web Flasher verifies the manifest hash against SHA256SUMS, checks the expected board profile and flash addresses, then verifies each binary's size and SHA-256 again before writing. It uses DIO / 80 MHz from the manifest and eraseAll: false; it does not perform a full-chip erase or write eFuses.

## Local build

~~~sh
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32@3.3.12
arduino-cli lib update-index
arduino-cli lib install "GxEPD2@1.6.9"
arduino-cli lib install "Adafruit GFX Library@1.13.0"
arduino-cli lib install "Adafruit BusIO@1.17.4"
arduino-cli lib install "U8g2_for_Adafruit_GFX@1.8.0"
arduino-cli lib install "Adafruit SSD1306@2.5.17"
arduino-cli lib install "JPEGDEC@1.8.5"
arduino-cli lib install "bb_spi_lcd@2.9.10"
arduino-cli lib install "TinyGPSPlus@1.0.3"

FQBN="esp32:esp32:esp32s3:CDCOnBoot=cdc,CPUFreq=240,FlashMode=dio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi"
arduino-cli compile --fqbn "$FQBN" --warnings all .
~~~

A successful compile is not a hardware test. Verify E-Ink, side OLED, keyboard/I²C, RTC, SHTC3, SD_MMC, audio, Wi-Fi, power management and repeated cold boots on the actual device.

## References

- [Waveshare ESP32-S3-ePaper-3.97](https://docs.waveshare.com/ESP32-S3-ePaper-3.97)
- [Waveshare Arduino setup](https://docs.waveshare.com/ESP32-S3-ePaper-3.97/Development-Environment-Setup-Arduino)
- [Espressif ESP32-S3 SPI flash modes](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/spi-flash-modes.html)
- [Espressif ESP32-S3 troubleshooting](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/troubleshooting.html)
