#!/usr/bin/env python3
"""Build a version-matched, mergeable PocketOS SD-card content archive."""
import hashlib
import json
import pathlib
import re
import sys
import zipfile

source = pathlib.Path(sys.argv[1])
out = pathlib.Path(sys.argv[2])
version = sys.argv[3]
commit = sys.argv[4]
out.mkdir(parents=True, exist_ok=True)

if version.startswith("PocketOS-"):
    version = version[len("PocketOS-"):]
if not re.fullmatch(r"v?\d+\.\d+\.\d+(?:-[A-Za-z0-9.-]+)?|development", version):
    raise SystemExit(f"Invalid SD content version: {version!r}")
if not re.fullmatch(r"[0-9a-f]{40}", commit):
    raise SystemExit(f"Invalid source commit SHA: {commit!r}")
if not source.is_dir():
    raise SystemExit(f"SD content source directory does not exist: {source}")

files = []
for path in sorted(source.rglob("*")):
    if path.is_symlink():
        raise SystemExit(f"Symlinks are not allowed in SD content: {path}")
    if path.is_dir():
        continue
    if not path.is_file():
        raise SystemExit(f"Unsupported SD content entry: {path}")
    rel = path.relative_to(source).as_posix()
    if rel.startswith("/") or ".." in pathlib.PurePosixPath(rel).parts:
        raise SystemExit(f"Unsafe archive path: {rel}")
    data = path.read_bytes()
    files.append({"path": rel, "size": len(data), "sha256": hashlib.sha256(data).hexdigest()})

if not files:
    raise SystemExit("SD content source is empty")

# Ensure every bundled App Manager package has a matching manifest and resources folder.
for manifest_path in (source / "apps").glob("*/app.ini"):
    package_id = manifest_path.parent.name
    raw = manifest_path.read_text(encoding="utf-8")
    values = {}
    for line in raw.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        if "=" not in line:
            raise SystemExit(f"Malformed package manifest: {manifest_path}")
        key, value = (part.strip() for part in line.split("=", 1))
        if key in values:
            raise SystemExit(f"Duplicate key {key!r} in {manifest_path}")
        values[key] = value
    if values.get("id") != package_id or values.get("type") != "content":
        raise SystemExit(f"Invalid data-only package manifest: {manifest_path}")
    for required in ("name", "version"):
        if not values.get(required):
            raise SystemExit(f"Missing {required!r} in {manifest_path}")
    if not (manifest_path.parent / "resources").is_dir():
        raise SystemExit(f"Missing resources directory for package {package_id}")

manifest = {
    "schema_version": 1,
    "firmware_version": version,
    "source_commit": commit,
    "content_root": "SD card root",
    "install_mode": "merge",
    "user_data_included": False,
    "files": files,
}
manifest_bytes = (json.dumps(manifest, ensure_ascii=False, indent=2) + "\n").encode("utf-8")
archive_name = f"PocketOS-SD-card-{version}.zip"
archive_path = out / archive_name
with zipfile.ZipFile(archive_path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
    for item in files:
        archive.write(source / item["path"], item["path"])
    archive.writestr("POCKETOS_SD_CARD.json", manifest_bytes)

with zipfile.ZipFile(archive_path, "r") as archive:
    names = set(archive.namelist())
    expected = {item["path"] for item in files} | {"POCKETOS_SD_CARD.json"}
    if names != expected:
        raise SystemExit(f"SD archive file set mismatch: {sorted(names ^ expected)}")
    for item in files:
        data = archive.read(item["path"])
        if len(data) != item["size"] or hashlib.sha256(data).hexdigest() != item["sha256"]:
            raise SystemExit(f"SD archive verification failed: {item['path']}")
    embedded = json.loads(archive.read("POCKETOS_SD_CARD.json"))
    if embedded != manifest:
        raise SystemExit("Embedded SD content manifest does not match source")

archive_hash = hashlib.sha256(archive_path.read_bytes()).hexdigest()
checksum_path = out / f"{archive_name}.sha256"
checksum_path.write_text(f"{archive_hash}  {archive_name}\n", encoding="ascii")
if hashlib.sha256(archive_path.read_bytes()).hexdigest() != checksum_path.read_text().split()[0]:
    raise SystemExit("SD archive SHA-256 verification failed")

print(json.dumps({"archive": archive_name, "sha256": archive_hash, "files": len(files), "version": version, "commit": commit}, indent=2))
