#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nds.h>

#include "_console.h"

#include "ipc6.h"
#include "memtoolARM7.h"
#include "a7sleep.h"

static inline void ShowLogHalt(void)
{
  while(1);
}

#include "main_ARM7_SelfCheck.h"

//---------------------------------------------------------------------------------

static inline u32 strpcmGetAudioVolume64(void)
{
//  return(64);
  return(IPC6->strpcmAudioVolume64);
}

//---------------------------------------------------------------------------------
static __attribute__((noinline)) void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format, bool isLoop) {
//---------------------------------------------------------------------------------
	if((data==NULL)||(bytes==0)) return;
	
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2 ;
	SCHANNEL_REPEAT_POINT(channel) = 0;
	if(isLoop==true){
		SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
		}else{
		SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
	}
}


static __attribute__((noinline)) s32 getFreeBGMChannel() {
  return(4);
}

static __attribute__((noinline)) void stopBGM() {
  int i=4;
  SCHANNEL_CR(i+0)     = 0;
  SCHANNEL_CR(i+1)     = 0;
}

//---------------------------------------------------------------------------------
static __attribute__((noinline)) s32 getFreeSoundChannel() {
//---------------------------------------------------------------------------------
	int i;
	for (i=6; i<16; i++) {
		if ( (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 ) return i;
	}
	return -1;
}

//---------------------------------------------------------------------------------
static __attribute__((noinline)) void stopSound() {
//---------------------------------------------------------------------------------
	int i;
	for (i=6; i<16; i++) {
		SCHANNEL_CR(i)     = 0;
	}
}

//////////////////////////////////////////////////////////////////////

static __attribute__((noinline)) void CallBackIRQ_strpcmUpdate(void);

// ---------------------------

static u32 strpcmCursorFlag=0;

static u32 strpcmFreq,strpcmSamples,strpcmChannels;
static EstrpcmFormat strpcmFormat=SPF_DWORD;

static s16 *strpcmL0=NULL,*strpcmL1=NULL,*strpcmR0=NULL,*strpcmR1=NULL;

#undef SOUND_FREQ
#define SOUND_FREQ(n)	(0x10000 - (16777216 / (n)))

static inline void strpcmPlay()
{
  _consolePrintf("strpcmPlay();\n");
  
  IPC6->IR=IR_NULL;
  
  strpcmCursorFlag=0;
  
  strpcmFormat=IPC6->strpcmFormat;
  
  switch(strpcmFormat){
    case SPF_PCMx1: SetMemoryMode(false); break;
    default: {
      _consolePrintf("Unknown strpcm format.\n");
      ShowLogHalt();
    }
  }
  
  switch(strpcmFormat){
    case SPF_PCMx1: {
    } break;
    default: {
      _consolePrintf("Unknown strpcm format.\n");
      ShowLogHalt();
    } break;
  }
  
  strpcmFreq=IPC6->strpcmFreq;
  strpcmSamples=IPC6->strpcmSamples;
  strpcmChannels=IPC6->strpcmChannels;
  
  switch(strpcmFormat){
    case SPF_PCMx1:  break;
  }
  
  _consolePrintf("strpcm freq=%d samples=%d chs=%d\n",strpcmFreq,strpcmSamples,strpcmChannels);
  
  strpcmL0=(s16*)safemalloc(strpcmSamples*2);
  strpcmL1=(s16*)safemalloc(strpcmSamples*2);
  strpcmR0=(s16*)safemalloc(strpcmSamples*2);
  strpcmR1=(s16*)safemalloc(strpcmSamples*2);
  
  _consolePrintf("strpcm buf 0x%x,0x%x 0x%x,0x%x\n",strpcmL0,strpcmL1,strpcmR0,strpcmR1);
  
  if((strpcmL0==NULL)||(strpcmL1==NULL)||(strpcmR0==NULL)||(strpcmR1==NULL)){
    a7led(3); while(1);
  }
  
//  powerON(POWER_SOUND);
//  SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
  SCHANNEL_CR(0) = 0;
  SCHANNEL_CR(1) = 0;
  SCHANNEL_CR(2) = 0;
  SCHANNEL_CR(3) = 0;
  
  TIMER0_DATA = SOUND_FREQ(strpcmFreq);
  TIMER0_CR = TIMER_DIV_1 | TIMER_ENABLE;
  
  TIMER1_DATA = 0x10000 - (strpcmSamples*2);
  TIMER1_CR = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
  
  for(u32 ch=0;ch<4;ch++){
    SCHANNEL_CR(ch) = 0;
    SCHANNEL_TIMER(ch) = SOUND_FREQ(strpcmFreq);
    SCHANNEL_LENGTH(ch) = (strpcmSamples*2) >> 2;
    SCHANNEL_REPEAT_POINT(ch) = 0;
  }

  IPC6->strpcmWriteRequest=0;
  
  _consolePrintf("strpcm initialized.\n");
}

static __attribute__((noinline)) void strpcmStop(void)
{
  _consolePrintf("strpcm Stop: strpcm buf 0x%x,0x%x 0x%x,0x%x\n",strpcmL0,strpcmL1,strpcmR0,strpcmR1);
  
//  powerOFF(POWER_SOUND);
//  SOUND_CR = 0;
  TIMER0_CR = 0;
  TIMER1_CR = 0;
  
  for(u32 ch=0;ch<4;ch++){
    SCHANNEL_CR(ch) = 0;
  }
  
  if(strpcmL0!=NULL){
    safefree(strpcmL0); strpcmL0=NULL;
  }
  if(strpcmL1!=NULL){
    safefree(strpcmL1); strpcmL1=NULL;
  }
  if(strpcmR0!=NULL){
    safefree(strpcmR0); strpcmR0=NULL;
  }
  if(strpcmR1!=NULL){
    safefree(strpcmR1); strpcmR1=NULL;
  }
  
  switch(strpcmFormat){
    case SPF_PCMx1: break;
    default: break;
  }
  strpcmFormat=SPF_DWORD;
  ARM7_SelfCheck_Check();
  
  SetMemoryMode_End();
  
  IPC6->IR=IR_NULL;
  
  _consolePrintf("strpcm Stopped.\n");
}

//////////////////////////////////////////////////////////////////////

#define MAX( x, y ) ( ( x > y ) ? x : y )
#define MIN( x, y ) ( ( x < y ) ? x : y )

static __attribute__((noinline)) void InterruptHandler_Timer1_SetSwapChannel(void)
{
  s16 *lbuf,*rbuf;
  
  if(strpcmCursorFlag==0){
    lbuf=strpcmL0;
    rbuf=strpcmR0;
    }else{
    lbuf=strpcmL1;
    rbuf=strpcmR1;
  }
  
  u32 channel=strpcmCursorFlag;
  
  u32 chcnt=IPC6->SoundChannels;
  
  if((chcnt==0)||(chcnt==1)){
  // Left channel
  SCHANNEL_CR(channel) = 0;
  SCHANNEL_SOURCE(channel) = (uint32)lbuf;
  SCHANNEL_CR(channel) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0) | SOUND_16BIT;
  }
  
  channel+=2;
  
  if((chcnt==0)||(chcnt==2)){
  // Right channel
  SCHANNEL_CR(channel) = 0;
  SCHANNEL_SOURCE(channel) = (uint32)rbuf;
  SCHANNEL_CR(channel) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0x7F) | SOUND_16BIT;
  }
  
  strpcmCursorFlag=1-strpcmCursorFlag;
}

