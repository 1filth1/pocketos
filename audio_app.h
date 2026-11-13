#pragma once
#include <ESP_I2S.h>
#include "es8311.h"
#include <math.h>
// Exact board wiring and codec sequence: Waveshare 01_Audio_Test.
static I2SClass audioBus;
static es8311_handle_t audioCodec=nullptr;
static bool codecReady=false,audioBusReady=false;
static String audioResult="NOT TESTED";
static int audioVolume=40;
static bool audioMuted=false;
static String micResult="NOT TESTED";
static void audioInit(){
  // NS4150B enable is active HIGH. Keep output muted except during a test.
  pinMode(39,OUTPUT);digitalWrite(39,LOW);
  Wire.beginTransmission(ES8311_ADDRRES_0);
  if(Wire.endTransmission()!=0){audioResult="CODEC NOT FOUND";return;}
  if(!audioCodec)audioCodec=es8311_create(I2C_NUM_0,ES8311_ADDRRES_0);
  es8311_clock_config_t cfg={};cfg.mclk_from_mclk_pin=true;
  cfg.mclk_frequency=24000*256;cfg.sample_frequency=24000;
  codecReady=audioCodec&&es8311_init(audioCodec,&cfg,ES8311_RESOLUTION_16,ES8311_RESOLUTION_16)==ESP_OK;
  if(codecReady)codecReady=es8311_voice_volume_set(audioCodec,audioMuted?0:audioVolume,nullptr)==ESP_OK&&es8311_microphone_config(audioCodec,false)==ESP_OK;
  if(!codecReady){audioResult="CODEC INIT ERROR";return;}
  audioBus.setPins(14,47,48,21,13);
  audioBusReady=audioBus.begin(I2S_MODE_STD,24000,I2S_DATA_BIT_WIDTH_16BIT,I2S_SLOT_MODE_MONO,I2S_STD_SLOT_LEFT);
  audioResult=audioBusReady?"READY / TONE NOT TESTED":"I2S INIT ERROR";
}
static void audioTone(){
 if(!codecReady||!audioBusReady){audioResult="INIT FAILED / NO TONE";return;}
 int16_t samples[240];bool ok=true;
 digitalWrite(39,HIGH);delay(10);
 // 300 ms, 660 Hz, low amplitude with fade-in/out to reduce clicks.
 for(int b=0;b<30;b++){
  for(int i=0;i<240;i++){int n=b*240+i;float fade=min(1.0f,min(n/240.0f,(7199-n)/240.0f));samples[i]=int16_t(2200*fade*sinf(2*PI*660*n/24000));}
  if(audioBus.write((uint8_t*)samples,sizeof(samples))!=sizeof(samples)){ok=false;break;}
 }
 memset(samples,0,sizeof(samples));audioBus.write((uint8_t*)samples,sizeof(samples));delay(30);digitalWrite(39,LOW);
 audioResult=ok?"TONE SENT / CHECK SPEAKER":"I2S WRITE ERROR";
}

// Notification chime: A4, C5, E5, A5, with soft edges and short gaps.
static void audioNotificationMelody(){
 if(!codecReady||!audioBusReady){audioResult="INIT FAILED / NO MELODY";return;}
 const float notes[]={440.0f,523.251f,659.255f,880.0f};
 const int blocks[]={18,18,18,36}; // 10 ms per block at 24 kHz.
 int16_t samples[240];bool ok=true;
 digitalWrite(39,HIGH);delay(10);
 for(int note=0;note<4&&ok;note++){
  const int count=blocks[note]*240;
  for(int b=0;b<blocks[note];b++){
   for(int i=0;i<240;i++){
    int n=b*240+i;
    float fade=min(1.0f,min(n/240.0f,(count-1-n)/240.0f));
    samples[i]=int16_t(2200*fade*sinf(2*PI*notes[note]*n/24000));
   }
   if(audioBus.write((uint8_t*)samples,sizeof(samples))!=sizeof(samples)){ok=false;break;}
  }
  memset(samples,0,sizeof(samples));
  for(int gap=0;gap<4&&ok;gap++)ok=audioBus.write((uint8_t*)samples,sizeof(samples))==sizeof(samples);
 }
 delay(30);digitalWrite(39,LOW);
 audioResult=ok?"MELODY SENT / CHECK SPEAKER":"I2S WRITE ERROR";
}

