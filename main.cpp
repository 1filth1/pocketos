#include <esp_bt.h>
// Wiring review: existing GPS UART RX GPIO44 / TX GPIO43 is unchanged.
// Existing I2C GPIO41/42 connects only to the BSS138 3.3V LV side.
#include <Arduino.h>
#include <esp_timer.h>
#include <esp_sleep.h>
#include <SPI.h>
#include <Wire.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "sd_settings.h"
#include "audio_app.h"
#include "app_notifications.h"
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <gdem/GxEPD2_397_GDEM0397T81.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

static constexpr int EPD_BUSY=3,EPD_DC=9,EPD_CS=10,EPD_SCLK=11,EPD_MOSI=12,EPD_RST=46;
static constexpr int KB_ADDR=0x5F,KB_SDA=41,KB_SCL=42,RTC_ADDR=0x51,RTC_SDA=41,RTC_SCL=42,SHTC3_ADDR=0x70,SD_CMD=17,SD_CLK=16,SD_D0=15,SD_D1=7,SD_D2=8,SD_D3=18;
static constexpr int W=800,H=480,ROWS=6,MAX_FILES=60,MAX_NETS=12;
using Driver=GxEPD2_397_GDEM0397T81;
#include "epaper_ui.h"
PaperDisplay epd;
bool largeText=false;
SdSettings prefs;
struct Entry { String path,name; uint64_t size; bool dir; };
Entry files[MAX_FILES]; String nets[MAX_NETS],netNames[MAX_NETS];
enum View:uint8_t {HOME,FILES,VIEWER,INPUT_VIEW,NOTE_NAME,NOTE_EDIT,SYSTEM,AUDIO_TEST,NET_PASS,TIME_SET,WEB_EDIT,WEB_PAGE,GAMES,SNAKE,TETRIS,SOKOBAN,G2048,FIVE_ROW,TAMAGOTCHI,INFO,CALENDAR,CAL_TASKS,CAL_ADD,CAL_DATE,TIMER_APP,TIMER_EDIT,PAPER_SETTINGS,FILE_EXCHANGE,RECORDER,SEQUENCER,JPEG_VIEW,GPS_APP,VIDEO_VIEW,NOTES_LIST,BOOK_LIBRARY,BOOK_READ,GROOVE_BOX,RAW_VIEW,BMP_VIEW,JDM_GAME};
View view=HOME; bool cardOK=false,kbOK=false,rtcOK=false,deleting=false;
int chosen=0,selected=0,first=0,count=0,textTop=0,netCount=0,netSelected=0,gameSelected=0;
String folder="/",textFile,status="READY",typed="",pageText="",url="http://example.com",wifiName="",infoName="",notePath="",noteText="";
uint8_t escState=0; unsigned long escAt=0,lastTick=0; bool firstPaint=true, time24=true; String shownClock="", cachedClock="--:--",cachedEnv="T --.-C H --%"; unsigned long clockReadAt=0,envReadAt=0;
int snakeX[80],snakeY[80],snakeLen=4,snakeDX=1,snakeDY=0,snakeFoodX=12,snakeFoodY=7,snakeScore=0;
int tetX=4,tetY=0,tetScore=0;  uint8_t tamaHunger=25,tamaHappy=75,tamaEnergy=75; unsigned long tamaLast=0,tamaSaved=0; int b2048[4][4]; int score2048=0;
String clip(const String&s,int n){return s.length()<=n?s:s.substring(0,n-3)+"...";} String leaf(const String&s){int p=s.lastIndexOf('/');return p<0?s:s.substring(p+1);} String parent(const String&s){int p=s.lastIndexOf('/');return p<=0?"/":s.substring(0,p);}
uint8_t bcdToDec(uint8_t v){return (v>>4)*10+(v&15);} uint8_t decToBcd(uint8_t v){return ((v/10)<<4)|(v%10);}
bool rtcRead(uint8_t &yy,uint8_t &mo,uint8_t &dd,uint8_t &hh,uint8_t &mm,uint8_t &ss){if(!rtcOK)return false;Wire.beginTransmission(RTC_ADDR);Wire.write(4);if(Wire.endTransmission(false)!=0||Wire.requestFrom(RTC_ADDR,7)!=7)return false;uint8_t rawSec=Wire.read();if(rawSec&0x80)return false;ss=bcdToDec(rawSec&0x7F);mm=bcdToDec(Wire.read()&0x7F);hh=bcdToDec(Wire.read()&0x3F);dd=bcdToDec(Wire.read()&0x3F);Wire.read();mo=bcdToDec(Wire.read()&0x1F);yy=bcdToDec(Wire.read());return true;}
bool rtcSet(uint8_t yy,uint8_t mo,uint8_t dd,uint8_t hh,uint8_t mm,uint8_t ss){Wire.beginTransmission(RTC_ADDR);Wire.write(0);Wire.write(0x00);if(Wire.endTransmission()!=0)return false;Wire.beginTransmission(RTC_ADDR);Wire.write(4);Wire.write(decToBcd(ss));Wire.write(decToBcd(mm));Wire.write(decToBcd(hh));Wire.write(decToBcd(dd));Wire.write(0);Wire.write(decToBcd(mo));Wire.write(decToBcd(yy));return Wire.endTransmission()==0;}
String rtcTime(){
  // The RTC and CardKB2 share one I2C bus.  Reading the RTC once per second
  // leaves the bus free for key presses instead of issuing 50 transactions/s.
  if(clockReadAt && millis()-clockReadAt<1000)return cachedClock;
  clockReadAt=millis();
  uint8_t yy,mo,dd,hh,mm,ss;
  if(!rtcRead(yy,mo,dd,hh,mm,ss)){cachedClock="--:--";return cachedClock;}
  char out[12];
  if(time24) snprintf(out,sizeof(out),"%02u:%02u",hh,mm);
  else { uint8_t shownHour=hh%12; if(!shownHour) shownHour=12; snprintf(out,sizeof(out),"%02u:%02u %s",shownHour,mm,hh>=12?"PM":"AM"); }
  cachedClock=String(out);
  return cachedClock;
}
U8G2_FOR_ADAFRUIT_GFX uiUnicode;
void uiPrint(const String &s,int x,int y,bool bold,bool inverted){
  // Keep the compact status and shortcut bars; enlarge application contents.
  bool large=largeText&&y>=80&&y<448&&!((view==HOME&&y==165)||(view==TIMER_APP&&y==295));
  epd.setFont(large?(bold?&FreeSansBold12pt7b:&FreeSans12pt7b):(bold?&FreeMonoBold9pt7b:&FreeMono9pt7b));
  epd.setTextColor(inverted?GxEPD_WHITE:GxEPD_BLACK);
  int right=W-10;
  if(view==HOME&&y>=116&&y<430&&x<398)right=389;
  if(view==FILES&&y>=82&&y<420&&x<398)right=388;
  bool unicode=false;for(unsigned i=0;i<s.length();i++)if((uint8_t)s[i]>=128){unicode=true;break;}
  if(unicode){
    uiUnicode.begin(epd);uiUnicode.setFontMode(1);
    uiUnicode.setForegroundColor(inverted?GxEPD_WHITE:GxEPD_BLACK);
    uiUnicode.setFont(large?u8g2_font_10x20_t_cyrillic:u8g2_font_unifont_t_cyrillic);
    String fitted=s;int cells=max(0,(right-x)/(large?10:16)),count=0;
    unsigned end=0;while(end<fitted.length()&&count<cells){end++;while(end<fitted.length()&&((uint8_t)fitted[end]&192)==128)end++;count++;}
    fitted=fitted.substring(0,end);uiUnicode.setCursor(x,y);uiUnicode.print(fitted);return;
  }
  String fitted=s;int16_t bx,by;uint16_t bw,bh;
  epd.getTextBounds(fitted,x,y,&bx,&by,&bw,&bh);
  while(fitted.length()&&bx+bw>right){fitted.remove(fitted.length()-1);epd.getTextBounds(fitted,x,y,&bx,&by,&bw,&bh);}
  epd.setCursor(x,y);epd.print(fitted);epd.setTextColor(GxEPD_BLACK);
}
void text(const String&s,int x,int y,bool b=false){uiPrint(s,x,y,b,false);}
void inv(const String&s,int x,int y){uiPrint(s,x,y,true,true);}
// Text-sized outline capsule: same font and ink for selected and idle rows.
void menuOvalLabel(String label,int x,int baseline,bool selected){
  epd.setFont(largeText?&FreeSansBold12pt7b:&FreeMonoBold9pt7b);
  int16_t bx,by;uint16_t bw,bh;
  epd.getTextBounds(label,x,baseline,&bx,&by,&bw,&bh);
  while(label.length()&&bx+bw+12>772){
    label.remove(label.length()-1);
    epd.getTextBounds(label,x,baseline,&bx,&by,&bw,&bh);
  }
  if(selected)epd.drawRoundRect(bx-10,by-7,bw+20,bh+14,(bh+14)/2,GxEPD_BLACK);
  text(label,x,baseline,true);
}
// Each SHTC3 measurement has two data bytes followed by its own CRC byte.
bool shtc3Crc(const uint8_t *data,uint8_t expected){uint8_t crc=0xFF;for(int i=0;i<2;i++){crc^=data[i];for(int bit=0;bit<8;bit++)crc=(crc&0x80)?(crc<<1)^0x31:crc<<1;}return crc==expected;}
String environment(){
  if(envReadAt && millis()-envReadAt<10000)return cachedEnv;
  envReadAt=millis();
  Wire.beginTransmission(SHTC3_ADDR);Wire.write(0x78);Wire.write(0x66);
  if(Wire.endTransmission()!=0){cachedEnv="T --.-C H --%";return cachedEnv;}
  delay(15);
  if(Wire.requestFrom(SHTC3_ADDR,(size_t)6)!=6){cachedEnv="T --.-C H --%";return cachedEnv;}
  uint8_t raw[6];for(int i=0;i<6;i++)raw[i]=Wire.read();
  if(!shtc3Crc(raw,raw[2])||!shtc3Crc(raw+3,raw[5])){cachedEnv="T --.-C H --%";return cachedEnv;}
  uint16_t rt=(uint16_t(raw[0])<<8)|raw[1],rh=(uint16_t(raw[3])<<8)|raw[4];
  float temp=-45.0f+175.0f*rt/65535.0f,humidity=100.0f*rh/65535.0f;
  char out[24];snprintf(out,sizeof(out),"T %.1fC H %.0f%%",temp,humidity);cachedEnv=String(out);return cachedEnv;
}
// Register map follows Waveshare's axpPower/XPowersAXP2101 driver.
// Avoid library begin(): it disables temperature sensing as a side effect.
// Only battery detection and voltage measurement are enabled here; charger
// currents, target voltage, protection, power rails and fuel gauge stay intact.
String batteryValue="--",chargeValue="--";
int batteryPercent=-1,batteryMillivolts=-1;
bool batteryUsb=false;
unsigned long powerReadAt=0;
bool powerStarted=false;
int powerReg(uint8_t reg){
  Wire.beginTransmission(0x34);Wire.write(reg);
  if(Wire.endTransmission(false)!=0||Wire.requestFrom(uint8_t(0x34),uint8_t(1))!=1)return -1;
  return Wire.read();
}
bool powerEnableBit(uint8_t reg){
  int old=powerReg(reg);if(old<0)return false;
  if(old&1)return true;
  Wire.beginTransmission(0x34);Wire.write(reg);Wire.write(uint8_t(old|1));
  return Wire.endTransmission()==0;
}
// PWR is wired to the PMIC, not an ESP32 GPIO. Poll its latched IRQ.
// Source: Waveshare axpPower/XPowersAXP2101 and REG/AXP2101Constants.
bool pwrKeyReady=false;
bool powerWrite(uint8_t reg,uint8_t value){
  Wire.beginTransmission(0x34);Wire.write(reg);Wire.write(value);
  return Wire.endTransmission()==0;
}
bool powerMask(uint8_t reg,uint8_t mask,uint8_t value){
  int old=powerReg(reg);
  return old>=0 && powerWrite(reg,(uint8_t(old)&~mask)|(value&mask));
}
void initPowerKey(){
  if(powerReg(0x03)!=0x4A)return;
  // 2 s long-press IRQ; 10 s hardware fallback allows the screen to finish.
  // Preserve power-on timing, rails, charging settings and other IRQs.
  pwrKeyReady=powerMask(0x27,0x3C,0x2C)
    && powerMask(0x22,0x03,0x02)
    && powerWrite(0x49,0x04) && powerMask(0x41,0x04,0x04);
}
void pollPowerKey();
// Fail closed: an I2C error or any valid VBUS prevents automatic shutdown.
// Waveshare XPowersAXP2101: STATUS1 bit 3 = battery, bit 5 = VBUS good.
bool batteryOnlyPower(){
  if(powerReg(0x03)!=0x4A)return false;
  int state=powerReg(0x00);
  return state>=0 && (state&0x08) && !(state&0x20);
}
void readPower(){
  if(powerReadAt && millis()-powerReadAt<5000)return;
  powerReadAt=millis();batteryValue="--";chargeValue="--";
  batteryPercent=-1;batteryMillivolts=-1;batteryUsb=false;
  if(powerReg(0x03)!=0x4A){powerStarted=false;return;}
  if(!powerStarted){
    powerStarted=powerEnableBit(0x68)&&powerEnableBit(0x30);
    return; // Give battery detection and ADC time to settle.
  }
  int a=powerReg(0x00),b=powerReg(0x01);
  if(a<0||b<0)return;
  batteryUsb=(a&32)!=0;
  if(!(a&8)){batteryValue="NOT FOUND";chargeValue="NO BATTERY";return;}
  int percent=powerReg(0xA4),hi=powerReg(0x34),lo=powerReg(0x35);
  batteryPercent=(percent>=0&&percent<=100)?percent:-1;
  String level=(percent>=0&&percent<=100)?String(percent)+"%":String("--%");
  if(hi>=0&&lo>=0){
    int mv=((hi&31)<<8)|lo;
    if(mv>=2000&&mv<=5000){batteryMillivolts=mv;char v[12];snprintf(v,sizeof(v)," %.2fV",mv/1000.0f);level+=v;}
  }
  batteryValue=level;
  bool usb=(a&32)&&!(b&8);
  if((b>>5)==1)chargeValue="CHARGING";
  else if(usb&&(b&7)==4)chargeValue="FULL";
  else chargeValue=usb?"USB / IDLE":"ON BATTERY";
}
bool knownWifiPassword(const String &name,String &password);
bool wifiWanted=false,wifiSuspended=false;
unsigned long wifiAttemptAt=0;
String wifiState(){
  if(!psramFound())return "OFF / PSRAM ERROR";
  if(WiFi.getMode()==WIFI_OFF)return "OFF";
  if(WiFi.status()==WL_CONNECTED && uint32_t(WiFi.localIP())!=0)return "CONNECTED";
  if(WiFi.status()==WL_CONNECTED)return "NO IP";
  if(wifiWanted && millis()-wifiAttemptAt<12000)return "CONNECTING";
  return "DISCONNECTED";
}
// Do not enter the network stack with unavailable external memory. The
// observed failed task allocations ended in abort(), not a recoverable error.
bool startWifiSafely(){
  if(!psramFound() || heap_caps_get_free_size(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)<100000){
    wifiWanted=false;
    status=!psramFound()?"WIFI OFF: PSRAM ERROR":"WIFI OFF: LOW MEMORY";
    Serial.println(status);
    return false;
  }
  if(!WiFi.mode(WIFI_STA)){wifiWanted=false;status="WIFI START FAILED";return false;}
  WiFi.setAutoReconnect(true);
  return true;
}
void resumeSavedWifi(){
  wifiWanted=true;wifiAttemptAt=millis();
  if(!startWifiSafely())return;
  String name=prefs.getString("lastWifi",""),password;
  if(name.length() && knownWifiPassword(name,password))WiFi.begin(name.c_str(),password.c_str());
}
String stat(){readPower();return "BAT "+batteryValue+" CHG "+chargeValue+" "+environment()+" TIME "+rtcTime()+" WIFI "+wifiState()+" BT OFF TF "+String(cardOK?"OK":"NOT FOUND");}
void drawTopStatus(const String &value){
  (void)value;
  // Status lives on display 2; no top strip on the main display.
}
void header(const String&,const String&sub,const String&keys){epd.fillScreen(GxEPD_WHITE);drawTopStatus(stat());text(sub,16,56);epd.drawLine(0,78,W,78,GxEPD_BLACK);epd.fillRect(0,452,W,28,GxEPD_BLACK);inv(keys,10,472);} void beginDraw(){epd.contentTop=view==HOME?82:32;epd.contentHeight=view==HOME?348:448;if(firstPaint){epd.setFullWindow();firstPaint=false;}else epd.setPartialWindow(0,0,W,H);epd.firstPage();}
String todaySnapshot();
void drawToday();
void refreshToday();
void refreshPowerCells(){} // Today owns its panel and refreshes changed values only.
void refreshHomeGrid();
void refreshClock(){
  if(view==HOME){refreshHomeGrid();return;}
  String now=wifiState();
  if(now==shownClock)return;
  shownClock=now;
  epd.setPartialWindow(0,0,W,32);epd.firstPage();
  do{drawTopStatus(now);}while(epd.nextPage());
}
void menuScreen(View v);
const char* menuNames[]={"QUICK MENU","APPLICATIONS","SETTINGS"};
// Fixed grid: aligned window edges, title bars and evenly spaced rows.
void rememberActivity(const String &kind,const String &path,int position){
 if(prefs.getString("resumeKind","")!=kind)prefs.putString("resumeKind",kind);
 if(prefs.getString("resumePath","")!=path)prefs.putString("resumePath",path);
 if(prefs.getInt("resumePos",-1)!=position)prefs.putInt("resumePos",position);
}
void showMenu(int c,View v);
void home(){showMenu(300,HOME);}
String appResult="";
int evalSimple(const String &s,bool &ok){
  String t=s; t.replace(" ",""); int op=-1; char sign=0;
  for(int i=1;i<(int)t.length();i++) if(t[i]=='+'||t[i]=='-'||t[i]=='*'||t[i]=='/'){op=i;sign=t[i];break;}
  if(op<1){ok=false;return 0;} int a=t.substring(0,op).toInt(),b=t.substring(op+1).toInt();
  if((sign=='/'&&b==0)||t.substring(op+1).length()==0){ok=false;return 0;}
  ok=true; if(sign=='+')return a+b;if(sign=='-')return a-b;if(sign=='*')return a*b;return a/b;
}
void terminalRun(){
  String c=typed; c.trim(); String u=c;u.toUpperCase(); appResult="";
  if(u=="DIR"){if(!cardOK)appResult="TF CARD NOT MOUNTED";else{File d=SD_MMC.open(folder.c_str());int n=0;while(d&&n<7){File f=d.openNextFile();if(!f)break;appResult+=(f.isDirectory()?"<DIR> ":"      ")+leaf(String(f.name()))+"\\n";f.close();n++;}if(d)d.close();if(!appResult.length())appResult="EMPTY";}}
  else if(u=="CD .."){if(cardOK){folder=parent(folder);appResult="NOW "+folder;}}
  else if(u.startsWith("CD ")){String p=c.substring(3);if(!p.startsWith("/"))p=folder+(folder=="/"?"":"/")+p;File d=cardOK?SD_MMC.open(p.c_str()):File();if(d&&d.isDirectory()){folder=p;appResult="NOW "+folder;d.close();}else{if(d)d.close();appResult="FOLDER NOT FOUND";}}
  else if(u.startsWith("TYPE ")){String p=c.substring(5);if(!p.startsWith("/"))p=folder+(folder=="/"?"":"/")+p;File f=cardOK?SD_MMC.open(p.c_str()):File();if(f&&!f.isDirectory()){appResult=clip(f.readStringUntil('\n'),78);f.close();}else{if(f)f.close();appResult="FILE NOT FOUND";}}
  else if(u.startsWith("DEL ")){String p=c.substring(4);if(!p.startsWith("/"))p=folder+(folder=="/"?"":"/")+p;File f=cardOK?SD_MMC.open(p.c_str()):File();if(f&&f.isDirectory())appResult="REFUSES TO DELETE FOLDERS";else if(f&&SD_MMC.remove(p.c_str()))appResult="FILE DELETED";else appResult="FILE NOT FOUND";if(f)f.close();}
  else if(u=="HELP")appResult="DIR  CD name  CD ..  TYPE file  DEL file";
  else appResult="UNKNOWN COMMAND - TYPE HELP";
  typed="";
}
String display2Status();
void infoView(){view=INFO;beginDraw();do{
  if(infoName=="NOTES"||infoName=="EDITOR"){header(infoName,"saved on TF card","TYPE: WRITE   ENTER: NEW LINE   S: SAVE   ESC: HOME");String note=prefs.getString("notes","");text("SAVED NOTE:",35,112,true);int pos=0;for(int row=0;row<5&&pos<(int)note.length();row++){int e=note.indexOf('\n',pos);if(e<0)e=note.length();text(clip(note.substring(pos,e),82),35,145+row*30);pos=e+1;}text("EDIT: "+clip(typed,74),35,330,true);text(status,35,405);}
  else if(infoName=="CALC"){header("CALCULATOR","integer arithmetic","TYPE e.g. 12*7   ENTER: CALCULATE   ESC: HOME");text("EXPRESSION: "+(typed.length()?typed:"--"),40,140,true);text("RESULT: "+(appResult.length()?appResult:"--"),40,205,true);text("SUPPORTED: +  -  *  /",40,275);}
  else if(infoName=="TERMINAL"){header("TERMINAL",String("TF ")+(cardOK?folder:"NOT MOUNTED"),"TYPE COMMAND  ENTER: RUN  ESC: CANCEL");text("> "+typed,35,125,true);int pos=0;for(int row=0;row<7&&pos<(int)appResult.length();row++){int e=appResult.indexOf('\n',pos);if(e<0)e=appResult.length();text(clip(appResult.substring(pos,e),82),35,170+row*32);pos=e+1;}text("HELP: DIR, CD, TYPE, DEL",35,424);}
  else if(infoName=="MONITOR"){header("MONITOR","live board status","ESC/BACKSPACE: HOME");text("UPTIME: "+String(millis()/1000)+" s",40,125,true);text("FREE HEAP: "+String(ESP.getFreeHeap())+" bytes",40,165);text(String("WIFI: ")+(wifiState()+String(WiFi.status()==WL_CONNECTED?" "+WiFi.SSID():"")),40,205);text(String("TF CARD: ")+(cardOK?"MOUNTED":"NOT MOUNTED"),40,245);text(String("KEYBOARD: ")+(kbOK?"READY":"NOT FOUND"),40,285);text(String("RTC: ")+(rtcOK?rtcTime():"NOT FOUND"),40,325);text("MIC LEVEL: NOT TESTED (NO CONFIRMED AUDIO PINOUT)",40,375);}
  else {header("STATUS","honest hardware state","ESC/BACKSPACE: HOME");text(String("DISPLAY: READY    TF: ")+(cardOK?"MOUNTED":"NOT MOUNTED"),40,130,true);text(String("CLOCK: ")+(rtcOK?rtcTime():"RTC NOT FOUND"),40,175);text(String("WIFI: ")+(wifiState()+String(WiFi.status()==WL_CONNECTED?" "+WiFi.SSID():"")),40,220);text(String("KEYBOARD: ")+(kbOK?"READY":"NOT FOUND"),40,265);text("CODEC: "+String(codecReady?"INITIALIZED":"NOT FOUND / INIT FAILED"),40,305);text("SPEAKER: "+clip(audioResult,49),40,340);text("MIC: "+clip(micResult,53),40,375);text("DISPLAY 2: "+display2Status(),40,410);}
}while(epd.nextPage());}
// Directories first; alphabetical names within each group.
static bool fileEntryBefore(const Entry &a,const Entry &b){
 if(a.dir!=b.dir)return a.dir;
 String an=a.name,bn=b.name;an.toLowerCase();bn.toLowerCase();
 int cmp=an.compareTo(bn);
 return cmp?cmp<0:a.name.compareTo(b.name)<0;
}
static void sortFolderEntries(){
 for(int i=1;i<count;i++){
  Entry item=files[i];int j=i;
  while(j>0&&fileEntryBefore(item,files[j-1])){files[j]=files[j-1];--j;}
  files[j]=item;
 }
}
bool loadFolder(const String&path){File d=SD_MMC.open(path.c_str());if(!d||!d.isDirectory()){if(d)d.close();status="CANNOT OPEN FOLDER";return false;}count=0;File f=d.openNextFile();while(f&&count<MAX_FILES){String name=leaf(String(f.name()));String p=path=="/"?"/"+name:path+"/"+name;files[count]={p,name,f.size(),f.isDirectory()};if(!files[count].path.startsWith("/"))files[count].path="/"+files[count].path;f.close();count++;f=d.openNextFile();}d.close();sortFolderEntries();folder=path;selected=first=0;status=String(count)+" ITEM(S)";return true;}
void fileManagerDraw();
void manager(){fileManagerDraw();}
void viewer(){File f=SD_MMC.open(textFile.c_str());if(!f||f.isDirectory()){if(f)f.close();status="CANNOT OPEN TEXT FILE";manager();return;}view=VIEWER;rememberActivity("book",textFile,textTop);beginDraw();do{header("TEXT VIEWER",leaf(textFile),"W/S: SCROLL       BACKSPACE OR ESC: RETURN");int line=0,shown=0;while(f.available()&&shown<8){String s=f.readStringUntil('\n');s.replace("\r","");if(line++<textTop)continue;text(clip(s,88),20,108+shown++*40);}}while(epd.nextPage());f.close();}
void inputView(){view=INPUT_VIEW;beginDraw();do{header("INPUT","keyboard test","PRESS KEYS - LAST KEY IS SHOWN   ESC/BACKSPACE: HOME");text(kbOK?"CARDKB2 READY":"CARDKB2 NOT FOUND",40,125,true);text("LAST KEY: "+(typed.length()?typed:"--"),40,185,true);text("A QUICK TAP SHOULD APPEAR ONCE.",40,245);}while(epd.nextPage());}
String noteFullPath(const String &name){return name.startsWith("/")?name:folder+(folder=="/"?"":"/")+name;}
// Independent note index: browsing notes never changes the file-manager folder.
struct NoteTile { String path,preview; };
NoteTile noteTiles[120];
int noteCount=0,noteSelected=0;
bool notesOrigin=false,notesLimited=false;
void scanNotes(const String &path,int depth){
 File dir=SD_MMC.open(path.c_str());if(!dir||!dir.isDirectory()){if(dir)dir.close();return;}
 while(noteCount<120){
  File f=dir.openNextFile();if(!f)break;
  String name=leaf(String(f.name())),full=path=="/"?"/"+name:path+"/"+name;
  bool sub=f.isDirectory();String lower=name;lower.toLowerCase();
  if(sub){f.close();if(depth<8&&!name.startsWith("."))scanNotes(full,depth+1);else notesLimited=true;continue;}
  if(lower.endsWith(".txt")||lower.endsWith(".md")||name.indexOf('.')<0||full==notePath){
   String preview;int bytes=0;while(f.available()&&preview.length()<42&&bytes++<160){char c=f.read();if(c=='\n'||c=='\r')c=' ';if(c>=32&&c<=126)preview+=c;}
   noteTiles[noteCount++]={full,preview};
  }f.close();
 }
 if(noteCount>=120)notesLimited=true;dir.close();
}
void notesDraw(){
 view=NOTES_LIST;int page=noteSelected/6,start=page*6;
 beginDraw();do{
  header("NOTES","N  + NEW NOTE","ARROWS: SELECT   ENTER: OPEN   N: NEW   ESC: BACK");
  for(int r=0;r<6&&start+r<noteCount;r++){
   int i=start+r,x=18+(r%3)*260,y=94+(r/3)*160;bool on=i==noteSelected;
   epd.drawRoundRect(x,y,246,146,6,GxEPD_BLACK);
   if(on)epd.drawRoundRect(x+3,y+3,240,140,5,GxEPD_BLACK);
   epd.fillRect(x+8,y+9,230,34,on?GxEPD_BLACK:GxEPD_WHITE);
   String title=clip(leaf(noteTiles[i].path),19);
   if(on)inv(title,x+14,y+33);else text(title,x+14,y+33,true);
   text(clip(noteTiles[i].preview,19),x+14,y+76);
   text(clip(parent(noteTiles[i].path),19),x+14,y+118);
  }
  if(!noteCount)text(cardOK?"NO NOTES YET - PRESS N":"TF CARD NOT MOUNTED",32,160,true);
  text(String(noteCount)+" NOTES   PAGE "+String(page+1)+"/"+String(max(1,(noteCount+5)/6))+(notesLimited?"   INDEX LIMIT":""),24,440);
 }while(epd.nextPage());
}
void notesOpen(){
 notesOrigin=true;noteCount=0;notesLimited=false;if(cardOK)scanNotes("/",0);
 for(int i=1;i<noteCount;i++){NoteTile item=noteTiles[i];int j=i;while(j>0&&item.path.compareTo(noteTiles[j-1].path)<0){noteTiles[j]=noteTiles[j-1];j--;}noteTiles[j]=item;}
 noteSelected=constrain(noteSelected,0,max(0,noteCount-1));notesDraw();
}
void noteNameView(){view=NOTE_NAME;beginDraw();do{header("NOTES","text files on the TF card","TYPE FILE NAME  ENTER: OPEN OR CREATE  ESC: CANCEL");text("FILE NAME:",35,130,true);text(typed.length()?clip(typed,76):"e.g. NOTE.TXT",35,180,true);text("An existing text file is loaded. A new name creates a file when you save.",35,245);text("Use a name in the current folder: "+folder,35,300);}while(epd.nextPage());}
void noteEditView(){view=NOTE_EDIT;rememberActivity("note",notePath,0);prefs.putString("resumeDraft",noteText);beginDraw();do{header("NOTES",clip(notePath,65),"TYPE: EDIT  ENTER: NEW LINE  ESC: ACTIONS");if(!noteText.length())text("NEW EMPTY NOTE",35,115,true);int pos=0;for(int row=0;row<8&&pos<=(int)noteText.length();row++){int end=noteText.indexOf('\n',pos);if(end<0)end=noteText.length();text(clip(noteText.substring(pos,end),88),35,120+row*35);if(end==(int)noteText.length())break;pos=end+1;}text(status,35,422,true);}while(epd.nextPage());}
bool openNote(const String &name){notePath=noteFullPath(name);File f=SD_MMC.open(notePath.c_str());noteText="";if(f){if(f.isDirectory()){f.close();status="CHOOSE A TEXT FILE NAME";return false;}while(f.available()&&noteText.length()<3000)noteText+=(char)f.read();f.close();status="LOADED - ESC OPENS ACTIONS";}else status="NEW NOTE - ESC OPENS ACTIONS";return true;}
bool saveNote(){File f=SD_MMC.open(notePath.c_str(),"w");if(!f){appNotify("NOTE","SAVE","ERROR");return false;}size_t written=f.print(noteText);f.close();bool ok=written==noteText.length();appNotify("NOTE",ok?"SAVED":"SAVE",ok?"TF":"ERROR");return ok;}
void system(){menuScreen(SYSTEM);return;view=SYSTEM;beginDraw();do{header("SYSTEM","network and device","A: AUDIO TEST  T: CLOCK  W/S: WIFI  ENTER: PASSWORD  R: SCAN  D: DISCONNECT  ESC: HOME");text("DISPLAY      READY",40,112);text(String("CLOCK        ")+(rtcOK?rtcTime():"RTC NOT FOUND"),40,146);text(String("KEYBOARD     ")+(kbOK?"READY":"NOT FOUND"),40,180);text(String("TF CARD      ")+(cardOK?"MOUNTED":"NOT MOUNTED"),40,214);text(String("WIFI         ")+(WiFi.status()==WL_CONNECTED?WiFi.SSID():"NOT CONNECTED"),40,248);text("A: AUDIO  E: E-PAPER  F: FILE EXCHANGE",40,281,true);text("WIFI NETWORKS:",40,314,true);if(!netCount)text("PRESS R TO SEARCH FOR NETWORKS",45,348);for(int i=0;i<netCount&&i<3;i++){int y=348+i*28;bool on=i==netSelected;epd.fillRect(38,y-20,620,25,on?GxEPD_BLACK:GxEPD_WHITE);String n=clip(nets[i],52);if(on)inv(n,48,y);else text(n,48,y);}}while(epd.nextPage());}
void audioTestView(){view=AUDIO_TEST;beginDraw();do{header("AUDIO TEST","onboard ES8311 / NS4150B","ENTER: TEST TONE   ESC/BACKSPACE: SYSTEM");text(String("CODEC: ")+(codecReady?"READY":"NOT READY"),40,128,true);text(String("I2S: ")+(audioBusReady?"READY":"NOT READY"),40,168,true);text("MIC: NOT TESTED",40,208);text(audioResult,40,268,true);text("ENTER: SHORT QUIET TONE (300 ms)",40,325);text("SPEAKER MUST BE CONNECTED TO SPK",40,385);}while(epd.nextPage());}
void timeSetView(){view=TIME_SET;beginDraw();do{header("SET CLOCK",rtcOK?"PCF85063 RTC":"RTC NOT FOUND","TYPE HHMM  ENTER: SAVE  BACKSPACE: ERASE  ESC: BACK");text("CURRENT TIME: "+rtcTime(),40,100,true);text("TIME FORMAT: "+String(time24?"24-HOUR":"12-HOUR"),40,130,true);text("TIME (HH:MM):",40,190,true);String shown=typed; if(shown.length()>2) shown=shown.substring(0,2)+":"+shown.substring(2);text(shown.length()?shown:"--:--",40,240,true);text(time24?"ENTER TIME FROM 00:00 TO 23:59.":"ENTER TIME FROM 01:00 TO 12:59, THEN CHOOSE AM/PM.",40,300);if(!time24)text("AM/PM: SELECT IN CLOCK MENU",40,340,true);text("DATE: SELECT IN CLOCK MENU",40,370,true);text(status,40,400,true);}while(epd.nextPage());}
bool saveClock(){if(!rtcOK||typed.length()!=4)return false;for(size_t i=0;i<typed.length();i++)if(!isDigit(typed[i]))return false;uint8_t yy=0,mo=1,dd=1,oldHour=0,oldMinute=0,ss=0;rtcRead(yy,mo,dd,oldHour,oldMinute,ss);if(mo<1||mo>12||dd<1||dd>31){yy=0;mo=1;dd=1;}int hh=typed.substring(0,2).toInt(),mm=typed.substring(2,4).toInt();if(mm>59)return false;if(time24){if(hh>23)return false;}else{if(hh<1||hh>12)return false;bool pm=prefs.getBool("timePM",oldHour>=12);if(hh==12)hh=0;if(pm)hh+=12;}bool saved=rtcSet(yy,mo,dd,hh,mm,0);if(saved){uint8_t ry,rm,rd,rh,rmin,rs;saved=rtcRead(ry,rm,rd,rh,rmin,rs)&&rh==hh&&rmin==mm;}if(saved){clockReadAt=0;rtcTime();shownClock="";}return saved;}
// Separate NVS namespace: credentials never go to TF, logs or the web server.
struct WifiCredential { char ssid[33]; char password[65]; };
static constexpr int WIFI_SAVED_LIMIT=64;
String wifiKey(int i){return "net"+String(i);}
bool readWifiCredential(SdSettings &store,int i,WifiCredential &entry){
  String key=wifiKey(i);
  if(!store.isKey(key.c_str())||store.getBytesLength(key.c_str())!=sizeof(entry))return false;
  if(store.getBytes(key.c_str(),&entry,sizeof(entry))!=sizeof(entry))return false;
  return entry.ssid[32]==0&&entry.password[64]==0&&entry.ssid[0];
}
bool knownWifiPassword(const String &name,String &password){
  SdSettings store;if(!store.begin("knownwifi",true))return false;
  WifiCredential entry;bool found=false;
  for(int i=0;i<WIFI_SAVED_LIMIT;i++)if(readWifiCredential(store,i,entry)&&name==entry.ssid){password=entry.password;found=true;break;}
  store.end();return found;
}
bool rememberWifi(const String &name,const String &password){
  if(name.length()>32||password.length()>64)return false;
  SdSettings store;if(!store.begin("knownwifi",false))return false;
  int slot=-1;WifiCredential entry;
  for(int i=0;i<WIFI_SAVED_LIMIT;i++){
    if(readWifiCredential(store,i,entry)){
      if(name==entry.ssid){if(password==entry.password){store.end();return true;}slot=i;break;}
    }else if(slot<0)slot=i;
  }
  if(slot<0){store.end();return false;}
  memset(&entry,0,sizeof(entry));name.toCharArray(entry.ssid,sizeof(entry.ssid));password.toCharArray(entry.password,sizeof(entry.password));
  String key=wifiKey(slot);bool ok=store.putBytes(key.c_str(),&entry,sizeof(entry))==sizeof(entry);
  WifiCredential verified;ok=ok&&readWifiCredential(store,slot,verified)&&name==verified.ssid&&password==verified.password;
  store.end();return ok;
}
void passwordView(){view=NET_PASS;beginDraw();do{header("WIFI PASSWORD",wifiName,"TYPE PASSWORD  ENTER: CONNECT  BACKSPACE: ERASE  ESC: CANCEL");text("PASSWORD:",40,130,true);String stars="";for(size_t i=0;i<typed.length();i++)stars+="*";text(stars.length()?stars:"(EMPTY NETWORK)",40,180,true);text("SAVED AFTER A SUCCESSFUL CONNECTION.",40,240);text(status,40,285);}while(epd.nextPage());}
void connectWifi(){
  if(!wifiName.length())return;
  wifiWanted=true;wifiAttemptAt=millis();
  if(!startWifiSafely()){passwordView();return;}WiFi.begin(wifiName.c_str(),typed.c_str());
  status="CONNECTING...";unsigned long until=millis()+12000;
  while(WiFi.status()!=WL_CONNECTED&&(long)(millis()-until)<0)delay(100);
  if(WiFi.status()==WL_CONNECTED){
    prefs.putString("lastWifi",wifiName);prefs.putBool("wifiEnabled",true);
    status=rememberWifi(wifiName,typed)?"WIFI CONNECTED / SAVED":"CONNECTED / SAVE FAILED";
    typed="";system();
  }else{
    WiFi.disconnect(false,false);typed="";status="FAILED - RE-ENTER PASSWORD";passwordView();
  }
}
void selectWifi(){
  wifiName=netNames[netSelected];typed="";
  if(knownWifiPassword(wifiName,typed))connectWifi();else{status="NEW NETWORK";passwordView();}
}
void scanWifi(){if(!startWifiSafely()){system();return;}int n=WiFi.scanNetworks();netCount=0;for(int i=0;i<n&&netCount<MAX_NETS;i++)if(WiFi.SSID(i).length()){netNames[netCount]=WiFi.SSID(i);String savedPassword;bool saved=knownWifiPassword(netNames[netCount],savedPassword);nets[netCount]=(saved?String("[S] "):String(""))+netNames[netCount]+"  "+String(WiFi.RSSI(i))+"dBm";netCount++;}WiFi.scanDelete();netSelected=0;status=n<0?"WIFI SCAN FAILED":netCount?String(netCount)+" NETWORKS FOUND":"NO NETWORKS FOUND";Serial.printf("WIFI scan: result=%d state=%s ip=%s\n",n,wifiState().c_str(),WiFi.localIP().toString().c_str());system();}
// Enable the radio and reconnect only using credentials already saved locally.
void enableSavedWifi(){
  wifiWanted=true;prefs.putBool("wifiEnabled",true);wifiAttemptAt=millis();
  if(!startWifiSafely())return;
  if(WiFi.status()==WL_CONNECTED){status="WIFI CONNECTED";return;}
  String chosenName="",chosenPassword="",last=prefs.getString("lastWifi","");
  int strongest=-1000;
  int found=WiFi.scanNetworks();
  for(int i=0;i<found;i++){
    String name=WiFi.SSID(i),password;
    if(!knownWifiPassword(name,password))continue;
    if(name==last){chosenName=name;chosenPassword=password;break;}
    if(WiFi.RSSI(i)>strongest){strongest=WiFi.RSSI(i);chosenName=name;chosenPassword=password;}
  }
  WiFi.scanDelete();
  if(!chosenName.length()){status="SAVED NETWORK NOT FOUND";return;}
  WiFi.begin(chosenName.c_str(),chosenPassword.c_str());
  unsigned long started=millis();
  while(WiFi.status()!=WL_CONNECTED&&millis()-started<12000)delay(100);
  if(WiFi.status()==WL_CONNECTED){prefs.putString("lastWifi",chosenName);status="CONNECTED: "+chosenName;}
  else {WiFi.disconnect(false,false);status="WIFI CONNECTION FAILED";}
}
#include "web_reader.h"
const char* gameNames[]={"SNAKE","TETRIS","SOKOBAN","2048","TAMAGOTCHI","5 IN A ROW"}; void games(){menuScreen(GAMES);return;view=GAMES;beginDraw();do{header("GAMES","offline games - progress saved in ESP32","W/S: SELECT   ENTER: START   ESC/BACKSPACE: HOME");for(int i=0;i<5;i++){int y=91+i*62;bool on=i==gameSelected;epd.fillRect(35,y,500,48,on?GxEPD_BLACK:GxEPD_WHITE);epd.drawRect(35,y,500,48,GxEPD_BLACK);String s=String(i+1)+". "+gameNames[i];if(on)inv(s,55,y+30);else text(s,55,y+30,true);}text("ESC RETURNS TO THIS LIST; IT DOES NOT RESTART A GAME.",35,423);}while(epd.nextPage());}
void saveScore(const char*k,int score){int old=prefs.getInt(k,0);if(score>old)prefs.putInt(k,score);} void startSnake(){snakeLen=4;snakeDX=1;snakeDY=0;snakeScore=0;for(int i=0;i<snakeLen;i++){snakeX[i]=8-i;snakeY[i]=7;}snakeFoodX=14;snakeFoodY=7;lastTick=millis();view=SNAKE;}
void drawSnake(){epd.gameUpdates=true;beginDraw();do{header("SNAKE","score "+String(snakeScore)+"  best "+String(prefs.getInt("snake",0)),"ARROWS: MOVE   ESC/BACKSPACE: GAMES");for(int y=0;y<18;y++)for(int x=0;x<30;x++)epd.drawRect(80+x*20,90+y*19,20,19,GxEPD_BLACK);epd.fillRect(80+snakeFoodX*20+3,90+snakeFoodY*19+3,14,13,GxEPD_BLACK);for(int i=0;i<snakeLen;i++)epd.fillRect(80+snakeX[i]*20+3,90+snakeY[i]*19+3,14,13,GxEPD_BLACK);}while(epd.nextPage());epd.gameUpdates=false;}
void snakeTick(){if(millis()-lastTick<300)return;lastTick=millis();int nx=snakeX[0]+snakeDX,ny=snakeY[0]+snakeDY;bool hit=nx<0||nx>=30||ny<0||ny>=18;for(int i=0;i<snakeLen;i++)if(snakeX[i]==nx&&snakeY[i]==ny)hit=true;if(hit){saveScore("snake",snakeScore);games();return;}for(int i=snakeLen;i>0;i--){snakeX[i]=snakeX[i-1];snakeY[i]=snakeY[i-1];}snakeX[0]=nx;snakeY[0]=ny;if(nx==snakeFoodX&&ny==snakeFoodY){if(snakeLen<79)snakeLen++;snakeScore+=10;snakeFoodX=random(30);snakeFoodY=random(18);}drawSnake();}
#include "tetris_app.h"
#include "sokoban_app.h"
#include "five_in_a_row_app.h"
bool won2048=false, continued2048=false, over2048=false;
bool movesAvailable2048(){
  for(int y=0;y<4;y++)for(int x=0;x<4;x++)
    if(!b2048[y][x] || (x<3 && b2048[y][x]==b2048[y][x+1]) ||
       (y<3 && b2048[y][x]==b2048[y+1][x]))return true;
  return false;
}
void add2048(){
  int empty[16][2],n=0;
  for(int y=0;y<4;y++)for(int x=0;x<4;x++)if(!b2048[y][x]){empty[n][0]=x;empty[n++][1]=y;}
  if(n){int i=random(n);b2048[empty[i][1]][empty[i][0]]=random(10)<9?2:4;}
}
void start2048(){
  memset(b2048,0,sizeof(b2048));score2048=0;
  won2048=continued2048=over2048=false;
  add2048();add2048();view=G2048;
}
void draw2048(){
  beginDraw();do{
    String state=over2048?"GAME OVER":(won2048&&!continued2048?"2048 - YOU WIN!":"score "+String(score2048));
    header("2048",state+"  best "+String(prefs.getInt("2048",0)),"ARROWS: SLIDE   ENTER: ACTIONS   ESC/BACKSPACE: GAMES");
    for(int y=0;y<4;y++)for(int x=0;x<4;x++){
      int px=190+x*105,py=105+y*75;epd.drawRect(px,py,96,66,GxEPD_BLACK);
      if(b2048[y][x])text(String(b2048[y][x]),px+12,py+41,true);
    }
    if(over2048)text("NO MOVES - ENTER: NEW GAME MENU",190,438);
    else if(won2048&&!continued2048)text("ENTER: ACTIONS > CONTINUE",190,438);
  }while(epd.nextPage());
}
void move2048(int dx,int dy){
  if(over2048 || (won2048&&!continued2048))return;
  if(abs(dx)+abs(dy)!=1)return;
  bool changed=false;
  for(int line=0;line<4;line++){
    int values[4]={0},out[4]={0},n=0,m=0;
    // Read from the destination edge, compact, then merge each pair once.
    for(int i=0;i<4;i++){
      int x=dx?(dx>0?3-i:i):line;
      int y=dy?(dy>0?3-i:i):line;
      if(b2048[y][x])values[n++]=b2048[y][x];
    }
    for(int i=0;i<n;i++){
      int v=values[i];
      if(i+1<n && v==values[i+1]){v*=2;i++;score2048+=v;if(v>=2048)won2048=true;}
      out[m++]=v;
    }
    for(int i=0;i<4;i++){
      int x=dx?(dx>0?3-i:i):line;
      int y=dy?(dy>0?3-i:i):line;
      if(b2048[y][x]!=out[i])changed=true;
      b2048[y][x]=out[i];
    }
  }
  if(changed){add2048();over2048=!movesAvailable2048();saveScore("2048",score2048);draw2048();}
}

