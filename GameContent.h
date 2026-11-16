#pragma once
#include <ArduinoJson.h>

namespace JDM {

// Runtime content is data-only. Files are bounded, parsed one at a time and
// validated before their values are exposed to the game.
static constexpr size_t JDM_CONTENT_MAX_FILE = 8192;
static constexpr uint8_t JDM_CONTENT_SCHEMA = 1;
static constexpr uint8_t JDM_CONTENT_CARS_MAX = 10;
static constexpr uint8_t JDM_CONTENT_RECORDS_MAX = 16;

String contentTitle = "JDM Garage Empire";
String contentVersion = "BUILT-IN";
String contentStatus = "CONTENT NOT LOADED";
String firstCar = "--", firstService = "--", firstPart = "--", firstCustomer = "--";
String firstUpgrade = "--", firstEvent = "--", firstTutorial = "--";
uint8_t contentCars = 0, contentServices = 0, contentParts = 0;
uint8_t contentCustomers = 0, contentUpgrades = 0, contentEvents = 0, contentTutorial = 0;
int contentStartingCash = 12000, contentStartingRep = 10, contentStartingDay = 1;
int contentDailyRent = 100, contentMechanicWage = 150;
bool contentLoaded = false;

static const char *jdmFallbackCars[JDM_CONTENT_CARS_MAX] = {
  "HACHI 86 / 1985", "KAGE R32 / 1990", "HAYATE S13 / 1991",
  "RYU A80 / 1994", "ROTARY FD / 1993", "MIDORI NS / 1992",
  "RAID EVO / 2001", "HOSHI STI / 2004", "CITY EK / 1998", "ROAD NA / 1990"
};

void resetContentDefaults() {
  for (uint8_t i = 0; i < JDM_CONTENT_CARS_MAX; ++i) cars[i] = jdmFallbackCars[i];
  contentTitle = "JDM Garage Empire";
  contentVersion = "BUILT-IN";
  contentStatus = "BUILT-IN FALLBACK";
  firstCar = firstService = firstPart = firstCustomer = "--";
  firstUpgrade = firstEvent = firstTutorial = "--";
  contentCars = contentServices = contentParts = contentCustomers = 0;
  contentUpgrades = contentEvents = contentTutorial = 0;
  contentStartingCash = 12000;
  contentStartingRep = 10;
  contentStartingDay = 1;
  contentDailyRent = 100;
  contentMechanicWage = 150;
  contentLoaded = false;
}

static bool jdmSafeId(const String &id) {
  if (!id.length() || id.length() > 40) return false;
  for (size_t i = 0; i < id.length(); ++i) {
    char c = id[i];
    if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
  }
  return true;
}

static bool jdmReadJson(const char *filename, JsonDocument &doc) {
  if (!cardOK || SD_MMC.cardType() == CARD_NONE) {
    contentStatus = "SD CARD NOT AVAILABLE";
    return false;
  }
  String path = String(root) + "/content/" + filename;
  File file = SD_MMC.open(path.c_str(), FILE_READ);
  if (!file || file.isDirectory()) {
    if (file) file.close();
    contentStatus = String("MISSING: ") + filename;
    return false;
  }
  size_t size = file.size();
  if (!size || size > JDM_CONTENT_MAX_FILE) {
    file.close();
    contentStatus = String("INVALID SIZE: ") + filename;
    return false;
  }
  DeserializationError error = deserializeJson(doc, file, DeserializationOption::NestingLimit(10));
  file.close();
  if (error) {
    contentStatus = String("JSON ERROR: ") + filename + " / " + error.c_str();
    return false;
  }
  if ((doc["schema_version"] | 0) != JDM_CONTENT_SCHEMA) {
    contentStatus = String("SCHEMA MISMATCH: ") + filename;
    return false;
  }
  return true;
}

enum JdmRecordKind : uint8_t {
  JDM_CAR, JDM_SERVICE, JDM_PART, JDM_CUSTOMER,
  JDM_UPGRADE, JDM_EVENT, JDM_TUTORIAL
};

static bool jdmRecordValid(JsonObjectConst item, JdmRecordKind kind) {
  const char *idValue = item["id"] | "";
  const char *nameValue = item["name"] | "";
  String id(idValue), name(nameValue);
  if (!jdmSafeId(id) || !name.length() || name.length() > 64) return false;

  switch (kind) {
    case JDM_CAR:
      return item["condition"].is<int>() && item["condition"].as<int>() >= 0 &&
        item["condition"].as<int>() <= 100 && item["value"].is<int>() &&
        item["value"].as<int>() > 0 && item["year"].is<int>();
    case JDM_SERVICE:
      return item["base_price"].is<int>() && item["base_cost"].is<int>() &&
        item["duration_days"].is<int>() && item["skill"].is<int>() && item["rep"].is<int>() &&
        item["base_price"].as<int>() >= item["base_cost"].as<int>() &&
        item["base_cost"].as<int>() >= 0 && item["duration_days"].as<int>() > 0 &&
        item["skill"].as<int>() >= 0 && item["rep"].as<int>() >= 0;
    case JDM_PART:
      return item["cost"].is<int>() && item["cost"].as<int>() > 0 &&
        item["quality"].is<int>() && item["quality"].as<int>() >= 1 &&
        item["quality"].as<int>() <= 5;
    case JDM_CUSTOMER:
      return item["patience_days"].is<int>() && item["patience_days"].as<int>() > 0 &&
        item["budget"].is<int>() && item["budget"].as<int>() > 0;
    case JDM_UPGRADE:
      return item["cost"].is<int>() && item["cost"].as<int>() > 0 &&
        item["rep_required"].is<int>() && item["rep_required"].as<int>() >= 0;
    case JDM_EVENT:
      return item["weight"].is<int>() && item["weight"].as<int>() > 0 &&
        item["duration_days"].is<int>() && item["duration_days"].as<int>() > 0;
    case JDM_TUTORIAL:
      return item["body"].is<const char*>();
  }
  return false;
}

static bool jdmLoadRecords(const char *filename, const char *arrayKey,
                           JdmRecordKind kind, uint8_t maxCount,
                           uint8_t &count, String &firstName) {
  JsonDocument doc;
  if (!jdmReadJson(filename, doc)) return false;
  JsonArrayConst records = doc[arrayKey].as<JsonArrayConst>();
  if (records.isNull() || !records.size() || records.size() > maxCount) {
    contentStatus = String("INVALID RECORD COUNT: ") + filename;
    return false;
  }

  String ids[JDM_CONTENT_RECORDS_MAX];
  uint8_t index = 0;
  for (JsonObjectConst item : records) {
    if (!jdmRecordValid(item, kind)) {
      contentStatus = String("INVALID RECORD: ") + filename + " #" + String(index + 1);
      return false;
    }
    String id = item["id"].as<const char*>();
    for (uint8_t prior = 0; prior < index; ++prior) {
      if (ids[prior] == id) {
        contentStatus = String("DUPLICATE ID: ") + filename + " / " + id;
        return false;
      }
    }
    ids[index] = id;
    if (index == 0) firstName = item["name"].as<const char*>();
    if (kind == JDM_CAR) cars[index] = item["name"].as<const char*>();
    ++index;
  }
  count = index;
  return true;
}

bool loadContent() {
  resetContentDefaults();
  if (!cardOK || SD_MMC.cardType() == CARD_NONE) return false;

  JsonDocument game;
  if (!jdmReadJson("game.json", game)) return false;
  if (String(game["game_id"] | "") != "jdm_garage_empire") {
    contentStatus = "WRONG GAME ID";
    return false;
  }
  const char *title = game["title"] | "";
  const char *version = game["content_version"] | "";
  JsonObjectConst start = game["starting_conditions"].as<JsonObjectConst>();
  JsonObjectConst economy = game["economy"].as<JsonObjectConst>();
  JsonArrayConst files = game["content_files"].as<JsonArrayConst>();
  if (!title[0] || strlen(title) > 48 || !version[0] || strlen(version) > 16 ||
      start.isNull() || economy.isNull() || files.size() != 7 ||
      !start["cash"].is<int>() || !start["reputation"].is<int>() ||
      !start["day"].is<int>() || !economy["daily_rent"].is<int>() ||
      !economy["mechanic_daily_wage"].is<int>()) {
    contentStatus = "INVALID GAME SETTINGS";
    return false;
  }
  int cash = start["cash"].as<int>();
  int rep = start["reputation"].as<int>();
  int day = start["day"].as<int>();
  int rent = economy["daily_rent"].as<int>();
  int wage = economy["mechanic_daily_wage"].as<int>();
  if (cash < 0 || cash > 100000000 || rep < 0 || rep > 100000 ||
      day < 1 || day > 100000 || rent < 0 || rent > 1000000 ||
      wage < 0 || wage > 1000000) {
    contentStatus = "GAME SETTINGS OUT OF RANGE";
    return false;
  }

  bool ok =
    jdmLoadRecords("cars.json", "cars", JDM_CAR, JDM_CONTENT_CARS_MAX, contentCars, firstCar) &&
    contentCars == JDM_CONTENT_CARS_MAX &&
    jdmLoadRecords("services.json", "services", JDM_SERVICE, JDM_CONTENT_RECORDS_MAX, contentServices, firstService) &&
    jdmLoadRecords("parts.json", "parts", JDM_PART, JDM_CONTENT_RECORDS_MAX, contentParts, firstPart) &&
    jdmLoadRecords("customers.json", "customers", JDM_CUSTOMER, JDM_CONTENT_RECORDS_MAX, contentCustomers, firstCustomer) &&
    jdmLoadRecords("upgrades.json", "upgrades", JDM_UPGRADE, JDM_CONTENT_RECORDS_MAX, contentUpgrades, firstUpgrade) &&
    jdmLoadRecords("events.json", "events", JDM_EVENT, JDM_CONTENT_RECORDS_MAX, contentEvents, firstEvent) &&
    jdmLoadRecords("tutorial.json", "chapters", JDM_TUTORIAL, JDM_CONTENT_RECORDS_MAX, contentTutorial, firstTutorial);
  if (!ok) {
    String failure = contentStatus;
    resetContentDefaults();
    contentStatus = failure;
    return false;
  }

  contentTitle = title;
  contentVersion = version;
  contentStartingCash = cash;
  contentStartingRep = rep;
  contentStartingDay = day;
  contentDailyRent = rent;
  contentMechanicWage = wage;
  contentLoaded = true;
  contentStatus = "SD CONTENT OK / " + contentVersion;
  return true;
}

void applyStartingConditions() {
  state.cash = contentStartingCash;
  state.reputation = constrain(contentStartingRep, 0, 65535);
  state.day = (uint32_t)contentStartingDay;
}

} // namespace JDM
