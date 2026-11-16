# PocketOS v0.1.2-rc.11 — Experimental pre-release

## Added
- Bounded, read-only SD package integrity audit during App Manager catalogue scans.
- Package details show integrity status, valid resource-file count and aggregate resource size.
- Warnings identify missing or empty resource directories, unsafe resource names and incomplete bounded scans.
- Manifest values containing ASCII control characters are rejected.

## Fixed
- Resource audit byte totals saturate safely instead of wrapping on 32-bit overflow.

## Changed
- Integrity warnings remain non-destructive: otherwise valid packages stay visible and browsable.
- Audit is limited to 64 entries, does not recurse, and never writes, extracts, installs, deletes or executes SD content.

## Known issues
- Hardware validation of the RC11 integrity indicators is still required.
- Resource audits are intentionally bounded; a package with more than 64 directory entries is reported as incomplete.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Firmware and Web Flasher artifacts are generated from the same tagged commit; SHA-256 checks are retained.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Select v0.1.2-rc.11 in the Web Flasher and confirm the displayed version after reboot.
- Test a normal package, missing/empty resources folder, unsafe filename and a directory exceeding the scan bound.

# Русский

## Добавлено
- Ограниченная проверка целостности SD-пакетов только для чтения при сканировании каталога App Manager.
- В сведениях о пакете отображаются состояние проверки, количество корректных файлов ресурсов и их общий размер.
- Предупреждения показывают отсутствие или пустоту папки ресурсов, небезопасные имена и неполное сканирование.
- Значения манифеста с управляющими ASCII-символами отклоняются.

## Исправлено
- Суммарный размер ресурсов защищён от переполнения 32-битного счётчика.

## Изменено
- Предупреждения не скрывают корректный манифест: пакет остаётся доступен для просмотра.
- Проверка ограничена 64 элементами, не обходит вложенные папки и не изменяет, не распаковывает, не устанавливает, не удаляет и не запускает содержимое SD-карты.

## Известные проблемы
- Индикаторы целостности RC11 ещё требуется проверить на устройстве.
- Проверка намеренно ограничена; для каталогов больше 64 элементов отображается предупреждение о неполном сканировании.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 МГц; app3M_fat9M_16MB.
- Прошивка и Web Flasher собираются из одного помеченного коммита; сохраняются проверки SHA-256.

## Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во Flash.
- Выберите v0.1.2-rc.11 в Web Flasher и проверьте версию после перезапуска.
- Проверьте обычный пакет, отсутствие/пустоту папки ресурсов, небезопасное имя файла и каталог с числом элементов выше лимита.