#include "tamagotchi_app.h"

#include "jpeg_viewer.h"
#include "video_viewer.h"
#include "book_reader.h"
#include "file_viewers.h"
 void openFile(){
 if(!count)return;
 const String path=files[selected].path;
 if(files[selected].dir){if(loadFolder(path))manager();return;}
 String n=files[selected].name;n.toLowerCase();
 if(bookSupported(n)||filePlainText(n)){bookOpen(path,true);if(view==FILES)manager();return;}
 if(n.endsWith(".epv")){videoOpen(path);return;}
 if(n.endsWith(".jpg")||n.endsWith(".jpeg")||n.endsWith(".jpe")){jpegPath=path;status="";jpegViewer();return;}
 rawPath=path;rawOffset=0;
 if(n.endsWith(".bmp")){bmpViewer();return;}
 rawDraw();
 }

static unsigned long deviceActivity=0;
static const uint16_t powerMinutes[]={0,1,2,5,10,15,30,60};
static uint8_t sleepChoice=2,offChoice=2;
// Reserve the page at boot: fragmented heap must never cancel idle sleep.
// Allocate a sleep snapshot only while sleeping; do not reserve 48 KB at boot.
String powerDelayLabel(uint8_t choice){return choice==0?String("OFF"):String(powerMinutes[choice])+" MIN";}
void loadPowerDelays(){
  sleepChoice=prefs.getUChar("sleepDelay",2);
  offChoice=prefs.getUChar("offDelay",2);
  if(sleepChoice>=8)sleepChoice=2;
  if(offChoice>=8)offChoice=2;
}
uint8_t key(){
  // CardKB2 sends the pending ASCII key directly when one byte is requested;
  // it has no keypad data-register write phase. Keeping this to one I2C
  // transaction avoids the invalid bus state from the former write/read pair.
  const size_t received=Wire.requestFrom(KB_ADDR,(size_t)1,true);
  if(received!=1){kbOK=false;return 0;}
  uint8_t value=Wire.read();
  if(value)deviceActivity=millis();
  return value;
} void appendText(uint8_t k,String&dst){if(k>=32&&k<=126&&dst.length()<80)dst+=(char)k;else if(k==8&&dst.length())dst.remove(dst.length()-1);} void startGame(){rememberActivity("game","",gameSelected);if(gameSelected==0)startSnake();else if(gameSelected==1)startTetris();else if(gameSelected==2)startSokoban();else if(gameSelected==3)start2048();else if(gameSelected==4)startTama();else startFiveRow();}
void home();
void timeSetView();
#include "calendar_app.h"
#include "timer_app.h"
#include "display2.h"
// Snapshot the panel before drawing: no I2C reads or preference writes in page loops.
String todayTime,todayDate,todayPending,todayEnv,todayTasks[5],todayCountdown[5];
String todaySignature;
unsigned long todayChecked=0;
String taskDateLabel(uint32_t d){char b[12];snprintf(b,sizeof(b),"%02u.%02u",unsigned(d%100),unsigned((d/100)%100));return String(b);}
// Gregorian minutes, independent of timezone and the calendar selection.
int64_t todayDateMinutes(uint32_t date){
  int y=date/10000,m=(date/100)%100,d=date%100;
  int64_t days=0;
  for(int year=2000;year<y;year++)days+=365+(year%4==0&&(year%100!=0||year%400==0));
  for(int month=1;month<m;month++)days+=monthDays(y,month);
  return (days+d-1)*1440;
}
String todaySnapshot(){
  calendarLoad();readPower();
  todayTime=rtcTime();todayEnv=environment();uint8_t yy,mo,dd,hh,mm,ss;
  bool dated=rtcRead(yy,mo,dd,hh,mm,ss)&&validDate(2000+yy,mo,dd);
  uint32_t date=dated?(2000+yy)*10000+mo*100+dd:0;
  int64_t now=dated?todayDateMinutes(date)+hh*60+mm:0;
  char b[24];if(dated)snprintf(b,sizeof(b),"%02u.%02u.%04u",dd,mo,2000+yy);
  todayDate=dated?String(b):String("DATE NOT SET");
  int pending=0,nearest[5]={-1,-1,-1,-1,-1};
  for(int i=0;i<64;i++){
    auto &t=calendarStore.tasks[i];auto &r=reminders[i];
    if(!t.date||t.done||!r.enabled||r.fired)continue;
    if(!validDate(t.date/10000,(t.date/100)%100,t.date%100)||r.minute>=1440)continue;
    int64_t due=todayDateMinutes(t.date)+r.minute;
    if(dated&&due<now)continue;
    pending++;
    for(int pos=0;pos<5;pos++){
      int other=nearest[pos];
      if(other<0||due<todayDateMinutes(calendarStore.tasks[other].date)+reminders[other].minute){
        for(int j=4;j>pos;j--)nearest[j]=nearest[j-1];
        nearest[pos]=i;break;
      }
    }
  }
  todayPending="REMINDERS: "+String(pending);
  readPower();
  String signature=todayTime+todayDate+todayEnv+todayPending+batteryValue+String(batteryUsb)+wifiState()+String((esp_bt_controller_get_status()==ESP_BT_CONTROLLER_STATUS_ENABLED));
  for(int row=0;row<5;row++){
    int i=nearest[row];todayTasks[row]="";todayCountdown[row]="";
    if(i<0){if(row==0)todayTasks[row]="NO UPCOMING REMINDERS";}
    else {
      todayTasks[row]=clip(String(calendarStore.tasks[i].title),23);
      if(!dated)todayCountdown[row]="--D--H--M";
      else {
        int64_t left=todayDateMinutes(calendarStore.tasks[i].date)+reminders[i].minute-now;
        snprintf(b,sizeof(b),"%luD-%02luH-%02luM",(unsigned long)(left/1440),(unsigned long)((left/60)%24),(unsigned long)(left%60));
        todayCountdown[row]=b;
      }
    }
    signature+="|"+todayTasks[row]+"|"+todayCountdown[row];
  }
  return signature;
}
// Draw geometric symbols: independent of font glyph coverage.
void drawTodayRadios(){
  const uint16_t ink=GxEPD_BLACK;
  // Equal 66-pixel cells beneath the battery; three-pixel strokes.
  auto boldLine=[&](int x1,int y1,int x2,int y2){
    for(int d=-1;d<=1;d++){
      epd.drawLine(x1+d,y1,x2+d,y2,ink);
      epd.drawLine(x1,y1+d,x2,y2+d,ink);
    }
  };
  const int x=265,y=143;
  boldLine(x,y+6,x+6,y+1);
  boldLine(x+6,y+1,x+20,y+1);
  boldLine(x+20,y+1,x+26,y+6);
  boldLine(x+5,y+12,x+10,y+8);
  boldLine(x+10,y+8,x+16,y+8);
  boldLine(x+16,y+8,x+21,y+12);
  epd.fillCircle(x+13,y+19,3,ink);
  if(WiFi.getMode()==WIFI_OFF)boldLine(x-1,y,x+27,y+23);
  const int bx=337,by=141;
  boldLine(bx,by,bx,by+26);
  boldLine(bx,by,bx+11,by+7);
  boldLine(bx+11,by+7,bx-9,by+21);
  boldLine(bx-9,by+5,bx+11,by+19);
  boldLine(bx+11,by+19,bx,by+26);
  if(esp_bt_controller_get_status()!=ESP_BT_CONTROLLER_STATUS_ENABLED)
    boldLine(bx-12,by,bx+13,by+26);
}
void drawTodayBattery(){
  const int x=244,y=43,w=132,h=88;
  // The TODAY panel clear starts below this icon: clear its entire old fill.
  epd.fillRect(x,y,w+6,h,GxEPD_WHITE);
  epd.drawRoundRect(x,y,w,h,7,GxEPD_BLACK);
  epd.fillRect(x+w,y+29,6,30,GxEPD_BLACK);
  epd.setFont(&FreeMonoBold9pt7b);epd.setTextSize(1);epd.setTextColor(GxEPD_BLACK);
  String percent=batteryPercent>=0?String(batteryPercent)+"%":"--%";
  String volts=batteryMillivolts>=0?String(batteryMillivolts/1000.0f,2)+"V":"--.--V";
  // Center visible glyph bounds, not the cursor or character count.
  auto centeredLine=[&](const String& label,int top){
    int16_t bx,by;uint16_t bw,bh;
    epd.getTextBounds(label,0,0,&bx,&by,&bw,&bh);
    epd.setCursor(x+(w-int(bw))/2-bx,top-by);
    epd.print(label);
  };
  // Keep both lines symmetric; reserve a separate centered row for USB.
  centeredLine(percent,y+(batteryUsb?12:23));
  centeredLine(volts,y+(batteryUsb?34:49));
  if(batteryUsb){
    const int cx=x+w/2,top=y+55;
    epd.fillTriangle(cx+2,top,cx-8,top+13,cx+2,top+13,GxEPD_BLACK);
    epd.fillTriangle(cx-2,top+9,cx+8,top+9,cx-2,top+22,GxEPD_BLACK);
  }
  // Invert the filled portion, including its lettering, for legible contrast.
  // Fixed left edge; the right edge retreats left as charge decreases.
  const int level=constrain(batteryPercent,0,100);
  const int fill=((w-8)*level+50)/100;
  for(int px=x+4;px<x+4+fill;px++)for(int py=y+4;py<y+h-4;py++)
    epd.drawPixel(px,py,epd.getPixel(px,py)?GxEPD_BLACK:GxEPD_WHITE);
}
void drawToday(){
  epd.fillRect(19,54,378,407,GxEPD_WHITE);
  epd.setTextSize(3);text(todayTime.substring(0,5),32,103,true);epd.setTextSize(1);
  if(todayTime.length()>5)text(todayTime.substring(6),166,139);
  drawTodayBattery();
  drawTodayRadios();
  text(todayDate,32,164,true);
  text(todayEnv,32,196);
  epd.drawLine(30,216,385,216,GxEPD_BLACK);
  text(todayPending,32,246,true);
  // Fixed-width rows keep the countdown column aligned in either text setting.
  epd.setFont(&FreeMono9pt7b);epd.setTextColor(GxEPD_BLACK);
  for(int row=0;row<5;row++){
    int y=286+row*38;
    int x=385-todayCountdown[row].length()*11;
    epd.setCursor(32,y);epd.print(clip(todayTasks[row],(x-32-11)/11));
    epd.setCursor(x,y);epd.print(todayCountdown[row]);
  }

}
void refreshToday(){
  if(millis()-todayChecked<1000)return;todayChecked=millis();
  String signature=todaySnapshot();
  if(signature!=todaySignature){
    todaySignature=signature;
    epd.setPartialWindow(18,54,380,408);epd.firstPage();
    do{drawToday();}while(epd.nextPage());
  }
  String top=stat();if(top!=shownClock){shownClock=top;epd.setPartialWindow(0,0,W,32);epd.firstPage();do{drawTopStatus(top);}while(epd.nextPage());}
}