// --------------------------------------------------------

asm void VolNoAGC(s16 *lbuf,s16 *rbuf,u32 count,s32 vol)
{
VolNoAGC_lbuf RN r0
VolNoAGC_rbuf RN r1
VolNoAGC_count RN r2
VolNoAGC_vol RN r3

VolNoAGC_tmpl RN r4
VolNoAGC_tmpr RN lr
VolNoAGC_limmin RN r5
VolNoAGC_limmax RN r6

  PUSH {r4,r5,r6,lr}

  ldr VolNoAGC_limmin,=-32768
  ldr VolNoAGC_limmax,=32767
  
VolNoAGC_loop
  ldrsh VolNoAGC_tmpl,[VolNoAGC_lbuf]
  ldrsh VolNoAGC_tmpr,[VolNoAGC_rbuf]
  mul VolNoAGC_tmpl,VolNoAGC_tmpl,VolNoAGC_vol
  mul VolNoAGC_tmpr,VolNoAGC_tmpr,VolNoAGC_vol
  asr VolNoAGC_tmpl,VolNoAGC_tmpl,#11 ; /2048
  asr VolNoAGC_tmpr,VolNoAGC_tmpr,#11 ; /2048
  cmps VolNoAGC_tmpl,VolNoAGC_limmin
  movlt VolNoAGC_tmpl,VolNoAGC_limmin
  cmps VolNoAGC_tmpl,VolNoAGC_limmax
  movgt VolNoAGC_tmpl,VolNoAGC_limmax
  cmps VolNoAGC_tmpr,VolNoAGC_limmin
  movlt VolNoAGC_tmpr,VolNoAGC_limmin
  cmps VolNoAGC_tmpr,VolNoAGC_limmax
  movgt VolNoAGC_tmpr,VolNoAGC_limmax
  strh VolNoAGC_tmpl,[VolNoAGC_lbuf],#2
  subs VolNoAGC_count,#1
  strh VolNoAGC_tmpr,[VolNoAGC_rbuf],#2
  bne VolNoAGC_loop
  
  POP {r4,r5,r6,pc}
}

