#pragma once
namespace JDM {
const char* root="/Games/JDM_Garage_Empire";
struct Record {uint32_t magic,size,hash;State data;};
uint32_t checksum(const State&s){const uint8_t*p=(const uint8_t*)&s;uint32_t h=2166136261u;for(size_t i=0;i<sizeof(s);i++)h=(h^p[i])*16777619u;return h;}
String savePath(){return String(root)+"/saves/slot"+String(slot);}
bool readRecord(String path,State &out){
 File f=SD_MMC.open(path);Record r={};
 bool ok=f&&f.size()==sizeof(r)&&f.read((uint8_t*)&r,sizeof(r))==sizeof(r);f.close();
 ok=ok&&r.magic==0x4A444D31&&r.size==sizeof(State)&&r.hash==checksum(r.data)&&
   (r.data.version==1||r.data.version==2)&&r.data.stage<=5&&r.data.car<6&&r.data.fault<4&&
   r.data.staff<=1&&r.data.stock<=1&&
   (r.data.stage!=3||((r.data.remaining>=1&&r.data.remaining<=2)||(r.data.remaining>=10&&r.data.remaining<=14)));
 if(ok){out=r.data;if(out.version==1){out.version=2;out.customer=0;}if(out.customer>=6)out.customer=0;}
 return ok;
}
bool save(){
 if(!cardOK){message="NOT SAVED / TF NOT MOUNTED";return false;}
 SD_MMC.mkdir("/Games");SD_MMC.mkdir(root);SD_MMC.mkdir((String(root)+"/saves").c_str());
 String p=savePath();state.version=2;Record r={0x4A444D31,sizeof(State),checksum(state),state};
 File f=SD_MMC.open(p+".tmp","w");bool ok=f&&f.write((uint8_t*)&r,sizeof(r))==sizeof(r);f.flush();f.close();
 State check;ok=ok&&readRecord(p+".tmp",check);
 if(ok&&SD_MMC.exists(p+".sav")){State old;if(readRecord(p+".sav",old)){if(SD_MMC.exists(p+".bak"))ok=SD_MMC.remove(p+".bak");if(ok)ok=SD_MMC.rename(p+".sav",p+".bak");}else ok=SD_MMC.remove(p+".sav");}
 if(ok)ok=SD_MMC.rename(p+".tmp",p+".sav");if(ok)ok=readRecord(p+".sav",check);
 message=ok?"SAVED AND READ BACK / SLOT "+String(slot):"SAVE FAILED / CHECK TF";return ok;
}
bool load(){State recovered;if(cardOK&&(readRecord(savePath()+".sav",recovered)||readRecord(savePath()+".bak",recovered))){state=recovered;active=true;message="LOADED SLOT "+String(slot);return true;}message="NO VALID SAVE / SLOT "+String(slot);return false;}
}
