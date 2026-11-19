#pragma once
namespace JDM {
bool accept(){
 uint8_t car=(state.day*3+state.completed*7+clientChoice)%6;
 uint8_t fault=(state.day+state.completed+clientChoice)%4;
 uint8_t customer=clientChoice%6;
 int quote=campaignServiceValue(fault,"base_price",campaignFallbackPrices[fault]);
 int budget=campaignCustomerValue(customer,"budget",1800);
 if(quote>budget){message="CUSTOMER BUDGET TOO LOW / CHOOSE ANOTHER";return false;}
 if(state.stage>=3&&state.stage<=5){
  if(state.queueCount>=JDM_ORDER_QUEUE_CAPACITY){message="ORDER BOARD FULL / FINISH A JOB";return false;}
  QueuedOrder &q=state.queue[state.queueCount++];
  q.acceptedDay=state.day;q.car=car;q.fault=fault;q.customer=customer;
  message="ADDED TO ORDER BOARD / "+campaignCustomerName(customer);
  return true;
 }
 if(state.stage){message="FINISH DIAGNOSIS OR ORDER FIRST";return false;}
 state.car=car;state.fault=fault;state.customer=customer;
 state.stage=1;state.remaining=(uint8_t)state.day;state.activeAcceptedDay=state.day;state.stock=0;
 message=campaignCustomerName(state.customer)+" / "+String(symptoms[state.fault]);
 return true;
}
bool cancelQueuedOrder(){
 if(queueSelection>=state.queueCount){message="SELECT A WAITING ORDER FIRST";return false;}
 String customer=campaignCustomerName(state.queue[queueSelection].customer);
 for(uint8_t i=queueSelection+1;i<state.queueCount;i++)state.queue[i-1]=state.queue[i];
 state.queueCount--;
 if(queueSelection>=state.queueCount&&queueSelection>0)queueSelection--;
 message="ORDER CANCELLED / "+customer+(state.queueCount?" / SLOT FREED":" / BOARD EMPTY");
 return true;
}
void promoteNextOrder(){
 if(state.stage||!state.queueCount)return;
 QueuedOrder q=state.queue[0];
 for(uint8_t i=1;i<state.queueCount;i++)state.queue[i-1]=state.queue[i];
 state.queueCount--;
 if(queueSelection>=state.queueCount&&queueSelection>0)queueSelection--;
 state.car=q.car;state.fault=q.fault;state.customer=q.customer;
 state.stage=1;state.stock=0;state.remaining=(uint8_t)q.acceptedDay;state.activeAcceptedDay=q.acceptedDay;
 message="NEXT CLIENT ON LIFT / "+campaignCustomerName(state.customer);
}
bool diagnose(){
 if(state.stage!=1){message="ACCEPT A JOB FIRST";return false;}
 if(repairNode!=state.fault){message=String(faults[repairNode])+" TEST OK / TRY ANOTHER NODE";return false;}
 state.stage=2;message=String("TEST FAILED: ")+faults[state.fault]+" / BUY REPLACEMENT";return true;
}
bool manualRepair(){
 if(state.stage==2){if(!state.stock&&!warehouseReserveForFault(state.fault)){message="PART NOT IN STOCK / BUY IN INVENTORY";return false;}repairNode=state.fault;state.stock=0;state.stage=3;state.remaining=10;bolt=0;message="SELECT TOOL / ENTER TO USE";return true;}
 if(state.stage!=3||state.remaining<10){message="MECHANIC WORKING / NEXT DAY";return false;}
 int step=state.remaining-10;
 if(bolt!=step%3){message="SELECT MARKED FASTENER / UP OR DOWN";return false;}
 if(tool!=toolOrder[state.fault][step]){message="WRONG TOOL / FOLLOW TOOL HINT";return false;}
 state.remaining++;
 if(state.remaining==15){state.remaining=0;state.stage=4;message="ASSEMBLED / RUN FINAL TEST";}
 else message=String("DONE / NEXT: ")+repairSteps[state.remaining-10];
 return true;
}
bool order(){
 if(state.stage!=2){message="DIAGNOSE FIRST";return false;}
 if(state.stock){message="PART ALREADY RESERVED";return false;}
 if(warehouseReserveForFault(state.fault)){state.stock=1;message=String("PART PICKED FROM WAREHOUSE / ")+faultPartIds[state.fault];return true;}
 int cost=partCost();if(state.cash<cost){message="NOT ENOUGH CASH";return false;}
 state.cash-=cost;state.expenses+=cost;state.stock=1;
 message=String("PART PURCHASED FOR JOB: ")+faultPartIds[state.fault]+" / "+String(cost)+" CR";return true;
}
bool assign(){
 if(state.stage!=2){message="DIAGNOSE FIRST";return false;}
 if(!state.staff){message="HIRE MECHANIC IN STAFF";return false;}
 if(!state.stock&&!warehouseReserveForFault(state.fault)){message="PART NOT IN STOCK / BUY IN INVENTORY";return false;}
 state.stock=0;state.stage=3;state.remaining=1+state.fault%2;
 message="MECHANIC ASSIGNED / BASIC TOOLS";return true;
}
bool quality(){
 if(state.stage!=4){message="REPAIR MUST FINISH FIRST";return false;}
 if(tool!=3){message="SELECT TESTER WITH LEFT/RIGHT / ENTER";return false;}
 state.stage=5;message="QC PASSED / READY FOR PAYMENT";return true;
}
bool collect(){
 if(state.stage!=5){message="QUALITY CHECK REQUIRED";return false;}
 int paid=payment();
 int tipChance=campaignCustomerValue(state.customer,"tip_chance",10);
 if(((state.day*13u+state.completed*7u+state.customer*11u)%100)<tipChance)paid+=max(25,paid/20);
 state.cash+=paid;state.revenue+=paid;state.completed++;
 state.reputation+=campaignJobRep();
 state.reputation+=max(0,campaignEventEffect(state.day,"reputation_bonus",0));
 state.lastProfit=state.revenue-state.expenses;state.stage=0;state.stock=0;state.remaining=0;state.activeAcceptedDay=0;
 message="JOB CLOSED / "+campaignCustomerName(state.customer)+" / "+String(paid)+" CR";
 promoteNextOrder();
 return true;
}
}
