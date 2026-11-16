# PocketOS v0.1.2-rc.4 — Experimental pre-release

## Added
- A single version definition in `version.h`, shared by the firmware boot diagnostics and the application registry.

## Fixed
- The boot diagnostics screen and application registry no longer carry stale, separately-maintained version strings.

## Changed
- Candidate version: v0.1.2-rc.4.
- No application behavior, hardware configuration, partition layout, peripheral wiring or Web Flasher flashing behavior is intentionally changed.

## Known issues
- RC3 is the known-good hardware baseline reported by the user. RC4 still requires a fresh build and a short on-device smoke test.
- Full validation of all peripherals and long-term stability remains outstanding.
- The Web Flasher is supported in desktop Chrome/Edge over HTTPS with Web Serial.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- The release workflow builds and validates the firmware bundle, publishes the GitHub pre-release and deploys the matching Web Flasher catalogue.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- Verify the boot diagnostics show v0.1.2-rc.4, then confirm the desktop and previously working functions.

# Русский

## Добавлено
- Единое определение версии в `version.h`, используемое диагностическим экраном загрузки и реестром приложений.

## Исправлено
- Диагностика загрузки и реестр приложений больше не содержат разрозненные строки со старой версией.

## Изменено
- Версия кандидата: v0.1.2-rc.4.
- Поведение приложений, конфигурация платы, разметка Flash, распиновка периферии и процесс прошивки через Web Flasher намеренно не меняются.

## Известные ограничения
- RC3 остаётся проверенной пользователем рабочей базовой версией. RC4 требует новой сборки и короткой проверки на устройстве.
- Полная проверка всей периферии и долговременной стабильности ещё впереди.
- Web Flasher поддерживается в настольных Chrome/Edge по HTTPS с Web Serial.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Релизный workflow собирает и проверяет пакет прошивки, публикует GitHub-пререлиз и развёртывает соответствующий каталог Web Flasher.

## Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не отключай USB и питание, пока флешер не подтвердит завершение.
- Убедись, что на экране диагностики отображается v0.1.2-rc.4, затем проверь рабочий стол и ранее работавшие функции.
