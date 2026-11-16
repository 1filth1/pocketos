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
- Maximum catalogue size: 16 packages. Additional folders are not scanned after the limit.

## Safety and current scope

- The catalogue opens metadata only and never interprets files as executable code.
- No automatic file extraction, writes, deletes or SD formatting occur.
- Resource browsing and installation are future work.
- A future built-in app integration must resolve to firmware-compiled code; SD packages remain data-only.
