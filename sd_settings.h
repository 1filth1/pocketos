#pragma once
#include <Preferences.h>
#include <SD_MMC.h>
#include <vector>
// All new writes go to SD. NVS is opened read-only for legacy migration.
class SdSettings {
 Preferences legacy; bool legacyOK=false,readonly=false; String dir;
 struct Header {uint32_t magic,size,hash;};
 uint32_t hash(const uint8_t*p,size_t n){uint32_t h=2166136261u;while(n--)h=(h^*p++)*16777619u;return h;}
 String path(const char*k){return dir+"/"+k;}
 bool readFile(String p,std::vector<uint8_t>&v){File f=SD_MMC.open(p);Header h;bool ok=f&&f.read((uint8_t*)&h,sizeof(h))==sizeof(h)&&h.magic==0x53445331&&h.size<=65536&&f.size()==sizeof(h)+h.size;if(ok){v.resize(h.size);ok=f.read(v.data(),h.size)==h.size&&hash(v.data(),v.size())==h.hash;}f.close();return ok;}
 bool read(const char*k,std::vector<uint8_t>&v){String p=path(k);return readFile(p+".bin",v)||readFile(p+".bak",v);}
 bool exists(const char*k){String p=path(k);return SD_MMC.exists((p+".bin").c_str())||SD_MMC.exists((p+".bak").c_str());}
 template<class T>T get(const char*k,T d){std::vector<uint8_t>v;if(read(k,v)&&v.size()==sizeof(T)){T x;memcpy(&x,v.data(),sizeof(x));return x;}return d;}
 public:
 bool begin(const char*n,bool ro=false){dir=String("/settings/")+n;readonly=ro;legacyOK=legacy.begin(n,true);if(SD_MMC.cardType()!=CARD_NONE){SD_MMC.mkdir("/settings");SD_MMC.mkdir(dir.c_str());return true;}return legacyOK;}
 bool remove(const char*k){uint8_t empty=0;return !readonly&&SD_MMC.cardType()!=CARD_NONE&&(putBytes(k,&empty,0)==0)&&exists(k);}
 void end(){if(legacyOK)legacy.end();legacyOK=false;}
 bool isKey(const char*k){return exists(k)||(legacyOK&&legacy.isKey(k));}
 size_t putBytes(const char*k,const void*p,size_t n){if(readonly||SD_MMC.cardType()==CARD_NONE)return 0;std::vector<uint8_t>v;if(read(k,v)&&v.size()==n&&!memcmp(v.data(),p,n))return n;String b=path(k);Header h={0x53445331,(uint32_t)n,hash((const uint8_t*)p,n)};File f=SD_MMC.open(b+".tmp","w");bool ok=f&&f.write((uint8_t*)&h,sizeof(h))==sizeof(h)&&f.write((const uint8_t*)p,n)==n;f.flush();f.close();ok=ok&&readFile(b+".tmp",v)&&v.size()==n&&!memcmp(v.data(),p,n);if(ok&&SD_MMC.exists((b+".bin").c_str())){std::vector<uint8_t>old;if(readFile(b+".bin",old)){if(SD_MMC.exists((b+".bak").c_str()))ok=SD_MMC.remove((b+".bak").c_str());if(ok)ok=SD_MMC.rename((b+".bin").c_str(),(b+".bak").c_str());}else ok=SD_MMC.remove((b+".bin").c_str());}if(ok)ok=SD_MMC.rename((b+".tmp").c_str(),(b+".bin").c_str());if(!ok)Serial.printf("SD settings write failed: %s\n",b.c_str());return ok?n:0;}
 size_t getBytesLength(const char*k){std::vector<uint8_t>v;if(read(k,v))return v.size();return !exists(k)&&legacyOK?legacy.getBytesLength(k):0;}
 size_t getBytes(const char*k,void*p,size_t n){std::vector<uint8_t>v;if(read(k,v)){if(v.size()>n)return 0;memcpy(p,v.data(),v.size());return v.size();}if(exists(k)||!legacyOK)return 0;size_t got=legacy.getBytes(k,p,n);if(got){bool ro=readonly;readonly=false;putBytes(k,p,got);readonly=ro;}return got;}
 String getString(const char*k,const String&d=""){std::vector<uint8_t>v;if(read(k,v)){String s;for(auto c:v)s+=(char)c;return s;}if(exists(k)||!legacyOK||!legacy.isKey(k))return d;String s=legacy.getString(k,d);bool ro=readonly;readonly=false;putString(k,s);readonly=ro;return s;}
 size_t putString(const char*k,const String&s){return putBytes(k,s.c_str(),s.length());}
#define SD_VALUE(Name,Type) \
 Type get##Name(const char*k,Type d=0){if(!exists(k)&&legacyOK&&legacy.isKey(k)){Type x=legacy.get##Name(k,d);bool ro=readonly;readonly=false;putBytes(k,&x,sizeof(x));readonly=ro;return x;}return get<Type>(k,d);} \
 size_t put##Name(const char*k,Type x){return putBytes(k,&x,sizeof(x));}
 SD_VALUE(Bool,bool)
 SD_VALUE(UChar,uint8_t)
 SD_VALUE(UShort,uint16_t)
 SD_VALUE(UInt,uint32_t)
 SD_VALUE(Int,int32_t)
#undef SD_VALUE
};
