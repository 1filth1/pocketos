#pragma once
static bool quickOpen=false;
static uint32_t quickLastActivity=0;
static uint8_t *quickBackground=nullptr;
static View quickSavedView,quickSavedMenuView;
static bool quickSavedActive;
static int quickSavedContext,quickSavedRow;
static void quickClose(){
 if(!quickOpen)return;
 quickOpen=false;
 view=quickSavedView;menuView=quickSavedMenuView;menuActive=quickSavedActive;
 menuContext=quickSavedContext;menuRow=quickSavedRow;
 if(menuActive)buildMenu(menuContext);
 memcpy(epd.getBuffer(),quickBackground,48000);epd.nextPage();
}
void quickShow(){
 if(reminderPopupActive)return;
 if(!quickOpen){
  if(!quickBackground)quickBackground=(uint8_t*)ps_malloc(48000);
  if(!quickBackground){status="POPUP: NO MEMORY";return;}
  memcpy(quickBackground,epd.getBuffer(),48000);
  quickSavedView=view;quickSavedMenuView=menuView;quickSavedActive=menuActive;
  quickSavedContext=menuContext;quickSavedRow=menuRow;
  quickOpen=true;menuRow=0;
 }
 menuContext=340;menuView=HOME;menuActive=true;buildMenu(340);
 quickLastActivity=millis();
 memcpy(epd.getBuffer(),quickBackground,48000);
 epd.fillRect(135,38,530,404,GxEPD_WHITE);
 epd.drawRect(135,38,530,404,GxEPD_BLACK);epd.drawRect(138,41,524,398,GxEPD_BLACK);
 text("QUICK",158,72,true);
 text("AUTO CLOSE: 5 SEC",350,72);
 for(int i=0;i<menuCount;i++){
  String label=menuItems[i].label;
  if(i==0)label=codecReady?"VOLUME < "+String(audioVolume)+"% >":"VOLUME: --";
  int y=92+i*42;
  if(i==menuRow){epd.fillRect(151,y,498,38,GxEPD_BLACK);inv(label,164,y+26);}
  else text(label,164,y+26);
 }
 text("ARROWS / ENTER   ESC: CLOSE",158,424);
 epd.nextPage();
 // Count the five seconds from completion of the e-paper refresh.
 quickLastActivity=millis();
}
static void quickKey(uint8_t k){
 if(k==27||k==8){quickClose();return;}
 quickLastActivity=millis();deviceActivity=millis();
 if(k==0xB5||k==0xB6){menuRow=(menuRow+menuCount+(k==0xB5?-1:1))%menuCount;quickShow();return;}
 if((k==0xB4||k==0xB7)&&menuRow!=0)return;
 if(k==0xB4||k==0xB7||(k==10||k==13)){
  if(menuRow<2){route(k);return;}
  int chosen=menuRow;
  quickClose();
  menuContext=340;menuView=HOME;menuActive=true;buildMenu(340);menuRow=chosen;
  route(k);
 }
}
static void quickToggle(){
 if(reminderPopupActive)return;
 escState=0;deviceActivity=millis();
 if(quickOpen)quickClose();else quickShow();
}
// Waveshare button_bsp: Boot_KEY=0, Boot_active=0, pull-up enabled.
static void quickBootPoll(){
 static bool raw=false,stable=false,armed=false;
 static uint32_t changed=0,pressed=0;
 bool down=digitalRead(0)==LOW;uint32_t now=millis();
 if(down!=raw){raw=down;changed=now;}
 if(now-changed<30||stable==raw)return;
 stable=raw;
 if(stable){pressed=now;armed=true;}
 else if(armed){armed=false;if(now-pressed<800){deviceActivity=now;display2ActivityReset();quickToggle();}}
}
