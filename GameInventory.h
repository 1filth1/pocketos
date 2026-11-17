#pragma once
namespace JDM {
static const uint8_t JDM_STARTING_STOCK[JDM_PART_COUNT] = {4,2,3,5,2,3,1,3,1,0};
static const char *jdmFallbackPartNames[JDM_PART_COUNT] = {
 "Street brake pads","Vented brake rotors","High-flow fuel pump","Performance fuel filter",
 "Aluminium radiator","Water pump kit","Street coilover kit","Poly bushing set",
 "Bolt-on turbo kit","Forged engine internals"
};
static const char *jdmPartIds[JDM_PART_COUNT] = {
 "brake_pads","brake_rotors","fuel_pump","fuel_filter","radiator",
 "water_pump","coilover_kit","bushing_set","turbo_kit","forged_internals"
};
static int inventoryPartIndexForFault(uint8_t fault) {
 static const uint8_t map[4]={0,2,4,6};
 return map[constrain((int)fault,0,3)];
}
static bool inventoryReadPart(uint8_t index, String &name, int &cost, int &quality, int &stock) {
 index=constrain((int)index,0,(int)JDM_PART_COUNT-1);
 name=jdmFallbackPartNames[index]; cost=100; quality=1; stock=JDM_STARTING_STOCK[index];
 JsonDocument doc;
 if (!campaignRead("parts.json",doc)) return false;
 uint8_t i=0;
 for(JsonObjectConst item:doc["parts"].as<JsonArrayConst>()) {
  if(i++!=index) continue;
  if(String(item["id"]|"")!=jdmPartIds[index]) return false;
  name=item["name"]|jdmFallbackPartNames[index];
  cost=item["cost"]|cost; quality=item["quality"]|quality; stock=item["stock"]|stock;
  return cost>0 && quality>=1 && quality<=5 && stock>=0 && stock<=50;
 }
 return false;
}
void inventoryInit() {
 for(uint8_t i=0;i<JDM_PART_COUNT;i++) {
  String name; int cost=100,quality=1,stock=JDM_STARTING_STOCK[i];
  inventoryReadPart(i,name,cost,quality,stock);
  state.inventory[i]=constrain(stock,0,50);
  state.inbound[i]=0; state.inboundDay[i]=0;
 }
 state.stock=0;
}
int inventoryCost(uint8_t index) {
 String name; int cost=100,quality=1,stock=0;
 inventoryReadPart(index,name,cost,quality,stock);
 int discount=campaignEventEffect(state.day,"parts_cost_percent",0);
 return max(1,cost*(100-constrain(discount,-50,90))/100);
}
String inventoryDeliverDue() {
 String delivered="";
 for(uint8_t i=0;i<JDM_PART_COUNT;i++) {
  if(state.inbound[i] && state.inboundDay[i] && state.inboundDay[i]<=state.day) {
   uint8_t room=50-state.inventory[i];
   uint8_t amount=min(room,state.inbound[i]);
   state.inventory[i]+=amount;
   state.inbound[i]=0; state.inboundDay[i]=0;
   String name; int cost=100,quality=1,stock=0; inventoryReadPart(i,name,cost,quality,stock);
   if(delivered.length()) delivered+=" / ";
   delivered+="DELIVERED "+name+" x"+String(amount);
  }
 }
 return delivered;
}
bool inventoryPurchase(uint8_t index) {
 if(index>=JDM_PART_COUNT){message="INVALID PART";return false;}
 if((uint16_t)state.inventory[index]+state.inbound[index]>=50){message="STOCK LIMIT REACHED";return false;}
 int cost=inventoryCost(index);
 if(state.cash<cost){message="NOT ENOUGH CASH / "+String(cost)+" CR";return false;}
 state.cash-=cost; state.expenses+=cost; state.inbound[index]++;
 uint32_t eta=state.day+1+(index%3);
 if(state.inboundDay[index] && state.inboundDay[index]>eta) eta=state.inboundDay[index];
 state.inboundDay[index]=eta;
 String name; int base=100,quality=1,stock=0; inventoryReadPart(index,name,base,quality,stock);
 message="ORDERED "+name+" / ARRIVES DAY "+String(eta);
 return true;
}
void inventoryReturnReservedPart() {
 if(state.stock) {
  uint8_t index=inventoryPartIndexForFault(state.fault);
  if(state.inventory[index]<50) state.inventory[index]++;
  state.stock=0;
 }
}
} // namespace JDM
