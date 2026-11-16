JDM GARAGE EMPIRE — GAME CONTENT 1.0.0
========================================

This folder contains the first data-driven content pack for JDM Garage Empire.
It is safe to merge into an existing PocketOS SD card.

CONTENT/
  game.json       Starting economy and progression rules
  cars.json       Ten fictional JDM-inspired customer cars
  services.json   Repair and tuning jobs with costs and durations
  parts.json      Parts catalogue, quality tiers and starting stock
  customers.json  Customer profiles, budgets and patience
  upgrades.json   Workshop expansion and equipment
  events.json     Daily events that affect demand and costs
  tutorial.json   Onboarding chapters

The content is separate from firmware and save files. Do not delete the
/Games/JDM_Garage_Empire/saves folder when updating game content.

PocketOS now reads these JSON catalogues at game startup. The loader checks
schema versions, file sizes, required fields, record limits, duplicate IDs
and basic economy sanity before accepting the pack. It loads the car names,
starting cash/reputation/day, daily rent and mechanic wage; the content screen
(key C in the game) reports all loaded catalogue counts and sample entries.
If the SD card or any required file is missing or invalid, the game falls back
to its built-in starter data. Saves remain separate in /saves/.

Back up your SD card before updating. Merge the Games folder; do not format
the card.
