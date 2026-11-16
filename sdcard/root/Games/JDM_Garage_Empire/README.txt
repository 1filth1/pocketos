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

This is the first content/schema drop. Firmware support for reading these
JSON catalogues will be introduced in the next game milestone. Until then,
the current built-in prototype continues to use its firmware-side starter
tables and artwork fallback.

Back up your SD card before updating. Merge the Games folder; do not format
the card.
