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

- [ ] Document the firmware architecture and application lifecycle.
- [ ] Define clear interfaces between system services and built-in applications.
- [ ] Improve error reporting, diagnostics and recovery behavior.
- [ ] Measure memory use and stability during long sessions and application switching.
- [ ] Review SD-card data layout and safe handling of interrupted writes.
- [ ] Add automated checks that can run without target hardware.

## Future exploration

These are ideas for evaluation, not commitments or implemented features:

- Modular application packaging and installation workflow.
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
- [ ] Измерить потребление памяти и стабильность при длительной работе.
- [ ] Проверить структуру данных SD-карты и безопасность прерванной записи.
- [ ] Добавить автоматические проверки, не требующие целевого устройства.

### Идеи на будущее

Это направления для исследования, а не обещание реализации:

- Модульная упаковка и установка приложений.
- Развитие сетевых служб и обмена файлами.
- Обновление системы и сценарий восстановления.
- Поддержка дополнительных периферийных модулей.