static bool audioSetVolume(int value){
 if(!codecReady){audioResult="CODEC NOT READY";return false;}
 value=constrain(value,0,100);
 if(es8311_voice_volume_set(audioCodec,audioMuted?0:value,nullptr)!=ESP_OK){audioResult="VOLUME SET FAILED";return false;}
 audioVolume=value;audioResult="VOLUME APPLIED";return true;
}
static bool audioSetMuted(bool muted){
 if(!codecReady){audioResult="CODEC NOT READY";return false;}
 if(es8311_voice_volume_set(audioCodec,muted?0:audioVolume,nullptr)!=ESP_OK){audioResult="MUTE SET FAILED";return false;}
 audioMuted=muted;audioResult=muted?"SOUND OFF":"SOUND ON";return true;
}
// Stream directly to TF: PCM WAV, mono, 24000 Hz, signed 16 bit.
static String micPath="";
static bool audioRecordMic(){
 digitalWrite(39,LOW);
 if(!SD_MMC.cardType() || SD_MMC.cardType()==CARD_NONE){micResult="TF CARD NOT MOUNTED";return false;}
 if(!codecReady||!audioBusReady){micResult="AUDIO NOT READY";return false;}
 if(es8311_microphone_gain_set(audioCodec,ES8311_MIC_GAIN_24DB)!=ESP_OK){micResult="MIC CONFIG ERROR";return false;}
 if(!SD_MMC.exists("/RECORDINGS")&&!SD_MMC.mkdir("/RECORDINGS")){micResult="FOLDER CREATE FAILED";return false;}
 String path;
 for(unsigned n=1;n<=99999;n++){char name[40];snprintf(name,sizeof(name),"/RECORDINGS/REC%05u.WAV",n);if(!SD_MMC.exists(name)){path=name;break;}}
 if(!path.length()){micResult="RECORDING NAMES FULL";return false;}
 File out=SD_MMC.open(path,FILE_WRITE);
 if(!out){micResult="FILE CREATE FAILED";return false;}
 constexpr uint32_t bytes=24000*2*3;
 uint8_t header[44]={};
 memcpy(header,"RIFF",4);memcpy(header+8,"WAVEfmt ",8);memcpy(header+36,"data",4);
 auto le32=[&](int o,uint32_t v){for(int i=0;i<4;i++)header[o+i]=(v>>(8*i))&255;};
 auto le16=[&](int o,uint16_t v){header[o]=v&255;header[o+1]=v>>8;};
 le32(4,36+bytes);le32(16,16);le16(20,1);le16(22,1);le32(24,24000);le32(28,48000);le16(32,2);le16(34,16);le32(40,bytes);
 bool ok=out.write(header,sizeof(header))==sizeof(header);
 int16_t chunk[480];
 // Discard old I2S input before starting this recording.
 for(int i=0;ok&&i<10;i++)ok=audioBus.readBytes((char*)chunk,sizeof(chunk))==sizeof(chunk);
 for(uint32_t pos=0;ok&&pos<bytes;pos+=sizeof(chunk)){
  ok=audioBus.readBytes((char*)chunk,sizeof(chunk))==sizeof(chunk);
  if(ok)ok=out.write((uint8_t*)chunk,sizeof(chunk))==sizeof(chunk);
 }
 out.flush();out.close();
 File check=SD_MMC.open(path,FILE_READ);
 ok=ok&&check&&check.size()==44+bytes;if(check)check.close();
 if(!ok){SD_MMC.remove(path);micResult="RECORD / SAVE FAILED";return false;}
 micPath=path;micResult="SAVED TO TF CARD";return true;
}
