
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_consoleWriteLog.h"
#include "_const.h"
#include "memtool.h"
#include "shell.h"
#include "inifile.h"
#include "arm9tcm.h"

#include "sndeff.h"

#include "procstate.h"

#include "ipc6.h"
#include "strpcm.h"

typedef struct {
  u32 ofs,smpcnt;
  u16 chs,smprate;
} TWaveHeader;

static FAT_FILE *pWaveFile;
static TWaveHeader *pWaveHeader;

typedef struct {
  EWAVFN WAVFN;
  u32 Freq;
  u32 BufCount;
  u32 Channels;
  u8 *lbuf,*rbuf;
} TSound;

static TSound SoundSE;
static TSound SoundBGM;

typedef struct {
  u32 Remain;
  u32 lbufofs;
  u32 rbufofs;
  u8 *plbuf,*prbuf;
} TStreamLoader;

static TStreamLoader StreamLoader;

typedef struct {
  bool Enabled;
  u32 Volume;
  bool RequestStartSE;
  float VBlankCount;
  float VBlankLoop;
} TSETiming;

static volatile TSETiming SETiming={false,false,0,0};

// #include "sndeff_WaveFile.h"

static void Sound_Free(TSound *pSound,bool isBGM)
{
  if(isBGM==false) SETiming.Enabled=false;
  
  FIFO_TX32(FIFOCMD_ToARM7_StopSoundBlock);
  FIFO_TX32(isBGM);
  
  pSound->WAVFN=(EWAVFN)-1;
  
  pSound->Freq=0;
  pSound->BufCount=0;
  pSound->Channels=0;
  
  if(pSound->lbuf!=NULL){
    safefree(&MM_System,pSound->lbuf); pSound->lbuf=NULL;
  }
  if(pSound->rbuf!=NULL){
    safefree(&MM_System,pSound->rbuf); pSound->rbuf=NULL;
  }
  
  if(isBGM==true){
    TStreamLoader *psl=&StreamLoader;
    psl->Remain=0;
    psl->lbufofs=0;
    psl->rbufofs=0;
    psl->plbuf=NULL;
    psl->prbuf=NULL;
  }
}

void Sound_Open(void)
{
  {
    TSound *pSound=&SoundSE;
    pSound->WAVFN=(EWAVFN)-1;
    pSound->Freq=0;
    pSound->BufCount=0;
    pSound->Channels=0;
    pSound->lbuf=NULL;
    pSound->rbuf=NULL;
  }
  
  {
    TSound *pSound=&SoundBGM;
    pSound->WAVFN=(EWAVFN)-1;
    pSound->Freq=0;
    pSound->BufCount=0;
    pSound->Channels=0;
    pSound->lbuf=NULL;
    pSound->rbuf=NULL;
  }
  
  {
    TStreamLoader *psl=&StreamLoader;
    psl->Remain=0;
    psl->lbufofs=0;
    psl->rbufofs=0;
    psl->plbuf=NULL;
    psl->prbuf=NULL;
  }
  
  SETiming.Enabled=false;

  pWaveFile=NULL;
  pWaveHeader=NULL;
  
  pWaveFile=Shell_FAT_fopen_Internal_WithCheckExists(SndEffDatFilename);
  if(pWaveFile==NULL) return;
  
  u32 cnt;
  FAT2_fread_fast(&cnt,4,1,pWaveFile);
  pWaveHeader=(TWaveHeader*)safemalloc_chkmem(&MM_System,sizeof(TWaveHeader)*cnt);
  FAT2_fread_fast(pWaveHeader,sizeof(TWaveHeader),cnt,pWaveFile);
  
  if(VerboseDebugLog==true) _consolePrintf("Wave header loaded. (%d,%d)\n",sizeof(TWaveHeader),cnt);
}

void Sound_Close(void)
{
  Sound_Free(&SoundSE,false);
  Sound_Free(&SoundBGM,true);
  
  if(pWaveFile!=NULL){
    FAT2_fclose(pWaveFile); pWaveFile=NULL;
  }
  
  if(pWaveHeader!=NULL){
    safefree(&MM_System,pWaveHeader); pWaveHeader=NULL;
  }
}

static void PlaySoundBlock(TSound *pSound,u32 Volume,bool isBGM,bool isLoop)
{
  u8 *lbuf,*rbuf;
  
  switch(pSound->Channels){
    case 1: {
      lbuf=pSound->lbuf;
      rbuf=pSound->lbuf;
    } break;
    case 2: {
      lbuf=pSound->lbuf;
      rbuf=pSound->rbuf;
      Volume/=2;
    } break;
    default: StopFatalError(13802,"sound effect illigal channels count.\n"); break;
  }
  
  if(127<Volume) Volume=127;
  
  FIFO_TX32(FIFOCMD_ToARM7_PlaySoundBlock);
  FIFO_TX32(isBGM);
  FIFO_TX32(isLoop);
  FIFO_TX32(pSound->Freq);
  FIFO_TX32((u32)lbuf);
  FIFO_TX32((u32)rbuf);
  FIFO_TX32(pSound->BufCount);
  FIFO_TX32(Volume);
  FIFO_TX32(1);
}