asm u32 VolUseAGC(s16 *lbuf,s16 *rbuf,u32 count,s32 vol)
{
VolUseAGC_lbuf RN r0
VolUseAGC_rbuf RN r1
VolUseAGC_count RN r2
VolUseAGC_vol RN r3

VolUseAGC_tmpl RN r4
VolUseAGC_tmpr RN lr
VolUseAGC_limmin RN r5
VolUseAGC_limmax RN r6

VolUseAGC_overflag RN r7

  PUSH {r4,r5,r6,r7,lr}

  ldr VolUseAGC_limmin,=-32768
  ldr VolUseAGC_limmax,=32767
  
  mov VolUseAGC_overflag,#0

VolUseAGC_loop
  ldrsh VolUseAGC_tmpl,[VolUseAGC_lbuf]
  ldrsh VolUseAGC_tmpr,[VolUseAGC_rbuf]
  mul VolUseAGC_tmpl,VolUseAGC_tmpl,VolUseAGC_vol
  mul VolUseAGC_tmpr,VolUseAGC_tmpr,VolUseAGC_vol
  asr VolUseAGC_tmpl,VolUseAGC_tmpl,#11 ; /2048
  asr VolUseAGC_tmpr,VolUseAGC_tmpr,#11 ; /2048
  cmps VolUseAGC_tmpl,VolUseAGC_limmin
  movlt VolUseAGC_tmpl,VolUseAGC_limmin
  movlt VolUseAGC_overflag,#1
  cmps VolUseAGC_tmpl,VolUseAGC_limmax
  movgt VolUseAGC_tmpl,VolUseAGC_limmax
  movgt VolUseAGC_overflag,#1
  cmps VolUseAGC_tmpr,VolUseAGC_limmin
  movlt VolUseAGC_tmpr,VolUseAGC_limmin
  movlt VolUseAGC_overflag,#1
  cmps VolUseAGC_tmpr,VolUseAGC_limmax
  movgt VolUseAGC_tmpr,VolUseAGC_limmax
  movgt VolUseAGC_overflag,#1
  strh VolUseAGC_tmpl,[VolUseAGC_lbuf],#2
  subs VolUseAGC_count,#1
  strh VolUseAGC_tmpr,[VolUseAGC_rbuf],#2
  bne VolUseAGC_loop
  
  mov r0,VolUseAGC_overflag
  
  POP {r4,r5,r6,r7,pc}
}

  static const s32 LogVolumeCount=128;
  static const s32 LogVolume[LogVolumeCount]={
  0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,480,
512,544,576,608,640,672,704,736,768,800,832,864,896,928,960,992,
1024,1056,1088,1120,1152,1184,1216,1248,1280,1312,1344,1376,1408,1440,1472,1504,
1536,1568,1600,1632,1664,1696,1728,1760,1792,1824,1856,1888,1920,1952,1984,2016,
2048,2123,2202,2283,2368,2455,2546,2641,2738,2840,2945,3054,3167,3284,3405,3531,
3662,3798,3938,4084,4235,4392,4554,4723,4898,5079,5267,5462,5664,5873,6091,6316,
6550,6792,7043,7304,7574,7854,8145,8446,8759,9083,9419,9768,10129,10504,10893,11296,
11714,12147,12597,13063,13546,14047,14567,15106,15665,16244,16846,17469,18115,18785,19481,20201,};

