# PocketOS Architecture (working document)

> Status: preliminary source-based overview. This document describes the repository snapshot; it is not a claim of a completed architecture review or runtime validation.

## 1. System overview

PocketOS is firmware for an ESP32-S3-based E-Ink device. The current source is organized around a central Arduino-style firmware entry point and a set of C/C++ headers and source files implementing UI, device services and built-in applications.

The current codebase is a monolithic firmware image. Built-in applications share the same address space; the application registry is a launcher/menu registry, not an operating-system process manager or a dynamic executable loader.

## 2. Main subsystems visible in source

| Subsystem | Current source evidence | Notes |
|---|---|---|
| Firmware entry and UI routing | `main.cpp` | Central state, view routing and hardware initialization |
| E-Ink rendering | `epaper_ui.h`, GxEPD2 usage | Display abstraction and rendering helpers |
| Launcher registry | `os_core.h` | Static registry of built-in applications |
| Navigation | `menu_navigation.h` | Menu/input navigation logic |
| Storage and file UI | `file_manager.h`, `sd_settings.h`, `file_viewers.h` | SD-card-backed files and settings |
| Network and exchange | `main.cpp`, `file_exchange.h`, `web_reader.h` | Wi-Fi, HTTP(S), web and file exchange code |
| Audio | `audio_app.h`, `es8311.cpp/.h`, `groove_box.h`, `sequencer_app.h` | Audio codec and audio-related features |
| Built-in apps | `*_app.h`, reader/viewer and game headers | Compiled into the firmware |

This grouping is a documentation aid. Exact ownership boundaries and call relationships still need a dedicated code review.

## 3. Application model

- Applications are compiled into the firmware.
- The static registry in `os_core.h` provides identifiers, labels and action IDs for launcher entries. `app_runtime.h` observes transitions between UI views and logs entry/exit duration to Serial. This is diagnostic telemetry, not an application scheduler or process manager.
- Application views and state are routed by the central firmware code.
- Applications are not isolated processes.
- Installing arbitrary executable applications from the SD card is not implemented by the registry.

## 4. Hardware and services

The source currently references the E-Ink display driver, SPI, I2C, SD_MMC, Wi-Fi, HTTP clients, RTC, keyboard and environmental/power-related devices. The exact supported board revision, peripheral population and electrical configuration must be confirmed against the actual hardware and schematic before treating source constants as a verified pinout.

See [HARDWARE.md](HARDWARE.md) for the current source-derived hardware notes.

## 5. Memory and reliability considerations

The E-Ink UI uses a retained framebuffer abstraction. Allocation behavior, PSRAM availability, heap fallback, peak memory use and fragmentation should be measured on the target board.

The SD-card code should be tested for interrupted writes, missing media, filesystem errors and recovery. Network and external-data paths should be reviewed for timeouts, input validation and failure handling.

## 6. Build and validation status

A reproducible build environment, exact dependency versions, partition layout and verified flashing procedure have not yet been documented in this repository snapshot.

Do not infer a successful build or hardware test from the presence of source files. Track evidence and test results in [PROJECT_STATUS.md](PROJECT_STATUS.md).

## Русский

> Статус: предварительный обзор по исходникам. Документ описывает текущую структуру репозитория и не означает, что архитектура полностью проверена или испытана на устройстве.

### 1. Общая схема

PocketOS — прошивка для устройства на базе ESP32-S3 с E-Ink дисплеем. Текущая версия организована вокруг центрального файла прошивки и набора модулей C/C++ для интерфейса, системных служб и встроенных приложений.

Сейчас это монолитная прошивка. Встроенные приложения работают в общем адресном пространстве. Реестр приложений — это статический список для запуска встроенных разделов интерфейса, а не диспетчер изолированных процессов и не динамический загрузчик исполняемых файлов.

### 2. Подсистемы, видимые в исходниках

| Подсистема | Файлы и признаки | Примечание |
|---|---|---|
| Точка входа и маршрутизация UI | `main.cpp` | Центральное состояние, переходы между экранами и инициализация |
| E-Ink | `epaper_ui.h`, GxEPD2 | Абстракция дисплея и функции отрисовки |
| Реестр приложений | `os_core.h` | Статический список встроенных приложений |
| Навигация | `menu_navigation.h` | Логика меню и ввода |
| Файлы и настройки | `file_manager.h`, `sd_settings.h`, `file_viewers.h` | Работа с SD-картой |
| Сеть и обмен | `main.cpp`, `file_exchange.h`, `web_reader.h` | Wi-Fi, HTTP(S), веб-функции и обмен файлами |
| Аудио | `audio_app.h`, `es8311.cpp/.h`, `groove_box.h`, `sequencer_app.h` | Аудиокодек и звуковые функции |
| Встроенные приложения | `*_app.h`, модули чтения, просмотра и игр | Компилируются в прошивку |

Это группировка для документации. Точные границы модулей и связи между ними ещё требуют отдельного анализа.

### 3. Модель приложений

- Приложения компилируются в прошивку.
- Статический реестр в `os_core.h` содержит идентификаторы, подписи и action ID.
- Переходы между экранами и состояние приложений обрабатываются центральным кодом прошивки.
- Изоляции приложений как отдельных процессов нет.
- Произвольная установка и запуск исполняемых файлов с SD-карты через этот реестр не реализованы.

### 4. Аппаратная часть и службы

В исходниках используются драйвер E-Ink, SPI, I2C, SD_MMC, Wi-Fi, HTTP-клиенты, RTC, клавиатура и датчики/узлы питания. Точную ревизию платы, комплектацию периферии и электрическую конфигурацию необходимо сверить с реальной платой и схемой. Константы в исходниках сами по себе не являются подтверждённой распиновкой.

Текущие заметки собраны в [HARDWARE.md](HARDWARE.md).

### 5. Память и надёжность

UI E-Ink использует буфер кадра. Нужно измерить поведение выделения памяти, доступность PSRAM, резервный сценарий во внутренней памяти, пиковое потребление и фрагментацию.

Работу с SD-картой следует проверить при прерывании записи, отсутствии карты и ошибках файловой системы. Сетевые сценарии и внешние данные требуют проверки тайм-аутов, валидации входных данных и обработки ошибок.

### 6. Сборка и проверка

В текущем состоянии репозитория ещё не зафиксированы воспроизводимая среда сборки, точные версии зависимостей, таблица разделов и проверенная инструкция прошивки.

Наличие исходников не доказывает успешную сборку или испытания на устройстве. Результаты и подтверждения необходимо фиксировать в [PROJECT_STATUS.md](PROJECT_STATUS.md).
