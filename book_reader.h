#pragma once
#include <U8g2_for_Adafruit_GFX.h>
#include "book_formats.h"
// Streaming UTF-8 TXT / extracted FB2 and EPUB reader. Only the visible page is kept in RAM.
U8G2_FOR_ADAFRUIT_GFX bookFont;
String bookPath,bookMessage,bookTitles[80];
uint32_t bookOffset=0,bookNext=0,bookSize=0,bookMark=0;
int bookCount=0,bookChoice=0;
bool bookLarge=false,bookFromFiles=false,bookLimited=false;
SdSettings bookPrefs;
String bookKey(){uint32_t h=2166136261u;for(unsigned i=0;i<bookPath.length();i++)h=(h^(uint8_t)bookPath[i])*16777619u;return String(h,16);}
struct BookState {uint32_t size,offset,mark,large;};
bool bookSave(){BookState s={bookSize,bookOffset,bookMark,bookLarge};String key=bookKey();if(bookPrefs.putString((key+"p").c_str(),bookPath)!=bookPath.length())return false;if(bookPrefs.putBytes(key.c_str(),&s,sizeof(s))!=sizeof(s))return false;BookState check={};return bookPrefs.getBytes(key.c_str(),&check,sizeof(check))==sizeof(check)&&!memcmp(&s,&check,sizeof(s));}
void bookDraw(){
 File f=SD_MMC.open(bookDataPath.c_str(),FILE_READ);String lines[20];int rows=bookLarge?16:20,cols=bookLarge?75:94;
 if(!f||f.isDirectory()){if(f)f.close();bookMessage="CANNOT READ BOOK";return;}
 f.seek(bookOffset);if(bookOffset==0&&f.size()>=3){uint8_t bom[3];f.read(bom,3);if(!(bom[0]==0xef&&bom[1]==0xbb&&bom[2]==0xbf))f.seek(0);}
 int r=0,col=0;unsigned budget=0;
 while(f.available()&&r<rows&&budget++<12000){uint32_t at=f.position();int c=f.read();if(c=='\r')continue;if(c=='\n'){r++;col=0;continue;}if(c<32&&c!='\t')continue;
 if(col>=cols){r++;col=0;if(r>=rows){f.seek(at);break;}}
 if(c=='\t'){lines[r]+=' ';col++;continue;}
 lines[r]+=(char)c;int extra=(c&0xe0)==0xc0?1:(c&0xf0)==0xe0?2:(c&0xf8)==0xf0?3:0;
 while(extra--&&f.available())lines[r]+=(char)f.read();col++;
 }
 bookNext=f.position();f.close();view=BOOK_READ;
 bookMessage=bookSave()?"SAVED":"SAVE FAILED / CHECK TF";
 beginDraw();do{
  epd.fillScreen(GxEPD_WHITE);drawTopStatus(stat());
  bookFont.begin(epd);bookFont.setFontMode(1);bookFont.setForegroundColor(GxEPD_BLACK);
  bookFont.setFont(u8g2_font_unifont_t_cyrillic);bookFont.setCursor(16,55);bookFont.print(leaf(bookPath));
  epd.drawLine(16,68,784,68,GxEPD_BLACK);
  bookFont.setFont(bookLarge?u8g2_font_10x20_t_cyrillic:u8g2_font_unifont_t_cyrillic);
  for(int i=0;i<rows;i++){bookFont.setCursor(24,96+i*(bookLarge?20:16));bookFont.print(lines[i]);}
  text(String(bookSize?(uint64_t)bookOffset*100/bookSize:0)+"%  "+bookMessage+(bookNext>=bookSize?" / END":""),16,445);
  text(String("F FONT: ")+(bookLarge?"10x20":"8x16")+"  E: "+bookEncodingNames[bookEncoding]+"  M MARK  ESC BACK",16,473);
 }while(epd.nextPage());
}
void bookOpen(const String &path,bool fromFiles){
 bookPrefs.begin("books",false);bookPath=path;bookFromFiles=fromFiles;
 bookEncoding=bookPrefs.getUChar((bookKey()+"e").c_str(),0);if(bookEncoding>6)bookEncoding=0;
 if(!bookPrepare(path)){status=bookFormatError;return;}
 File f=SD_MMC.open(bookDataPath.c_str());if(!f||f.isDirectory()){if(f)f.close();status="CANNOT OPEN BOOK";return;}bookSize=f.size();f.close();bookOffset=bookMark=0;bookLarge=false;
 BookState s={};String key=bookKey();if(bookPrefs.getString((key+"p").c_str(),"")==path&&bookPrefs.getBytes(key.c_str(),&s,sizeof(s))==sizeof(s)&&s.size==bookSize){bookOffset=s.offset<bookSize?s.offset:0;bookMark=s.mark<bookSize?s.mark:0;bookLarge=s.large!=0;}
 bookDraw();
}
void bookLibraryDraw(){
 view=BOOK_LIBRARY;beginDraw();do{header("","BOOKS / TXT FB2 EPUB","UP/DOWN SELECT   ENTER READ   ESC BACK");
 bookFont.begin(epd);bookFont.setFontMode(1);bookFont.setForegroundColor(GxEPD_BLACK);bookFont.setFont(u8g2_font_unifont_t_cyrillic);
 for(int i=(bookChoice/8)*8;i<min(bookCount,(bookChoice/8)*8+8);i++){int y=107+(i%8)*38;if(i==bookChoice)epd.drawRect(12,y-23,776,32,GxEPD_BLACK);bookFont.setCursor(24,y);bookFont.print(leaf(bookTitles[i]));}
 if(!bookCount){text(cardOK?"Put UTF-8 TXT / FB2 / DRM-free EPUB in /books":"TF CARD NOT MOUNTED",20,130);text("Or select a book in FILES and press Enter.",20,180);}
 if(bookFormatError.length())text(clip(bookFormatError,78),20,400);
 text(bookLimited?"LIMIT: FIRST 80 BOOKS / USE FILES FOR OTHERS":String(bookCount)+" BOOKS / /books",20,435);
 }while(epd.nextPage());
}
void bookLibrary(){bookFormatError="";bookCount=bookChoice=0;bookLimited=false;if(cardOK){if(!SD_MMC.exists("/books"))SD_MMC.mkdir("/books");File d=SD_MMC.open("/books");if(d&&d.isDirectory()){File f=d.openNextFile();while(f){String n=leaf(String(f.name())),l=n;l.toLowerCase();if(!f.isDirectory()&&bookSupported(l)){if(bookCount==80){bookLimited=true;f.close();break;}bookTitles[bookCount++]="/books/"+n;}f.close();f=d.openNextFile();}d.close();}}bookLibraryDraw();}
void bookKeyEvent(uint8_t k){
 if(view==BOOK_LIBRARY){if(k==27||k==8){manager();return;}if((k==13||k==10)&&bookCount){bookOpen(bookTitles[bookChoice],false);if(view==BOOK_LIBRARY)bookLibraryDraw();return;}int next=constrain(bookChoice+(k==0xb5?-1:k==0xb6?1:0),0,max(0,bookCount-1));if(next!=bookChoice){bookChoice=next;bookLibraryDraw();}return;}
 if(k==27||k==8){bookSave();if(bookFromFiles)manager();else bookLibraryDraw();return;}
 if(k==0xb7||k==0xb6||k==32){if(bookNext>=bookSize)return;bookOffset=bookNext;}
 else if(k==0xb4||k==0xb5){if(!bookOffset)return;uint32_t target=bookOffset,pos=0; // Recreate exact page boundaries with bounded RAM.
 while(pos<target){bookOffset=pos; // Compute boundaries without display or SD writes.
 File f=SD_MMC.open(bookDataPath.c_str());if(!f)return;f.seek(pos);int row=0,col=0,rows=bookLarge?16:20,cols=bookLarge?75:94;
 while(f.available()&&row<rows){uint32_t at=f.position();int c=f.read();if(c=='\r')continue;if(c=='\n'){row++;col=0;continue;}if(c<32&&c!='\t')continue;if(col>=cols){row++;col=0;if(row>=rows){f.seek(at);break;}}int extra=(c&0xe0)==0xc0?1:(c&0xf0)==0xe0?2:(c&0xf8)==0xf0?3:0;while(extra--&&f.available())f.read();col++;}uint32_t next=f.position();f.close();if(next>=target||next<=pos)break;pos=next;delay(1);}
 bookOffset=pos;
 }
 else if(k=='f'||k=='F'){bookLarge=!bookLarge;bookOffset=0;}
 else if(k=='e'||k=='E'){
  String ext=bookPath;ext.toLowerCase();if(!ext.endsWith(".txt"))return;
  uint8_t old=bookEncoding;bookEncoding=(bookEncoding+1)%7;
  if(!bookPrepare(bookPath)){bookEncoding=old;bookPrepare(bookPath);status=bookFormatError;return;}
  File f=SD_MMC.open(bookDataPath);bookSize=f?f.size():0;f.close();bookOffset=bookMark=0;
  bookPrefs.putUChar((bookKey()+"e").c_str(),bookEncoding);
 }
 else if(k=='m'||k=='M')bookMark=bookOffset;
 else if(k=='g'||k=='G')bookOffset=bookMark;
 else return;
 bookDraw();
}
