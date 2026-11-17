#pragma once
namespace JDM {
bool accept(){if(state.stage){message="FINISH CURRENT JOB FIRST";return false;}state.car=(state.day*3+state.completed*7+clientChoice)%6;state.fault=(state.day+state.completed+clientChoice)%4;state.stage=1;state.remaining=0;state.stock=0;message=symptoms[state.fault];return true;}
bool diagnose(){
 if(state.stage!=1){message="ACCEPT A JOB FIRST";return false;}
 if(repairNode!=state.fault){message=String(faults[repairNode])+" TEST OK / TRY ANOTHER NODE";return false;}
 state.stage=2;message=String("TEST FAILED: ")+faults[state.fault]+" / BUY REPLACEMENT";return true;
}
bool manualRepair(){
 if(state.stage==2){if(!state.stock){message="BUY A REPLACEMENT FIRST";return false;}repairNode=state.fault;state.stock=0;state.stage=3;state.remaining=10;bolt=0;message="SELECT TOOL / ENTER TO USE";return true;}
 if(state.stage!=3||state.remaining<10){message="MECHANIC WORKING / NEXT DAY";return false;}
 int step=state.remaining-10;
 if(bolt!=step%3){message="SELECT MARKED FASTENER / UP OR DOWN";return false;}
 if(tool!=toolOrder[state.fault][step]){message="WRONG TOOL / FOLLOW TOOL HINT";return false;}
 state.remaining++;
 if(state.remaining==15){state.remaining=0;state.stage=4;message="ASSEMBLED / RUN FINAL TEST";}
 else message=String("DONE / NEXT: ")+repairSteps[state.remaining-10];
 return true;
}
bool order(){if(state.stage!=2){message="DIAGNOSE FIRST";return false;}if(state.stock){message="PART ALREADY IN STOCK";return false;}int cost=partCost();if(state.cash<cost){message="NOT ENOUGH CASH";return false;}state.cash-=cost;state.expenses+=cost;state.stock=1;message="COMPATIBLE PART DELIVERED";return true;}
bool assign(){if(state.stage!=2||!state.stock){message="NEED DIAGNOSIS AND PART";return false;}if(!state.staff){message="HIRE MECHANIC IN STAFF";return false;}state.stock=0;state.stage=3;state.remaining=1+state.fault%2;message="MECHANIC ASSIGNED / BASIC TOOLS";return true;}
bool quality(){if(state.stage!=4){message="REPAIR MUST FINISH FIRST";return false;}if(tool!=3){message="SELECT TESTER WITH LEFT/RIGHT / ENTER";return false;}state.stage=5;message="QC PASSED / READY FOR PAYMENT";return true;}
bool collect(){if(state.stage!=5){message="QUALITY CHECK REQUIRED";return false;}int paid=payment();state.cash+=paid;state.revenue+=paid;state.completed++;state.reputation++;state.lastProfit=state.revenue-state.expenses;state.stage=0;message="PAYMENT RECEIVED: "+String(paid);return true;}
}
