# PocketOS v0.1.2-rc.7

**Experimental pre-release / Экспериментальный предварительный выпуск**

## Added / Добавлено
- HTTPS certificate validation using the built-in Arduino-ESP32 CA certificate bundle.
- UTC time synchronization before TLS validation when the device clock is not yet valid.
- Clear fail-closed behavior when time synchronization is unavailable.

## Security / Безопасность
- Removed insecure TLS mode from web-page and JPEG downloads.
- HTTPS-to-HTTP redirects are blocked.
- Redirect URLs containing embedded username/password information are rejected.
- Plain HTTP remains available without encryption; do not use it for sensitive information.

## Hardware test / Проверка на устройстве
1. Connect PocketOS to Wi-Fi with internet access.
2. Open a valid HTTPS article and confirm the page loads.
3. Open a JPEG image from an HTTPS page.
4. Confirm an invalid/untrusted TLS certificate is rejected.
5. If HTTPS reports a time-sync failure, check that the network allows NTP.

## Ограничения
- Initial TLS use may require access to NTP servers (`pool.ntp.org` or `time.cloudflare.com`).
- This release does not add arbitrary executable apps from SD or change Wi-Fi credential storage.

## Build target
Waveshare ESP32-S3-ePaper-3.97 (N16R8), 16 MB Flash, 8 MB OPI PSRAM.
Arduino-ESP32 3.3.12; DIO / 80 MHz; app3M_fat9M_16MB.
