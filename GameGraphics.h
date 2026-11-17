#pragma once
#include "GarageSprite.h"
namespace JDM {
const char* pages[]={"GARAGE","JOBS","DIAGNOSTICS","REPAIR","INVENTORY","STAFF","FINANCE","SETTINGS"};
const char* actions[]={"ACCEPT CLIENT CAR","DIAGNOSE SYMPTOMS","ORDER PART","ASSIGN MECHANIC","QUALITY CHECK","COLLECT PAYMENT","NEXT WORKING DAY","SAVE SLOT","LOAD SLOT","NEW GAME"};
void line(int x,int y,int X,int Y){epd.drawLine(x,y,X,Y,GxEPD_BLACK);}
void box(int x,int y,int w,int h){epd.drawRect(x,y,w,h,GxEPD_BLACK);}
void solid(int x,int y,int w,int h){epd.fillRect(x,y,w,h,GxEPD_BLACK);}
void wheel(int x,int y){epd.fillCircle(x,y,20,GxEPD_BLACK);epd.fillCircle(x,y,13,GxEPD_WHITE);epd.drawCircle(x,y,10,GxEPD_BLACK);for(int i=0;i<5;i++){float a=i*1.256637f;line(x,y,x+int(10*cosf(a)),y+int(10*sinf(a)));}epd.fillCircle(x,y,3,GxEPD_BLACK);}
// Original JDM-inspired profiles: hatch, coupe, sedan and roadster.
void carArt(int m){
 const int roofs[10]={220,218,225,217,224,231,211,209,213,236};const int tails[10]={112,123,116,134,143,147,115,112,103,143};
 int roof=roofs[m],rear=tails[m],front=m==5?330:305;
 epd.fillTriangle(74,302,rear,273,455,302,GxEPD_BLACK);epd.fillRect(74,302,380,29,GxEPD_BLACK);epd.fillTriangle(rear,273,177,roof,front,273,GxEPD_BLACK);epd.fillTriangle(177,roof,front-39,roof,front,273,GxEPD_BLACK);
 epd.fillRect(83,291,355,30,GxEPD_WHITE);epd.fillTriangle(91,291,rear+8,276,429,291,GxEPD_WHITE);epd.fillTriangle(rear+12,268,181,roof+7,front-13,268,GxEPD_WHITE);epd.fillTriangle(181,roof+7,front-42,roof+7,front-13,268,GxEPD_WHITE);
 line(rear,273,177,roof);line(177,roof,front-39,roof);line(front-39,roof,front,273);line(front,273,446,280);line(446,280,462,305);line(462,305,454,334);line(74,302,74,330);line(74,330,454,334);
 for(int y=roof+12;y<266;y+=5)line(183,y,front-45+(y-roof)/2,y);solid(226,roof+6,6,62-(roof-215));line(234,272,234,315);line(234,315,324,315);line(324,315,333,272);box(292,280,18,4);solid(330,266,14,6);line(332,273,342,270);line(91,299,433,299);line(159,321,370,321);for(int y=324;y<333;y+=3)line(177,y,355,y);
 if(m==0){solid(77,279,38,8);line(121,277,151,246);solid(78,312,47,5);}if(m==1){box(418,281,30,11);box(77,284,27,9);solid(71,258,64,5);line(87,263,87,277);line(116,263,116,277);}if(m==2||m==4){epd.fillTriangle(401,278,411,260,428,281,GxEPD_BLACK);box(406,264,15,9);}if(m==3){epd.drawRoundRect(409,281,36,12,5,GxEPD_BLACK);solid(89,256,61,5);line(107,261,107,277);}if(m==5){solid(401,279,33,6);for(int i=0;i<5;i++)line(359+i*5,296,359+i*5,306);}
 box(427,311,26,10);for(int i=0;i<4;i++)line(429,314+i*2,450,314+i*2);box(77,307,18,9);line(88,333,435,336);
 for(int x: {143,389}){epd.fillCircle(x,326,29,GxEPD_WHITE);epd.drawCircle(x,326,29,GxEPD_BLACK);epd.fillCircle(x,328,23,GxEPD_BLACK);epd.fillCircle(x,328,16,GxEPD_WHITE);epd.drawCircle(x,328,14,GxEPD_BLACK);for(int i=0;i<8;i++){float a=i*0.785398f;line(x,328,x+int(14*cosf(a)),328+int(14*sinf(a)));epd.fillCircle(x+int(19*cosf(a)),328+int(19*sinf(a)),1,GxEPD_WHITE);}}
 if(state.stage>=2&&state.stage<=4){epd.fillTriangle(350,271,386,230,442,272,GxEPD_WHITE);line(350,271,386,230);line(386,230,442,272);box(354,277,43,18);for(int i=0;i<5;i++)box(359+i*7,280,4,10);}
}
void garage(){
 box(35,140,446,247);line(35,360,481,360);box(43,170,42,160);for(int i=0;i<5;i++)box(48,185+i*26,31,17);box(410,150,61,66);for(int i=0;i<4;i++)line(418,159+i*13,461,159+i*13);line(99,371,428,371);solid(104,363,8,16);solid(420,363,8,16);
 if(state.stage)drawCarSprite(state.car % JDM_CAR_ASSET_COUNT,132,230);else text("ENTER: ACCEPT A CLIENT",112,290,true);
 text(state.stage?cars[state.car]:"YOUR WORKSHOP",32,414);
 text("COMPLETED "+String(state.completed)+" / REP "+String(state.reputation),32,440);
}
void repairScreen(){
 text(String("< ")+faults[repairNode]+" >",32,151,true);bool target=state.stage>=2&&state.fault==repairNode;text(target?"CLIENT FAULT / DIAGNOSED":"COMPONENT VIEW",32,181);
 const uint8_t repairPartAsset[4]={7,10,11,6};drawPartSprite(repairPartAsset[repairNode],315,220);
 text("01  MOUNT / BRACKET",32,224);text("02  FASTENER POINTS",32,251);text("03  FLUID / POWER LINE",32,278);text("04  CONNECTOR / SEAL",32,305);text("NODE MAP: SELECTED SYSTEM",32,369);
 if(state.stage==3&&state.remaining>=10){int step=state.remaining-10;int shift=step<2?step*12:(4-step)*12;box(430-shift,286,32,35);line(433-shift,290,457-shift,316);for(int i=0;i<3;i++){int x=123+i*113;epd.drawCircle(x,392,16,GxEPD_BLACK);if(i==bolt)epd.drawCircle(x,392,20,GxEPD_BLACK);line(x-8,392,x+8,392);line(x,384,x,400);if(i==step%3)epd.fillCircle(x,392,4,GxEPD_BLACK);}text(String(step+1)+"/5  "+repairSteps[step],32,432);}
 else{text(state.stage==1?"LEFT/RIGHT: NODE / ENTER: TEST":!target?"SELECT CLIENT FAULT NODE":state.stage==2?(state.stock?"ENTER: START HAND REPAIR":"ENTER: BUY REPLACEMENT"):state.stage==3?"MECHANIC / NEXT DAY":state.stage==4?String("QC TOOL: ")+tools[tool]+" / NEED TESTER":"NODE VIEW",32,412);}
}
String nextHint(){if(!active)return "START A GARAGE OR LOAD A SLOT";switch(state.stage){case 0:return "ACCEPT A CAR / BUILD REPUTATION";case 1:return "MATCH SYMPTOM TO NODE / TEST IT";case 2:return state.stock?"REPAIR YOURSELF OR M: DELEGATE":"BUY PART: "+String(partCost());case 3:return state.remaining>=10?"LEFT/RIGHT: TOOL / ENTER: USE":"NEXT DAY / WAIT FOR MECHANIC";case 4:return "RUN QUALITY TEST BEFORE PAYMENT";default:return "COLLECT "+String(payment())+" / NEXT CLIENT";}}
String primaryLabel(){if(!active)return "START GARAGE";switch(state.stage){case 0:return "ACCEPT CLIENT";case 1:return "TEST SELECTED NODE";case 2:return state.stock?"START HAND REPAIR":"BUY PART "+String(partCost());case 3:return state.remaining>=10?"USE SELECTED TOOL":"NEXT WORKING DAY";case 4:return "QUALITY TEST";default:return "COLLECT "+String(payment());}}
void orderBoard(){
 text("CUSTOMER ORDER BOARD",30,145,true);
 text("ACTIVE BAY",30,184,true);
 if(state.stage){
  text(cars[state.car]+" / "+campaignCustomerName(state.customer),42,216);
  text(String(faults[state.fault])+" / "+(state.stage<=2?"WAITING":state.stage==3?"IN REPAIR":state.stage==4?"QUALITY CHECK":"READY TO COLLECT"),42,244);
  text("BUDGET "+String(campaignCustomerValue(state.customer,"budget",1800))+" CR",42,270);
 }else text("NO ACTIVE CAR IN THE BAY",42,222);
 line(30,292,475,292);
 text("WAITING LIST  "+String(state.queueCount)+"/"+String(JDM_ORDER_QUEUE_CAPACITY),30,326,true);
 for(uint8_t i=0;i<JDM_ORDER_QUEUE_CAPACITY;i++){
  int y=362+i*39;
  if(i<state.queueCount){
   QueuedOrder &q=state.queue[i];
   text(String(i+1)+". "+campaignCustomerName(q.customer)+" / "+cars[q.car],42,y);
   text(String(faults[q.fault])+" / DAY "+String(q.acceptedDay),58,y+22);
  }else text(String(i+1)+". EMPTY SLOT",42,y);
 }
 text("N: ACCEPT ANOTHER CLIENT WHILE REPAIRING",30,448,true);
}
void inventoryScreen(){
 text("PARTS WAREHOUSE / LIVE STOCK",30,139,true);
 text("PART                     ON HAND   INBOUND   UNIT COST",30,170);
 line(30,180,480,180);
 for(uint8_t i=0;i<JDM_PART_COUNT;i++){
  String name;int cost=100,quality=1,stock=0;inventoryReadPart(i,name,cost,quality,stock);
  int y=207+i*25;
  if(i==inventorySelection){solid(26,y-18,456,23);inv(String(i+1)+". "+clip(name,25),34,y);inv(String(state.inventory[i]),300,y);inv(String(state.inbound[i])+(state.inbound[i]?" / D"+String(state.inboundDay[i]):""),365,y);inv(String(inventoryCost(i)),448,y);}
  else{text(String(i+1)+". "+clip(name,25),34,y);text(String(state.inventory[i]),300,y);text(String(state.inbound[i])+(state.inbound[i]?" / D"+String(state.inboundDay[i]):""),365,y);text(String(inventoryCost(i)),448,y);}
 }
 line(30,455,480,455);
 text("UP/DOWN: SELECT   ENTER: ORDER 1   DELIVERY: 1-3 DAYS",30,477,true);
}
void staffScreen(){
 text("WORKSHOP STAFF / MECHANIC ROSTER",30,139,true);
 text("EMPLOYEES "+String(state.staff)+"/2   DAILY PAY "+String(mechanicWageTotal())+" CR",30,174,true);
 line(30,188,480,188);
 if(!state.staff)text("NO MECHANICS HIRED / PRESS H",42,232,true);
 for(uint8_t i=0;i<state.staff;i++){
  int y=226+i*82;
  if(i==staffSelection){solid(28,y-25,450,65);inv(String("#")+String(i+1)+"  GARAGE MECHANIC",42,y);inv("SKILL "+String(state.mechanicSkill[i])+" / "+(state.mechanicSkill[i]==1?"TRAINEE":state.mechanicSkill[i]==2?"TECHNICIAN":"EXPERT"),42,y+25);inv("WAGE "+String(contentMechanicWage+(state.mechanicSkill[i]-1)*75)+" CR/DAY",300,y+25);}
  else{text(String("#")+String(i+1)+"  GARAGE MECHANIC",42,y);text("SKILL "+String(state.mechanicSkill[i])+" / "+(state.mechanicSkill[i]==1?"TRAINEE":state.mechanicSkill[i]==2?"TECHNICIAN":"EXPERT"),42,y+25);text("WAGE "+String(contentMechanicWage+(state.mechanicSkill[i]-1)*75)+" CR/DAY",300,y+25);}
 }
 line(30,405,480,405);
 text("H: HIRE   T: TRAIN SELECTED   UP/DOWN: SELECT",30,438,true);
 text("TRAINING COST: 600 CR PER CURRENT SKILL LEVEL",30,468);
}
void contentScreen(){
 text(contentTitle,30,128,true);text("CONTENT VERSION: "+contentVersion+"  /  SOURCE: "+String(contentLoaded?"SD CARD":"BUILT-IN"),30,164);
 text("CARS "+String(contentCars)+"   SERVICES "+String(contentServices)+"   PARTS "+String(contentParts),30,212,true);text("CUSTOMERS "+String(contentCustomers)+"   UPGRADES "+String(contentUpgrades),30,250);text("EVENTS "+String(contentEvents)+"   TUTORIAL CHAPTERS "+String(contentTutorial),30,288);text("FIRST CAR: "+clip(firstCar,42)+"  /  SERVICE: "+clip(firstService,28),30,332);text("FIRST PART: "+clip(firstPart,42),30,364);text("START CASH "+String(contentStartingCash)+" CR  /  DAILY RENT "+String(contentDailyRent),30,408);text(clip(contentStatus,72),30,440,true);
}
void draw(bool full=false){
 view=JDM_GAME;if(full)epd.forceClean();beginDraw();do{
 header("","JDM / WORKSHOP","I: INVENTORY   S: STAFF   J: ORDERS   N: NEW CLIENT   ENTER: USE");
 text("DAY "+String(state.day)+"   CASH "+String(state.cash)+"   MECHANICS "+String(state.staff),30,108,true);
 if(page==7)contentScreen();else if(page==5)staffScreen();else if(page==4)inventoryScreen();else if(page==1)orderBoard();else if(page==3&&state.stage)repairScreen();else garage();
 line(500,120,500,444);
 String labels[]={primaryLabel(),"NEXT DAY (-"+String(contentDailyRent+mechanicWageTotal())+")","SAVE SLOT","LOAD SLOT","NEW GARAGE"};
 for(int i=0;i<5;i++){int y=147+i*39;if(i==row){solid(512,y-23,283,34);inv(labels[i],520,y);}else text(labels[i],520,y);}
 text("TOOLS",515,350,true);
 for(int i=0;i<4;i++){int x=538+i*66;box(x-22,363,45,43);if(i==tool)box(x-25,360,51,49);if(i==0){line(x-10,397,x+9,375);epd.drawCircle(x+9,375,7,GxEPD_BLACK);}if(i==1){line(x-10,395,x+10,373);line(x+10,395,x-10,373);}if(i==2){solid(x-3,385,6,15);line(x,385,x,370);line(x-4,370,x+4,370);}if(i==3){box(x-12,371,24,25);box(x-8,375,16,8);line(x-6,396,x-10,402);}}
 text(String(tools[tool]),516,433);solid(0,452,800,28);inv(clip(message,70),10,472);
 if(popup){
 solid(94,154,616,228);epd.fillRect(88,148,616,228,GxEPD_WHITE);box(88,148,616,228);box(94,154,604,216);
 text(popup==2?"CLIENT / CHOOSE CAR":"WORKSHOP",112,187,true);
 if(popup==2){
 int m=(state.day*3+state.completed*7+clientChoice)%6;int f=(state.day+state.completed+clientChoice)%4;
 text(cars[m],116,220,true);text(campaignCustomerName(clientChoice),116,250,true);
 text("JOB: "+String(faults[f])+" / QUOTE "+String(campaignServiceValue(f,"base_price",payment()))+" CR",116,280);
 text("BUDGET "+String(campaignCustomerValue(clientChoice,"budget",1800))+" CR / PATIENCE "+String(campaignCustomerValue(clientChoice,"patience_days",3))+"D",116,310);
 text("LEFT / RIGHT: CUSTOMER   ENTER: ACCEPT",116,342);
 }else{
 epd.drawCircle(142,246,25,GxEPD_BLACK);line(127,246,139,258);line(139,258,159,232);String t=message;int cut=t.indexOf(" / ");if(cut<0)cut=t.length();text(clip(t.substring(0,cut),39),185,242);text(clip(t.substring(min(cut+3,(int)t.length())),39),116,291);text("ENTER: CONTINUE",116,342);
 }}
 }while(epd.nextPage());
}
void open(){popup=0;bolt=0;page=0;row=0;active=false;confirmNew=false;repairNode=tool=0;prepareAssets();loadContent();message=contentLoaded?"SD CONTENT READY / ENTER: NEW GARAGE":"BUILT-IN DATA / C: CONTENT INFO";draw(true);}
}
