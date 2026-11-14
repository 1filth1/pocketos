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

CI FQBN: `esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi`.

## Framework and libraries pinned in CI

- Arduino-ESP32 core **3.3.12**
- GxEPD2 **1.6.9** — includes `gdem/GxEPD2_397_GDEM0397T81.h`
- Adafruit GFX Library **1.13.0**
- Adafruit BusIO **1.17.4**
- U8g2_for_Adafruit_GFX **1.8.0**
- Adafruit SSD1306 **2.5.17**
- JPEGDEC **1.8.5**
- bb_spi_lcd **2.9.10** (JPEGDEC dependency)
- QRCode **0.0.1**
- TinyGPSPlus **1.1.0**

The GDEM0397T81 header is not missing from the project ecosystem: it is provided by GxEPD2 1.6.9. The previous audit's missing-driver blocker is resolved. The repository's vendored ES8311 driver remains sourced from Waveshare's ESP32-S3 ePaper 3.97 Arduino audio example.

The project uses `ESP_I2S.h`, Arduino Wi-Fi/SD_MMC/Preferences and ESP-IDF headers; the pinned Arduino-ESP32 framework is therefore significant.

## Arduino sketch entry

Arduino CLI expects an `.ino` sketch whose name matches the sketch folder. `pocketos.ino` is a minimal sketch marker; the real `setup()` and `loop()` remain in `main.cpp`.

## Flash layout migration

The historical `firmware.zip` uses a partition table covering only the first 4 MiB, with a 3 MiB app partition plus SPIFFS and coredump. The manufacturer's current 16 MB profile uses `app3M_fat9M_16MB`. The app starts at `0x10000` in both layouts, but the remaining partition map differs.

The current source uses SD_MMC and NVS Preferences; it does not directly mount SPIFFS or FATFS. The CI profile follows the current 16 MB Waveshare profile. A future browser flasher must bundle the partition table from this same build and warn that changing the table replaces the old layout; old SPIFFS data must not be promised to survive.

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
arduino-cli lib install "QRCode@0.0.1"
arduino-cli lib install "TinyGPSPlus@1.1.0"
FQBN="esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi"
arduino-cli compile --fqbn "$FQBN" --warnings all .
```

## Remaining verification gates

1. Run the clean pinned build in GitHub Actions and fix real compiler/linker errors.
2. Inspect generated bootloader, partition table and app image; record sizes and SHA-256.
3. Resolve the PMIC discrepancy for the exact board revision. Waveshare's schematic and board materials must be checked against the actual hardware before validating power-management code.
4. Flash and test E-Ink, side OLED, CardKB/I2C, SD_MMC, audio, Wi-Fi and power behavior on the physical device.
5. Only then publish v0.1 and the browser flasher.

An archive-integrity pass is not a firmware build. A successful CI compile is not a hardware test.

## Русский

### Базовая конфигурация CI

Зафиксированы Arduino-ESP32 **3.3.12**, плата `ESP32S3 Dev Module`, Flash 16 MB, OPI PSRAM 8 MB, E-Ink 800 × 480, разметка `16M Flash (3MB APP/9.9MB FATFS)`, CPU 240 MHz, QIO 80 MHz и USB CDC On Boot.

Официальные материалы: [страница платы Waveshare](https://docs.waveshare.com/ESP32-S3-ePaper-3.97), [настройка Arduino IDE](https://docs.waveshare.com/ESP32-S3-ePaper-3.97/Development-Environment-Setup-Arduino).

### Закреплённые библиотеки

GxEPD2 1.6.9, Adafruit GFX 1.13.0, Adafruit BusIO 1.17.4, U8g2_for_Adafruit_GFX 1.8.0, Adafruit SSD1306 2.5.17, JPEGDEC 1.8.5, bb_spi_lcd 2.9.10, QRCode 0.0.1, TinyGPSPlus 1.1.0.

**Исправление аудита:** заголовок `gdem/GxEPD2_397_GDEM0397T81.h` предоставляется библиотекой GxEPD2 1.6.9. Он не потерян, прежняя блокирующая пометка снята. ES8311 остаётся локальным драйвером, происхождение которого — пример Waveshare для этой платы.

### Разметка Flash

Старая `firmware.zip` использует таблицу разделов на первые 4 MB. Профиль производителя для 16 MB использует `app3M_fat9M_16MB`. Начало приложения совпадает (`0x10000`), остальная разметка отличается. Будущий Web Flasher обязан использовать таблицу от той же сборки и предупредить о замене старой разметки. Сохранность старых данных SPIFFS не гарантируется.

### Что осталось

Сначала дождаться CI-компиляции и исправить реальные ошибки, затем проверить бинарники и SHA-256, после этого прошить устройство и проверить дисплеи, клавиатуру/I2C, SD, аудио, Wi-Fi и питание. Только после этого можно выпускать v0.1 и Web Flasher. CI-сборка не заменяет проверку на физической плате.
