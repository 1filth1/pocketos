# Test Plan

> No test in this document is marked as passed by default. Record actual results, hardware revision, firmware commit and test date before changing a status to PASS.

## Result format

For each test, record:

- Result: PASS / FAIL / BLOCKED / NOT RUN
- Firmware commit:
- Board model and revision:
- Test date:
- Steps and observed result:
- Logs, photos or other evidence:

## Hardware smoke tests

| ID | Test | Expected result | Status |
|---|---|---|---|
| HW-01 | Cold boot | Device starts without reset loop | NOT RUN |
| HW-02 | E-Ink initialization | Display initializes and renders the home screen | NOT RUN |
| HW-03 | Input device | Key presses are detected reliably | NOT RUN |
| HW-04 | RTC | Time can be read and displayed correctly | NOT RUN |
| HW-05 | Environmental sensor | Valid readings are displayed or sensor absence is handled | NOT RUN |
| HW-06 | SD card present | Card mounts and files can be listed | NOT RUN |
| HW-07 | SD card absent | System reports unavailable storage without crashing | NOT RUN |
| HW-08 | Battery/power telemetry | Readings are plausible and failures are handled | NOT RUN |

## Core software tests

| ID | Test | Expected result | Status |
|---|---|---|---|
| SW-01 | Launcher navigation | All registered entries can be selected | NOT RUN |
| SW-02 | Application entry/exit | Navigation does not leave stale or corrupted UI state | NOT RUN |
| SW-03 | Long text rendering | Text remains within display bounds | NOT RUN |
| SW-04 | File operations | Errors and unavailable files are handled safely | NOT RUN |
| SW-05 | Interrupted SD write | Filesystem remains recoverable; data-loss behavior documented | NOT RUN |
| SW-06 | Wi-Fi unavailable | Network-dependent features fail gracefully | NOT RUN |
| SW-07 | HTTP/TLS failure | Timeouts and connection errors are handled | NOT RUN |
| SW-08 | Extended runtime | No progressive memory exhaustion or unexpected reset | NOT RUN |

## Release gate

A build should not be described as a verified release until:

- [ ] Build instructions are reproducible from a clean environment.
- [ ] Toolchain and dependency versions are recorded.
- [ ] Flash and partition settings are confirmed.
- [ ] A firmware artifact is produced and its checksum recorded.
- [ ] Relevant hardware smoke tests are completed.
- [ ] Known limitations and failed tests are documented.

## Русский

> Ни один тест не считается пройденным автоматически. Перед установкой статуса PASS укажите реальный результат, ревизию платы, коммит прошивки и дату проверки.

### Формат записи результата

Для каждого теста фиксируйте:

- Результат: PASS / FAIL / BLOCKED / NOT RUN.
- Коммит прошивки.
- Модель и ревизию платы.
- Дату проверки.
- Шаги и фактический результат.
- Логи, фотографии или другие подтверждения.

### Аппаратные проверки

| ID | Проверка | Ожидаемый результат | Статус |
|---|---|---|---|
| HW-01 | Холодный запуск | Устройство стартует без циклической перезагрузки | NOT RUN |
| HW-02 | Инициализация E-Ink | Дисплей запускается и показывает главный экран | NOT RUN |
| HW-03 | Устройство ввода | Нажатия клавиш стабильно распознаются | NOT RUN |
| HW-04 | RTC | Время корректно считывается и отображается | NOT RUN |
| HW-05 | Датчик окружающей среды | Корректные значения отображаются или отсутствие датчика обрабатывается | NOT RUN |
| HW-06 | SD-карта установлена | Карта монтируется, файлы отображаются | NOT RUN |
| HW-07 | SD-карта отсутствует | Система сообщает об отсутствии хранилища без аварии | NOT RUN |
| HW-08 | Телеметрия питания | Значения правдоподобны, ошибки обрабатываются | NOT RUN |

### Программные проверки

| ID | Проверка | Ожидаемый результат | Статус |
|---|---|---|---|
| SW-01 | Навигация лаунчера | Все зарегистрированные пункты доступны | NOT RUN |
| SW-02 | Вход и выход из приложений | Переходы не оставляют повреждённое состояние UI | NOT RUN |
| SW-03 | Отображение длинного текста | Текст остаётся в пределах экрана | NOT RUN |
| SW-04 | Операции с файлами | Ошибки и отсутствующие файлы обрабатываются безопасно | NOT RUN |
| SW-05 | Прерывание записи на SD | Файловая система остаётся восстанавливаемой; поведение при потере данных описано | NOT RUN |
| SW-06 | Недоступность Wi-Fi | Сетевые функции корректно сообщают об ошибке | NOT RUN |
| SW-07 | Ошибки HTTP/TLS | Тайм-ауты и ошибки соединения обрабатываются | NOT RUN |
| SW-08 | Длительная работа | Нет постепенного исчерпания памяти и неожиданных перезагрузок | NOT RUN |

### Условия публикации релиза

Сборку нельзя называть проверенным релизом, пока:

- [ ] Инструкция сборки не воспроизводится в чистой среде.
- [ ] Версии инструментов и библиотек не зафиксированы.
- [ ] Параметры Flash и разделов не подтверждены.
- [ ] Бинарный файл не собран и его контрольная сумма не записана.
- [ ] Не завершены необходимые аппаратные smoke tests.
- [ ] Не описаны известные ограничения и проваленные тесты.
