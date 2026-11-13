#pragma once
#include <TinyGPSPlus.h>
static constexpr int GPS_POWER=1,GPS_RX=44,GPS_TX=43;
// Active-high control, matching the observed GPS power switch on this device.
static constexpr uint8_t GPS_POWER_ON=HIGH,GPS_POWER_OFF=LOW;
HardwareSerial gpsUart(1);
TinyGPSPlus gpsParser;
TinyGPSCustom gpsGsa(gpsParser,"GPGSA",2);
bool gpsPowered=false;
static const uint32_t gpsBaudOptions[]={4800,9600,19200,38400,57600,115200};
uint32_t gpsBaud=9600;
bool gpsSettingsOpen=false;
int gpsBaudRow=1;
String gpsSettingsError;
void gpsLoadSettings(){
 uint32_t saved=prefs.getUInt("gpsBaud",9600);gpsBaud=9600;
 for(uint32_t rate:gpsBaudOptions)if(rate==saved)gpsBaud=rate;
}
unsigned long gpsOpenedAt=0,gpsLastByte=0,gpsLastValid=0,gpsPaintAt=0;
String gpsPainted;
void gpsInit(){digitalWrite(GPS_POWER,GPS_POWER_OFF);pinMode(GPS_POWER,OUTPUT);pinMode(GPS_RX,INPUT);pinMode(GPS_TX,INPUT);}
void gpsStop(){
 if(gpsPowered)gpsUart.end();
 pinMode(GPS_RX,INPUT);pinMode(GPS_TX,INPUT);
 digitalWrite(GPS_POWER,GPS_POWER_OFF);gpsPowered=false;
}
void gpsStart(){
 gpsStop();gpsParser=TinyGPSPlus();gpsGsa.begin(gpsParser,"GPGSA",2);
 gpsLastByte=gpsLastValid=0;gpsPainted="";
 digitalWrite(GPS_POWER,GPS_POWER_ON);gpsPowered=true;gpsOpenedAt=millis();
 gpsUart.setRxBufferSize(2048);gpsUart.begin(gpsBaud,SERIAL_8N1,GPS_RX,GPS_TX);
}
String gpsSnapshot(){
 bool nmea=gpsLastValid&&millis()-gpsLastValid<5000;
 bool location=nmea&&gpsParser.location.isValid()&&gpsParser.location.age()<5000;
 bool gsa=nmea&&gpsGsa.isValid()&&gpsGsa.age()<5000;
 int fix=gsa?atoi(gpsGsa.value()):0;
 bool coords=location&&(!gsa||fix==2||fix==3);
 String rx=nmea?"NMEA OK":gpsLastByte&&millis()-gpsLastByte<5000?"UART DATA / NO NMEA":millis()-gpsOpenedAt<10000?"WAITING FOR DATA":"NOT FOUND / NO DATA";
 String state=!nmea?"--":gsa?(fix==3&&coords?"3D FIX":fix==2&&coords?"2D FIX":"NO FIX"):"FIX TYPE --";
 String sats=nmea&&gpsParser.satellites.isValid()&&gpsParser.satellites.age()<5000?String(gpsParser.satellites.value()):"--";
 return rx+"\n"+state+"\n"+(coords?String(gpsParser.location.lat(),6):"--")+"\n"+(coords?String(gpsParser.location.lng(),6):"--")+"\n"+sats+"\n"+(coords&&fix==3&&gpsParser.altitude.isValid()&&gpsParser.altitude.age()<5000?String(gpsParser.altitude.meters(),1)+" m":"--");
}
void gpsDraw(const String &snap){
 String lines[6];int from=0;for(int i=0;i<6;i++){int p=snap.indexOf('\n',from);lines[i]=p<0?snap.substring(from):snap.substring(from,p);from=p+1;}
 beginDraw();do{
 header("","GPS","S: UART SETTINGS   ESC / BACKSPACE: CLOSE GPS");
 epd.drawRect(18,82,380,348,GxEPD_BLACK);epd.drawRect(414,82,368,348,GxEPD_BLACK);
 text("RECEIVER",34,118,true);text("POSITION",430,118,true);
 text("POWER: ON REQUESTED",34,165);text("UART: "+String(gpsBaud),34,205);text(lines[0],34,245);text(lines[1],34,295,true);text("SATELLITES: "+lines[4],34,345);text("OUTDOORS FOR FIRST FIX",34,395);
 text("LATITUDE",430,165);text(lines[2],430,205,true);text("LONGITUDE",430,255);text(lines[3],430,295,true);text("ALTITUDE",430,345);text(lines[5],430,385,true);
 if(gpsSettingsOpen){
  epd.fillRect(165,76,470,365,GxEPD_WHITE);
  epd.drawRoundRect(165,76,470,365,16,GxEPD_BLACK);
  text("GPS UART SPEED",195,111,true);
  text("UP/DOWN: SELECT  ENTER: SAVE",185,143);
  for(int i=0;i<6;i++)menuOvalLabel(String(gpsBaudOptions[i])+" baud",275,180+i*34,i==gpsBaudRow);
  text(gpsSettingsError.length()?gpsSettingsError:"ESC / BACKSPACE: CANCEL",185,412);
 }
 }while(epd.nextPage());gpsPainted=snap;
}
void gpsKey(uint8_t k){
 if(!gpsSettingsOpen){
  if(k=='s'||k=='S'){
   gpsSettingsOpen=true;gpsSettingsError="";
   for(int i=0;i<6;i++)if(gpsBaudOptions[i]==gpsBaud)gpsBaudRow=i;
   gpsDraw(gpsSnapshot());
  }
  return;
 }
 if(k==27||k==8){gpsSettingsOpen=false;gpsDraw(gpsSnapshot());return;}
 if(k==0xB5||k==0xB6){gpsBaudRow=(gpsBaudRow+(k==0xB5?5:1))%6;gpsSettingsError="";gpsDraw(gpsSnapshot());return;}
 if(k==13){
  uint32_t chosen=gpsBaudOptions[gpsBaudRow];
  if(prefs.putUInt("gpsBaud",chosen)!=sizeof(chosen)||prefs.getUInt("gpsBaud",0)!=chosen){
   gpsSettingsError="SD SAVE FAILED - RETRY";gpsDraw(gpsSnapshot());return;
  }
  gpsSettingsOpen=false;
  if(chosen!=gpsBaud){gpsBaud=chosen;gpsStart();}
  gpsPaintAt=millis();gpsDraw(gpsSnapshot());
 }
}
void gpsOpen(){gpsSettingsOpen=false;gpsLoadSettings();gpsStart();view=GPS_APP;gpsPaintAt=millis();gpsDraw(gpsSnapshot());}
void gpsTick(){
 if(!gpsPowered)return;
 if(view!=GPS_APP){gpsStop();return;}
 int budget=4096;while(budget--&&gpsUart.available()){char c=gpsUart.read();gpsLastByte=millis();if(gpsParser.encode(c))gpsLastValid=millis();}
 if(millis()-gpsPaintAt>=5000){gpsPaintAt=millis();String s=gpsSnapshot();if(!gpsSettingsOpen&&s!=gpsPainted)gpsDraw(s);}
}
