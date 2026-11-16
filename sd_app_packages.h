#pragma once

// SD package catalogue v1. Reads metadata only; never loads or executes code.
static constexpr uint8_t SD_APP_MAX_PACKAGES = 16;
static constexpr uint8_t SD_APP_MAX_SCAN_DIRS = 32;
static constexpr size_t SD_APP_MANIFEST_MAX_BYTES = 768;

struct SdAppPackage {
  String id;
  String name;
  String version;
  String description;
  String type;
  String path;
};

SdAppPackage sdAppPackages[SD_APP_MAX_PACKAGES];
uint8_t sdAppPackageCount = 0;
uint8_t sdAppSelected = 0;
uint8_t sdAppFirst = 0;
bool sdAppInfoOpen = false;
uint8_t sdAppInvalidCount = 0;

static bool sdAppSafeToken(const String &value, size_t maxLength) {
  if (!value.length() || value.length() > maxLength) return false;
  for (size_t i = 0; i < value.length(); ++i) {
    char c = value[i];
    if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_')) return false;
  }
  return true;
}

static bool sdAppReadManifest(const String &dirPath, const String &dirName, SdAppPackage &out) {
  if (!sdAppSafeToken(dirName, 24)) return false;
  String manifestPath = dirPath + "/app.ini";
  File file = SD_MMC.open(manifestPath.c_str(), FILE_READ);
  if (!file) return false;
  size_t size = file.size();
  if (!size || size > SD_APP_MANIFEST_MAX_BYTES) { file.close(); return false; }
  String raw = file.readString();
  file.close();
  if (raw.length() != size) return false;

  String id, name, version, description, type;
  uint8_t seen = 0;
  int start = 0;
  while (start < (int)raw.length()) {
    int end = raw.indexOf('
', start);
    if (end < 0) end = raw.length();
    String line = raw.substring(start, end);
    line.trim();
    start = end + 1;
    if (!line.length() || line.startsWith("#")) continue;
    int equals = line.indexOf('=');
    if (equals <= 0) return false;
    String key = line.substring(0, equals);
    String value = line.substring(equals + 1);
    key.trim(); value.trim();
    uint8_t bit = 0;
    if (key == "id") { bit = 1; id = value; }
    else if (key == "name") { bit = 2; name = value; }
    else if (key == "version") { bit = 4; version = value; }
    else if (key == "type") { bit = 8; type = value; }
    else if (key == "description") { bit = 16; description = value; }
    else return false;
    if (seen & bit) return false;
    seen |= bit;
  }

  if ((seen & 15) != 15) return false;
  if (!sdAppSafeToken(id, 24) || id != dirName) return false;
  if (!name.length() || name.length() > 32 || !version.length() || version.length() > 16) return false;
  if (description.length() > 96 || type != "content") return false;
  out.id = id;
  out.name = name;
  out.version = version;
  out.description = description;
  out.type = type;
  out.path = dirPath;
  return true;
}

void sdAppScan() {
  sdAppPackageCount = 0;
  sdAppInvalidCount = 0;
  if (!cardOK || SD_MMC.cardType() == CARD_NONE) {
    status = "SD APP CATALOG: NO CARD";
    return;
  }
  File root = SD_MMC.open("/apps", FILE_READ);
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    status = "NO /apps DIRECTORY ON SD";
    return;
  }
  uint8_t inspectedDirectories = 0;
  while (sdAppPackageCount < SD_APP_MAX_PACKAGES && inspectedDirectories < SD_APP_MAX_SCAN_DIRS) {
    File entry = root.openNextFile();
    if (!entry) break;
    bool isDirectory = entry.isDirectory();
    String name = leaf(String(entry.name()));
    entry.close();
    if (!isDirectory || name.startsWith(".")) continue;
    ++inspectedDirectories;
    String path = String("/apps/") + name;
    SdAppPackage package;
    if (sdAppReadManifest(path, name, package)) {
      sdAppPackages[sdAppPackageCount++] = package;
    } else {
      ++sdAppInvalidCount;
    }
    delay(1);
  }
  root.close();
  for (int i = 1; i < sdAppPackageCount; ++i) {
    SdAppPackage item = sdAppPackages[i];
    int j = i;
    while (j > 0 && item.name.compareTo(sdAppPackages[j - 1].name) < 0) {
      sdAppPackages[j] = sdAppPackages[j - 1];
      --j;
    }
    sdAppPackages[j] = item;
  }
  sdAppSelected = constrain(sdAppSelected, 0, max(0, (int)sdAppPackageCount - 1));
  sdAppFirst = (sdAppSelected / 6) * 6;
  status = String(sdAppPackageCount) + " PACKAGE(S), " + String(sdAppInvalidCount) + " INVALID";
}

