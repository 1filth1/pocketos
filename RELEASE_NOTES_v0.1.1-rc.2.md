# PocketOS v0.1.1-rc.2 — pre-release

**Target:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Profile:** 16 MB Flash / 8 MB OPI PSRAM / DIO 80 MHz / app3M_fat9M_16MB  
**Status:** software candidate; physical validation is still required.

## Changes

- Explicitly pin the N16R8 build profile in GitHub Actions.
- Use DIO / 80 MHz for the bootable images. The earlier QIO candidate repeatedly stopped in the ROM loader at ets_loader.c 78; the user's known-good Schematic.io archive uses DIO / 80 MHz. The failure occurs before Arduino setup(), so the earlier Serial trace was not a fix.
- Use the requested app3M_fat9M_16MB partition table: two 3 MiB OTA application slots and a 9.875 MiB FATFS partition.
- Decode and validate the generated partition binary and both ESP image headers during packaging.
- Include the manifest's own SHA-256 in SHA256SUMS.
- Make the Web Flasher validate the board profile, commit, partition scheme, addresses, sizes, manifest checksum and each downloaded binary before writing.
- Keep eraseAll: false; no eFuse operation is performed.

## Important data warning

This candidate changes the partition map from the older 4 MiB single-app/SPIFFS reference archive. Back up the SD card and important internal Flash data before testing. Old SPIFFS contents may become inaccessible under the new layout. Keep USB and power connected until the browser reports that flashing is complete.

## Not yet hardware-verified

The assistant cannot access the physical board. After a controlled test, the user must verify cold boot, E-Ink, side OLED, SD_MMC, keyboard/I²C, RTC, SHTC3, audio, Wi-Fi, battery/power behavior and application stability.

---

# PocketOS v0.1.1-rc.2 — предварительный релиз

**Цель:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Профиль:** Flash 16 MB / OPI PSRAM 8 MB / DIO 80 MHz / app3M_fat9M_16MB  
**Статус:** программный кандидат; аппаратная проверка ещё не выполнена.

## Изменения

- Зафиксирован точный профиль N16R8 в GitHub Actions.
- Для загрузочных образов используется DIO / 80 MHz. Предыдущий кандидат с QIO зацикливался на этапе ROM-загрузчика и останавливался на ets_loader.c 78. Рабочий архив пользователя использует DIO / 80 MHz. Сбой происходил до Arduino setup(), поэтому ранний Serial trace не являлся исправлением.
- Используется требуемая таблица app3M_fat9M_16MB: два OTA-раздела приложения по 3 MiB и FATFS 9.875 MiB.
- Упаковка декодирует и проверяет бинарную таблицу разделов и заголовки обоих ESP-образов.
- В SHA256SUMS добавлена контрольная сумма самого manifest.json.
- Web Flasher проверяет профиль платы, commit, таблицу разделов, адреса, размеры, checksum манифеста и каждого бинарника до записи.
- Оставлен eraseAll: false; операции с eFuse не выполняются.

## Важное предупреждение о данных

Кандидат меняет разметку по сравнению со старым архивом с единственным приложением и SPIFFS в пределах 4 MiB. Перед тестированием сделай резервную копию SD-карты и важных данных во внутренней Flash. Старое содержимое SPIFFS может стать недоступным. Не отключай USB и питание, пока браузер не сообщит о завершении прошивки.

## Аппаратная проверка ещё не выполнена

Ассистент не имеет доступа к физической плате. После контролируемой прошивки необходимо проверить холодный запуск, E-Ink, боковой OLED, SD_MMC, клавиатуру/I²C, RTC, SHTC3, аудио, Wi-Fi, питание и стабильность приложений.
