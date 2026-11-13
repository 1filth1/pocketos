static int seqDisplay2State();
// Display 1 remains unchanged. Display 2 is a portrait SSD1306 (32x128).
#include <Adafruit_SSD1306.h>
#include <esp_bt.h>
static Adafruit_SSD1306 display2(128,32,&Wire,-1,100000,100000);
static bool display2Ready=false,display2Sleeping=false;
static constexpr unsigned long DISPLAY2_IDLE_MS=20000UL;
static unsigned long display2Activity=0,display2Checked=0;
static String display2Previous;
static bool display2UsbKnown=false,display2Usb=false;
static unsigned long display2BoltAt=0;
static bool display2BoltActive=false;
void display2ActivityReset(){
  display2Activity=millis();
  if(!display2Ready)return;
  if(display2Sleeping){
    display2.ssd1306_command(SSD1306_DISPLAYON);
    display2Sleeping=false;
    display2Previous="";
    display2Checked=millis()-1000;
  }
}
void display2Init(){
  Wire.beginTransmission(0x3C);
  if(Wire.endTransmission()!=0){Serial.println("DISPLAY 2: NOT FOUND (0x3C)");return;}
  display2Ready=display2.begin(SSD1306_SWITCHCAPVCC,0x3C,false,false);
  if(!display2Ready){Serial.println("DISPLAY 2: allocation failed");return;}
  display2.setRotation(1);
  display2.clearDisplay();display2.display();
  display2.setTextColor(SSD1306_WHITE);display2.setTextWrap(false);
  display2ActivityReset();
  Serial.println("DISPLAY 2: portrait 32x128; idle off 20 seconds on battery only");
}
void display2Line(int y,const String &s){display2.setCursor(1,y);display2.print(s);}
// Current foreground view only: no claim that a background app is running.
void display2AppIcon(){
  const int x=4,y=64;
  const uint16_t c=SSD1306_WHITE;
  switch(view){
    case HOME:
      break; // No home icon: keep the home screen uncluttered.
    case FILES: case FILE_EXCHANGE:
      display2.drawRect(x,y+5,24,18,c);display2.drawRect(x+1,y+1,10,5,c);
      if(view==FILE_EXCHANGE){display2.drawLine(x+4,y+13,x+19,y+13,c);display2.drawTriangle(x+15,y+9,x+19,y+13,x+15,y+17,c);}break;
    case NOTE_NAME: case NOTE_EDIT: case INPUT_VIEW: case VIEWER:
      display2.drawRect(x+3,y,18,24,c);
      for(int i=0;i<3;i++)display2.drawFastHLine(x+7,y+6+i*5,10,c);
      break;
    case CALENDAR: case CAL_TASKS: case CAL_ADD: case CAL_DATE:
      display2.drawRect(x,y+3,24,21,c);display2.drawFastHLine(x,y+9,24,c);
      display2.drawFastVLine(x+6,y,6,c);display2.drawFastVLine(x+17,y,6,c);
      for(int i=0;i<3;i++)for(int j=0;j<2;j++)display2.fillRect(x+4+i*7,y+13+j*6,3,3,c);break;
    case TIMER_APP: case TIMER_EDIT:
      display2.drawCircle(x+12,y+13,10,c);display2.drawFastHLine(x+8,y,9,c);
      display2.drawLine(x+12,y+13,x+12,y+6,c);display2.drawLine(x+12,y+13,x+18,y+16,c);break;
    case GROOVE_BOX: case SEQUENCER: case AUDIO_TEST:
      display2.drawLine(x+9,y+3,x+21,y,c);display2.drawFastVLine(x+9,y+3,16,c);
      display2.drawFastVLine(x+21,y,17,c);display2.fillCircle(x+5,y+20,4,c);display2.fillCircle(x+17,y+18,4,c);break;
    case RECORDER:
      display2.drawRoundRect(x+8,y,9,16,4,c);display2.drawRoundRect(x+4,y+7,17,13,6,c);
      display2.drawFastVLine(x+12,y+20,4,c);display2.drawFastHLine(x+7,y+24,11,c);break;
    case WEB_EDIT: case WEB_PAGE: case NET_PASS:
      display2.drawCircle(x+12,y+12,11,c);display2.drawRoundRect(x+7,y+1,11,23,5,c);
      display2.drawFastHLine(x+1,y+12,23,c);break;
    case GAMES: case SNAKE: case TETRIS: case SOKOBAN: case G2048: case TAMAGOTCHI:
      display2.drawRoundRect(x,y+5,24,16,4,c);
      display2.drawFastHLine(x+3,y+13,7,c);display2.drawFastVLine(x+6,y+10,7,c);
      display2.fillCircle(x+17,y+11,1,c);display2.fillCircle(x+20,y+15,1,c);break;
    case GPS_APP:
      // Satellite: central body, two solar panels and a radio dish.
      display2.drawRect(x+8,y+8,9,10,c);
      display2.drawFastHLine(x+5,y+12,3,c);
      display2.drawFastHLine(x+17,y+12,3,c);
      display2.drawRect(x,y+7,6,13,c);
      display2.drawRect(x+19,y+7,6,13,c);
      display2.drawFastVLine(x+3,y+7,13,c);
      display2.drawFastVLine(x+22,y+7,13,c);
      display2.drawFastHLine(x,y+13,6,c);
      display2.drawFastHLine(x+19,y+13,6,c);
      display2.drawTriangle(x+8,y+3,x+16,y+3,x+12,y+7,c);
      display2.drawFastVLine(x+12,y,3,c);
      display2.drawPixel(x+12,y+21,c);
      break;
    case JPEG_VIEW:
      display2.drawRect(x,y,24,24,c);display2.drawCircle(x+17,y+6,3,c);
      display2.drawTriangle(x+2,y+21,x+9,y+10,x+17,y+21,c);break;
    default:
      display2.drawCircle(x+12,y+12,8,c);display2.drawCircle(x+12,y+12,3,c);
      for(int i=-1;i<=1;i+=2){display2.fillRect(x+10,y+12+i*10-2,5,5,c);display2.fillRect(x+12+i*10-2,y+10,5,5,c);}break;
  }
}
void display2Tick(){
  if(!display2Ready)return;
  if(millis()-display2Checked<100)return;
  display2Checked=millis();
  // Reuse the existing PMIC VBUS-good status, independent of battery presence.
  int usbStatus=powerReg(0x00);
  if(usbStatus>=0){
    bool connected=(usbStatus&0x20)!=0;
    if(connected && (!display2UsbKnown || !display2Usb)){
      display2BoltAt=millis();display2BoltActive=true;
      display2ActivityReset();
    }
    display2Usb=connected;display2UsbKnown=true;
  }
  if(display2BoltActive && millis()-display2BoltAt>=2000UL)display2BoltActive=false;
  const bool batteryOnly=batteryOnlyPower();
  if(!batteryOnly && display2Sleeping){
    display2.ssd1306_command(SSD1306_DISPLAYON);
    display2Sleeping=false;
    display2Previous="";
  }
  if(batteryOnly && !display2Sleeping && millis()-display2Activity>=DISPLAY2_IDLE_MS){
    display2.ssd1306_command(SSD1306_DISPLAYOFF);
    display2Sleeping=true;
  }
  if(appNoticePending){appNoticePending=false;display2ActivityReset();}
  if(display2Sleeping)return;
  readPower();
  int percentEnd=batteryValue.indexOf('%');
  String battery=percentEnd>=0?batteryValue.substring(0,percentEnd+1):String("--%");
  bool bell=(int32_t)(reminderBellUntil-millis())>0;
  bool notice=appNoticeActive && millis()-appNoticeAt<10000UL;
  if(!notice)appNoticeActive=false;
  String timer="",remaining="";
  if(timerLoaded && timerEngaged){
    unsigned long sec=(timerLeft()+999)/1000;
    char b[24];snprintf(b,sizeof(b),"%02lu:%02lu",sec/60,sec%60);
    remaining=String(b);timer=timerRunning?"RUN":"PAUSE";
  }
  uint8_t yy,mo,dd,hh,mm,ss;
  String clock="--:--";
  if(rtcRead(yy,mo,dd,hh,mm,ss)){
    char b[6];snprintf(b,sizeof(b),"%02u:%02u",hh,mm);clock=b;
  }
  bool wifiOn=WiFi.getMode()!=WIFI_OFF;
  bool btOn=esp_bt_controller_get_status()==ESP_BT_CONTROLLER_STATUS_ENABLED;
  int seqIcon=seqDisplay2State();
  String state=String(wifiOn)+String(btOn)+clock+"|"+String(int(view))+"|"+String(display2BoltActive)+"|"+String(seqIcon)+"|"+String(bell)+battery+"|"+timer+"|"+remaining+"|"+(notice?appNotice[0]+appNotice[1]+appNotice[2]:String(""))+String(timerPomo);
  if(state==display2Previous)return;
  display2Previous=state;
  display2.clearDisplay();display2.setTextSize(1);
  // Phone-style outline and terminal, with honest gauge fill.
  display2.drawRoundRect(3,2,24,11,2,SSD1306_WHITE);
  display2.fillRect(27,5,2,5,SSD1306_WHITE);
  if(percentEnd>0 && battery!="--%"){
    int level=constrain(battery.substring(0,percentEnd).toInt(),0L,100L);
    int width=(20*level+50)/100;
    if(width)display2.fillRect(5,4,width,7,SSD1306_WHITE);
  }
  display2.setCursor((32-int(battery.length())*6)/2,15);
  display2.print(battery);
  // Independent stacked Wi-Fi / Bluetooth icons; no shared text label.
  const uint16_t ink=SSD1306_WHITE;
  display2.drawLine(6,29,10,26,ink);display2.drawLine(10,26,21,26,ink);
  display2.drawLine(21,26,25,29,ink);
  display2.drawLine(10,32,13,30,ink);display2.drawLine(13,30,18,30,ink);
  display2.drawLine(18,30,21,32,ink);display2.fillCircle(16,35,1,ink);
  if(!wifiOn){
    display2.drawLine(5,25,26,37,ink);
    display2.drawLine(5,26,26,38,ink);
  }
  display2.drawLine(15,39,15,51,ink);
  display2.drawLine(15,39,21,43,ink);display2.drawLine(21,43,10,49,ink);
  display2.drawLine(10,41,21,47,ink);display2.drawLine(21,47,15,51,ink);
  if(!btOn){
    display2.drawLine(7,39,25,51,ink);
    display2.drawLine(7,40,25,52,ink);
  }
  display2.setCursor((32-int(clock.length())*6)/2,55);
  display2.print(clock);
  if(display2BoltActive){
    // Large lightning silhouette; normal content returns after two seconds.
    display2.fillTriangle(19,38,6,65,17,65,SSD1306_WHITE);
    display2.fillTriangle(14,59,26,59,11,88,SSD1306_WHITE);
    display2.display();return;
  }
  display2AppIcon();
  if(bell){
    display2.fillCircle(16,73,10,SSD1306_WHITE);display2.fillRect(6,73,21,16,SSD1306_WHITE);
    display2.fillTriangle(6,84,2,92,30,92,SSD1306_WHITE);display2.fillCircle(16,97,3,SSD1306_WHITE);
    display2.fillCircle(16,61,2,SSD1306_WHITE);
  }else {
  if(seqIcon&&!notice){
    if(seqIcon==1)display2.fillTriangle(7,92,7,112,27,102,SSD1306_WHITE);
    else display2.fillRect(6,93,21,19,SSD1306_WHITE);
  }
  if(notice){
    display2.drawFastHLine(0,90,32,SSD1306_WHITE);
    for(int i=0;i<3;i++)display2Line(94+i*11,appNotice[i]);
  }else if(timer.length()){
  display2.drawFastHLine(0,90,32,SSD1306_WHITE);

  // Narrow colon keeps even 999:59 inside the portrait 32-pixel width.
  int timerX=0;
  for(unsigned int i=0;i<remaining.length();i++){
    if(remaining[i]==':'){
      display2.drawPixel(timerX,108,SSD1306_WHITE);
      display2.drawPixel(timerX,111,SSD1306_WHITE);
      timerX+=2;
    }else{
      display2.setCursor(timerX,106);display2.print(remaining[i]);timerX+=6;
    }
  }
  }
  }
  display2.display();
}

String display2Status(){return display2Ready?String("INITIALIZED"):String("NOT FOUND");}
