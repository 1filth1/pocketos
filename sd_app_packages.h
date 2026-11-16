#pragma once

// SD content package catalogue and bounded, read-only resource browser.
// SD packages contain data only: this module never writes, extracts or executes files.
static constexpr uint8_t SD_APP_MAX_PACKAGES = 16;
static constexpr uint8_t SD_APP_MAX_SCAN_DIRS = 32;
static constexpr size_t SD_APP_MANIFEST_MAX_BYTES = 768;
static constexpr uint8_t SD_APP_MAX_RESOURCES = 24;
static constexpr uint8_t SD_APP_MAX_RESOURCE_SCAN = 64;
static constexpr size_t SD_APP_TEXT_MAX_BYTES = 4096;
static constexpr uint8_t SD_APP_TEXT_LINES_PER_PAGE = 8;

struct SdAppPackage {
  String id;
  String name;
  String version;
  String description;
  String type;
  String path;
};

struct SdAppResource {
  String name;
  String path;
  uint32_t size;
};

SdAppPackage sdAppPackages[SD_APP_MAX_PACKAGES];
SdAppResource sdAppResources[SD_APP_MAX_RESOURCES];
uint8_t sdAppPackageCount = 0;
uint8_t sdAppSelected = 0;
uint8_t sdAppFirst = 0;
bool sdAppInfoOpen = false;
bool sdAppResourcesOpen = false;
bool sdAppTextOpen = false;
uint8_t sdAppInvalidCount = 0;
uint8_t sdAppResourceCount = 0;
uint8_t sdAppResourceSelected = 0;
uint8_t sdAppResourceFirst = 0;
uint8_t sdAppResourceOverflow = 0;
String sdAppTextPath;
String sdAppTextName;
uint16_t sdAppTextPage = 0;
uint16_t sdAppTextPageCount = 1;

static bool sdAppSafeToken(const String &value, size_t maxLength) {
  if (!value.length() || value.length() > maxLength) return false;
  for (size_t i = 0; i < value.length(); ++i) {
    char c = value[i];
    if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_')) return false;
  }
  return true;
}

static bool sdAppSafeResourceName(const String &name) {
  if (!name.length() || name.length() > 64 || name == "." || name == ".." || name.startsWith(".")) return false;
  for (size_t i = 0; i < name.length(); ++i) {
    uint8_t c = (uint8_t)name[i];
    if (c < 32 || c == '/' || c == '\\' || c == ':' ) return false;
  }
  return true;
}