void sdAppsDraw() {
  view = sdAppInfoOpen ? SD_APP_INFO : SD_APPS;
  beginDraw();
  do {
    if (!sdAppInfoOpen) {
      header("SD APP LIBRARY", "/apps  /  METADATA CATALOG", "UP/DOWN: SELECT   ENTER: DETAILS   R: RESCAN   ESC: BACK");
      text("CONTENT PACKAGES ONLY  /  NO EXECUTABLE CODE", 24, 105, true);
      for (int i = 0; i < 6 && sdAppFirst + i < sdAppPackageCount; ++i) {
        int index = sdAppFirst + i;
        int y = 153 + i * 47;
        bool selected = index == sdAppSelected;
        if (selected) epd.drawRoundRect(18, y - 27, 760, 39, 8, GxEPD_BLACK);
        text(sdAppPackages[index].name, 34, y, selected);
        text("v" + sdAppPackages[index].version, 540, y);
      }
      if (!sdAppPackageCount) text(cardOK ? "NO VALID PACKAGES FOUND" : "TF CARD NOT MOUNTED", 32, 180, true);
      text("PAGE: " + String(sdAppFirst / 6 + 1) + "/" + String(max(1, (sdAppPackageCount + 5) / 6)) + "   VALID: " + String(sdAppPackageCount) + "   INVALID: " + String(sdAppInvalidCount), 24, 430);
    } else {
      SdAppPackage &p = sdAppPackages[sdAppSelected];
      header("PACKAGE DETAILS", p.name, "ESC: BACK TO CATALOG");
      text("ID: " + p.id, 34, 125, true);
      text("VERSION: " + p.version, 34, 170);
      text("TYPE: CONTENT / RESOURCE PACK", 34, 215);
      text("PATH: " + p.path, 34, 260);
      text("DESCRIPTION:", 34, 315, true);
      text(p.description.length() ? clip(p.description, 78) : "NO DESCRIPTION", 34, 355);
      text("Package metadata is read-only in this release.", 34, 415);
    }
  } while (epd.nextPage());
}

void sdAppsOpen() {
  sdAppInfoOpen = false;
  sdAppScan();
  sdAppsDraw();
}

void sdAppsKey(uint8_t key) {
  if (key == 27 || key == 8) {
    if (sdAppInfoOpen) {
      sdAppInfoOpen = false;
      sdAppsDraw();
    } else {
      showMenu(331, HOME);
    }
    return;
  }
  if (sdAppInfoOpen) return;
  if (key == 'r' || key == 'R') {
    sdAppScan();
    sdAppsDraw();
    return;
  }
  if (key == 0xB5 || key == 'w' || key == 'W') {
    if (sdAppPackageCount) sdAppSelected = (sdAppSelected + sdAppPackageCount - 1) % sdAppPackageCount;
    sdAppFirst = (sdAppSelected / 6) * 6;
    sdAppsDraw();
  } else if (key == 0xB6 || key == 's' || key == 'S') {
    if (sdAppPackageCount) sdAppSelected = (sdAppSelected + 1) % sdAppPackageCount;
    sdAppFirst = (sdAppSelected / 6) * 6;
    sdAppsDraw();
  } else if ((key == 10 || key == 13) && sdAppPackageCount) {
    sdAppInfoOpen = true;
    sdAppsDraw();
  }
}
