#include <new>
#include <LittleFS.h>
#include <SPIFFS.h>
// New planner writes use TF only; legacy storage is read-only.
struct CalendarTask { uint32_t date; uint8_t done; char title[75]; };
struct CalendarStore { uint32_t version; uint32_t selectedDate; CalendarTask tasks[64]; };
static CalendarStore calendarStore;
static int calYear=2026,calMonth=10,calDay=27,calSelected=0;
struct ReminderMeta { uint16_t minute; uint8_t enabled, sound, fired; };
static ReminderMeta reminders[64];
static int calStage=0,calOption=0;
static uint16_t calMinute=540;
static bool calSound=true;
static String calTitle;
static String calInput,calMessage;
String reminderTime(int i){char b[8];snprintf(b,sizeof(b),"%02u:%02u",reminders[i].minute/60,reminders[i].minute%60);return String(b);}
bool calendarSave();
void reminderSave(){calendarSave();}
static bool calLoaded=false,calDelete=false;
static bool calSetCurrent=false,calDateFromHome=false;
void dateInputReturn(){bool toHome=calDateFromHome;calDateFromHome=false;calSetCurrent=false;if(toHome)home();else timeSetView();}
int monthDays(int y,int m){static const int days[]={31,28,31,30,31,30,31,31,30,31,30,31};return days[m-1]+(m==2&&y%4==0&&(y%100!=0||y%400==0));}
bool validDate(int y,int m,int d){return y>=2000&&y<=2099&&m>=1&&m<=12&&d>=1&&d<=monthDays(y,m);}
uint32_t calendarDate(){return calYear*10000+calMonth*100+calDay;}
String calendarDateText(){char b[16];snprintf(b,sizeof(b),"%04d-%02d-%02d",calYear,calMonth,calDay);return String(b);}
// Unified snapshot keeps task text and alarm metadata in sync.
struct PlannerStore { uint32_t version; CalendarStore calendar; ReminderMeta alarms[64]; };
// Compact version 2: omit empty slots and unused title bytes. Keep legacy
// keys intact until a new record has been written and verified.
static size_t plannerEncode(uint8_t *out){
 size_t n=0;out[n++]=2;uint32_t date=calendarStore.selectedDate;
 memcpy(out+n,&date,4);n+=4;size_t countAt=n++;out[countAt]=0;
 for(int i=0;i<64;i++)if(calendarStore.tasks[i].date){
  auto &t=calendarStore.tasks[i];auto &r=reminders[i];
  size_t len=strnlen(t.title,74);out[n++]=i;
  memcpy(out+n,&t.date,4);n+=4;out[n++]=t.done;
  memcpy(out+n,&r.minute,2);n+=2;
  out[n++]=r.enabled;out[n++]=r.sound;out[n++]=r.fired;
  out[n++]=len;memcpy(out+n,t.title,len);n+=len;out[countAt]++;
 }
 return n;
}
static bool plannerDecode(const uint8_t *in,size_t size){
 if(size<6||in[0]!=2||in[5]>64)return false;
 // Validate the whole record before changing live tasks.
 size_t n=6;uint64_t seen=0;
 for(int j=0;j<in[5];j++){
  if(n+12>size)return false;
  uint8_t slot=in[n],len=in[n+11];
  if(slot>=64||(seen&(1ULL<<slot))||len>74||n+12+len>size)return false;
  seen|=1ULL<<slot;n+=12+len;
 }
 if(n!=size)return false;
 memset(&calendarStore,0,sizeof(calendarStore));memset(reminders,0,sizeof(reminders));
 calendarStore.version=1;memcpy(&calendarStore.selectedDate,in+1,4);n=6;
 for(int j=0;j<in[5];j++){
  int slot=in[n++];auto &t=calendarStore.tasks[slot];auto &r=reminders[slot];
  memcpy(&t.date,in+n,4);n+=4;t.done=in[n++];
  memcpy(&r.minute,in+n,2);n+=2;r.enabled=in[n++];r.sound=in[n++];r.fired=in[n++];
  uint8_t len=in[n++];memcpy(t.title,in+n,len);t.title[len]=0;n+=len;
 }
 return true;
}
// Never format or erase storage while recovering the planner.
static fs::FS *plannerDisk=nullptr;
static const char *plannerMedium="UNAVAILABLE";
static const char *plannerFailure="NO WRITABLE STORAGE";
static bool plannerFS(){
 if(plannerDisk)return true;
 if(cardOK){plannerDisk=&SD_MMC;plannerMedium="TF CARD";}
 if(!plannerDisk){plannerFailure="TF CARD NOT MOUNTED";return false;}
 return true;
}
static bool plannerFileLoad(const char *path){
 File file=plannerDisk->open(path,"r");if(!file){plannerFailure="OPEN FILE FAILED";return false;}
 size_t size=file.size();if(size<6||size>5510){file.close();return false;}
 uint8_t *bytes=new(std::nothrow) uint8_t[size];
 bool ok=bytes&&file.read(bytes,size)==size&&plannerDecode(bytes,size);
 file.close();delete[] bytes;return ok;
}
static bool plannerFileSave(const uint8_t *bytes,size_t size,uint8_t *verify){
 if(!plannerFS())return false;
 plannerFailure="FILE ROTATION FAILED";
 const char *tmp="/planner.tmp",*live="/planner.bin",*backup="/planner.bak";
 File file=plannerDisk->open(tmp,"w");if(!file){plannerFailure="OPEN FILE FAILED";return false;}
 bool ok=file.write(bytes,size)==size;file.flush();file.close();
 file=plannerDisk->open(tmp,"r");
 ok=ok&&file&&file.size()==size&&file.read(verify,size)==size&&!memcmp(bytes,verify,size);
 file.close();if(!ok){plannerFailure="TEMP WRITE / READBACK FAILED";plannerDisk->remove(tmp);return false;}
 // Preserve the previous committed record across power loss during rotation.
 if(plannerDisk->exists(live)){
  // Do not replace a good backup with a damaged live file.
  File old=plannerDisk->open(live,"r");size_t oldSize=old?old.size():0;
  uint8_t *oldBytes=oldSize>=6&&oldSize<=5510?new(std::nothrow) uint8_t[oldSize]:nullptr;
  bool valid=false;
  if(oldBytes&&old.read(oldBytes,oldSize)==oldSize&&oldBytes[0]==2&&oldBytes[5]<=64){
   size_t n=6;uint64_t seen=0;valid=true;
   for(int j=0;j<oldBytes[5];j++){
    if(n+12>oldSize){valid=false;break;}
    uint8_t slot=oldBytes[n],len=oldBytes[n+11];
    if(slot>=64||(seen&(1ULL<<slot))||len>74||n+12+len>oldSize){valid=false;break;}
    seen|=1ULL<<slot;n+=12+len;
   }
   valid=valid&&n==oldSize;
  }
  old.close();delete[] oldBytes;
  if(valid){
   if(plannerDisk->exists(backup)&&!plannerDisk->remove(backup))return false;
   if(!plannerDisk->rename(live,backup))return false;
  }else if(!plannerDisk->remove(live))return false;
 }
 return plannerDisk->rename(tmp,live);
}
bool calendarSave(){
 calendarStore.version=1;calendarStore.selectedDate=calendarDate();
 uint8_t *packed=new(std::nothrow) uint8_t[5510];
 if(!packed){calMessage="SAVE FAILED - NOT ENOUGH MEMORY";return false;}
 size_t size=plannerEncode(packed);
 uint8_t *verify=new(std::nothrow) uint8_t[size];
 plannerFailure="NOT ENOUGH MEMORY";
 bool ok=verify&&plannerFileSave(packed,size,verify);
 Serial.printf("Planner save: %s (%u bytes, %s): %s\n",ok?"OK":"FAILED",(unsigned)size,plannerMedium,ok?"write/readback verified":plannerFailure);
 delete[] verify;delete[] packed;
 calMessage=ok?String("SAVED: ")+plannerMedium:String("SAVE FAILED: ")+plannerFailure;
 return ok;
}
int dayTasks(int day){int n=0;uint32_t date=calYear*10000+calMonth*100+day;for(auto &t:calendarStore.tasks)if(t.date==date)n++;return n;}
int taskIndex(int row){for(int i=0;i<64;i++)if(calendarStore.tasks[i].date==calendarDate()&&row--==0)return i;return -1;}
void calendarDraw(){view=CALENDAR;beginDraw();do{
 header("CALENDAR", "CALENDAR / "+calendarDateText(),"ARROWS: DATE  P/N: MONTH  ENTER: TASKS  T: TIME  ESC: HOME");
 text("MON   TUE   WED   THU   FRI   SAT   SUN",25,115,true);
 // Gregorian weekday; column zero is Monday.
 int y=calYear,m=calMonth;static const int offsets[]={0,3,2,5,0,3,5,1,4,6,2,4};if(m<3)y--;
 int start=(y+y/4-y/100+y/400+offsets[m-1]+1+6)%7;
 for(int d=1;d<=monthDays(calYear,calMonth);d++){
  int cell=start+d-1,x=22+(cell%7)*65,top=133+(cell/7)*45;bool on=d==calDay;
  if(on)epd.fillRect(x,top,60,39,GxEPD_BLACK);else epd.drawRect(x,top,60,39,GxEPD_BLACK);
  String label=String(d)+(dayTasks(d)?"*":"");if(on)inv(label,x+6,top+26);else text(label,x+6,top+26);
 }
 text("SELECTED DATE",505,125,true);text(calendarDateText(),505,160,true);
 text("TASKS: "+String(dayTasks(calDay)),505,202);text("* = HAS TASKS",505,240);
 text("D: TYPE DATE",505,290);text("T: SET TIME",505,325);text(clip(calMessage,66),22,435);
 }while(epd.nextPage());}