static void Sound_Start_ins(TSound *pSound,EWAVFN WAVFN,u32 Volume,bool isBGM,bool isLoop)
{
  if((pWaveFile==NULL)||(pWaveHeader==NULL)) return;
  
  if(ProcState.System.ClickSound==false) return;
  
  if(pSound->WAVFN!=WAVFN){
    SETiming.RequestStartSE=false;
    
    Sound_Free(pSound,isBGM);
    
    TWaveHeader *pwh=&pWaveHeader[(u32)WAVFN];
    if(VerboseDebugLog==true) _consolePrintf("SE: ID:%d, %dHz, %dsmps, %dchs. ofs=%dbyte.\n",WAVFN,pwh->smprate,pwh->smpcnt,pwh->chs,pwh->ofs);
    
    pSound->Freq=pwh->smprate;
    pSound->Channels=pwh->chs;
    pSound->BufCount=pwh->smpcnt;
    
    if(isBGM==false){
      FAT2_fseek(pWaveFile,pwh->ofs,SEEK_SET);
      pSound->lbuf=(u8*)safemalloc_chkmem(&MM_System,pwh->smpcnt);
      FAT2_fread_fast(pSound->lbuf,1,pwh->smpcnt,pWaveFile);
      if(pwh->chs==2){
        pSound->rbuf=(u8*)safemalloc_chkmem(&MM_System,pwh->smpcnt);
        FAT2_fread_fast(pSound->rbuf,1,pwh->smpcnt,pWaveFile);
      }
      }else{
      TStreamLoader *psl=&StreamLoader;
      psl->Remain=pwh->smpcnt;
      psl->lbufofs=pwh->ofs;
      pSound->lbuf=(u8*)safemalloc_chkmem(&MM_System,pwh->smpcnt);
      psl->plbuf=pSound->lbuf;
      psl->rbufofs=0;
      psl->prbuf=NULL;
      if(pwh->chs==2){
        psl->rbufofs=psl->lbufofs+psl->Remain;
        pSound->rbuf=(u8*)safemalloc_chkmem(&MM_System,pwh->smpcnt);
        psl->prbuf=pSound->rbuf;
      }
      SoundSE_MainVBlankHandler();
    }
    
    pSound->WAVFN=WAVFN;
  }
  
/*
  // Quantize SE.
  if((SETiming.Enabled==true)&&(isBGM==false)){
    SETiming.Volume=Volume;
    SETiming.RequestStartSE=true;
    return;
  }
*/
  
  PlaySoundBlock(pSound,Volume,isBGM,isLoop);
  
  if(isBGM==true){
    SETiming.Enabled=false;
    SETiming.RequestStartSE=false;
    SETiming.VBlankCount=0;
    float loop=SoundBGM.BufCount;
    loop=loop*59.827/SoundBGM.Freq/(16*16);
    SETiming.VBlankLoop=loop;
    SETiming.Enabled=true;
   }
}

void SoundSE_Start(EWAVFN WAVFN)
{
  u32 Volume=GetSEVolume128();
  if(WAVFN==WAVFN_CurMove) Volume/=4;
  if(WAVFN==WAVFN_Erase) Volume/=2;
  if(WAVFN==WAVFN_AllClear) Volume/=2;
  Sound_Start_ins(&SoundSE,WAVFN,Volume,false,false);
}

void SoundBGM_Start(EWAVFN WAVFN,bool isLoop)
{
//  return;
  
  Sound_Free(&SoundSE,false); // Stop SE.
  
  u32 Volume=GetBGMVolume128();
  Sound_Start_ins(&SoundBGM,WAVFN,Volume,true,isLoop);
}

u32 SoundSE_GetCurrentPlayTimePerVsync(void)
{
  u32 freq=SoundSE.Freq;
  u32 samples=SoundSE.BufCount*1;
  
  return(samples*60/freq);
}

u32 SoundBGM_GetCurrentPlayTimePerVsync(void)
{
  u32 freq=SoundBGM.Freq;
  u32 samples=SoundBGM.BufCount*1;
  
  return(samples*60/freq);
}

void SoundSE_Stop(void)
{
  Sound_Free(&SoundSE,false);
}

void SoundBGM_Stop(void)
{
  Sound_Free(&SoundBGM,true);
}

void SoundSE_IRQVBlankHandler(void)
{
  if(SETiming.Enabled==false) return;
  SETiming.VBlankCount++;
  
  if(SETiming.VBlankCount<=(SETiming.VBlankLoop/4)){
    if(SETiming.RequestStartSE==true){
      SETiming.RequestStartSE=false;
      PlaySoundBlock(&SoundSE,SETiming.Volume,false,false);
    }
  }
  
  if(SETiming.VBlankLoop<=SETiming.VBlankCount){
    SETiming.VBlankCount-=SETiming.VBlankLoop;
    if(SETiming.RequestStartSE==true){
      SETiming.RequestStartSE=false;
      PlaySoundBlock(&SoundSE,SETiming.Volume,false,false);
    }
  }
}

void SoundSE_MainVBlankHandler(void)
{
  TStreamLoader *psl=&StreamLoader;
  if(psl->Remain==0) return;
  
  u32 BlockSize=psl->Remain;
  const u32 MaxSize=8*1024;
  if(MaxSize<BlockSize) BlockSize=MaxSize;
//  if(VerboseDebugLog==true) _consolePrintf("StreamBGM: Load %dbytes.\n",BlockSize);
  psl->Remain-=BlockSize;
  
  if(psl->plbuf!=NULL){
    FAT2_fseek(pWaveFile,psl->lbufofs,SEEK_SET);
    psl->lbufofs+=BlockSize;
    if((BlockSize&3)==0){
      FAT2_fread_fast(psl->plbuf,1,BlockSize,pWaveFile);
      }else{
      FAT2_fread(psl->plbuf,1,BlockSize,pWaveFile);
    }
    psl->plbuf+=BlockSize;
  }
  
  if(psl->prbuf!=NULL){
    FAT2_fseek(pWaveFile,psl->rbufofs,SEEK_SET);
    psl->rbufofs+=BlockSize;
    if((BlockSize&3)==0){
      FAT2_fread_fast(psl->prbuf,1,BlockSize,pWaveFile);
      }else{
      FAT2_fread(psl->prbuf,1,BlockSize,pWaveFile);
    }
    psl->prbuf+=BlockSize;
  }
}

