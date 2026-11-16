# PocketOS Roadmap

> This roadmap describes development priorities, not promised delivery dates. Items move to **Done** only after implementation and evidence-based validation.

## v0.1 — Establish a reproducible baseline

- [ ] Review the current Schematic.io project export and repository source for consistency.
- [ ] Confirm the exact Waveshare board revision and ESP32-S3 configuration.
- [ ] Record framework, compiler and library versions.
- [ ] Verify Flash, PSRAM and partition settings.
- [ ] Produce a clean, repeatable build.
- [ ] Validate core display, input, storage and power-management functions on hardware.
- [ ] Publish firmware artifacts with checksums and verified flashing instructions.

## v0.x — Improve system foundations

- [x] Document the current firmware architecture and shared-address-space application model.
- [x] Add initial UI-view lifecycle telemetry to help trace navigation.
- [ ] Add explicit app start/stop hooks and resource ownership only after the current routes are mapped and tested.
- [ ] Define clear interfaces between system services and built-in applications.
- [ ] Improve error reporting, diagnostics and recovery behavior.
- [x] Add internal-heap and PSRAM snapshots at application-view transitions (RC8; user confirmed RC8 boots and works; log-based trend analysis remains).
- [x] Extend memory snapshots with total capacity and low-water marks for internal heap and PSRAM (RC9).
- [ ] Analyze repeated-navigation Serial logs from RC9 for memory leaks and fragmentation.
- [ ] Measure memory use and stability during long sessions and application switching.
- [ ] Review SD-card data layout and safe handling of interrupted writes.
- [ ] Add automated checks that can run without target hardware.

## Future exploration

These are ideas for evaluation, not commitments or implemented features:

- [x] Add a bounded, read-only SD content-package metadata catalogue (RC6; confirmed working on hardware).
- [ ] Add safe resource browsing and package integrity checks.
- [ ] Add explicit install/remove lifecycle for data-only packages.
- [ ] Map approved package IDs to firmware-compiled app modules; never execute arbitrary SD binaries.
- [x] Remove insecure TLS mode from the web reader and validate HTTPS certificates (RC7; user-confirmed working on hardware).
- More capable network and file-transfer services.
- System update and recovery workflow.
- Expanded peripheral support.

## Русский

> Это план приоритетов разработки, а не обещание сроков. Пункт считается выполненным только после реализации и проверки с подтверждающими результатами.

### v0.1 — Надёжная исходная точка

- [ ] Сверить экспорт проекта Schematic.io с исходниками в репозитории.
- [ ] Подтвердить ревизию платы Waveshare и конфигурацию ESP32-S3.
- [ ] Зафиксировать версии framework, компилятора и библиотек.
- [ ] Проверить настройки Flash, PSRAM и таблицу разделов.
- [ ] Получить чистую воспроизводимую сборку.
- [ ] Проверить на устройстве дисплей, ввод, SD-карту и управление питанием.
- [ ] Опубликовать прошивку с контрольными суммами и проверенной инструкцией установки.

### v0.x — Развитие основы системы

- [ ] Описать архитектуру прошивки и жизненный цикл приложений.
- [ ] Определить интерфейсы системных служб и встроенных приложений.
- [ ] Улучшить диагностику, обработку ошибок и восстановление.
- [x] Расширить снимки памяти: общий объём и минимальный остаток внутренней heap-памяти и PSRAM (RC9).
- [ ] По Serial-логам RC9 проверить утечки и фрагментацию при многократном переключении приложений.
- [ ] Измерить потребление памяти и стабильность при длительной работе.
- [ ] Проверить структуру данных SD-карты и безопасность прерванной записи.
- [ ] Добавить автоматические проверки, не требующие целевого устройства.

### Идеи на будущее

Это направления для исследования, а не обещание реализации:

- [ ] Каталог метаданных пакетов контента на SD (RC6).
- Безопасный просмотр ресурсов и проверка целостности пакетов.
- Явная установка/удаление пакетов данных.
- Связь разрешённых ID пакетов со встроенными в прошивку модулями; без запуска произвольных бинарников с SD.
- Развитие сетевых служб и обмена файлами.
- Обновление системы и сценарий восстановления.
- Поддержка дополнительных периферийных модулей.
