# PocketOS v0.1.2-rc.10

**Experimental pre-release / Экспериментальный предварительный выпуск**

## Added
- Read-only resource browser for SD content packages under `/apps/<id>/resources/`.
- Lists up to 24 top-level files with a bounded 64-entry scan.
- Paginated preview for `.txt`, `.md` and `.ini` files up to 4 KiB.

## Fixed
- No changes to the known-good RC9 runtime behavior outside the new SD resource browsing path.

## Changed
- SD package resources can now be inspected from the package details view.
- Browsing remains data-only: no recursive traversal, writes, deletes, extraction, installation or execution.

## Known issues
- Resource browser behavior still requires on-device validation with representative SD package contents.
- Unsupported file types and text files larger than 4 KiB are listed but cannot be previewed.
- The browser intentionally limits the list to 24 files and scans at most 64 directory entries.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Firmware compilation and package validation completed successfully in GitHub Actions.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- After flashing, open Documents → SD App Library, select a package with a `resources/` folder, inspect the file list, and preview small `.txt`, `.md` and `.ini` files.
- Confirm unsupported and oversized files are not opened, navigation returns correctly, and normal built-in apps continue to work.

## Русский

### Добавлено
- Браузер ресурсов пакетов на SD-карте в режиме только чтения: каталог `/apps/<id>/resources/`.
- Отображение до 24 файлов верхнего уровня; сканирование ограничено 64 записями.
- Постраничный просмотр файлов `.txt`, `.md` и `.ini` размером до 4 КиБ.

### Исправлено
- За пределами нового пути просмотра ресурсов SD-пакетов поведение проверенной RC9 не меняется.

### Изменено
- Ресурсы SD-пакета теперь можно просматривать из экрана сведений о пакете.
- Работа остаётся строго в режиме данных: нет рекурсивного обхода, записи, удаления, распаковки, установки или исполнения файлов.

### Известные ограничения
- Работу браузера ресурсов ещё необходимо проверить на устройстве с реальными пакетами на SD-карте.
- Неподдерживаемые типы и текстовые файлы больше 4 КиБ отображаются в списке, но не открываются.
- Список ограничен 24 файлами, сканирование — 64 записями каталога.

### Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Компиляция прошивки и проверка пакета успешно завершились в GitHub Actions.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во Flash.
- После прошивки откройте Documents → SD App Library, выберите пакет с каталогом `resources/`, проверьте список и откройте небольшие файлы `.txt`, `.md`, `.ini`.
- Убедитесь, что неподдерживаемые и слишком большие файлы не открываются, навигация работает корректно, а встроенные приложения запускаются как прежде.
