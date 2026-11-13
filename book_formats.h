#pragma once
#if __has_include(<esp32s3/rom/miniz.h>)
#include <esp32s3/rom/miniz.h>
#else
#include <miniz.h>
#endif
#include <vector>
// Original books are never modified. Derived plain text is built atomically on TF.
String bookDataPath,bookFormatError;
uint8_t bookEncoding=0;
const char* bookEncodingNames[]={"AUTO","UTF-8","WINDOWS-1251","CP866","KOI8-R","UTF-16 LE","UTF-16 BE"};
bool filePlainText(String s){s.toLowerCase();const char* exts[]={".txt",".log",".csv",".ini",".md",".json",".xml",".html",".htm",".css",".js",".yaml",".yml",".cfg",".conf",".cpp",".h",".c",".py",".sh"};for(auto e:exts)if(s.endsWith(e))return true;return false;}
bool bookSupported(String s){s.toLowerCase();return s.endsWith(".txt")||s.endsWith(".fb2")||s.endsWith(".epub");}
String bxLocal(String s){int p=s.indexOf(':');return p<0?s:s.substring(p+1);}
String bxName(String t){t.trim();int p=0;while(p<(int)t.length()&&!isspace((unsigned char)t[p])&&t[p]!='/'&&t[p]!='>')p++;return bxLocal(t.substring(0,p));}
String bxAttr(String t,String key){int p=0;while(p<(int)t.length()){while(p<(int)t.length()&&(isspace((unsigned char)t[p])||t[p]=='/'))p++;int a=p;while(p<(int)t.length()&&!isspace((unsigned char)t[p])&&t[p]!='=')p++;String k=bxLocal(t.substring(a,p));while(p<(int)t.length()&&isspace((unsigned char)t[p]))p++;if(t[p]!='='){if(p==(int)t.length())break;continue;}p++;while(p<(int)t.length()&&isspace((unsigned char)t[p]))p++;char q=t[p++];if(q!='\''&&q!='"')break;int b=p;while(p<(int)t.length()&&t[p]!=q)p++;if(k==key)return t.substring(b,p);p++;}return "";}
String bxUtf(uint32_t c){String s;if(!c||c>0x10ffff||(c>=0xd800&&c<=0xdfff))return "";if(c<128)s+=(char)c;else if(c<2048){s+=(char)(0xc0|(c>>6));s+=(char)(0x80|(c&63));}else if(c<65536){s+=(char)(0xe0|(c>>12));s+=(char)(0x80|((c>>6)&63));s+=(char)(0x80|(c&63));}else{s+=(char)(0xf0|(c>>18));s+=(char)(0x80|((c>>12)&63));s+=(char)(0x80|((c>>6)&63));s+=(char)(0x80|(c&63));}return s;}
String bxEntity(String s){if(s=="amp")return "&";if(s=="lt")return "<";if(s=="gt")return ">";if(s=="quot")return "\"";if(s=="apos")return "'";if(s=="nbsp")return " ";if(s=="mdash")return bxUtf(0x2014);if(s=="ndash")return bxUtf(0x2013);if(s=="hellip")return bxUtf(0x2026);if(s.startsWith("#")){bool hex=s.length()>2&&(s[1]=='x'||s[1]=='X');return bxUtf(strtoul(s.c_str()+(hex?2:1),nullptr,hex?16:10));}return "&"+s+";";}
bool bxEncoding(File &f){uint8_t b[2]={};f.read(b,2);f.seek(0);if(b[0]==0xff||b[0]==0xfe||b[0]==0){bookFormatError="USE UTF-8, NOT UTF-16";return false;}String h;for(int i=0;i<200&&f.available();i++){char c=f.read();h+=c;if(c=='>')break;}f.seek(0);h.toLowerCase();int p=h.indexOf("encoding");if(p>=0){String e=bxAttr(h.substring(p),"encoding");if(e.length()&&e!="utf-8"&&e!="utf8"&&e!="us-ascii"){bookFormatError="CONVERT BOOK TO UTF-8";return false;}}return true;}
// Bounded tag/entity buffers; binary FB2 covers and HTML scripts never enter text.
bool bxText(File &in,File &out,bool fb2){if(!bxEncoding(in))return false;bool body=false;int suppress=0;String suppressed;bool space=false;uint32_t tick=0;
 auto emit=[&](String s){if(!body||suppress)return true;for(unsigned i=0;i<s.length();i++){uint8_t c=s[i];if(c=='\r')continue;if(c=='\n'||c=='\t'||c==' '){if(space)continue;c=' ';space=true;}else space=false;if(out.write(c)!=1)return false;}return true;};
 while(in.available()){int c=in.read();if((++tick&4095)==0)delay(1);if(out.position()>32u*1024*1024){bookFormatError="BOOK TEXT EXCEEDS 32 MB";return false;}
 if(c=='<'){String t;char quote=0;bool end=false;for(unsigned n=0;in.available();n++){char v=in.read();if(v=='>'&&!quote){end=true;break;}if(v=='\''||v=='"'){if(!quote)quote=v;else if(quote==v)quote=0;}if(n<4096)t+=v;else{bookFormatError="XML TAG TOO LONG";return false;}}if(!end)return false;
 t.trim();if(t.startsWith("!--")){// Comments may contain >; consume through their actual terminator.
 if(!t.endsWith("--")){int a=0,b=0;while(in.available()){int v=in.read();if(a=='-'&&b=='-'&&v=='>')break;a=b;b=v;}}continue;}
 bool closing=t.startsWith("/");if(closing)t.remove(0,1);String n=bxName(t);n.toLowerCase();
 if(n=="body"){body=!closing;continue;}
 if(n=="binary"||n=="script"||n=="style"||n=="head"){if(!closing){suppress++;suppressed=n;}else if(suppress&&n==suppressed)suppress--;continue;}
 if(body&&!suppress&&(n=="p"||n=="br"||n=="div"||n=="section"||n=="title"||n=="subtitle"||n=="empty-line"||n=="li"||n=="h1"||n=="h2"||n=="h3"||n=="v"||n=="tr")){if(out.write('\n')!=1)return false;space=true;}
 }else if(c=='&'){String e;while(in.available()&&e.length()<32){char v=in.read();if(v==';')break;e+=v;}if(!emit(bxEntity(e)))return false;}else {String s((char)c);if(!emit(s))return false;}}
 return true;}
