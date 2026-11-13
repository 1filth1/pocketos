#pragma once
static unsigned long reminderBellUntil=0;
static String appNotice[3];
static unsigned long appNoticeAt=0;
static bool appNoticePending=false,appNoticeActive=false;
static String noticeLog[12];
static int noticeCount=0;
static bool noticeRead[12]={};
static bool noticeSave(){
 SdSettings n;if(!n.begin("notices",false))return false;
 bool ok=n.putUChar("count",noticeCount)>0;
 for(int i=0;i<noticeCount;i++){ok=(n.putString(String(i).c_str(),noticeLog[i])>0)&&ok;n.putUChar(("r"+String(i)).c_str(),noticeRead[i]);}n.end();return ok;
}
static void noticeMark(int i){if(i>=0&&i<noticeCount){noticeRead[i]=true;noticeSave();}}
static void noticeClearRead(){int keep=0;for(int i=0;i<noticeCount;i++)if(!noticeRead[i]){noticeLog[keep]=noticeLog[i];noticeRead[keep++]=false;}noticeCount=keep;noticeSave();}

void noticeLoad(){SdSettings n;if(!n.begin("notices",true))return;noticeCount=min(12,(int)n.getUChar("count",0));for(int i=0;i<noticeCount;i++){noticeLog[i]=n.getString(String(i).c_str(),"");noticeRead[i]=n.getUChar(("r"+String(i)).c_str(),0);}n.end();}
void appNotify(const String &app,const String &event,const String &detail=""){
 for(int i=min(noticeCount,11);i>0;i--){noticeLog[i]=noticeLog[i-1];noticeRead[i]=noticeRead[i-1];}noticeRead[0]=false;
 noticeLog[0]=app+" / "+event+(detail.length()?" / "+detail:"");noticeCount=min(12,noticeCount+1);
 noticeSave();
 appNotice[0]=app.substring(0,5);appNotice[1]=event.substring(0,5);appNotice[2]=detail.substring(0,5);
 appNoticeAt=millis();appNoticePending=true;appNoticeActive=true;
}
