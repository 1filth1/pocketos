# PocketOS v0.1.2-rc.9

**Experimental pre-release / Экспериментальный предварительный выпуск**

## Added
- Extended application-transition memory snapshots with total capacity and minimum-free low-water marks for internal heap and PSRAM.
- Each memory pool now reports total, currently free, minimum free and largest contiguous free block.

## Fixed
- Branch-triggered CI builds now skip release upload when the fallback prerelease already exists with assets, leaving the published release untouched.
- Tag-triggered builds still refuse to overwrite a release that already contains assets.

## Changed
- This release extends RC8's observational diagnostics only. App lifecycle, allocation policy, display, storage and network behavior are unchanged.

## Known issues
- Transition snapshots identify memory trends, not the exact allocation responsible for a leak.
- No automatic memory reclamation or application isolation is introduced.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Navigate between several apps repeatedly and capture Serial lines beginning with `[MEM]`.
- Compare `internal_min` and `psram_min` over time; compare free and largest-block values for fragmentation trends.

## Русский

### Добавлено
- Расширены снимки памяти при переходах между приложениями: общий объём и минимальный остаток внутренней heap-памяти и PSRAM.
- Для каждого пула памяти показываются общий объём, текущая свободная память, минимальный остаток и крупнейший непрерывный свободный блок.

### Исправлено
- Сборки из ветки больше не завершаются ошибкой, если резервный пререлиз уже опубликован и содержит файлы прошивки; существующий релиз остаётся нетронутым.
- Сборка по Git-тегу по-прежнему запрещает перезапись релиза, в котором уже есть файлы.

### Изменено
- RC9 расширяет только диагностические измерения RC8; поведение приложений, дисплей, хранилище и сеть не меняются.

### Известные ограничения
- Снимки при переходах показывают динамику памяти, но не определяют конкретное место утечки.
- Автоматическое освобождение памяти и изоляция приложений не добавляются.

### Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во Flash.
- Несколько раз переключитесь между приложениями и сохраните строки Serial, начинающиеся с `[MEM]`.
- Сравните `internal_min` и `psram_min` в динамике; показатели свободной памяти и крупнейшего блока помогут заметить фрагментацию.
