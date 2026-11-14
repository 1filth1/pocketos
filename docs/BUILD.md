# Reproducible build investigation

**Status:** blocked — the current repository snapshot does not yet contain enough information to define a safe, reproducible build.

This document records findings from the checked-in source tree on branch `release/pocketos-1.0`. It deliberately does not guess board options, dependency versions or flash settings.

## Source-derived framework requirements

- The firmware is an Arduino-style ESP32-S3 sketch with `main.cpp` as its entry point.
- `audio_app.h` includes `ESP_I2S.h`. This is the Arduino-ESP32 I2S C++ API; the project must pin a framework release that actually provides this header and the API used by the source.
- `es8311.cpp` uses ESP-IDF headers such as `driver/i2c.h`, `driver/gpio.h`, `esp_err.h` and `esp_check.h`, plus Arduino `Wire.h`. Compatibility must be checked against the selected Arduino-ESP32 / ESP-IDF combination.
- Framework-provided headers also include `Arduino.h`, `SPI.h`, `Wire.h`, `SD_MMC.h`, `WiFi.h`, `HTTPClient.h`, `WiFiClientSecure.h`, `esp_bt.h`, `esp_timer.h`, `esp_sleep.h`, `esp_heap_caps.h` and `esp32-hal-psram.h`.

## External library dependencies visible in includes

| Dependency | Evidence | Version status |
|---|---|---|
| GxEPD2 | `GxEPD2_BW.h`, `Fonts/*.h` | Not pinned |
| U8g2 for Adafruit GFX | `U8g2_for_Adafruit_GFX.h` | Not pinned |
| Adafruit GFX | Used through GxEPD2 / U8g2 display APIs | Transitive version not pinned |

The exact transitive dependency versions must be captured from a known-good Schematic.io/Arduino environment or an existing build manifest before CI can reproduce the build.

## Confirmed source-tree blocker

`main.cpp` includes:

```cpp
#include <gdem/GxEPD2_397_GDEM0397T81.h>
```

The current Git tree does **not** contain a `gdem/` directory or this header. No checked-in dependency manifest identifies the library/package that provides it. The display driver is therefore an unresolved dependency, not something CI can safely infer from the generic GxEPD2 library name.

The checked-in `es8311.cpp/.h` files are a vendored ES8311 driver. Their source comments identify the Waveshare ESP32-S3 ePaper 3.97 Arduino audio example as the origin and the source carries Espressif's Apache-2.0 SPDX notice. Keep this provenance and license notice when packaging the firmware.

## Firmware archive is not a reproducible build

`firmware.zip` contains prebuilt binaries and offsets. It does not identify the source commit, compiler, Arduino-ESP32 version, board FQBN, external library versions or exact build command. It is useful as a historical flashing artifact for analysis, but cannot prove that the current source compiles or that a new binary is equivalent.

The archive's image header and partition table are validated separately by `scripts/validate_firmware_archive.py`. That validation does not identify the physical flash chip or prove compatibility with the user's exact board revision.

## Safe next steps

1. Resolve the missing GDEM0397T81 driver header and identify its upstream/package provenance and license.
2. Capture exact versions of GxEPD2, U8g2_for_Adafruit_GFX and their transitive dependencies from the known-good project environment.
3. Confirm the exact Waveshare board/module revision and Arduino-ESP32 target/core version.
4. Add a pinned build manifest only after steps 1–3 are evidenced.
5. Run a clean build in CI, retain the compiler log and upload generated artifacts.
6. Test the resulting image on the target hardware before describing it as a verified release.

## Русский

**Статус:** сборка пока заблокирована — в текущем снимке репозитория не хватает сведений и как минимум одной зависимости для безопасной воспроизводимой компиляции.

### Что установлено по исходникам

- Проект использует Arduino-подход для ESP32-S3, точка входа — `main.cpp`.
- `audio_app.h` подключает `ESP_I2S.h`; это API Arduino-ESP32 I2S. Нужно закрепить версию framework, в которой реально присутствует используемый API.
- `es8311.cpp` использует ESP-IDF заголовки `driver/i2c.h`, `driver/gpio.h`, `esp_err.h`, `esp_check.h` и Arduino `Wire.h`. Совместимость зависит от выбранной версии Arduino-ESP32 / ESP-IDF.
- Из внешних библиотек в исходниках явно видны GxEPD2 и U8g2_for_Adafruit_GFX; их версии и транзитивные зависимости не закреплены.

### Подтверждённая блокирующая проблема

В `main.cpp` есть подключение `gdem/GxEPD2_397_GDEM0397T81.h`, но в текущем Git tree нет каталога `gdem/` и самого заголовка. Манифеста, указывающего пакет, который предоставляет этот драйвер, тоже нет. Подменять его случайной версией GxEPD2 небезопасно.

### Почему нельзя просто собрать старый ZIP

В `firmware.zip` лежат готовые бинарники и адреса записи, но нет связи с конкретным коммитом исходников, версии компилятора, Arduino-ESP32, FQBN платы, списка библиотек и команды сборки. Архив пригоден для анализа, но не доказывает воспроизводимость текущих исходников.

### Следующие безопасные шаги

1. Найти точное происхождение и лицензию драйвера GDEM0397T81.
2. Зафиксировать версии библиотек из заведомо рабочего окружения Schematic.io / Arduino.
3. Подтвердить точную ревизию платы Waveshare и версию Arduino-ESP32.
4. Только после этого добавить манифест сборки и CI-компиляцию.
5. Выполнить чистую сборку и сохранить лог/артефакты.
6. Проверить полученную прошивку на устройстве до объявления релиза проверенным.
