#pragma once
#include <WebServer.h>
#include <qrcode.h>
WebServer exchangeServer(80);
bool exchangeOn=false,exchangeCancel=false,exchangeConfigured=false;
String exchangePassword,exchangeToken,exchangeTemp,exchangeTarget,exchangeResult;
File exchangeUpload;
size_t exchangeBytes=0;
unsigned long exchangeStarted=0;
void exchangeAbortUpload(){
  if(exchangeUpload)exchangeUpload.close();
  if(exchangeTemp.length())SD_MMC.remove(exchangeTemp.c_str());
  exchangeTemp="";
}
void exchangeStop(){
  exchangeOn=false;
  exchangeServer.client().stop();exchangeServer.stop();exchangeAbortUpload();
  exchangePassword="";exchangeToken="";
}
// Checked between upload/download chunks as well as by the main input loop.
void exchangeCheckCancel(){
  if(kbOK){uint8_t k=key();if(k=='x'||k=='X'||k==27||k==8)exchangeCancel=true;}
  if(WiFi.status()!=WL_CONNECTED||millis()-exchangeStarted>600000)exchangeCancel=true;
  if(exchangeCancel){exchangeAbortUpload();exchangeServer.client().stop();}
}
String exchangeEscape(String s){s.replace("&","&amp;");s.replace("<","&lt;");s.replace(">","&gt;");s.replace("\"","&quot;");s.replace("'","&#39;");return s;}
String exchangeEncode(const String&s){String r;char b[4];for(size_t i=0;i<s.length();i++){uint8_t c=s[i];if(isalnum(c)||c=='-'||c=='_'||c=='.')r+=(char)c;else{snprintf(b,4,"%%%02X",c);r+=b;}}return r;}
bool exchangePath(const String&p){if(!p.startsWith("/")||p.length()>220||p.indexOf('\\')>=0)return false;for(size_t i=0;i<p.length();i++)if((uint8_t)p[i]<32)return false;return p.indexOf("..")<0&&p.indexOf("/.exchange-")<0;}
bool exchangeCredentials(){
  if(!exchangeOn)return false;
  String cookie=exchangeServer.header("Cookie");
  int start=0;while(start<(int)cookie.length()){int end=cookie.indexOf(';',start);if(end<0)end=cookie.length();String item=cookie.substring(start,end);item.trim();if(item=="exchange="+exchangeToken)return true;start=end+1;}
  return exchangeServer.header("Authorization").startsWith("Basic ")&&exchangeServer.authenticate("user",exchangePassword.c_str());
}
void exchangePair(){
  exchangeServer.sendHeader("Cache-Control","no-store");
  exchangeServer.sendHeader("Referrer-Policy","no-referrer");
  if(!exchangeOn||exchangeServer.arg("key")!=exchangeToken){exchangeServer.send(403,"text/plain","Access expired. Scan the current QR code.");return;}
  // The QR is a session secret. Fill credentials only after validating it.
  String html="<!doctype html><html lang='ru'><meta charset='utf-8'><meta name='viewport' content='width=device-width'><title>Вход</title><body><h1>Вход в файловый обмен</h1><p>Логин и пароль заполнены автоматически.</p><form method='post' action='/login'><label>Логин <input name='login' autocomplete='username' value='user' readonly></label><br><label>Пароль <input type='password' name='password' autocomplete='current-password' value='"+exchangeEscape(exchangePassword)+"' readonly></label><input type='hidden' name='key' value='"+exchangeToken+"'><p><button type='submit'>Войти</button></p></form><script>history.replaceState(null,'','/pair');document.querySelector('form').submit();</script></body></html>";
  exchangeServer.send(200,"text/html; charset=utf-8",html);
}
void exchangeLogin(){
  exchangeServer.sendHeader("Cache-Control","no-store");
  exchangeServer.sendHeader("Referrer-Policy","no-referrer");
  if(!exchangeOn||exchangeServer.arg("key")!=exchangeToken||exchangeServer.arg("login")!="user"||exchangeServer.arg("password")!=exchangePassword){exchangeServer.send(403,"text/plain","Access expired. Scan the current QR code.");return;}
  exchangeServer.sendHeader("Set-Cookie","exchange="+exchangeToken+"; Path=/; HttpOnly; SameSite=Strict");
  exchangeServer.sendHeader("Location","/");exchangeServer.send(303,"text/plain","");
}
bool exchangeAuth(){
  exchangeServer.sendHeader("Cache-Control","no-store");
  if(exchangeCredentials())return true;
  exchangeServer.requestAuthentication(BASIC_AUTH,"File exchange");return false;
}
void exchangePage(){
  if(!exchangeAuth())return;
  String dir=exchangeServer.arg("dir");if(!dir.length())dir="/";
  if(!exchangePath(dir)){exchangeServer.send(400,"text/plain","Invalid path");return;}
  File d=SD_MMC.open(dir);if(!d||!d.isDirectory()){exchangeServer.send(404,"text/plain","Folder not found");return;}
  String html="<!doctype html><meta charset='utf-8'><meta name='viewport' content='width=device-width'><title>File exchange</title><style>body{font:18px sans-serif;max-width:850px;margin:30px auto;padding:16px}li{margin:12px}button,input{font:inherit}</style><h1>Обмен файлами</h1><p>"+exchangeEscape(dir)+"</p><p>HTTP без шифрования. Только доверенная Wi-Fi-сеть. Выход из приложения на устройстве отключает доступ.</p><p>До 32 МиБ на файл. Существующие файлы не заменяются. Имена новых файлов: латинские буквы, цифры, пробел, . _ -</p><form method='post' enctype='multipart/form-data' action='/upload?dir="+exchangeEncode(dir)+"&token="+exchangeToken+"'><input type='file' name='file' required><button>Передать на карту</button></form><p id='result' role='status'></p><script>const f=document.querySelector('form');f.onsubmit=e=>{e.preventDefault();const b=f.querySelector('button'),r=document.getElementById('result'),x=new XMLHttpRequest();b.disabled=true;r.textContent='Загрузка…';x.open('POST',f.action);x.upload.onprogress=e=>{if(e.lengthComputable)r.textContent='Загрузка: '+Math.round(e.loaded*100/e.total)+'%';};x.onload=()=>{b.disabled=false;r.textContent=x.status===200?'Файл сохранён на SD-карту. Обновите список ниже.':x.responseText;};x.onerror=()=>{b.disabled=false;r.textContent='Связь прервана. Проверьте соединение и список файлов.';};x.send(new FormData(f));};</script><p><a href='/?dir="+exchangeEncode(parent(dir))+"'>Вверх</a></p><ul>";
  int n=0;File f=d.openNextFile();while(f&&n<150){String name=leaf(String(f.name()));String path=(dir=="/"?"/":dir+"/")+name;
    if(!name.startsWith(".exchange-")){html+="<li><a href='"+String(f.isDirectory()?"/?dir=":"/download?path=")+exchangeEncode(path)+"'>"+exchangeEscape(name)+(f.isDirectory()?"/":"")+"</a> "+String((unsigned long)f.size())+" B</li>";n++;}
    f.close();f=d.openNextFile();
  }if(f)html+="<li>Показаны первые 150 файлов.</li>";f.close();d.close();html+="</ul><p><a href='/?dir="+exchangeEncode(dir)+"'>Обновить список</a></p>";
  exchangeServer.send(200,"text/html; charset=utf-8",html);
}
void exchangeReceive(){
  HTTPUpload &u=exchangeServer.upload();
  exchangeCheckCancel();if(exchangeCancel)return;
  if(u.status==UPLOAD_FILE_START){
    exchangeAbortUpload();exchangeBytes=0;exchangeResult="Upload rejected";
    if(!exchangeCredentials()||exchangeServer.arg("token")!=exchangeToken)return;
    String dir=exchangeServer.arg("dir"),name=u.filename;
    if(!exchangePath(dir)||!name.length()||name.length()>100||name=="."||name.indexOf("..")>=0)return;
    for(size_t i=0;i<name.length();i++){char c=name[i];if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c==' '||c=='.'||c=='_'||c=='-'))return;}
    File d=SD_MMC.open(dir);bool valid=d&&d.isDirectory();d.close();if(!valid)return;
    exchangeTarget=(dir=="/"?"/":dir+"/")+name;
    if(SD_MMC.exists(exchangeTarget)){exchangeResult="File already exists. Rename it on your phone or computer first.";return;}
    exchangeTemp=(dir=="/"?"/":dir+"/")+".exchange-"+exchangeToken+".part";
    exchangeUpload=SD_MMC.open(exchangeTemp,FILE_WRITE);
    if(!exchangeUpload){exchangeResult="Cannot write card";exchangeAbortUpload();}
  }else if(u.status==UPLOAD_FILE_WRITE&&exchangeUpload){
    if(exchangeBytes+u.currentSize>32UL*1024*1024||exchangeUpload.write(u.buf,u.currentSize)!=u.currentSize){exchangeResult="Write failed or file exceeds 32 MiB";exchangeAbortUpload();return;}
    exchangeBytes+=u.currentSize;
  }else if(u.status==UPLOAD_FILE_END&&exchangeUpload){
    exchangeUpload.flush();exchangeUpload.close();
    if(!SD_MMC.exists(exchangeTarget)&&SD_MMC.rename(exchangeTemp,exchangeTarget)){exchangeResult="File saved to TF card";appNotify("FILE","SAVED","TF");exchangeTemp="";}else{exchangeResult="Cannot finish upload";exchangeAbortUpload();}
  }else if(u.status==UPLOAD_FILE_ABORTED){exchangeResult="Upload interrupted";exchangeAbortUpload();}
}
void exchangeDownload(){
  if(!exchangeAuth())return;String p=exchangeServer.arg("path");if(!exchangePath(p)){exchangeServer.send(400,"text/plain","Invalid path");return;}
  File f=SD_MMC.open(p);if(!f||f.isDirectory()){f.close();exchangeServer.send(404,"text/plain","Not found");return;}
  exchangeServer.sendHeader("Content-Disposition","attachment");exchangeServer.sendHeader("X-Content-Type-Options","nosniff");exchangeServer.setContentLength(f.size());exchangeServer.send(200,"application/octet-stream","");
  uint8_t buf[1024];while(f.available()&&exchangeServer.client().connected()){
    exchangeCheckCancel();if(exchangeCancel)break;size_t n=f.read(buf,sizeof(buf));if(!n)break;exchangeServer.sendContent((const char*)buf,n);delay(1);
  }f.close();
}
void exchangeDraw(){
  view=FILE_EXCHANGE;
  String address="http://"+WiFi.localIP().toString()+"/";
  String link=address+"pair?key="+exchangeToken;
  QRCode qr;uint8_t modules[qrcode_getBufferSize(5)];
  bool qrOK=qrcode_initText(&qr,modules,5,ECC_MEDIUM,link.c_str())==0;
  beginDraw();do{
    header("FILE EXCHANGE","Wi-Fi file exchange / phone + computer","ESC / BACKSPACE: STOP AND RETURN");
    text("FILE ACCESS: ON",28,120,true);
    text("SCAN QR: AUTOMATIC LOGIN",28,165,true);
    text("Phone must use the same Wi-Fi.",28,203);
    text("Login + password filled automatically.",28,235);
    text(address,28,280,true);
    text("LOGIN: user",28,320);
    text("PASSWORD: "+exchangePassword,28,350);
    text("Auto-off: 10 minutes. Exit stops access.",28,390);
    text("Trusted Wi-Fi only. QR grants access.",28,425);
    if(qrOK){
      const int scale=6,x0=510,y0=112;
      epd.fillRect(x0,y0,(qr.size+8)*scale,(qr.size+8)*scale,GxEPD_WHITE);
      for(int y=0;y<qr.size;y++)for(int x=0;x<qr.size;x++)
        if(qrcode_getModule(&qr,x,y))epd.fillRect(x0+(x+4)*scale,y0+(y+4)*scale,scale,scale,GxEPD_BLACK);
    }else text("QR NOT AVAILABLE",510,230);
  }while(epd.nextPage());
}
void exchangeStart(){
  if(WiFi.status()!=WL_CONNECTED){status="CONNECT WIFI FIRST";system();return;}
  if(!cardOK){status="TF CARD NOT MOUNTED";system();return;}
  if(!exchangeConfigured){const char* h[]={"Authorization","Cookie"};exchangeServer.collectHeaders(h,2);exchangeServer.on("/pair",HTTP_GET,exchangePair);exchangeServer.on("/login",HTTP_POST,exchangeLogin);exchangeServer.on("/",HTTP_GET,exchangePage);exchangeServer.on("/download",HTTP_GET,exchangeDownload);exchangeServer.on("/upload",HTTP_POST,[]{if(!exchangeAuth())return;exchangeServer.send(exchangeResult=="File saved to TF card"?200:400,"text/plain; charset=utf-8",exchangeResult+"\nUse browser Back, then refresh the folder.");},exchangeReceive);exchangeServer.onNotFound([]{if(exchangeAuth())exchangeServer.send(404,"text/plain","Not found");});exchangeConfigured=true;}
  char p[17],t[33];snprintf(p,sizeof(p),"%08lx%08lx",(unsigned long)esp_random(),(unsigned long)esp_random());snprintf(t,sizeof(t),"%08lx%08lx%08lx%08lx",(unsigned long)esp_random(),(unsigned long)esp_random(),(unsigned long)esp_random(),(unsigned long)esp_random());exchangePassword=p;exchangeToken=t;
  exchangeCancel=false;exchangeStarted=millis();exchangeOn=true;exchangeServer.begin();exchangeDraw();
}
void exchangeTick(){if(!exchangeOn)return;if(WiFi.status()!=WL_CONNECTED||millis()-exchangeStarted>600000)exchangeCancel=true;if(!exchangeCancel)exchangeServer.handleClient();if(exchangeCancel){exchangeStop();loadFolder(folder);status="FILE ACCESS OFF";system();}}
