# PocketOS v0.1.2-rc.5 — Experimental pre-release

## Added
- Lightweight application-view lifecycle telemetry: serial logs record each view entry, exit, elapsed time and transition count.
- Human-readable names for the built-in UI views in lifecycle diagnostics.

## Changed
- The runtime monitor observes the existing single-address-space UI state only. It does not introduce process isolation, unload applications or change app launch behavior.
- Release workflow fallback tag and package version are synchronized to v0.1.2-rc.5.

## Validation
- RC4 is the latest user-confirmed working hardware baseline.
- RC5 requires a clean CI build and an on-device smoke test. Serial output should include `[APP] enter` and `[APP] leave` messages when navigating between screens.

## Known issues
- Application state remains globally shared; applications are not isolated processes.
- Lifecycle hooks for app-specific initialization and cleanup, memory accounting and resource ownership are not yet implemented.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- After flashing, verify the boot diagnostics show v0.1.2-rc.5 and navigate between several applications.
- If a USB serial monitor is available, confirm readable `[APP]` entry/exit messages. Existing app behavior should remain unchanged.

# Русский

## Добавлено
- Лёгкий монитор жизненного цикла экранов приложений: в Serial выводятся вход, выход, время работы и число переходов.
- Читаемые названия встроенных экранов в диагностических сообщениях.

## Изменено
- Монитор наблюдает за существующим состоянием интерфейса в общей памяти. Он не добавляет изоляцию процессов, выгрузку приложений или изменения их запуска.
- Резервная версия релизного workflow и версия пакета синхронизированы с v0.1.2-rc.5.

## Проверка
- RC4 — последняя версия, работоспособность которой пользователь подтвердил на устройстве.
- Для RC5 необходимы чистая CI-сборка и короткая проверка на устройстве. При переходах между экранами Serial должен показывать сообщения `[APP] enter` и `[APP] leave`.

## Известные ограничения
- Состояние приложений по-прежнему общее; изоляции процессов нет.
- Специализированные хуки запуска и остановки приложений, учёт памяти и владение ресурсами пока не реализованы.

## Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 MB, OPI PSRAM 8 MB.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

## Обновление
- Перед прошивкой сделай резервную копию SD-карты и важных данных во внутренней Flash.
- После прошивки проверь, что на диагностическом экране указана версия v0.1.2-rc.5, и попереключай несколько приложений.
- Если доступен Serial Monitor, проверь сообщения входа и выхода `[APP]`. Остальное поведение приложений должно остаться прежним.
