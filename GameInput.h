#pragma once
namespace JDM {
int primaryAction(){if(!active)return 9;switch(state.stage){case 0:return 0;case 1:return 1;case 2:return state.stock?3:2;case 3:return state.remaining>=10?3:6;case 4:return 4;default:return 5;}}
bool keyEvent(uint8_t k){
 if(popup){
 if(popup==2&&(k==0xB4||k==0xB7)){clientChoice=(clientChoice+(k==0xB4?5:1))%6;draw();return true;}
 if(k==27||k==8){popup=0;draw();return true;}
 if(k==10||k==13){int old=popup;popup=0;if(old==2){if(accept()){page=0;row=0;String result=message;save();message=result;popup=1;}}draw();return true;}return true;
 }
 if(active&&page!=4&&state.stage==3&&state.remaining>=10&&(k==0xB5||k==0xB6)){bolt=(bolt+(k==0xB5?2:1))%3;draw();return true;}

 if(k==27||k==8){if(confirmNew){confirmNew=false;message="NEW GAME CANCELLED";draw();return true;}if(active&&!save()){draw();return true;}games();return true;}
 if(k==9){page=page==0?3:0;draw();return true;}
 if(k=='j'||k=='J'){page=page==1?0:1;draw();return true;}
 if((k=='n'||k=='N')&&active&&state.stage>=3&&state.stage<=5){popup=2;clientChoice=0;page=1;draw();return true;}
 if(k=='i'||k=='I'){page=page==4?0:4;draw();return true;}
 if(page==4&&(k==0xB5||k==0xB6)){inventorySelection=(inventorySelection+(k==0xB5?JDM_PART_COUNT-1:1))%JDM_PART_COUNT;draw();return true;}
 if(page==4&&(k==10||k==13)){bool bought=inventoryPurchase(inventorySelection);if(bought){String result=message;if(save())message=result;else message=result+" / NOT SAVED";}draw();return true;}
 if(k=='c'||k=='C'){page=page==7?0:7;draw();return true;}
 if((k==0xB4||k==0xB7)&&page!=4){if((state.stage==3&&state.remaining>=10)||state.stage==4)tool=(tool+(k==0xB4?3:1))%4;else repairNode=(repairNode+(k==0xB4?3:1))%4;page=3;draw();return true;}
 if(k==0xB5||k==0xB6){row=(row+(k==0xB5?4:1))%5;confirmNew=false;draw();return true;}
 if(k>='1'&&k<='3'){slot=k-'0';message="SLOT "+String(slot);draw();return true;}
 bool changed=false;
 if(k=='h'||k=='H'){if(active)changed=hire();else message="START OR LOAD FIRST";}
 else if(k=='m'||k=='M'){if(active)changed=assign();}
 else if(k==10||k==13){
 int action=row==0?primaryAction():row==1?6:row==2?7:row==3?8:9;
 if(action==9){if(!confirmNew){confirmNew=true;message="ENTER AGAIN TO RESET THIS GARAGE";}else{state=State{};applyStartingConditions();inventoryInit();active=true;confirmNew=false;repairNode=tool=0;row=0;message="GARAGE OPEN / ENTER TO ACCEPT A CAR";}}
 else if(action==8){if(load()){row=0;repairNode=state.fault;}}
 else if(!active)message="START OR LOAD FIRST";
 else switch(action){case 0:popup=2;clientChoice=0;page=0;break;case 1:changed=diagnose();page=3;break;case 2:changed=order();break;case 3:changed=manualRepair();page=3;break;case 4:changed=quality();break;case 5:changed=collect();page=0;break;case 6:if(state.stage==3&&state.remaining>=10){message="FINISH HAND REPAIR BEFORE NEXT DAY";}else{nextDay();changed=true;}break;case 7:save();break;}
 }else return true;
 if(changed){String result=message;if(save())message=result;else message=result+" / NOT SAVED";row=0;}
 if(!popup&&(changed||k=='h'||k=='H'||k=='m'||k=='M'))popup=1;
 draw();return true;
}
}
