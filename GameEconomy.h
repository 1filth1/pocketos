#pragma once
namespace JDM {
bool hire(){if(state.staff){message="ONE POST / ONE MECHANIC";return false;}if(state.cash<500){message="HIRING COST: 500";return false;}state.cash-=500;state.expenses+=500;state.staff=1;message="MECHANIC HIRED / SKILL 1";return true;}
void nextDay(){int cost=contentDailyRent+(state.staff?contentMechanicWage:0);state.cash-=cost;state.expenses+=cost;state.lastProfit=state.revenue-state.expenses;state.day++;if(state.stage==3&&state.remaining&&state.remaining<10){state.remaining--;if(!state.remaining)state.stage=4;}message="DAY "+String(state.day)+" / RENT+WAGES: "+String(cost);}
}
