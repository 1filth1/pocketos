# PocketOS v0.1.2-rc.1 — pre-release

**Target:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Profile:** 16 MB Flash / 8 MB OPI PSRAM / DIO 80 MHz / app3M_fat9M_16MB  
**Status:** experimental firmware candidate; user hardware validation required.

## Added
- A visible E-Ink boot diagnostics screen is shown after peripheral initialization and before the home screen.
- The startup screen reports the main display, SD card, CardKB2 keyboard, RTC, audio initialization, PSRAM, Flash capacity, free heap and side OLED status.
- Optional peripherals are reported as unavailable rather than treated as fatal boot errors.
- The firmware version in the application registry is updated to 0.1.2-rc.1.

## Fixed
- No unrelated behavior or peripheral wiring changes are included in this candidate.

## Changed
- Startup now pauses briefly on the diagnostics screen before opening the desktop. This adds a short delay to boot by design.
- The existing home screen, applications, storage layout, Wi-Fi behavior and Web Flasher write procedure are unchanged.

## Known issues
- The diagnostics screen reports initialization state; it is not a full functional test of each peripheral.
- Full hardware validation of E-Ink refresh, side OLED, SD read/write, keyboard/I²C, RTC, sensors, audio, Wi-Fi, power and long-term stability remains outstanding.
- The Web Flasher is supported only in desktop Chrome/Edge over HTTPS with Web Serial.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- The release workflow builds the firmware, manifest and SHA-256 checksums from the tagged commit and deploys the matching Web Flasher.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Keep USB and power connected until the flasher confirms completion.
- After flashing, check that the new diagnostics screen reports expected hardware states, then verify normal desktop operation. Report any unexpected NOT FOUND state.

---

# PocketOS v0.1.2-rc.1 — предварительный релиз

**Цель:** Waveshare ESP32-S3-ePaper-3.97 (N16R8)  
**Профиль:** Flash 16 MB / OPI PSRAM 8 MB / DIO 80 MHz / app3M_fat9M_16MB  
**Статус:** экспериментальная прошивка-кандидат; требуется проверка пользователем на устройстве.

## Добавлено
- После инициализации периферии и перед главным экраном появляется диагностический экран загрузки на E-Ink.
- На экране отображаются состояния основного дисплея, SD-карты, клавиатуры CardKB2, RTC, аудиосистемы, PSRAM, объём Flash, свободная heap-память и боковой OLED.
- Отсутствие необязательной периферии отображается как NOT FOUND и само по себе не должно останавливать загрузку.
- Версия прошивки в реестре приложений обновлена до 0.1.2-rc.1.

## Исправлено
- Посторонние функции и распиновка периферии в этом кандидате не менялись.

## Изменено
- Перед переходом на рабочий стол система ненадолго задерживается на диагностическом экране. Это намеренная небольшая прибавка ко времени загрузки.
- Главный экран, приложения, разметка хранилища, работа Wi-Fi и процедура записи Web Flasher не меняются.

## Известные ограничения
- Диагностический экран показывает результат инициализации, но не является полной функциональной проверкой каждого устройства.
- Полная аппаратная проверка обновления E-Ink, бокового OLED, чтения/записи SD, клавиатуры/I²C, RTC, датчиков, аудио, Wi-Fi, питания и длительной стабильности ещё впереди.
- Web Flasher поддерживается только в настольных Chrome/Edge по HTTPS с Web Serial.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Релизный workflow собирает прошивку, манифест и SHA-256 из помеченного коммита и публикует соответствующий ему Web Flasher.

## Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- Не отключай USB и питание, пока флешер не подтвердит завершение.
- После прошивки проверь диагностический экран: состояния должны соответствовать подключённой аппаратуре. Затем убедись, что открылся обычный рабочий стол. Сообщи, если какой-либо ожидаемый модуль отображается как NOT FOUND.
