#pragma once

namespace JDM {
// SD-native art pack. Firmware contains only the bounded decoder/cache.
// assets.bin is generated from the 1-bit source sprites and shipped in the
// matching SD-card archive; no PROGMEM artwork is copied to the card.
static constexpr uint32_t JDM_ASSET_PACK_BYTES = 43296;
static constexpr uint8_t JDM_CAR_ASSET_COUNT = 6;
static constexpr uint8_t JDM_PART_ASSET_COUNT = 12;
static constexpr uint32_t JDM_CAR_ASSET_BYTES = 3120;
static constexpr uint32_t JDM_PART_ASSET_BYTES = 2048;
static constexpr char JDM_ASSET_PATH[] = "/Games/JDM_Garage_Empire/assets/assets.bin";

uint8_t spriteCache[JDM_CAR_ASSET_BYTES];
uint8_t partCache[JDM_PART_ASSET_BYTES];
int cachedModel = -1;
int cachedPart = -1;
bool cachedFromSD = false;
bool partCachedFromSD = false;
String assetStatus = "SD ART NOT CHECKED";

static const uint32_t jdmCarOffsets[JDM_CAR_ASSET_COUNT] = {
  0, 3120, 6240, 9360, 12480, 15600
};
static const uint32_t jdmPartOffsets[JDM_PART_ASSET_COUNT] = {
  18720, 20768, 22816, 24864, 26912, 28960,
  31008, 33056, 35104, 37152, 39200, 41248
};

void prepareAssets() {
  cachedModel = cachedPart = -1;
  cachedFromSD = partCachedFromSD = false;
  if (!cardOK || SD_MMC.cardType() == CARD_NONE) {
    assetStatus = "SD CARD NOT AVAILABLE";
    return;
  }
  File f = SD_MMC.open(JDM_ASSET_PATH, FILE_READ);
  if (!f || f.isDirectory() || f.size() != JDM_ASSET_PACK_BYTES) {
    if (f) f.close();
    assetStatus = "ART PACK MISSING / INVALID";
    return;
  }
  f.close();
  assetStatus = "SD ART PACK READY";
}

bool loadCarAsset(int model) {
  model = constrain(model, 0, JDM_CAR_ASSET_COUNT - 1);
  if (cachedModel == model && cachedFromSD) return true;
  cachedModel = model;
  cachedFromSD = false;
  if (!cardOK) return false;
  File f = SD_MMC.open(JDM_ASSET_PATH, FILE_READ);
  if (!f || f.isDirectory() || f.size() != JDM_ASSET_PACK_BYTES) {
    if (f) f.close();
    assetStatus = "ART PACK MISSING / INVALID";
    return false;
  }
  bool ok = f.seek(jdmCarOffsets[model]) &&
            f.read(spriteCache, JDM_CAR_ASSET_BYTES) == JDM_CAR_ASSET_BYTES;
  f.close();
  cachedFromSD = ok;
  assetStatus = ok ? "CAR ART: SD CARD" : "CAR ART READ ERROR";
  return ok;
}

bool loadPartAsset(int part) {
  part = constrain(part, 0, JDM_PART_ASSET_COUNT - 1);
  if (cachedPart == part && partCachedFromSD) return true;
  cachedPart = part;
  partCachedFromSD = false;
  if (!cardOK) return false;
  File f = SD_MMC.open(JDM_ASSET_PATH, FILE_READ);
  if (!f || f.isDirectory() || f.size() != JDM_ASSET_PACK_BYTES) {
    if (f) f.close();
    assetStatus = "ART PACK MISSING / INVALID";
    return false;
  }
  bool ok = f.seek(jdmPartOffsets[part]) &&
            f.read(partCache, JDM_PART_ASSET_BYTES) == JDM_PART_ASSET_BYTES;
  f.close();
  partCachedFromSD = ok;
  assetStatus = ok ? "PART ART: SD CARD" : "PART ART READ ERROR";
  return ok;
}

void drawCarSprite(int model, int x, int y) {
  if (loadCarAsset(model)) {
    epd.drawBitmap(x, y, spriteCache, 240, 104, GxEPD_BLACK, GxEPD_WHITE);
  } else {
    epd.drawRect(x, y, 240, 104, GxEPD_BLACK);
    epd.setCursor(x + 12, y + 56);
    epd.print("CAR ART NOT FOUND");
  }
}

void drawPartSprite(int part, int x, int y) {
  if (loadPartAsset(part)) {
    epd.drawBitmap(x, y, partCache, 128, 128, GxEPD_BLACK, GxEPD_WHITE);
  } else {
    epd.drawRect(x, y, 128, 128, GxEPD_BLACK);
    epd.setCursor(x + 8, y + 68);
    epd.print("PART ART N/A");
  }
}
} // namespace JDM
