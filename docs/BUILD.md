# Reproducible build

**CI build baseline:** Arduino-ESP32 3.3.12, ESP32-S3 Dev Module and pinned Arduino libraries. This is a source-supported build baseline, not a claim that the historical `firmware.zip` was produced with these exact versions.

## Target board profile

Waveshare documentation for ESP32-S3-ePaper-3.97 identifies ESP32-S3R8 / N16R8, 16 MB Flash, 8 MB PSRAM and 800 × 480 E-Ink. Its Arduino setup guide selects:

- Board: `ESP32S3 Dev Module`
- Flash: `16MB (128Mb)`; mode `QIO 80MHz`
- PSRAM: `OPI PSRAM`
- Partition: `16M Flash (3MB APP/9.9MB FATFS)`
- CPU: `240MHz`
- USB CDC On Boot: enabled

References: [Waveshare board documentation](https://docs.waveshare.com/ESP32-S3-ePaper-3.97), [Arduino setup](https://docs.waveshare.com/ESP32-S3-ePaper-3.97/Development-Environment-Setup-Arduino).

CI FQBN: `esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashFreq=80,FlashMode=dio,FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi`. The sketch-local `partitions.csv` takes precedence and reproduces the exact partition table from the user-tested reference archive.

## Framework and libraries pinned in CI

- Arduino-ESP32 core **3.3.12**
- GxEPD2 **1.6.9** — includes `gdem/GxEPD2_397_GDEM0397T81.h`
- Adafruit GFX Library **1.13.0**
- Adafruit BusIO **1.17.4**
- U8g2_for_Adafruit_GFX **1.8.0**
- Adafruit SSD1306 **2.5.17**
- JPEGDEC **1.8.5**
- bb_spi_lcd **2.9.10** (JPEGDEC dependency)
- TinyGPSPlus **1.0.3**

The GDEM0397T81 header is not missing from the project ecosystem: it is provided by GxEPD2 1.6.9. The previous audit's missing-driver blocker is resolved. The repository's vendored ES8311 driver remains sourced from Waveshare's ESP32-S3 ePaper 3.97 Arduino audio example.

The QR encoder from ricmoo/QRCode is vendored under `src/` with its upstream MIT license, so Arduino's unrelated `qrcode.h` cannot shadow its API. The project uses `ESP_I2S.h`, Arduino Wi-Fi/SD_MMC/Preferences and ESP-IDF headers; the pinned Arduino-ESP32 framework is therefore significant.

## Arduino sketch entry

Arduino CLI expects an `.ino` sketch whose name matches the sketch folder. `pocketos.ino` is a minimal sketch marker; the real `setup()` and `loop()` remain in `main.cpp`.

## Flash layout migration

The user supplied `firmware.zip` and reports that it successfully boots on the physical Waveshare ESP32-S3-ePaper-3.97 (N16R8). Its image headers specify DIO / 80 MHz / 16 MB, while its partition table maps the first 4 MiB: NVS, OTA data, one 3 MiB OTA_0 application slot, SPIFFS and coredump. The physical Flash is still 16 MB; the remaining address space is intentionally left unused by this reference layout.

The repository `partitions.csv` reproduces that exact reference table. Arduino-ESP32 copies a sketch-local `partitions.csv` over the selected built-in table during the build. The source uses SD_MMC and NVS Preferences; it does not mount SPIFFS or FATFS. Keeping the reference table avoids changing existing NVS/SPIFFS offsets while the known-good firmware is the hardware baseline. The Web Flasher reads `flash_mode` and `flash_freq` from the generated manifest and verifies the SHA-256 of every image before writing.

## Build locally

```sh
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
FQBN="esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi"
arduino-cli compile --fqbn "$FQBN" --warnings all .
```

## Release and hardware verification gates

1. GitHub Actions performs the clean pinned compile and packages bootloader, partition table, boot_app0 and application from the same build.
2. The packaging step records file sizes and SHA-256; the Web Flasher checks these hashes before writing.
3. After the firmware build and GitHub Pages deployment succeed, CI publishes the v0.1.0 GitHub Release with the matching binaries and manifest.
4. The PMIC discrepancy between board materials must be checked against the exact physical revision before claiming power-management support.
5. On the physical device, verify E-Ink, side OLED, CardKB/I2C, SD_MMC, audio, Wi-Fi and power behavior. This hardware test is deliberately separate from CI compilation.

The package step fails closed unless the generated partition table hash and bootloader/application image headers match the tested reference profile. An archive-integrity pass is not a firmware build. A successful CI compile is not a hardware test.

## Русский

### Базовая конфигурация CI

Зафиксированы Arduino-ESP32 **3.3.12**, плата `ESP32S3 Dev Module`, Flash 16 MB, OPI PSRAM 8 MB, E-Ink 800 × 480, DIO 80 MHz и USB CDC On Boot. Локальный `partitions.csv` повторяет проверенную пользователем таблицу разделов из `firmware.zip`.

Официальные материалы: [страница платы Waveshare](https://docs.waveshare.com/ESP32-S3-ePaper-3.97), [настройка Arduino IDE](https://docs.waveshare.com/ESP32-S3-ePaper-3.97/Development-Environment-Setup-Arduino).

### Закреплённые библиотеки

GxEPD2 1.6.9, Adafruit GFX 1.13.0, Adafruit BusIO 1.17.4, U8g2_for_Adafruit_GFX 1.8.0, Adafruit SSD1306 2.5.17, JPEGDEC 1.8.5, bb_spi_lcd 2.9.10, TinyGPSPlus 1.0.3.

**Исправление аудита:** заголовок `gdem/GxEPD2_397_GDEM0397T81.h` предоставляется библиотекой GxEPD2 1.6.9. Он не потерян, прежняя блокирующая пометка снята. ES8311 остаётся локальным драйвером, происхождение которого — пример Waveshare для этой платы.

### Разметка Flash

Проверенный пользователем `firmware.zip` использует DIO / 80 MHz и таблицу разделов на первые 4 MB при физической Flash 16 MB. В PocketOS сохранена именно эта таблица: NVS, OTA data, OTA_0 (3 MB), SPIFFS и coredump. Это не использует оставшиеся 12 MB внутренней Flash, зато сохраняет совместимость с известной рабочей прошивкой. Web Flasher использует режим DIO из манифеста и не стирает всю Flash.

### Что осталось

Сначала дождаться CI-компиляции и исправить реальные ошибки, затем проверить бинарники и SHA-256, после этого прошить устройство и проверить дисплеи, клавиатуру/I2C, SD, аудио, Wi-Fi и питание. Только после этого можно выпускать v0.1 и Web Flasher. CI-сборка не заменяет проверку на физической плате.
