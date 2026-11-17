# PocketOS v0.2.0-rc.3 — JDM Garage Empire visual workshop candidate

## English

### Added
- Replaced the generic launch roster with six approved JDM-inspired vehicles: Nissan Skyline GT-R R34, Toyota Supra A80, Mazda RX-7 FD3S, Nissan Silvia S15, Mitsubishi Lancer Evolution VI and Honda NSX NA1.
- Added a CI artwork generator that creates a 1-bit monochrome car and mechanical-component pack for the SD card. The firmware remains a renderer/loader; the artwork itself is not compiled into PROGMEM.
- Added system, connector-node and mating-interface metadata to every launch parts-catalogue record.
- The repair screen now displays a component illustration for the selected fault system, alongside numbered service/connection callouts.

### Fixed
- Corrected client-car cycling and save validation so both stay within the six-car launch catalogue.

### Changed
- Replacement-part prices now match the selected launch catalogue components: brake pads, fuel pump, radiator and coilover kit.
- Client-car selection and save validation are constrained to the six-car launch roster.
- CI generates the SD art pack, writes its SHA-256 manifest and validates the complete pack before packaging.

### Known issues
- This is an experimental release candidate. The current workshop loop covers accepting a client car, diagnosing one of four fault systems, ordering a replacement, completing a repair sequence, running quality control and collecting payment.
- Full inventory management, multi-part repair trees, detailed tuning dependencies, staff progression and additional cars remain future work.
- The generated monochrome artwork is the first in-device asset pass; higher-detail final art can replace it without changing the SD asset format.
- CI success does not prove hardware behavior. A Waveshare N16R8 device test is still required.

### Build & package
- Board: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; `app3M_fat9M_16MB`.
- Release bundle includes firmware images, manifest, SHA-256 checksums and the matching SD-card archive.

### Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Flash v0.2.0-rc.3 using the Web Flasher after the release workflow completes.
- Extract the matching SD-card archive to the card root. Merge the `Games/JDM_Garage_Empire` folder and preserve existing saves and unrelated files.
- Launch JDM Garage Empire. Verify that the six cars render in the garage and that the repair screen shows the correct component illustration. Report any missing-art status or restart.

## Русский

### Добавлено
- Стартовый автопарк заменён на шесть утверждённых JDM-моделей: Nissan Skyline GT-R R34, Toyota Supra A80, Mazda RX-7 FD3S, Nissan Silvia S15, Mitsubishi Lancer Evolution VI и Honda NSX NA1.
- Добавлен генератор 1-битной монохромной графики автомобилей и механических деталей. Он создаёт пакет на SD-карте во время CI; изображения не зашиваются в PROGMEM прошивки.
- В каталог запчастей добавлены системы, узлы соединения и ответные интерфейсы креплений.
- На экране ремонта теперь отображается иллюстрация компонента выбранной неисправности и нумерованные выноски сервисных точек.

### Изменено
- Стоимость замены соответствует деталям стартового каталога: тормозные колодки, топливный насос, радиатор и комплект койловеров.
- Выбор клиентской машины и проверка сохранений ограничены шестью автомобилями первой версии.
- CI генерирует графический пакет, записывает SHA-256 в манифест и проверяет комплектность перед упаковкой.

### Известные ограничения
- Это экспериментальный кандидат релиза. Текущий игровой цикл включает приём машины, диагностику одной из четырёх систем, заказ детали, последовательность ремонта, контроль качества и получение оплаты.
- Полноценный инвентарь, многоступенчатые деревья ремонта, зависимости тюнинга, развитие персонала и новые автомобили остаются дальнейшими задачами.
- Текущая монохромная графика — первый проход для устройства; более детальные финальные изображения можно будет заменить без изменения формата SD-ресурсов.
- Успешная CI-сборка не заменяет проверку на устройстве. Требуется тест на Waveshare N16R8.

### Сборка и пакет
- Плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 МГц; `app3M_fat9M_16MB`.
- В комплект входят образы прошивки, манифест, SHA-256 и соответствующий архив SD-карты.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во внутренней Flash.
- После завершения workflow прошейте v0.2.0-rc.3 через Web Flasher.
- Распакуйте соответствующий SD-архив в корень карты. Объедините папку `Games/JDM_Garage_Empire`, сохранив старые сохранения и остальные файлы.
- Запустите JDM Garage Empire. Проверьте отображение шести машин в гараже и правильной детали на экране ремонта. Сообщите, если графика не загрузилась или устройство перезапустилось.
