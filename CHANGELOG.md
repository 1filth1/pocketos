# Changelog

All notable changes to PocketOS are documented here. Every release and pre-release must have a matching, bilingual GitHub Release description and a version-specific release-notes file.

## [0.1.2-rc.1] — 2026-11-15 — Experimental pre-release

### Added
- Clearer bilingual repository documentation for the browser flasher and 3D-printable enclosure models.

### Fixed
- Corrected Web Flasher version-tag validation so valid versioned prereleases are accepted while development builds remain rejected.
- Fixed README rendering on GitHub Pages by removing the unsupported centering wrapper and replacing literal escaped newline sequences with real Markdown line breaks.

### Changed
- No firmware feature changes are intended in this candidate; the firmware is rebuilt from the same source tree for a traceable, versioned package.

### Known issues
- Full hardware validation of E-Ink, side OLED, SD_MMC, keyboard/I²C, RTC, sensors, audio, Wi-Fi, power and long-term stability remains outstanding.
- The Web Flasher is supported only in desktop Chrome/Edge over HTTPS with Web Serial.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- The release workflow builds the firmware, manifest and SHA-256 checksums from the tagged commit and deploys the matching Web Flasher.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- After flashing, perform the controlled hardware checklist before relying on the device.

## [Unreleased] — В разработке

### Added
- Добавлена игра «5 в ряд» (Gomoku) для E-Ink 800×480: поле 15×15, управление клавиатурой, сохранение партии и игра против компьютера.

### Fixed
- ИИ в «5 в ряд» теперь сначала выбирает немедленный выигрыш, затем блокирует немедленную победу игрока и только после этого оценивает позиционные ходы.
- Проверка сохранённой партии сверяет количество ходов, очередность, состояние победы и допустимые значения клеток.


### Fixed
- Исправлено регулярное выражение проверки версии в Web Flasher: корректные теги `PocketOS-vX.Y.Z` и `alpha/beta/rc` снова распознаются; development-сборки по-прежнему отклоняются.

### Added
- Mandatory release-note structure for every release and pre-release: new features, fixes, improvements, known issues, build/package details and upgrade instructions.
- A bilingual release-notes template and CI checks that require version-specific notes before publishing a GitHub Release.

### Validated
- User-reported smoke test of v0.1.1-rc.2 on Waveshare ESP32-S3-ePaper-3.97 (N16R8): firmware flashed successfully and PocketOS starts and works on the device. This is an initial smoke test, not full peripheral validation.

### Changed
- Release preparation now uses version tags; ordinary branch pushes build and validate artifacts without silently reusing or overwriting an existing release.

## [0.1.1-rc.2] — 2026-11-15 — Experimental pre-release

### Added
- Explicit Waveshare N16R8 profile: 16 MB Flash, 8 MB OPI PSRAM, DIO / 80 MHz.
- `app3M_fat9M_16MB` partition layout with two 3 MiB OTA app slots and a 9.875 MiB FATFS partition.
- Package validation for the partition table, ESP image headers, flash-region bounds, manifest and SHA-256 checksum list.
- Web Flasher validation of the board profile, source commit, partition map, addresses, file sizes and SHA-256 hashes.

### Fixed
- Switched boot images from QIO to DIO / 80 MHz based on the user's known-good firmware configuration, addressing the earlier ROM-stage boot failure before Arduino `setup()`.
- Included `SHA256SUMS` inside `PocketOS-firmware.zip` and verified every ZIP entry byte-for-byte.

### Safety
- The flasher keeps `eraseAll: false` and performs no eFuse operations.
- Flashing instructions warn about partition-layout/data compatibility, SD/internal Flash backups and keeping USB/power connected until completion.

### Validation
- GitHub Actions firmware build, package validation, release publication and GitHub Pages deployment completed successfully for commit `418e2b1eb55004bbdc65e15980d3b29a1c69e016`.
- The user's later report confirms a successful flash and basic device operation. Full peripheral validation remains outstanding.

## [0.1] — Experimental development baseline

### Added
- Initial PocketOS firmware source for the ESP32-S3 E-Ink target.
- Graphical home screen, launcher, settings and built-in application modules.
- SD-card file access and user-data features.
- Connectivity and peripheral integrations represented in the source.
- Preliminary architecture, hardware, testing and release documentation.

### Status
- Experimental baseline; feature presence in source code does not imply full hardware validation.

---

## [0.1.2-rc.1] — 2026-11-15 — Экспериментальный пререлиз

### Добавлено
- Уточнена двуязычная документация репозитория: описание веб-флешера и моделей корпуса для 3D-печати.