static __attribute__((noinline)) void InterruptHandler_Timer1_ApplyVolume(s16 *lbuf,s16 *rbuf,u32 count,u32 Volume64)
{
  if((lbuf==NULL)||(rbuf==NULL)) return;
  
  s32 vol=(s32)Volume64;
  if((LogVolumeCount-1)<vol) vol=LogVolumeCount-1;
  
  if(vol==0){
    for(u32 idx=0;idx<count;idx++){
      lbuf[idx]=0;
      rbuf[idx]=0;
    }
    return;
  }
  
  u32 MasterVolume=LogVolume[vol];
  
  VolNoAGC(lbuf,rbuf,count,MasterVolume);
  ARM7_SelfCheck_Check();
}

// --------------------------------------------------------------------

static void InterruptHandler_Timer1_Null(void)
{
}

static void InterruptHandler_Timer1_PCMx1(void)
{
  InterruptHandler_Timer1_SetSwapChannel();
  
  s16 *lbuf,*rbuf;
  
  if(strpcmCursorFlag==0){
    lbuf=strpcmL0;
    rbuf=strpcmR0;
    }else{
    lbuf=strpcmL1;
    rbuf=strpcmR1;
  }
  
  u32 Samples=strpcmSamples;
  
  if(IPC6->strpcmWriteRequest!=0){
    MemSet16DMA3(0,lbuf,Samples*2);
    MemSet16DMA3(0,rbuf,Samples*2);
    }else{
    u32 *plrsrc=IPC6->strpcmLRBuf;
    if(plrsrc==NULL){
      MemSet16DMA3(0,lbuf,Samples*2);
      MemSet16DMA3(0,rbuf,Samples*2);
      }else{
      s16 *plbuf=lbuf,*prbuf=rbuf;
      for(u32 idx=0;idx<Samples;idx++){
        u32 lrsrc=*plrsrc++;
        *plbuf++=lrsrc&0xffff;
        *prbuf++=lrsrc>>16;
      }
    }
    
    IPC6->IR=IR_NextSoundData;
    IPC6->strpcmWriteRequest=1;
    REG_IPC_SYNC|=IPC_SYNC_IRQ_REQUEST;
    
    InterruptHandler_Timer1_ApplyVolume(lbuf,rbuf,Samples,strpcmGetAudioVolume64());
  }
  
  CallBackIRQ_strpcmUpdate();
}

//////////////////////////////////////////////////////////////////////

static u32 CSE_Volume;
static u32 CSE_Index,CSE_Count;
static u8 CSE_Data[5*60]; // max 5 secs.

static __attribute__((noinline)) void StartClickSE(volatile TClickSE *pClickSE)
{
  CSE_Volume=pClickSE->Volume;
  
  CSE_Index=0;
  CSE_Count=0;
  
  for(u32 idx=0;idx<pClickSE->Count;idx++){
    CSE_Data[CSE_Count++]=1;
    for(u32 w=0;w<4;w++){
      CSE_Data[CSE_Count++]=0;
    }
    CSE_Count++;
  }
  
  if(pClickSE->AddLong==true) CSE_Data[CSE_Count++]=2;
  
  pClickSE->Apply=false;
}

