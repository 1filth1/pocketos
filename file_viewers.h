#pragma once
// Read-only, bounded viewers. No full-file allocation and no source writes.
static String rawPath;
static uint32_t rawOffset=0;
static uint16_t fv16(const uint8_t*p){return uint16_t(p[0])|(uint16_t(p[1])<<8);}
static uint32_t fv32(const uint8_t*p){return uint32_t(p[0])|(uint32_t(p[1])<<8)|(uint32_t(p[2])<<16)|(uint32_t(p[3])<<24);}
static bool bmpDraw(const String &path){
 File f=SD_MMC.open(path.c_str());uint8_t h[54];
 if(!f||f.read(h,54)!=54||h[0]!='B'||h[1]!='M'){if(f)f.close();return false;}
 uint32_t off=fv32(h+10),dib=fv32(h+14);int32_t w=(int32_t)fv32(h+18),sh=(int32_t)fv32(h+22);
 unsigned bits=fv16(h+28);uint32_t compression=fv32(h+30);
 if(dib<40||w<=0||w>8192||!sh||sh==INT32_MIN||abs(sh)>8192||fv16(h+26)!=1||(bits!=24&&bits!=32)||compression||off<14ull+dib){f.close();return false;}
 int height=abs(sh);uint32_t stride=((uint32_t(w)*bits+31)/32)*4;
 if(uint64_t(off)+uint64_t(stride)*height>f.size()){f.close();return false;}
 int dw=760,dh=int(int64_t(height)*dw/w);if(dh>350){dh=350;dw=int(int64_t(w)*dh/height);}dw=max(1,dw);dh=max(1,dh);
 int left=(W-dw)/2,top=88+(350-dh)/2;std::vector<uint8_t> row(stride);
 // A single bounded source row, allocated only during BMP rendering.
 for(int y=0;y<dh;y++){
  int sy=int(int64_t(y)*height/dh);if(sh>0)sy=height-1-sy;
  if(!f.seek(off+uint32_t(sy)*stride)||f.read(row.data(),stride)!=stride){f.close();return false;}
  for(int x=0;x<dw;x++){unsigned sx=uint64_t(x)*w/dw;uint8_t*p=row.data()+sx*(bits/8);int lum=(29*p[0]+150*p[1]+77*p[2])>>8;
   static const uint8_t d[4][4]={{0,8,2,10},{12,4,14,6},{3,11,1,9},{15,7,13,5}};
   if(lum<=d[y&3][x&3]*16+8)epd.drawPixel(left+x,top+y,GxEPD_BLACK);
  }delay(1);
 }f.close();return true;
}
static void rawDraw(){
 view=RAW_VIEW;File f=SD_MMC.open(rawPath.c_str());uint32_t size=f?f.size():0;
 if(size)rawOffset=min(rawOffset,((size-1)/128)*128);else rawOffset=0;
 beginDraw();do{
 header("",leaf(rawPath),"UP/DOWN: PAGE   ESC/BACKSPACE: FILES");
 text("BYTES / HEX - NOT A DOCUMENT DECODER",18,108,true);
 text(String(size)+" bytes   OFFSET "+String(rawOffset),18,140);
 if(!f)text("CANNOT READ FILE",18,190,true);
 else {f.seek(rawOffset);for(int r=0;r<8&&f.available();r++){
  char addr[12];snprintf(addr,sizeof(addr),"%08lX",(unsigned long)f.position());String line=String(addr)+"  ",ascii;
  for(int i=0;i<16;i++){if(f.available()){int b=f.read();char hex[4];snprintf(hex,sizeof(hex),"%02X ",b);line+=hex;ascii+=(b>=32&&b<127)?char(b):'.';}else{line+="   ";ascii+=' ';}}
  text(line+" "+ascii,18,184+r*32);
 }}
 }while(epd.nextPage());if(f)f.close();
}
static void bmpViewer(){view=BMP_VIEW;beginDraw();do{header("",leaf(rawPath),"BMP 24/32 BIT UNCOMPRESSED   ESC/BACKSPACE: FILES");if(!bmpDraw(rawPath)){epd.fillRect(0,80,W,370,GxEPD_WHITE);text("UNSUPPORTED OR DAMAGED BMP",18,160,true);text("Use uncompressed 24-bit or 32-bit BMP, up to 8192 px.",18,205);}}while(epd.nextPage());}
static bool fileViewerKey(uint8_t k){
 if(view!=RAW_VIEW&&view!=BMP_VIEW)return false;
 if(k==27||k==8){manager();return true;}
 if(view==RAW_VIEW){if(k==0xB5&&rawOffset){rawOffset-=128;rawDraw();}else if(k==0xB6){File f=SD_MMC.open(rawPath.c_str());bool more=f&&uint64_t(rawOffset)+128<f.size();if(f)f.close();if(more){rawOffset+=128;rawDraw();}}}return true;
}
