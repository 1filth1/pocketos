# JDM Garage Empire — M4 Parts Warehouse

## Русский

M4 превращает запчасти из мгновенной покупки в управляемый ресурс гаража.

### Игровой цикл
1. Начните новую кампанию: стартовые остатки десяти типов деталей загружаются из SD-каталога `parts.json`.
2. После диагностики откройте склад клавишей `I`.
3. Выберите деталь стрелками вверх/вниз и нажмите Enter, чтобы заказать одну единицу.
4. Стоимость списывается сразу, а заказ попадает во входящие поставки.
5. Срок доставки составляет от одного до трёх игровых дней. Поставка поступает при переходе на следующий день.
6. При ремонте нужная деталь резервируется со склада. Ручной ремонт или назначение механику расходует зарезервированную деталь.
7. Если активный клиент уходит до начала ремонта, зарезервированная деталь возвращается на склад.

### Склад
- Десять отдельных позиций из SD-каталога: тормоза, топливная система, охлаждение, подвеска и детали двигателя.
- Экран показывает остаток, количество в пути и текущую цену заказа.
- Максимум 50 единиц каждого типа, включая уже заказанные.
- Событийные скидки кампании учитываются в цене закупки.
- Повторные заказы одной позиции объединяются в поставку с более поздней датой прибытия.

### Управление
- `I` — открыть или закрыть склад.
- Стрелки вверх/вниз — выбрать позицию.
- `Enter` — заказать одну единицу выбранной детали.
- `J` — доска заказов клиентов.
- После заказа продвигайте игровой день, чтобы получить поставку.

### Сохранения и совместимость
Формат сохранения повышен до версии 4. В нём хранятся остатки, входящие поставки и даты доставки. Сохранения версии 3 мигрируют с сохранением активного заказа и очереди клиентов; купленная, но ещё не установленная деталь переносится в склад. Более старые форматы также мигрируют с начальными складскими остатками.

### Ограничения текущего этапа
- Заказ пока оформляется по одной единице за раз.
- Доставка привязана к игровым дням, а не к реальному времени.
- Качество детали отображается в каталоге данных, но пока не влияет на надёжность ремонта или репутацию.
- Проверка CI подтверждает компиляцию и корректность SD-каталога, но не заменяет проверку игрового цикла на устройстве.

## English

M4 makes replacement parts a persistent workshop resource instead of an instant one-off purchase.

- Ten part types are initialized from the SD card parts catalogue.
- Press `I` to open the warehouse, use Up/Down to select a part, and Enter to order one unit.
- Purchase cost is charged immediately. Deliveries arrive after one to three in-game days.
- The warehouse screen shows on-hand stock, inbound quantity, and current unit cost.
- A diagnosed repair reserves the matching part. Starting a manual repair or assigning a mechanic consumes that reservation.
- If the active customer leaves before repair begins, the reserved part is returned to stock.
- Each part type is capped at 50 units, including inbound orders.
- Save format v4 stores stock and delivery schedules. Version-3 saves migrate while preserving the active job and waiting customer queue.

This milestone still requires on-device playtesting. Part quality is catalogue metadata for now and does not yet alter repair outcomes.
