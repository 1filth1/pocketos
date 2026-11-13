// One background countdown; no audio hardware is touched.
static bool timerLoaded=false,timerPomo=false,timerRunning=false,timerAlert=false;
static bool timerEngaged=false;
static int timerPhase=0,timerCycles=0,timerField=0;
static uint32_t timerMinutes[4]={5,25,5,15};
static int64_t timerDeadline=0,timerRemaining=0;
static String timerInput,timerMessage;
static int64_t timerShown=-1;
int64_t timerNow(){return esp_timer_get_time()/1000;}
const char* timerPhaseName(){return !timerPomo?"COUNTDOWN":timerPhase==0?"WORK":timerPhase==1?"SHORT BREAK":"LONG BREAK";}
int timerDurationIndex(){return !timerPomo?0:timerPhase==0?1:timerPhase==1?2:3;}
int64_t timerLeft(){return timerRunning?max(int64_t(0),timerDeadline-timerNow()):timerRemaining;}
void timerLoad(){if(timerLoaded)return;for(int i=0;i<4;i++){String key="tmMin"+String(i);uint32_t m=prefs.getUInt(key.c_str(),timerMinutes[i]);if(m>=1&&m<=999)timerMinutes[i]=m;}timerLoaded=true;timerRemaining=int64_t(timerMinutes[0])*60000;}
void timerDraw(){menuScreen(TIMER_APP);return;timerLoad();view=TIMER_APP;int64_t left=timerLeft();timerShown=(left+9999)/10000;beginDraw();do{
 header("TIMER","TIMER / POMODORO","ENTER: START/PAUSE  M: MODE  E: SETUP  R: RESET  ESC: HOME");
 text(timerPomo?"POMODORO":"TIMER",30,125,true);text(String("PHASE: ")+timerPhaseName(),30,170,true);
 text(String("STATE: ")+(timerAlert?"FINISHED - CONFIRM":timerRunning?"RUNNING":left?"PAUSED / READY":"STOPPED"),30,210);
 int64_t secs=((left+9999)/10000)*10;char b[24];snprintf(b,sizeof(b),"%02lu:%02lu",(unsigned long)(secs/60),(unsigned long)(secs%60));
 epd.setTextSize(3);text(String(b),30,295,true);epd.setTextSize(1);
 text("WORK INTERVALS COMPLETED: "+String(timerCycles),360,255);
 text("WORK "+String(timerMinutes[1])+"m / REST "+String(timerMinutes[2])+"m",360,295);
 text("LONG REST "+String(timerMinutes[3])+"m EVERY 4 WORKS",360,330);
 text("10-second display steps. Countdown continues outside this screen.",25,380);
 text("SILENT. Power off cancels countdown. Settings are saved.",25,415);
 if(timerAlert){epd.fillRect(15,90,770,345,GxEPD_BLACK);inv(String(timerPhaseName())+" FINISHED",45,165);inv(timerPomo?"ENTER: START NEXT INTERVAL":"ENTER: ACKNOWLEDGE",45,235);inv("ESC/BACKSPACE: DISMISS TO HOME",45,295);inv("NO SOUND - VISUAL NOTIFICATION ONLY",45,360);}
 }while(epd.nextPage());}
void timerReset(){timerEngaged=false;timerRunning=false;timerAlert=false;timerPhase=0;timerCycles=0;timerRemaining=int64_t(timerMinutes[timerPomo?1:0])*60000;timerMessage="";}
void timerEdit(){view=TIMER_EDIT;beginDraw();do{
 header("TIMER","TIMER SETTINGS / MINUTES","UP/DOWN: FIELD  DIGITS: VALUE  ENTER: SAVE  ESC: CANCEL");
 const char* labels[]={"TIMER","WORK","SHORT REST","LONG REST"};for(int i=0;i<4;i++){int y=125+i*55;text(String(i==timerField?"> ":"  ")+labels[i]+": "+String(timerMinutes[i])+" min",35,y,i==timerField);}
 text("NEW VALUE (1-999 MIN): "+timerInput,35,365,true);text(timerMessage,35,415);
 }while(epd.nextPage());}
void timerTick(){
 if(timerRunning&&timerLeft()==0){timerRunning=false;timerRemaining=0;timerAlert=true;timerEngaged=false;appNotify(timerPomo?"POMO":"TIMER","DONE");if(timerPomo&&timerPhase==0)timerCycles++;timerDraw();return;}
 if(view==TIMER_APP&&!timerAlert&&(timerLeft()+9999)/10000!=timerShown)timerDraw();
}
bool timerKey(uint8_t k){
 if(view!=TIMER_APP&&view!=TIMER_EDIT)return false;
 if(view==TIMER_EDIT){
  if(k==27){timerDraw();return true;}
  if(k==8){if(timerInput.length())timerInput.remove(timerInput.length()-1);else {timerDraw();return true;}}
  else if(k==0xB5||k==0xB6){timerField=(timerField+(k==0xB5?3:1))%4;timerInput="";timerMessage="";}
  else if(k>= '0'&&k<='9'&&timerInput.length()<3)timerInput+=(char)k;
  else if(k==13||k==10){int m=timerInput.toInt();if(m<1||m>999)timerMessage="ENTER 1 TO 999 MINUTES";else {String key="tmMin"+String(timerField);if(prefs.putUInt(key.c_str(),m)==sizeof(uint32_t)){timerMinutes[timerField]=m;timerReset();timerMessage="SAVED TO TF CARD";timerInput="";}else timerMessage="SAVE FAILED - RETRY";}}
  timerEdit();return true;
 }
 if(k==27||k==8){timerAlert=false;home();return true;}
 if(timerAlert){if(k==13||k==10){timerAlert=false;if(timerPomo){timerPhase=timerPhase==0?(timerCycles%4==0?2:1):0;timerRemaining=int64_t(timerMinutes[timerDurationIndex()])*60000;timerDeadline=timerNow()+timerRemaining;timerRunning=true;timerEngaged=true;}timerDraw();}return true;}
 if(k==13||k==10){if(timerRunning){timerRemaining=timerLeft();timerRunning=false;}else{if(!timerRemaining)timerRemaining=int64_t(timerMinutes[timerDurationIndex()])*60000;timerDeadline=timerNow()+timerRemaining;timerRunning=true;timerEngaged=true;}}
 else if(k=='r'||k=='R')timerReset();
 else if(k=='m'||k=='M'){if(timerRunning)return true;timerPomo=!timerPomo;timerReset();}
 else if(k=='e'||k=='E'){if(timerRunning)return true;timerInput="";timerMessage="";timerEdit();return true;}
 else return true;
 timerDraw();return true;
}
