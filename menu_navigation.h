#include "os_core.h"
// One navigation vocabulary for every application. Legacy handlers are invoked
// only by explicit menu actions, never by letter shortcuts from the keyboard.
struct MenuItem { String label; int action; };
MenuItem menuItems[80];int menuCount=0,menuRow=0,menuContext=-1;
View menuView=HOME;bool menuActive=false;
void addMenu(String label,int action){if(menuCount<80)menuItems[menuCount++]={label,action};}
int rememberedMenuRow[400]={};
int appGroup=300;
int applicationParent(int c){
 if(c==CALENDAR||c==TIMER_APP)return 330;
 if(c==FILES)return 300;
 if(c==GAMES)return 300;
 return appGroup;
}
String menuTitle(int c){
 if(c==343)return "POCKET OS / SERVICES";
 if(c==340)return "QUICK MENU";
 if(c==341)return "NOTIFICATIONS";
 if(c==342)return "CONTINUE LAST ACTIVITY?";
 if(c==300)return "APPLICATIONS";
 if(c==330)return "PLANNER";
 if(c==331)return "DOCUMENTS";
 if(c==332)return "NETWORK";
 if(c==333)return "TOOLS";
 if(c==334)return "MUSIC";
 if(c==SYSTEM)return "SETTINGS";
 if(c==301||c==200)return "WI-FI / NETWORKS";
 if(c==FILES)return "APPLICATIONS / FILES";
 if(c==GAMES)return "APPLICATIONS / GAMES";
 if(c==CALENDAR)return "PLANNER / CALENDAR";
 if(c==TIMER_APP)return "PLANNER / TIMER";
 if(c==CAL_TASKS)return "CALENDAR / TASKS";
 if(c==PAPER_SETTINGS)return "SETTINGS / DISPLAY";
 if(c==201)return "SETTINGS / DATE & TIME";
 if(c==209)return "SETTINGS / SOUND";if(c==220)return "SOKOBAN / RESTART?";if(c==221)return "SOKOBAN / LEVELS";
 if(c==216)return "SETTINGS / SLEEP & POWER";
 if(c==217)return "SLEEP DELAY";
 if(c==218)return "POWER OFF DELAY";
 return "ACTIONS";
}
void buildMenu(int c){
 menuCount=0;
 if(c==343){
  readPower();
  addMenu("REFRESH STATUS",1902);
  addMenu("BATTERY: "+batteryValue,1902);
  addMenu("POWER: "+chargeValue,1902);
  bool mounted=cardOK&&SD_MMC.cardType()!=CARD_NONE;
  addMenu(String("TF: ")+(mounted?"MOUNTED":"NOT MOUNTED"),1902);
  addMenu(String("FILES / SAVE: ")+(mounted?"AVAILABLE":"UNAVAILABLE"),1902);
  addMenu("WI-FI: "+wifiState(),1902);
  addMenu("IP: "+String(WiFi.status()==WL_CONNECTED?WiFi.localIP().toString():"--"),1902);
  addMenu("INTERNET: NOT TESTED",1902);
  addMenu(String("SOUND: ")+(codecReady?(audioMuted?"OFF":"ON"):"--"),1902);
  addMenu(String("PLAY: ")+(codecReady&&audioBusReady?"AVAILABLE":"UNAVAILABLE"),1902);
  addMenu(String("RECORD: ")+(codecReady&&audioBusReady&&mounted?"AVAILABLE":"UNAVAILABLE"),1902);
  addMenu("SPEAKER: NOT TESTED",1902);
  addMenu(String("KEYBOARD: ")+(kbOK?"DETECTED":"NOT FOUND"),1902);
  addMenu(String("RTC: ")+(rtcOK?rtcTime():"NOT FOUND"),1902);
  addMenu("OLED: "+display2Status(),1902);
  addMenu(String("BLUETOOTH: ")+(esp_bt_controller_get_status()==ESP_BT_CONTROLLER_STATUS_ENABLED?"RADIO ON":"OFF"),1902);
  addMenu("BT KEYBOARD: NOT TESTED",1902);
  addMenu("FREE HEAP: "+String(ESP.getFreeHeap()/1024)+" KB",1902);
 }

 if(c==340){addMenu("VOLUME",1800);addMenu(String("SOUND: ")+(codecReady?(audioMuted?"OFF":"ON"):"--"),1801);addMenu(String("WI-FI: ")+wifiState(),319);addMenu("TIMER / POMODORO",211);addMenu("SLEEP NOW",1803);addMenu("NOTIFICATIONS",1804);addMenu("DEVICE STATUS",1902);}
 if(c==341){
 addMenu("CLEAR READ NOTIFICATIONS",1810);
 // Live calendar snapshot, not duplicate notifications on every visit.
 calendarLoad();int nearest[5]={-1,-1,-1,-1,-1};
 for(int i=0;i<64;i++){auto &t=calendarStore.tasks[i];if(!t.date||t.done||reminders[i].fired)continue;
 for(int p=0;p<5;p++){int old=nearest[p];if(old<0||t.date<calendarStore.tasks[old].date||(t.date==calendarStore.tasks[old].date&&reminders[i].minute<reminders[old].minute)){for(int j=4;j>p;j--)nearest[j]=nearest[j-1];nearest[p]=i;break;}}}
 for(int p=0;p<5;p++)if(nearest[p]>=0){int i=nearest[p];auto &t=calendarStore.tasks[i];addMenu("TASK "+String(t.date)+" "+(reminders[i].enabled?reminderTime(i):String("--:--"))+" "+String(t.title),210);}
 if(!noticeCount)addMenu("NO RECENT EVENTS",1804);
 for(int i=0;i<noticeCount;i++)addMenu(String(noticeRead[i]?"[READ] ":"[NEW] ")+noticeLog[i],1820+i);
 }
 if(c==342){if(prefs.getString("resumeKind","").length())addMenu("CONTINUE "+prefs.getString("resumeKind",""),1805);addMenu("NOT NOW",1806);}

 if(c==FILES){addMenu("NEW / EDIT NOTE",'n');addMenu("COMMAND LINE",'t');addMenu("REFRESH",'r');for(int i=0;i<count;i++)addMenu(String(files[i].dir?"[DIR] ":"")+files[i].name,1000+i);}
 if(c==300){for(const auto &app:PocketOS::apps)addMenu(app.label,app.action);}
 if(c==330){addMenu("CALENDAR / TASKS",210);addMenu("TIMER / POMODORO",211);}
 if(c==331){addMenu("BOOKS",323);addMenu("NEW NOTE",311);addMenu("VOICE RECORDER",295);}
 if(c==332){addMenu("WEB READER",312);addMenu("FILE EXCHANGE",314);}
 if(c==334){addMenu("GROOVE BOX",324);addMenu("SEQUENCER / PIANO",321);}
 if(c==333){addMenu("GROOVE BOX",324);addMenu("MUSIC SEQUENCER",321);addMenu("CALCULATOR",315);addMenu("COMMAND LINE",316);}
 if(c==SYSTEM){addMenu(String("WI-FI: ")+wifiState(),319);addMenu("SCAN / CONNECT WI-FI",318);addMenu("DISPLAY",'e');addMenu("SOUND / MICROPHONE",209);addMenu("DATE / TIME",201);addMenu("SLEEP / POWER",216);addMenu("DEVICE STATUS",317);addMenu("OS / SERVICES",1902);}
 if(c==301){addMenu("WI-FI NETWORKS",200);addMenu("SCAN WI-FI",318);}
 if(c==GAMES){const char* n[]={"SNAKE","TETRIS","SOKOBAN","2048","TAMAGOTCHI"};for(int i=0;i<5;i++)addMenu(n[i],1100+i);addMenu("JDM GARAGE EMPIRE",1105);}
 if(c==PAPER_SETTINGS){addMenu(String("TEXT: ")+(largeText?"LARGE":"NORMAL"),1200);addMenu("CLEAN EVERY "+String(epd.cleanEvery)+" UPDATES",1201);addMenu(String("PARTIAL UPDATE: ")+(epd.partial?"ON":"OFF"),1202);addMenu("CLEAN SCREEN NOW",'c');}
 if(c==TIMER_APP){timerLoad();addMenu(timerAlert?"CONFIRM / NEXT INTERVAL":timerRunning?"PAUSE":"START / CONTINUE",13);addMenu(String("MODE: ")+(timerPomo?"POMODORO":"TIMER"),'m');addMenu("EDIT DURATIONS",'e');addMenu("RESET",'r');}
 if(c==CALENDAR){addMenu("TASKS FOR "+calendarDateText(),13);addMenu("CHOOSE DATE",'d');addMenu("PREVIOUS DAY",1300);addMenu("NEXT DAY",1301);addMenu("PREVIOUS MONTH",'p');addMenu("NEXT MONTH",'n');}
 if(c==CAL_TASKS){addMenu("ADD TASK",'a');for(int i=0;i<dayTasks(calDay);i++){auto &t=calendarStore.tasks[taskIndex(i)];addMenu(String(t.done?"[X] ":"[ ] ")+(reminders[taskIndex(i)].enabled?reminderTime(taskIndex(i))+" ":String(""))+t.title,1400+i);}}
 if(c==200){addMenu("SCAN AGAIN",'r');for(int i=0;i<netCount;i++)addMenu(nets[i],1500+i);}
 if(c==201){addMenu("SET TIME",212);addMenu("SET CURRENT DATE",213);addMenu(String("FORMAT: ")+(time24?"24 HOUR":"12 HOUR"),214);addMenu(String("PERIOD: ")+(prefs.getBool("timePM",false)?"PM":"AM"),215);}
 if(c==216){addMenu("SLEEP: "+powerDelayLabel(sleepChoice),217);addMenu("POWER OFF: "+powerDelayLabel(offChoice),218);}
 if(c==217||c==218){uint8_t chosen=c==217?sleepChoice:offChoice;for(int i=0;i<8;i++)addMenu(String(i==chosen?"[X] ":"[ ] ")+powerDelayLabel(i),1600+i);}
 if(c==202){addMenu("OPEN",220);addMenu("EDIT AS NOTE",221);addMenu("DELETE FILE",222);}
 if(c==203){addMenu("CANCEL",230);addMenu("DELETE",231);}
 if(c==204){addMenu("CONTINUE EDITING",240);addMenu("SAVE TO TF CARD",241);addMenu("EXIT WITHOUT SAVING",242);}
 if(c==205){addMenu("TOGGLE DONE",250);addMenu("DELETE TASK",251);}
 if(c==206){addMenu("CANCEL",260);addMenu("DELETE TASK",261);}
 if(c==207||c==TAMAGOTCHI){addMenu("FEED",'f');addMenu("PLAY",'p');addMenu("SLEEP",'z');addMenu("RETURN TO GAMES",270);}
 if(c==209){addMenu("TEST SPEAKER",290);addMenu("RECORD TO TF (3 SEC)",291);addMenu("VOLUME + 10%",292);addMenu("VOLUME - 10%",293);addMenu("MUTE (0%)",294);}
 if(c==219){addMenu(tet.over?"GAME OVER":"CONTINUE",282);if(!tet.over)addMenu("HARD DROP",283);addMenu("NEW GAME",284);addMenu("SAVE / RETURN TO GAMES",285);}
 if(c==220){addMenu("KEEP PLAYING",286);addMenu("RESTART LEVEL",287);}
 if(c==221){for(int i=0;i<min(SOK_LEVELS,skCompleted+1);i++)addMenu("LEVEL "+String(i+1)+(i<skCompleted?" / SOLVED":""),1700+i);}
 if(c==208){if(view==SOKOBAN){if(skWon&&sk.level+1<SOK_LEVELS)addMenu("NEXT LEVEL",288);addMenu("CONTINUE",280);addMenu("UNDO MOVE",289);addMenu("RESTART LEVEL...",286);addMenu("SELECT LEVEL",296);addMenu("SAVE / RETURN TO GAMES",297);}else{addMenu("CONTINUE",280);addMenu("NEW GAME",281);addMenu("RETURN TO GAMES",270);}}
}
#include "app_icons.h"
String gridClock;
void drawAppGrid(){
 readPower();gridClock=rtcTime();shownClock=gridClock;
 todaySignature=todaySnapshot();
 beginDraw();do{
  epd.fillScreen(GxEPD_WHITE);
  epd.setFont(&FreeMonoBold9pt7b);epd.setTextColor(GxEPD_BLACK);
  epd.drawRect(18,18,380,444,GxEPD_BLACK);
  epd.setCursor(34,43);epd.print("TODAY");
  drawToday();
  epd.drawRect(414,18,368,444,GxEPD_BLACK);
  for(int i=0;i<12;i++){
   int x=419+(i%3)*120,y=23+(i/3)*110;
   if(i==menuRow){epd.drawRect(x,y,118,108,GxEPD_BLACK);epd.drawRect(x+2,y+2,114,104,GxEPD_BLACK);}
   drawAppIcon(i,x+31,y+5);
   // 56px artwork with clear space before the caption.
   epd.setFont(&FreeMonoBold9pt7b);
   int16_t bx,by;uint16_t bw,bh;
   epd.getTextBounds(menuItems[i].label,0,0,&bx,&by,&bw,&bh);
   epd.setCursor(x+(118-bw)/2-bx,y+83);epd.print(menuItems[i].label);
  }
 }while(epd.nextPage());
}
void refreshHomeGrid(){
 if(menuActive&&menuContext==300)refreshToday();
}
void quickShow();
void showMenu(int c,View v){
 if(c==340){quickShow();return;}
 if(menuContext!=c){if(menuContext>=0&&menuContext<400)rememberedMenuRow[menuContext]=menuRow;menuRow=(c>=0&&c<400)?rememberedMenuRow[c]:0;menuContext=c;}menuView=v;view=v;menuActive=true;buildMenu(c);menuRow=constrain(menuRow,0,max(0,menuCount-1));
 if(c==300){drawAppGrid();return;}
 beginDraw();do{
 header("",menuTitle(c),c==340?"UP/DOWN: SELECT  LEFT/RIGHT: VOLUME  ESC: BACK":"UP/DOWN: SELECT  ENTER: OPEN  ESC/BACKSPACE: BACK");
 epd.drawRect(18,82,380,348,GxEPD_BLACK);epd.drawRect(414,82,368,348,GxEPD_BLACK);
 epd.fillRect(18,82,380,34,GxEPD_BLACK);inv("INFORMATION",34,105);epd.fillRect(414,82,368,34,GxEPD_BLACK);inv("FUNCTIONS",430,105);
 text(rtcTime(),34,155,true);text("TF: "+String(cardOK?"MOUNTED":"NOT MOUNTED"),34,198);text("WIFI: "+wifiState(),34,237);
 String detail=c==343?String(""):c==209?"VOLUME: "+String(audioVolume)+"%":c==FILES?folder:c==CALENDAR||c==CAL_TASKS?calendarDateText():c==204?leaf(notePath):status;
 if(c==341){
  String full=menuCount?menuItems[menuRow].label:"NO EVENTS";
  for(int row=0;row<6&&full.length();row++){int take=min(29,(int)full.length());text(full.substring(0,take),34,278+row*25,row==0);full.remove(0,take);}
 }else text(clip(detail,29),34,283,true);
 if(c==SYSTEM||c==200||c==301){text("IP: "+String(wifiState()=="CONNECTED"?WiFi.localIP().toString():"--"),34,325);text("INTERNET: NOT TESTED",34,365);}
 if(c==216||c==217||c==218){
  text("SLEEP: "+powerDelayLabel(sleepChoice),34,325);
  text("OFF AFTER SLEEP: "+powerDelayLabel(offChoice),34,365);
  text("POWER OFF: BATTERY ONLY",34,405);
 }
 if(c==343){
  text("POCKET OS "+String(PocketOS::version),34,283,true);
  text("KERNEL: FREERTOS",34,325);
  text("BUILT-IN APPS: "+String(PocketOS::appCount),34,365);
  text("UPTIME: "+String(millis()/1000)+" S",34,405);
 }
 if(c==209){text(clip(audioResult,29),34,325);text("MIC: "+clip(micResult,24),34,365);text(micPath.length()?micPath.substring(micPath.lastIndexOf('/')+1):"WAV / 24 kHz / MONO",34,405);}
 if(c==TIMER_APP){int64_t sec=(timerLeft()+999)/1000;char b[24];snprintf(b,sizeof(b),"%02lu:%02lu",(unsigned long)(sec/60),(unsigned long)(sec%60));text(b,34,333,true);text(timerAlert?"FINISHED":timerRunning?"RUNNING":"PAUSED",34,380);timerShown=(timerLeft()+9999)/10000;}
 if(c==207||c==TAMAGOTCHI){text("HUNGER "+String(tamaHunger),34,325);text("HAPPY "+String(tamaHappy),34,360);text("ENERGY "+String(tamaEnergy),34,395);}
 int start=(menuRow/6)*6;for(int i=start;i<min(start+6,menuCount);i++){
  int y=125+(i-start)*49;bool on=i==menuRow;
  String label=menuItems[i].label;
  if(c==340&&menuItems[i].action==1800)label=codecReady?"VOLUME < "+String(audioVolume)+"% >":"VOLUME: --";
  menuOvalLabel(clip(label,25),440,y+27,on);
 }
 }while(epd.nextPage());
}
void menuScreen(View v){showMenu(v==HOME?300:int(v),v);}
#include "file_manager.h"
void route(uint8_t k){
 if(view==JDM_GAME){JDM::keyEvent(k);return;}
 if(view==BOOK_LIBRARY||view==BOOK_READ){bookKeyEvent(k);return;}
 if(view==GPS_APP){if(gpsSettingsOpen){gpsKey(k);return;}if(k==27||k==8){gpsStop();showMenu(300,HOME);}else gpsKey(k);return;}
 if(view==NOTES_LIST&&!menuActive){
  if(k==27||k==8){showMenu(appGroup,HOME);return;}
  if((k=='n'||k=='N')&&cardOK){folder="/";typed="";noteNameView();return;}
  if((k==10||k==13)&&noteCount){if(openNote(noteTiles[noteSelected].path))noteEditView();return;}
  int next=noteSelected;
  if(k==0xB4)next--;else if(k==0xB7)next++;else if(k==0xB5)next-=3;else if(k==0xB6)next+=3;else return;
  next=constrain(next,0,max(0,noteCount-1));if(next!=noteSelected){noteSelected=next;notesDraw();}return;
 }
 if(view==FILES&&!menuActive){notesOrigin=false;fileManagerKey(k);return;}
 if(videoKey(k))return;
 if(fileViewerKey(k))return;
 if(jpegKey(k))return;
 if(webKey(k))return;
 if(view==GROOVE_BOX){gbKey(k);return;}
 if(view==SEQUENCER){seqKey(k);return;}
 if(view==CALENDAR&&!menuActive){calendarKey(k);return;}
 if(view==TAMAGOTCHI){menuActive=false;petKey(k);return;}
 if(view==TETRIS&&!menuActive){if(k==10||k==13){tetPaused=true;tetSave();showMenu(219,TETRIS);}else if(k==27||k==8){tetSave();games();}else tetrisKey(k);return;}
 if(view==RECORDER){if((k==27||k==8)&&!recBusy)showMenu(331,HOME);else recorderKey(k);return;}
 bool enter=k==10||k==13,back=k==27||k==8;
 if(menuActive){int c=menuContext;View v=menuView;
  if(c==300&&(k==0xB4||k==0xB7||k==0xB5||k==0xB6)){
   int row=menuRow/3,col=menuRow%3;
   if(k==0xB4)col=(col+2)%3;
   if(k==0xB7)col=(col+1)%3;
   if(k==0xB5)row=(row+3)%4;
   if(k==0xB6)row=(row+1)%4;
   menuRow=row*3+col;showMenu(c,v);return;
  }
  if(k==0xB5||k==0xB6){if(menuCount<2)return;menuRow=(menuRow+menuCount+(k==0xB5?-1:1))%menuCount;showMenu(c,v);return;}
  if(c==340&&menuCount&&menuItems[menuRow].action==1800&&(k==0xB4||k==0xB7)){
   int target=constrain(audioVolume+(k==0xB7?10:-10),0,100);
   if(target==audioVolume)return;
   if(audioSetVolume(target)){bool ok=prefs.putUChar("volume",audioVolume)==1&&prefs.getUChar("volume",255)==audioVolume;status=ok?"VOLUME SAVED":"APPLIED / SAVE FAILED";}
   else status=audioResult;
   showMenu(c,v);return;
  }
  if(back){menuActive=false;if(c==204){noteEditView();return;}if(c==202||c==203){manager();return;}if(c==205||c==206){calendarTasks();return;}if(c==217||c==218){showMenu(216,SYSTEM);return;}if(c==343){system();return;}if(c==200){system();return;}if(c==301||c==201||c==209||c==216||c==PAPER_SETTINGS){system();return;}if(c==219){if(tet.over){tetSave();games();}else{tetPaused=false;tetAt=millis();drawTetris();}return;}if(c==220||c==221){showMenu(208,SOKOBAN);return;}if(c==208&&v==SOKOBAN){drawSokoban();return;}if(c==207||c==TAMAGOTCHI||c==208){games();return;}if(c==CAL_TASKS){calendarDraw();return;}if(c==FILES&&folder!="/"){loadFolder(parent(folder));manager();return;}if(c==341){showMenu(340,HOME);return;}if(c==330||c==331||c==332||c==333||c==334){showMenu(300,HOME);return;}if(c==FILES||c==GAMES||c==CALENDAR||c==TIMER_APP){showMenu(applicationParent(c),HOME);return;}home();return;}
  if(!enter||!menuCount)return;int a=menuItems[menuRow].action;menuActive=false;
  if(a==1902){showMenu(343,SYSTEM);return;}
  if(a==1900){system();return;}
  if(a==1901){showMenu(340,HOME);return;}
  if(a==1801){if(audioSetMuted(!audioMuted)){bool ok=prefs.putBool("soundMuted",audioMuted)==1&&prefs.getBool("soundMuted",!audioMuted)==audioMuted;status=ok?"SOUND SAVED":"APPLIED / SAVE FAILED";}else status=audioResult;showMenu(340,HOME);return;}
  if(a==1800){showMenu(340,HOME);return;}
  if(a==1803){if(gpsPowered||seqBusy||gbBusy||recBusy||reminderWake||reminderSoundPending){status="SLEEP BLOCKED: ACTIVE APP";showMenu(340,HOME);return;}uint8_t old=sleepChoice;sleepChoice=1;deviceActivity=millis()-60000UL;idleSleep();sleepChoice=old;home();return;}
  if(a==1810){noticeClearRead();showMenu(341,HOME);return;}
  if(a>=1820&&a<1832){noticeMark(a-1820);showMenu(341,HOME);return;}
  if(a==1804){showMenu(341,HOME);return;}
  if(a==1806){home();return;}
  if(a==1805){resumeLast();return;}
  if(a>=1000&&a<1060){selected=a-1000;showMenu(202,FILES);return;}
  if(a==1105){menuActive=false;JDM::open();return;}
  if(a>=1100&&a<1105){gameSelected=a-1100;startGame();if(view==SNAKE)drawSnake();else if(view==TETRIS)drawTetris();else if(view==SOKOBAN)drawSokoban();else if(view==G2048)draw2048();if(view==TAMAGOTCHI){menuActive=false;petDraw();}return;}
  if(a>=1200&&a<=1202){paperField=a-1200;legacyRoute(13);return;}
  if(a==1300||a==1301){calendarMove(a==1300?-1:1);calendarDraw();return;}
  if(a>=1400&&a<1464){calSelected=a-1400;showMenu(205,CAL_TASKS);return;}
  if(a>=1500&&a<1512){netSelected=a-1500;view=SYSTEM;legacyRoute(13);return;}
  if(a==216||a==217||a==218){showMenu(a,SYSTEM);return;}
  if((c==217||c==218)&&a>=1600&&a<1608){
   uint8_t choice=a-1600;
   const char *setting=c==217?"sleepDelay":"offDelay";
   bool saved=prefs.putUChar(setting,choice)==1 && prefs.getUChar(setting,255)==choice;
   if(saved){loadPowerDelays();}
   status=saved?"SETTING SAVED": "SAVE FAILED - NOT APPLIED";
   deviceActivity=millis();showMenu(216,SYSTEM);return;
  }
  if(a>=330&&a<=334){appGroup=a;showMenu(a,HOME);return;}
  if(a==301){showMenu(301,SYSTEM);return;}
  if(a==310){appGroup=300;if(cardOK)manager();else{status="TF CARD NOT MOUNTED";showMenu(300,HOME);}return;}
  if(a==311){if(cardOK){notesOpen();}else{status="TF CARD NOT MOUNTED";showMenu(appGroup,HOME);}return;}
  if(a==312){url="";webEdit();return;}
  if(a==313){appGroup=300;games();return;}
  if(a==314){exchangeStart();return;}
  if(a==315||a==316||a==317){infoName=a==315?"CALC":a==316?"TERMINAL":"STATUS";typed="";appResult="";infoView();return;}
  if(a==318){scanWifi();showMenu(200,SYSTEM);return;}
  if(a==319){
   if(WiFi.getMode()!=WIFI_OFF){exchangeStop();wifiWanted=false;prefs.putBool("wifiEnabled",false);WiFi.setAutoReconnect(false);WiFi.disconnect(false,false);WiFi.mode(WIFI_OFF);status="WIFI OFF";}
   else{status="CONNECTING TO SAVED WIFI...";showMenu(SYSTEM,SYSTEM);enableSavedWifi();}
   showMenu(c==340?340:SYSTEM,c==340?HOME:SYSTEM);return;
  }
  if(a==323){bookLibrary();return;}
  if(a==322){gpsOpen();return;}
  if(a==324){gbOpen();return;}
  if(a==321){seqOpen();return;}
  if(a==295){recorderOpen();return;}
  if(a==209){showMenu(209,AUDIO_TEST);return;}
  if(c==209){
   if(a==290){audioTone();}
   if(a==291){micResult="RECORDING: SPEAK NOW";showMenu(209,AUDIO_TEST);if(cardOK){if(audioRecordMic())audioResult="WAV SAVED / 3 SECONDS";}else micResult="TF CARD NOT MOUNTED";}
   if(a>=292&&a<=294){int target=a==294?0:audioVolume+(a==292?10:-10);if(audioSetVolume(target)){prefs.putUChar("volume",audioVolume);}}
   showMenu(209,AUDIO_TEST);return;
  }
  if(a==200||a==201){showMenu(a,SYSTEM);return;}
  if(a==210){openCalendar();return;}if(a==211){timerDraw();return;}if(a==212){typed="";timeSetView();return;}
  if(a==213){calSetCurrent=true;calInput="";calMessage="";calendarInput(true);return;}
  if(a==214||a==215){if(a==214){time24=!time24;prefs.putBool("time24",time24);clockReadAt=0;}else prefs.putBool("timePM",!prefs.getBool("timePM",false));showMenu(201,SYSTEM);return;}
  if(a==220){openFile();return;}if(a==221){if(cardOK&&openNote(files[selected].name))noteEditView();else manager();return;}
  if(a==222){showMenu(203,FILES);return;}if(a==230){manager();return;}
  if(a==231){status=cardOK&&count&&!files[selected].dir&&SD_MMC.remove(files[selected].path.c_str())?"FILE DELETED":"DELETE FAILED";String result=status;loadFolder(folder);status=result;manager();return;}
  if(a==240){noteEditView();return;}if(a==241){status=cardOK&&saveNote()?"NOTE SAVED TO TF CARD":"SAVE FAILED";showMenu(204,NOTE_EDIT);return;}if(a==242){if(notesOrigin)notesOpen();else{loadFolder(folder);manager();}return;}
  if(a==250){legacyRoute(13);return;}if(a==251){showMenu(206,CAL_TASKS);return;}if(a==260){calendarTasks();return;}if(a==261){int i=taskIndex(calSelected);if(i>=0){memset(&calendarStore.tasks[i],0,sizeof(CalendarTask));memset(&reminders[i],0,sizeof(reminders[i]));calendarSave();}calendarTasks();return;}
  if(a>=282&&a<=285){if(a==282){if(tet.over){showMenu(219,TETRIS);return;}tetPaused=false;tetAt=millis();drawTetris();}if(a==283)tetHardDrop();if(a==284){tetNew();drawTetris();}if(a==285){tetSave();games();}return;}
  if(v==SOKOBAN){
   if(a>=1700&&a<1700+SOK_LEVELS){skNew(a-1700);drawSokoban();return;}
   if(a==286){if(c==220)drawSokoban();else showMenu(220,SOKOBAN);return;}
   if(a==287){skNew(sk.level);drawSokoban();return;}
   if(a==288){if(skWon&&sk.level+1<SOK_LEVELS)skNew(sk.level+1);drawSokoban();return;}
   if(a==289){skUndoMove();return;}
   if(a==296){showMenu(221,SOKOBAN);return;}
   if(a==297){skSave();games();return;}
  }
  if(a==270){saveTama();games();return;}if(a==280){if(v==SOKOBAN)drawSokoban();else{if(won2048)continued2048=true;draw2048();}return;}if(a==281){if(v==SOKOBAN){startSokoban();drawSokoban();}else{start2048();draw2048();}return;}
  legacyRoute(a);if((c==207||c==TAMAGOTCHI)&&view==TAMAGOTCHI)showMenu(207,TAMAGOTCHI);return;
 }
 if(view==HOME){
  if(k==0xB5||k==0xB6){chosen=(chosen+3+(k==0xB5?-1:1))%3;home();return;}
  if(enter){if(chosen==0)showMenu(340,HOME);else if(chosen==1)showMenu(300,HOME);else if(chosen==2)system();return;}return;
 }
 if((k==27&&(view==WEB_EDIT||view==INFO))||(back&&(view==INPUT_VIEW||(view==INFO&&infoName!="TERMINAL"&&infoName!="CALC")))){showMenu(appGroup,HOME);return;}

 if(view==NOTE_EDIT&&k==0x13)return;
 if(view==NOTE_EDIT&&k==27){showMenu(204,NOTE_EDIT);return;}
 if(view==TIME_SET&&k==27){showMenu(201,SYSTEM);return;}
 if(view==TIME_SET&&(k=='f'||k=='F'||k=='d'||k=='D'||k=='a'||k=='A'||k=='p'||k=='P'))return;
 if((view==SOKOBAN||view==G2048)&&enter){showMenu(208,view);return;}
 // Editors accept ordinary text; non-editor screens never treat letters as commands.
 bool editing=view==NOTE_EDIT||view==NOTE_NAME||view==NET_PASS||view==TIME_SET||view==WEB_EDIT||view==CAL_ADD||view==CAL_DATE||view==TIMER_EDIT||(view==INFO&&(infoName=="TERMINAL"||infoName=="CALC"));
 if(!editing&&k>=32&&k<=126)return;
 legacyRoute(k);
}