struct BZipEntry {String name;uint32_t packed,size,crc,offset;uint16_t method,flags;};
uint16_t bz16(const uint8_t*p){return p[0]|(uint16_t)p[1]<<8;}
uint32_t bz32(const uint8_t*p){return bz16(p)|(uint32_t)bz16(p+2)<<16;}
bool bzIndex(File &f,std::vector<BZipEntry>&v){if(f.size()<22)return false;uint32_t start=f.size()>65557?f.size()-65557:0;uint8_t h[46];uint32_t cd=0;unsigned count=0;bool found=false;
 for(int64_t p=(int64_t)f.size()-22;p>=start;p--){f.seek(p);if(f.read(h,22)!=22)return false;if(bz32(h)==0x06054b50&&p+22+bz16(h+20)==f.size()){if(bz16(h+4)||bz16(h+6)||bz16(h+8)!=bz16(h+10))return false;count=bz16(h+10);cd=bz32(h+16);found=true;break;}}
 if(!found||!count||count>4096)return false;f.seek(cd);
 for(unsigned i=0;i<count;i++){if(f.read(h,46)!=46||bz32(h)!=0x02014b50)return false;uint16_t len=bz16(h+28);if(len>1024)return false;String name;for(unsigned j=0;j<len;j++){int c=f.read();if(c<0)return false;name+=(char)c;}BZipEntry e={name,bz32(h+20),bz32(h+24),bz32(h+16),bz32(h+42),bz16(h+10),bz16(h+8)};v.push_back(e);uint32_t p=f.position()+bz16(h+30)+bz16(h+32);if(p>f.size())return false;f.seek(p);delay(1);}return true;}
