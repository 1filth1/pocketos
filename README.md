<div align="center">

# ◈ PocketOS

### A pocket-sized E-Ink computer powered by ESP32-S3

**An experimental embedded OS for the Waveshare ESP32-S3 E-Ink platform.**

[![Version](https://img.shields.io/badge/version-0.1-20252b?style=for-the-badge)](RELEASE_NOTES_v0.1.md)
[![Platform](https://img.shields.io/badge/platform-ESP32--S3-20252b?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![Display](https://img.shields.io/badge/display-E--Ink%20800%C3%97480-20252b?style=for-the-badge)](#hardware-platform)
[![Status](https://img.shields.io/badge/status-experimental-orange?style=for-the-badge)](#project-status)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](LICENSE)

**[English](#pocketos-v01--english) · [Русский](#pocketos-v01--русский) · [Release notes](RELEASE_NOTES_v0.1.md) · [Project status](docs/PROJECT_STATUS.md) · [Roadmap](docs/ROADMAP.md) · [Security](SECURITY.md) · [Architecture](docs/ARCHITECTURE.md) · [Hardware](docs/HARDWARE.md) · [Testing](docs/TESTING.md) · [Release process](docs/RELEASE_PROCESS.md) · [Firmware archive audit](docs/FIRMWARE_ARCHIVE_AUDIT.md) · [License](LICENSE)**

</div>

---

> [!IMPORTANT]
> PocketOS is an early-stage project. Features and hardware integrations are still being validated. Do not treat this development build as a stable release or flash it without understanding the risks.

## ✦ What is PocketOS?

PocketOS turns an ESP32-S3 E-Ink device into a compact standalone computer. It combines a graphical interface, built-in tools, SD-card storage, connectivity and multimedia features in one firmware project.

The interface targets an **800 × 480 E-Ink display** and is navigated with a connected keyboard.

## ◈ At a glance

| Area | What is included |
|---|---|
| **System UI** | Home screen, application launcher, settings and system screens |
| **Files & notes** | SD-card file manager, text notes and document reading |
| **Productivity** | Calendar, timer, RTC clock and quick settings |
| **Media** | Audio player/tools, voice recorder, image and video viewers |
| **Connectivity** | Wi-Fi, web reader and file exchange |
| **Navigation** | GPS features when a compatible module is connected |
| **Entertainment** | Built-in games and interactive applications |
| **Device status** | Battery, charging, temperature, humidity, Wi-Fi and SD-card status |

Availability depends on the connected hardware and the current state of development. A feature in the source tree is not automatically a verified hardware feature.

## ▣ Hardware platform

| Component | Target |
|---|---|
| Main controller | Waveshare ESP32-S3 |
| Main display | 800 × 480 E-Ink |
| E-Ink controller | GDEM0397T81 |
| Storage | SD card via SD_MMC |
| Input | CardKB2 keyboard over I²C |
| Clock | I²C RTC |
| Environment sensor | SHTC3 |
| Power management | AXP2101 |
| Wireless | ESP32-S3 Wi-Fi |
| Optional expansion | GPS and other compatible peripherals |

> Pin mapping, board revision, Flash/PSRAM configuration, partition layout and library versions must be confirmed against the actual Schematic.io project and hardware revision before producing release firmware.

## ◈ Project architecture

PocketOS v0.1 is implemented as a **single firmware image**. Built-in applications are compiled into the project and run in the same address space.

This means PocketOS is not yet a conventional desktop/mobile OS with isolated processes, and it does not currently execute arbitrary application binaries installed from the SD card.

The SD card is intended for user files, application data, resources and saves.

## ▣ Repository map

| Path | Purpose |
|---|---|
| `main.cpp` | Main firmware entry point and system integration |
| `*_app.h` | Built-in application modules |
| `display2.h`, `epaper_ui.h` | Display and UI support |
| `os_core.h` | Core application registry |
| `RELEASE_NOTES_v0.1.md` | Version 0.1 release notes |
| `docs/PROJECT_STATUS.md` | Current development and validation status |
| `docs/ARCHITECTURE.md` | Preliminary system architecture |
| `docs/HARDWARE.md` | Source-derived hardware reference (not a verified wiring guide) |
| `docs/TESTING.md` | Hardware and software test plan |
| `docs/RELEASE_PROCESS.md` | Release and verification process |
| `docs/FIRMWARE_ARCHIVE_AUDIT.md` | Static audit of the supplied firmware ZIP |
| `LICENSE` | MIT License |

The source tree is evolving; this map highlights the main entry points rather than every header.

## ◈ Project status

**Current stage: experimental v0.1 / pre-release preparation.**

- [x] Bilingual project overview
- [x] Initial v0.1 release notes
- [ ] Reproducible build configuration
- [ ] Dependency and toolchain versions pinned
- [ ] Flash, PSRAM and partition settings verified
- [ ] Automated build workflow
- [ ] Firmware artifacts and flashing instructions
- [ ] Full validation on the target device

See **[Project status & validation checklist](docs/PROJECT_STATUS.md)** for details.

## ▣ Development

The project is currently developed in **Schematic.io** for the Waveshare ESP32-S3 E-Ink target.

Before a reproducible build can be published, the project export must be checked and the exact toolchain, board definition, libraries, memory configuration and partition table must be established. Build instructions will be added after those settings are verified—not guessed.

For contribution workflow and coding expectations, see [CONTRIBUTING.md](CONTRIBUTING.md). The development priorities are listed in the [roadmap](docs/ROADMAP.md). See the preliminary [architecture overview](docs/ARCHITECTURE.md), [source-derived hardware reference](docs/HARDWARE.md), and [test plan](docs/TESTING.md). Security reports are covered by [SECURITY.md](SECURITY.md). The project is distributed under the [MIT License](LICENSE), subject to the notices and licenses of third-party components.

## ◈ Known limitations

- Built-in applications share one address space.
- Arbitrary executable applications cannot currently be installed and launched from the SD card.
- Some features depend on external modules, Wi-Fi or SD-card availability.
- Memory use, refresh behavior, power consumption and long-term stability require testing on real hardware.
- No stable firmware binary or universal flashing configuration is promised by this development branch.

## ▣ Safety & data

- Back up the SD card before testing experimental firmware.
- Do not assume an update preserves every user file until that behavior has been tested.
- Use firmware and flashing parameters only when they match the exact board revision.
- Report reproducible failures with logs, steps and hardware details.

---

# PocketOS v0.1 — Русский

### Карманный E-Ink-компьютер на базе ESP32-S3

**PocketOS** — экспериментальная встроенная операционная система для устройства на базе Waveshare ESP32-S3 с E-Ink-дисплеем 800 × 480. Проект объединяет графический интерфейс, встроенные приложения, хранение пользовательских данных на SD-карте, сетевые и мультимедийные функции.

> [!IMPORTANT]
> PocketOS находится на ранней стадии разработки. Возможности и работа периферии ещё проверяются. Это не стабильная прошивка для повседневного использования. Перед экспериментами сделайте резервную копию SD-карты.

## ✦ Что умеет PocketOS

| Направление | Возможности |
|---|---|
| **Интерфейс** | Главный экран, меню приложений, настройки и системные экраны |
| **Файлы и заметки** | Файловый менеджер SD-карты, текстовые заметки и чтение документов |
| **Органайзер** | Календарь, таймер, часы RTC и быстрые настройки |
| **Мультимедиа** | Аудиоплеер и инструменты, диктофон, просмотр изображений и видео |
| **Связь** | Wi-Fi, веб-просмотр и обмен файлами |
| **Навигация** | GPS-функции при подключении совместимого модуля |
| **Развлечения** | Встроенные игры и интерактивные приложения |
| **Состояние устройства** | Батарея, зарядка, температура, влажность, Wi-Fi и SD-карта |

Наличие модуля в исходниках ещё не означает, что функция полностью проверена на реальном устройстве.

## ▣ Аппаратная платформа

| Компонент | Целевая конфигурация |
|---|---|
| Контроллер | Waveshare ESP32-S3 |
| Основной экран | E-Ink 800 × 480 |
| Контроллер E-Ink | GDEM0397T81 |
| Хранилище | SD-карта через SD_MMC |
| Ввод | Клавиатура CardKB2 по I²C |
| Часы | RTC по I²C |
| Датчик среды | SHTC3 |
| Управление питанием | AXP2101 |
| Беспроводная связь | Wi-Fi ESP32-S3 |
| Дополнительные модули | GPS и другая совместимая периферия |

Распиновку, ревизию платы, параметры Flash/PSRAM, таблицу разделов и версии библиотек необходимо сверить с проектом Schematic.io и конкретным устройством.

## ◈ Как устроена система

PocketOS v0.1 собирается как **единая прошивка**. Встроенные приложения входят в проект и работают в общем адресном пространстве.

Это пока не классическая многозадачная ОС с изоляцией процессов. Произвольные исполняемые приложения с SD-карты сейчас не запускаются.

SD-карта предназначена для пользовательских файлов, данных приложений, ресурсов и сохранений.

## ▣ Состояние проекта

**Текущий этап: экспериментальная v0.1 / подготовка предварительного релиза.**

- [x] Описание проекта на русском и английском
- [x] Первые Release Notes v0.1
- [ ] Воспроизводимая конфигурация сборки
- [ ] Зафиксированные версии инструментов и библиотек
- [ ] Проверенные параметры Flash, PSRAM и разделов
- [ ] Автоматическая сборка
- [ ] Бинарники и инструкция по прошивке
- [ ] Полная проверка на целевом устройстве

Подробности: **[статус проекта и чек-лист проверки](docs/PROJECT_STATUS.md)**.

## ▣ Разработка

Проект разрабатывается в **Schematic.io** под Waveshare ESP32-S3 E-Ink.

Перед публикацией воспроизводимой сборки необходимо проверить экспорт проекта, точную конфигурацию платы, версии библиотек, параметры памяти и таблицу разделов. Инструкции по сборке появятся после подтверждения этих настроек.

Правила работы с проектом описаны в [CONTRIBUTING.md](CONTRIBUTING.md). Приоритеты дальнейшей разработки собраны в [дорожной карте](docs/ROADMAP.md). Предварительное описание архитектуры — в [ARCHITECTURE.md](docs/ARCHITECTURE.md), аппаратные заметки — в [HARDWARE.md](docs/HARDWARE.md), план проверок — в [TESTING.md](docs/TESTING.md). Порядок сообщения об уязвимостях — в [SECURITY.md](SECURITY.md). Проект распространяется по [лицензии MIT](LICENSE) с учётом лицензий и уведомлений сторонних компонентов.

## ◈ Ограничения и безопасность

- Встроенные приложения используют общее адресное пространство.
- Установка и запуск произвольных исполняемых приложений с SD-карты пока не реализованы.
- Часть функций зависит от внешних модулей, Wi-Fi или наличия SD-карты.
- Память, обновление E-Ink, энергопотребление и стабильность требуют испытаний на реальном устройстве.
- В этой ветке пока не гарантируются стабильный бинарник и универсальные параметры прошивки.

Перед тестированием сделайте резервную копию SD-карты. Используйте только те параметры прошивки, которые соответствуют вашей аппаратной ревизии.

---

<div align="center">

**PocketOS · Built for the E-Ink form factor**

*Small device. Big possibilities.*

</div>
