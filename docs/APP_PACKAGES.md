# SD application package format (RC6)

RC6 introduces a read-only catalogue for SD content/resource packages. It does not execute code from the card.

## Directory layout

```
/apps/
  example-pack/
    app.ini
    resources/
      cover.bmp
      readme.txt
```

The folder name must be a lowercase package ID containing only `a-z`, `0-9`, hyphen or underscore, up to 24 characters.

## Manifest: app.ini

UTF-8 text, maximum 768 bytes. One `key=value` pair per line. Blank lines and lines beginning with `#` are ignored.

Required keys:

```ini
id=example-pack
name=Example Pack
version=1.0
type=content
description=Example resource package
```

- `id`: required, must exactly match the containing folder name.
- `name`: required, 1–32 characters.
- `version`: required, 1–16 characters.
- `type`: required; RC6 accepts only `content`.
- `description`: optional, up to 96 characters.
- Duplicate keys, unknown keys, malformed lines, oversized files and unsupported types invalidate the manifest.
- Maximum catalogue size: 16 valid packages. The scanner inspects at most 32 package directories per scan; additional directories are left for a later firmware version.

## Safety and current scope

- The catalogue and resource browser are read-only; they never interpret files as executable code.
- The resource browser lists up to 24 regular files in the package's top-level `resources/` directory, inspecting at most 64 directory entries per scan. It does not recurse into subdirectories.
- Text preview supports `.txt`, `.md` and `.ini` files up to 4096 bytes. Longer files and other formats remain listed but are not opened.
- Text is displayed in bounded pages of eight lines; each rendered line is capped before display.
- Hidden names, path separators and unsafe path components are rejected. Missing SD media or a missing `resources/` directory is reported without changing package data.
- No file writes, deletes, extraction, installation or SD formatting occur.
- A future built-in app integration must resolve to firmware-compiled code; SD packages remain data-only.

## RC11 package integrity audit

When App Manager scans `/apps`, each package with a valid manifest receives a bounded, read-only resource audit:

- `resources/` must be a directory for a clean integrity result. A missing directory is reported as a warning; the package remains visible so the user can inspect or repair it.
- The audit inspects at most 64 directory entries and never recurses. A directory entry or additional entries beyond the scan bound mark the result as incomplete.
- Safe regular-file names are counted; hidden names, path separators, control characters and unsafe components are excluded and reported as a warning.
- The details screen reports the integrity state, accepted file count and aggregate byte size. The aggregate is saturated at the 32-bit unsigned maximum rather than wrapping.
- An empty resource directory is reported as a warning, not silently treated as a complete package.
- Manifest values reject ASCII control characters. Existing required keys, duplicate/unknown-key rejection, folder-ID matching, length limits and `type=content` restriction remain in force.

Warnings are diagnostic, not destructive: RC11 does not modify package files, delete entries, install packages or execute code from the SD card.

## Versioned SD-card content

The repository's `sdcard/root/` directory is the source of truth for default files shipped with PocketOS. CI packages it for the same commit and firmware version as the release into `PocketOS-SD-card-v<version>.zip`, with `POCKETOS_SD_CARD.json` and a separate SHA-256 sidecar. The Web Flasher exposes the archive for the currently selected firmware release when available.

The initial bundle includes a `pocketos-guide` data-only package under `/apps/`. This provides a ready-made catalogue entry and text resources for the App Manager resource browser.

Only distributable defaults belong in this tree. Personal documents, notes, saves, credentials, runtime settings and logs are excluded. Install the archive by merging its contents into the SD-card root after making a backup; it is not a full-card image and must not format or replace the card.
