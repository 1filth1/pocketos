# JDM Garage Empire — visual game implementation plan

Status: approved direction for the next PocketOS release. This is an implementation specification, not a claim that the features below are already complete.

## Product direction

JDM Garage Empire is a visual, step-by-step garage management and repair game for the Waveshare ESP32-S3 N16R8 and its 800x480 monochrome E-Ink display. Cars and mechanical components must be recognizable, detailed original illustrations rather than text labels or schematic placeholders.

The first launch roster is fixed at six original JDM-inspired vehicle illustrations, in this order: Nissan Skyline GT-R R34, Toyota Supra A80, Mazda RX-7 FD3S, Nissan Silvia S15, Mitsubishi Lancer Evolution VI and Honda NSX NA1. The first release is limited to these six cars. Adding later models will extend the SD catalogue and asset manifest, with a small matching runtime-capacity update if needed.

The game should feel like a compact illustrated workshop simulator:
- inspect a customer car;
- diagnose a fault by examining the relevant system;
- remove and replace individual components;
- choose tools and perform repair steps;
- test the result and collect payment;
- reinvest in stock, staff, tools and workshop equipment;
- build a collection of project cars and tuning builds.

## Non-negotiable storage rule

All game-specific content belongs on the MicroSD card. Firmware contains the game runtime, rendering code, input handling and safe file-format readers only. It must not contain the final car/part art catalog as PROGMEM fallback artwork.

Target SD layout:

```text
/Games/JDM_Garage_Empire/
  game.json
  content/
    game.json
    cars.json
    parts.json
    services.json
    customers.json
    upgrades.json
    events.json
    tutorial.json
  assets/
    manifest.json
    cars/
      hachi_86.mono
      kage_r32.mono
      hayate_s13.mono
      ryu_a80.mono
      rotary_fd.mono
      midori_ns.mono
      raid_evo.mono
      hoshi_sti.mono
      city_ek.mono
      road_na.mono
    parts/
      brake_disc.mono
      brake_caliper.mono
      brake_pads.mono
      fuel_pump.mono
      fuel_filter.mono
      radiator.mono
      water_pump.mono
      coilover.mono
      control_arm.mono
      turbocharger.mono
      engine.mono
      gearbox.mono
    workshop/
      lift.mono
      tool_chest.mono
      workbench.mono
      dyno.mono
    ui/
      icons/
      panels/
      backgrounds/
  saves/
    slot1.sav
    slot2.sav
    slot3.sav
```

The SD archive is merge-installed at the card root. Updating the game must never overwrite user saves or unrelated files.

## Art and display specification

- Native monochrome, 1-bit assets; black/white only, with deliberate dithering for metal, rubber, glass and shadows.
- Car side-profile canvas: 240x104 pixels; preserve the aspect ratio and avoid smoothing at runtime.
- Component canvas: 128x128 pixels with numbered connector nodes and mating-interface metadata in the parts catalogue.
- `scripts/generate_jdm_assets.py` creates the 1-bit art pack and SHA-256 manifest during CI before SD-content validation.
- Component canvas: 128x128 pixels initially.
- Workshop/background pieces should be tiled or composed from separate layers so the screen does not need a full-screen bitmap for every state.
- Artwork must have clean silhouettes, readable contours and mechanical detail at native resolution.
- No real manufacturer logos, trademark badges or copied brand graphics. Use original JDM-inspired fictional designs already represented by the game catalogue.
- A car is composed from a body illustration plus replaceable visual layers (wheels, bumpers, aero, hood, lights and condition/damage overlays) as the asset format evolves.
- A repair view should show the actual relevant part art and its position in the system, with an exploded/removed state for the active repair step.

## Asset format

