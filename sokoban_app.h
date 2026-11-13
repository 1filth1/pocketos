#pragma once
#include "sokoban_levels.h"
static constexpr int SK_W=24,SK_H=20,SK_N=SK_W*SK_H,SK_UNDO=256;
// Terrain never changes when a crate or player leaves a target.
uint8_t skTerrain[SK_N]; // 0 wall/outside, 1 floor, 2 target
struct SokUndo { uint16_t player; int8_t dx,dy; bool push; };
struct SokState {
 uint32_t magic;uint16_t level,player;uint32_t moves,pushes;
 uint8_t boxes[SK_N];uint16_t undoCount;SokUndo undo[SK_UNDO];
} sk;
int skWidth=0,skHeight=0,skCompleted=0;bool skWon=false;String skMessage;
void drawSokoban();
bool skFloor(int p){return p>=0&&p<SK_N&&skTerrain[p]!=0;}
bool skSolved(){for(int p=0;p<SK_N;p++)if(skTerrain[p]==2&&!sk.boxes[p])return false;return true;}
void skSave(){if(prefs.putBytes("sokState3",&sk,sizeof(sk))!=sizeof(sk))skMessage="SAVE FAILED";}
void skTerrainLoad(int level){
 memset(skTerrain,0,sizeof(skTerrain));skWidth=skHeight=0;int x=0,y=0;
 for(const char* p=sokLevels[level];*p;p++){
  if(*p=='\n'){skWidth=max(skWidth,x);x=0;y++;continue;}
  if(x<SK_W&&y<SK_H&&*p!='#')skTerrain[y*SK_W+x]=(*p=='.'||*p=='*'||*p=='+')?2:1;
  x++;
 }skHeight=y;
 // Flood from the player's start so exterior spaces are never walkable.
 bool seen[SK_N]={};uint16_t queue[SK_N];int n=0,read=0;x=y=0;
 for(const char* p=sokLevels[level];*p;p++){if(*p=='\n'){x=0;y++;continue;}if(*p=='@'||*p=='+'){int pos=y*SK_W+x;queue[n++]=pos;seen[pos]=true;}x++;}
 while(read<n){int pos=queue[read++];for(int d: {-1,1,-SK_W,SK_W}){int q=pos+d;if(skFloor(q)&&!seen[q]){seen[q]=true;queue[n++]=q;}}}
 for(int i=0;i<SK_N;i++)if(!seen[i]&&skTerrain[i])skTerrain[i]=0;
}
void skNew(int level){
 memset(&sk,0,sizeof(sk));sk.magic=0x534F4B33;sk.level=constrain(level,0,SOK_LEVELS-1);skTerrainLoad(sk.level);
 int x=0,y=0;for(const char* p=sokLevels[sk.level];*p;p++){if(*p=='\n'){x=0;y++;continue;}int pos=y*SK_W+x;if(*p=='$'||*p=='*')sk.boxes[pos]=1;if(*p=='@'||*p=='+')sk.player=pos;x++;}
 skWon=false;skMessage="";skSave();
}
void startSokoban(){
 skCompleted=constrain((int)prefs.getUChar("sokDone3",0),0,SOK_LEVELS);
 bool valid=prefs.getBytesLength("sokState3")==sizeof(sk)&&prefs.getBytes("sokState3",&sk,sizeof(sk))==sizeof(sk)&&sk.magic==0x534F4B33&&sk.level<SOK_LEVELS&&sk.player<SK_N&&sk.undoCount<=SK_UNDO;
 if(valid){skTerrainLoad(sk.level);int boxes=0,goals=0;valid=skFloor(sk.player)&&!sk.boxes[sk.player];for(int i=0;i<SK_N;i++){if(sk.boxes[i]>1||(sk.boxes[i]&&!skFloor(i)))valid=false;boxes+=sk.boxes[i];goals+=skTerrain[i]==2;}valid=valid&&boxes==goals;}
 if(!valid)skNew(0);skWon=skSolved();skMessage="";view=SOKOBAN;
}
void drawSokoban(){
 view=SOKOBAN;int on=0,total=0;for(int i=0;i<SK_N;i++)if(skTerrain[i]==2){total++;if(sk.boxes[i])on++;}
 int cell=min(34,min(530/skWidth,326/skHeight));int ox=250+(530-skWidth*cell)/2,oy=96+(326-skHeight*cell)/2;
 beginDraw();do{
 header("SOKOBAN","CLASSIC / ORIGINAL COLLECTION","ARROWS: MOVE   ENTER: ACTIONS   ESC/BACKSPACE: GAMES");
 text("LEVEL "+String(sk.level+1)+" / "+String(SOK_LEVELS),22,110,true);
 text("MOVES: "+String(sk.moves),22,151);text("PUSHES: "+String(sk.pushes),22,187);
 text("TARGETS: "+String(on)+"/"+String(total),22,223);text("SOLVED: "+String(skCompleted),22,259);
 text(skWon?"LEVEL COMPLETE!":"PUSH, NEVER PULL",22,310,true);text(skWon?"ENTER: NEXT LEVEL":"ENTER: UNDO / RESET",22,347);text(skMessage,22,410);
 for(int y=0;y<skHeight;y++)for(int x=0;x<skWidth;x++){
  int p=y*SK_W+x,px=ox+x*cell,py=oy+y*cell;
  if(!skTerrain[p]){epd.fillRect(px,py,cell,cell,GxEPD_BLACK);epd.drawRect(px+1,py+1,cell-2,cell-2,GxEPD_WHITE);continue;}
  if(skTerrain[p]==2){epd.drawCircle(px+cell/2,py+cell/2,max(2,cell/6),GxEPD_BLACK);}
  if(sk.boxes[p]){epd.fillRect(px+3,py+3,cell-6,cell-6,GxEPD_BLACK);epd.drawRect(px+5,py+5,cell-10,cell-10,GxEPD_WHITE);if(skTerrain[p]==2)epd.fillCircle(px+cell/2,py+cell/2,max(2,cell/6),GxEPD_WHITE);else {epd.drawLine(px+5,py+5,px+cell-6,py+cell-6,GxEPD_WHITE);epd.drawLine(px+cell-6,py+5,px+5,py+cell-6,GxEPD_WHITE);}}
  if(sk.player==p){epd.fillCircle(px+cell/2,py+cell/2,max(3,cell/3),GxEPD_BLACK);epd.fillCircle(px+cell/2-2,py+cell/2-2,1,GxEPD_WHITE);if(skTerrain[p]==2)epd.drawRect(px+1,py+1,cell-2,cell-2,GxEPD_BLACK);}
 }
 }while(epd.nextPage());
}
void moveSok(int dx,int dy){
 if(skWon)return;int d=dy*SK_W+dx,n=sk.player+d,b=n+d;if(!skFloor(n))return;bool push=sk.boxes[n];if(push&&(!skFloor(b)||sk.boxes[b]))return;
 if(sk.undoCount==SK_UNDO){memmove(sk.undo,sk.undo+1,(SK_UNDO-1)*sizeof(SokUndo));sk.undoCount--;}
 sk.undo[sk.undoCount++]={sk.player,(int8_t)dx,(int8_t)dy,push};
 if(push){sk.boxes[n]=0;sk.boxes[b]=1;sk.pushes++;}sk.player=n;sk.moves++;skWon=skSolved();skMessage="";
 if(skWon&&sk.level==skCompleted){skCompleted++;prefs.putUChar("sokDone3",skCompleted);}
 skSave();drawSokoban();
}
void skUndoMove(){
 if(!sk.undoCount){skMessage="NO MOVES TO UNDO";drawSokoban();return;}
 SokUndo u=sk.undo[--sk.undoCount];int d=u.dy*SK_W+u.dx,n=u.player+d,b=n+d;
 // Validate even persisted history before touching the board.
 if(!skFloor(u.player)||abs(u.dx)+abs(u.dy)!=1||n!=sk.player||(u.push&&!skFloor(b))){sk.undoCount=0;skMessage="UNDO UNAVAILABLE";drawSokoban();return;}
 if(u.push){sk.boxes[b]=0;sk.boxes[n]=1;if(sk.pushes)sk.pushes--;}
 sk.player=u.player;if(sk.moves)sk.moves--;skWon=false;skMessage="";skSave();drawSokoban();
}
