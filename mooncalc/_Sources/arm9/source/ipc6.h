
#pragma once

//////////////////////////////////////////////////////////////////////
//
// ipc.h -- Inter-processor communication
//
// version 0.1, February 14, 2005
//
//  Copyright (C) 2005 Michael Noland (joat) and Jason Rogers (dovoto)
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any
//  damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any
//  purpose, including commercial applications, and to alter it and
//  redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source
//     distribution.
//
// Changelog:
//   0.1: First version
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

#include <nds.h>

//////////////////////////////////////////////////////////////////////

enum EstrpcmFormat {SPF_PCMx1=1,SPF_DWORD=0xffffffff};

enum EstrpcmControl {ESC_NOP=0,ESC_Play,ESC_Stop,ESC_DWORD=0xffffffff};

enum ELCDPC {LCDPC_NOP=0,LCDPC_OFF_BOTH,LCDPC_ON_BOTTOM,LCDPC_ON_TOP,LCDPC_ON_BOTH,LCDPC_SOFT_POWEROFF,LCDPC_DWORD=0xffffffff};

enum EIPCREQ {IR_NULL,IR_NextSoundData,IR_DWORD=0xffffffff};

enum EARM7SelfCheck {E7SC_OK,E7SC_StackOverflow_SVC,E7SC_StackOverflow_IRQ,E7SC_StackOverflow_SYS,E7SC_DWORD=0xffffffff};

typedef struct {
  bool Apply;
  u32 Volume;
  u32 Count;
  bool AddLong;
} TClickSE;

typedef struct {
  void *pCopyFrom,*pCopyTo;
  u32 CopySize;
  u32 ExecAddr;
} TARMInfo;

enum EDSType {DST_DS=0,DST_DSLite,DST_DSi};

typedef struct sTransferRegion6 {
  u32 StartFlagARM7,StartFlagARM9;
  
  u32 DSTT_SDHCFlag;
  
  u32 RESET;
  TARMInfo ARMInfo7,ARMInfo9;
  bool RequestClearMemory;
  
  // --------------------------
  
  u32 VarDebug;
  
  uint32 heartbeat;          // counts frames
  u32 UserLanguage;
  
  bool RequestUpdateIPC;
  
  u32 buttons;            // X, Y, /PENIRQ , LID buttons
  bool PanelOpened;
  
  u32 touchXpx,touchYpx;
  
  EDSType DSType;
  u32 Brightness,DefaultBrightness; // 0=darkness 1=dark 2=light 3=lightness
  
  u32 BirthMonth;
  u32 BirthDay;
  
  ELCDPC LCDPowerControl;
  
  // for PCM streaming
  
  EIPCREQ IR;
  
  EstrpcmControl strpcmControl;
  u32 strpcmWriteRequest;
  u32 strpcmFreq,strpcmSamples,strpcmChannels;
  u32 strpcmAudioVolume64;
  EstrpcmFormat strpcmFormat;
  u32 *strpcmLRBuf;
  
  u32 strpcm_ARM7FreeMemSize;
  
  bool curtimeFlag;
  union {
    uint8 curtime[8];        // current time response from RTC
    
    struct {
      vu8 command;
      vu8 year;    //add 2000 to get 4 digit year
      vu8 month;    //1 to 12
      vu8 day;    //1 to (days in month)
      
      vu8 weekday;  // day of week
      vu8 hours;    //0 to 11 for AM, 52 to 63 for PM
      vu8 minutes;  //0 to 59
      vu8 seconds;  //0 to 59
    } rtc;
  } time;
  
  EARM7SelfCheck ARM7SelfCheck;
  
  u32 SoundChannels;
  
  bool ExternalPowerPresent;
  
  TClickSE ClickSE;
  
  bool DPG_ControlPowerLED;
  
  bool FIFO_ARM9to7_Empty;
  u32 FIFO_ARM9to7_Data;
} TransferRegion6, * pTransferRegion6;

//////////////////////////////////////////////////////////////////////

#define IPC6 ((TransferRegion6 volatile *)(0x027FF400))

//////////////////////////////////////////////////////////////////////

#define REG_IPC_FIFO_TX		(*(vu32*)0x4000188)
#define REG_IPC_FIFO_RX		(*(vu32*)0x4100000)
#define REG_IPC_FIFO_CR		(*(vu16*)0x4000184)

enum IPC_CONTROL_BITS {
	IPC_FIFO_SEND_EMPTY	=	(1<<0),
	IPC_FIFO_SEND_FULL	=	(1<<1),
	IPC_FIFO_SEND_IRQ	=	(1<<2),
	IPC_FIFO_SEND_CLEAR	=	(1<<3),
	IPC_FIFO_RECV_EMPTY	=	(1<<8),
	IPC_FIFO_RECV_FULL	=	(1<<9),
	IPC_FIFO_RECV_IRQ	=	(1<<10),
	IPC_FIFO_ERROR		=	(1<<14),
	IPC_FIFO_ENABLE		=	(1<<15)
};

#define FIFOCMD_ToARM7_PlaySoundBlock (1)
#define FIFOCMD_ToARM7_StopSoundBlock (2)

#if 0 // for Real.

static bool FIFO_isEmpty(void)
{
  if((REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY)!=0) return(true);
  return(false);
}

#define FIFO_TX32(x) { \
  while((REG_IPC_FIFO_CR&IPC_FIFO_SEND_FULL)!=0); \
  REG_IPC_FIFO_TX=(x); \
}

#define FIFO_RX32(x) { \
  while((REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY)!=0); \
  (x)=REG_IPC_FIFO_RX; \
}

#else // for Emulator.

static bool FIFO_isEmpty(void)
{
  return(IPC6->FIFO_ARM9to7_Empty);
}

#define FIFO_TX32(x) { \
  while(IPC6->FIFO_ARM9to7_Empty==false); \
  IPC6->FIFO_ARM9to7_Data=(x); \
  IPC6->FIFO_ARM9to7_Empty=false; \
}

#define FIFO_RX32(x) { \
  while(IPC6->FIFO_ARM9to7_Empty==true); \
  (x)=IPC6->FIFO_ARM9to7_Data; \
  IPC6->FIFO_ARM9to7_Empty=true; \
}

#endif

