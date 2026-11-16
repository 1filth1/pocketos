# Changelog

## [0.1.2-rc.5] — 2026-11-16 — Experimental pre-release

### Added
- Lightweight application-view lifecycle telemetry: serial logs record each view entry, exit, elapsed time and transition count.
- Human-readable names for the built-in UI views in lifecycle diagnostics.

### Changed
- The runtime monitor observes the existing single-address-space UI state only. It does not introduce process isolation, unload applications or change app launch behavior.
- Release workflow fallback tag and package version are synchronized to v0.1.2-rc.5.

### Validation
- RC4 is the latest user-confirmed working hardware baseline.
- RC5 requires a clean CI build and an on-device smoke test. Serial output should include `[APP] enter` and `[APP] leave` messages when navigating between screens.

### Known issues
- Application state remains globally shared; applications are not isolated processes.
- Lifecycle hooks for app-specific initialization and cleanup, memory accounting and resource ownership are not yet implemented.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- After flashing, verify the boot diagnostics show v0.1.2-rc.5 and navigate between several applications.
- If a USB serial monitor is available, confirm readable `[APP]` entry/exit messages. Existing app behavior should remain unchanged.

## [0.1.2-rc.4] — 2026-11-16 — Experimental pre-release

### Added
- A single version definition in `version.h`, shared by the firmware boot diagnostics and the application registry.

### Fixed
- The boot diagnostics screen and application registry no longer carry stale, separately-maintained version strings.

### Changed
- Candidate version: v0.1.2-rc.4.
- No application behavior, hardware configuration, partition layout, peripheral wiring or Web Flasher flashing behavior is intentionally changed.

### Known issues
- RC3 is the known-good hardware baseline reported by the user. RC4 still requires a fresh build and a short on-device smoke test.
- Full validation of all peripherals and long-term stability remains outstanding.
- The Web Flasher is supported in desktop Chrome/Edge over HTTPS with Web Serial.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Release workflow fallback tag is synchronized to v0.1.2-rc.4; RC3 remains unchanged.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- Verify the boot diagnostics show v0.1.2-rc.4, then confirm the desktop and previously working functions.

## [0.1.2-rc.3] — 2026-11-16 — Experimental pre-release

### Added
- No new user-facing firmware features; this candidate focuses on reliable release publication from the maintained release branch.

### Fixed
- Release workflow now creates the version tag and publishes the pre-release in the same branch-triggered run, avoiding the previous dependency on a separately-created tag event.
- The complete firmware bundle, manifest, SHA-256 checksums and bilingual release notes are attached by the workflow.
- The Web Flasher catalogue continues to omit releases without a complete firmware bundle.

### Changed
- Candidate version: v0.1.2-rc.3.
- Firmware source, hardware profile, partition layout and runtime behavior are unchanged from the release-branch source used for this candidate.

### Known issues
- Full hardware validation of E-Ink, side OLED, SD read/write, keyboard/I²C, RTC, sensors, audio, Wi-Fi, power and long-term stability remains outstanding.
- The Web Flasher is supported only in desktop Chrome/Edge over HTTPS with Web Serial.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Release-branch workflow builds and validates the firmware, creates the Git tag and GitHub pre-release, uploads the firmware bundle and deploys the matching Web Flasher.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- After flashing, perform a controlled smoke test of the diagnostics screen, desktop, SD and peripherals.

All notable changes to PocketOS are documented here. Every release and pre-release must have a matching, bilingual GitHub Release description and a version-specific release-notes file.

## [0.1.2-rc.2] — 2026-11-16 — Experimental pre-release

### Added
- No new user-facing firmware features; this candidate republishes the existing v0.1.2-rc.1 firmware source with corrected release automation.

### Fixed
- Corrected the release workflow so an existing empty GitHub Release can be completed with the firmware bundle and bilingual release notes.
- The Web Flasher catalogue now skips incomplete legacy releases instead of listing versions that cannot be flashed.

### Changed
- Release publication remains tag-driven. This candidate must be built from a commit containing the repaired workflow.
- Firmware behavior, hardware profile, partition layout and peripheral wiring are unchanged from v0.1.2-rc.1.

### Known issues
- Full hardware validation of E-Ink, side OLED, SD read/write, keyboard/I²C, RTC, sensors, audio, Wi-Fi, power and long-term stability remains outstanding.
- The Web Flasher is supported only in desktop Chrome/Edge over HTTPS with Web Serial.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- The tag-triggered workflow builds the firmware, manifest and SHA-256 checksums, deploys the matching Web Flasher and publishes the GitHub pre-release.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- Since firmware behavior is unchanged from rc.1, this candidate primarily validates release packaging and publication; perform the usual controlled hardware smoke test after flashing.

