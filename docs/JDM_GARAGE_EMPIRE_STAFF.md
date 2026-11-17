# JDM Garage Empire — M5 Staff Management

## Русский

M5 превращает найм механика из одноразового переключателя в небольшую систему управления персоналом.

### Состав команды
- В гараже можно нанять до двух механиков.
- Первый найм стоит 500 CR, второй — 850 CR.
- Каждый новый механик начинает с навыком 1 (ученик).
- На экране персонала отображаются выбранный сотрудник, уровень навыка и его дневная зарплата.

### Навыки и обучение
- Уровень 1 — ученик: ремонт занимает два игровых дня.
- Уровень 2 — техник: ремонт занимает один игровой день.
- Уровень 3 — эксперт: ремонт занимает один игровой день; дальнейшее обучение недоступно.
- Обучение стоит 600 CR за текущий уровень: 600 CR для перехода 1→2 и 1200 CR для 2→3.
- Зарплата каждого сотрудника увеличивается на 75 CR/день за каждый уровень выше первого.

### Рабочий цикл
1. Нажмите S, чтобы открыть экран персонала.
2. Используйте Up/Down для выбора механика.
3. H нанимает сотрудника, T обучает выбранного механика.
4. После диагностики и резервирования детали нажмите M, чтобы назначить выбранного механика.
5. Продвигайте игровой день. Оставшееся время ремонта уменьшается автоматически.
6. По завершении работа переходит на этап контроля качества; после него можно получить оплату.

### Экономика и сохранения
Дневные расходы включают аренду и зарплату всех нанятых механиков. Сохранение версии 5 хранит состав команды, навыки и назначенного сотрудника. Миграция из версии 4 сохраняет склад, поставки, активный заказ и очередь клиентов; ранее нанятый механик переносится как сотрудник первого уровня. Более старые версии продолжают проходить через существующие миграции.

### Ограничения этапа
- Одновременно ведётся один активный ремонт, поэтому механики пока не работают параллельно.
- Усталость, настроение, специализации и случайные ошибки персонала пока не моделируются.
- Навык влияет на длительность ремонта, но не на цену/качество результата.
- CI-сборка и проверка сохранений не заменяют игровое тестирование на устройстве.

## English

M5 adds a compact staff-management system instead of treating hiring as a single on/off upgrade.

- Hire up to two mechanics. Hiring costs 500 CR for the first mechanic and 850 CR for the second.
- Every new mechanic starts at skill level 1. Level 2 mechanics complete jobs in one in-game day; level 1 mechanics take two days. Level 3 is an expert tier and also completes jobs in one day.
- Training costs 600 CR per current skill level: 600 CR for level 1→2 and 1200 CR for level 2→3.
- Daily payroll includes every mechanic, with a 75 CR/day premium per skill level above level 1.
- Press S to open the staff roster, Up/Down to select a mechanic, H to hire, and T to train. After diagnosing a fault and reserving its part, press M to assign the selected mechanic.
- Advance in-game days to progress mechanic work. Completed jobs move to quality control.
- Save format v5 stores staff count, skills, and assignment state. Version-4 saves preserve warehouse inventory, inbound deliveries, active jobs, and the customer queue; an existing mechanic migrates as a level-1 employee.

Parallel work bays, fatigue, morale, specializations, and staff-driven quality bonuses remain future work. Hardware gameplay testing is still required.
