#pragma once
// Independent instrument: existing sequencer data and recordings are untouched.
struct GroovePattern {uint16_t bpm;uint8_t swing,level[6],tone[6],mute;uint8_t cell[6][16];};
struct GrooveSong {uint32_t magic;GroovePattern bank[4];};
static GrooveSong groove;
static int gbBank=0,gbTrack=0,gbCol=0,gbSolo=-1;
static bool gbDirty=false;
static volatile bool gbBusy=false,gbHalt=false,gbError=false;
static volatile int gbStep=-1;
static String gbMessage;
static const char* gbNames[]={"KICK","SNARE","HAT","CLAP","BASS","LEAD"};
static portMUX_TYPE gbMux=portMUX_INITIALIZER_UNLOCKED;
static void gbStop(){gbHalt=true;while(gbBusy)delay(1);gbStep=-1;}
static bool gbSave(){
 bool ok=prefs.putBytes("groove1",&groove,sizeof(groove))==sizeof(groove);
 GrooveSong check={};ok=ok&&prefs.getBytes("groove1",&check,sizeof(check))==sizeof(check)&&!memcmp(&check,&groove,sizeof(check));
 if(ok)gbDirty=false;gbMessage=ok?"SAVED + READ VERIFIED / TF":"SAVE FAILED / CHECK TF CARD";return ok;
}
static void gbWorker(void*){
 uint32_t age[6]={240000,240000,240000,240000,240000,240000},rng=1234567,left=0;
 float phase[6]={},filter[6]={};uint8_t hit[6]={};int step=0,solo=-1;GroovePattern p={};bool ok=true;
 digitalWrite(39,HIGH);delay(10);
 while(!gbHalt&&ok){
  int16_t out[240];
  for(int i=0;i<240;i++){
   if(!left){
    portENTER_CRITICAL(&gbMux);p=groove.bank[gbBank];solo=gbSolo;portEXIT_CRITICAL(&gbMux);
    gbStep=step;
    for(int t=0;t<6;t++)if(p.cell[t][step]&&!(p.mute&(1<<t))&&(solo<0||solo==t)){hit[t]=p.cell[t][step];age[t]=0;phase[t]=0;}
    uint32_t base=360000UL/p.bpm;left=base*(100+(step%2?-int(p.swing):int(p.swing)))/100;step=(step+1)%16;
   }
   --left;float mix=0;
   rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;float noise=int32_t(rng)/2147483648.0f;
   for(int t=0;t<6;t++){
    float time=age[t]/24000.0f;if(time>0.6f)continue;
    float freq=t==0?48+120*expf(-time*35):t==1?180:t==2?6500:t==3?900:440*powf(2,(int(hit[t])-69)/12.0f);
    phase[t]+=freq/24000;phase[t]-=floorf(phase[t]);
    float wave=sinf(2*PI*phase[t]),decay=t==0?18:t==1?25:t==2?90:t==3?22:t==4?9:6;
    if(t==1)wave=0.8f*noise+0.2f*wave;
    if(t==2)wave=noise;
    if(t==3)wave=noise*(time<0.035f?(fmodf(time,0.012f)<0.006f?1:0):0.7f);
    if(t>=4){wave=t==4?(phase[t]<0.5f?0.7f:-0.7f):2*phase[t]-1;filter[t]+=(0.02f+p.tone[t]/110.0f)*(wave-filter[t]);wave=filter[t];}
    float env=expf(-time*decay)*min(1.0f,age[t]/72.0f);
    if((p.mute&(1<<t))||(solo>=0&&solo!=t))env=0;
    mix+=wave*env*p.level[t]*45;age[t]++;
   }
   out[i]=int16_t(24000*tanhf(mix/24000));
  }
  ok=audioBus.write((uint8_t*)out,sizeof(out))==sizeof(out);
 }
 int16_t silence[240]={};audioBus.write((uint8_t*)silence,sizeof(silence));delay(25);digitalWrite(39,LOW);
 gbError=!ok;gbStep=-1;gbBusy=false;vTaskDelete(nullptr);
}
static void gbDraw(){
 view=GROOVE_BOX;menuActive=false;readPower();
 GroovePattern p;portENTER_CRITICAL(&gbMux);p=groove.bank[gbBank];portEXIT_CRITICAL(&gbMux);
 beginDraw();do{
  epd.fillScreen(GxEPD_WHITE);
  text("GROOVE BOX",18,34,true);text("BANK "+String(char('A'+gbBank))+"   "+String(p.bpm)+" BPM   SWING "+String(p.swing)+"%",300,34);
  text(rtcTime()+"   BAT: "+batteryValue,18,65);
  for(int c=0;c<16;c++)text(String(c+1),166+c*38,95);
  for(int t=0;t<6;t++){
   int y=108+t*37;text(gbNames[t],18,y+25);
   if(p.mute&(1<<t))text("M",126,y+25);else if(gbSolo==t)text("S",126,y+25);
   for(int c=0;c<16;c++){
    int x=162+c*38;epd.drawRect(x,y,33,31,GxEPD_BLACK);
    if(p.cell[t][c])epd.fillRect(x+7,y+7,19,17,GxEPD_BLACK);
    if(t==gbTrack&&c==gbCol){epd.drawRect(x-2,y-2,37,35,GxEPD_BLACK);epd.drawRect(x+2,y+2,29,27,GxEPD_BLACK);}
   }
  }
  if(gbBusy)epd.fillTriangle(20,346,20,368,40,357,GxEPD_BLACK);else epd.fillRect(20,346,20,20,GxEPD_BLACK);
  text(gbBusy?"PLAY":"STOP",50,365,true);
  text(String(gbNames[gbTrack])+"  LVL "+String(p.level[gbTrack])+"  TONE "+String(p.tone[gbTrack])+"  "+(gbTrack>=4?seqNote(p.cell[gbTrack][gbCol]):String("DRUM")),180,365);
  text(gbDirty?"UNSAVED": "SAVED",18,397);text(gbMessage,180,397);
  text("ARROWS: CELL  ENTER: HIT  SPACE: PLAY  +/-: BPM",18,424);
  text("A-D: BANK  M: MUTE  O: SOLO  [/]: NOTE  S: SAVE",18,446);
  text("J/K: LEVEL  U/I: TONE  W/E: SWING  ESC: SAVE/BACK",18,468);
 }while(epd.nextPage());
}
static void gbOpen(){
 seqStop();gbStop();gbSolo=-1;gbBank=gbTrack=gbCol=0;
 bool valid=prefs.getBytesLength("groove1")==sizeof(groove)&&prefs.getBytes("groove1",&groove,sizeof(groove))==sizeof(groove)&&groove.magic==0x47524231;
 if(valid)for(auto &p:groove.bank){if(p.bpm<40||p.bpm>240||p.swing>60)valid=false;for(int t=0;t<6;t++){if(p.level[t]>100||p.tone[t]>100)valid=false;for(auto n:p.cell[t])if(n&&(t<4?n!=1:n<24||n>96))valid=false;}}
 if(!valid){memset(&groove,0,sizeof(groove));groove.magic=0x47524231;
  for(auto &p:groove.bank){p.bpm=120;for(int t=0;t<6;t++){p.level[t]=t==2?35:65;p.tone[t]=35;}for(int c=0;c<16;c++){p.cell[0][c]=c%4==0;p.cell[1][c]=c%8==4;p.cell[2][c]=c%2==0;p.cell[4][c]=c%4==0?uint8_t(33+(c/4%2)*3):0;}}
 }
 gbDirty=!valid;gbMessage=valid?"TF PATTERNS RESTORED":"NEW STARTER BEAT / NOT SAVED";gbDraw();
}
static void gbKey(uint8_t k){
 if(k>='A'&&k<='Z')k+=32;
 if(k==27||k==8){gbStop();if(gbDirty&&!gbSave()){gbDraw();return;}showMenu(334,HOME);return;}
 if(k==' '){if(gbBusy)gbStop();else if(!codecReady||!audioBusReady||recBusy||seqBusy)gbMessage="AUDIO NOT READY / BUSY";else {gbHalt=false;gbError=false;gbBusy=true;if(xTaskCreate(gbWorker,"groove",6144,nullptr,2,nullptr)!=pdPASS){gbBusy=false;gbMessage="AUDIO TASK FAILED";}}gbDraw();return;}
 if(k=='s'){gbSave();gbDraw();return;}
 portENTER_CRITICAL(&gbMux);
 auto &p=groove.bank[gbBank];bool edit=true;
 if(k==0xB4){gbCol=(gbCol+15)%16;edit=false;}else if(k==0xB7){gbCol=(gbCol+1)%16;edit=false;}
 else if(k==0xB5){gbTrack=(gbTrack+5)%6;edit=false;}else if(k==0xB6){gbTrack=(gbTrack+1)%6;edit=false;}
 else if(k>='a'&&k<='d'){gbBank=k-'a';edit=false;}
 else if(k==13||k==10)p.cell[gbTrack][gbCol]=p.cell[gbTrack][gbCol]?0:(gbTrack<4?1:gbTrack==4?33:69);
 else if(k=='+'||k=='=')p.bpm=min(240,int(p.bpm)+1);else if(k=='-')p.bpm=max(40,int(p.bpm)-1);
 else if(k=='m')p.mute^=1<<gbTrack;
 else if(k=='o'){gbSolo=gbSolo==gbTrack?-1:gbTrack;edit=false;}
 else if(k=='j'||k=='k')p.level[gbTrack]=constrain(int(p.level[gbTrack])+(k=='k'?5:-5),0,100);
 else if(k=='u'||k=='i')p.tone[gbTrack]=constrain(int(p.tone[gbTrack])+(k=='i'?5:-5),0,100);
 else if(k=='w'||k=='e')p.swing=constrain(int(p.swing)+(k=='e'?5:-5),0,60);
 else if((k=='['||k==']')&&gbTrack>=4)p.cell[gbTrack][gbCol]=constrain(int(p.cell[gbTrack][gbCol]?p.cell[gbTrack][gbCol]:48)+(k==']'?1:-1),24,96);
 else edit=false;
 portEXIT_CRITICAL(&gbMux);if(edit){gbDirty=true;gbMessage="";}gbDraw();
}
static void gbTick(){if(view==GROOVE_BOX&&gbError){gbError=false;gbMessage="I2S WRITE FAILED";gbDraw();}}
