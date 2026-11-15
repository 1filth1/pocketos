# PocketOS — 3D printing files / Файлы для 3D-печати

This directory contains the printable STL models currently committed for the PocketOS hardware enclosure and controls. The source files are kept in `docs/` so they can be downloaded individually.

В репозитории находятся STL-модели деталей корпуса и органов управления PocketOS. Исходные файлы оставлены в каталоге `docs/`; каждую модель можно скачать отдельно.

## Model files / Список моделей

| File | Download | Notes |
|---|---|---|
| `KB.stl` | [Download STL](https://raw.githubusercontent.com/1filth1/pocketos/release/pocketos-1.0/docs/KB.stl) | Model filename: KB |
| `backeink.stl` | [Download STL](https://raw.githubusercontent.com/1filth1/pocketos/release/pocketos-1.0/docs/backeink.stl) | Rear E-Ink part (name-based identification) |
| `button_eink (1).stl` | [Download STL](https://raw.githubusercontent.com/1filth1/pocketos/release/pocketos-1.0/docs/button_eink%20(1).stl) | E-Ink button part (name-based identification) |
| `front_eink.stl` | [Download STL](https://raw.githubusercontent.com/1filth1/pocketos/release/pocketos-1.0/docs/front_eink.stl) | Front E-Ink part (name-based identification) |
| `speaker.stl` | [Download STL](https://raw.githubusercontent.com/1filth1/pocketos/release/pocketos-1.0/docs/speaker.stl) | Speaker-related part (name-based identification) |

The descriptions above are based on filenames only. Exact dimensions, fit, orientation and intended printer settings have not yet been independently measured or validated.

Описания деталей приведены по именам файлов. Точные размеры, посадки, ориентация и параметры печати пока отдельно не измерялись и не проверялись.

## Before printing / Перед печатью

1. Open the STL in your slicer and check the model scale and units.
2. Inspect the mesh for errors and confirm that the part fits the intended hardware revision.
3. Check orientation, supports, wall thickness and clearances in the slicer preview.
4. Print a small fit-check first before printing the complete set.
5. Keep the original STL files unchanged; save any repaired or modified versions separately.

1. Открой STL в слайсере и проверь масштаб и единицы измерения.
2. Проверь сетку модели и соответствие конкретной ревизии устройства.
3. В предпросмотре слайсера проверь ориентацию, поддержки, толщину стенок и зазоры.
4. Сначала напечатай пробную деталь для проверки посадки.
5. Оригинальные STL не изменяй; исправленные и модифицированные варианты сохраняй отдельно.

## Hardware compatibility / Совместимость

These files are part of the PocketOS hardware documentation. Their presence in the repository does not by itself certify dimensional accuracy or successful fit on every Waveshare ESP32-S3-ePaper-3.97 (N16R8) board revision. Compare the models with the actual device and connected peripherals before a full print.

Эти файлы относятся к аппаратной части PocketOS. Само наличие STL в репозитории не гарантирует точность размеров и совместимость со всеми ревизиями Waveshare ESP32-S3-ePaper-3.97 (N16R8). Перед полной печатью сравни модели с реальной платой и установленными модулями.
