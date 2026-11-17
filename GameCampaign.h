#pragma once
// Data-driven campaign helpers. The SD pack is authoritative; safe fallbacks
// keep the game playable when an older or incomplete card is inserted.
namespace JDM {
static const char *campaignServiceIds[4] = {
  "brake_service", "fuel_system", "cooling_system", "suspension"
};
static const char *campaignPartIds[4] = {
  "brake_pads", "fuel_pump", "radiator", "coilover_kit"
};
static const char *campaignFallbackCustomers[6] = {
  "Kenji Sato", "Mika Tanaka", "Daichi Mori",
  "Yui Nakamura", "Ren Kobayashi", "Akira Fujita"
};
static const int campaignFallbackPrices[4] = {950, 1250, 1450, 1750};
static const int campaignFallbackParts[4] = {180, 260, 390, 780};
static const int campaignFallbackRep[4] = {2, 3, 3, 4};

static bool campaignRead(const char *file, JsonDocument &doc) {
  if (!cardOK || SD_MMC.cardType() == CARD_NONE) return false;
  String path = String("/Games/JDM_Garage_Empire/content/") + file;
  File f = SD_MMC.open(path.c_str(), FILE_READ);
  if (!f || f.isDirectory() || !f.size() || f.size() > JDM_CONTENT_MAX_FILE) {
    if (f) f.close();
    return false;
  }
  bool ok = !deserializeJson(doc, f, DeserializationOption::NestingLimit(10));
  f.close();
  return ok && (doc["schema_version"] | 0) == JDM_CONTENT_SCHEMA;
}
static int campaignServiceValue(uint8_t fault, const char *field, int fallback) {
  fault = constrain(fault, 0, 3);
  JsonDocument doc;
  if (!campaignRead("services.json", doc)) return fallback;
  for (JsonObjectConst item : doc["services"].as<JsonArrayConst>()) {
    if (String(item["id"] | "") == campaignServiceIds[fault])
      return item[field] | fallback;
  }
  return fallback;
}
static int campaignPartValue(uint8_t fault, const char *field, int fallback) {
  fault = constrain(fault, 0, 3);
  JsonDocument doc;
  if (!campaignRead("parts.json", doc)) return fallback;
  for (JsonObjectConst item : doc["parts"].as<JsonArrayConst>()) {
    if (String(item["id"] | "") == campaignPartIds[fault])
      return item[field] | fallback;
  }
  return fallback;
}
String campaignCustomerName(uint8_t index) {
  index %= 6;
  JsonDocument doc;
  if (campaignRead("customers.json", doc)) {
    uint8_t i = 0;
    for (JsonObjectConst item : doc["customers"].as<JsonArrayConst>()) {
      if (i++ == index) return String(item["name"] | campaignFallbackCustomers[index]);
    }
  }
  return campaignFallbackCustomers[index];
}
int campaignCustomerValue(uint8_t index, const char *field, int fallback) {
  index %= 6;
  JsonDocument doc;
  if (campaignRead("customers.json", doc)) {
    uint8_t i = 0;
    for (JsonObjectConst item : doc["customers"].as<JsonArrayConst>()) {
      if (i++ == index) return item[field] | fallback;
    }
  }
  return fallback;
}
String campaignEventTitle(uint32_t day) {
  JsonDocument doc;
  if (!campaignRead("events.json", doc)) return "NORMAL WORKING DAY";
  JsonArrayConst events = doc["events"].as<JsonArrayConst>();
  if (events.isNull() || !events.size()) return "NORMAL WORKING DAY";
  uint16_t total = 0;
  for (JsonObjectConst item : events) total += max(1, (int)(item["weight"] | 1));
  uint16_t pick = (day * 7u + 3u) % total;
  for (JsonObjectConst item : events) {
    uint8_t weight = max(1, (int)(item["weight"] | 1));
    if (pick < weight) return String(item["title"] | "WORKSHOP NEWS");
    pick -= weight;
  }
  return "NORMAL WORKING DAY";
}
static int campaignEventEffect(uint32_t day, const char *effectName, int fallback) {
  JsonDocument doc;
  if (!campaignRead("events.json", doc)) return fallback;
  JsonArrayConst events = doc["events"].as<JsonArrayConst>();
  if (events.isNull() || !events.size()) return fallback;
  uint16_t total = 0;
  for (JsonObjectConst item : events) total += max(1, (int)(item["weight"] | 1));
  uint16_t pick = (day * 7u + 3u) % total;
  for (JsonObjectConst item : events) {
    uint8_t weight = max(1, (int)(item["weight"] | 1));
    if (pick < weight) return item["effect"][effectName] | fallback;
    pick -= weight;
  }
  return fallback;
}
int partCost() {
  uint8_t f = constrain((int)state.fault, 0, 3);
  int cost = campaignPartValue(f, "cost", campaignFallbackParts[f]);
  int discount = campaignEventEffect(state.day, "parts_cost_percent", 0);
  return max(1, cost * (100 - constrain(discount, -50, 90)) / 100);
}
int payment() {
  uint8_t f = constrain((int)state.fault, 0, 3);
  return campaignServiceValue(f, "base_price", campaignFallbackPrices[f]);
}
int campaignJobRep() {
  uint8_t f = constrain((int)state.fault, 0, 3);
  return campaignServiceValue(f, "rep", campaignFallbackRep[f]);
}
uint8_t campaignCustomerIndex() {
  // remaining stores the selected customer while a job is active (1..6).
  return state.remaining >= 1 && state.remaining <= 6 ? state.remaining - 1 : 0;
}
} // namespace JDM
