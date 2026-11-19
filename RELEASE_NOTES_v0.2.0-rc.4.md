# PocketOS v0.2.0-rc.4 — JDM Garage Empire warehouse & save migration

## English

### Added
- Added a persistent ten-item parts warehouse with stock initialized from the SD parts catalogue.
- Added warehouse browsing and purchasing from the game UI (I key; arrows to select; Enter/B to buy).
- Repair jobs can reserve available stock for the four currently modeled fault systems; assigned mechanics use the reserved part rather than buying a duplicate.
- Added save-state version 4 with a version 3 migration path, preserving existing garage progress while introducing inventory data.
- Added inventory rows to the monochrome game UI.

### Fixed
- Mechanic assignment checks staff availability before reserving a part, avoiding stock being consumed when assignment cannot proceed.
- Save writes include inventory and are read back for verification.

### Changed
- The game now starts a new garage with warehouse stock based on the shipped parts catalogue.
- Firmware and game state schema are advanced for this candidate.

### Known issues
- The warehouse currently maps only four fault types to four parts. The other six catalogue items can be purchased but are not yet consumed by repair jobs.
- Warehouse capacity is currently capped at 99 per item; upgrade-based capacity and supplier ordering are not integrated.
- The v3-to-v4 migration compiles in CI but has not yet been exercised on the physical device with a real legacy save.
- This is an experimental gameplay candidate. Staff progression, multi-part repair trees, detailed tuning dependencies and broader campaign progression remain incomplete.
- CI success does not prove hardware behavior. Test on Waveshare N16R8 before treating this as a stable release.

### Build & package
- Board: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; `app3M_fat9M_16MB`.
- Release bundle is expected to include firmware images, manifest, SHA-256 checksums and the matching SD-card archive.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Preserve existing game saves. The firmware includes a v3-to-v4 migration, but keep a separate backup until migration is confirmed on-device.
- Install the matching SD archive at the SD-card root, merging `Games/JDM_Garage_Empire` and preserving unrelated files and saves.
- Launch JDM Garage Empire. Verify warehouse stock, buy one part, complete a repair using available stock, reboot, and confirm cash/inventory/progress persist.

## Русский

### Добавлено
- Добавлен постоянный склад из десяти видов деталей. Начальные остатки загружаются из каталога запчастей на SD-карте.
- Добавлены просмотр склада и покупка деталей прямо в игре (клавиша I; стрелки — выбор; Enter/B — покупка).
- Заказы на ремонт могут резервировать детали со склада для четырёх уже реализованных типов неисправностей. Назначенный механик использует зарезервированную деталь, а не покупает вторую.
- Добавлена версия сохранения 4 и миграция формата версии 3: прогресс гаража сохраняется, появляется новый массив инвентаря.
- Добавлен экран склада со списком деталей для монохромного дисплея.

### Исправлено
- Перед резервированием детали проверяется доступность механика. Если назначение невозможно, складской остаток не расходуется.
- Сохранение включает инвентарь и проверяется чтением записанных данных.

### Изменено
- При создании нового гаража склад заполняется по начальному каталогу деталей с SD-карты.
- Для этого кандидата обновлены версия прошивки и схема игрового сохранения.

### Известные ограничения
- Сейчас склад связан только с четырьмя типами неисправностей и четырьмя деталями. Остальные шесть позиций можно покупать, но ремонт пока их не расходует.
- Вместимость пока ограничена 99 единицами на позицию; улучшения вместимости и заказы поставщикам ещё не подключены.
- Миграция сохранения v3 → v4 успешно проходит компиляцию в CI, но ещё не проверена на реальном устройстве с существующим сохранением.
- Это экспериментальный игровой кандидат. Развитие персонала, многоэтапные ремонты, подробные зависимости тюнинга и дальнейшее развитие кампании ещё не завершены.
- Успешная CI-сборка не подтверждает работу на устройстве. Перед признанием версии стабильной необходим тест на Waveshare N16R8.

### Сборка и пакет
- Плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 МГц; `app3M_fat9M_16MB`.
- В релизный комплект должны войти образы прошивки, манифест, SHA-256 и соответствующий архив SD-карты.

### Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не удаляй существующие сохранения. В прошивке есть миграция v3 → v4, но сохрани отдельную резервную копию до подтверждения миграции на устройстве.
- Установи соответствующий SD-архив в корень карты, объединив папку `Games/JDM_Garage_Empire` и сохранив остальные файлы и сохранения.
- Запусти игру. Проверь остатки склада, покупку детали, ремонт с использованием складской детали, перезагрузку и сохранность денег, инвентаря и прогресса.
