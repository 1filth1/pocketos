#pragma once
namespace JDM {
bool hire(){if(state.staff){message="ONE POST / ONE MECHANIC";return false;}if(state.cash<500){message="HIRING COST: 500";return false;}state.cash-=500;state.expenses+=500;state.staff=1;message="MECHANIC HIRED / SKILL 1";return true;}
void nextDay(){
 int cost=contentDailyRent+(state.staff?contentMechanicWage:0);
 state.cash-=cost;state.expenses+=cost;state.lastProfit=state.revenue-state.expenses;state.day++;
 // Every waiting order has its own deadline. Work already underway is protected.
 String expired="";
 if(state.stage==1||state.stage==2){
  uint32_t elapsed=state.day-state.activeAcceptedDay;
  int patience=campaignCustomerValue(state.customer,"patience_days",3);
  if(elapsed>=(uint32_t)constrain(patience,1,255)){
   int refund=state.stock?partCost()/2:0;
   state.cash+=refund;state.stock=0;state.stage=0;state.remaining=0;state.activeAcceptedDay=0;
   if(state.reputation)state.reputation--;
   expired="ACTIVE CLIENT LEFT / REFUND "+String(refund)+" CR";
  }
 }
 for(uint8_t i=0;i<state.queueCount;){
  QueuedOrder &q=state.queue[i];
  int patience=campaignCustomerValue(q.customer,"patience_days",3);
  if(state.day-q.acceptedDay>=(uint32_t)constrain(patience,1,255)){
   if(state.reputation)state.reputation--;
   expired="QUEUED CLIENT LEFT / "+campaignCustomerName(q.customer);
   for(uint8_t j=i+1;j<state.queueCount;j++)state.queue[j-1]=state.queue[j];
   state.queueCount--;
  }else i++;
 }
 if(!state.stage&&state.queueCount)promoteNextOrder();
 message=expired.length()?expired:("DAY "+String(state.day)+" / "+campaignEventTitle(state.day)+" / COST "+String(cost));
}
}
