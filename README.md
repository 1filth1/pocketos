# PocketOS v0.1

**PocketOS** — экспериментальная встроенная операционная система для устройства на базе **Waveshare ESP32-S3 с E-Ink-дисплеем 800×480**. Проект объединяет интерфейс устройства, встроенные приложения, работу с файлами на SD-карте и системные функции в одной прошивке.

> **Статус:** ранняя версия v0.1. Проект находится в активной разработке. Наличие функции в исходном коде не означает, что она уже прошла полный цикл испытаний на реальном устройстве.

## Что такое PocketOS

PocketOS превращает ESP32-S3 в компактное автономное устройство с графическим интерфейсом и набором повседневных инструментов. Интерфейс рассчитан на E-Ink: информация выводится на экран 800×480, а навигация выполняется с помощью подключённой клавиатуры.

Система построена как единая прошивка: встроенные приложения работают внутри одного адресного пространства и запускаются через общий интерфейс. Это не полноценная многозадачная ОС с изоляцией процессов и не загрузчик произвольных исполняемых файлов с SD-карты.

## Основные возможности

- **Графический интерфейс:** главный экран, меню приложений, настройки и системные экраны.
- **Файловый менеджер:** просмотр файлов и папок на SD-карте, работа с пользовательскими данными.
- **Заметки и чтение:** создание и просмотр текстовых заметок, чтение книг и веб-страниц.
- **Мультимедиа:** музыкальный плеер, диктофон, просмотр изображений и видео, аудио-инструменты.
- **Организация времени:** календарь, таймер и часы с использованием RTC.
- **Сетевые функции:** подключение к Wi-Fi, веб-просмотр и обмен файлами.
- **Навигация:** GPS-функции при наличии и подключении соответствующего модуля.
- **Развлечения:** встроенные игры и интерактивные приложения.
- **Системная информация:** состояние батареи и зарядки, температура и влажность, часы, Wi-Fi и состояние SD-карты.
- **Настройки интерфейса:** в том числе режим увеличенного текста.

Набор приложений и доступность отдельных функций зависят от подключённой периферии и текущего состояния разработки.

## Аппаратная платформа

Основная целевая платформа проекта:

- Waveshare ESP32-S3 E-Ink;
- E-Ink-дисплей 800×480 на контроллере GDEM0397T81;
- SD-карта через интерфейс SD_MMC;
- клавиатурный модуль CardKB2 по I²C;
- RTC по I²C;
- датчик температуры и влажности SHTC3;
- система управления питанием и мониторинга батареи AXP2101;
- Wi-Fi, встроенный в ESP32-S3;
- дополнительные модули, например GPS, при соответствующем подключении.

Конкретная конфигурация платы, распиновка и список библиотек должны соответствовать используемой аппаратной ревизии и настройкам проекта в Schematic.io.

## Принципы проекта

1. **Автономность.** Основные функции устройства должны быть доступны без постоянного подключения к компьютеру или интернету.
2. **Расширяемость.** Новые возможности добавляются как приложения и системные модули.
3. **Работа с SD-картой.** Карта используется для пользовательских файлов, данных и ресурсов приложений.
4. **Бережное использование ресурсов.** Учитываются ограничения памяти ESP32-S3 и особенности E-Ink-дисплея.
5. **Безопасность данных.** Обновления и новые функции не должны без необходимости форматировать карту или повреждать пользовательские файлы.

## Важные ограничения v0.1

- Встроенные приложения являются частью прошивки и используют общее адресное пространство.
- Установка произвольных исполняемых приложений непосредственно с SD-карты пока не реализована.
- Работа некоторых приложений зависит от наличия SD-карты, Wi-Fi и внешних модулей.
- Производительность, энергопотребление и стабильность должны проверяться на реальном устройстве.
- Проект пока не следует считать стабильным релизом для повседневного использования.

## Разработка

Исходный код PocketOS развивается в Arduino/ESP32-экосистеме. Проект ведётся с использованием **Schematic.io** и ориентирован на указанную выше плату Waveshare ESP32-S3.

Перед выпуском стабильной версии необходимо зафиксировать конфигурацию сборки, версии библиотек, параметры Flash/PSRAM и разметку разделов, а затем проверить прошивку на целевом устройстве.

---

# PocketOS v0.1 — English

**PocketOS** is an experimental embedded operating system for a device based on the **Waveshare ESP32-S3 with an 800×480 E-Ink display**. It brings together the device interface, built-in applications, SD-card file handling and system features in a single firmware image.

> **Status:** early v0.1 development. The project is actively evolving. A feature present in the source code should not be considered fully validated until it has been tested on the actual hardware.

## What is PocketOS?

PocketOS turns the ESP32-S3 into a compact standalone device with a graphical interface and a collection of everyday tools. The interface is designed for an 800×480 E-Ink screen, with navigation provided by a connected keyboard.

The system is implemented as a single firmware image. Built-in applications run within the same address space and are launched through a shared interface. PocketOS is not yet a fully multitasking operating system with process isolation, nor does it load arbitrary executable applications from the SD card.

## Key features

- **Graphical interface:** home screen, application launcher, settings and system screens.
- **File manager:** browse files and folders on the SD card and manage user data.
- **Notes and reading:** create and view text notes, read books and web pages.
- **Multimedia:** music player, voice recorder, image and video viewers, and audio tools.
- **Time utilities:** calendar, timer and RTC-based clock.
- **Networking:** Wi-Fi connectivity, web browsing and file exchange.
- **Navigation:** GPS-related features when a compatible module is connected.
- **Entertainment:** built-in games and interactive applications.
- **System information:** battery and charging status, temperature and humidity, clock, Wi-Fi and SD-card status.
- **Interface settings:** including an enlarged-text mode.

The available applications and individual features depend on connected peripherals and the current development state.

## Hardware platform

The primary target platform is:

- Waveshare ESP32-S3 E-Ink;
- 800×480 E-Ink display with the GDEM0397T81 controller;
- SD card connected through SD_MMC;
- CardKB2 keyboard module over I²C;
- I²C real-time clock (RTC);
- SHTC3 temperature and humidity sensor;
- AXP2101 power-management and battery-monitoring system;
- ESP32-S3 built-in Wi-Fi;
- optional peripherals, such as a GPS module, when properly connected.

The board configuration, pin mapping and library versions must match the actual hardware revision and the project settings in Schematic.io.

## Project principles

1. **Standalone operation.** Core device functions should be available without a permanent computer or internet connection.
2. **Extensibility.** New capabilities are added as applications and system modules.
3. **SD-card support.** The card stores user files, application data and resources.
4. **Careful resource management.** The firmware accounts for ESP32-S3 memory constraints and E-Ink display characteristics.
5. **Data integrity.** Updates and new features should not unnecessarily format the card or damage user files.

## Known limitations in v0.1

- Built-in applications are part of the firmware and share the same address space.
- Installing and running arbitrary executable applications directly from the SD card is not implemented yet.
- Some applications require an SD card, Wi-Fi or additional hardware modules.
- Performance, power consumption and reliability still need validation on the actual device.
- This development version should not yet be considered a stable release for everyday use.

## Development

PocketOS is developed within the Arduino/ESP32 ecosystem. The project uses **Schematic.io** and targets the Waveshare ESP32-S3 board described above.

Before a stable release, the build configuration, library versions, Flash/PSRAM settings and partition layout must be pinned down, followed by firmware validation on the target hardware.
