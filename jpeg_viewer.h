#pragma once
#include <JPEGDEC.h>
// Decoder and file exist only during a render; no retained JPEG framebuffer.
// JPEGDEC relies on iPos as well as the underlying File cursor.
static int32_t jpegRead(JPEGFILE *f,uint8_t *buf,int32_t len){
 if(!f||!f->fHandle||!buf||len<=0)return 0;
 int32_t remaining=f->iSize-f->iPos;
 if(remaining<=0)return 0;
 int32_t n=(int32_t)((File*)f->fHandle)->read(buf,(size_t)min(len,remaining));
 if(n>0)f->iPos+=n;
 return n>0?n:0;
}
static int32_t jpegSeek(JPEGFILE *f,int32_t pos){
 if(!f||!f->fHandle||pos<0||pos>f->iSize)return -1;
 if(!((File*)f->fHandle)->seek((uint32_t)pos))return -1;
 f->iPos=pos;
 return pos;
}
static void jpegClose(void *){} // File lifetime belongs to drawJpeg.
// Read frame metadata independently, including unsupported JPEG encodings.
struct JpegInfo { int width=0,height=0,bits=0,components=0,marker=0; String error; };
static JpegInfo jpegInfo(const String &path){
 JpegInfo a; File f=SD_MMC.open(path.c_str());
 if(!f){a.error="FILE OPEN FAILED";return a;}
 if(f.read()!=0xff||f.read()!=0xd8){a.error="NOT A JPEG: MISSING SOI SIGNATURE";f.close();return a;}
 while(f.available()){
  int c=f.read();if(c!=255)continue;do{c=f.read();}while(c==255);
  if(c<0||c==0xda||c==0xd9)break;
  if(c==0||c==1||(c>=0xd0&&c<=0xd8))continue;
  int hi=f.read(),lo=f.read();if(hi<0||lo<0)break;int len=(hi<<8)|lo;
  if(len<2||f.position()+(uint32_t)len-2>f.size())break;
  if(c>=0xc0&&c<=0xcf&&c!=0xc4&&c!=0xc8&&c!=0xcc){
   if(len<8)break;a.marker=c;a.bits=f.read();int yh=f.read(),yl=f.read(),xh=f.read(),xl=f.read();
   a.height=(yh<<8)|yl;a.width=(xh<<8)|xl;a.components=f.read();
   if(!a.width||!a.height)a.error="INVALID IMAGE DIMENSIONS";
   else if(a.bits!=8)a.error="ONLY 8-BIT JPEG IS SUPPORTED";
   else if(a.components!=1&&a.components!=3)a.error="CMYK / MULTI-COMPONENT JPEG NOT SUPPORTED";
   else if(c!=0xc0&&c!=0xc2)a.error="UNSUPPORTED JPEG ENCODING";
   f.close();return a;
  }
  if(!f.seek(f.position()+len-2))break;
 }
 a.error="DAMAGED JPEG: FRAME HEADER NOT FOUND";f.close();return a;
}
static String jpegPath;
static String jpegError;
static bool jpegPreview=false;
static String jpegDecodeError(int code){
 switch(code){
 case JPEG_UNSUPPORTED_FEATURE:return "UNSUPPORTED JPEG COLOR / ENCODING";
 case JPEG_ERROR_MEMORY:return "NOT ENOUGH DECODER MEMORY";
 case JPEG_INVALID_FILE:return "INVALID OR DAMAGED JPEG";
 case JPEG_INVALID_PARAMETER:return "INVALID JPEG PARAMETERS";
 default:return "JPEG DECODE / READ ERROR";
 }
}
static int jpgSW,jpgSH,jpgDW,jpgDH,jpgX,jpgY;
static int jpegPixels(JPEGDRAW *b){
 static const uint8_t matrix[4][4]={{0,8,2,10},{12,4,14,6},{3,11,1,9},{15,7,13,5}};
 for(int y=0;y<b->iHeight;y++)for(int x=0;x<b->iWidthUsed;x++){
  int sx=b->x+x,sy=b->y+y;
  if(sx>=jpgSW||sy>=jpgSH)continue;
  int x0=sx*jpgDW/jpgSW,x1=(sx+1)*jpgDW/jpgSW;
  int y0=sy*jpgDH/jpgSH,y1=(sy+1)*jpgDH/jpgSH;
  if(x1==x0||y1==y0)continue;
  uint16_t p=b->pPixels[y*b->iWidth+x];
  int lum=(((p>>11)&31)*255/31*77+((p>>5)&63)*255/63*150+(p&31)*255/31*29)>>8;
  for(int yy=y0;yy<y1;yy++)for(int xx=x0;xx<x1;xx++)
   epd.drawPixel(jpgX+xx,jpgY+yy,lum>matrix[yy&3][xx&3]*16+8?GxEPD_WHITE:GxEPD_BLACK);
 }
 return 1;
}
static bool drawJpeg(const String &path,int top,int height){
 jpegError="SD CARD / PATH NOT AVAILABLE";jpegPreview=false;
 if(!cardOK||!path.length())return false;
 File f=SD_MMC.open(path.c_str());if(!f||f.isDirectory()){jpegError="FILE OPEN FAILED";if(f)f.close();return false;}
 JpegInfo meta=jpegInfo(path);
 if(meta.error.length()){jpegError=meta.error;f.close();return false;}
 // Decode a bounded immutable snapshot when PSRAM is available. This avoids
 // SD cursor/callback interaction during header parsing and subsequent seeks.
 uint8_t *snapshot=nullptr;size_t bytes=f.size();
 if(psramFound()&&bytes<=4*1024*1024&&ESP.getFreePsram()>bytes+128*1024){
  snapshot=(uint8_t*)ps_malloc(bytes);
  if(snapshot&&f.read(snapshot,bytes)!=bytes){free(snapshot);f.close();jpegError="SD READ FAILED / TRUNCATED FILE";return false;}
 }
 if(!snapshot)f.seek(0);
 JPEGDEC *decoder=new(std::nothrow) JPEGDEC;
 if(!decoder){free(snapshot);jpegError="NOT ENOUGH DECODER MEMORY";f.close();return false;}
 bool ok=false; jpegError="INVALID OR UNSUPPORTED JPEG";
 if(snapshot?decoder->openRAM(snapshot,bytes,jpegPixels):decoder->open((void*)&f,(int)f.size(),jpegClose,jpegRead,jpegSeek,jpegPixels)){
  int w=decoder->getWidth(),h=decoder->getHeight();
  jpegError="IMAGE DIMENSIONS EXCEED 8192";
  // Bound decode time and arithmetic on damaged/unreasonably large files.
  if(w>0&&h>0&&w<=8192&&h<=8192){
   int scale=1;while(scale<8&&(w/scale>W*2||h/scale>height*2))scale*=2;
   // Progressive decoding supports the first (DC) scan only, at 1/8 scale.
   jpegPreview=decoder->getJPEGType()==JPEG_MODE_PROGRESSIVE;
   if(jpegPreview)scale=8;
   jpgSW=max(1,w/scale);jpgSH=max(1,h/scale);
   if(jpgSW>0&&jpgSH>0){
    jpgDW=W;jpgDH=(int)((int64_t)jpgSH*W/jpgSW);
    if(jpgDH>height){jpgDH=height;jpgDW=(int)((int64_t)jpgSW*height/jpgSH);}
    jpgDW=max(1,jpgDW);jpgDH=max(1,jpgDH);jpgX=(W-jpgDW)/2;jpgY=top+(height-jpgDH)/2;
    decoder->setPixelType(RGB565_LITTLE_ENDIAN);
    ok=decoder->decode(0,0,scale==1?0:scale)!=0;
    if(!ok)jpegError=jpegDecodeError(decoder->getLastError());
   }
  }
  decoder->close();
 }else jpegError=jpegDecodeError(decoder->getLastError());
 delete decoder;free(snapshot);f.close();return ok;
}
static void jpegViewer(){
 view=JPEG_VIEW;beginDraw();
 header("JPEG",leaf(jpegPath),"S: SET SLEEP  O: SET OFF  R: RESET BOTH  ESC: FILES");
 JpegInfo meta=jpegInfo(jpegPath);
 text(meta.width?String(meta.width)+" x "+String(meta.height)+" px / "+String(meta.bits)+" bit":"DIMENSIONS: --",20,78);
 if(!drawJpeg(jpegPath,90,350)){
  epd.fillRect(0,90,W,350,GxEPD_WHITE);
  text(clip(jpegError,65),20,160,true);
  text("Try saving as standard RGB JPEG (baseline).",30,205);
  text("Maximum image dimensions: 8192 x 8192.",30,245);
 }else if(status.startsWith("IMAGE:"))text(status,12,460,true);
 else if(jpegPreview)text("PROGRESSIVE JPEG: 1/8 RESOLUTION PREVIEW",12,460);
 epd.nextPage();
}
static bool jpegKey(uint8_t k){
 if(view!=JPEG_VIEW)return false;
 if(k==27||k==8){manager();return true;}
 if(k=='s'||k=='S'||k=='o'||k=='O'){
  // Revalidate before committing an assignment.
  if(drawJpeg(jpegPath,80,370)){
   bool sleep=k=='s'||k=='S';
   size_t n=prefs.putString(sleep?"sleepImage":"offImage",jpegPath);
   status=n?String("IMAGE: SAVED FOR ")+(sleep?"SLEEP":"POWER OFF"):"IMAGE: SAVE FAILED";
  }else status="IMAGE: READ FAILED - NOT ASSIGNED";
  jpegViewer();
 }else if(k=='r'||k=='R'){
  prefs.remove("sleepImage");prefs.remove("offImage");status="IMAGE: DEFAULT SCREENS RESTORED";jpegViewer();
 }
 return true;
}

static bool drawWebJpeg(uint8_t* data,int size){
 if(!data||size<=0)return false;
 JPEGDEC *d=new(std::nothrow) JPEGDEC;if(!d)return false;bool ok=false;
 if(d->openRAM(data,size,jpegPixels)){
 int w=d->getWidth(),h=d->getHeight();if(w>0&&h>0&&w<=8192&&h<=8192){int scale=1;while(scale<8&&(w/scale>1600||h/scale>660))scale*=2;
 if(d->getJPEGType()==JPEG_MODE_PROGRESSIVE)scale=8;
 jpgSW=max(1,w/scale);jpgSH=max(1,h/scale);if(jpgSW>0&&jpgSH>0){jpgDW=760;jpgDH=(int)((int64_t)jpgSH*760/jpgSW);if(jpgDH>330){jpgDH=330;jpgDW=(int)((int64_t)jpgSW*330/jpgSH);}jpgDW=max(1,jpgDW);jpgDH=max(1,jpgDH);jpgX=(W-jpgDW)/2;jpgY=85+(330-jpgDH)/2;d->setPixelType(RGB565_LITTLE_ENDIAN);ok=d->decode(0,0,scale==1?0:scale)!=0;}}d->close();}delete d;return ok;
}
