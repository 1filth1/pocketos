#pragma once
#include "CarSprites.h"
namespace JDM {
// Fixed-size raw bitmap: 240x104, 30 bytes per row, 1=black.
uint8_t spriteCache[3120];
int cachedModel=-1;
bool cachedFromSD=false;
String assetStatus="BUILT-IN ART";
String spritePath(int model){return String(root)+"/assets/cars/car_shaded_v2_"+String(model)+".mono";}
void prepareAssets(){
 cachedModel=-1;cachedFromSD=false;
 if(!cardOK){assetStatus="BUILT-IN / TF NOT MOUNTED";return;}
 SD_MMC.mkdir("/Games");SD_MMC.mkdir(root);
 SD_MMC.mkdir((String(root)+"/assets").c_str());\n if(!SD_MMC.mkdir((String(root)+"/assets/cars").c_str())&&!SD_MMC.exists(String(root)+"/assets/cars")){assetStatus="BUILT-IN / ASSET FOLDER FAILED";return;}
 bool ok=true;
 for(int i=0;i<10;i++){
  String path=spritePath(i);
  if(SD_MMC.exists(path))continue; // Never overwrite user artwork.
  File f=SD_MMC.open(path,"w");
  bool wrote=f&&f.write(carSprites[i],sizeof(spriteCache))==sizeof(spriteCache);
  f.flush();f.close();
  File check=SD_MMC.open(path);uint8_t buf[64];size_t pos=0;
  bool valid=wrote&&check&&check.size()==sizeof(spriteCache);
  while(valid&&pos<sizeof(spriteCache)){
   size_t n=min(sizeof(buf),sizeof(spriteCache)-pos);
   if(check.read(buf,n)!=n){valid=false;break;}
   for(size_t k=0;k<n;k++)if(buf[k]!=pgm_read_byte(&carSprites[i][pos+k]))valid=false;
   pos+=n;
  }
  check.close();if(!valid){SD_MMC.remove(path);ok=false;}
 }
 assetStatus=ok?"TF ASSETS READY":"BUILT-IN / ASSET WRITE FAILED";
}
void drawCarSprite(int model,int x,int y){
 model=constrain(model,0,9);
 if(cachedModel!=model){
  cachedModel=model;cachedFromSD=false;
  if(cardOK){File f=SD_MMC.open(spritePath(model));cachedFromSD=f&&f.size()==sizeof(spriteCache)&&f.read(spriteCache,sizeof(spriteCache))==sizeof(spriteCache);f.close();}
  assetStatus=cachedFromSD?"CAR ART: TF CARD":"CAR ART: BUILT-IN FALLBACK";
 }
 epd.drawBitmap(x,y,cachedFromSD?spriteCache:carSprites[model],240,104,GxEPD_BLACK,GxEPD_WHITE);
}
}
