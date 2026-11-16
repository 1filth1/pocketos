# PocketOS v0.2.0-rc.2 — JDM Garage Empire content loader (experimental)

## English

### Added
- Added the JDM Garage Empire v1 SD content pack under `/Games/JDM_Garage_Empire/content/`.
- Added a bounded ArduinoJson loader for `game.json` and the seven game catalogues: cars, services, parts, customers, upgrades, events and tutorial.
- Added validation for schema/game identity, file size, nesting, record limits, required fields, duplicate IDs, record types and core numeric ranges.
- Added an in-game Content panel (press **C**) showing the loaded content version, source, catalogue counts and sample entries.
- SD content now supplies car names, starting cash, reputation, day, daily rent and mechanic wages. Built-in defaults remain the fallback if SD content is absent or invalid.

### Fixed
- Daily operating expenses now use the configured rent and mechanic wage values instead of fixed constants.

### Changed
- Firmware version is synchronized to v0.2.0-rc.2.
- The SD content pack is validated by CI before packaging.

### Known issues
- Services, parts, customers, upgrades, events and tutorial catalogues are loaded and validated, but do not yet power the complete repair, inventory, customer, progression or event gameplay systems.
- This candidate has passed CI compilation/package validation only; hardware testing on the Waveshare N16R8 is still required.
- This is an experimental content-loader milestone, not a finished or fully playable game.

### Build & package
- Board: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; `app3M_fat9M_16MB`.
- Release includes firmware images, manifest, SHA-256 checksums and the matching SD-card content archive.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Use the Web Flasher entry for v0.2.0-rc.2 once its release workflow has completed.
- Install the matching SD-card archive to the card's root, preserving personal saves and files.
- After boot, launch JDM Garage Empire and press **C** to inspect the loaded content. Report any missing catalogue, fallback status or crash before relying on the build.

## Русский

### Добавлено
- Добавлен SD-пакет контента JDM Garage Empire v1 в `/Games/JDM_Garage_Empire/content/`.
- Реализован ограниченный загрузчик ArduinoJson для `game.json` и семи каталогов: автомобили, услуги, запчасти, клиенты, улучшения, события и обучение.
- Добавлены проверки версии схемы и ID игры, размера файлов, глубины JSON, лимитов записей, обязательных полей, повторяющихся ID, типов записей и основных числовых диапазонов.
- Добавлена внутриигровая панель Content (клавиша **C**) с версией и источником контента, количеством записей и примерами.
- Из SD-контента берутся названия автомобилей, стартовые деньги и репутация, день, ежедневная аренда и зарплата механика. При отсутствии или ошибке SD-контента используются встроенные значения по умолчанию.

### Исправлено
- Ежедневные расходы теперь рассчитываются по заданным значениям аренды и зарплаты механика, а не по фиксированным константам.

### Изменено
- Версия прошивки синхронизирована с v0.2.0-rc.2.
- CI проверяет SD-пакет контента перед упаковкой.

### Известные проблемы
- Каталоги услуг, запчастей, клиентов, улучшений, событий и обучения загружаются и проверяются, но пока не управляют полноценной игровой логикой ремонта, инвентаря, клиентов, развития и событий.
- Кандидат прошёл компиляцию и проверки упаковки в CI; проверка на Waveshare N16R8 ещё необходима.
- Это экспериментальный этап загрузчика контента, а не законченная или полностью играбельная версия.

### Сборка и пакет
- Плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 МГц; `app3M_fat9M_16MB`.
- В релиз входят образы прошивки, манифест, SHA-256 и соответствующий архив содержимого SD-карты.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во внутренней Flash.
- Используйте запись v0.2.0-rc.2 во Web Flasher после завершения workflow публикации.
- Распакуйте соответствующий SD-архив в корень карты, сохранив личные файлы и сохранения.
- После загрузки откройте JDM Garage Empire и нажмите **C**, чтобы проверить загруженный контент. Сообщите о пропущенных каталогах, переходе на значения по умолчанию или сбое.
