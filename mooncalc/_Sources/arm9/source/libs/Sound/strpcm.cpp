
#include <NDS.h>
//#include <NDS/ARM9/CP15.h>

#include "_console.h"
#include "_consoleWriteLog.h"
#include "_const.h"

#include <stdio.h>
#include <stdlib.h>

#include "memtool.h"

#include "ipc6.h"

#include "arm9tcm.h"
#include "maindef.h"
#include "procstate.h"
#include "setarm9_reg_waitcr.h"

#include "strpcm.h"

DATA_IN_DTCM volatile bool VBlankPassedFlag;
DATA_IN_DTCM volatile u32 VBlankPassedCount;

DATA_IN_DTCM static int strpcmAudioVolume64,strpcmVideoVolume64;

DATA_IN_DTCM volatile bool strpcmRequestStop=false;

DATA_IN_DTCM volatile bool strpcmRingEmptyFlag;
DATA_IN_DTCM volatile u32 strpcmRingBufReadIndex;
DATA_IN_DTCM volatile u32 strpcmRingBufWriteIndex;

DATA_IN_DTCM u32 *strpcmRingLRBuf=NULL;

DATA_IN_DTCM bool strpcm_ExclusivePause=false;

DATA_IN_DTCM bool strpcm_UseLoopOnOverflow=true;

static __attribute__ ((noinline)) void strpcmUpdate(void);

#include "strpcm_ARM7_SelfCheck.h"

static volatile bool VBlank_AutoFlip=false;

__attribute__ ((noinline)) void IlligalWritedNullPointer_Check(u32 state)
{
  u64 *p=(u64*)NULL;
  if(*p!=NULLSTR64bit){
    _consolePrintf("0x%08x: ",NULL);
    u8 *p8=(u8*)p;
    for(u32 idx=0;idx<16;idx++){
      _consolePrintf("0x%02x,",p8[idx]);
    }
    _consolePrintf("\n");
    StopFatalError(14004,"Illigal writed on NULL pointer. (%d)",state);
  }
}

static CODE_IN_ITCM void InterruptHandler_VBlank(void)
{
  ARM7_SelfCheck_Check();
  
  {
    u64 *p=(u64*)NULL;
    if(*p!=NULLSTR64bit) IlligalWritedNullPointer_Check((u32)-1);
  }
  
  VBlankPassedFlag=true;
  VBlankPassedCount++;
  
  CallBack_ExecuteVBlankHandler();
  
  if(VBlank_AutoFlip==true) pScreenMain->FlipForVSyncAuto();
}

static void InterruptHandler_GBACART(void)
{
  IPC6->LCDPowerControl=LCDPC_SOFT_POWEROFF;
}

// ------------------------------------------

static CODE_IN_ITCM void InterruptHandler_IPC_SYNC(void)
{
//  _consolePrintf("CallIPC(%d)\n",IPC6->IR);
  switch(IPC6->IR){
    case IR_NULL: {
    } break;
    case IR_NextSoundData: {
      strpcmUpdate();
      
      const u32 Samples=IPC6->strpcmSamples;
      const u32 Channels=IPC6->strpcmChannels;
      
      DCache_CleanRangeOverrun(IPC6->strpcmLRBuf,Samples*2*2);
      
      IPC6->strpcmWriteRequest=0;
    } break;
  }
  IPC6->IR=IR_NULL;
}

extern "C" {
extern struct IntTable irqTable[16];
}

#include "__debug.h"

void InitInterrupts(void)
{
  REG_IME = 0;
  
  VBlankPassedFlag=false;
  VBlankPassedCount=0;
  
  irqInit();
  
  irqSet_u32(IRQ_VBLANK,(u32)InterruptHandler_VBlank);
  irqEnable(IRQ_VBLANK);
  
  irqSet_u32(IRQ_IPC_SYNC,(u32)InterruptHandler_IPC_SYNC);

#ifdef InternalDebugMode
  irqSet_u32(IRQ_CART,(u32)InterruptHandler_GBACART);
#endif
  
  REG_IPC_SYNC=IPC_SYNC_IRQ_ENABLE;
  
  if(VerboseDebugLog==true){
    _consolePrint("IRQ jump table.\n");
    u32 *p=(u32*)irqTable;
    while(1){
      if(p[1]==0) break;
      _consolePrintf("adr=0x%x trig=%x\n",p[0],p[1]);
      p+=2;
    }
    _consolePrint("----------\n");
  }
  
  REG_IME = 1;
}

