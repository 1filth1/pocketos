#pragma once
// Original monochrome icon pack. Native 56px artwork, no SD or JPEG dependency.
// Uniform two-pixel outlines and generous negative space survive e-paper refresh.
static void drawAppIcon(int id,int x,int y){
 const uint16_t B=GxEPD_BLACK,W=GxEPD_WHITE;
 auto line=[&](int a,int b,int c,int d){epd.drawLine(x+a,y+b,x+c,y+d,B);epd.drawLine(x+a+1,y+b,x+c+1,y+d,B);};
 auto box=[&](int a,int b,int w,int h,int r){epd.drawRoundRect(x+a,y+b,w,h,r,B);epd.drawRoundRect(x+a+1,y+b+1,w-2,h-2,max(0,r-1),B);};
 auto dot=[&](int a,int b,int r){epd.fillCircle(x+a,y+b,r,B);};
 auto circle=[&](int a,int b,int r){epd.drawCircle(x+a,y+b,r,B);epd.drawCircle(x+a,y+b,r-1,B);};
 switch(id){
 case 0: // folder: tab, layered pages, open front
  box(5,12,44,35,4);epd.fillRect(x+7,y+7,20,9,B);
  box(12,17,30,22,2);line(17,23,36,23);line(17,29,33,29);
  epd.fillRoundRect(x+3,y+29,50,22,4,W);box(3,29,50,22,4);line(12,44,26,44);break;
 case 1: // bound notebook and pencil
  box(9,5,33,46,4);for(int j=13;j<46;j+=10){box(5,j,9,4,1);}
  line(19,16,34,16);line(19,24,32,24);line(19,32,27,32);
  epd.fillTriangle(x+29,y+45,x+34,y+48,x+49,y+23,B);
  epd.fillTriangle(x+29,y+45,x+34,y+48,x+28,y+51,B);break;
 case 2: // calendar with rings and marked date
  box(5,10,46,41,4);epd.fillRect(x+7,y+18,42,6,B);
  box(15,4,5,13,2);box(36,4,5,13,2);
  for(int j=0;j<2;j++)for(int k=0;k<3;k++)epd.fillRect(x+13+k*12,y+29+j*12,5,5,B);
  box(33,37,13,12,2);break;
 case 3: // stopwatch
  circle(28,32,20);box(21,3,14,5,2);line(27,8,27,12);line(43,13,48,9);
  line(28,19,28,32);line(28,32,38,37);dot(28,32,3);
  for(int j=-1;j<=1;j+=2){line(28+j*15,31,28+j*17,31);line(27,32+j*15,27,32+j*17);}break;
 case 4: // miniature studio reel deck
  box(3,8,50,43,4);circle(16,23,10);circle(40,23,10);
  for(int j=0;j<2;j++){int c=16+j*24;dot(c,23,2);dot(c,17,2);dot(c-5,26,2);dot(c+5,26,2);}
  line(9,31,16,38);line(16,38,40,38);line(40,38,47,31);
  dot(13,45,2);epd.fillTriangle(x+24,y+42,x+24,y+48,x+30,y+45,B);epd.fillRect(x+37,y+42,6,6,B);break;
 case 5: // paired musical notes, staff
  line(6,44,50,44);line(6,38,50,38);line(6,32,50,32);
  epd.fillRect(x+21,y+10,4,32,B);epd.fillRect(x+43,y+5,4,30,B);
  epd.fillTriangle(x+21,y+10,x+47,y+5,x+47,y+13,B);epd.fillTriangle(x+21,y+10,x+21,y+18,x+47,y+13,B);
  epd.fillCircle(x+17,y+42,6,B);epd.fillCircle(x+39,y+35,6,B);break;
 case 6: // globe on stand
  circle(28,26,21);epd.drawEllipse(x+28,y+26,10,20,B);epd.drawEllipse(x+28,y+26,9,20,B);
  line(8,25,48,25);line(12,15,44,15);line(12,36,44,36);line(27,47,27,51);line(16,52,40,52);break;
 case 7: // two documents, directional exchange
  box(3,6,19,27,3);line(8,13,16,13);line(8,19,16,19);
  box(34,24,19,27,3);line(39,38,47,38);line(39,44,47,44);
  line(27,13,49,13);line(43,7,49,13);line(43,19,49,13);
  line(6,43,28,43);line(6,43,12,37);line(6,43,12,49);break;
 case 8: // game controller
  box(3,17,50,31,10);line(16,24,16,38);line(9,31,23,31);
  dot(39,27,3);dot(46,34,3);line(25,41,30,41);line(20,17,20,10);line(20,10,35,10);break;
 case 9: // satellite: solar panels, dish and radio waves
  box(3,5,16,18,1);box(37,33,16,18,1);line(11,6,11,22);line(4,14,18,14);line(45,34,45,50);line(38,42,52,42);
  line(18,21,24,27);line(31,32,38,37);box(21,22,15,14,3);
  epd.fillTriangle(x+27,y+18,x+39,y+29,x+41,y+16,B);line(39,17,45,10);dot(46,9,2);
  line(8,34,8,47);line(8,47,22,47);line(14,34,14,41);line(14,41,22,41);break;
 case 10: // gear with eight consistent teeth
  for(int j=0;j<8;j++){float a=j*PI/4;int cx=28+int(cosf(a)*20),cy=28+int(sinf(a)*20);epd.fillRoundRect(x+cx-5,y+cy-5,10,10,2,B);}
  epd.fillCircle(x+28,y+28,21,B);epd.fillCircle(x+28,y+28,15,W);circle(28,28,9);break;
 case 11: // quick controls, three distinct sliders
  box(4,5,48,46,5);
  for(int j=0;j<3;j++){int c=15+j*13,t=j==0?19:j==1?36:25;line(c,12,c,44);epd.fillRect(x+c-4,y+t-4,10,9,W);box(c-4,t-4,10,9,2);}break;
 case 12: // App Manager: package list with a verified check mark
  box(6,5,43,46,4);
  for(int j=0;j<3;j++){int yy=16+j*12;box(12,yy-4,7,7,1);line(24,yy,42,yy);}
  line(14,40,21,47);line(21,47,39,30);break;
 }
}