---

## [0.1.2-rc.1] — 2026-11-15 — Experimental pre-release

### Added
- A visible E-Ink boot diagnostics screen reporting display, SD card, CardKB2, RTC, audio, PSRAM, Flash capacity, free heap and side OLED initialization states.
- Clearer bilingual repository documentation for the browser flasher and 3D-printable enclosure models.

### Fixed
- Corrected Web Flasher version-tag validation so valid versioned prereleases are accepted while development builds remain rejected.
- Fixed README rendering on GitHub Pages by removing the unsupported centering wrapper and replacing literal escaped newline sequences with real Markdown line breaks.

### Changed
- Startup now pauses briefly on the diagnostics screen before opening the desktop; this is an intentional boot-time increase.
- Existing applications, storage layout, peripheral wiring and Web Flasher write behavior remain unchanged.

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

## [0.1.2-rc.6] — 2026-11-16 — Experimental pre-release

### Added
- Read-only SD App Library catalogue for bounded content/resource package manifests under `/apps/<id>/app.ini`.
- Package detail view and invalid-manifest reporting.

### Fixed
- Invalid and oversized manifests are rejected; duplicate and unknown fields do not silently pass validation.

### Changed
- Documents menu gains SD App Library. No SD code execution or changes to built-in app behavior.

### Known issues
- Resource browsing, installation/removal and runtime integration remain future work; only `type=content` is supported.

### Validation
- CI compilation and package checks are required before publication; on-device SD catalogue behavior still requires user testing.

## [Unreleased] — В разработке

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
- Добавлен видимый диагностический экран E-Ink при загрузке: состояние дисплея, SD-карты, CardKB2, RTC, аудио, PSRAM, объём Flash, свободная heap-память и инициализация бокового OLED.
- Уточнена двуязычная документация репозитория: описание веб-флешера и моделей корпуса для 3D-печати.

### Исправлено
- Исправлена проверка версии в Web Flasher: корректные версионные пререлизы принимаются, development-сборки по-прежнему отклоняются.
- Исправлено отображение README на GitHub Pages: удалена неподдерживаемая HTML-обёртка центрирования, буквальные последовательности переноса строк заменены настоящими переносами Markdown.

### Изменено
- Перед открытием рабочего стола система ненадолго задерживается на диагностическом экране; это намеренно увеличивает время загрузки.
- Существующие приложения, разметка хранилища, распиновка периферии и поведение Web Flasher не меняются.

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

## [0.1.2-rc.6] — 2026-11-16 — Экспериментальный пререлиз

### Добавлено
- Каталог SD App Library для пакетов контента и ресурсов с манифестами `/apps/<id>/app.ini`.
- Экран сведений о пакете и счётчик некорректных манифестов.

### Исправлено
- Некорректные и слишком большие манифесты отклоняются; повторяющиеся и неизвестные поля не принимаются.

### Изменено
- В раздел «Документы» добавлен каталог SD App Library. Запуск кода с SD и поведение встроенных приложений не меняются.

### Известные ограничения
- Просмотр ресурсов, установка/удаление пакетов и интеграция с runtime пока не реализованы; поддерживается только `type=content`.

### Проверка
- Перед публикацией обязательны успешные CI-сборка и проверки пакета; работу каталога на устройстве должен проверить пользователь.


## [0.1.2-rc.4] — 2026-11-16 — Экспериментальный пререлиз

### Добавлено
- Единое определение версии в `version.h`, используемое диагностическим экраном загрузки и реестром приложений.

### Исправлено
- Диагностика загрузки и реестр приложений больше не содержат разрозненные строки со старой версией.

### Изменено
- Версия кандидата: v0.1.2-rc.4.
- Поведение приложений, конфигурация платы, разметка Flash, распиновка периферии и процесс прошивки через Web Flasher намеренно не меняются.

### Известные ограничения
- RC3 остаётся проверенной пользователем рабочей базовой версией. RC4 требует новой сборки и короткой проверки на устройстве.
- Полная проверка всей периферии и долговременной стабильности ещё впереди.
- Web Flasher поддерживается в настольных Chrome/Edge по HTTPS с Web Serial.

### Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Резервная версия в workflow синхронизирована с v0.1.2-rc.4; RC3 остаётся без изменений.

### Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не отключай USB и питание, пока флешер не подтвердит завершение.
- Убедись, что на экране диагностики отображается v0.1.2-rc.4, затем проверь рабочий стол и ранее работавшие функции.

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