void strpcmStart(bool FastStart,u32 SampleRate,u32 SamplePerBuf,u32 ChannelCount,EstrpcmFormat strpcmFormat)
{
#ifdef notuseSound
  return;
#endif
  
  _consolePrintf("strpcmStart: FastStart=%d, SampleRate=%dHz, SamplePerBuf=%dsmps, ChannelCount=%dchs.\n",FastStart,SampleRate,SamplePerBuf,ChannelCount);
  
  u32 StandbyTimeout=0x800000;
  while(IPC6->strpcmControl!=ESC_NOP){
    StandbyTimeout--;
    if(StandbyTimeout==0) StopFatalError(0,"strpcmStart: ARM7 standby none.\n");
    ARM7_SelfCheck_Check();
  }
  
  switch(strpcmFormat){
    case SPF_PCMx1: _consolePrintf("strpcm: set format SPF_PCMx1.\n"); break;
    default: StopFatalError(14001,"strpcm unknown format. (%d)\n",strpcmFormat); break;
  }
  
  if((SampleRate==0)||(SamplePerBuf==0)||(ChannelCount==0)) StopFatalError(14002,"strpcmStart: Driver setting error.\n");
  
  switch(strpcmFormat){
    case SPF_PCMx1: {
      strpcmRequestStop=false;
      
      u32 Samples=SamplePerBuf;
      u32 RingSamples=Samples*strpcmRingBufCount;
      
      strpcmRingEmptyFlag=false;
      strpcmRingBufReadIndex=0;
      if(FastStart==false){
        strpcmRingBufWriteIndex=strpcmRingBufCount-1;
        }else{
        strpcmRingBufWriteIndex=1;
      }
      
      strpcmRingLRBuf=(u32*)safemalloc_chkmem(&MM_SystemAfter,RingSamples*2*2);
      
      MemSet32CPU(0,strpcmRingLRBuf,RingSamples*2*2);
      
      IPC6->strpcmFreq=SampleRate;
      IPC6->strpcmSamples=Samples;
      IPC6->strpcmChannels=ChannelCount;
      IPC6->strpcmFormat=strpcmFormat;
      
      // ------
      
      IPC6->strpcmLRBuf=NULL;
    } break;
  }
  
  // ------
  
  IPC6->strpcmControl=ESC_Play;
  
  u32 PlayTimeout=0x800000;
  while(IPC6->strpcmControl!=ESC_NOP){
    PlayTimeout--;
    if(PlayTimeout==0) StopFatalError(0,"strpcmStart: ARM7 ESC_Play response none.\n");
    ARM7_SelfCheck_Check();
  }
  
  _consolePrintf("ESC_Play: ARM7 free mem %dbyte.\n",IPC6->strpcm_ARM7FreeMemSize);
}

void strpcmStop(void)
{
#ifdef notuseSound
  return;
#endif
  
  strpcmRequestStop=false;
  
  if(VerboseDebugLog==true) _consolePrint("Wait for terminate. (0)\n");
  while(IPC6->strpcmControl!=ESC_NOP){
    ARM7_SelfCheck_Check();
  }
  
  IPC6->strpcmControl=ESC_Stop;
  
  if(VerboseDebugLog==true) _consolePrint("Wait for terminate. (1)\n");
  while(IPC6->strpcmControl!=ESC_NOP){
    ARM7_SelfCheck_Check();
  }
  
  if(VerboseDebugLog==true) _consolePrint("ARM7 strpcm terminated.\n");
  
  EstrpcmFormat strpcmFormat=IPC6->strpcmFormat;
  
  switch(strpcmFormat){
    case SPF_PCMx1: {
      strpcmRingEmptyFlag=false;
      strpcmRingBufReadIndex=0;
      strpcmRingBufWriteIndex=0;
      
      if(strpcmRingLRBuf!=NULL){
        safefree(&MM_SystemAfter,strpcmRingLRBuf); strpcmRingLRBuf=NULL;
      }
      
      IPC6->strpcmFreq=0;
      IPC6->strpcmSamples=0;
      IPC6->strpcmChannels=0;
      IPC6->strpcmFormat=SPF_DWORD;
      
/*
      if(IPC6->strpcmLBuf!=NULL){
        safefree(IPC6->strpcmLBuf); IPC6->strpcmLBuf=NULL;
      }
      if(IPC6->strpcmRBuf!=NULL){
        safefree(IPC6->strpcmRBuf); IPC6->strpcmRBuf=NULL;
      }
*/
      IPC6->strpcmLRBuf=NULL;
    } break;
  }
  
  _consolePrint("strpcm stopped.\n");
}

// ----------------------------------------------

static __attribute__ ((noinline)) void strpcmUpdate(void)
{
#ifdef notuseSound
  strpcmRingBufReadIndex=(strpcmRingBufReadIndex+1) & strpcmRingBufBitMask;
  return;
#endif
  
  if(strpcm_ExclusivePause==true){
    IPC6->strpcmLRBuf=NULL;
    return;
  }
  
  u32 Samples=IPC6->strpcmSamples;
  const u32 Channels=IPC6->strpcmChannels;
  
  if(strpcmRingLRBuf==NULL) StopFatalError(14003,"strpcmUpdate: strpcmRingLRBuf is NULL.\n");
  
  bool IgnoreFlag=false;
  
  u32 CurIndex=(strpcmRingBufReadIndex+1) & strpcmRingBufBitMask;
  
  if(CurIndex==strpcmRingBufWriteIndex){
    strpcmRingEmptyFlag=true;
    IgnoreFlag=true;
  }
  
  if(strpcm_UseLoopOnOverflow==true){
    if(IgnoreFlag==true) CurIndex=(strpcmRingBufWriteIndex+1) & strpcmRingBufBitMask;
    IPC6->strpcmLRBuf=&strpcmRingLRBuf[Samples*CurIndex];
    strpcmRingBufReadIndex=CurIndex;
    }else{
    if(IgnoreFlag==true){
      IPC6->strpcmLRBuf=NULL;
      }else{
      IPC6->strpcmLRBuf=&strpcmRingLRBuf[Samples*CurIndex];
      strpcmRingBufReadIndex=CurIndex;
    }
  }
}

void strpcmSetAudioVolume64(int v)
{
  _consolePrintf("strpcmSetAudioVolume64(%d);\n",v);
  
  if(v<0) v=0;
  if(128<v) v=128;
  
  strpcmAudioVolume64=v;
  
  IPC6->strpcmAudioVolume64=strpcmAudioVolume64;
}

int strpcmGetAudioVolume64(void)
{
  return(strpcmAudioVolume64);
}

// ----------------------------------------------------------------------------------