//////////////////////////////////////////////////////////////////////

static bool VsyncPassed;

#include "resources/snd_click_short_c_bin.h"
#include "resources/snd_click_long_c_bin.h"

static void InterruptHandler_VBlank(void)
{
  ARM7_SelfCheck_Check();
  
  IPC6->heartbeat++;
  
  VsyncPassed=true;
  
  if(CSE_Index==CSE_Count) return;
  
  u32 data=CSE_Data[CSE_Index++];
  u32 vol=0,addr=0,size=0;
  switch(data){
    case 0: break; // wait for vblank
    case 1: {
      vol=127;
      addr=(u32)snd_click_short_c_bin;
      size=snd_click_short_c_bin_Size;
    } break;
    case 2: {
      vol=0;
      vol=96;
      addr=(u32)snd_click_long_c_bin;
      size=snd_click_long_c_bin_Size;
    } break;
  }
  
  vol=(vol*CSE_Volume)/256;
  
  if((vol!=0)&&(addr!=0)&&(size!=0)){
    u32 ch=14;
    SCHANNEL_CR(ch)=0;
    SCHANNEL_TIMER(ch)  = SOUND_FREQ(32768);
    SCHANNEL_SOURCE(ch) = addr;
    SCHANNEL_LENGTH(ch) = size >> 2;
    SCHANNEL_REPEAT_POINT(ch) = 0;
    SCHANNEL_CR(ch)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(64) | SOUND_8BIT;
  }
}

//////////////////////////////////////////////////////////////////////

static inline void main_InitIRQ(void)
{
  REG_IME = 0;
  irqInit();
  irqSet_u32(IRQ_TIMER1,(u32)InterruptHandler_Timer1_Null);
  irqSet_u32(IRQ_VBLANK,(u32)InterruptHandler_VBlank);
  REG_IME = 1;
}

static inline void main_InitVsync(void)
{
  VsyncPassed=false;
}

#define PM_NDSLITE_ADR (4)
#define PM_NDSLITE_ISLITE BIT(6)
#define PM_NDSLITE_ExternalPowerPresent BIT(3)
#define PM_NDSLITE_BRIGHTNESS(x) ((x & 0x03)<<0)
#define PM_NDSLITE_BRIGHTNESS_MASK (PM_NDSLITE_BRIGHTNESS(3))

static bool isNDSLite;

//---------------------------------------------------------------------------------
static uint16 main_InitNDSL_touchRead(uint32 command) {
//---------------------------------------------------------------------------------
	uint16 result;
	SerialWaitBusy();

	// Write the command and wait for it to complete
	REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS; //0x0A01;
	REG_SPIDATA = command;
	SerialWaitBusy();

	// Write the second command and clock in part of the data
	REG_SPIDATA = 0;
	SerialWaitBusy();
	result = REG_SPIDATA;

	// Clock in the rest of the data (last transfer)
	REG_SPICNT = SPI_ENABLE | 0x201;
	REG_SPIDATA = 0;
	SerialWaitBusy();

	// Return the result
	return ((result & 0x7F) << 5) | (REG_SPIDATA >> 3);
}

static inline void main_InitNDSL(void)
{
  isNDSLite=false;
  IPC6->DefaultBrightness=0;
  IPC6->Brightness=0xff;
  
  u32 td1=main_InitNDSL_touchRead(TSC_MEASURE_TEMP1);
  u32 td2=main_InitNDSL_touchRead(TSC_MEASURE_TEMP2);
  if((td1==0x0fff)&&(td2==0x0fff)){
    IPC6->DSType=DST_DSi;
    return;
  }
  
  if( (PM_GetRegister(PM_NDSLITE_ADR) & PM_NDSLITE_ISLITE) == 0){
    IPC6->DSType=DST_DS;
    return;
  }
  
  IPC6->DSType=DST_DSLite;
  isNDSLite=true;
  
  u8 data;
  data=PM_GetRegister(PM_NDSLITE_ADR);
  data&=PM_NDSLITE_BRIGHTNESS_MASK;
  IPC6->DefaultBrightness=data;
}

