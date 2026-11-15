#pragma once
// Built-in applications share an address space. This registry is not a loader
// for SD executables and does not claim process isolation.
namespace PocketOS {
static constexpr const char *version = "0.1.2-rc.1";
struct Application { const char *id; const char *label; int action; };
static constexpr Application apps[] = {
 {"files","FILES",310}, {"notes","NOTES",311},
 {"calendar","CALENDAR",210}, {"timer","TIMER",211},
 {"recorder","RECORDER",295}, {"music","MUSIC",334},
 {"web","WEB",312}, {"exchange","EXCHANGE",314},
 {"games","GAMES",313}, {"gps","GPS",322},
 {"settings","SETTINGS",1900}, {"quick","QUICK",1901}
};
static constexpr size_t appCount = sizeof(apps)/sizeof(apps[0]);
static_assert(appCount == 12, "Launcher requires a 3 by 4 grid");
}
