#pragma once
// Seven tetrominoes; occupied cells rotate inside their local square.
const uint16_t tetShapes[7]={0x00F0,0x0066,0x0072,0x0071,0x0074,0x0036,0x0063};
struct TetrisState {
 uint32_t magic; uint8_t cells[20][10];
 int32_t x,y,piece,rotation,next,score,lines;
 bool over;
} tet;
uint8_t tetBag[7]; int tetBagPos=7; bool tetPaused=false; unsigned long tetAt=0;
bool tetCell(int piece,int rotation,int x,int y){
 int size=piece==0?4:piece==1?2:3;
 if(x>=size||y>=size)return false;
 if(piece==1)return true;
 for(int r=0;r<rotation;r++){int old=x;x=y;y=size-1-old;}
 return (tetShapes[piece]>>(y*4+x))&1;
}
int tetPick(){
 if(tetBagPos==7){for(int i=0;i<7;i++)tetBag[i]=i;for(int i=6;i>0;i--){int j=random(i+1);uint8_t t=tetBag[i];tetBag[i]=tetBag[j];tetBag[j]=t;}tetBagPos=0;}
 return tetBag[tetBagPos++];
}
bool tetFits(int x,int y,int rotation){
 for(int j=0;j<4;j++)for(int i=0;i<4;i++)if(tetCell(tet.piece,rotation,i,j)){
  int xx=x+i,yy=y+j;if(xx<0||xx>=10||yy>=20)return false;if(yy>=0&&tet.cells[yy][xx])return false;
 }return true;
}
void tetSave(){tet.score=tetScore;prefs.putBytes("tetState",&tet,sizeof(tet));saveScore("tetris",tetScore);}
void tetSpawn(){tet.piece=tet.next;tet.next=tetPick();tet.x=3;tet.y=0;tet.rotation=0;if(!tetFits(tet.x,tet.y,0))tet.over=true;}
void tetNew(){memset(&tet,0,sizeof(tet));tet.magic=0x54455432;tetBagPos=7;tet.next=tetPick();tetScore=0;tetSpawn();tetPaused=false;tetAt=millis();}
void startTetris(){
 view=TETRIS;tetPaused=false;
 bool valid=prefs.getBytesLength("tetState")==sizeof(tet)&&prefs.getBytes("tetState",&tet,sizeof(tet))==sizeof(tet)&&tet.magic==0x54455432;
 if(valid){valid=tet.piece>=0&&tet.piece<7&&tet.next>=0&&tet.next<7&&tet.rotation>=0&&tet.rotation<4&&tet.x>=-3&&tet.x<=9&&tet.y>=0&&tet.y<20&&tet.score>=0&&tet.lines>=0;}
 if(!valid||tet.over)tetNew();else {tetScore=tet.score;tetPaused=true;}
 tetAt=millis();
}
void tetLock(){
 for(int y=0;y<4;y++)for(int x=0;x<4;x++)if(tetCell(tet.piece,tet.rotation,x,y)){
  int yy=tet.y+y,xx=tet.x+x;if(yy<0){tet.over=true;continue;}if(yy<20&&xx>=0&&xx<10)tet.cells[yy][xx]=tet.piece+1;
 }
 int cleared=0;
 for(int y=19;y>=0;y--){bool full=true;for(int x=0;x<10;x++)if(!tet.cells[y][x])full=false;
  if(full){for(int j=y;j>0;j--)memcpy(tet.cells[j],tet.cells[j-1],10);memset(tet.cells[0],0,10);cleared++;y++;}
 }
 const int points[5]={0,40,100,300,1200};tetScore+=points[cleared]*(tet.lines/10+1);tet.lines+=cleared;
 if(!tet.over)tetSpawn();tetSave();
}
void drawTetris(){
 // Use changed-region updates without periodic full-screen cleaning in play.
 const unsigned long renderStarted=millis();
 const bool previousGameUpdates=epd.gameUpdates;epd.gameUpdates=true;
 beginDraw();do{
  header("TETRIS","CLASSIC 10 x 20","LEFT/RIGHT: MOVE  UP: ROTATE  DOWN: FALL  ENTER: MENU");
  const int bx=270,by=83,s=17;
  epd.drawRect(bx-2,by-2,174,344,GxEPD_BLACK);
  for(int y=0;y<20;y++)for(int x=0;x<10;x++){
   bool active=false;for(int j=0;j<4;j++)for(int i=0;i<4;i++)if(tetCell(tet.piece,tet.rotation,i,j)&&tet.x+i==x&&tet.y+j==y)active=true;
   if(tet.cells[y][x]||active){epd.fillRect(bx+x*s+1,by+y*s+1,s-2,s-2,GxEPD_BLACK);epd.drawRect(bx+x*s+4,by+y*s+4,s-8,s-8,GxEPD_WHITE);}
  }
  text("SCORE",30,125,true);text(String(tetScore),30,160,true);text("LINES "+String(tet.lines),30,215);text("LEVEL "+String(tet.lines/10+1),30,250);text("BEST",30,310);text(String(max(tetScore,int(prefs.getInt("tetris",0)))),30,345,true);
  text("NEXT",495,120,true);for(int y=0;y<4;y++)for(int x=0;x<4;x++)if(tetCell(tet.next,0,x,y))epd.fillRect(495+x*22,140+y*22,20,20,GxEPD_BLACK);
  text(tet.over?"GAME OVER":tetPaused?"PAUSED":"PLAYING",495,295,true);text("ENTER: ACTIONS",495,345);text("ESC: SAVE / EXIT",495,385);
 }while(epd.nextPage());
 // Do not count blocking panel time as gameplay time, but preserve the
 // remaining fall interval across sideways movement and rotation.
 tetAt+=millis()-renderStarted;
 epd.gameUpdates=previousGameUpdates;
}
void tetrisTick(){
 if(tetPaused||tet.over)return;
 // E-paper needs a readable interval between positions, even at high levels.
 unsigned long interval=max(600,1600-int(tet.lines/10)*100);if(millis()-tetAt<interval)return;
 tetAt=millis();
 if(tetFits(tet.x,tet.y+1,tet.rotation))tet.y++;else tetLock();drawTetris();
}
void tetrisKey(uint8_t k){
 if(tetPaused||tet.over)return;bool changed=false;
 if(k==0xB4||k==0xB7){int dx=k==0xB4?-1:1;if(tetFits(tet.x+dx,tet.y,tet.rotation)){tet.x+=dx;changed=true;}}
 if(k==0xB5){int r=(tet.rotation+1)%4;const int kicks[]={0,-1,1,-2,2};for(int dx:kicks)if(tetFits(tet.x+dx,tet.y,r)){tet.x+=dx;tet.rotation=r;changed=true;break;}}
 if(k==0xB6){tetAt=millis();if(tetFits(tet.x,tet.y+1,tet.rotation)){tet.y++;tetScore++;}else tetLock();changed=true;}
 if(changed)drawTetris();
}
void tetHardDrop(){if(tet.over)return;tetAt=millis();while(tetFits(tet.x,tet.y+1,tet.rotation)){tet.y++;tetScore+=2;}tetLock();tetPaused=false;drawTetris();}