static inline void main_InitSoundDevice(void)
{
  powerON(POWER_SOUND);
  SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
  
  REG_POWERCNT&=~POWER_UNKNOWN; // wifi power off
  
  swiChangeSoundBias(1,0x400);
  a7SetSoundAmplifier(true);
}

#include "main_readUserSettings.h"

static __attribute__((noinline)) void main_InitAll(void)
{
  IPC6->heartbeat=0;
  
  REG_IME=0;
  REG_IE=0;
  
  // Clear DMA
  for(int i=0;i<0x30/4;i++){
    *((vu32*)(0x40000B0+i))=0;
  }
  
  // Reset the clock if needed
  rtcReset();
  
  IPC6->RequestUpdateIPC=true;
  IPC6->curtimeFlag=true;
  
  IPC6->buttons=0;
  IPC6->PanelOpened=true;
  
  IPC6->LCDPowerControl=LCDPC_NOP;
  
  IPC6->strpcmControl=ESC_NOP;
  
  IPC6->IR=IR_NULL;
  
  IPC6->ClickSE.Apply=false;
  
  IPC6->RESET=0;
  
  REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
  libnds131_readUserSettings();
  REG_SPICNT = 0;
  
  IPC6->UserLanguage=PersonalData->_user_data.language;
  IPC6->BirthMonth=PersonalData->birthMonth;
  IPC6->BirthDay=PersonalData->birthDay;
  
  main_InitNDSL();
  
  main_InitVsync();
  
  main_InitIRQ();
  
  main_InitSoundDevice();
}

static bool strpcmPlayFlag;

static __attribute__((noinline)) void main_Proc_strpcmControl(void)
{
  _consolePrintf("main_Proc_strpcmControl();\nIPC6->strpcmControl=%d.\n",IPC6->strpcmControl);
  switch(IPC6->strpcmControl){
    case ESC_NOP: {
    } break;
    case ESC_Play: {
      strpcmPlay();
      switch(strpcmFormat){
        case SPF_PCMx1: irqSet_u32(IRQ_TIMER1,(u32)InterruptHandler_Timer1_PCMx1); break;
        default: irqSet_u32(IRQ_TIMER1,(u32)InterruptHandler_Timer1_Null); break;
      }
      strpcmPlayFlag=true;
    } break;
    case ESC_Stop: {
      strpcmPlayFlag=false;
      strpcmStop();
      irqSet_u32(IRQ_TIMER1,(u32)InterruptHandler_Timer1_Null);
    } break;
    default: {
      REG_IME=0;
      _consolePrintf("Unknown strpcmControl(%d).\n",IPC6->strpcmControl);
      ShowLogHalt();
    } break;
  }
  
  if(1){
    u32 freemem=0;
    const u32 FreeMemSeg=1*1024;
    
    for(u32 i=FreeMemSeg;i<64*1024;i+=FreeMemSeg){
      void *ptr=malloc(i);
      if(ptr==NULL){
        freemem=i-FreeMemSeg;
        break;
      }
      free(ptr); ptr=NULL;
    }
    _consolePrintf("FreeMem=%dbyte\n",freemem);
    IPC6->strpcm_ARM7FreeMemSize=freemem;
  }
  
  IPC6->strpcmControl=ESC_NOP;
  ARM7_SelfCheck_Check();
}

static __attribute__((noinline)) void main_Proc_Brightness(u32 Brightness)
{
  u8 data;
  
  data=PM_GetRegister(PM_NDSLITE_ADR);
  data&=~PM_NDSLITE_BRIGHTNESS_MASK;
  data|=PM_NDSLITE_BRIGHTNESS(Brightness);
  
  PM_SetRegister(PM_NDSLITE_ADR,data);
  
  _consolePrintf("Set brightness=%d\n",Brightness);
}

