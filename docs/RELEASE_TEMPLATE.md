# PocketOS vX.Y.Z — Release

> Release status: Draft / Pre-release / Stable  
> Do not mark a release Stable until all release gates in `docs/RELEASE_PROCESS.md` are satisfied.

## Summary
<!-- Briefly describe this release in Russian and English. -->

## What's changed
### Added
- 

### Changed
- 

### Fixed
- 

## Build information
- Source tag:
- Commit:
- Target board (exact product name and revision):
- Framework / toolchain:
- Board definition:
- Flash size:
- PSRAM configuration:
- Partition table:
- Dependency lock / manifest:
- Build environment and command:
- Build log:

## Firmware artifacts
| File | SHA-256 | Size | Notes |
|---|---|---:|---|
| | | | |

Attach only binaries built from the exact source tag above. Never publish placeholder, unverified or board-mismatched firmware images.

## Flashing instructions
1. Confirm that the exact board model and hardware revision match the build information.
2. Back up SD-card contents and any user data.
3. Download the firmware artifact(s) attached to this release.
4. Use the verified flashing tool and exact offsets/settings documented for this board revision.
5. Do not guess flash addresses, partition layouts, bootloader offsets or USB settings.
6. After flashing, record the boot result and run the smoke tests listed below.

### Verified flashing configuration
- Flashing tool and version:
- Connection (USB/UART):
- Baud rate:
- Flash mode / frequency:
- Erase-flash requirement:
- File-to-address map:
- Recovery procedure:

## Validation
| Test | Result (PASS / FAIL / BLOCKED / NOT RUN) | Notes |
|---|---|---|
| Clean reproducible build | NOT RUN | |
| Cold boot | NOT RUN | |
| E-Ink display | NOT RUN | |
| Keyboard / input | NOT RUN | |
| SD-card mount and file access | NOT RUN | |
| RTC and sensors | NOT RUN | |
| Power / battery telemetry | NOT RUN | |
| Wi-Fi and network features | NOT RUN | |
| Extended runtime | NOT RUN | |

- Test date:
- Test hardware revision:
- Tested firmware commit:
- Evidence / logs:

## Known limitations
- 

## Upgrade notes and data safety
- Back up SD-card contents before flashing.
- State explicitly whether user data is preserved, migrated, or erased. Do not claim preservation unless tested.
- List any required migration or recovery steps.

## Contributors
- 

---

# PocketOS vX.Y.Z — Релиз

> Статус выпуска: Черновик / Предварительный релиз / Стабильный  
> Не указывайте «Стабильный», пока не выполнены все условия из `docs/RELEASE_PROCESS.md`.

## Краткое описание
<!-- Кратко опишите выпуск на русском и английском. -->

## Изменения
### Добавлено
- 

### Изменено
- 

### Исправлено
- 

## Информация о сборке
- Тег исходников:
- Коммит:
- Целевая плата (точное название и ревизия):
- Framework / toolchain:
- Определение платы:
- Объём Flash:
- Конфигурация PSRAM:
- Таблица разделов:
- Файл фиксации зависимостей / manifest:
- Среда и команда сборки:
- Лог сборки:

## Файлы прошивки
| Файл | SHA-256 | Размер | Примечания |
|---|---|---:|---|
| | | | |

Прикладывайте только бинарники, собранные из указанного тега. Не публикуйте заглушки, непроверенные образы или прошивки для другой ревизии платы.

## Инструкция по прошивке
1. Убедитесь, что модель и аппаратная ревизия платы совпадают с указанными в информации о сборке.
2. Сделайте резервную копию SD-карты и пользовательских данных.
3. Скачайте файл(ы) прошивки из этого релиза.
4. Используйте проверенную программу прошивки и точные адреса/параметры для этой ревизии платы.
5. Не подбирайте наугад адреса Flash, таблицу разделов, адрес загрузчика или настройки USB.
6. После прошивки зафиксируйте результат запуска и выполните базовые тесты.

### Подтверждённая конфигурация прошивки
- Программа и версия:
- Подключение (USB/UART):
- Скорость:
- Режим / частота Flash:
- Требуется ли полное стирание Flash:
- Соответствие файлов адресам:
- Процедура восстановления:

## Проверка
| Тест | Результат (PASS / FAIL / BLOCKED / NOT RUN) | Примечания |
|---|---|---|
| Воспроизводимая сборка из чистой среды | NOT RUN | |
| Холодный запуск | NOT RUN | |
| E-Ink-дисплей | NOT RUN | |
| Клавиатура / ввод | NOT RUN | |
| Монтирование SD и работа с файлами | NOT RUN | |
| RTC и датчики | NOT RUN | |
| Питание / телеметрия батареи | NOT RUN | |
| Wi-Fi и сетевые функции | NOT RUN | |
| Длительная работа | NOT RUN | |

- Дата тестирования:
- Ревизия тестовой платы:
- Проверенный коммит прошивки:
- Подтверждения / логи:

## Известные ограничения
- 

## Обновление и сохранность данных
- Перед прошивкой сделайте резервную копию SD-карты.
- Явно укажите, сохраняются ли пользовательские данные, переносятся или стираются. Не обещайте сохранность без тестирования.
- Перечислите необходимые действия по миграции или восстановлению.

## Участники
- 