int paperField=0;
String paperMessage="";
void paperSettings(){menuScreen(PAPER_SETTINGS);return;
  view=PAPER_SETTINGS;beginDraw();do{
    header("E-PAPER","E-PAPER SETTINGS","UP/DOWN: SELECT  ENTER: CHANGE  C: CLEAN NOW  ESC: BACK");
    text(String(paperField==0?"> ":"  ")+"TEXT: "+(largeText?"LARGE":"NORMAL"),32,130,true);
    text(String(paperField==1?"> ":"  ")+"FULL CLEAN: EVERY "+String(epd.cleanEvery)+" UPDATES",32,190,true);
    text(String(paperField==2?"> ":"  ")+"CHANGED AREAS ONLY: "+(epd.partial?"ON":"OFF"),32,250,true);
    text("No changes = no screen refresh. No UI animations.",32,315);
    text("Clock changes by minute; timer display by 10 seconds.",32,352);
    text("C: remove ghosting now. Settings survive power off.",32,389);
    text(paperMessage,32,430);
  }while(epd.nextPage());
}
bool paperKey(uint8_t k){
  if(view!=PAPER_SETTINGS)return false;
  if(k==8||k==27){system();return true;}
  if(k==0xB5)paperField=(paperField+2)%3;
  else if(k==0xB6)paperField=(paperField+1)%3;
  else if(k=='c'||k=='C'){epd.forceClean();paperMessage="FULL CLEAN REQUESTED";}
  else if(k==10||k==13){
    bool saved=false;
    if(paperField==0){largeText=!largeText;saved=prefs.putBool("epLarge",largeText)>0;}
    if(paperField==1){epd.cleanEvery=epd.cleanEvery==5?10:epd.cleanEvery==10?20:epd.cleanEvery==20?50:5;saved=prefs.putUShort("epClean",epd.cleanEvery)>0;}
    if(paperField==2){epd.partial=!epd.partial;saved=prefs.putBool("epPartial",epd.partial)>0;}
    paperMessage=saved?"SETTINGS SAVED":"SAVE FAILED";
  }else return true;
  paperSettings();return true;
}