static bool sdAppTextExtension(const String &name) {
  String lower = name;
  lower.toLowerCase();
  return lower.endsWith(".txt") || lower.endsWith(".md") || lower.endsWith(".ini");
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
    int end = raw.indexOf('\\n', start);
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

static bool sdAppResourceBefore(const SdAppResource &a, const SdAppResource &b) {
  String an = a.name, bn = b.name;
  an.toLowerCase(); bn.toLowerCase();
  int cmp = an.compareTo(bn);
  return cmp ? cmp < 0 : a.name.compareTo(b.name) < 0;
}

static void sdAppResourcesLoad() {
  sdAppResourceCount = 0;
  sdAppResourceOverflow = 0;
  sdAppResourceSelected = 0;
  sdAppResourceFirst = 0;
  if (!cardOK || SD_MMC.cardType() == CARD_NONE) {
    status = "SD CARD NOT AVAILABLE";
    return;
  }
  if (sdAppSelected >= sdAppPackageCount || !sdAppSafeToken(sdAppPackages[sdAppSelected].id, 24)) {
    status = "INVALID PACKAGE";
    return;
  }
  String expected = sdAppPackages[sdAppSelected].path + "/resources";
  File dir = SD_MMC.open(expected.c_str(), FILE_READ);
  if (!dir || !dir.isDirectory()) {
    if (dir) dir.close();
    status = "NO RESOURCES DIRECTORY";
    return;
  }
  uint8_t inspectedEntries = 0;
  while (inspectedEntries < SD_APP_MAX_RESOURCE_SCAN) {
    File entry = dir.openNextFile();
    if (!entry) break;
    ++inspectedEntries;
    bool isDirectory = entry.isDirectory();
    String name = leaf(String(entry.name()));
    uint32_t size = entry.size();
    entry.close();
    // This first browser is deliberately flat: no recursive traversal or path joins
    // from untrusted directory entries. Subdirectories are ignored.
    if (isDirectory || !sdAppSafeResourceName(name)) continue;
    if (sdAppResourceCount >= SD_APP_MAX_RESOURCES) {
      if (sdAppResourceOverflow < 255) ++sdAppResourceOverflow;
      continue;
    }
    String path = expected + "/" + name;
    sdAppResources[sdAppResourceCount++] = {name, path, size};
  }
  if (inspectedEntries == SD_APP_MAX_RESOURCE_SCAN) {
    File probe = dir.openNextFile();
    if (probe) { if (sdAppResourceOverflow < 255) ++sdAppResourceOverflow; probe.close(); }
  }
  dir.close();
  for (int i = 1; i < sdAppResourceCount; ++i) {
    SdAppResource item = sdAppResources[i];
    int j = i;
    while (j > 0 && sdAppResourceBefore(item, sdAppResources[j - 1])) {
      sdAppResources[j] = sdAppResources[j - 1];
      --j;
    }
    sdAppResources[j] = item;
  }
  status = String(sdAppResourceCount) + " RESOURCE(S)";
  if (sdAppResourceOverflow) status += " / MORE HIDDEN";
}

static uint16_t sdAppCountTextLines(File &file) {
  uint16_t lines = 0;
  bool hasData = false;
  while (file.available()) {
    int c = file.read();
    if (c == '\\n') { ++lines; hasData = false; }
    else hasData = true;
  }
  if (hasData) ++lines;
  return max((uint16_t)1, lines);
}

static bool sdAppOpenText(const SdAppResource &resource) {
  if (!sdAppTextExtension(resource.name)) {
    status = "PREVIEW: TXT / MD / INI ONLY";
    return false;
  }
  if (resource.size > SD_APP_TEXT_MAX_BYTES) {
    status = "TEXT FILE TOO LARGE (MAX 4 KB)";
    return false;
  }
  File file = SD_MMC.open(resource.path.c_str(), FILE_READ);
  if (!file || file.isDirectory() || file.size() > SD_APP_TEXT_MAX_BYTES) {
    if (file) file.close();
    status = "CANNOT OPEN TEXT FILE";
    return false;
  }
  sdAppTextPath = resource.path;
  sdAppTextName = resource.name;
  sdAppTextPage = 0;
  sdAppTextPageCount = (sdAppCountTextLines(file) + SD_APP_TEXT_LINES_PER_PAGE - 1) / SD_APP_TEXT_LINES_PER_PAGE;
  file.close();
  sdAppTextOpen = true;
  status = "READ ONLY";
  return true;
}

static void sdAppTextDraw() {
  File file = SD_MMC.open(sdAppTextPath.c_str(), FILE_READ);
  beginDraw();
  do {
    header("RESOURCE PREVIEW", sdAppTextName, "UP/DOWN: PAGE   ESC: RESOURCES");
    if (!file || file.isDirectory() || file.size() > SD_APP_TEXT_MAX_BYTES) {
      text("FILE UNAVAILABLE OR TOO LARGE", 24, 150, true);
    } else {
      uint16_t skip = sdAppTextPage * SD_APP_TEXT_LINES_PER_PAGE;
      while (skip-- && file.available()) {
        while (file.available()) { int c = file.read(); if (c == '\\n') break; }
      }
      for (uint8_t row = 0; row < SD_APP_TEXT_LINES_PER_PAGE && file.available(); ++row) {
        String line;
        uint8_t consumed = 0;
        while (file.available()) {
          int c = file.read();
          if (c == '\\n') break;
          if (c != '\\r' && consumed < 112) line += (char)c;
          if (c != '\\r' && consumed < 255) ++consumed;
        }
        text(clip(line, 88), 20, 112 + row * 39);
      }
      text("PAGE " + String(sdAppTextPage + 1) + "/" + String(sdAppTextPageCount) + "  /  MAX 4 KB", 20, 430);
    }
  } while (epd.nextPage());
  if (file) file.close();
}

void sdAppsDraw() {
  view = sdAppInfoOpen ? SD_APP_INFO : SD_APPS;
  if (sdAppTextOpen) {
    sdAppTextDraw();
    return;
  }
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
    } else if (sdAppResourcesOpen) {
      SdAppPackage &p = sdAppPackages[sdAppSelected];
      header("PACKAGE RESOURCES", p.name, "UP/DOWN: SELECT   ENTER: OPEN   ESC: DETAILS   R: REFRESH");
      for (int i = 0; i < 6 && sdAppResourceFirst + i < sdAppResourceCount; ++i) {
        int index = sdAppResourceFirst + i;
        int y = 132 + i * 48;
        bool selected = index == sdAppResourceSelected;
        if (selected) epd.drawRoundRect(18, y - 25, 760, 39, 8, GxEPD_BLACK);
        text(sdAppResources[index].name, 32, y, selected);
        text(String(sdAppResources[index].size) + " B", 650, y);
      }
      if (!sdAppResourceCount) text(status == "NO RESOURCES DIRECTORY" ? status : "NO FILE RESOURCES FOUND", 28, 180, true);
      text("FILES: " + String(sdAppResourceCount) + " / LIMIT: " + String(SD_APP_MAX_RESOURCES) +
        (sdAppResourceOverflow ? " / EXTRA FILES OMITTED" : ""), 24, 430);
    } else {
      SdAppPackage &p = sdAppPackages[sdAppSelected];
      header("PACKAGE DETAILS", p.name, "ENTER: BROWSE RESOURCES   ESC: CATALOG");
      text("ID: " + p.id, 34, 125, true);
      text("VERSION: " + p.version, 34, 170);
      text("TYPE: CONTENT / RESOURCE PACK", 34, 215);
      text("PATH: " + p.path, 34, 260);
      text("DESCRIPTION:", 34, 315, true);
      text(p.description.length() ? clip(p.description, 78) : "NO DESCRIPTION", 34, 355);
      text("ENTER TO OPEN /resources", 34, 415);
    }
  } while (epd.nextPage());
}