int bzFind(const std::vector<BZipEntry>&v,const String &n){for(unsigned i=0;i<v.size();i++)if(v[i].name==n)return i;return -1;}
uint32_t bzCRC(const uint8_t*p,uint32_t n){uint32_t c=~0u;while(n--){c^=*p++;for(int i=0;i<8;i++)c=(c>>1)^(0xedb88320u&-(int32_t)(c&1));}return ~c;}
bool bzExtract(File &zip,const BZipEntry &e,const String &dest){if(e.flags&1){bookFormatError="ENCRYPTED EPUB NOT SUPPORTED";return false;}if(e.size>2*1024*1024||e.packed>2*1024*1024){bookFormatError="EPUB CHAPTER EXCEEDS 2 MB";return false;}if(e.method!=0&&e.method!=8){bookFormatError="UNSUPPORTED ZIP COMPRESSION";return false;}uint8_t h[30];zip.seek(e.offset);if(zip.read(h,30)!=30||bz32(h)!=0x04034b50)return false;uint32_t off=e.offset+30+bz16(h+26)+bz16(h+28);if(off>zip.size()||e.packed>zip.size()-off)return false;zip.seek(off);
 uint8_t *a=(uint8_t*)ps_malloc(max<uint32_t>(1,e.packed)),*b=(uint8_t*)ps_malloc(max<uint32_t>(1,e.size));if(!a)a=(uint8_t*)malloc(max<uint32_t>(1,e.packed));if(!b)b=(uint8_t*)malloc(max<uint32_t>(1,e.size));bool ok=a&&b;
 if(ok)ok=zip.read(a,e.packed)==e.packed;
 if(ok&&e.method==0){ok=e.packed==e.size;if(ok)memcpy(b,a,e.size);}else if(ok){tinfl_decompressor *d=(tinfl_decompressor*)malloc(sizeof(tinfl_decompressor));if(!d)ok=false;else{tinfl_init(d);size_t ni=e.packed,no=e.size;ok=tinfl_decompress(d,a,&ni,b,b,&no,TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)==TINFL_STATUS_DONE&&no==e.size;free(d);}}
 if(ok)ok=bzCRC(b,e.size)==e.crc;File f;if(ok){f=SD_MMC.open(dest,"w");ok=f&&f.write(b,e.size)==e.size;f.flush();f.close();}free(a);free(b);if(!ok&&bookFormatError.isEmpty())bookFormatError="EPUB DAMAGED / MEMORY OR TF ERROR";delay(1);return ok;}
