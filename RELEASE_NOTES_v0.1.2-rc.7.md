# PocketOS v0.1.2-rc.7

**Experimental pre-release / Экспериментальный предварительный выпуск**

## Added
- HTTPS certificate validation using the built-in Arduino-ESP32 CA certificate bundle.
- UTC time synchronization before TLS validation when the device clock is not yet valid.
- Clear fail-closed behavior when time synchronization is unavailable.

## Fixed
- Removed insecure TLS mode from web-page and JPEG downloads.
- HTTPS-to-HTTP redirects and redirect URLs containing embedded credentials are rejected.

## Changed
- Plain HTTP remains available, but it is not encrypted or authenticated.
- Wi-Fi credential storage, SD package catalogue and app runtime behavior are unchanged.

## Known issues
- Initial TLS use may require outbound NTP access to `pool.ntp.org` or `time.cloudflare.com`.
- The built-in CA bundle depends on the pinned Arduino-ESP32 core.
- Hardware verification of valid HTTPS, JPEG loading and rejection of untrusted certificates is still required.

## Build & package
- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

## Upgrade
- Back up the SD card and important internal Flash data before flashing.
- Connect to Wi-Fi with internet access, open a valid HTTPS article and JPEG image, then confirm an invalid/untrusted certificate is rejected.
- If HTTPS reports a time-sync failure, check NTP access.

## Русский

### Добавлено
- Проверка HTTPS-сертификатов по встроенному набору доверенных центров сертификации Arduino-ESP32.
- Синхронизация системного времени по UTC перед TLS-подключением, если часы ещё не установлены.
- Безопасный отказ от HTTPS, если корректное время получить не удалось.

### Исправлено
- Удалён небезопасный режим TLS без проверки сертификатов для загрузки страниц и JPEG-изображений.
- Блокируются перенаправления с HTTPS на обычный HTTP и URL со встроенными логином/паролем.

### Изменено
- Обычный HTTP остаётся доступен, но не обеспечивает шифрование и аутентификацию.
- Хранение Wi-Fi-паролей, каталог приложений SD и работа приложений не менялись.

### Известные ограничения
- При первом HTTPS-подключении может понадобиться доступ к NTP-серверам `pool.ntp.org` или `time.cloudflare.com`.
- Проверка зависит от встроенного набора сертификатов закреплённой версии Arduino-ESP32.
- Нужна проверка на устройстве: HTTPS-страница, JPEG по HTTPS и блокировка недоверенного сертификата.

### Сборка и пакет
- Целевая плата: Waveshare ESP32-S3-ePaper-3.97 (N16R8), Flash 16 МБ, OPI PSRAM 8 МБ.
- Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.

### Обновление
- Перед прошивкой сделайте резервную копию SD-карты и важных данных во Flash.
- Подключите Wi-Fi с доступом в интернет, проверьте открытие HTTPS-страницы и JPEG, затем убедитесь, что недоверенный сертификат блокируется.
- Если HTTPS сообщает об ошибке синхронизации времени, проверьте доступность NTP.
