#pragma once
namespace JDM {
const char* root="/Games/JDM_Garage_Empire";
struct Record {uint32_t magic,size,hash;State data;};
template<typename T> uint32_t checksumData(const T&s){const uint8_t*p=(const uint8_t*)&s;uint32_t h=2166136261u;for(size_t i=0;i<sizeof(s);i++)h=(h^p[i])*16777619u;return h;}
uint32_t checksum(const State&s){return checksumData(s);}
String savePath(){return String(root)+"/saves/slot"+String(slot);}
struct LegacyState {
 uint32_t version,day; int32_t cash,revenue,expenses,lastProfit;
 uint16_t reputation,completed; uint8_t stage,car,fault,stock,staff,remaining,customer;
};
struct LegacyRecord {uint32_t magic,size,hash;LegacyState data;};
bool validState(const State&s){
 return (s.version==1||s.version==2||s.version==3)&&s.stage<=5&&s.car<6&&s.fault<4&&
  s.staff<=1&&s.stock<=1&&s.customer<6&&s.queueCount<=JDM_ORDER_QUEUE_CAPACITY&&
  (s.stage<1||s.stage>2||(s.activeAcceptedDay>0&&s.activeAcceptedDay<=s.day))&&
  (s.stage!=3||((s.remaining>=1&&s.remaining<=2)||(s.remaining>=10&&s.remaining<=14)));
}
bool readRecord(String path,State &out){
 File f=SD_MMC.open(path);if(!f)return false;
 bool ok=false;
 if(f.size()==sizeof(Record)){
  Record r={};ok=f.read((uint8_t*)&r,sizeof(r))==sizeof(r);
  ok=ok&&r.magic==0x4A444D31&&r.size==sizeof(State)&&r.hash==checksum(r.data)&&validState(r.data);
  if(ok){out=r.data;for(uint8_t i=0;i<out.queueCount;i++)if(out.queue[i].car>=6||out.queue[i].fault>=4||out.queue[i].customer>=6||out.queue[i].acceptedDay>out.day)ok=false;}
 }else if(f.size()==sizeof(LegacyRecord)){
  LegacyRecord r={};ok=f.read((uint8_t*)&r,sizeof(r))==sizeof(r);
  uint32_t h=checksumData(r.data);
  ok=ok&&r.magic==0x4A444D31&&r.size==sizeof(LegacyState)&&r.hash==h&&
   (r.data.version==1||r.data.version==2)&&r.data.stage<=5&&r.data.car<6&&r.data.fault<4&&
   r.data.staff<=1&&r.data.stock<=1&&r.data.customer<6;
  if(ok){
   out=State{};out.version=3;out.day=r.data.day;out.cash=r.data.cash;out.revenue=r.data.revenue;
   out.expenses=r.data.expenses;out.lastProfit=r.data.lastProfit;out.reputation=r.data.reputation;
   out.completed=r.data.completed;out.stage=r.data.stage;out.car=r.data.car;out.fault=r.data.fault;
   out.stock=r.data.stock;out.staff=r.data.staff;out.remaining=r.data.remaining;out.customer=r.data.customer;
   if(out.stage==1||out.stage==2){uint8_t elapsed=(uint8_t)((uint8_t)out.day-out.remaining);out.activeAcceptedDay=out.day-elapsed;}
  }
 }
 f.close();return ok;
}
bool save(){
 if(!cardOK){message="NOT SAVED / TF NOT MOUNTED";return false;}
 SD_MMC.mkdir("/Games");SD_MMC.mkdir(root);SD_MMC.mkdir((String(root)+"/saves").c_str());
 String p=savePath();state.version=3;Record r={0x4A444D31,sizeof(State),checksum(state),state};
 File f=SD_MMC.open(p+".tmp","w");bool ok=f&&f.write((uint8_t*)&r,sizeof(r))==sizeof(r);f.flush();f.close();
 State check;ok=ok&&readRecord(p+".tmp",check);
 if(ok&&SD_MMC.exists(p+".sav")){State old;if(readRecord(p+".sav",old)){if(SD_MMC.exists(p+".bak"))ok=SD_MMC.remove(p+".bak");if(ok)ok=SD_MMC.rename(p+".sav",p+".bak");}else ok=SD_MMC.remove(p+".sav");}
 if(ok)ok=SD_MMC.rename(p+".tmp",p+".sav");if(ok)ok=readRecord(p+".sav",check);
 message=ok?"SAVED AND READ BACK / SLOT "+String(slot):"SAVE FAILED / CHECK TF";return ok;
}
bool load(){State recovered;if(cardOK&&(readRecord(savePath()+".sav",recovered)||readRecord(savePath()+".bak",recovered))){state=recovered;active=true;message="LOADED SLOT "+String(slot);return true;}message="NO VALID SAVE / SLOT "+String(slot);return false;}
}
