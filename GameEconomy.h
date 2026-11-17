#pragma once
namespace JDM {
int mechanicWageTotal(){int total=0;for(uint8_t i=0;i<state.staff;i++)total+=contentMechanicWage+(max(1,(int)state.mechanicSkill[i])-1)*75;return total;}
bool hire(){if(state.staff>=2){message="ROSTER FULL / MAX 2 MECHANICS";return false;}int cost=500+state.staff*350;if(state.cash<cost){message="HIRING COST: "+String(cost);return false;}state.cash-=cost;state.expenses+=cost;state.mechanicSkill[state.staff]=1;state.staff++;message="MECHANIC HIRED / SKILL 1 / WAGE "+String(contentMechanicWage);return true;}
bool trainMechanic(){if(!state.staff){message="HIRE A MECHANIC FIRST";return false;}uint8_t i=constrain(staffSelection,0,(int)state.staff-1);uint8_t skill=state.mechanicSkill[i];if(skill>=3){message="MECHANIC ALREADY EXPERT";return false;}int cost=600*skill;if(state.cash<cost){message="TRAINING COST: "+String(cost);return false;}state.cash-=cost;state.expenses+=cost;state.mechanicSkill[i]++;message=String("MECHANIC #")+String(i+1)+" TRAINED / SKILL "+String(state.mechanicSkill[i]);return true;}
void nextDay(){
 int cost=contentDailyRent+mechanicWageTotal();
 state.cash-=cost;state.expenses+=cost;state.lastProfit=state.revenue-state.expenses;state.day++;
 String mechanicDone="";
 if(state.stage==3&&state.remaining>0&&state.remaining<10){state.remaining--;if(!state.remaining){state.stage=4;state.assignedMechanic=255;mechanicDone="MECHANIC FINISHED / RUN QUALITY CHECK";}}
 // Every waiting order has its own deadline. Work already underway is protected.
 String expired="";
 if(state.stage==1||state.stage==2){
  uint32_t elapsed=state.day-state.activeAcceptedDay;
  int patience=campaignCustomerValue(state.customer,"patience_days",3);
  if(elapsed>=(uint32_t)constrain(patience,1,255)){
   inventoryReturnReservedPart();
   state.stage=0;state.remaining=0;state.activeAcceptedDay=0;
   if(state.reputation)state.reputation--;
   expired="ACTIVE CLIENT LEFT / PART RETURNED TO STOCK";
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
 String deliveries=inventoryDeliverDue();
 if(!state.stage&&state.queueCount)promoteNextOrder();
 message=expired.length()?expired:(mechanicDone.length()?mechanicDone:(deliveries.length()?deliveries:("DAY "+String(state.day)+" / "+campaignEventTitle(state.day)+" / COST "+String(cost))));
}
}