String bxResolve(String base,String href){int hash=href.indexOf('#');if(hash>=0)href=href.substring(0,hash);String decoded;for(unsigned i=0;i<href.length();i++){if(href[i]=='%'&&i+2<href.length()){String h=href.substring(i+1,i+3);decoded+=(char)strtoul(h.c_str(),nullptr,16);i+=2;}else decoded+=href[i];}String path=base+decoded,result;int p=0;std::vector<String> seg;while(p<(int)path.length()){int e=path.indexOf('/',p);if(e<0)e=path.length();String s=path.substring(p,e);if(s==".."){if(seg.empty())return "";seg.pop_back();}else if(s.length()&&s!=".")seg.push_back(s);p=e+1;}for(auto &s:seg){if(result.length())result+='/';result+=s;}return result;}
String bxReadMeta(const String &p){File f=SD_MMC.open(p);String s;if(f&&f.size()<=256*1024){s.reserve(f.size());while(f.available())s+=(char)f.read();}f.close();return s;}
String bxTag(String &s,int &p){int a=s.indexOf('<',p);if(a<0){p=s.length();return "";}int b=s.indexOf('>',a);if(b<0){p=s.length();return "";}p=b+1;return s.substring(a+1,b);}
// Normalize TXT to UTF-8 without modifying the source. Validate the whole stream
// before choosing Windows-1251, including multibyte sequences across read boundaries.
bool bxPrepareTxt(const String &path){
 File in=SD_MMC.open(path);if(!in||in.isDirectory()){bookFormatError="CANNOT OPEN TEXT";return false;}
 int a=in.read(),b=in.read();bool le=bookEncoding==5||(bookEncoding==0&&a==255&&b==254),be=bookEncoding==6||(bookEncoding==0&&a==254&&b==255);
 bool valid=true;int need=0;uint32_t cp=0,minimum=0;in.seek(0);unsigned tick=0;
 if(!le&&!be)while(in.available()){int c=in.read();if(!need){if(c<128){}else if(c>=194&&c<=244){need=c<224?1:c<240?2:3;cp=c&((1<<(6-need))-1);minimum=need==1?128:need==2?2048:65536;}else{valid=false;break;}}else{if((c&192)!=128){valid=false;break;}cp=(cp<<6)|(c&63);if(!--need&&(cp<minimum||cp>0x10ffff||(cp>=0xd800&&cp<=0xdfff))){valid=false;break;}}if((++tick&4095)==0)delay(1);}
 if(bookEncoding==1||(bookEncoding==0&&!le&&!be&&valid&&!need)){in.close();return true;}
 static const uint16_t win1251[128]={0x0402,0x0403,0x201a,0x0453,0x201e,0x2026,0x2020,0x2021,0x20ac,0x2030,0x0409,0x2039,0x040a,0x040c,0x040b,0x040f,0x0452,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0xfffd,0x2122,0x0459,0x203a,0x045a,0x045c,0x045b,0x045f,0x00a0,0x040e,0x045e,0x0408,0x00a4,0x0490,0x00a6,0x00a7,0x0401,0x00a9,0x0404,0x00ab,0x00ac,0x00ad,0x00ae,0x0407,0x00b0,0x00b1,0x0406,0x0456,0x0491,0x00b5,0x00b6,0x00b7,0x0451,0x2116,0x0454,0x00bb,0x0458,0x0405,0x0455,0x0457,0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,0x0418,0x0419,0x041a,0x041b,0x041c,0x041d,0x041e,0x041f,0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,0x0428,0x0429,0x042a,0x042b,0x042c,0x042d,0x042e,0x042f,0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,0x0438,0x0439,0x043a,0x043b,0x043c,0x043d,0x043e,0x043f,0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,0x0448,0x0449,0x044a,0x044b,0x044c,0x044d,0x044e,0x044f};
 static const uint16_t dos866[128]={0x410,0x411,0x412,0x413,0x414,0x415,0x416,0x417,0x418,0x419,0x41a,0x41b,0x41c,0x41d,0x41e,0x41f,0x420,0x421,0x422,0x423,0x424,0x425,0x426,0x427,0x428,0x429,0x42a,0x42b,0x42c,0x42d,0x42e,0x42f,0x430,0x431,0x432,0x433,0x434,0x435,0x436,0x437,0x438,0x439,0x43a,0x43b,0x43c,0x43d,0x43e,0x43f,0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255d,0x255c,0x255b,0x2510,0x2514,0x2534,0x252c,0x251c,0x2500,0x253c,0x255e,0x255f,0x255a,0x2554,0x2569,0x2566,0x2560,0x2550,0x256c,0x2567,0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256b,0x256a,0x2518,0x250c,0x2588,0x2584,0x258c,0x2590,0x2580,0x440,0x441,0x442,0x443,0x444,0x445,0x446,0x447,0x448,0x449,0x44a,0x44b,0x44c,0x44d,0x44e,0x44f,0x401,0x451,0x404,0x454,0x407,0x457,0x40e,0x45e,0xb0,0x2219,0xb7,0x221a,0x2116,0xa4,0x25a0,0xa0};
 static const uint16_t koi8[128]={0x2500,0x2502,0x250c,0x2510,0x2514,0x2518,0x251c,0x2524,0x252c,0x2534,0x253c,0x2580,0x2584,0x2588,0x258c,0x2590,0x2591,0x2592,0x2593,0x2320,0x25a0,0x2219,0x221a,0x2248,0x2264,0x2265,0xa0,0x2321,0xb0,0xb2,0xb7,0xf7,0x2550,0x2551,0x2552,0x451,0x2553,0x2554,0x2555,0x2556,0x2557,0x2558,0x2559,0x255a,0x255b,0x255c,0x255d,0x255e,0x255f,0x2560,0x2561,0x401,0x2562,0x2563,0x2564,0x2565,0x2566,0x2567,0x2568,0x2569,0x256a,0x256b,0x256c,0xa9,0x44e,0x430,0x431,0x446,0x434,0x435,0x444,0x433,0x445,0x438,0x439,0x43a,0x43b,0x43c,0x43d,0x43e,0x43f,0x44f,0x440,0x441,0x442,0x443,0x436,0x432,0x44c,0x44b,0x437,0x448,0x44d,0x449,0x447,0x44a,0x42e,0x410,0x411,0x426,0x414,0x415,0x424,0x413,0x425,0x418,0x419,0x41a,0x41b,0x41c,0x41d,0x41e,0x41f,0x42f,0x420,0x421,0x422,0x423,0x416,0x412,0x42c,0x42b,0x417,0x428,0x42d,0x429,0x427,0x42a};
 SD_MMC.mkdir("/settings");SD_MMC.mkdir("/settings/books");
 String tmp="/settings/books/txt-convert.tmp",dest="/settings/books/txt-convert.txt";
 File out=SD_MMC.open(tmp,"w");bool ok=(bool)out;in.seek(((le&&a==255&&b==254)||(be&&a==254&&b==255))?2:0);uint32_t high=0;
 while(ok&&in.available()){
  uint32_t c=in.read();
  if(le||be){int d=in.read();if(d<0){ok=false;break;}c=le?c|((uint32_t)d<<8):(c<<8)|d;
   if(c>=0xd800&&c<=0xdbff){if(high){ok=false;break;}high=c;continue;}
   if(c>=0xdc00&&c<=0xdfff){if(!high){ok=false;break;}c=0x10000+((high-0xd800)<<10)+(c-0xdc00);high=0;}
   else if(high){ok=false;break;}
  }else if(c>=128)c=bookEncoding==3?dos866[c-128]:bookEncoding==4?koi8[c-128]:win1251[c-128];
  String u=bxUtf(c);ok=out.print(u)==u.length();if((++tick&4095)==0)delay(1);
 }
 ok=ok&&!high;uint32_t size=out.position();out.flush();out.close();in.close();
 if(ok){File v=SD_MMC.open(tmp);ok=v&&v.size()==size;v.close();}
 if(ok&&SD_MMC.exists(dest))ok=SD_MMC.remove(dest);
 if(ok)ok=SD_MMC.rename(tmp,dest);
 if(!ok){SD_MMC.remove(tmp);bookFormatError="TEXT CONVERSION / TF WRITE ERROR";return false;}
 bookDataPath=dest;return true;
}
bool bookPrepare(const String &path){bookFormatError="";bookDataPath=path;String ext=path;ext.toLowerCase();if(filePlainText(ext)){return bxPrepareTxt(path);}SD_MMC.mkdir("/settings");SD_MMC.mkdir("/settings/books");String tmp="/settings/books/reader.tmp",xml="/settings/books/chapter.tmp";File in=SD_MMC.open(path),out=SD_MMC.open(tmp,"w");bool ok=in&&out;if(!ok)bookFormatError="CANNOT OPEN BOOK / TF WRITE FAILED";
 if(ok&&ext.endsWith(".fb2"))ok=bxText(in,out,true);
 else if(ok){std::vector<BZipEntry> entries;ok=bzIndex(in,entries);if(!ok)bookFormatError="INVALID EPUB ZIP";
 if(ok&&bzFind(entries,"META-INF/encryption.xml")>=0){ok=false;bookFormatError="ENCRYPTED/OBFUSCATED EPUB NOT SUPPORTED";}
 int idx=ok?bzFind(entries,"META-INF/container.xml"):-1;String opf;
 if(ok){ok=idx>=0&&bzExtract(in,entries[idx],xml);if(ok){String meta=bxReadMeta(xml);int p=0;while(p<(int)meta.length()){String t=bxTag(meta,p);if(bxName(t)=="rootfile"){opf=bxAttr(t,"full-path");break;}}ok=opf.length()>0;}}
 idx=ok?bzFind(entries,opf):-1;String meta;
 if(ok){ok=idx>=0&&bzExtract(in,entries[idx],xml);if(ok){meta=bxReadMeta(xml);ok=meta.length()>0;}}
 struct Item{String id,href;};std::vector<Item> manifest;std::vector<String> spine;
 if(ok){int p=0;while(p<(int)meta.length()){String t=bxTag(meta,p),n=bxName(t);if(n=="item"){String type=bxAttr(t,"media-type");if(type=="application/xhtml+xml"||type=="text/html")manifest.push_back({bxAttr(t,"id"),bxAttr(t,"href")});}else if(n=="itemref")spine.push_back(bxAttr(t,"idref"));}ok=!spine.empty();}
 String base=opf.substring(0,opf.lastIndexOf('/')+1);
 for(auto &id:spine){if(!ok)break;String href;for(auto &item:manifest)if(item.id==id){href=item.href;break;}idx=bzFind(entries,bxResolve(base,href));ok=href.length()&&idx>=0&&bzExtract(in,entries[idx],xml);if(ok){File chapter=SD_MMC.open(xml);ok=chapter&&bxText(chapter,out,false);chapter.close();if(ok)ok=out.write('\n')==1;}delay(1);}
 }
 uint32_t size=out.position();out.flush();out.close();in.close();SD_MMC.remove(xml);if(ok&&!size){ok=false;bookFormatError="NO READABLE TEXT IN BOOK";}if(ok){File verify=SD_MMC.open(tmp);ok=verify&&verify.size()==size;verify.close();}String finalPath="/settings/books/reader.txt";if(ok){if(SD_MMC.exists(finalPath))ok=SD_MMC.remove(finalPath);if(ok)ok=SD_MMC.rename(tmp,finalPath);}if(!ok){SD_MMC.remove(tmp);if(!bookFormatError.length())bookFormatError="BOOK FORMAT / TF WRITE ERROR";return false;}bookDataPath=finalPath;return true;}