static __attribute__((noinline)) void main_Proc_LCDPowerControl(ELCDPC LCDPC)
{
  switch(LCDPC){
    case LCDPC_OFF_BOTH: {
      a7led(3);
      a7lcd_select(0);
    } break;
    case LCDPC_ON_BOTTOM: {
      a7led(0);
      a7lcd_select(PM_BACKLIGHT_BOTTOM);
    } break;
    case LCDPC_ON_TOP: {
      a7led(0);
      a7lcd_select(PM_BACKLIGHT_TOP);
    } break;
    case LCDPC_ON_BOTH: {
      a7led(0);
      a7lcd_select(PM_BACKLIGHT_BOTTOM | PM_BACKLIGHT_TOP);
    } break;
    case LCDPC_SOFT_POWEROFF: {
      a7poff();
      while(1);
    }
    default: ShowLogHalt(); break; // this execute is bug.
  }
  ARM7_SelfCheck_Check();
}

#include "main_touch.h"

static void updateipc(void)
{
  IPC6->buttons = (~REG_KEYXY) & 0x7f;
  IPC6->buttons &= ~IPC_LID_CLOSED;
  
  if((REG_KEYXY & IPC_LID_CLOSED)!=0){
    IPC6->PanelOpened=false;
    }else{
    IPC6->PanelOpened=true;
  }
  
  if((IPC6->buttons & IPC_PEN_DOWN)!=0){
    IPC6->buttons&=~IPC_PEN_DOWN;
    _touchPosition tempPos;
    tempPos.rawx=0;
    tempPos.rawy=0;
    _touchReadXY(&tempPos);
    if((tempPos.rawx!=0)&&(tempPos.rawy!=0)){
      if((tempPos.px<=256)&&(tempPos.py<=192)){
        IPC6->buttons|=IPC_PEN_DOWN;
        IPC6->touchXpx=tempPos.px;
        IPC6->touchYpx=tempPos.py;
      }
    }
  }
  
/*
  if((IPC6->buttons & IPC_PEN_DOWN)!=0){
    touchPosition touchPos=touchReadXY();
    
    int px=(int)touchPos.px;
    int py=(int)touchPos.py;
    
    if((px<0)||(256<px)||(py<0)||(192<py)){
      }else{
    }
  }
*/
  
  if(isNDSLite==false){
    IPC6->ExternalPowerPresent=true; // ‹ŒDS‚Í•s–¾‚È’l‚ð•Ô‚·‚Ì‚Åí‚ÉÚ‘±‚³‚ê‚Ä‚¢‚é‚±‚Æ‚É‚·‚éB
    }else{
    IPC6->ExternalPowerPresent=( (PM_GetRegister(PM_NDSLITE_ADR) & PM_NDSLITE_ExternalPowerPresent) != 0) ? true : false;
  }
  
  ARM7_SelfCheck_Check();
}

static __attribute__((noinline)) void CallBackIRQ_strpcmUpdate(void)
{
  if(IPC6->RequestUpdateIPC==true){
    updateipc();
    IPC6->RequestUpdateIPC=false;
    ARM7_SelfCheck_Check();
  }
  if(IPC6->curtimeFlag==true){
    rtcGetTimeAndDate((uint8 *)&(IPC6->time.rtc.year));
    IPC6->curtimeFlag=false;
    ARM7_SelfCheck_Check();
  }
}

static __attribute__((noinline)) void main_Proc_CheckSoundCode(void)
{
  if(IPC6->ClickSE.Apply==true){
    REG_IME=0;
    StartClickSE(&IPC6->ClickSE);
    REG_IME=1;
  }
}

#include "main_boot_gbarom.h"

static ELCDPC LCDPC_LastState;

