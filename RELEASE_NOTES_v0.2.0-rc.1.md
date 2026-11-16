# PocketOS v0.2.0-rc.1 — Experimental pre-release

## English

### Added
- Added explicit runtime enter/leave boundaries and STOPPED / RUNNING state tracking for the existing view-based UI.
- Added allocation-free per-view launch counters, last session duration and accumulated runtime.
- Added internal RAM and PSRAM snapshots at application entry and exit.
- Added app lifecycle architecture and hardware validation documentation.

### Fixed
- Kept legacy navigation behavior intact while introducing the runtime transition bridge.

### Changed
- Synchronized firmware and branch-build fallback version to v0.2.0-rc.1.

### Known issues
- This is lifecycle telemetry and a migration foundation, not process isolation.
- Apps still share one address space. No automatic memory reclamation, dynamic SD executable loading or automatic service shutdown is provided.
- Existing blocking screens remain in place; cooperative app hooks will be introduced incrementally.

### Build & package
- Board: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Firmware and SD-card content are packaged with manifest and SHA-256 checksums.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Use the Web Flasher and keep USB/power connected until it confirms completion.
- After boot, repeatedly open and leave several apps. Check Serial output for matching [APP] enter/leave events and compare [MEM] internal RAM / PSRAM measurements.

## Русский

### Добавлено
- Добавлены явные границы входа и выхода из приложения и состояния STOPPED / RUNNING для существующей системы экранов.
- Добавлены счётчики запусков каждого экрана, длительность последнего сеанса и суммарное время работы без динамического выделения памяти.
- Добавлены снимки внутренней RAM и PSRAM при входе и выходе из приложения.
- Добавлена документация по архитектуре жизненного цикла и проверке на устройстве.

### Исправлено
- Сохранена прежняя навигация; новый runtime-мост только отслеживает переходы между экранами.

### Изменено
- Версия прошивки и резервная версия сборки из основной ветки синхронизированы с v0.2.0-rc.1.

### Известные ограничения
- Это телеметрия жизненного цикла и основа для дальнейшей миграции, а не изоляция процессов.
- Приложения по-прежнему работают в общем адресном пространстве. Автоматическое освобождение памяти, запуск исполняемых файлов с SD и автоматическое выключение системных сервисов не реализованы.
- Существующие блокирующие экраны пока не изменены; кооперативные обработчики приложений будут добавляться постепенно.

### Сборка и пакет
- Плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Прошивка и содержимое SD-карты упаковываются с манифестом и SHA-256 контрольными суммами.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во внутренней Flash.
- Используйте Web Flasher и не отключайте USB/питание до подтверждения завершения.
- После загрузки несколько раз откройте и закройте разные приложения. Проверьте в Serial сообщения [APP] enter/leave и сравните показатели внутренней RAM и PSRAM в строках [MEM].