Initial runtime format: row-major, MSB-first monochrome bitmap. A set bit means black. Each asset has an explicit width, height, byte count and asset ID in `assets/manifest.json`. The loader must:
- read only from the game directory on SD;
- validate manifest schema, dimensions, exact byte count and file size before drawing;
- reject path traversal and unknown IDs;
- use a bounded cache in PSRAM where available;
- release/replace cache entries deterministically;
- show a clear missing/corrupt asset state rather than silently substituting firmware-bundled art.

The current prototype's 240x104 car bitmap convention is a useful starting point, but the existing PROGMEM `CarSprites.h` copy-to-SD bootstrap is not the final design. The completed implementation must load the shipped SD artwork as the source of truth.

## Gameplay screens

1. **Garage** — detailed customer/project car, workshop equipment, condition, current job and compact finance/status panel.
2. **Inspection** — exterior walkaround and selectable systems: engine, fuel, cooling, brakes, suspension, transmission and body.
3. **Diagnostics** — symptoms, test actions, clues and confirmed fault; diagnosis should not be a random single-button result.
4. **Exploded repair view** — separate component illustrations, fasteners, connectors and tool prompts; animate the selected part between installed and removed positions using a small number of E-Ink-friendly frames.
5. **Parts and inventory** — compatible part, quality tier, price, stock, supplier lead time and installed part history.
6. **Workshop management** — staff, lifts, work bays, tool upgrades, rent, wages, reputation and customer queue.
7. **Tuning/project build** — engine, turbo, intake, exhaust, cooling, suspension, brakes, wheels and body upgrades with meaningful stats and trade-offs.
8. **Save/load** — robust versioned saves with checksum, temporary write, read-back verification and backup recovery.

## E-Ink and ESP32-S3 constraints

- Keep the retained 800x480 framebuffer and use partial refresh for small UI changes.
- Prefer deliberate screen transitions over continuous animation; use short 1-bit frame sequences only where they improve repair feedback.
- Avoid unnecessary full-screen refreshes and ghosting; force a clean refresh at defined transitions.
- Do not decode large source images at runtime. Convert and validate assets before packaging.
- Keep JSON parsing bounded and avoid loading the full catalogue and all images into RAM at once.
- Cache only the current car and current component art; measure internal heap and PSRAM high-water marks.
- Missing SD card, malformed manifest, truncated asset and failed save must be handled without crashing PocketOS.

## Release acceptance criteria

- Every car and component used by the game resolves to an SD asset in the matching release archive.
- Firmware does not depend on compiled-in car/part art for normal operation.
- Asset manifest validation runs in CI and verifies IDs, dimensions, byte counts, hashes and archive inclusion.
- The game loads content and art from SD after a cold boot.
- The garage, inspection, diagnostics, repair, inventory, tuning and save/load flows are playable.
- A matching firmware and SD content archive are published together with checksums and a changelog.
- Hardware validation is recorded separately from CI; a successful build is not evidence of a successful device test.

## Implementation sequence

1. Replace the current compiled-art bootstrap with an SD-native asset manifest and bounded loader.
2. Produce and package the first complete car and component art set.
3. Render the car asset in the garage and the matching component asset in the repair screen.
4. Build modular vehicle layers and condition/upgrade overlays.
5. Connect the existing catalogues to actual parts, jobs, customers, inventory and progression.
6. Add inspection, diagnosis and multi-step component repair interactions.
7. Add tuning builds, workshop upgrades, robust saves and automated content validation.
8. Publish a release candidate only after firmware compilation and SD package checks pass; request user hardware validation separately.

## Current codebase notes

The existing prototype already has:
- a JDM game view and navigation;
- a bounded JSON loader for the seven catalogues;
- a basic repair state machine and save slots;
- a simple garage illustration and placeholder component diagrams;
- an SD content archive pipeline.

The next work must evolve these pieces rather than create a parallel game. In particular, the existing catalogues currently validate more gameplay data than the game actually consumes. The implementation must connect those records to real game systems and replace placeholder drawings with SD-loaded art.

All development for this work is restricted to `release/pocketos-1.0`. Never modify `main`.
