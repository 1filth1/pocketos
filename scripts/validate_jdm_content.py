#!/usr/bin/env python3
"""Validate the versioned JDM Garage Empire SD content catalogue."""
import json
import pathlib
import sys

root = pathlib.Path(sys.argv[1]) / "Games" / "JDM_Garage_Empire" / "content"
required = ("game.json", "cars.json", "services.json", "parts.json",
            "customers.json", "upgrades.json", "events.json", "tutorial.json")
data = {}
for name in required:
    path = root / name
    if not path.is_file():
        raise SystemExit(f"Missing JDM content file: {path}")
    try:
        data[name] = json.loads(path.read_text(encoding="utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        raise SystemExit(f"Invalid JSON in {path}: {exc}")
    if data[name].get("schema_version") != 1:
        raise SystemExit(f"Unsupported schema_version in {path}")

def records(filename, key):
    values = data[filename].get(key)
    if not isinstance(values, list) or not values:
        raise SystemExit(f"{filename}: expected non-empty '{key}' array")
    ids = [item.get("id") for item in values]
    if any(not isinstance(value, str) or not value for value in ids):
        raise SystemExit(f"{filename}: every {key} item needs a non-empty id")
    if len(ids) != len(set(ids)):
        raise SystemExit(f"{filename}: duplicate IDs in {key}")
    return values

# The visual game is not releasable without its complete SD-native art pack.
asset_dir = pathlib.Path(sys.argv[1]) / "Games" / "JDM_Garage_Empire" / "assets"
asset_manifest_path = asset_dir / "manifest.json"
asset_blob_path = asset_dir / "assets.bin"
if not asset_manifest_path.is_file() or not asset_blob_path.is_file():
    raise SystemExit("Missing JDM SD art pack: assets/manifest.json and assets/assets.bin are both required")
try:
    asset_manifest = json.loads(asset_manifest_path.read_text(encoding="utf-8"))
except (UnicodeDecodeError, json.JSONDecodeError) as exc:
    raise SystemExit(f"Invalid JDM art manifest: {exc}")
if asset_manifest.get("schema_version") != 1 or asset_manifest.get("format") != "MONO_MSB_1BIT":
    raise SystemExit("Unsupported JDM art manifest schema or bitmap format")
asset_blob = asset_blob_path.read_bytes()
if len(asset_blob) != asset_manifest.get("total_bytes") or len(asset_blob) != 43296:
    raise SystemExit(f"JDM art pack size mismatch: expected 43296, got {len(asset_blob)}")
assets = asset_manifest.get("assets")
if not isinstance(assets, list) or len(assets) != 18:
    raise SystemExit("JDM art manifest must contain exactly 6 cars and 12 parts")
seen_ids = set()
for item in assets:
    asset_id = item.get("id")
    if not isinstance(asset_id, str) or asset_id in seen_ids:
        raise SystemExit(f"Invalid or duplicate JDM asset id: {asset_id!r}")
    seen_ids.add(asset_id)
    kind = item.get("kind")
    width, height = (240, 104) if kind == "car" else (128, 128) if kind == "part" else (0, 0)
    if item.get("width") != width or item.get("height") != height:
        raise SystemExit(f"Invalid dimensions for JDM asset {asset_id}")
    expected_size = ((width + 7) // 8) * height
    offset, length = item.get("offset"), item.get("length")
    if not isinstance(offset, int) or not isinstance(length, int) or length != expected_size or offset < 0 or offset + length > len(asset_blob):
        raise SystemExit(f"Invalid byte range for JDM asset {asset_id}")
    import hashlib
    if hashlib.sha256(asset_blob[offset:offset + length]).hexdigest() != item.get("sha256"):
        raise SystemExit(f"JDM art checksum mismatch: {asset_id}")
if sum(1 for item in assets if item.get("kind") == "car") != 6 or sum(1 for item in assets if item.get("kind") == "part") != 12:
    raise SystemExit("JDM art manifest must contain 6 cars and 12 parts")

game = data["game.json"]
if game.get("game_id") != "jdm_garage_empire":
    raise SystemExit("Unexpected game_id")
if game.get("content_version") != "1.0.0":
    raise SystemExit("Unexpected initial content_version")
for key in ("cash", "reputation", "day", "workshop_level", "staff_slots"):
    if not isinstance(game.get("starting_conditions", {}).get(key), int):
        raise SystemExit(f"game.json: starting_conditions.{key} must be an integer")
cars = records("cars.json", "cars")
services = records("services.json", "services")
parts = records("parts.json", "parts")
records("customers.json", "customers")
upgrades = records("upgrades.json", "upgrades")
records("events.json", "events")
records("tutorial.json", "chapters")

for car in cars:
    if not 0 <= car.get("condition", -1) <= 100 or car.get("value", 0) <= 0:
        raise SystemExit(f"Invalid car condition/value: {car.get('id')}")
for service in services:
    for field in ("base_price", "base_cost", "duration_days", "skill", "rep"):
        if not isinstance(service.get(field), int) or service[field] < 0:
            raise SystemExit(f"Invalid {field} in service {service.get('id')}")
    if service["base_price"] < service["base_cost"]:
        raise SystemExit(f"Service is loss-making before overhead: {service['id']}")
for part in parts:
    if part.get("cost", 0) <= 0 or not 1 <= part.get("quality", 0) <= 5:
        raise SystemExit(f"Invalid part cost/quality: {part.get('id')}")
for upgrade in upgrades:
    if upgrade.get("cost", 0) <= 0 or upgrade.get("rep_required", -1) < 0:
        raise SystemExit(f"Invalid upgrade cost/reputation: {upgrade.get('id')}")

declared = set(game.get("content_files", []))
actual = {name for name in required if name != "game.json"}
if declared != actual:
    raise SystemExit(f"game.json content_files mismatch: {sorted(declared ^ actual)}")

print(f"JDM content OK: {len(cars)} cars, {len(services)} services, "
      f"{len(parts)} parts, {len(upgrades)} upgrades")
