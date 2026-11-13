#pragma once
// Persistent pet with bounded, local e-paper sprite animation.
struct PetSave { uint32_t magic=0x50455432, minutes=0, xp=0, coins=10; int64_t stamp=0; uint8_t hunger=25,happy=75,energy=75,clean=90,health=100; bool sleeping=false; } pet;
bool petLoaded=false;unsigned long petAt=0;int petRow=0,petPage=0,petChoice=0,petRound=0,petWins=0,petTarget=0;int petPattern[5];String petMessage="WELCOME! CARE HELPS ME GROW.";
int64_t petClock(){uint8_t y,m,d,h,n,s;if(!rtcRead(y,m,d,h,n,s)||m<1||m>12||d<1||d>31)return 0;int yr=2000+y;int64_t days=0;for(int i=2000;i<yr;i++)days+=365+(i%4==0);static const int md[]={31,28,31,30,31,30,31,31,30,31,30,31};for(int i=1;i<m;i++)days+=md[i-1]+(i==2&&yr%4==0);return ((days+d-1)*24+h)*3600+n*60+s;}
// Verified card snapshots avoid repeated writes into a full NVS partition.
static bool petReadFile(const char *path,PetSave &value){
 File f=SD_MMC.open(path,"r");
 bool ok=f&&f.size()==sizeof(value)&&f.read((uint8_t*)&value,sizeof(value))==sizeof(value)
         &&value.magic==0x50455432;
 f.close();return ok;
}
static bool petWriteFile(){
 if(!cardOK)return false;
 File f=SD_MMC.open("/pet.tmp","w");if(!f)return false;
 bool ok=f.write((const uint8_t*)&pet,sizeof(pet))==sizeof(pet);f.flush();f.close();
 PetSave check;ok=ok&&petReadFile("/pet.tmp",check)&&!memcmp(&pet,&check,sizeof(pet));
 if(!ok)return false;
 if(SD_MMC.exists("/pet.bin")){
  PetSave old;
  if(petReadFile("/pet.bin",old)){
   if(SD_MMC.exists("/pet.bak")&&!SD_MMC.remove("/pet.bak"))return false;
   if(!SD_MMC.rename("/pet.bin","/pet.bak"))return false;
  }else return false; // Do not erase an unreadable existing save.
 }
 if(!SD_MMC.rename("/pet.tmp","/pet.bin"))return false;
 return petReadFile("/pet.bin",check)&&!memcmp(&pet,&check,sizeof(pet));
}
void saveTama(){
 pet.hunger=tamaHunger;pet.happy=tamaHappy;pet.energy=tamaEnergy;pet.stamp=petClock();
 bool ok=petWriteFile();const char *medium="TF CARD";
 if(!ok&&!cardOK){
  medium="TF CARD";PetSave check;
  ok=prefs.putBytes("petV2",&pet,sizeof(pet))==sizeof(pet)
    &&prefs.getBytes("petV2",&check,sizeof(check))==sizeof(check)&&!memcmp(&pet,&check,sizeof(pet));
 }
 if(ok)tamaSaved=millis();else petMessage="SAVE FAILED - KEEP POWER ON";
 Serial.printf("Pet save: %s (%s, %u bytes)\n",ok?"OK: readback verified":"FAILED",medium,(unsigned)sizeof(pet));
}
int petStage(){return pet.minutes<5?0:pet.minutes<60||pet.xp<20?1:pet.minutes<240||pet.xp<100?2:3;}
String petStageName(){const char* names[]={"EGG","BABY","JUNIOR","ADULT"};return names[petStage()];}
void petAdvance(unsigned steps){for(unsigned i=0;i<steps;i++){pet.minutes++;if(pet.sleeping){tamaEnergy=min(100,int(tamaEnergy)+2);}else if(pet.minutes%3==0&&tamaEnergy)tamaEnergy--;if(pet.minutes%3==0)tamaHunger=min(100,int(tamaHunger)+1);if(pet.minutes%5==0){if(pet.clean)pet.clean--;if(tamaHappy)tamaHappy--;if(tamaHunger>85||pet.clean<20){if(pet.health>1)pet.health--;}else if(tamaHunger<60&&pet.clean>60)pet.health=min(100,int(pet.health)+1);}if(pet.minutes%10==0&&pet.health>60&&tamaHunger<70)pet.xp++;}}
void petLoad(){if(petLoaded)return;petLoaded=true;PetSave read;if(prefs.getBytesLength("petV2")==sizeof(read)&&prefs.getBytes("petV2",&read,sizeof(read))==sizeof(read)&&read.magic==pet.magic)pet=read;if(cardOK&&(petReadFile("/pet.bin",read)||petReadFile("/pet.bak",read)))pet=read;tamaHunger=min(100,int(pet.hunger));tamaHappy=min(100,int(pet.happy));tamaEnergy=min(100,int(pet.energy));int64_t now=petClock();if(now>pet.stamp&&pet.stamp>0)petAdvance(min(int64_t(480),(now-pet.stamp)/60));petAt=millis();saveTama();}
// Hand-drawn LCD-style sprites: egg, Babytchi-like baby, round child,
// and a Mametchi-inspired adult. Not extracted original game assets.
// Each pose owns its eyes/feet: no solid black patches over the face.
static const char *const petArt[4][16]={
 {"................","......####......",".....#....#.....","....#......#....","...#..##....#...","...#..##....#...","..#.......#..#..","..#......##..#..","..#..........#..","..#..##......#..","..#..##..##..#..","...#.....##.#...","...#........#...","....#......#....",".....######.....","................"},
 {"................","................","................","................","......####......","....########....","...##########...","...##.####.##...","...##.####.##...","...##########...","....##....##....",".....######.....","................","................","................","................"},
 {"................",".....######.....","...##......##...","..#..........#..","..#..........#..",".#...#....#...#.",".#...#....#...#.",".#............#.",".#.....##.....#.",".#............#.","..#..........#..","..#..........#..","...##......##...",".....######.....","....##....##....","................"},
 {"...###....###...","..#####..#####..","..#####..#####..","..#...####...#..","..#..........#..","..#..........#..","..#..#....#..#..","..#..#....#..#..","..#..........#..","...#..####..#...","....#......#....","...#........#...","..##........##..","....#......#....","....###..###....","................"}
};
unsigned long petAnimAt=0;
uint8_t petFrame=0;int petWalkX=0,petWalkDir=1;
void petPixel(int x,int y,int col,int row,bool black=true){
 epd.fillRect(x+col*6,y+row*6,6,6,black?GxEPD_BLACK:GxEPD_WHITE);
}
void petSprite(){
 const int stage=petStage();
 const bool asleep=pet.sleeping,blink=petFrame%9==7;
 int x=151+petWalkX,y=151;
 if(stage==0)x=151+((petFrame%4==1)?6:(petFrame%4==3)?-6:0);
 else if(!asleep&&(petFrame&1))y-=6;
 for(int row=0;row<16;row++)for(int col=0;col<16;col++){
  int source=petWalkDir<0?15-col:col;
  bool ink=petArt[stage][row][source]=='#';
  if(stage>0){
   const int eye=stage==1?7:stage==2?5:6;
   const int left=5,right=10;
   if((source==left||source==right)&&(row==eye||row==eye+1)){
    // Baby has white eyes in a black body; older pets have black eyes.
    bool eyeInk=!(asleep||blink)||row==eye+1;
    ink=stage==1?!eyeInk:eyeInk;
   }
   if(tamaHappy<=40&&!asleep){
    int mouth=stage==1?10:stage==2?8:9;
    if(row>=mouth&&row<=mouth+1&&source>=6&&source<=9){
     bool frown=row==mouth?(source==7||source==8):(source==6||source==9);
     ink=stage==1?!frown:frown;
    }
   }
   // Keep the baby's round silhouette; only older pets have walking feet.
   if(stage>=2&&row==14){
    bool step=!asleep&&(petFrame&1);
    ink=step?((source>=3&&source<=5)||(source>=10&&source<=12)):
              ((source>=4&&source<=6)||(source>=9&&source<=11));
   }
  }
  if(ink)petPixel(x,y,col,row);
 }
 if(stage>0&&asleep)text(petFrame&1?"z Z":"Z z",x+98,y+25,true);
}
void petAnimate(){
 if(view!=TAMAGOTCHI||petPage!=0)return;
 unsigned long now=millis();
 if(now-petAnimAt<(pet.sleeping?4000UL:1400UL))return;
 petAnimAt=now;petFrame=(petFrame+1)%18;
 if(!pet.sleeping&&petStage()>0){
  if(petFrame%6!=0)petWalkX+=petWalkDir*12;
  if(petWalkX>=66){petWalkX=66;petWalkDir=-1;}
  if(petWalkX<=-72){petWalkX=-72;petWalkDir=1;}
 }
 // Clear both old and new positions; the retained framebuffer sends only changes.
 epd.fillRect(35,140,350,125,GxEPD_WHITE);
 petSprite();epd.nextPage();
}
void petDraw(){view=TAMAGOTCHI;beginDraw();do{header("TAMAGOTCHI",petStageName()+" / AGE "+String(pet.minutes/60)+"h "+String(pet.minutes%60)+"m","UP/DOWN: SELECT   ENTER: OK   ESC/BACKSPACE: BACK");epd.drawLine(405,90,405,430,GxEPD_BLACK);
text(petStageName()+String(pet.sleeping?" / SLEEPING":" / AWAKE"),30,112,true);
petSprite();
text("HUNGER "+String(tamaHunger)+"  HAPPY "+String(tamaHappy),25,290);text("ENERGY "+String(tamaEnergy)+"  CLEAN "+String(pet.clean),25,322);text("HEALTH "+String(pet.health)+"  XP "+String(pet.xp),25,354);text("COINS "+String(pet.coins),25,386);text(clip(petMessage,32),25,420);
if(petPage==0){String labels[]={"FEED (2 COINS)",pet.sleeping?"WAKE UP":"SLEEP","WASH","MEDICINE (5 COINS)","MINI-GAMES","GROWTH / HELP"};for(int i=0;i<6;i++){int y=105+i*52;epd.fillRect(420,y,365,43,i==petRow?GxEPD_BLACK:GxEPD_WHITE);if(i==petRow)inv("> "+labels[i],430,y+28);else text(labels[i],430,y+28,true);}}
else if(petPage==1){text("MINI-GAMES",430,125,true);text(String(petChoice==0?"> ":"  ")+"MEMORY",430,190,true);text(String(petChoice==1?"> ":"  ")+"FIND THE TREAT",430,250,true);text("Earn coins, XP and happiness",420,335);text("No moving / timed targets",420,375);}
else if(petPage==2){text("MEMORIZE THE ARROWS",425,130,true);const char* a[]={"UP","RIGHT","DOWN","LEFT"};for(int i=0;i<3+petRound;i++)text(String(i+1)+". "+a[petPattern[i]],450,180+i*42);text("ENTER: HIDE AND ANSWER",420,415);}
else if(petPage==3){text("REPEAT WITH ARROW KEYS",420,140,true);text("STEP "+String(petChoice+1)+" / "+String(3+petRound),435,220,true);text("ESC: CANCEL ROUND",435,340);}
else if(petPage==4){text("FIND THE TREAT",430,130,true);text("ROUND "+String(petRound+1)+" / 5",430,180);for(int i=0;i<3;i++){int x=440+i*110;epd.drawRect(x,225,80,70,GxEPD_BLACK);text(i==petChoice?"> ?":"?",x+15,266,true);}text("LEFT/RIGHT: PICK A BOX",420,350);text("ENTER: OPEN",420,395);}
else {text("GROW BY CARING",430,125,true);text("Egg: 5 minutes to hatch",420,180);text("Junior: 1h + 20 XP",420,220);text("Adult: 4h + 100 XP",420,260);text("Games earn coins and XP.",420,310);text("Sleep restores energy.",420,350);text("Away care: up to 8h (RTC)",420,390);text("Progress saved in device",420,425);}
}while(epd.nextPage());}
void startTama(){petLoad();petPage=0;petRow=0;petAnimAt=millis();petDraw();}
void tamaStats(bool partial){petDraw();}
void petReward(int coins,int xp){pet.coins+=coins;pet.xp+=xp;tamaHappy=min(100,int(tamaHappy)+8);saveTama();}
void petMemory(){for(int i=0;i<5;i++)petPattern[i]=random(4);petChoice=0;petPage=2;}
void petKey(uint8_t k){if(k==27||k==8){saveTama();if(petPage){petPage=0;petDraw();}else games();return;}
bool enter=k==10||k==13;
if(petPage==0){if(k==0xB5)petRow=(petRow+5)%6;else if(k==0xB6)petRow=(petRow+1)%6;else if(enter){if(petRow==1){pet.sleeping=!pet.sleeping;petMessage=pet.sleeping?"RESTING...":"GOOD MORNING!";}else if(petRow==5)petPage=5;else if(pet.sleeping)petMessage="WAKE ME UP FIRST";else if(petRow==0){if(pet.coins<2)petMessage="PLAY TO EARN COINS";else if(tamaHunger<10)petMessage="I AM FULL";else{pet.coins-=2;tamaHunger=max(0,int(tamaHunger)-25);pet.xp+=2;petMessage="YUMMY!";}}else if(petRow==2){pet.clean=100;pet.xp++;petMessage="CLEAN AND HAPPY";}else if(petRow==3){if(pet.coins<5)petMessage="NEED 5 COINS";else if(pet.health>=100)petMessage="ALREADY HEALTHY";else{pet.coins-=5;pet.health=min(100,int(pet.health)+30);petMessage="FEELING BETTER";}}else{petPage=1;petChoice=0;}saveTama();}else return;}
else if(petPage==1){if(k==0xB5||k==0xB6)petChoice=1-petChoice;else if(enter){if(tamaEnergy<5){petMessage="LOW ENERGY: SLEEP FIRST";petPage=0;}else{tamaEnergy-=5;petRound=petWins=0;if(petChoice==0)petMemory();else{petPage=4;petChoice=0;petTarget=random(3);}saveTama();}}else return;}
else if(petPage==2){if(!enter)return;petPage=3;petChoice=0;}
else if(petPage==3){int dir=k==0xB5?0:k==0xB7?1:k==0xB6?2:k==0xB4?3:-1;if(dir<0)return;if(dir!=petPattern[petChoice]){petMessage="TRY AGAIN! +1 COIN";petReward(1,1);petPage=1;petChoice=0;}else if(++petChoice==3+petRound){petReward(4,5);if(++petRound==3){petMessage="MEMORY MASTER! +12 COINS";petPage=1;petChoice=0;}else petMemory();}}
else if(petPage==4){if(k==0xB4)petChoice=(petChoice+2)%3;else if(k==0xB7)petChoice=(petChoice+1)%3;else if(enter){bool won=petChoice==petTarget;if(won)petWins++;petMessage=String(won?"FOUND!":"EMPTY!")+" TREAT WAS IN BOX "+String(petTarget+1);if(++petRound==5){petReward(2+petWins*3,2+petWins*2);petMessage="FOUND "+String(petWins)+"/5. +"+String(2+petWins*3)+" COINS";petPage=1;petChoice=0;}else petTarget=random(3);}else return;}
else return;petDraw();}
void tamaTick(){petLoad();unsigned long elapsed=millis()-petAt;if(elapsed<60000){petAnimate();return;}unsigned steps=elapsed/60000;petAt+=steps*60000;int stage=petStage();petAdvance(min(steps,480U));if(stage!=petStage())petMessage="I GREW! NOW "+petStageName();saveTama();if(view==TAMAGOTCHI)petDraw();}
