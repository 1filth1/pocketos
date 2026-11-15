# PocketOS v0.1.2-rc.1 — pre-release

**Target:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Profile:** 16 MB Flash / 8 MB OPI PSRAM / DIO 80 MHz / app3M_fat9M_16MB  
**Status:** software candidate; full physical validation remains outstanding.

## Added
- Clearer bilingual repository documentation for the browser flasher and 3D-printable enclosure models.

## Fixed
- Corrected Web Flasher version-tag validation so valid versioned prereleases are accepted while development builds remain rejected.
- Fixed README rendering on GitHub Pages by removing the unsupported centering wrapper and replacing literal escaped newline sequences with real Markdown line breaks.

## Changed
- No firmware feature changes are intended in this candidate; the firmware is rebuilt from the same source tree for a traceable, versioned package.

## Known issues
- Full hardware validation of E-Ink, side OLED, SD_MMC, keyboard/I²C, RTC, sensors, audio, Wi-Fi, power and long-term stability remains outstanding.
- The Web Flasher is supported only in desktop Chrome/Edge over HTTPS with Web Serial.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- The release workflow builds the firmware, manifest and SHA-256 checksums from the tagged commit and deploys the matching Web Flasher.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- After flashing, perform the controlled hardware checklist before relying on the device.

---

# PocketOS v0.1.2-rc.1 — предварительный релиз

**Цель:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Профиль:** Flash 16 MB / OPI PSRAM 8 MB / DIO 80 MHz / app3M_fat9M_16MB  
**Статус:** программный кандидат; полная аппаратная проверка ещё впереди.

## Добавлено
- Уточнена двуязычная документация репозитория: описание веб-флешера и моделей корпуса для 3D-печати.

## Исправлено
- Исправлена проверка версии в Web Flasher: корректные версионные пререлизы принимаются, development-сборки по-прежнему отклоняются.
- Исправлено отображение README на GitHub Pages: удалена неподдерживаемая HTML-обёртка центрирования, буквальные последовательности переноса строк заменены настоящими переносами Markdown.

## Изменено
- Изменения функциональности прошивки в этом кандидате не планируются; прошивка пересобирается из того же дерева исходников для получения воспроизводимого пакета с версионной меткой.

## Известные ограничения
- Полная аппаратная проверка E-Ink, бокового OLED, SD_MMC, клавиатуры/I²C, RTC, датчиков, аудио, Wi-Fi, питания и долговременной стабильности ещё не выполнена.
- Web Flasher поддерживается только в настольных Chrome/Edge по HTTPS с Web Serial.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Релизный workflow собирает прошивку, манифест и SHA-256 из помеченного коммита и публикует соответствующий ему Web Flasher.

## Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не отключай USB и питание, пока флешер не подтвердит завершение.
- После прошивки выполни контролируемую проверку аппаратуры, прежде чем полагаться на устройство.
