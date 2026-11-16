# PocketOS v0.1.2-rc.12 — Experimental pre-release

## Added
- App Manager is available as a dedicated icon on the desktop launcher, separate from the general file browser.
- The desktop launcher uses a four-column grid and includes a dedicated monochrome App Manager icon.
- App Manager returns to the desktop when opened from its desktop icon. The existing Documents / SD App Library entry remains available and keeps its previous navigation behavior.
- Runtime logs use the clearer names `APP_MANAGER` and `APP_MANAGER_DETAILS` for SD catalogue views.

## Fixed
- Corrected launcher navigation for the partially populated final row so empty cells cannot be selected.

## Changed
- Four-way launcher navigation uses a four-column layout.
- Firmware version and branch-build release fallback are synchronized to v0.1.2-rc.12.

## Known issues
- Built-in applications still share one address space. This release does not add process isolation, automatic memory reclamation or full per-app resource ownership.
- App Manager remains read-only. SD packages contain data only; arbitrary code from the SD card is never loaded or executed.
- Memory diagnostics report view-boundary snapshots. Hardware log analysis is still required to establish leak/fragmentation trends.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
- Firmware and SD-card content remain version-matched through the release workflow.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Select v0.1.2-rc.12 in the Web Flasher.
- Confirm all 13 desktop icons are visible and navigate the 4×4 grid in all directions.
- Open APP MGR, browse the package catalogue and resources, then press ESC from the catalogue root; it should return to the desktop.
- Open SD APP LIBRARY through Documents and confirm its existing return behavior is unchanged.
- Capture Serial output during repeated app switching. Compare `[MEM]` free/min/largest values and `[APP]` transitions over repeated cycles.

# Русский

## Добавлено
- App Manager вынесен на рабочий стол отдельным значком, независимо от обычного файлового менеджера.
- Сетка рабочего стола расширена до четырёх колонок; добавлена отдельная монохромная иконка App Manager.
- При запуске с рабочего стола App Manager возвращается на рабочий стол. Существующий пункт SD App Library в разделе Documents сохранён и продолжает возвращаться в Documents.
- Для экранов каталога SD в Serial-логах используются понятные имена `APP_MANAGER` и `APP_MANAGER_DETAILS`.

## Исправлено
- Навигация по сетке корректно обрабатывает незаполненные ячейки последней строки.

## Изменено
- Версия прошивки и резервная версия для сборок ветки синхронизированы с v0.1.2-rc.12.

## Известные ограничения
- Встроенные приложения по-прежнему работают в общем адресном пространстве. В этом релизе нет изоляции процессов, автоматического освобождения памяти или полного владения ресурсами каждым приложением.
- App Manager остаётся инструментом только для чтения. Пакеты SD содержат только данные; произвольный код с SD не загружается и не запускается.
- Диагностика памяти показывает снимки на границах экранов. Для выявления утечек и фрагментации ещё требуется анализ логов с устройства.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 МГц; app3M_fat9M_16MB.
- Прошивка и содержимое SD-карты остаются привязаны к одной версии релиза.

## Обновление и проверка
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во Flash.
- Выберите v0.1.2-rc.12 во Web Flasher.
- Убедитесь, что видны все 13 значков, и проверьте навигацию по сетке во всех направлениях.
- Откройте APP MGR, просмотрите каталог пакетов и ресурсы, затем нажмите ESC на корневом экране каталога — должен открыться рабочий стол.
- Откройте SD APP LIBRARY через Documents и убедитесь, что прежний маршрут возврата не изменился.
- Запишите Serial-лог при многократном переключении приложений. Сравните значения `[MEM]` free/min/largest и переходы `[APP]`.
