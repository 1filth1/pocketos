#pragma once
namespace JDM {
static const char* warehouseIds[10]={"brake_pads","brake_rotors","fuel_pump","fuel_filter","radiator","water_pump","coilover_kit","bushing_set","turbo_kit","forged_internals"};
static const char* warehouseNames[10]={"BRAKE PADS","BRAKE ROTORS","FUEL PUMP","FUEL FILTER","RADIATOR","WATER PUMP","COILOVER KIT","BUSHING SET","TURBO KIT","FORGED INTERNALS"};
static const uint16_t warehouseFallbackCosts[10]={180,320,260,75,390,210,780,240,2450,3100};
uint8_t partSelection=0;
int warehouseIndexForFault(uint8_t fault){static const uint8_t map[4]={0,2,4,6};return map[constrain((int)fault,0,3)];}
int warehouseCost(uint8_t index){
 if(index>=10)return 0;
 JsonDocument doc;
 if(!campaignRead("parts.json",doc))return warehouseFallbackCosts[index];
 for(JsonObjectConst item:doc["parts"].as<JsonArrayConst>())
  if(String(item["id"]|"")==warehouseIds[index])return item["cost"]|warehouseFallbackCosts[index];
 return warehouseFallbackCosts[index];
}
void initializeWarehouseStock(){
 for(uint8_t i=0;i<10;i++)state.inventory[i]=0;
 JsonDocument doc;
 if(!campaignRead("parts.json",doc))return;
 for(JsonObjectConst item:doc["parts"].as<JsonArrayConst>()){
  String id=item["id"]|"";
  for(uint8_t i=0;i<10;i++)if(id==warehouseIds[i]){
   int count=constrain((int)(item["stock"]|0),0,99);state.inventory[i]=(uint8_t)count;break;
  }
 }
}
bool warehouseBuy(uint8_t index){
 if(index>=10){message="UNKNOWN PART";return false;}
 int cost=warehouseCost(index);
 if(state.inventory[index]>=99){message="WAREHOUSE SLOT FULL";return false;}
 if(state.cash<cost){message="NOT ENOUGH CASH / "+String(cost)+" CR";return false;}
 state.cash-=cost;state.expenses+=cost;state.inventory[index]++;
 message=String("STOCKED: ")+warehouseNames[index]+" / "+String(state.inventory[index])+" PCS";
 return true;
}
bool warehouseReserveForFault(uint8_t fault){
 uint8_t index=warehouseIndexForFault(fault);
 if(state.inventory[index]==0)return false;
 state.inventory[index]--;return true;
}
}
