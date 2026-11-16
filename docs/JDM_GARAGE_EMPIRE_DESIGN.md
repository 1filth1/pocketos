# JDM Garage Empire — game design and content plan

## Direction

JDM Garage Empire is a visual, turn-based workshop management and economic strategy for the 800×480 monochrome e-paper display. The player starts with a small one-bay garage and grows it into a respected specialist workshop. The atmosphere mixes late-1980s/1990s Japanese street-car culture with a modern management sim.

The game should be readable at a glance, use large illustrated car/workshop panels, and avoid rapid animation that would cause unnecessary e-paper refreshes. It is designed around deliberate actions and short play sessions.

## Core loop

1. Review the day: cash, reputation, open work orders, staff and workshop capacity.
2. Browse incoming customers and inspect their cars, budgets, expectations and symptoms.
3. Diagnose faults; choose a repair scope and estimate price, parts and delivery date.
4. Order parts from suppliers. Balance cost, quality, stock and delivery time.
5. Perform the repair as a compact interactive procedure, or assign it to a mechanic whose skill and workload affect quality and duration.
6. Run quality control, deliver the car, collect payment and receive reputation/customer feedback.
7. Pay rent and wages, review the ledger, invest in equipment and advance to the next day.

## Strategic decisions

- Fast, low-margin work keeps cash moving; specialist builds bring larger profit and reputation but tie up a bay and capital.
- Cheap parts protect short-term cash but increase comeback risk. Premium parts cost more and can improve quality and reputation.
- Hiring increases throughput but creates recurring wages. Staff have skill levels, fatigue and specialities.
- Workshop upgrades unlock new service categories and capacity; buying every upgrade immediately should not be optimal.
- Customer patience, budgets and expectations create meaningful scheduling choices.
- Random local events change demand and supplier prices without turning the game into pure luck.

## Progression

| Tier | Workshop identity | Unlock direction |
|---|---|---|
| 1 | Backstreet Shed | Basic inspections and repairs |
| 2 | Street Workshop | More capacity, regular customers |
| 3 | Performance Garage | Engine tuning and suspension setups |
| 4 | Tuning Atelier | Turbo builds, bodywork and premium jobs |
| 5 | Legendary Works | Restorations, rare builds and elite clients |

Reputation is the main progression gate; cash alone must not unlock the whole game.

## Content stored on the SD card

The initial content pack lives at `/Games/JDM_Garage_Empire/` and is independent of firmware:

- `content/game.json`: starting balance, economy and progression settings.
- `content/cars.json`: fictional JDM-inspired car catalogue.
- `content/customers.json`: customer profiles, budgets, patience and preferences.
- `content/services.json`: service prices, costs, durations, skill and reputation rewards.
- `content/parts.json`: parts catalogue, quality and stock.
- `content/upgrades.json`: equipment and workshop expansion.
- `content/events.json`: daily demand and supplier events.
- `content/tutorial.json`: onboarding chapters.

Save slots are runtime user data under `/Games/JDM_Garage_Empire/saves/`; they must never be included in the distributable content archive or overwritten by content updates.

## Implementation milestones

### M1 — content foundation
- Ship the versioned SD content pack with the firmware-matched SD bundle.
- Validate JSON syntax, required fields, unique IDs, value ranges and referenced IDs in CI.
- Keep the existing firmware-side defaults as a safe fallback.

### M2 — content loader (first runtime slice)
- [x] Load all seven catalogues plus `game.json` from SD at game startup with an 8 KB per-file limit and schema-v1 checks.
- [x] Validate record counts, required values, unique IDs and basic economy constraints; reject the whole pack on any failure.
- [x] Fall back to built-in car names and starting defaults if the card or any required file is unavailable/invalid.
- [x] Apply SD car names, starting cash/reputation/day, daily rent and mechanic wages; expose catalogue counts on the in-game Content panel (`C`).
- [ ] Use service/parts/customer/upgrade/event/tutorial records to drive the full management loop (next milestone).
- SD data remains data-only; no executable code is loaded.

### M3 — playable management loop
- Replace the one-job prototype with a daily job board, multiple work orders and workshop capacity.
- Add estimates, delivery deadlines, customer patience, parts lead times, wages and recurring rent.
- Add staff skill, delegation and quality outcomes.

### M4 — visual polish and depth
- Add illustrated job cards, garage upgrades, customer portraits/icons and car condition panels.
- Add event notices, milestones, a readable financial ledger and a campaign recap.
- Tune e-paper refresh behavior: static panels, partial updates only where safe, no rapid flashing.

## Hardware and UX constraints

- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 800×480 monochrome e-paper, 8 MB PSRAM.
- Navigation: CardKB directional keys, Enter to confirm, Tab to switch major panels, Escape to return.
- Prioritize clear typography, high-contrast shapes and meaningful status indicators over tiny text.
- Avoid frequent full-screen refreshes; the game is turn-based, not real-time.
- Keep saves crash-safe with temporary file, verification and backup rotation.

## Current status

The repository contains the firmware-side JDM prototype, car artwork, repair flow, economy primitives and SD save slots. The versioned SD pack is now parsed at game startup by a bounded ArduinoJson loader. It validates every catalogue and uses SD car names plus the starting economy/rent/wage settings; a Content panel reports the loaded catalogues. The existing repair and job logic is still partly firmware-defined: the other catalogue records are loaded and validated but do not yet drive jobs, inventory, staff progression or events. The full data-driven management loop remains the next milestone, and hardware testing is still required.