void sdAppsOpen() {
  sdAppInfoOpen = false;
  sdAppResourcesOpen = false;
  sdAppTextOpen = false;
  sdAppScan();
  sdAppsDraw();
}

void sdAppsKey(uint8_t key) {
  if (key == 27 || key == 8) {
    if (sdAppTextOpen) {
      sdAppTextOpen = false;
      sdAppsDraw();
    } else if (sdAppResourcesOpen) {
      sdAppResourcesOpen = false;
      sdAppsDraw();
    } else if (sdAppInfoOpen) {
      sdAppInfoOpen = false;
      sdAppsDraw();
    } else {
      showMenu(331, HOME);
    }
    return;
  }

  if (sdAppTextOpen) {
    if (key == 0xB5 || key == 'w' || key == 'W') {
      if (sdAppTextPage) --sdAppTextPage;
      sdAppTextDraw();
    } else if (key == 0xB6 || key == 's' || key == 'S') {
      if (sdAppTextPage + 1 < sdAppTextPageCount) ++sdAppTextPage;
      sdAppTextDraw();
    }
    return;
  }

  if (sdAppResourcesOpen) {
    if (key == 'r' || key == 'R') {
      sdAppResourcesLoad();
      sdAppsDraw();
    } else if (key == 0xB5 || key == 'w' || key == 'W') {
      if (sdAppResourceCount) sdAppResourceSelected = (sdAppResourceSelected + sdAppResourceCount - 1) % sdAppResourceCount;
      sdAppResourceFirst = (sdAppResourceSelected / 6) * 6;
      sdAppsDraw();
    } else if (key == 0xB6 || key == 's' || key == 'S') {
      if (sdAppResourceCount) sdAppResourceSelected = (sdAppResourceSelected + 1) % sdAppResourceCount;
      sdAppResourceFirst = (sdAppResourceSelected / 6) * 6;
      sdAppsDraw();
    } else if ((key == 10 || key == 13) && sdAppResourceCount) {
      if (sdAppOpenText(sdAppResources[sdAppResourceSelected])) {
        sdAppTextDraw();
      } else {
        sdAppsDraw();
      }
    }
    return;
  }

  if (sdAppInfoOpen) {
    if ((key == 10 || key == 13) && sdAppPackageCount) {
      sdAppResourcesOpen = true;
      sdAppResourcesLoad();
      sdAppsDraw();
    }
    return;
  }

  if (key == 'r' || key == 'R') {
    sdAppScan();
    sdAppsDraw();
  } else if (key == 0xB5 || key == 'w' || key == 'W') {
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
