#pragma once
#include <Arduino.h>

// Runtime transition monitor for the current single-address-space app model.
// This records view entry/exit and duration; it does not create process isolation.
static bool appRuntimeInitialized = false;
static View appRuntimeActiveView = HOME;
static uint32_t appRuntimeEnteredAt = 0;
static uint32_t appRuntimeTransitionCount = 0;

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
    default: return "UNKNOWN";
  }
}

static void appRuntimeTick(View current) {
  const uint32_t now = millis();
  if (!appRuntimeInitialized) {
    appRuntimeInitialized = true;
    appRuntimeActiveView = current;
    appRuntimeEnteredAt = now;
    Serial.printf("[APP] enter %s (view=%u)\n", appRuntimeViewName(current), unsigned(current));
    return;
  }
  if (current == appRuntimeActiveView) return;

  const uint32_t duration = now - appRuntimeEnteredAt;
  Serial.printf("[APP] leave %s after %lu ms\n",
    appRuntimeViewName(appRuntimeActiveView), (unsigned long)duration);
  appRuntimeActiveView = current;
  appRuntimeEnteredAt = now;
  ++appRuntimeTransitionCount;
  Serial.printf("[APP] enter %s (view=%u, transitions=%lu)\n",
    appRuntimeViewName(current), unsigned(current), (unsigned long)appRuntimeTransitionCount);
}
