# PocketOS v0.1 — предварительный релиз

**Статус:** подготовка / pre-release draft  
**Целевая платформа:** Waveshare ESP32-S3 E-Ink, 800×480  
**Версия прошивки в исходниках:** 0.1

## Описание

PocketOS v0.1 — ранняя версия встроенной системы для ESP32-S3 с E-Ink-дисплеем. Проект объединяет графический интерфейс, встроенные приложения, работу с SD-картой, сетевые функции и поддержку подключённой периферии.

## Основные компоненты

- Главный экран и меню приложений.
- Файловый менеджер и работа с SD-картой.
- Заметки, календарь, таймер и часы RTC.
- Музыкальные и аудио-функции, диктофон.
- Просмотр изображений, видео, книг и веб-страниц.
- Wi-Fi и обмен файлами.
- GPS-функции при наличии соответствующего модуля.
- Встроенные игры.
- Отображение системных показателей: батарея, зарядка, температура, влажность, часы, сеть и SD-карта.

## Архитектура и ограничения

- Встроенные приложения входят в состав одной прошивки и работают в общем адресном пространстве.
- Изоляция процессов не реализована.
- Загрузка произвольных исполняемых приложений с SD-карты не реализована.
- Некоторые функции зависят от подключённой периферии и доступности SD-карты или Wi-Fi.

## Важное предупреждение

Это предварительная версия для разработки и тестирования. Она **не является проверенным стабильным релизом**. Перед публикацией прошивочных бинарников необходимо:

1. Получить и зафиксировать полный экспорт проекта из Schematic.io.
2. Подтвердить точную модель платы, Flash/PSRAM, настройки USB и схему разделов.
3. Зафиксировать версии Arduino-ядра и всех библиотек.
4. Настроить воспроизводимую сборку.
5. Успешно собрать прошивку и проверить её запуск на целевой плате.
6. Проверить дисплей, клавиатуру, SD-карту, RTC, датчики, питание, Wi-Fi и сохранность пользовательских данных.
7. Подготовить корректные адреса бинарных файлов для браузерного прошивальщика.

## Известные пробелы перед публикацией

- Нет подтверждённой воспроизводимой конфигурации сборки в репозитории.
- Нет проверенных релизных бинарных файлов и таблицы адресов прошивки.
- Нет опубликованного Web Flasher и его manifest.
- Нет протокола полного аппаратного тестирования.

## Рекомендации по обновлению

Не прошивайте эту версию поверх важной пользовательской конфигурации без резервной копии данных SD-карты. Не используйте случайные бинарные файлы или адреса прошивки от другой платы.

---

# PocketOS v0.1 — Pre-release notes

**Status:** preparation / pre-release draft  
**Target hardware:** Waveshare ESP32-S3 E-Ink, 800×480  
**Firmware version in source:** 0.1

## Overview

PocketOS v0.1 is an early embedded system for the ESP32-S3 with an E-Ink display. It combines a graphical interface, built-in applications, SD-card storage, networking features and support for connected peripherals.

## Main components

- Home screen and application launcher.
- File manager and SD-card access.
- Notes, calendar, timer and RTC clock.
- Music and audio features, voice recorder.
- Image, video, book and web viewers.
- Wi-Fi and file exchange.
- GPS features when a compatible module is connected.
- Built-in games.
- System status information: battery, charging, temperature, humidity, clock, network and SD-card status.

## Architecture and limitations

- Built-in applications are part of a single firmware image and share the same address space.
- Process isolation is not implemented.
- Loading arbitrary executable applications from the SD card is not implemented.
- Some features depend on connected peripherals, SD-card availability or Wi-Fi connectivity.

## Important notice

This is an early development and testing version. It is **not a verified stable release**. Before publishing firmware binaries, the following work is required:

1. Obtain and pin the complete project export from Schematic.io.
2. Confirm the exact board model, Flash/PSRAM, USB settings and partition layout.
3. Pin the Arduino core and all library versions.
4. Set up a reproducible build.
5. Build the firmware successfully and verify boot on the target board.
6. Test the display, keyboard, SD card, RTC, sensors, power management, Wi-Fi and user-data integrity.
7. Prepare verified firmware offsets for the browser-based flasher.

## Known release blockers

- No confirmed reproducible build configuration in the repository.
- No verified release firmware binaries or firmware offset map.
- No published Web Flasher or its manifest.
- No completed hardware test report.

## Upgrade notes

Back up SD-card data before flashing any development build over an existing setup. Do not use firmware binaries or flash offsets intended for a different board.
