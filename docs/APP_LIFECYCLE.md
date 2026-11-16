# PocketOS App Lifecycle / Жизненный цикл приложений

## Purpose

This is the first compatibility layer between the current view-based firmware and a future modular application model.

The current firmware is a single-address-space Arduino application. A PocketOS view is **not** an isolated process, and changing views does not automatically free memory or stop background services.

## Current contract

- `appRuntimeEnter(view, reason)` records a view session start.
- `appRuntimeLeave(reason)` records its end and elapsed time.
- `appRuntimeTick(view)` bridges existing navigation by detecting view changes in the main loop.
- Per-view counters retain launch count, last session duration, and accumulated runtime.
- Heap snapshots distinguish internal RAM from PSRAM and do not allocate memory.

The statistics table is fixed-size (64 entries) and indexed by the numeric `View` value. Out-of-range values are safely ignored.

## Deliberate limitations

This first step does not introduce:
- process isolation, dynamic executable loading, or multitasking;
- automatic heap reclamation or claims of leak prevention;
- automatic shutdown of GPS, audio, Wi-Fi, recorder, sequencer, or other services;
- changes to the existing keyboard/navigation behavior.

Some current screens use blocking draw/input loops. A real suspend/resume or resource-release hook must not be added until those loops are converted to cooperative event-driven screens and each service has an explicit owner.

## Next implementation steps

1. Inventory each view's owned resources and background services.
2. Convert one low-risk application to a non-blocking event-driven screen.
3. Add explicit `start / handle / draw / stop` hooks for that application.
4. Verify repeated open/close cycles and heap stability on the Waveshare ESP32-S3-ePaper hardware.
5. Migrate other applications incrementally; keep legacy routing as a compatibility path until coverage is complete.

## Hardware validation

Capture serial output while opening and leaving several views repeatedly. Check matching `[APP] enter/leave` events, session durations, and `[MEM]` internal/PSRAM snapshots. A falling free-heap value alone is not proof of a leak; compare repeated cycles and largest free blocks.
