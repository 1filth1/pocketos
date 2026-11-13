#pragma once
// Audio runs separately so slow e-paper refreshes cannot interrupt the stream.
static volatile bool recBusy=false,recStop=false;
static volatile int recMode=0,recPeak=0,recOutcome=0;
static volatile uint32_t recBytes=0;
static String recFile;
static int recButton=0,recFrame=0;
static unsigned long recPaint=0;
static bool recWasBusy=false;
static volatile uint16_t recWave[96]={};
static volatile unsigned recWaveHead=0;
static unsigned long recVolumeUntil=0;
static bool recVolumeVisible=false;
static void recHeader(uint8_t *h,uint32_t n){
 memset(h,0,44);memcpy(h,"RIFF",4);memcpy(h+8,"WAVEfmt ",8);memcpy(h+36,"data",4);
 auto w32=[&](int o,uint32_t v){for(int i=0;i<4;i++)h[o+i]=v>>(i*8);};
 auto w16=[&](int o,uint16_t v){h[o]=v;h[o+1]=v>>8;};
 w32(4,n+36);w32(16,16);w16(20,1);w16(22,1);w32(24,24000);w32(28,48000);w16(32,2);w16(34,16);w32(40,n);
}
static void recWorker(void*){
 bool recording=recMode==1,ok=true;uint8_t h[44];int16_t data[480];
 File f=SD_MMC.open(recFile,recording?FILE_WRITE:FILE_READ);
 if(!f)ok=false;
 uint32_t remaining=0;
 if(ok&&recording){recHeader(h,0);ok=f.write(h,44)==44;}
 if(ok&&!recording){
  ok=f.read(h,44)==44;
  uint8_t expected[44];recHeader(expected,0);
  for(int i=0;i<44&&ok;i++)if(!((i>=4&&i<8)||i>=40)&&h[i]!=expected[i])ok=false;
  if(ok){remaining=uint32_t(h[40])|(uint32_t(h[41])<<8)|(uint32_t(h[42])<<16)|(uint32_t(h[43])<<24);ok=remaining<=f.size()-44&&(remaining%2)==0;}
  if(ok){digitalWrite(39,HIGH);delay(10);}
 }
 while(ok&&!recStop&&(recording||remaining)){
  size_t n=recording?sizeof(data):min(uint32_t(sizeof(data)),remaining);
  if(recording){n=audioBus.readBytes((char*)data,n);if(n!=sizeof(data)){ok=false;break;}ok=f.write((uint8_t*)data,n)==n;}
  else {ok=f.read((uint8_t*)data,n)==n;if(ok)ok=audioBus.write((uint8_t*)data,n)==n;remaining-=n;}
  int peak=0;for(size_t i=0;i<n/2;i++)peak=max(peak,abs(int(data[i])));recPeak=peak;
  // One real peak per audio block, never a decorative/generated waveform.
  recWave[recWaveHead%96]=peak;recWaveHead++;
  if(ok)recBytes+=n;
  // Bound WAV size and stop safely before the card fills.
  if(recording&&recBytes>=48000UL*60*60)break;
  vTaskDelay(1);
 }
 if(recording&&f){recHeader(h,recBytes);ok=f.seek(0)&&f.write(h,44)==44&&ok;f.flush();}
 if(f)f.close();
 if(recording&&ok){File verify=SD_MMC.open(recFile);ok=verify&&verify.size()==recBytes+44;if(verify)verify.close();}
 if(!recording){memset(data,0,sizeof(data));audioBus.write((uint8_t*)data,sizeof(data));delay(25);}
 digitalWrite(39,LOW);recOutcome=ok?(recording?1:2):-1;recPeak=0;recBusy=false;vTaskDelete(nullptr);
}
void recorderDraw(){
 view=RECORDER;recPaint=millis();
 uint16_t wave[96];unsigned head=recWaveHead;
 for(int i=0;i<96;i++)wave[i]=recWave[(head+i)%96];
 uint32_t seconds=recBytes/48000;
 char timer[24];snprintf(timer,sizeof(timer),"%02lu:%02lu:%02lu",(unsigned long)(seconds/3600),(unsigned long)(seconds/60%60),(unsigned long)(seconds%60));
 beginDraw();do{
 epd.fillScreen(GxEPD_WHITE);drawTopStatus(stat());
 // Large Akai-style metal reels; only the transport keys sit below the deck.
 const int phase=(recBytes/6000)%24;
 auto caption=[&](const String &label,int baseline){
  epd.setFont(&FreeMono9pt7b);epd.setTextSize(1);epd.setTextColor(GxEPD_BLACK);
  int16_t x,y;uint16_t w,h;epd.getTextBounds(label,0,0,&x,&y,&w,&h);
  epd.setCursor((W-w)/2-x,baseline);epd.print(label);
 };
 caption(recBusy?(recMode==1?"REC":"PLAY"):"STOP",180);
 caption(timer,208);
 if(recOutcome==-1)caption("ERROR",239);
 else if(recOutcome==1)caption("SAVED",239);
 for(int reel=0;reel<2;reel++){
  const int cx=reel?615:185,cy=207;
  epd.drawCircle(cx,cy,158,GxEPD_BLACK);
  epd.drawCircle(cx,cy,155,GxEPD_BLACK);
  epd.drawCircle(cx,cy,148,GxEPD_BLACK);
  // Three broad curved cut-outs in the flange, not triangular spokes.
  // All vertices share the same rigid rotation, including the hub bolts.
  for(int slot=0;slot<3;slot++){
   float base=phase*0.261799388f+slot*2.094395102f;
   for(int step=0;step<24;step++){
    float a=base-0.49f+step*0.98f/24;
    float b=base-0.49f+(step+1)*0.98f/24;
    int x1=cx+lroundf(62*cosf(a)),y1=cy+lroundf(62*sinf(a));
    int x2=cx+lroundf(133*cosf(a)),y2=cy+lroundf(133*sinf(a));
    int x3=cx+lroundf(133*cosf(b)),y3=cy+lroundf(133*sinf(b));
    int x4=cx+lroundf(62*cosf(b)),y4=cy+lroundf(62*sinf(b));
    epd.fillTriangle(x1,y1,x2,y2,x3,y3,GxEPD_BLACK);
    epd.fillTriangle(x1,y1,x3,y3,x4,y4,GxEPD_BLACK);
   }
   float bolt=base+1.047197551f;
   int bx=cx+lroundf(41*cosf(bolt)),by=cy+lroundf(41*sinf(bolt));
   epd.drawCircle(bx,by,4,GxEPD_BLACK);
   epd.drawLine(bx-2,by,bx+2,by,GxEPD_BLACK);
  }
  epd.drawCircle(cx,cy,30,GxEPD_BLACK);
  epd.fillCircle(cx,cy,18,GxEPD_BLACK);
  epd.fillCircle(cx,cy,7,GxEPD_WHITE);
 }
 // Symmetric guide rollers on one level. Real audio peaks form the tape.
 epd.drawLine(61,296,82,378,GxEPD_BLACK);
 epd.drawLine(739,296,718,378,GxEPD_BLACK);
 for(int x: {96,704}){
  epd.fillCircle(x,378,15,GxEPD_BLACK);
  epd.fillCircle(x,378,11,GxEPD_WHITE);
  epd.drawCircle(x,378,6,GxEPD_BLACK);
  epd.fillCircle(x,378,2,GxEPD_BLACK);
 }
 epd.drawLine(96,393,145,393,GxEPD_BLACK);
 epd.drawLine(655,393,704,393,GxEPD_BLACK);
 int previous=393;
 for(int i=0;i<96;i++){
  int amplitude=min(16,int(wave[i])*16/16000);
  int y=393+((i&1)?amplitude:-amplitude);
  int x=145+i*510/95;
  if(i)epd.drawLine(145+(i-1)*510/95,previous,x,y,GxEPD_BLACK);
  previous=y;
 }
 for(int i=0;i<2;i++){
  int x=26+i*386;bool selected=i==recButton;
  uint16_t ink=selected?GxEPD_WHITE:GxEPD_BLACK;
  epd.fillRoundRect(x,423,360,45,9,selected?GxEPD_BLACK:GxEPD_WHITE);
  epd.drawRoundRect(x,423,360,45,9,GxEPD_BLACK);
  bool active=recBusy&&recMode==i+1;
  if(active)epd.fillRect(x+22,436,19,19,ink);
  else if(i==0)epd.fillCircle(x+31,445,10,ink);
  else epd.fillTriangle(x+22,434,x+22,457,x+43,445,ink);
  epd.setFont(&FreeSansBold12pt7b);epd.setTextSize(1);epd.setTextColor(ink);epd.setCursor(x+65,454);
  epd.print(active?"STOP":i==0?"REC":"PLAY");
 }
 if(recVolumeVisible){
  epd.fillRoundRect(650,155,138,227,10,GxEPD_WHITE);epd.drawRoundRect(650,155,138,227,10,GxEPD_BLACK);
  text("VOLUME",665,184,true);text(String(audioVolume)+"%",678,218,true);
  epd.drawRect(704,236,30,126,GxEPD_BLACK);
  int h=audioVolume*120/100;epd.fillRect(707,359-h,24,h,GxEPD_BLACK);
 }
 epd.setTextColor(GxEPD_BLACK);epd.setTextSize(1);
 }while(epd.nextPage());
}
void recorderOpen(){if(!recFile.length())recFile=prefs.getString("lastRec","");status="READY";recOutcome=0;recVolumeVisible=false;recorderDraw();}
void recorderKey(uint8_t k){
 if(k==0xB5||k==0xB6){
  if(!audioSetVolume(audioVolume+(k==0xB5?5:-5)))status=audioResult;
  recVolumeVisible=true;recVolumeUntil=millis()+2000;recorderDraw();return;
 }
 if(k==0xB4||k==0xB7){recButton=1-recButton;recorderDraw();return;}
 if(k==27||k==8){if(recBusy){recStop=true;status="STOPPING...";recorderDraw();}else menuScreen(HOME);return;}
 if(k!=10&&k!=13)return;
 if(recBusy){if(recMode==recButton+1){recStop=true;status="STOPPING...";recorderDraw();}return;}
 recOutcome=0;
 if(!cardOK){status="TF CARD NOT MOUNTED";recorderDraw();return;}
 if(!codecReady||!audioBusReady){status="AUDIO NOT READY";recorderDraw();return;}
 if(recButton==0){
  if(es8311_microphone_gain_set(audioCodec,ES8311_MIC_GAIN_24DB)!=ESP_OK){status="MIC CONFIG ERROR";recorderDraw();return;}
  if(!SD_MMC.exists("/RECORDINGS")&&!SD_MMC.mkdir("/RECORDINGS")){status="FOLDER CREATE FAILED";recorderDraw();return;}
  String p;for(unsigned i=1;i<=99999;i++){char b[40];snprintf(b,sizeof(b),"/RECORDINGS/REC%05u.WAV",i);if(!SD_MMC.exists(b)){p=b;break;}}
  if(!p.length()){status="RECORDING NAMES FULL";recorderDraw();return;}recFile=p;
 }else if(!recFile.length()||!SD_MMC.exists(recFile)){status="NO RECORDING FOUND";recorderDraw();return;}
 for(int i=0;i<96;i++)recWave[i]=0;recWaveHead=0;
 recMode=recButton==0?1:2;recStop=false;recBytes=0;recPeak=0;recBusy=true;recWasBusy=true;status="";
 if(xTaskCreate(recWorker,"recorder",6144,nullptr,2,nullptr)!=pdPASS){recBusy=false;recWasBusy=false;recOutcome=-1;}
 recorderDraw();
}
void recorderTick(){
 if(recBusy)deviceActivity=millis();
 if(recWasBusy&&!recBusy){recWasBusy=false;appNotify("AUDIO",recOutcome==1?"SAVED":recOutcome==-1?"ERROR":"STOP",recOutcome==1?"TF":"");if(recOutcome==1)prefs.putString("lastRec",recFile);if(view==RECORDER)recorderDraw();}
 if(recVolumeVisible&&(int32_t)(millis()-recVolumeUntil)>=0){recVolumeVisible=false;if(view==RECORDER)recorderDraw();}
 if(view==RECORDER&&recBusy&&millis()-recPaint>=700){recFrame++;recorderDraw();}
}
