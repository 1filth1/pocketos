# PocketOS SD-card content

This directory is the version-controlled source of the **default SD-card content** shipped with PocketOS. It is versioned alongside firmware because the expected folders, sample packages and resource formats can evolve between firmware releases.

## Source layout

- `root/` mirrors the root of the physical microSD card.
- Put distributable, firmware-compatible defaults here: bundled data-only app packages, examples, templates and documentation.
- Do **not** put user saves, notes, Wi-Fi credentials, settings, logs or other personal data in this directory.
- Runtime-generated files and user data are not part of the release image and must never be overwritten by an update.

## Release packaging

CI runs `scripts/package_sdcard.py` for the same commit as the firmware build. Each release receives:

- `PocketOS-SD-card-v<version>.zip` — files to copy to the root of the microSD card.
- `PocketOS-SD-card-v<version>.sha256` — SHA-256 checksum for the archive.

The archive includes `POCKETOS_SD_CARD.json` with the matching firmware version, source commit and hashes of all shipped files. This keeps the SD content tied to the exact firmware source revision.

## Installing or updating the card

1. Back up the entire microSD card.
2. Extract the archive and copy its contents to the card root.
3. Merge the supplied `apps/` directory with the existing one; do not format the card or delete unrelated files.
4. If a destination file already exists, compare it before replacing it. Preserve personal data.

The release archive is a **mergeable default-content bundle**, not a full-card image and not a disk formatter.

## Current default content

- `root/README.txt` — short on-card installation and backup guidance.
- `root/apps/pocketos-guide/` — a data-only guide package used to verify App Manager listing and resource preview.
- `root/Games/JDM_Garage_Empire/` — versioned game content for cars, customers, repair services, parts, workshop upgrades, daily events and tutorial chapters. Saves remain runtime-generated and are not shipped.

As PocketOS gains firmware features that require additional SD files, add or update them here and document compatibility in this file and the release notes.
