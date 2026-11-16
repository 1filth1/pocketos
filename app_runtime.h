#pragma once
#include <Arduino.h>
#include <esp_heap_caps.h>

// Transitional lifecycle layer for PocketOS's current single-address-space UI.
// A View is not yet an isolated process: leave/enter records ownership boundaries
// for diagnostics and provides the seam for future app resource hooks.
enum class AppRuntimeState : uint8_t { STOPPED, RUNNING };

struct AppRuntimeStats {
  uint32_t launches;
  uint32_t totalRunMs;
  uint32_t lastRunMs;
};

static bool appRuntimeInitialized = false;
static bool appRuntimeActive = false;
static View appRuntimeActiveView = HOME;
static AppRuntimeState appRuntimeState = AppRuntimeState::STOPPED;
static uint32_t appRuntimeEnteredAt = 0;
static uint32_t appRuntimeTransitionCount = 0;
static constexpr size_t APP_RUNTIME_STATS_CAPACITY = 64;
static AppRuntimeStats appRuntimeStats[APP_RUNTIME_STATS_CAPACITY] = {};

static const char *appRuntimeViewName(View v) {
  switch (v) {
    case HOME: return "HOME";
    case FILES: return "FILES";
    case VIEWER: return "VIEWER";
    case INPUT_VIEW: return "INPUT";
    case NOTE_NAME: return "NOTE_NAME";
    case NOTE_EDIT: return "NOTE_EDIT";
    case SYSTEM: return "SYSTEM";
    case AUDIO_TEST: return "AUDIO_TEST";
    case NET_PASS: return "NET_PASS";
    case TIME_SET: return "TIME_SET";
    case WEB_EDIT: return "WEB_EDIT";
    case WEB_PAGE: return "WEB_PAGE";
    case GAMES: return "GAMES";
    case SNAKE: return "SNAKE";
    case TETRIS: return "TETRIS";
    case SOKOBAN: return "SOKOBAN";
    case G2048: return "2048";
    case TAMAGOTCHI: return "TAMAGOTCHI";
    case INFO: return "INFO";
    case CALENDAR: return "CALENDAR";
    case CAL_TASKS: return "CAL_TASKS";
    case CAL_ADD: return "CAL_ADD";
    case CAL_DATE: return "CAL_DATE";
    case TIMER_APP: return "TIMER";
    case TIMER_EDIT: return "TIMER_EDIT";
    case PAPER_SETTINGS: return "DISPLAY_SETTINGS";
    case FILE_EXCHANGE: return "FILE_EXCHANGE";
    case RECORDER: return "RECORDER";
    case SEQUENCER: return "SEQUENCER";
    case JPEG_VIEW: return "JPEG_VIEW";
    case GPS_APP: return "GPS";
    case VIDEO_VIEW: return "VIDEO";
    case NOTES_LIST: return "NOTES";
    case BOOK_LIBRARY: return "BOOK_LIBRARY";
    case BOOK_READ: return "BOOK_READ";
    case GROOVE_BOX: return "GROOVE_BOX";
    case RAW_VIEW: return "RAW_VIEW";
    case BMP_VIEW: return "BMP_VIEW";
    case JDM_GAME: return "JDM_GAME";
    case SD_APPS: return "APP_MANAGER";
    case SD_APP_INFO: return "APP_MANAGER_DETAILS";
    default: return "UNKNOWN";
  }
}

// Heap snapshots are allocation-free and keep internal RAM separate from PSRAM.
static void appRuntimeLogMemory(const char *phase) {
  const uint32_t internalTotal = heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  const uint32_t internalFree = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  const uint32_t internalMin = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  const uint32_t internalLargest = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  const bool hasPsram = psramFound();
  const uint32_t psramTotal = hasPsram ? heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT) : 0;
  const uint32_t psramFree = hasPsram ? heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT) : 0;
  const uint32_t psramMin = hasPsram ? heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT) : 0;
  const uint32_t psramLargest = hasPsram ? heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT) : 0;
  Serial.printf("[MEM] %s internal_total=%lu internal_free=%lu internal_min=%lu internal_largest=%lu psram_total=%lu psram_free=%lu psram_min=%lu psram_largest=%lu\\n",
    phase, (unsigned long)internalTotal, (unsigned long)internalFree,
    (unsigned long)internalMin, (unsigned long)internalLargest,
    (unsigned long)psramTotal, (unsigned long)psramFree,
    (unsigned long)psramMin, (unsigned long)psramLargest);
}

static void appRuntimeLeave(const char *reason = "switch") {
  if (!appRuntimeActive || appRuntimeState != AppRuntimeState::RUNNING) return;
  const uint32_t duration = millis() - appRuntimeEnteredAt;
  const unsigned index = unsigned(appRuntimeActiveView);
  if (index < APP_RUNTIME_STATS_CAPACITY) {
    appRuntimeStats[index].lastRunMs = duration;
    appRuntimeStats[index].totalRunMs += duration;
  }
  Serial.printf("[APP] leave %s reason=%s duration_ms=%lu\\n",
    appRuntimeViewName(appRuntimeActiveView), reason, (unsigned long)duration);
  appRuntimeLogMemory("leave");
  appRuntimeState = AppRuntimeState::STOPPED;
  appRuntimeActive = false;
}

static void appRuntimeEnter(View next, const char *reason = "launch") {
  if (appRuntimeActive) appRuntimeLeave("switch");
  const uint32_t now = millis();
  appRuntimeActiveView = next;
  appRuntimeEnteredAt = now;
  appRuntimeState = AppRuntimeState::RUNNING;
  appRuntimeActive = true;
  const unsigned index = unsigned(next);
  uint32_t launches = 0;
  if (index < APP_RUNTIME_STATS_CAPACITY) launches = ++appRuntimeStats[index].launches;
  Serial.printf("[APP] enter %s reason=%s view=%u launches=%lu transitions=%lu\\n",
    appRuntimeViewName(next), reason, unsigned(next), (unsigned long)launches,
    (unsigned long)appRuntimeTransitionCount);
  appRuntimeLogMemory("enter");
}

static const AppRuntimeStats *appRuntimeGetStats(View v) {
  const unsigned index = unsigned(v);
  return index < APP_RUNTIME_STATS_CAPACITY ? &appRuntimeStats[index] : nullptr;
}

// Called from loop(); detects legacy view changes without changing navigation.
static void appRuntimeTick(View current) {
  if (!appRuntimeInitialized) {
    appRuntimeInitialized = true;
    appRuntimeEnter(current, "boot");
    return;
  }
  if (appRuntimeActive && current == appRuntimeActiveView) return;
  if (appRuntimeActive) appRuntimeLeave("view_change");
  ++appRuntimeTransitionCount;
  appRuntimeEnter(current, "view_change");
}
