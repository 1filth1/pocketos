## [0.2.0-rc.4] — 2026-11-19 — JDM Garage Empire persistent warehouse

### Added
- Added a persistent ten-item parts warehouse initialized from the SD parts catalogue.
- Added in-game warehouse browsing and purchasing.
- Repair jobs reserve warehouse stock for the four currently modeled fault systems.
- Added save-state v4 and a v3-to-v4 migration path for existing garage progress.
- Added the warehouse inventory screen.

### Fixed
- Mechanic availability is checked before stock reservation.
- Save writes now include inventory and are verified by reading back the saved state.

### Known limitations
- Only four of ten catalogue parts are currently consumed by modeled repair faults.
- Warehouse capacity upgrades and supplier ordering are not integrated.
- Save migration and gameplay still require physical-device testing.

### Validation
- CI firmware compilation, SD artwork generation and content/package validation passed for commit 13bf92b.
- Hardware gameplay and legacy-save migration tests are pending.

### Build & package
- Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Upgrade
- Back up SD and internal Flash saves before flashing.
- Preserve the existing game save; verify v3-to-v4 migration on first launch.
- Merge the matching SD archive at the card root without deleting unrelated files.
- Test purchase → repair → payment → reboot → save restoration.


## [0.2.0-rc.2] — 2026-11-16 — JDM Garage Empire content loader (experimental)

### Added
- Added the versioned JDM Garage Empire SD content pack and a bounded JSON loader for game settings and seven catalogues.
- Added schema, size, nesting, record-count, required-field, duplicate-ID and numeric validation.
- Added the in-game Content panel (C) with content source, version, catalogue counts and sample entries.
- SD content now controls car names, starting conditions, daily rent and mechanic wages.

### Fixed
- Daily rent and mechanic wages now use the configured content values.

### Changed
- Firmware version synchronized to v0.2.0-rc.2; CI validates the JDM content pack before packaging.

### Validation
- CI firmware compilation and repository/content/package validation are required before publication.
- Hardware smoke test on Waveshare N16R8 remains pending.

### Known limitations
- Services, parts, customers, upgrades, events and tutorial are loaded and validated but do not yet drive their full gameplay systems.
- This is an experimental content-loader milestone, not a complete playable game.

### Build & package
- Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Upgrade
- Back up SD and important internal Flash data before flashing.
- Install the matching SD archive at the SD-card root while preserving personal files and saves.
- Launch JDM Garage Empire and press C to inspect the loaded content.

## [0.2.0-rc.1] — 2026-11-16 — Experimental pre-release

### Added
- Introduced explicit app runtime states STOPPED / RUNNING and enter/leave transition boundaries around the existing view-based UI.
- Added allocation-free per-view launch counters, last-session and accumulated runtime measurements.
- Added internal RAM and PSRAM snapshots at application entry and exit, including total, free, minimum-free and largest contiguous block.
- Added bilingual app lifecycle architecture and hardware validation documentation.

### Changed
- Legacy navigation is preserved; the runtime bridge observes view changes from the main loop.
- Firmware and release workflow fallback version are synchronized to v0.2.0-rc.1.

### Validation
- CI firmware compilation and firmware/SD package validation are required before publication.
- Hardware smoke test must verify boot, desktop navigation, repeated app switching, matching [APP] enter/leave events and stable internal RAM / PSRAM trends.

### Known limitations
- Applications still share one address space; these boundaries are lifecycle telemetry, not process isolation.
- No dynamic SD executable loading, automatic heap reclamation, or automatic shutdown of Wi-Fi, GPS, audio, recorder or sequencer services is introduced.
- Existing blocking screens remain unchanged; cooperative start / handle / draw / stop app hooks are a later migration step.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Flash using the Web Flasher and keep USB/power connected until completion.
- After boot, switch between several applications repeatedly and capture Serial output containing [APP] and [MEM] lines.

# Changelog

## [0.2.0-rc.3] - 2026-11-17

### Added
- Added the six-car JDM launch roster: Skyline GT-R R34, Supra A80, RX-7 FD3S, Silvia S15, Lancer Evolution VI and NSX NA1.
- Added a CI-generated 1-bit monochrome car and component art pack on the SD card.
- Added mechanical system and connector/mating-node metadata to the parts catalogue.
- Repair view now renders the matching SD component illustration with numbered connection callouts.

### Changed
- Repair replacement prices now follow the launch parts catalogue.
- Save validation now rejects vehicle indices outside the six-car launch roster.
- CI generates and validates the SD artwork pack before packaging.

### Known limitations
- The game remains an experimental release candidate; deeper inventory, tuning, staff and progression systems are still being expanded.
- Hardware validation on the Waveshare N16R8 is still required.



