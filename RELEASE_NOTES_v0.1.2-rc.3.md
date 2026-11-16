# PocketOS v0.1.2-rc.3 — pre-release

**Target:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Profile:** 16 MB Flash / 8 MB OPI PSRAM / DIO 80 MHz / app3M_fat9M_16MB  
**Status:** experimental pre-release.

## Added
- No new user-facing firmware features; this candidate focuses on making release publication reproducible from the maintained release branch.

## Fixed
- The release workflow now publishes a versioned pre-release directly from the release-branch build, so publishing no longer depends on a separately-created tag event.
- The workflow creates the Git tag through `gh release create --target` and attaches the complete firmware bundle and bilingual release notes in the same run.
- The Web Flasher catalogue continues to omit releases that do not contain a complete firmware bundle.

## Changed
- Release candidate: v0.1.2-rc.3.
- Firmware source, hardware profile, partition layout and runtime behavior are unchanged from the current release-branch source.

## Known issues
- Full hardware validation of E-Ink refresh, side OLED, SD read/write, keyboard/I²C, RTC, sensors, audio, Wi-Fi, power and long-term stability remains outstanding.
- The Web Flasher is supported only in desktop Chrome/Edge over HTTPS with Web Serial.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- The release-branch workflow compiles and validates firmware, creates the Git tag and GitHub pre-release, uploads firmware binaries, manifest and SHA-256 checksums, and deploys the Web Flasher catalogue.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- After flashing, verify the boot diagnostics screen, desktop, SD access and attached peripherals.

---

# Русский

# PocketOS v0.1.2-rc.3 — предварительный релиз

**Целевая плата:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Профиль:** Flash 16 MB / OPI PSRAM 8 MB / DIO 80 MHz / app3M_fat9M_16MB  
**Статус:** экспериментальный пререлиз.

## Добавлено
- Новых пользовательских функций прошивки нет; этот кандидат в первую очередь делает публикацию релизов воспроизводимой из основной релизной ветки.

## Исправлено
- Workflow теперь публикует версионный пререлиз непосредственно в ходе сборки релизной ветки — отдельное предварительное создание Git-тега больше не требуется.
- Workflow создаёт Git-тег через `gh release create --target` и в том же запуске прикрепляет полный комплект прошивки и двуязычные Release Notes.
- Каталог Web Flasher по-прежнему исключает релизы без полного комплекта файлов прошивки.

## Изменено
- Версия кандидата: v0.1.2-rc.3.
- Исходный код прошивки, профиль платы, разметка разделов и поведение системы не меняются относительно текущего состояния релизной ветки.

## Известные ограничения
- Полная аппаратная проверка обновления E-Ink, бокового OLED, чтения/записи SD, клавиатуры/I²C, RTC, датчиков, аудио, Wi-Fi, питания и долговременной стабильности ещё не выполнена.
- Web Flasher поддерживается только в настольных Chrome/Edge по HTTPS с Web Serial.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Workflow релизной ветки компилирует и проверяет прошивку, создаёт Git-тег и GitHub-пререлиз, загружает бинарники, манифест и SHA-256, а также развёртывает каталог Web Flasher.

## Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не отключай USB и питание, пока флешер не подтвердит завершение.
- После прошивки проверь диагностический экран загрузки, рабочий стол, доступ к SD и подключённую периферию.
