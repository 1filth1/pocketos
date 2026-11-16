# PocketOS v0.1.2-rc.2 — pre-release

**Target:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Profile:** 16 MB Flash / 8 MB OPI PSRAM / DIO 80 MHz / app3M_fat9M_16MB  
**Status:** experimental pre-release; this candidate primarily validates corrected release packaging and publication.

## Added
- No new user-facing firmware features are included. The firmware source and runtime behavior are unchanged from v0.1.2-rc.1.

## Fixed
- The release workflow can now complete an existing empty GitHub Release by uploading the firmware bundle and attaching the version-specific release notes.
- The Web Flasher catalogue skips incomplete legacy releases that do not contain a complete firmware bundle.

## Changed
- Release publication is tag-driven and must run from a commit containing the repaired workflow.
- The hardware profile, partition layout, peripheral wiring, applications and firmware behavior are unchanged from rc.1.

## Known issues
- The boot diagnostics screen reports initialization state; it is not a full functional test of each peripheral.
- Full hardware validation of E-Ink refresh, side OLED, SD read/write, keyboard/I²C, RTC, sensors, audio, Wi-Fi, power and long-term stability remains outstanding.
- The Web Flasher is supported only in desktop Chrome/Edge over HTTPS with Web Serial.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- The tag-triggered workflow builds the firmware, manifest and SHA-256 checksums, deploys the matching Web Flasher and publishes the GitHub pre-release.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- This candidate has no intended firmware behavior changes from rc.1. After flashing, verify the diagnostic screen and normal desktop operation.

---

# Русский

# PocketOS v0.1.2-rc.2 — предварительный релиз

**Целевая плата:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Профиль:** Flash 16 MB / OPI PSRAM 8 MB / DIO 80 MHz / app3M_fat9M_16MB  
**Статус:** экспериментальный пререлиз; в первую очередь проверяет исправленную сборку и публикацию релиза.

## Добавлено
- Новых пользовательских функций прошивки нет. Исходный код и поведение прошивки не отличаются от v0.1.2-rc.1.

## Исправлено
- Workflow теперь может завершить публикацию уже существующего пустого GitHub Release: загрузить файлы прошивки и добавить версионные Release Notes.
- Каталог Web Flasher пропускает неполные старые релизы, в которых нет полного комплекта файлов прошивки.

## Изменено
- Публикация релиза запускается по Git-тегу и должна выполняться из коммита, содержащего исправленный workflow.
- Профиль платы, разметка разделов, распиновка периферии, приложения и поведение прошивки не меняются относительно rc.1.

## Известные ограничения
- Диагностический экран показывает результат инициализации, но не является полной функциональной проверкой каждого устройства.
- Полная проверка E-Ink, бокового OLED, чтения/записи SD, клавиатуры/I²C, RTC, датчиков, аудио, Wi-Fi, питания и долговременной стабильности ещё впереди.
- Web Flasher поддерживается только в настольных Chrome/Edge по HTTPS с Web Serial.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Запуск по тегу собирает прошивку, манифест и SHA-256, развёртывает соответствующий Web Flasher и публикует GitHub-пререлиз.

## Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не отключай USB и питание, пока флешер не подтвердит завершение.
- В этом кандидате не предполагается изменений поведения прошивки относительно rc.1. После прошивки проверь диагностический экран и запуск рабочего стола.