## [0.1.2-rc.12] — 2026-11-16 — Experimental pre-release

### Added
- App Manager is now a dedicated application icon on the desktop launcher, separate from the general file browser.
- Expanded the launcher to a four-column grid and added a dedicated monochrome App Manager icon.
- App Manager remembers its launch origin and returns to the desktop when launched from its desktop icon; the existing Documents / SD App Library entry keeps its previous return path.
- Runtime diagnostics identify the SD catalogue views as `APP_MANAGER` and `APP_MANAGER_DETAILS` for clearer lifecycle and memory-log analysis.

### Changed
- Launcher navigation now handles a partially filled final row without selecting empty cells.
- Firmware version and branch-build release fallback are synchronized to v0.1.2-rc.12.

### Validation
- CI firmware compilation and package validation are required before publication.
- Hardware validation must confirm all 13 launcher entries, four-way navigation, App Manager launch, package browsing and return-to-desktop behavior.
- Repeat app switching and capture `[APP]` / `[MEM]` Serial logs; memory diagnostics remain observational and do not imply process isolation or automatic cleanup.

### Known limitations
- Built-in apps still share one address space; explicit per-app start/stop resource ownership is not introduced in this candidate.
- App Manager remains a bounded, read-only catalogue; SD packages are data-only and no SD executable is loaded or run.

## [0.1.2-rc.11] — 2026-11-16 — Experimental pre-release

### Added
- Versioned default SD-card content source under `sdcard/root/`, packaged as a release-matched ZIP with an embedded file manifest and SHA-256 sidecar.
- PocketOS Guide data-only package on the SD card, providing a ready-made App Manager/resource-preview example.
- Web Flasher now offers the SD-card content archive and checksum for the selected release.
- Bounded SD package integrity audit when the App Manager catalogue is scanned.
- Package details now report resource-directory presence, valid file count, aggregate data size and warnings for empty folders, unsafe filenames or incomplete scans.
- Manifest values containing control characters are rejected.

### Changed
- SD-card release archives are mergeable defaults only; user files, saves, settings and other runtime data are excluded and must be preserved.
- Integrity warnings do not hide otherwise valid packages; users can still open the package and inspect available resources.
- The audit is read-only, scans at most 64 resource entries per package and never executes SD content.

### Validation
- CI firmware compilation and repository integrity checks are required before publication.
- On-device validation must cover clean, missing, empty, unsafe-name and over-limit resource directories.

## [0.1.2-rc.10] — 2026-11-16 — Experimental pre-release

### Added
- Read-only SD package resource browser: lists up to 24 top-level files from a package's `resources/` directory, with a 64-entry scan limit.
- Paginated preview for `.txt`, `.md` and `.ini` files up to 4 KiB, with bounded line rendering and safe filename filtering.

### Safety and limitations
- No recursive traversal, writes, deletes, extraction or execution of SD content.
- Unsupported formats and oversized text files remain listed but cannot be previewed; hardware validation is pending.

## [0.1.2-rc.9] — 2026-11-16 — Experimental pre-release

### Added
- Expanded application-transition memory snapshots with total and minimum-free capacity for both internal heap and PSRAM.
- Memory diagnostics now report total, current free, low-water mark and largest contiguous free block for each memory pool.

### Fixed
- Branch-triggered CI builds no longer fail when the fallback prerelease already exists and contains firmware assets; the existing release is left untouched.
- Tag-triggered builds continue to reject overwriting a release that already contains assets.

### Changed
- RC9 extends RC8's observational diagnostics only; it does not change app lifecycle, allocation policy, display, storage or network behavior.

### Validation
- CI must compile and package the firmware before publication.
- Hardware check: compare `internal_min` and `psram_min` after repeated navigation; compare `*_free` and `*_largest` for fragmentation trends.

### Known limitations
- Transition snapshots show memory state at screen boundaries, not the exact allocation responsible for a leak.
- No automatic memory reclamation or app isolation is introduced.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- After flashing, navigate through several apps repeatedly and capture Serial lines beginning with `[MEM]`.

## [0.1.2-rc.8] — 2026-11-16 — Experimental pre-release

### Added
- Memory snapshots at every application-view entry and exit in Serial diagnostics.
- Separate internal-heap and PSRAM metrics: free bytes, minimum free bytes for internal heap, and largest free contiguous block.

### Changed
- Diagnostics are observational and allocation-free; application behavior, screen rendering, storage and peripheral configuration are unchanged.
- RC7 HTTPS certificate validation was confirmed working by the user on hardware.

### Validation
- CI compilation and firmware package validation are required before publication.
- On-device diagnostic test: navigate between several applications and confirm [MEM] enter / [MEM] leave lines report internal heap and PSRAM values.
- For a stability run, repeat app switching and compare memory snapshots; investigate a persistent downward trend or shrinking largest block.

