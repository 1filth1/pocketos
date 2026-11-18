#pragma once
namespace JDM {
static constexpr uint8_t JDM_ORDER_QUEUE_CAPACITY=2;
struct QueuedOrder { uint32_t acceptedDay=0; uint8_t car=0,fault=0,customer=0; };
struct State {
 uint32_t version=3,day=1; int32_t cash=12000,revenue=0,expenses=0,lastProfit=0;
 uint16_t reputation=10,completed=0; uint8_t stage=0,car=0,fault=0,stock=0,staff=0,remaining=0,customer=0;
 uint32_t activeAcceptedDay=0; uint8_t queueCount=0; QueuedOrder queue[JDM_ORDER_QUEUE_CAPACITY];
};
State state;
String cars[]={"SKYLINE GT-R R34","SUPRA A80","RX-7 FD3S","SILVIA S15","LANCER EVO VI","NSX NA1"};
const char* faults[]={"BRAKES","FUEL PUMP","COOLING","SUSPENSION"};
const char* symptoms[]={"LONG STOPPING DISTANCE","ENGINE HESITATES","TEMPERATURE RISES","KNOCK OVER BUMPS"};
const char* faultPartIds[]={"brake_pads","fuel_pump","radiator","coilover_kit"};
String message="NEW GAME OR LOAD A SLOT";
int repairNode=0,tool=0;
int popup=0,clientChoice=0,bolt=0,queueSelection=0;
const char* tools[]={"WRENCH","PLIERS","DRIVER","TESTER"};
const uint8_t toolOrder[4][5]={{0,2,1,2,0},{2,1,0,1,2},{0,1,2,1,0},{0,0,1,0,0}};
const char* repairSteps[]={"REMOVE FASTENERS","DISCONNECT / REMOVE","FIT NEW COMPONENT","RECONNECT / ASSEMBLE","TIGHTEN / FINISH"};
int page=0,row=0,slot=1;bool active=false,confirmNew=false;
}
