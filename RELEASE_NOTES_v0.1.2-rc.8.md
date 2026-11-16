# PocketOS v0.1.2-rc.8

**Experimental pre-release / Экспериментальный предварительный выпуск**

## Added
- Serial memory snapshots whenever the active application view changes.
- Separate internal heap and PSRAM free-space metrics, including the largest contiguous free block and the internal heap low-water mark.

## Fixed
- No user-facing bug fix is claimed in this diagnostic-only release.

## Changed
- Memory logging is observational and does not allocate memory.
- Display, application behavior, SD storage, network settings and peripheral configuration are unchanged.
- RC7 HTTPS certificate validation has been confirmed working on the user's hardware.

## Known issues
- Memory snapshots occur at application-view transitions and do not identify the exact source of an allocation.
- A longer stability run is still needed to establish memory trends under real use.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Navigate between several applications and inspect Serial output for [MEM] enter and [MEM] leave snapshots.
- Repeat the same navigation sequence to compare free heap, largest blocks and PSRAM values.

## Русский

### Добавлено
- Снимки состояния памяти в Serial при каждом переходе между экранами приложений.
- Раздельные показатели внутренней heap-памяти и PSRAM: свободный объём, минимальный остаток внутренней heap-памяти и крупнейший непрерывный свободный блок.

### Исправлено
- В этом диагностическом выпуске исправления пользовательских ошибок не заявляются.

### Изменено
- Диагностика только считывает показатели и не выделяет дополнительную память.
- Экран, поведение приложений, SD-хранилище, сетевые настройки и конфигурация периферии не меняются.
- Работу проверки HTTPS-сертификатов из RC7 пользователь подтвердил на устройстве.

### Известные ограничения
- Снимки снимаются только при переходах между экранами и не показывают конкретное место возможной утечки.
- Для оценки динамики памяти нужен более длительный тест в обычном режиме использования.

### Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во Flash.
- Переключитесь между несколькими приложениями и проверьте в Serial строки [MEM] enter и [MEM] leave.
- Повторите тот же маршрут и сравните свободную heap-память, крупнейшие блоки и PSRAM.

