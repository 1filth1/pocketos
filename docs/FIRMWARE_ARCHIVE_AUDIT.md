# Firmware archive audit and boot-loop root-cause analysis

## Known-good reference archive

The user supplied a firmware.zip from Schematic.io and reported that it boots and launches applications on the physical **Waveshare ESP32-S3-ePaper-3.97 (N16R8)**. Static inspection of that archive found:

| File | Size | SHA-256 |
|---|---:|---|
| bootloader.bin | 19,984 bytes | b68c1ed33c43a4290375213f24d608c125e31e22880d16d4a5dcba89e083716d |
| partitions.bin | 3,072 bytes | aaae2888c5a6a348004b5b436f47abb25ae32e72d9003902955a998eda723edd |
| boot_app0.bin | 8,192 bytes | f94c5d786a7a8fab06ac5d10e33bf37711a6697636dc037559ea19cc410a17f0 |
| firmware.bin | 1,705,856 bytes | 600fefc715ba7ba48c1c1cafbe32a64cb8103c74e8187107c2549d471d14829f |

Its bootable image headers declare **DIO / 16 MB / 80 MHz**. Its partition table is an older single-app layout ending at 4 MiB: NVS at 0x9000, OTA data at 0xE000, a 3 MiB app at 0x10000, SPIFFS at 0x310000, and coredump at 0x3F0000. This archive is a user-confirmed recovery baseline, but it has no embedded source commit and is not the PocketOS CI release package.

## Observed failure

The user-reported ROM output from the earlier PocketOS candidate was:

~~~text
ESP-ROM:esp32s3-20210327
rst:0x7 (TG0WDT_SYS_RST) boot:0x2a (SPI_FAST_FLASH_BOOT)
mode:QIO, clock div:1
load:0x3fce2820,len:0x10cc
ets_loader.c 78
~~~

The sequence repeated and the USB serial port detached/reappeared. The log stops during the ROM's attempt to load the second-stage bootloader from SPI Flash. It does not reach the ESP-IDF second-stage boot messages or Arduino setup().

## Root cause assessment

**The strongest evidence points to a QIO flash-read-mode mismatch on this N16R8 board.**

- The failed image explicitly reports mode:QIO in the ROM boot line.
- The user-confirmed working archive declares DIO / 80 MHz.
- The initial ROM bootloader must read the second-stage bootloader before it can parse the partition table or run any application code.
- Espressif documents that some devices cannot boot when QIO is selected, even if writing succeeds, and recommends trying DIO when flash reads fail: [SPI flash modes](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/spi-flash-modes.html) and [troubleshooting](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/troubleshooting.html).

The earlier diagnostic commit moved Serial.begin() before peripheral initialization and added trace messages. That change cannot fix this failure because Arduino setup() is never reached. It was diagnostic instrumentation, not a root-cause fix.

The partition table is also not the cause of this specific ROM-stage failure: the ROM must load the second-stage bootloader before that bootloader reads the partition table. The old partition layout still matters for data compatibility, so it is handled as a separate release decision.

This is a high-confidence software/configuration diagnosis, not a physical proof. The new candidate must still be tested on the user's board. No eFuse changes are proposed or performed.

## Current candidate profile

The candidate is built for:

- Waveshare ESP32-S3-ePaper-3.97 (N16R8)
- 16 MB Flash
- 8 MB OPI PSRAM
- DIO / 80 MHz flash mode
- Arduino-ESP32 3.3.12
- app3M_fat9M_16MB partition scheme

The requested 16 MB partition map is intentionally different from the 4 MiB reference archive. It has two 3 MiB OTA app slots and a 9.875 MiB FATFS partition. Old SPIFFS data may become inaccessible after changing the partition table. Back up the SD card and important internal Flash data before testing.

The Web Flasher writes only the bootloader, partition table, boot_app0 and application at the manifest addresses. It uses eraseAll: false, does not issue a full-chip erase and does not write eFuses.

## Validation limits

- The original archive's boot success is user-reported physical evidence.
- CI compile/package/deployment success is software evidence only.
- The new DIO + app3M_fat9M_16MB candidate has not been physically validated by the assistant.
- E-Ink, side OLED, SD_MMC, I²C, audio, Wi-Fi, power behavior and repeated cold boots remain user-side hardware tests.