int main(int argc, char ** argv)
{
  IPC6->DSTT_SDHCFlag=*(u32*)0x27FFC24;
  
  IPC6->FIFO_ARM9to7_Empty=true;
  IPC6->FIFO_ARM9to7_Data=0;
  
  IPC6->UserLanguage=(u32)-1;
  
//  IPC6->DebugStrFlag=0;
//  IPC6->DebugStr[0]=0;
  
  ARM7_SelfCheck_Init();
  
  main_InitAll();
  
  VsyncPassed=false;
  
  PrintFreeMem();
  
  strpcmPlayFlag=false;
  
  LCDPC_LastState=LCDPC_ON_BOTH;
  
  // Keep the ARM7 out of main RAM
  while (1){
    ARM7_SelfCheck_Check();
    while(VsyncPassed==false){
      swiWaitForVBlank();
      ARM7_SelfCheck_Check();
    }
    VsyncPassed=false;
    
    main_Proc_CheckSoundCode();
    
    if(strpcmPlayFlag==false){
      REG_IME=0;
      CallBackIRQ_strpcmUpdate();
      REG_IME=1;
    }
    
    if(IPC6->strpcmControl!=ESC_NOP){
      REG_IME=0;
      main_Proc_strpcmControl();
      REG_IME=1;
    }
    
    if(IPC6->Brightness!=0xff){
      REG_IME=0;
      if(isNDSLite==true) main_Proc_Brightness(IPC6->Brightness);
      IPC6->Brightness=0xff;
      REG_IME=1;
    }
    
    if(IPC6->RESET!=0){
      REG_IME=0;
      reboot();
      while(1);
    }
    
    bool pcapply=false;
    
    if(IPC6->LCDPowerControl!=LCDPC_NOP){
      REG_IME=0;
      LCDPC_LastState=IPC6->LCDPowerControl;
      IPC6->LCDPowerControl=LCDPC_NOP;
      pcapply=true;
      REG_IME=1;
    }
    
    static u32 LastLID=IPC_LID_CLOSED;
    
    {
      u32 LID=REG_KEYXY & IPC_LID_CLOSED;
      if(LastLID!=LID){
        LastLID=LID;
        pcapply=true;
      }
    }
    
    if(pcapply==true){
      REG_IME=0;
      _consolePrintf("LCD apply: %d,%d.\n",LastLID,LCDPC_LastState);
      if(LastLID==IPC_LID_CLOSED){
        main_Proc_LCDPowerControl(LCDPC_OFF_BOTH);
        if(LCDPC_LastState==LCDPC_SOFT_POWEROFF){
          main_Proc_LCDPowerControl(LCDPC_LastState);
          while(1);
        }
        }else{
        main_Proc_LCDPowerControl(LCDPC_LastState);
      }
      REG_IME=1;
    }
    
    while(FIFO_isEmpty()==false){
      u32 fifocmd;
      FIFO_RX32(fifocmd);
      switch(fifocmd){
        case FIFOCMD_ToARM7_PlaySoundBlock: {
          bool isBGM,isLoop;
          u32 rate,lbuf,rbuf,len,vol,fmt;
          FIFO_RX32(isBGM);
          FIFO_RX32(isLoop);
          FIFO_RX32(rate);
          FIFO_RX32(lbuf);
          FIFO_RX32(rbuf);
          FIFO_RX32(len);
          FIFO_RX32(vol);
          FIFO_RX32(fmt);
          if(isBGM==true){
            s32 ch=getFreeBGMChannel();
            startSound(rate, (const void*)lbuf, len*1, ch+0, vol, 0, fmt ,isLoop);
            startSound(rate, (const void*)rbuf, len*1, ch+1, vol, 127, fmt ,isLoop);
            }else{
            s32 ch;
            ch=getFreeSoundChannel();
            if(ch>=0) startSound(rate, (const void*)lbuf, len*1, ch, vol, 0, fmt ,isLoop);
            ch=getFreeSoundChannel();
            if(ch>=0) startSound(rate, (const void*)rbuf, len*1, ch, vol, 127, fmt ,isLoop);
          }
        } break;
        case FIFOCMD_ToARM7_StopSoundBlock: {
          bool isBGM;
          FIFO_RX32(isBGM);
          if(isBGM==true){
            stopBGM();
            }else{
            stopSound();
          }
        } break;
      }
    }
    
  }
  
  return 0;
}

