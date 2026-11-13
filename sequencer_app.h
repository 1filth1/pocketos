#pragma once
// Four synthesized voices, mixed at 24 kHz. The audio task never draws e-paper.
struct SeqPattern {uint32_t magic;uint16_t bpm;uint8_t cells[4][16];};
static SeqPattern seq={};
// Lengths are stored separately to preserve existing patterns on the TF card.
static uint8_t seqLengths[16]={};
static uint8_t pianoNotes[16]={},pianoLengths[16]={};
static int pianoCursor=0,pianoLength=1,pianoOctave=4;
static String pianoMessage;
static const char* pianoKeys="awsedftgyhujk";
static void pianoDraw();
static void pianoKey(uint8_t k);
static portMUX_TYPE seqMux=portMUX_INITIALIZER_UNLOCKED;
static volatile bool seqBusy=false,seqHalt=false,seqError=false;
static int seqTrack=0,seqColumn=0;
static bool seqCodecBoost=false;
static void seqRestoreVolume(){
 if(seqCodecBoost&&!seqBusy){es8311_voice_volume_set(audioCodec,audioVolume,nullptr);seqCodecBoost=false;}
}
static bool seqDirty=false;
static uint8_t seqVolume=80,seqNotes[4]={36,60,84,36};
static volatile int seqPlayingStep=-1;
static int seqShownStep=-1,seqPopup=0,seqDraft=0;
static unsigned long seqVisualAt=0;
static String seqMessage;
static const char* seqNames[]={"KICK","SNARE","HI-HAT","BASS"};
static void seqStop(){
 seqHalt=true;while(seqBusy)delay(1);seqPlayingStep=-1;seqRestoreVolume();
}
static bool seqSave(){
 bool ok=prefs.putBytes("seqPattern",&seq,sizeof(seq))==sizeof(seq);
 SeqPattern check={};ok=ok&&prefs.getBytes("seqPattern",&check,sizeof(check))==sizeof(check)&&!memcmp(&check,&seq,sizeof(seq));
 ok=ok&&prefs.putUChar("seqVolume",seqVolume)==1;
 ok=ok&&prefs.putBytes("seqNotes",seqNotes,sizeof(seqNotes))==sizeof(seqNotes);
 ok=ok&&prefs.putBytes("seqLengths",seqLengths,sizeof(seqLengths))==sizeof(seqLengths);
 uint8_t lengthsCheck[16]={};
 ok=ok&&prefs.getBytes("seqLengths",lengthsCheck,sizeof(lengthsCheck))==sizeof(lengthsCheck)&&!memcmp(lengthsCheck,seqLengths,sizeof(seqLengths));
 if(ok)seqDirty=false;seqMessage=ok?"SAVED / TF CARD":"SAVE FAILED";return ok;
}
static void seqWorker(void*){
 float phase[4]={};uint32_t age[4]={240000,240000,240000,240000};
 uint32_t bassGate=0;
 uint8_t voices[4]={};uint8_t notes[4]={};float gain=0;uint32_t left=0,rng=0x12345678;int step=0;bool ok=true;
 digitalWrite(39,HIGH);delay(10);
 while(!seqHalt&&ok){
  int16_t out[240];
  portENTER_CRITICAL(&seqMux);
  gain=seqVolume/100.0f;memcpy(notes,seqNotes,sizeof(notes));
  portEXIT_CRITICAL(&seqMux);
  for(int i=0;i<240;i++){
   if(!left){
    uint16_t bpm;
    portENTER_CRITICAL(&seqMux);
    bpm=seq.bpm;seqPlayingStep=step;
    for(int t=0;t<4;t++){uint8_t n=seq.cells[t][step];if(n){voices[t]=n;age[t]=0;phase[t]=0;if(t==3)bassGate=(360000UL/bpm)*max(1,int(seqLengths[step]));}}
    portEXIT_CRITICAL(&seqMux);
    left=360000UL/bpm;step=(step+1)%16;
   }
   left--;float mix=0;
   rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;
   float noise=(int32_t(rng)/2147483648.0f);
   for(int t=0;t<4;t++){
    float time=age[t]/24000.0f;if(t==3?age[t]>=bassGate:time>0.4f)continue;
    float freq=t==0?48+110*expf(-time*40):t==1?180:t==2?7000:440*powf(2,(int(voices[t])-69)/12.0f);
    if(t<3)freq*=powf(2,(int(notes[t])-(t==0?36:t==1?60:84))/12.0f);
    phase[t]+=freq/24000;phase[t]-=floorf(phase[t]);
    float wave=sinf(2*PI*phase[t]);
    if(t==1)wave=0.8f*noise+0.2f*wave;
    if(t==2)wave=noise*(phase[t]<0.5f?1.0f:0.15f);
    float env=expf(-time*(t==0?18:t==1?25:t==2?85:12));
    if(t==3)env=0.75f*min(1.0f,(bassGate-age[t])/240.0f);
    env*=min(1.0f,age[t]/48.0f);
    mix+=wave*env*(t==0?5500:t==1?3300:t==2?1600:3200);age[t]++;
   }
   // Louder mix with a soft limiter: preserve overlapping hits without hard clipping.
   float amplified=mix*gain*4.0f;
   out[i]=int16_t(30000.0f*tanhf(amplified/30000.0f));
  }
  ok=audioBus.write((uint8_t*)out,sizeof(out))==sizeof(out);
 }
 int16_t silence[240]={};audioBus.write((uint8_t*)silence,sizeof(silence));delay(25);digitalWrite(39,LOW);
 seqPlayingStep=-1;seqError=!ok;seqBusy=false;vTaskDelete(nullptr);
}
static String seqNote(uint8_t n){const char* names[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};return n?String(names[n%12])+String(int(n)/12-1):"OFF";}
static void seqDraw(){
 if(seqPopup==4){pianoDraw();return;}
 view=SEQUENCER;menuActive=false;seqShownStep=seqPlayingStep;
 epd.gameUpdates=seqBusy;
 beginDraw();do{
  header("","MUSIC / 4 TRACK STEP SEQUENCER","ARROWS: CELL  ENTER: ON/OFF  SPACE: PLAY/STOP  ESC: BACK");
  epd.drawRect(16,88,162,348,GxEPD_BLACK);epd.drawRect(188,88,594,348,GxEPD_BLACK);
  text(seqBusy?"PLAYING":"STOPPED",25,122,true);text(String(seq.bpm)+" BPM",25,165,true);
  text(seqNames[seqTrack],25,210,true);text("STEP "+String(seqColumn+1),25,250);
  text(seqTrack==3?seqNote(seq.cells[3][seqColumn]):seq.cells[seqTrack][seqColumn]?"ON":"OFF",25,289,true);
  text(seqDirty?"UNSAVED":"SAVED",25,335);
  for(int c=0;c<16;c++){
   text(String(c+1),202+c*36,120);
   if(seqBusy&&c==seqShownStep)epd.fillTriangle(204+c*36,127,222+c*36,127,213+c*36,137,GxEPD_BLACK);
  }
  for(int t=0;t<4;t++)for(int c=0;c<16;c++){
   int x=199+c*36,y=140+t*49;bool on=seq.cells[t][c];
   epd.drawRect(x,y,31,36,GxEPD_BLACK);
   if(on)epd.fillRect(x+6,y+7,19,22,GxEPD_BLACK);
   if(t==seqTrack&&c==seqColumn){epd.drawRect(x-3,y-3,37,42,GxEPD_BLACK);epd.drawRect(x-2,y-2,35,40,GxEPD_BLACK);}
  }
  text("T: TEMPO  V: VOL  N: NOTE  K: PIANO  S: SAVE",202,366);
  text("VOLUME "+String(seqVolume)+"%  NOTE "+seqNote(seqNotes[seqTrack]),202,396);
  text(clip(seqMessage,47),202,425);
   if(seqPopup){
   int lo=seqPopup==1?40:seqPopup==2?0:24,hi=seqPopup==1?240:seqPopup==2?100:96;
   epd.fillRect(210,160,550,180,GxEPD_WHITE);epd.drawRect(210,160,550,180,GxEPD_BLACK);
   epd.drawRect(213,163,544,174,GxEPD_BLACK);
   text(seqPopup==1?"TEMPO":seqPopup==2?"VOLUME":"NOTE / "+String(seqNames[seqTrack]),235,198,true);
   text(seqPopup==1?String(seqDraft)+" BPM":seqPopup==2?String(seqDraft)+" %":seqNote(seqDraft),235,235,true);
   epd.drawRect(235,255,495,18,GxEPD_BLACK);
   int pos=(seqDraft-lo)*483/(hi-lo);
   if(pos)epd.fillRect(240,260,pos,8,GxEPD_BLACK);
   epd.fillRect(237+pos,251,5,26,GxEPD_BLACK);
   text("ARROWS: CHANGE  ENTER: OK  ESC: CANCEL",235,310);
  }
 }while(epd.nextPage());epd.gameUpdates=false;
}
static void seqOpen(){
 seqStop();seqPopup=0;
 memset(seqLengths,1,sizeof(seqLengths));
 if(prefs.getBytesLength("seqLengths")==sizeof(seqLengths))prefs.getBytes("seqLengths",seqLengths,sizeof(seqLengths));
 for(int i=0;i<16;i++)seqLengths[i]=constrain(int(seqLengths[i]),1,16-i);seqVolume=constrain(int(prefs.getUChar("seqVolume",80)),0,100);
 if(prefs.getBytesLength("seqNotes")==sizeof(seqNotes))prefs.getBytes("seqNotes",seqNotes,sizeof(seqNotes));
 for(int t=0;t<4;t++)seqNotes[t]=constrain(int(seqNotes[t]),24,96);
 SeqPattern saved={};
 bool valid=prefs.getBytesLength("seqPattern")==sizeof(saved)&&prefs.getBytes("seqPattern",&saved,sizeof(saved))==sizeof(saved)&&saved.magic==0x53455131&&saved.bpm>=40&&saved.bpm<=240;
 for(int t=0;t<4&&valid;t++)for(int c=0;c<16;c++)if(saved.cells[t][c]&&(t==3?(saved.cells[t][c]<24||saved.cells[t][c]>72):saved.cells[t][c]!=1))valid=false;
 if(valid)seq=saved;
 else {memset(&seq,0,sizeof(seq));seq.magic=0x53455131;seq.bpm=120;seqDirty=true;}
 seqMessage=valid?"PATTERN LOADED":"EMPTY PATTERN / ENTER TO ADD STEPS";seqError=false;seqDraw();
}
static void seqTick(){
 seqRestoreVolume();
 if(view!=SEQUENCER||seqPopup||!seqBusy||millis()-seqVisualAt<350)return;
 if(seqPlayingStep!=seqShownStep){seqVisualAt=millis();seqDraw();}
}
static void seqKey(uint8_t k){
 if(seqPopup==4){pianoKey(k);return;}
 if(!seqPopup&&(k=='k'||k=='K')){
  seqStop();memcpy(pianoNotes,seq.cells[3],16);memcpy(pianoLengths,seqLengths,16);
  pianoCursor=0;pianoMessage="DRAFT / ENTER: EXPORT TO BASS";seqPopup=4;seqDraw();return;
 }
 if(seqPopup){
  if(k==27||k==8){seqPopup=0;seqDraw();return;}
  if(k==13||k==10){
   portENTER_CRITICAL(&seqMux);
   if(seqPopup==1)seq.bpm=seqDraft;
   else if(seqPopup==2)seqVolume=seqDraft;
   else {seqNotes[seqTrack]=seqDraft;
    if(seqTrack==3)for(int c=0;c<16;c++)if(seq.cells[3][c])seq.cells[3][c]=seqDraft;
   }
   portEXIT_CRITICAL(&seqMux);
   seqDirty=true;seqPopup=0;seqDraw();return;
  }
  int d=(k==0xB7||k==0xB5)?1:(k==0xB4||k==0xB6)?-1:0;
  if(d){int lo=seqPopup==1?40:seqPopup==2?0:24,hi=seqPopup==1?240:seqPopup==2?100:96;
   int next=constrain(seqDraft+d,lo,hi);if(next!=seqDraft){seqDraft=next;seqDraw();}}
  return;
 }
 if(k=='t'||k=='T'||k=='v'||k=='V'||k=='n'||k=='N'){
  seqPopup=(k=='t'||k=='T')?1:(k=='v'||k=='V')?2:3;
  seqDraft=seqPopup==1?seq.bpm:seqPopup==2?seqVolume:seqNotes[seqTrack];seqDraw();return;
 }
 if(k==27||k==8){seqStop();if(seqDirty&&!seqSave()){seqDraw();return;}showMenu(333,HOME);return;}
 if(k==' '){if(seqBusy)seqStop();else if(!codecReady||!audioBusReady||recBusy)seqMessage="AUDIO NOT READY / BUSY";else{
  if(es8311_voice_volume_set(audioCodec,100,nullptr)!=ESP_OK){seqMessage="CODEC VOLUME FAILED";seqDraw();return;}
  seqCodecBoost=true;seqHalt=false;seqError=false;seqBusy=true;
  if(xTaskCreate(seqWorker,"sequencer",4096,nullptr,2,nullptr)!=pdPASS){seqBusy=false;seqMessage="AUDIO TASK FAILED";seqRestoreVolume();}
 }seqDraw();return;}
 if(k==0xB5)seqTrack=(seqTrack+3)%4;
 else if(k==0xB6)seqTrack=(seqTrack+1)%4;
 else if(k==0xB4)seqColumn=(seqColumn+15)%16;
 else if(k==0xB7)seqColumn=(seqColumn+1)%16;
 else if(k=='s'||k=='S'){seqSave();}
 else if(k==13||k==10||k=='+'||k=='='||k=='-'||k=='['||k==']'){
  portENTER_CRITICAL(&seqMux);
  uint8_t &n=seq.cells[seqTrack][seqColumn];
  if(k==13||k==10){n=n?0:(seqTrack==3?seqNotes[3]:1);if(seqTrack==3)seqLengths[seqColumn]=1;}
  else if(k=='+'||k=='=')seq.bpm=min(240,int(seq.bpm)+1);
  else if(k=='-')seq.bpm=max(40,int(seq.bpm)-1);
  else if(seqTrack==3)n=constrain(int(n?n:36)+(k==']'?1:-1),24,96);
  portEXIT_CRITICAL(&seqMux);seqDirty=true;seqMessage="";
 }else return;
 seqDraw();
}