void calendarTasks(){menuScreen(CAL_TASKS);return;view=CAL_TASKS;int n=dayTasks(calDay);calSelected=n?min(calSelected,n-1):0;beginDraw();do{
 header("TASKS","TASKS / "+calendarDateText(),"UP/DOWN: SELECT  ENTER: DONE  A: ADD  D: DELETE  ESC: MONTH");
 if(!n)text("NO TASKS. PRESS A TO ADD.",30,130,true);
 int first=(calSelected/6)*6;
 for(int r=0;r<6&&first+r<n;r++){
  int row=first+r,i=taskIndex(row),top=91+r*53;auto &t=calendarStore.tasks[i];bool on=row==calSelected;
  if(on)epd.fillRect(20,top,760,47,GxEPD_BLACK);
  String line=String(t.done?"[X] ":"[ ] ")+clip(String(t.title),62);
  if(on)inv(line,30,top+29);else text(line,30,top+29);
 }
 text(clip(calMessage,68),25,435);
 if(calDelete){epd.fillRect(100,185,600,95,GxEPD_BLACK);inv("DELETE SELECTED TASK?",120,220);inv("Y: DELETE   N/ESC/BACKSPACE: CANCEL",120,258);}
 }while(epd.nextPage());}
void calendarInput(bool date){view=date?CAL_DATE:CAL_ADD;beginDraw();do{
 header("CALENDAR",date?(calSetCurrent?"SET CURRENT RTC DATE":"GO TO DATE"):"NEW TASK / "+calendarDateText(),"ENTER: SAVE   BACKSPACE: ERASE   ESC: CANCEL");
 text(date?"TYPE YYYYMMDD (2000-2099)":(calStage==0?"REMINDER TEXT (UP TO 74 CHARACTERS)":calStage==1?"TIME: HHMM (24 HOUR)":"NOTIFICATION OPTIONS"),25,125,true);
 if(!date&&calStage==2){
 text(String(calOption==0?"> ":"  ")+String(calSound?"[X] SOUND":"[ ] SOUND"),25,185,true);
 text(String(calOption==1?"> SAVE REMINDER":"  SAVE REMINDER"),25,240,true);
 text("UP/DOWN: SELECT    ENTER: TOGGLE / SAVE",25,350);
 }else {text(calInput.substring(0,60),25,185);text(calInput.substring(60),25,220);}
 text(date?(calSetCurrent?"Changes the date below the HOME clock.":"Calendar selection only. Does not change RTC."):"Keyboard text: Latin letters, numbers, punctuation.",25,300);text(clip(calMessage,68),25,400);
 }while(epd.nextPage());}
