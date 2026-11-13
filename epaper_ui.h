#pragma once
#include <esp_heap_caps.h>
#include <esp32-hal-psram.h>
// A retained full framebuffer makes a partial update safe even when a full
// cleaning cycle becomes due while an application redraws only one small panel.
class PaperDisplay : public GFXcanvas1 {
  Driver panel;
  static constexpr size_t frameBytes=800*480/8;
  uint8_t *previous=nullptr;
  uint8_t visible[800*480/8];
  bool valid=false, forced=false;
  uint16_t updates=0;
public:
  uint16_t cleanEvery=20;
  int contentTop=32,contentHeight=448;
  bool partial=true;
  bool gameUpdates=false;
  PaperDisplay():GFXcanvas1(800,480),panel(EPD_CS,EPD_DC,EPD_RST,EPD_BUSY){}
  void init(uint32_t baud,bool initial,uint16_t reset,bool pulldown){
    // GFX allocates the original canvas before setup. Release it so it cannot
    // occupy the internal heap needed by Wi-Fi tasks and DMA descriptors.
    free(buffer);buffer=nullptr;
    if(psramFound()) {
      buffer=(uint8_t*)heap_caps_malloc(frameBytes,MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
      previous=(uint8_t*)heap_caps_malloc(frameBytes,MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    }
    Serial.printf("Memory: PSRAM total=%u free=%u; internal free=%u largest=%u; screen=%s\n",
      (unsigned)ESP.getPsramSize(),(unsigned)ESP.getFreePsram(),
      (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT),
      (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT),
      buffer&&previous?"PSRAM (96000 bytes)":"ALLOCATION FAILED");
    if(!buffer||!previous){
      free(buffer);free(previous);
      buffer=(uint8_t*)heap_caps_malloc(frameBytes,MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
      previous=(uint8_t*)heap_caps_malloc(frameBytes,MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
      Serial.println("PSRAM unavailable: internal fallback; Wi-Fi memory fix NOT verified");
      if(!buffer||!previous){Serial.println("Screen allocation failed");while(true)delay(1000);}
    }
    panel.init(baud,initial,reset,pulldown);fillScreen(GxEPD_WHITE);setTextWrap(false);
  }
  // Existing application page loops now draw once into the retained canvas.
  void setFullWindow(){}
  void setPartialWindow(int,int,int,int){}
  void firstPage(){}
  void forceClean(){forced=true;}
  void hibernate(){panel.hibernate();}
  bool nextPage(){
    uint8_t *source=getBuffer();if(!source)return false;
    // Native pixels only: layout is positioned by the UI, never resampled.
    memcpy(visible,source,frameBytes);
    uint8_t *image=visible;
    int x0=100,x1=-1,y0=480,y1=-1;
    for(int y=0;y<480;y++)for(int x=0;x<100;x++){
      int i=y*100+x;
      if(!valid||image[i]!=previous[i]){x0=min(x0,x);x1=max(x1,x);y0=min(y0,y);y1=max(y1,y);}
    }
    if(x1<0&&!forced)return false;
    bool full=!valid||forced||(!partial&&!gameUpdates)||!Driver::hasPartialUpdate||(!gameUpdates&&updates+1>=cleanEvery);
    if(full){
      panel.writeImageForFullRefresh(image,0,0,800,480);
      panel.refresh(false);
      if(Driver::hasFastPartialUpdate)panel.writeImageAgain(image,0,0,800,480);
      panel.powerOff();updates=0;
    }else{
      int x=x0*8,w=(x1-x0+1)*8,h=y1-y0+1;
      panel.writeImagePart(image,x,y0,800,480,x,y0,w,h);
      panel.refresh(x,y0,w,h);
      if(Driver::hasFastPartialUpdate)panel.writeImagePartAgain(image,x,y0,800,480,x,y0,w,h);
      if(updates<65535)updates++;
    }
    memcpy(previous,image,frameBytes);valid=true;forced=false;
    return false;
  }
};