#include "file_exchange.h"
void legacyRoute(uint8_t k){if(view==FILE_EXCHANGE){if(k==27||k==8||k=='x'||k=='X'){exchangeStop();loadFolder(folder);status="FILE ACCESS OFF";system();}return;}if(paperKey(k))return;if(timerKey(k))return;if(calendarKey(k))return;if(view==HOME){if(k=='d'||k=='D'){calDateFromHome=true;calSetCurrent=true;calInput="";calMessage="ENTER YYYYMMDD, THEN ENTER TO SAVE TO RTC";calendarInput(true);return;}int old=chosen;if(k=='t'||k=='T'){timerDraw();return;}if(k=='w'||k=='W'||k==0xB5)chosen=(chosen+5)%6;else if(k=='s'||k=='S'||k==0xB6)chosen=(chosen+1)%6;else if(k==10||k==13){if(chosen==0){if(cardOK)manager();else{status="TF CARD NOT MOUNTED";home();}}else if(chosen==1)system();else if(chosen==2){url="";webEdit();}else if(chosen==3)games();else if(chosen==4){openCalendar();}else{infoName="STATUS";infoView();}}else if(k=='r'||k=='R')home();else return;if(old!=chosen)home();return;}if(view==INFO){
  if(k==8||k==27){if(infoName=="TERMINAL")manager();else home();return;}
  if(infoName=="NOTES"||infoName=="EDITOR"){if(k=='s'||k=='S'||k==0x13){prefs.putString("notes",typed);status="NOTE SAVED";typed="";}else if(k==10||k==13){if(typed.length()<700)typed+='\n';}else appendText(k,typed);infoView();return;}
  if(infoName=="CALC"){if(k==10||k==13){bool ok=false;int value=evalSimple(typed,ok);appResult=ok?String(value):"USE TWO INTEGERS, E.G. 12*7";typed="";}else appendText(k,typed);infoView();return;}
  if(infoName=="TERMINAL"){if(k==10||k==13)terminalRun();else appendText(k,typed);infoView();return;}
  infoView();return;
}if(view==INPUT_VIEW){if(k==8||k==27)home();else{typed=String((char)k);inputView();}return;}if(view==NOTE_NAME){if(k==27){if(notesOrigin)notesOpen();else manager();return;}if(k==10||k==13){String name=typed;name.trim();if(!name.length()){status="ENTER A FILE NAME";noteNameView();return;}if(notesOrigin&&!name.endsWith(".txt")&&!name.endsWith(".md"))name+=".txt";if(openNote(name))noteEditView();else noteNameView();return;}appendText(k,typed);noteNameView();return;}if(view==NOTE_EDIT){if(k==27){loadFolder(folder);manager();return;}if(k==0x13){status=saveNote()?"NOTE SAVED TO TF CARD":"SAVE FAILED - CHECK TF CARD";noteEditView();return;}if(k==10||k==13){if(noteText.length()<3000)noteText+='\n';}else if(k>=32&&k<=126&&noteText.length()<3000)noteText+=(char)k;else if(k==8&&noteText.length())noteText.remove(noteText.length()-1);noteEditView();return;}if(view==SYSTEM){if(k=='f'||k=='F'){exchangeStart();return;}if(k=='e'||k=='E'){paperSettings();return;}if(k=='a'||k=='A'){audioTestView();return;}if(k=='t'||k=='T'){typed="";timeSetView();return;}if(k=='r'||k=='R'){scanWifi();return;}if(k=='d'||k=='D'){wifiWanted=false;prefs.putBool("wifiEnabled",false);WiFi.setAutoReconnect(false);WiFi.disconnect();status="WIFI DISCONNECTED";system();return;}if(k=='w'||k=='W'||k==0xB5){if(netCount)netSelected=(netSelected+netCount-1)%netCount;system();return;}if(k=='s'||k=='S'||k==0xB6){if(netCount)netSelected=(netSelected+1)%netCount;system();return;}if((k==10||k==13)&&netCount){selectWifi();return;}if(k==8||k==27)home();return;}if(view==AUDIO_TEST){if(k==8||k==27){system();return;}if(k==10||k==13){audioTone();audioTestView();return;}return;}if(view==NET_PASS){if(k==27){system();return;}if(k==10||k==13){connectWifi();return;}appendText(k,typed);passwordView();return;}if(view==TIME_SET){if(k=='d'||k=='D'){calSetCurrent=true;calInput="";calMessage="";calendarInput(true);return;}if(k==27){system();return;}if(k=='f'||k=='F'){time24=!time24;prefs.putBool("time24",time24);typed="";timeSetView();return;}if(!time24&&(k=='a'||k=='A'||k=='p'||k=='P')){prefs.putBool("timePM",k=='p'||k=='P');timeSetView();return;}if(k==10||k==13){status=saveClock()?"CLOCK SAVED AND VERIFIED":"CLOCK NOT SAVED - CHECK TIME / RTC";typed="";timeSetView();return;}if(k>=32&&k<=126&&isDigit(k)&&typed.length()<4)typed+=(char)k;else if(k==8&&typed.length())typed.remove(typed.length()-1);timeSetView();return;}if(view==WEB_EDIT){if(k==27){home();return;}if(k==10||k==13){fetchWeb();webPage();return;}appendText(k,url);webEdit();return;}if(view==WEB_PAGE){if(k=='w'||k=='W'||k==0xB5){if(textTop)textTop--;webPage();}else if(k=='s'||k=='S'||k==0xB6){textTop++;webPage();}else if(k==8||k==27)webEdit();return;}if(view==GAMES){if(k=='w'||k=='W'||k==0xB5){gameSelected=(gameSelected+4)%5;games();}else if(k=='s'||k=='S'||k==0xB6){gameSelected=(gameSelected+1)%5;games();}else if(k==10||k==13)startGame();else if(k==8||k==27)home();return;}if(view==TAMAGOTCHI){if(k==8||k==27){saveTama();games();return;}if(k=='f'||k=='F'){tamaHunger=tamaHunger>20?tamaHunger-20:0;tamaHappy=min(100,(int)tamaHappy+3);}else if(k=='p'||k=='P'){if(tamaEnergy>=12){tamaEnergy-=12;tamaHappy=min(100,(int)tamaHappy+15);}}else if(k=='z'||k=='Z'){tamaEnergy=min(100,(int)tamaEnergy+25);tamaHunger=min(100,(int)tamaHunger+4);}else return;saveTama();tamaStats(false);return;}if(view==SNAKE){if(k==8||k==27){saveScore("snake",snakeScore);games();return;}if((k=='w'||k=='W'||k==0xB5)&&snakeDY==0){snakeDX=0;snakeDY=-1;}if((k=='s'||k=='S'||k==0xB6)&&snakeDY==0){snakeDX=0;snakeDY=1;}if(k==0xB4&&snakeDX==0){snakeDX=-1;snakeDY=0;}if(k==0xB7&&snakeDX==0){snakeDX=1;snakeDY=0;}return;}if(view==TETRIS){if(k==8||k==27){saveScore("tetris",tetScore);games();return;}if(k==0xB4&&tetX>0)tetX--;if(k==0xB7&&tetX<8)tetX++;if(k=='s'||k=='S'||k==0xB6)tetY=min(14,tetY+1);drawTetris();return;}if(view==FIVE_ROW){if(k==8||k==27){frSave();games();return;}fiveRowKey(k);return;}if(view==SOKOBAN){if(k==8||k==27){skSave();games();return;}if(k==0xB5)moveSok(0,-1);else if(k==0xB6)moveSok(0,1);else if(k==0xB4)moveSok(-1,0);else if(k==0xB7)moveSok(1,0);return;}if(view==G2048){if(k==8||k==27){saveScore("2048",score2048);games();return;}if(k=='r'||k=='R'){start2048();draw2048();return;}if(k=='w'||k=='W'||k==0xB5)move2048(0,-1);else if(k=='s'||k=='S'||k==0xB6)move2048(0,1);else if(k==0xB4)move2048(-1,0);else if(k==0xB7)move2048(1,0);return;}if(view==VIEWER){if(k=='w'||k=='W'||k==0xB5){if(textTop)textTop--;viewer();}else if(k=='s'||k=='S'||k==0xB6){textTop++;viewer();}else if(k==8||k==27)manager();return;}if(deleting){if(k=='y'||k=='Y'){if(!files[selected].dir)SD_MMC.remove(files[selected].path.c_str());loadFolder(folder);deleting=false;manager();}else if(k=='n'||k=='N'||k==8||k==27){deleting=false;manager();}return;}if(k=='w'||k=='W'||k==0xB5){selected=max(0,selected-1);if(selected<first)first=selected;manager();}else if(k=='s'||k=='S'||k==0xB6){selected=min(count-1,selected+1);if(selected>=first+ROWS)first=selected-ROWS+1;manager();}else if(k==10||k==13)openFile();else if(k=='n'||k=='N'){typed="";noteNameView();}else if(k=='t'||k=='T'){infoName="TERMINAL";typed="";appResult="";infoView();}else if((k==8||k==27)&&folder!="/"){if(loadFolder(parent(folder)))manager();}else if(k==8||k==27)home();else if((k=='d'||k=='D')&&count){deleting=true;manager();}else if(k=='r'||k=='R'){loadFolder(folder);manager();}}
#include "recorder_app.h"
void showMenu(int c,View v);
extern bool menuActive;
#include "sequencer_app.h"
#include "groove_box.h"
void idleSleep();
struct ResumeGame {int x[80],y[80],len,dx,dy,fx,fy,score;int board[4][4],points;bool won,continued,over;};
void checkpointGame(){
 if(view!=SNAKE&&view!=G2048)return;
 ResumeGame g={};
 if(view==SNAKE){memcpy(g.x,snakeX,sizeof(snakeX));memcpy(g.y,snakeY,sizeof(snakeY));g.len=snakeLen;g.dx=snakeDX;g.dy=snakeDY;g.fx=snakeFoodX;g.fy=snakeFoodY;g.score=snakeScore;}
 else{memcpy(g.board,b2048,sizeof(b2048));g.points=score2048;g.won=won2048;g.continued=continued2048;g.over=over2048;}
 const char *key=view==SNAKE?"resumeSnake":"resume2048";
 ResumeGame old;if(prefs.getBytesLength(key)!=sizeof(g)||prefs.getBytes(key,&old,sizeof(old))!=sizeof(old)||memcmp(&g,&old,sizeof(g)))prefs.putBytes(key,&g,sizeof(g));
}
void resumeLast(){
 String kind=prefs.getString("resumeKind","");String path=prefs.getString("resumePath","");int pos=prefs.getInt("resumePos",0);
 if(kind=="book"||kind=="note"){
  if(!cardOK){status="TF CARD NOT MOUNTED";home();return;}
  if(kind=="book"){textFile=path;textTop=max(0,pos);viewer();return;}
  if(openNote(path)){noteText=prefs.getString("resumeDraft",noteText);noteEditView();}else home();return;
 }
 if(kind=="game"){gameSelected=constrain(pos,0,5);startGame();if(view==FIVE_ROW){drawFiveRow();return;}ResumeGame g;const char *key=view==SNAKE?"resumeSnake":"resume2048";
 if((view==SNAKE||view==G2048)&&prefs.getBytesLength(key)==sizeof(g)&&prefs.getBytes(key,&g,sizeof(g))==sizeof(g)){
 if(view==SNAKE&&g.len>=4&&g.len<=80){memcpy(snakeX,g.x,sizeof(snakeX));memcpy(snakeY,g.y,sizeof(snakeY));snakeLen=g.len;snakeDX=g.dx;snakeDY=g.dy;snakeFoodX=g.fx;snakeFoodY=g.fy;snakeScore=g.score;}
 if(view==G2048){memcpy(b2048,g.board,sizeof(b2048));score2048=g.points;won2048=g.won;continued2048=g.continued;over2048=g.over;}}
 if(view==SNAKE)drawSnake();else if(view==TETRIS)drawTetris();else if(view==SOKOBAN)drawSokoban();else if(view==G2048)draw2048();else if(view==FIVE_ROW)drawFiveRow();else petDraw();return;}
 home();
}
static bool reminderSoundPending=false,reminderWake=false;
static unsigned long reminderSoundAt=0;
static bool reminderPopupActive=false;
static uint8_t *reminderBackground=nullptr;
static String reminderPopupText;
void reminderPopupShow(){
 if(!reminderPopupActive){
  reminderBackground=(uint8_t*)heap_caps_malloc(W*H/8,(psramFound()?MALLOC_CAP_SPIRAM:MALLOC_CAP_INTERNAL)|MALLOC_CAP_8BIT);
  if(reminderBackground)memcpy(reminderBackground,epd.getBuffer(),W*H/8);
 }
 reminderPopupActive=true;
 beginDraw();do{
  epd.fillRect(65,125,670,245,GxEPD_WHITE);
  epd.drawRect(65,125,670,245,GxEPD_BLACK);
  epd.fillRect(65,125,670,42,GxEPD_BLACK);
  epd.setFont(&FreeMonoBold9pt7b);epd.setTextColor(GxEPD_WHITE);epd.setCursor(85,153);epd.print("REMINDER / TASK DUE");
  // Large bold body, wrapped at word boundaries within the 630px content area.
  epd.setTextColor(GxEPD_BLACK);epd.setFont(&FreeSansBold12pt7b);
  int pos=0;
  for(int row=0;row<3 && pos<(int)reminderPopupText.length();row++){
   int end=pos;int lastSpace=-1;
   while(end<(int)reminderPopupText.length()){
    int16_t bx,by;uint16_t bw,bh;
    epd.getTextBounds(reminderPopupText.substring(pos,end+1),0,0,&bx,&by,&bw,&bh);
    if(bw>625)break;
    if(reminderPopupText[end]==' ')lastSpace=end;
    end++;
   }
   if(end<(int)reminderPopupText.length() && lastSpace>pos)end=lastSpace;
   if(end==pos)end++;
   epd.setCursor(85,207+row*40);epd.print(reminderPopupText.substring(pos,end));
   pos=end;while(pos<(int)reminderPopupText.length() && reminderPopupText[pos]==' ')pos++;
  }
  epd.setFont(&FreeMono9pt7b);epd.setCursor(85,342);epd.print("ANY KEY: DISMISS AND RETURN");
 }while(epd.nextPage());
}
void reminderPopupDismiss(){
 reminderPopupActive=false;reminderWake=false;reminderSoundPending=false;reminderBellUntil=0;display2Previous="";
 if(reminderBackground){memcpy(epd.getBuffer(),reminderBackground,W*H/8);free(reminderBackground);reminderBackground=nullptr;epd.nextPage();}
 else home();
 deviceActivity=millis();
}
void reminderSoundTick(){
 if(!reminderSoundPending)return;
 if((int32_t)(reminderBellUntil-millis())<=0){reminderSoundPending=false;return;}
 // Never use the I2S stream concurrently with recording or playback.
 if(recBusy || seqBusy || gbBusy || (reminderSoundAt && millis()-reminderSoundAt<5000))return;
 reminderSoundAt=millis();
 audioNotificationMelody();
}
void taskNoticeTick(){
 reminderSoundTick();
 static unsigned long checked=0;if(millis()-checked<1000)return;checked=millis();
 uint8_t y,m,d,h,mi,se;if(!rtcRead(y,m,d,h,mi,se)||!validDate(2000+y,m,d))return;
 calendarLoad();uint32_t date=(2000+y)*10000+m*100+d;
 for(int i=0;i<64;i++){auto &t=calendarStore.tasks[i];auto &r=reminders[i];
 if(!t.date||t.done||!r.enabled||r.fired)continue;
 if(t.date>date||(t.date==date&&h*60+mi<r.minute))continue;
 r.fired=1;reminderSave();appNotify("REMINDER","DUE",String(t.title));reminderBellUntil=millis()+60000UL;
 status="REMINDER: "+String(t.title);reminderPopupText=String(t.title);reminderWake=true;deviceActivity=millis();
 if(r.sound){reminderSoundPending=true;reminderSoundAt=0;}
 }
 reminderSoundTick();
}
#include "gps_app.h"
#include "jdm_app.h"
#include "menu_navigation.h"
#include "quick_settings.h"
void handle(uint8_t k){if(quickOpen){quickKey(k);return;}if(reminderPopupActive){reminderPopupDismiss();return;}if(k==27&&escState==0){escState=1;escAt=millis();return;}if(escState==1){if(k=='['){escState=2;return;}escState=0;route(27);route(k);return;}if(escState==2){escState=0;if(k=='A')k=0xB5;else if(k=='B')k=0xB6;else if(k=='C')k=0xB7;else if(k=='D')k=0xB4;else return;}route(k);}
// Plain power-state screen; independent of application text-size settings.
static void drawPowerText(const char *label){
  epd.fillScreen(GxEPD_WHITE);
  if(drawJpeg(prefs.getString(strcmp(label,"sleep")==0?"sleepImage":"offImage",""),0,H))return;
  epd.fillScreen(GxEPD_WHITE);
  epd.setFont(&FreeSansBold12pt7b);
  epd.setTextSize(1);
  epd.setTextColor(GxEPD_BLACK);
  int16_t x,y;uint16_t w,h;
  epd.getTextBounds(label,0,0,&x,&y,&w,&h);
  epd.setCursor((W-int(w))/2-x,(H-int(h))/2-y);
  epd.print(label);
}
void shutdownDevice(){
  gpsStop();
  if(view==TAMAGOTCHI)saveTama();
  if(view==SNAKE)saveScore("snake",snakeScore);
  if(view==TETRIS)tetSave();
  if(view==FIVE_ROW)frSave();
  if(view==G2048)saveScore("2048",score2048);
  exchangeStop();
  WiFi.disconnect();WiFi.mode(WIFI_OFF);
  // Complete the e-paper refresh BEFORE cutting the PMIC outputs.
  epd.setPartialWindow(0,0,W,H);epd.firstPage();
  do{
    drawPowerText("power off");
  }while(epd.nextPage());
  epd.hibernate();
  powerEnableBit(0x10); // XPowersAXP2101::shutdown(): COMMON_CONFIG bit 0
  delay(500);
  // USB can keep the system alive. Do not resume the desktop or claim a cut.
  Serial.println("PMIC shutdown requested; USB power may remain.");
  for(;;){powerEnableBit(0x10);delay(1000);}
}
void pollPowerKey(){
  static unsigned long checked=0;
  if(millis()-checked<50)return;
  checked=millis();
  if(!pwrKeyReady)return;
  int irq=powerReg(0x49);
  if(irq>=0 && (irq&0x04)){
    powerWrite(0x49,0x04);shutdownDevice();
  }
}
// CardKB2 has no wired key interrupt: retain its power and poll between
// short real light-sleep intervals. RAM, drafts and the timer remain intact.
static constexpr unsigned long SLEEP_POWER_CHECK_MS = 1000UL;
void idleSleep(){
  if(gpsPowered)return;
  if(seqBusy||gbBusy||recBusy)return;
  if(reminderWake || reminderSoundPending)return;
  if(!sleepChoice || millis()-deviceActivity<(unsigned long)powerMinutes[sleepChoice]*60000UL)return;
  // Preserve the exact application page, including unsaved edits and dialogs.
  const size_t pageBytes=W*H/8;
  uint8_t *savedPage=(uint8_t*)heap_caps_malloc(pageBytes,
    (psramFound()?MALLOC_CAP_SPIRAM:MALLOC_CAP_INTERNAL)|MALLOC_CAP_8BIT);
  if(!savedPage){deviceActivity=millis();Serial.println("SLEEP: snapshot unavailable; staying awake");return;}
  memcpy(savedPage,epd.getBuffer(),pageBytes);
  bool restoreWifi=wifiWanted;wifiSuspended=true;
  exchangeStop(); // Never reopen password-protected access automatically.
  WiFi.disconnect(false,false);WiFi.mode(WIFI_OFF);
  drawPowerText("sleep");
  epd.nextPage();
  epd.hibernate(); // Keep the sleep text visible throughout light sleep.
  display2Tick(); // USB keeps OLED on; battery retains its idle timeout.
  escState=0;
  Serial.println("IDLE: light sleep; keyboard polling 50 ms");
  Serial.flush();
  uint8_t wakeKey=0;
  const unsigned long sleepStarted=millis();
  unsigned long autoOffChecked=sleepStarted;
  while(true){
    // esp_timer and millis include time spent in light sleep.
    if(timerRunning && timerLeft()==0)break;
    taskNoticeTick();
    if(reminderWake)break;
    pollPowerKey(); // PWR keeps its established shutdown action.
    display2Tick(); // Also detect USB insertion/removal while asleep.
    if(!kbOK){
      Wire.beginTransmission(KB_ADDR);kbOK=Wire.endTransmission()==0;
    }
    if(kbOK){wakeKey=key();if(wakeKey)break;}
    if(offChoice && millis()-sleepStarted>=(unsigned long)powerMinutes[offChoice]*60000UL && millis()-autoOffChecked>=SLEEP_POWER_CHECK_MS){
      autoOffChecked=millis();
      if(batteryOnlyPower())shutdownDevice();
    }
    esp_sleep_enable_timer_wakeup(50000);
    esp_err_t result=esp_light_sleep_start();
    if(result!=ESP_OK)delay(50); // Remain responsive if sleep is rejected.
  }
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
  deviceActivity=millis();lastTick=millis();
  display2ActivityReset();
  wifiSuspended=false;
  if(restoreWifi){resumeSavedWifi();Serial.println("WIFI: wake reconnect requested");}
  Serial.println("IDLE: awake");
  // The wake key is consumed: no accidental edit, launch or delete.
  // Drain only the ANSI tail of an Escape-arrow sequence, not further keys.
  if(wakeKey==27){
    unsigned long until=millis();
    while(millis()-until<75){if(kbOK)key();delay(5);}
  }
  memcpy(epd.getBuffer(),savedPage,pageBytes);
  free(savedPage);
  epd.nextPage();
  timerTick();display2Tick();
}
void setup(){
  // Start USB CDC before touching peripherals so early boot failures are visible.
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("[PocketOS] setup: entered");

  pinMode(0, INPUT_PULLUP);
  Serial.println("[PocketOS] init GPS power control");
  gpsInit();

  Serial.println("[PocketOS] init SD_MMC");
  SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3);
  cardOK = SD_MMC.begin("/sdcard", true) && SD_MMC.cardType() != CARD_NONE;
  Serial.printf("[PocketOS] SD_MMC: %s\\n", cardOK ? "ready" : "unavailable");

  Serial.println("[PocketOS] init preferences");
  prefs.begin("epaperpc", false);
  loadPowerDelays();
  time24 = prefs.getBool("time24", true);
  largeText = prefs.getBool("epLarge", false);
  epd.partial = prefs.getBool("epPartial", true);
  epd.cleanEvery = prefs.getUShort("epClean", 20);
  if (epd.cleanEvery != 5 && epd.cleanEvery != 10 && epd.cleanEvery != 20 && epd.cleanEvery != 50) epd.cleanEvery = 20;

  Serial.println("[PocketOS] init I2C");
  Wire.begin(KB_SDA, KB_SCL);
  Wire.setClock(100000);
  Wire.setTimeOut(25);
  audioVolume = constrain((int)prefs.getUChar("volume", 40), 0, 100);
  audioMuted = prefs.getBool("soundMuted", false);

  Serial.println("[PocketOS] init audio");
  audioInit();
  Serial.println("[PocketOS] init power key");
  initPowerKey();
  Wire.beginTransmission(RTC_ADDR);
  rtcOK = !Wire.endTransmission();
  Wire.beginTransmission(KB_ADDR);
  kbOK = !Wire.endTransmission();

  Serial.println("[PocketOS] init E-Ink SPI pins");
  SPI.begin(EPD_SCLK, -1, EPD_MOSI, EPD_CS);
  pinMode(EPD_CS, OUTPUT);
  digitalWrite(EPD_CS, HIGH);
  pinMode(EPD_RST, OUTPUT);
  digitalWrite(EPD_RST, HIGH);
  pinMode(EPD_DC, OUTPUT);
  digitalWrite(EPD_DC, HIGH);

  Serial.println("[PocketOS] init E-Ink panel");
  epd.init(115200, true, 2, false);
  epd.setRotation(0);
  Serial.println("[PocketOS] E-Ink initialized");

  if (cardOK && !loadFolder("/")) cardOK = false;
  Serial.println("[PocketOS] init side display");
  display2Init();
  noticeLoad();
  if (prefs.getBool("wifiEnabled", false)) resumeSavedWifi();

  Serial.println("[PocketOS] draw home");
  home();
  deviceActivity = millis();
  display2ActivityReset();
  display2Tick();
  Serial.println("[PocketOS] setup complete");
}
void wifiTick(){
  if(wifiWanted && !wifiSuspended && WiFi.status()!=WL_CONNECTED && millis()-wifiAttemptAt>=30000){
    if(WiFi.scanComplete()!=WIFI_SCAN_RUNNING)resumeSavedWifi();
  }
  static String previous;
  String state=wifiState()+" "+WiFi.localIP().toString();
  if(state==previous)return;
  previous=state;
  Serial.printf("WIFI: %s status=%d RSSI=%d\n",state.c_str(),int(WiFi.status()),WiFi.status()==WL_CONNECTED?WiFi.RSSI():0);
  if(menuActive && (menuContext==SYSTEM||menuContext==340||menuContext==200||menuContext==301))showMenu(menuContext,menuView);
}
void loop(){
 videoTick();
  quickBootPoll();
  if(quickOpen){
    taskNoticeTick();
    if(reminderWake){quickClose();}else{
      pollPowerKey();display2Tick();
      if(kbOK){uint8_t k=key();if(k)quickKey(k);}
      if(quickOpen && uint32_t(millis()-quickLastActivity)>=5000)quickClose();
      delay(20);return;
    }
  }
  taskNoticeTick();
  if(reminderWake){reminderWake=false;reminderPopupShow();}
  if(reminderPopupActive){
    // Keep the modal above all application redraws until acknowledged.
    pollPowerKey();display2Tick();
    if(kbOK){uint8_t k=key();if(k){escState=0;display2ActivityReset();reminderPopupDismiss();}}
    delay(20);return;
  }
  gpsTick();
  wifiTick();
  static unsigned long gameCheckpointAt=0;if(millis()-gameCheckpointAt>=5000){gameCheckpointAt=millis();checkpointGame();}
  pollPowerKey();
  recorderTick();
  seqTick();
  gbTick();
  if(view==SEQUENCER&&seqError){seqError=false;seqMessage="I2S WRITE ERROR";seqDraw();}
  exchangeTick();
  timerTick();
  // A missed startup probe must not permanently disable input. Retry without
  // changing shared I2C pins or reinitializing the display or the TF card.
  static unsigned long lastProbe=0,lastHeartbeat=0;
  if(!kbOK && millis()-lastProbe>=1000){
    lastProbe=millis();
    Wire.beginTransmission(KB_ADDR);
    kbOK=Wire.endTransmission()==0;
    if(kbOK) Serial.println("INPUT: CardKB2 detected");
  }
  if(kbOK){
    uint8_t k=key();
    if(k) { display2ActivityReset(); display2Tick(); Serial.printf("INPUT: 0x%02X\n",k); checkpointGame();handle(k); }
  }
  // Escape expiration is independent of whether a keyboard is online.
  if(escState && millis()-escAt>65){escState=0;route(27);}
  tamaTick();
  if(view==SNAKE)snakeTick();
  else if(view==TETRIS)tetrisTick();
  else if(view==TAMAGOTCHI){refreshClock();}
  else refreshClock();
  refreshPowerCells();
  display2Tick();
  if(millis()-lastHeartbeat>=10000){
    lastHeartbeat=millis();
    Serial.printf("UI alive: view=%u keyboard=%s heap=%u\n",unsigned(view),kbOK?"ready":"missing",unsigned(ESP.getFreeHeap()));
  }
  // Handle any pending key before testing inactivity, so a key at the
  // two-minute boundary is not mistaken for a wake-only press.
  idleSleep();
  delay(20);
}