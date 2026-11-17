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
- [x] Use customer profiles for job budgets, patience and tips; use service/part records for quotes and costs; use weighted daily events for supplier discounts and reputation bonuses.

### M3 — playable management loop
- [x] One active customer job with budget validation, diagnosis, part purchase, manual repair procedure, mechanic delegation, quality check and payment.
- [x] Customer patience deadline while a job is waiting for diagnosis or parts; missed deadlines cancel the job, return half the purchased-part cost and reduce reputation.
- [ ] Replace the single active job with a persistent multi-order board and bay scheduling.
- [ ] Add a real warehouse with quantities and multiple part types.
- [ ] Add staff skill, fatigue, specialities and quality outcomes.
- [ ] Add upgrade purchase flow and workshop tier progression.

### M4 — visual polish and depth
- [x] Original car artwork and component/repair views are included in the firmware/content asset pipeline.
- [ ] Add illustrated job cards, garage upgrades, customer portraits/icons and car condition panels.
- [ ] Add event notices, milestones, a readable financial ledger and a campaign recap.
- [ ] Tune e-paper refresh behavior: static panels, partial updates only where safe, no rapid flashing.

## Hardware and UX constraints

- Target: Waveshare ESP32-S3-ePaper-3.97 (N16R8), 800×480 monochrome e-paper, 8 MB PSRAM.
- Navigation: CardKB directional keys, Enter to confirm, Tab to switch major panels, Escape to return.
- Prioritize clear typography, high-contrast shapes and meaningful status indicators over tiny text.
- Avoid frequent full-screen refreshes; the game is turn-based, not real-time.
- Keep saves crash-safe with temporary file, verification and backup rotation.

## Current status

The current branch has a data-driven single-job campaign slice. SD customer profiles now affect budgets, tips and patience; service and part records determine prices and weighted daily events can alter supplier costs and reputation. A customer deadline is tracked from job acceptance while the car is waiting for diagnosis or parts. Missing the deadline cancels that waiting job, returns half the part cost if a part had already been purchased, and reduces reputation. This is still not the full management game: there is no multi-order scheduling, persistent stock warehouse, upgrade economy, staff progression or hardware playtest. A successful firmware compile proves buildability only, not that the complete game loop is balanced or usable on the device.
