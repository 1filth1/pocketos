# SD-card layout

The SD card has two categories of files:

## Versioned PocketOS defaults

These are maintained in the repository under `sdcard/root/` and shipped in a version-matched release archive.

- `/apps/<package-id>/app.ini` — package metadata.
- `/apps/<package-id>/resources/` — read-only package resources.

## User and runtime data

These belong to the device owner and are not replaced by the release content archive:

- Personal documents, photos and downloads.
- Notes and game saves.
- Settings, logs and other runtime-generated data.

When updating, merge the release archive into the card root. Never format the card or replace the entire card contents with the archive.