void calendarLoad(){
 if(!calLoaded){
  memset(reminders,0,sizeof(reminders));if(prefs.getBytesLength("reminders")==sizeof(reminders))prefs.getBytes("reminders",reminders,sizeof(reminders));
  memset(&calendarStore,0,sizeof(calendarStore));
  if(prefs.getBytesLength("calendar1")==sizeof(calendarStore))prefs.getBytes("calendar1",&calendarStore,sizeof(calendarStore));
  if(calendarStore.version!=1)memset(&calendarStore,0,sizeof(calendarStore));
  // Prefer the unified store; retain compatibility with existing saved tasks.
  SdSettings storage;bool migrated=true;
  if(storage.begin("planner",true)){
   PlannerStore *saved=new(std::nothrow) PlannerStore{};
   if(saved&&storage.getBytesLength("state")==sizeof(*saved)&&storage.getBytes("state",saved,sizeof(*saved))==sizeof(*saved)&&saved->version==1&&saved->calendar.version==1){
    calendarStore=saved->calendar;memcpy(reminders,saved->alarms,sizeof(reminders));migrated=false;
   }
   // Prefer compact storage, but keep the legacy record as a fallback.
   size_t packedSize=storage.getBytesLength("state2");
   if(packedSize>=6&&packedSize<=5510){
    uint8_t *packed=new(std::nothrow) uint8_t[packedSize];
    if(packed&&storage.getBytes("state2",packed,packedSize)==packedSize
       &&plannerDecode(packed,packedSize))migrated=false;
    delete[] packed;
   }
   // Never overwrite an existing planner if it could not be loaded.
   if(storage.isKey("state")||storage.isKey("state2"))migrated=false;
   delete saved;
   storage.end();
  }
  // Import old internal snapshots only when no SD planner exists.
  if(cardOK&&!SD_MMC.exists("/planner.bin")&&!SD_MMC.exists("/planner.bak")){
   fs::FS *old=nullptr;
   if(LittleFS.begin(false))old=&LittleFS;else if(SPIFFS.begin(false))old=&SPIFFS;
   if(old){plannerDisk=old;bool recovered=plannerFileLoad("/planner.bin")||plannerFileLoad("/planner.bak");plannerDisk=nullptr;if(recovered)migrated=true;}
  }
  // A committed filesystem snapshot supersedes the legacy NVS records.
  if(plannerFS()&&(plannerFileLoad("/planner.bin")||plannerFileLoad("/planner.bak")))migrated=false;
  for(auto &t:calendarStore.tasks)t.title[74]=0;
  uint32_t d=calendarStore.selectedDate;if(validDate(d/10000,(d/100)%100,d%100)){calYear=d/10000;calMonth=(d/100)%100;calDay=d%100;}
  uint8_t yy,mo,dd,hh,mm,ss;if(rtcRead(yy,mo,dd,hh,mm,ss)&&validDate(2000+yy,mo,dd)){calYear=2000+yy;calMonth=mo;calDay=dd;calMessage="DATE FROM RTC";}else calMessage="RTC DATE UNAVAILABLE - D: CHOOSE DATE";
  calLoaded=true;
  if(migrated||(cardOK&&!SD_MMC.exists("/planner.bin")&&!SD_MMC.exists("/planner.bak")))calendarSave();
 }
}
void openCalendar(){calendarLoad();calendarDraw();}
void calendarMove(int days){calDay+=days;if(calDay<1){if(--calMonth<1){calMonth=12;calYear--;}if(calYear<2000){calYear=2000;calMonth=1;calDay=1;}else calDay+=monthDays(calYear,calMonth);}else if(calDay>monthDays(calYear,calMonth)){calDay-=monthDays(calYear,calMonth);if(++calMonth>12){calMonth=1;calYear++;}if(calYear>2099){calYear=2099;calMonth=12;calDay=31;}}}
bool calendarKey(uint8_t k){
 if(view!=CALENDAR&&view!=CAL_TASKS&&view!=CAL_ADD&&view!=CAL_DATE)return false;
 if(view==CAL_ADD||view==CAL_DATE){bool date=view==CAL_DATE;
  if(k==27){if(date&&calSetCurrent){dateInputReturn();return true;}if(date)calendarDraw();else calendarTasks();return true;}
  if(!date&&calStage==2&&(k==0xB5||k==0xB6)){calOption=1-calOption;calendarInput(false);return true;}
  if(k==8){if(!date&&calStage==2){calStage=1;char b[5];snprintf(b,sizeof(b),"%02u%02u",calMinute/60,calMinute%60);calInput=b;calendarInput(false);return true;}if(calInput.length()){calInput.remove(calInput.length()-1);calendarInput(date);}return true;}
  if(k==10||k==13){
   if(date){int y=calInput.substring(0,4).toInt(),m=calInput.substring(4,6).toInt(),d=calInput.substring(6,8).toInt();if(calInput.length()!=8||!validDate(y,m,d)){calMessage="INVALID DATE";calendarInput(true);return true;}if(calSetCurrent){
    uint8_t yy,mo,dd,hh,mm,ss;
    if(!rtcRead(yy,mo,dd,hh,mm,ss)||!rtcSet(y-2000,m,d,hh,mm,ss)){calMessage="DATE NOT SAVED - CHECK RTC / SET TIME FIRST";calendarInput(true);return true;}
    uint8_t ry,rm,rd,rh,rmin,rs;
    if(!rtcRead(ry,rm,rd,rh,rmin,rs)||ry!=y-2000||rm!=m||rd!=d){calMessage="RTC DATE VERIFICATION FAILED";calendarInput(true);return true;}
    clockReadAt=0;shownClock="";status="RTC DATE SAVED AND VERIFIED";dateInputReturn();
   }else {calYear=y;calMonth=m;calDay=d;calendarSave();calendarDraw();}}
   else {
    if(calStage==0){calInput.trim();if(!calInput.length()){calMessage="ENTER REMINDER TEXT";calendarInput(false);return true;}calTitle=calInput;calInput="";calStage=1;calMessage="TYPE HHMM, E.G. 0930";calendarInput(false);return true;}
    if(calStage==1){int h=calInput.substring(0,2).toInt(),m=calInput.substring(2).toInt();if(calInput.length()!=4||h>23||m>59){calMessage="INVALID TIME - USE HHMM";calendarInput(false);return true;}calMinute=h*60+m;calStage=2;calOption=0;calMessage="";calendarInput(false);return true;}
    if(calOption==0){calSound=!calSound;calendarInput(false);return true;}
    int slot=-1;for(int i=0;i<64;i++)if(!calendarStore.tasks[i].date){slot=i;break;}
    if(slot<0){calMessage="64 TASK LIMIT";calendarInput(false);return true;}
    auto &t=calendarStore.tasks[slot];t.date=calendarDate();t.done=0;calTitle.toCharArray(t.title,sizeof(t.title));
    reminders[slot]={calMinute,1,(uint8_t)calSound,0};
    if(!calendarSave()){memset(&t,0,sizeof(t));memset(&reminders[slot],0,sizeof(reminders[slot]));calendarInput(false);return true;}
    calSelected=dayTasks(calDay)-1;calendarTasks();}return true;
  }
  if(k>=32&&k<=126&&calInput.length()<(date?8:calStage==1?4:74)&&(!date&&calStage==0||isDigit(k))&& (date||calStage<2)){calInput+=(char)k;calendarInput(date);}return true;
 }
 if(view==CAL_TASKS){
  if(calDelete){if(k=='y'||k=='Y'){int i=taskIndex(calSelected);if(i>=0){memset(&calendarStore.tasks[i],0,sizeof(CalendarTask));memset(&reminders[i],0,sizeof(reminders[i]));calendarSave();}calDelete=false;calendarTasks();}else if(k=='n'||k=='N'||k==8||k==27){calDelete=false;calendarTasks();}return true;}
  if(k==8||k==27){calendarDraw();return true;}
  if(k=='a'||k=='A'){calStage=0;calSound=true;calInput="";calMessage="";calendarInput(false);return true;}
  if((k==10||k==13)&&taskIndex(calSelected)>=0){auto &t=calendarStore.tasks[taskIndex(calSelected)];t.done=!t.done;calendarSave();calendarTasks();}
  else if((k=='d'||k=='D')&&dayTasks(calDay)){calDelete=true;calendarTasks();}
  else if(k==0xB5||k=='w'||k=='W'){if(calSelected>0){calSelected--;calendarTasks();}}
  else if(k==0xB6||k=='s'||k=='S'){if(calSelected+1<dayTasks(calDay)){calSelected++;calendarTasks();}}
  return true;
 }
 if(k==8||k==27){home();return true;}
 if(k==10||k==13){calSelected=0;calendarTasks();return true;}
 if(k=='t'||k=='T'){typed="";timeSetView();return true;}
 if(k=='d'||k=='D'){calSetCurrent=false;calInput="";calMessage="";calendarInput(true);return true;}
 if(k==0xB4)calendarMove(-1);else if(k==0xB7)calendarMove(1);else if(k==0xB5)calendarMove(-7);else if(k==0xB6)calendarMove(7);
 else if(k=='p'||k=='P'||k=='n'||k=='N'){int step=(k=='p'||k=='P')?-1:1;int month=calYear*12+calMonth-1+step;if(month<2000*12||month>2099*12+11)return true;calYear=month/12;calMonth=month%12+1;calDay=min(calDay,monthDays(calYear,calMonth));}
 else return true;
 calendarDraw();return true;
}
