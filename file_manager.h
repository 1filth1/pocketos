// Bounded previews and streaming copies: never load a whole file into RAM.
String fmClipboard, fmInput;
int fmMode=0; // 1 rename, 2 mkdir, 3 delete confirmation, 4 file details
String fmJoin(const String &name){return folder=="/"?"/"+name:folder+"/"+name;}
bool fmText(String name){return filePlainText(name); }
void fmRefresh(const String &message){loadFolder(folder);status=message;appNotify("FILES",message);}
void fileManagerDraw(){
 menuActive=false;view=FILES;
 if(!cardOK){count=0;status="TF CARD NOT MOUNTED";}
 selected=constrain(selected,0,max(0,count-1));first=(selected/6)*6;
 String preview[5];String kind="--",size="--",name="NO SELECTION";
 if(cardOK&&count){auto &e=files[selected];name=e.name;kind=e.dir?"DIRECTORY":"FILE";size=e.dir?"--":String((uint32_t)e.size)+" bytes";
  if(!e.dir&&fmText(name)){File f=SD_MMC.open(e.path.c_str(),FILE_READ);if(f){int row=0,col=0,read=0;while(f.available()&&row<5&&read++<512){char c=f.read();if(c=='\r')continue;if(c=='\n'){row++;col=0;continue;}preview[row]+=(c>=32&&c<=126)?c:'.';if(++col>=28){row++;col=0;}}f.close();}else preview[0]="CANNOT READ FILE";}
  else preview[0]=e.dir?"ENTER: OPEN FOLDER":"ENTER: OPEN VIEWER";
 }
 beginDraw();do{
 header("", "FILES  "+clip(folder,60),"B BOOKS  C COPY  V PASTE  I INFO  R RENAME  M MKDIR  D DELETE");
 epd.drawRect(12,84,386,332,GxEPD_BLACK);epd.drawRect(408,84,380,332,GxEPD_BLACK);
 epd.fillRect(12,84,386,32,GxEPD_BLACK);inv("FILES / "+String(count),24,107);
 epd.fillRect(408,84,380,32,GxEPD_BLACK);inv("PREVIEW / INFO",420,107);
 for(int r=0;r<6&&first+r<count;r++){int i=first+r,y=122+r*47;bool on=i==selected;epd.fillRect(20,y,370,43,on?GxEPD_BLACK:GxEPD_WHITE);String label=String(files[i].dir?"[D] ":"    ")+clip(files[i].name,25);if(on)inv(label,26,y+28);else text(label,26,y+28,true);}
 if(!count)text(cardOK?"EMPTY FOLDER":"TF NOT MOUNTED",24,155,true);
 text(clip(name,28),420,146,true);text(kind+"  "+size,420,180);epd.drawLine(420,194,776,194,GxEPD_BLACK);
 for(int i=0;i<5;i++)text(preview[i],420,224+i*32);
 text(clip(status,69),16,440);
 if(fmMode==4&&count){
  auto &e=files[selected];epd.fillRect(40,100,720,310,GxEPD_WHITE);epd.drawRect(40,100,720,310,GxEPD_BLACK);
  text("FILE INFORMATION",60,138,true);text(clip(e.name,62),60,178,true);
  text(clip(e.path,62),60,215);text(e.dir?"DIRECTORY":String((uint32_t)e.size)+" bytes",60,252);
  String n=e.name;n.toLowerCase();
  if(n.endsWith(".jpg")||n.endsWith(".jpeg")||n.endsWith(".jpe")){
   JpegInfo j=jpegInfo(e.path);text(j.width?String(j.width)+" x "+String(j.height)+" px / "+String(j.bits)+" bit":"DIMENSIONS: --",60,290);
   text(clip(j.error.length()?j.error:j.marker==0xc2?String("PROGRESSIVE / 1:8 PREVIEW"):String("BASELINE JPEG"),62),60,327);
  }else text("MODIFIED TIME: NOT AVAILABLE",60,290);
  text("ESC / ENTER: CLOSE",60,382);
 }
 else if(fmMode){epd.fillRect(70,155,660,180,GxEPD_WHITE);epd.drawRect(70,155,660,180,GxEPD_BLACK);
 text(fmMode==1?"RENAME":fmMode==2?"MAKE DIRECTORY":"DELETE SELECTED ITEM?",88,190,true);
 text(clip(fmMode==3?name:fmInput,48),88,236,true);
 text(fmMode==3?"ENTER / Y: DELETE  ESC: CANCEL":"ENTER: SAVE  ESC: CANCEL",88,279);text(fmMode==3?"ALL CONTENTS DELETED. CANNOT UNDO.":"Existing files are never overwritten.",88,313);
 }
 }while(epd.nextPage());
}
void fmPaste(){
 if(!fmClipboard.length()){status="COPY A FILE FIRST";return;}
 String dest=fmJoin(leaf(fmClipboard));
 if(SD_MMC.exists(dest.c_str())){status="NAME EXISTS - RENAME SOURCE OR CHANGE FOLDER";return;}
 File in=SD_MMC.open(fmClipboard.c_str(),FILE_READ);if(!in||in.isDirectory()){if(in)in.close();status="SOURCE NOT AVAILABLE / FILES ONLY";return;}
 String tmp=dest+".copying";if(SD_MMC.exists(tmp.c_str())){in.close();status="TEMP FILE EXISTS - REMOVE IT FIRST";return;}
 File out=SD_MMC.open(tmp.c_str(),FILE_WRITE);if(!out){in.close();status="CANNOT CREATE COPY";return;}
 uint8_t buffer[2048];size_t total=0,expected=in.size();bool ok=true;
 while(in.available()){int n=in.read(buffer,sizeof(buffer));if(n<=0||out.write(buffer,n)!=(size_t)n){ok=false;break;}total+=n;delay(1);}
 out.flush();out.close();in.close();File check=SD_MMC.open(tmp.c_str());ok=ok&&total==expected&&check&&check.size()==expected;if(check)check.close();
 if(ok)ok=SD_MMC.rename(tmp.c_str(),dest.c_str());if(!ok)SD_MMC.remove(tmp.c_str());fmRefresh(ok?"COPY SAVED TO TF CARD":"COPY FAILED - SOURCE UNCHANGED");
}
// Walk one directory at a time: no recursion or MAX_FILES listing limit.
// Reopen after each removal so directory iteration cannot skip entries.
bool fmRemoveTree(const String &root){
 if(!root.startsWith("/")||root=="/"||root.endsWith("/")||root.indexOf("/../")>=0||root.endsWith("/.."))return false;
 String current=root;
 while(true){
  File node=SD_MMC.open(current.c_str(),FILE_READ);
  if(!node)return false;
  if(!node.isDirectory()){
   node.close();
   if(!SD_MMC.remove(current.c_str())||SD_MMC.exists(current.c_str()))return false;
   if(current==root)return true;
   current=parent(current);delay(1);continue;
  }
  File child=node.openNextFile();
  if(child){
   String name=leaf(String(child.name()));child.close();node.close();
   if(!name.length()||name=="."||name==".."||name.indexOf('\\')>=0)return false;
   current+="/"+name;
  }else{
   node.close();
   if(!SD_MMC.rmdir(current.c_str())||SD_MMC.exists(current.c_str()))return false;
   if(current==root)return true;
   current=parent(current);
  }
  delay(1);
 }
}
void fileManagerKey(uint8_t k){
 if(!fmMode&&(k=='b'||k=='B')){bookLibrary();return;}
 if(!cardOK){if(k==27||k==8)showMenu(300,HOME);return;}
 if(fmMode){
  if(fmMode==4){if(k==27||k==8||k==13||k==10){fmMode=0;fileManagerDraw();}return;}
  if(k==27){fmMode=0;fileManagerDraw();return;}
  if(fmMode==3){if(k=='y'||k=='Y'||k==13||k==10){String target=count?files[selected].path:String();bool ok=count&&fmRemoveTree(target);ok=ok&&!SD_MMC.exists(target.c_str());fmMode=0;fmRefresh(ok?"DELETED":"DELETE INCOMPLETE - CHECK CARD / CONTENTS");}else if(k=='n'||k=='N'||k==8)fmMode=0;else return;}
  else if(k==13||k==10){fmInput.trim();if(!fmInput.length()||fmInput=="."||fmInput==".."||fmInput.indexOf('/')>=0||fmInput.indexOf('\\')>=0||fmInput.indexOf(':')>=0||fmInput.indexOf('*')>=0||fmInput.indexOf('?')>=0||fmInput.indexOf('\"')>=0||fmInput.indexOf('<')>=0||fmInput.indexOf('>')>=0||fmInput.indexOf('|')>=0||fmInput.endsWith(".")){status="INVALID NAME";fmMode=0;}else{String dest=fmJoin(fmInput);if(SD_MMC.exists(dest.c_str())){status="NAME ALREADY EXISTS";fmMode=0;}else{bool ok=fmMode==2?SD_MMC.mkdir(dest.c_str()):SD_MMC.rename(files[selected].path.c_str(),dest.c_str());fmMode=0;fmRefresh(ok?"SAVED TO TF CARD":"OPERATION FAILED");}}}
  else if(k==8){if(fmInput.length())fmInput.remove(fmInput.length()-1);else return;}
  else if(k>=32&&k<=126&&fmInput.length()<80)fmInput+=(char)k;else return;
  fileManagerDraw();return;
 }
 if(k==0xB5||k==0xB6){int next=constrain(selected+(k==0xB5?-1:1),0,max(0,count-1));if(next==selected)return;selected=next;}
 else if(k==27||k==8){if(folder=="/"){showMenu(300,HOME);return;}loadFolder(parent(folder));}
 else if(k==13||k==10){openFile();return;}
 else if(k=='c'||k=='C'){if(!count)return;if(files[selected].dir)status="COPY: SELECT A FILE (NOT A FOLDER)";else{fmClipboard=files[selected].path;status="COPIED: "+files[selected].name+" / V: PASTE";}}
 else if(k=='v'||k=='V')fmPaste();
 else if(k=='m'||k=='M'){fmInput="";fmMode=2;}
 else if((k=='i'||k=='I')&&count)fmMode=4;
 else if((k=='r'||k=='R')&&count){fmInput=files[selected].name;fmMode=1;}
 else if((k=='d'||k=='D')&&count)fmMode=3;
 else if(k=='n'||k=='N'){typed="";noteNameView();return;}
 else if(k=='t'||k=='T'){infoName="TERMINAL";typed="";appResult="";infoView();return;}
 else return;
 fileManagerDraw();
}
