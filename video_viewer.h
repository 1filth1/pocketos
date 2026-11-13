#pragma once
// Experimental silent JPEG sequence, not an MP4 decoder or timed video player.
static String videoFrames[256];
static int videoCount=0,videoIndex=0;
static bool videoPlaying=false;
static unsigned long videoNext=0;
static String videoMessage;
static void videoDraw(){
 view=VIDEO_VIEW;beginDraw();
 header("FRAMES",String("EXPERIMENTAL B/W  ")+(videoPlaying?"PLAY":"PAUSED")+"  "+String(videoIndex+1)+" / "+String(videoCount),"ENTER: PLAY/PAUSE  LEFT/RIGHT: FRAME  ESC: FILES");
 if(!videoCount){text(videoMessage,20,160,true);}
 else if(!drawJpeg(videoFrames[videoIndex],82,330)){
  epd.fillRect(0,82,W,330,GxEPD_WHITE);
  text("FRAME READ / JPEG ERROR",20,165,true);
  text(clip(videoFrames[videoIndex],65),20,205);
  videoPlaying=false;
 }
 text("NO AUDIO / NO GUARANTEED FRAME RATE",20,438);
 epd.nextPage();
 // Delay is a minimum pause AFTER the display finishes, not an FPS target.
 videoNext=millis()+2000;
}
static void videoOpen(const String &path){
 videoPlaying=false;videoIndex=0;
 for(int i=0;i<videoCount;i++)videoFrames[i]="";
 videoCount=0;videoMessage="EMPTY SEQUENCE";
 File f=cardOK?SD_MMC.open(path.c_str()):File();
 if(!f||f.isDirectory()){if(f)f.close();videoMessage="SEQUENCE NOT FOUND";videoDraw();return;}
 String base=path.substring(0,path.lastIndexOf('/')+1);
 bool invalid=false;
 while(f.available()){
  String line;
  while(f.available()){int c=f.read();if(c=='\n')break;if(line.length()>=240){invalid=true;break;}line+=(char)c;}
  if(invalid)break;
  line.trim();if(!line.length()||line.startsWith("#"))continue;
  String lower=line;lower.toLowerCase();
  if(videoCount>=256||(!lower.endsWith(".jpg")&&!lower.endsWith(".jpeg"))){invalid=true;break;}
  videoFrames[videoCount++]=line.startsWith("/")?line:base+line;
 }
 f.close();
 if(invalid){for(int i=0;i<videoCount;i++)videoFrames[i]="";videoCount=0;videoMessage="INVALID EPV / LIMIT: 256 JPEG FRAMES";}
 videoDraw();
}
static bool videoKey(uint8_t k){
 if(view!=VIDEO_VIEW)return false;
 if(k==27||k==8){videoPlaying=false;for(int i=0;i<videoCount;i++)videoFrames[i]="";videoCount=0;manager();return true;}
 if(!videoCount)return true;
 if(k==13||k==10||k==' '){videoPlaying=!videoPlaying;if(videoPlaying&&videoIndex==videoCount-1)videoIndex=0;videoDraw();}
 else if(k==0xB4||k==0xB7){videoPlaying=false;videoIndex=constrain(videoIndex+(k==0xB7?1:-1),0,videoCount-1);videoDraw();}
 return true;
}
static void videoTick(){
 if(view!=VIDEO_VIEW||!videoPlaying||(int32_t)(millis()-videoNext)<0)return;
 if(videoIndex+1<videoCount){videoIndex++;if(videoIndex==videoCount-1)videoPlaying=false;}else videoPlaying=false;
 videoDraw();
}
