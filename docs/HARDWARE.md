# Hardware Reference (source-derived)

> This is a preliminary reference extracted from the current source. It is not a verified schematic, wiring guide or production pinout. Confirm every connection against the exact board revision and schematic before wiring or flashing.

## Target platform

- MCU family: ESP32-S3
- Display target: 800 × 480 E-Ink
- Display driver referenced in source: `GxEPD2_397_GDEM0397T81`
- Display library family: GxEPD2
- Storage interface referenced: SD_MMC
- Main firmware entry: `main.cpp`

## Pin and bus constants found in `main.cpp`

| Function | Source constant | GPIO / value |
|---|---|---:|
| EPD BUSY | `EPD_BUSY` | 3 |
| EPD DC | `EPD_DC` | 9 |
| EPD CS | `EPD_CS` | 10 |
| EPD SCLK | `EPD_SCLK` | 11 |
| EPD MOSI | `EPD_MOSI` | 12 |
| EPD RST | `EPD_RST` | 46 |
| I2C SDA | `KB_SDA / RTC_SDA` | 41 |
| I2C SCL | `KB_SCL / RTC_SCL` | 42 |
| CardKB address | `KB_ADDR` | 0x5F |
| RTC address | `RTC_ADDR` | 0x51 |
| SHTC3 address | `SHTC3_ADDR` | 0x70 |
| SD CMD | `SD_CMD` | 17 |
| SD CLK | `SD_CLK` | 16 |
| SD D0 | `SD_D0` | 15 |
| SD D1 | `SD_D1` | 7 |
| SD D2 | `SD_D2` | 8 |
| SD D3 | `SD_D3` | 18 |

The source also contains comments referring to GPS UART RX GPIO44 and TX GPIO43. Confirm the active wiring and initialization path before treating these as final assignments.

## I2C devices referenced

- CardKB keyboard controller: address 0x5F.
- RTC: address 0x51.
- SHTC3 environmental sensor: address 0x70.
- Power-management device access at address 0x34 is referenced in the firmware.

Addresses and bus sharing are source-derived. Verify device variants, voltage levels, pull-ups and bus topology on the actual board.

## Before publishing a verified hardware guide

- [ ] Confirm exact Waveshare product name and hardware revision.
- [ ] Compare source pin constants with the official schematic.
- [ ] Confirm display panel and controller revision.
- [ ] Confirm PSRAM and Flash size on the actual module.
- [ ] Confirm SD_MMC mode and card compatibility.
- [ ] Verify I2C voltage levels and all device addresses.
- [ ] Confirm GPS/audio/power peripheral wiring where populated.
- [ ] Record photos or a reviewed schematic revision.
- [ ] Run a hardware smoke test and record firmware commit, board revision and results.

## Русский

> Это предварочная справка, составленная по исходникам. Она не является проверенной электрической схемой, инструкцией по подключению или окончательной распиновкой. Перед подключением и прошивкой сверьте каждый сигнал с точной ревизией платы и её схемой.

### Целевая платформа

- Семейство MCU: ESP32-S3.
- Целевой дисплей: E-Ink 800 × 480.
- Указанный в исходниках драйвер: `GxEPD2_397_GDEM0397T81`.
- Семейство библиотеки дисплея: GxEPD2.
- Интерфейс карты памяти в исходниках: SD_MMC.
- Основной файл прошивки: `main.cpp`.

### Найденные в `main.cpp` константы выводов

| Назначение | Константа | GPIO / значение |
|---|---|---:|
| EPD BUSY | `EPD_BUSY` | 3 |
| EPD DC | `EPD_DC` | 9 |
| EPD CS | `EPD_CS` | 10 |
| EPD SCLK | `EPD_SCLK` | 11 |
| EPD MOSI | `EPD_MOSI` | 12 |
| EPD RST | `EPD_RST` | 46 |
| I2C SDA | `KB_SDA / RTC_SDA` | 41 |
| I2C SCL | `KB_SCL / RTC_SCL` | 42 |
| Адрес CardKB | `KB_ADDR` | 0x5F |
| Адрес RTC | `RTC_ADDR` | 0x51 |
| Адрес SHTC3 | `SHTC3_ADDR` | 0x70 |
| SD CMD | `SD_CMD` | 17 |
| SD CLK | `SD_CLK` | 16 |
| SD D0 | `SD_D0` | 15 |
| SD D1 | `SD_D1` | 7 |
| SD D2 | `SD_D2` | 8 |
| SD D3 | `SD_D3` | 18 |

В исходнике также есть комментарии о GPS UART: RX GPIO44 и TX GPIO43. Перед тем как считать их окончательными назначениями, проверьте фактическое подключение и путь инициализации.

### Устройства на I2C, упомянутые в исходниках

- Контроллер клавиатуры CardKB: адрес 0x5F.
- RTC: адрес 0x51.
- Датчик SHTC3: адрес 0x70.
- В прошивке также есть обращение к узлу управления питанием по адресу 0x34.

Адреса и совместное использование шины взяты из исходников. Проверьте варианты микросхем, уровни напряжения, подтяжки и топологию шины на реальной плате.

### Что проверить перед публикацией окончательной аппаратной документации

- [ ] Точное название платы Waveshare и аппаратная ревизия.
- [ ] Сверка констант GPIO со схемой производителя.
- [ ] Ревизия E-Ink панели и контроллера.
- [ ] Объём PSRAM и Flash конкретного модуля.
- [ ] Режим SD_MMC и совместимость карт.
- [ ] Уровни I2C и адреса всех устройств.
- [ ] Подключение GPS, аудио и питания в фактической комплектации.
- [ ] Фотографии или проверенная версия схемы.
- [ ] Аппаратный smoke test с указанием коммита прошивки, ревизии платы и результатов.