### Исправлено
- Исправлена проверка версии в Web Flasher: корректные версионные пререлизы принимаются, development-сборки по-прежнему отклоняются.
- Исправлено отображение README на GitHub Pages: удалена неподдерживаемая HTML-обёртка центрирования, буквальные последовательности переноса строк заменены настоящими переносами Markdown.

### Изменено
- Изменения функциональности прошивки в этом кандидате не планируются; прошивка пересобирается из того же дерева исходников для получения воспроизводимого пакета с версионной меткой.

### Известные ограничения
- Полная аппаратная проверка E-Ink, бокового OLED, SD_MMC, клавиатуры/I²C, RTC, датчиков, аудио, Wi-Fi, питания и долговременной стабильности ещё не выполнена.
- Web Flasher поддерживается только в настольных Chrome/Edge по HTTPS с Web Serial.

### Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Релизный workflow собирает прошивку, манифест и SHA-256 из помеченного коммита и публикует соответствующий ему Web Flasher.

### Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не отключай USB и питание, пока флешер не подтвердит завершение.
- После прошивки выполни контролируемую проверку аппаратуры, прежде чем полагаться на устройство.

# Журнал изменений

Здесь фиксируются значимые изменения PocketOS. Для каждого релиза и пререлиза обязательны отдельный файл Release Notes и двуязычное описание GitHub Release.

## [Unreleased] — В разработке

### Исправлено
- Исправлено регулярное выражение проверки версии в Web Flasher: корректные теги `PocketOS-vX.Y.Z` и `alpha/beta/rc` снова распознаются; development-сборки по-прежнему отклоняются.

### Добавлено
- Обязательная структура Release Notes для каждого релиза и пререлиза: новые функции, исправления, улучшения, известные проблемы, сведения о сборке/пакете и инструкция обновления.
- Двуязычный шаблон Release Notes и проверки CI, не позволяющие опубликовать GitHub Release без версионного описания.
 
### Проверено
- По сообщению пользователя, v0.1.1-rc.2 успешно прошилась на Waveshare ESP32-S3-ePaper-3.97 (N16R8), PocketOS запускается и работает. Это первичная проверка запуска, а не полная проверка всей периферии.

### Изменено
- Подготовка релизов переведена на версионные Git-теги: обычные push в ветку собирают и проверяют артефакты, но не переиспользуют существующий релиз и не перезаписывают его.

## [0.1.1-rc.2] — 2026-11-15 — Экспериментальный пререлиз

### Добавлено
- Явный профиль Waveshare N16R8: Flash 16 MB, OPI PSRAM 8 MB, DIO / 80 MHz.
- Разметка `app3M_fat9M_16MB`: два OTA-раздела приложения по 3 MiB и FATFS 9.875 MiB.
- Проверка таблицы разделов, заголовков ESP-образов, границ flash-разделов, манифеста и списка SHA-256.
- Проверка Web Flasher профиля платы, исходного commit, разметки, адресов, размеров файлов и SHA-256.

### Исправлено
- Режим загрузочных образов переключён с QIO на DIO / 80 MHz на основании рабочей конфигурации прошивки пользователя. Это устраняет прежнюю ошибку загрузки на стадии ROM до Arduino `setup()`.
- В `PocketOS-firmware.zip` добавлен `SHA256SUMS`; каждый файл архива проверяется на побайтовое совпадение.

### Безопасность
- Флешер использует `eraseAll: false` и не выполняет операций с eFuse.
- Инструкция предупреждает о смене разметки, резервном копировании SD/Flash и запрете отключать USB/питание до завершения прошивки.

### Проверка
- Сборка GitHub Actions, проверка пакета, публикация пререлиза и развёртывание GitHub Pages успешно завершились для commit `418e2b1eb55004bbdc65e15980d3b29a1c69e016`.
- Последующее сообщение пользователя подтверждает успешную прошивку и базовую работу устройства. Полная проверка периферии ещё впереди.

## [0.1] — Экспериментальная исходная версия

### Добавлено
- Первоначальные исходники PocketOS для ESP32-S3 с E-Ink-дисплеем.
- Графический главный экран, лаунчер, настройки и встроенные приложения.
- Работа с SD-картой и пользовательскими данными.
- Сетевые функции и интеграции периферии, представленные в исходниках.
- Предварительная документация по архитектуре, аппаратной части, тестированию и выпуску.

### Статус
- Экспериментальная исходная версия; наличие функции в исходниках не означает её полной проверки на реальном устройстве.
