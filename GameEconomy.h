#pragma once
namespace JDM {
bool hire(){if(state.staff){message="ONE POST / ONE MECHANIC";return false;}if(state.cash<500){message="HIRING COST: 500";return false;}state.cash-=500;state.expenses+=500;state.staff=1;message="MECHANIC HIRED / SKILL 1";return true;}
void nextDay(){
 int cost=contentDailyRent+(state.staff?contentMechanicWage:0);
 state.cash-=cost;state.expenses+=cost;state.lastProfit=state.revenue-state.expenses;state.day++;
 // Customer patience applies while a car is waiting for diagnosis or parts.
 // Once repair work is underway, the job is no longer in the waiting queue.
 if(state.stage==1||state.stage==2){
  uint8_t elapsed=(uint8_t)((uint8_t)state.day-state.remaining);
  int patience=campaignCustomerValue(state.customer,"patience_days",3);
  if(elapsed>=constrain(patience,1,255)){
   int refund=state.stock?partCost()/2:0;
   state.cash+=refund;state.stock=0;state.stage=0;state.remaining=0;
   if(state.reputation)state.reputation--;
   message="CLIENT LEFT / DEADLINE MISSED / REFUND "+String(refund)+" CR";
   return;
  }
 }
 message="DAY "+String(state.day)+" / "+campaignEventTitle(state.day)+" / COST "+String(cost);
}
}