static int seqDisplay2State(){return view==SEQUENCER?(seqBusy?1:2):0;}

// Modal editor: typing notes advances by their length, arrows only move the cursor.
static void pianoDraw(){
 view=SEQUENCER;menuActive=false;
 beginDraw();do{
  header("","PIANO / STEP RECORDING","ESC: CANCEL   ENTER: EXPORT TO BASS");
  epd.fillRect(18,85,764,355,GxEPD_WHITE);epd.drawRect(18,85,764,355,GxEPD_BLACK);
  text("PIANO   OCT "+String(pianoOctave)+"   LENGTH "+String(pianoLength)+"/16",32,117,true);
  text("1:1/16  2:1/8  3:1/4  4:1/2  5:1/1   Z/X: OCTAVE",32,146);
  for(int i=0;i<16;i++){
   int x=32+i*46;
   epd.drawRect(x,161,43,58,GxEPD_BLACK);
   if(i==pianoCursor){epd.drawRect(x-2,159,47,62,GxEPD_BLACK);epd.fillTriangle(x+14,153,x+28,153,x+21,159,GxEPD_BLACK);}
   text(pianoNotes[i]?seqNote(pianoNotes[i]):"--",x+2,182);
   text(pianoNotes[i]?String(pianoLengths[i])+"/16":"",x+2,207);
  }
  const int whiteSemis[]={0,2,4,5,7,9,11,12};
  const char* whiteLabels[]={"A","S","D","F","G","H","J","K"};
  for(int i=0;i<8;i++){
   int x=48+i*88;epd.drawRect(x,237,88,132,GxEPD_BLACK);
   text(whiteLabels[i],x+34,346,true);
   text(seqNote((pianoOctave+1)*12+whiteSemis[i]),x+25,365);
  }
  const int blackSlots[]={0,1,3,4,5};const char* blackLabels[]={"W","E","T","Y","U"};
  for(int i=0;i<5;i++){
   int x=48+(blackSlots[i]+1)*88-24;
   epd.fillRect(x,237,48,66,GxEPD_BLACK);epd.fillRect(x+4,279,40,21,GxEPD_WHITE);
   text(blackLabels[i],x+16,296);
  }
  text("SPACE: REST   BACKSPACE: ERASE   ARROWS: STEP",32,398);
  text(clip(pianoMessage,62),32,427);
 }while(epd.nextPage());
}
static void pianoKey(uint8_t k){
 if(k==27){seqPopup=0;seqMessage="PIANO DRAFT CANCELLED";seqDraw();return;}
 if(k==13||k==10){
  memcpy(seq.cells[3],pianoNotes,16);memcpy(seqLengths,pianoLengths,16);
  seqDirty=true;seqTrack=3;seqColumn=pianoCursor;seqPopup=0;
  seqSave();seqDraw();return;
 }
 if(k==0xB4)pianoCursor=max(0,pianoCursor-1);
 else if(k==0xB7)pianoCursor=min(15,pianoCursor+1);
 else if(k>='1'&&k<='5')pianoLength=1<<(k-'1');
 else if(k=='z'||k=='Z')pianoOctave=max(1,pianoOctave-1);
 else if(k=='x'||k=='X')pianoOctave=min(4,pianoOctave+1);
 else if(k==8||k==127){pianoNotes[pianoCursor]=0;pianoLengths[pianoCursor]=1;}
 else {
  char lower=(k>='A'&&k<='Z')?k+32:k;
  const char* key=lower?strchr(pianoKeys,lower):nullptr;
  if(k!=' '&&!key)return;
  int length=min(pianoLength,16-pianoCursor);
  // Replacing any part of an earlier held note trims it at this position.
  for(int i=0;i<pianoCursor;i++)if(pianoNotes[i]&&i+pianoLengths[i]>pianoCursor)pianoLengths[i]=pianoCursor-i;
  for(int i=pianoCursor;i<pianoCursor+length;i++){pianoNotes[i]=0;pianoLengths[i]=1;}
  pianoNotes[pianoCursor]=k==' '?0:(pianoOctave+1)*12+(key-pianoKeys);
  pianoLengths[pianoCursor]=length;
  pianoMessage="STEP "+String(pianoCursor+1)+": "+(k==' '?String("REST"):seqNote(pianoNotes[pianoCursor]))+" / "+String(length)+" SIXTEENTHS";
  pianoCursor=min(15,pianoCursor+length);
 }
 pianoDraw();
}