### Known limitations
- Snapshots are taken only at UI-view transitions; they do not identify the exact allocation responsible for a leak.
- This release adds measurement, not automatic reclamation or application isolation.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- After flashing, navigate between applications and inspect Serial output for [MEM] lines. Existing user-facing behavior should remain unchanged.

## [0.1.2-rc.7] — 2026-11-16 — Experimental pre-release

### Security
- HTTPS page and image downloads now validate server certificates against the Arduino-ESP32 built-in CA bundle.
- Removed insecure TLS mode. The web reader requests UTC time synchronization before TLS validation and fails closed when a trustworthy system time is unavailable.
- HTTPS-to-HTTP redirects and redirect URLs containing embedded credentials are rejected.

### Changed
- Plain HTTP remains available but is not encrypted or authenticated.
- Wi-Fi credential storage, SD package catalogue and app runtime behavior are unchanged.

### Validation
- CI compilation and package validation are required before publication.
- Hardware test: connect to Wi-Fi with internet access, open a valid HTTPS page and JPEG image, then confirm an invalid/untrusted TLS endpoint is blocked.
- If NTP is unavailable, HTTPS must fail with a clear message rather than bypass certificate checks.

### Known limitations
- The built-in CA bundle depends on the pinned Arduino-ESP32 core.
- Initial TLS use requires outbound NTP access when the system clock is unset.

### Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- After flashing, test HTTPS article and JPEG loading and confirm an invalid certificate is rejected.


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
- Браузер ресурсов SD-пакетов только для чтения: показывает до 24 файлов верхнего уровня в папке `resources/`, с ограничением сканирования 64 элементами.
- Постраничный просмотр файлов `.txt`, `.md` и `.ini` размером до 4 КБ с ограничением длины отображаемых строк и проверкой имён файлов.

### Проверено
- По сообщению пользователя, v0.1.1-rc.2 успешно прошилась на Waveshare ESP32-S3-ePaper-3.97 (N16R8), PocketOS запускается и работает. Это первичная проверка запуска, а не полная проверка всей периферии.

### Изменено
- Подготовка релизов переведена на версионные Git-теги; обычные push в ветку собирают и проверяют артефакты, но не переиспользуют существующий релиз и не перезаписывают его.

### Безопасность и ограничения
- Нет рекурсивного обхода, записи, удаления, распаковки или запуска содержимого SD-карты.
- Неподдерживаемые форматы и слишком большие текстовые файлы отображаются в списке, но не открываются; аппаратная проверка ещё не выполнена.

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

## [0.1.2-rc.9] — 2026-11-16 — Экспериментальный пререлиз

### Добавлено
- Расширены снимки памяти при переходах между приложениями: общий объём и минимальный остаток внутренней heap-памяти и PSRAM.
- Для каждого пула памяти показываются общий объём, текущая свободная память, минимальный остаток и крупнейший непрерывный свободный блок.

### Исправлено
- Сборки из ветки больше не завершаются ошибкой, если резервный пререлиз уже опубликован и содержит файлы прошивки; существующий релиз остаётся нетронутым.
- Сборка по Git-тегу по-прежнему запрещает перезапись релиза, в котором уже есть файлы.

### Изменено
- RC9 расширяет только диагностические измерения RC8; поведение приложений, дисплей, хранилище и сеть не меняются.

### Проверка
- Перед публикацией обязательны успешные CI-сборка и упаковка прошивки.
- На устройстве сравните `internal_min` и `psram_min` после повторной навигации; по `*_free` и `*_largest` можно оценивать фрагментацию.

### Известные ограничения
- Снимки при переходах показывают состояние памяти, но не определяют конкретное место утечки.
- Автоматическое освобождение памяти и изоляция приложений не добавляются.

### Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во Flash.
- После прошивки несколько раз переключитесь между приложениями и сохраните строки Serial, начинающиеся с `[MEM]`.


## [0.1.2-rc.10] — 2026-11-16 — Экспериментальный предварительный выпуск

### Добавлено
- Браузер ресурсов SD-пакетов только для чтения: список до 24 файлов верхнего уровня из `resources/` пакета; ограничение сканирования — 64 записи.
- Постраничный просмотр файлов `.txt`, `.md` и `.ini` размером до 4 КиБ с ограниченным выводом строк и безопасной фильтрацией имён.

### Безопасность и ограничения
- Нет рекурсивного обхода, записи, удаления, распаковки или исполнения содержимого SD-карты.
- Неподдерживаемые форматы и слишком большие текстовые файлы отображаются в списке, но не открываются для просмотра.
- Проверка на реальном устройстве ещё требуется.
