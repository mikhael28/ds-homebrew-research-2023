
#pragma Ospace

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "ipc6.h"

#include "_console.h"

#include "_const.h"

#include "fat2.h"
#include "io_dldi.h"

#include "memtool.h"

#include "plug_ndsrom.h"

#define _VRAM_OFFSET(n) ((n)<<3)
#define _VRAM_PTR ((u16*)0x06000000)
#define _VRAM_CD_MAIN_BG_0x6000000 (1 | _VRAM_OFFSET(0))
#define _VRAM_CD_MAIN_BG_0x6020000 (1 | _VRAM_OFFSET(1))
#define _VRAM_CD_ARM7_0x6000000 (2 | _VRAM_OFFSET(0))
#define _VRAM_CD_ARM7_0x6020000 (2 | _VRAM_OFFSET(1))

#include "plug_ndsrom_dfs.h"

#include "plug_ndsrom_resetmem.h"
#include "plug_ndsrom_dldipatch.h"

static void reboot(void)
{
  REG_IME = IME_DISABLE;	// Disable interrupts
  REG_IF = REG_IF;	// Acknowledge interrupt
  
  _consolePrint("_consoleClearLogFile();\n");
  _consoleClearLogFile();
  
  _consolePrint("FAT_FreeFiles\n");
  FAT2_FreeFiles();
	
  u32 zero;
  asm {
    mov zero,#0
    MCR p15, 0, zero, c7, c10, 4 // drain write buffer
  }
  DC_FlushAll();
  
	IPC6->RESET=3;
	while(IPC6->RESET!=4){
    // ARM7Wait: Copy EWRAM to ARM7InternalMemory. and, Reset memory.
	  for(vu32 w=0;w<0x100;w++);
	}
  
//  chkmd5(pFileBuf,FileSize);
  
//  _consolePrintf("REBOOT\n");
  
  {
    u16 KEYS_CUR;
    
    KEYS_CUR=(~REG_KEYINPUT)&0x3ff;
    while(KEYS_CUR!=0){
      KEYS_CUR=(~REG_KEYINPUT)&0x3ff;
    }
  }
  
//  _consolePrint("resetMemory1_ARM9\n");
  resetMemory1_ARM9();
  
#define _REG_WAIT_CR (*(vuint16*)0x04000204)
//  _REG_WAIT_CR|=1 << 7;
  
//  resetMemory2_ARM9(&IPC6->RESET_BootAddress,BootAddress,true);

  resetMemory2load_ARM9_NoBIOS();
  while(1); // ŒÄ‚Ño‚µŒ³‚É‚Í‹A‚ç‚È‚¢B
}

static void RebootNDSHomeBrew(const char *pFilename)
{
  u8 *pDLDIBuf=NULL;
  s32 DLDISize=0;
  
/*
  const char fn[]="DLDIBODY.BIN";
  if(Shell_FAT_ReadAlloc(&MM_Temp,fn,(void**)&pDLDIBuf,&DLDISize)==false){
    _consolePrintf("Can not load DLDI patch file. (%s)\n",fn);
    pDLDIBuf=NULL;
    DLDISize=0;
  }
*/
  pDLDIBuf=(u8*)&dldibodytop;
  DLDISize=32*1024;
  
//  DLDISize=0; // Disabled auto DLDI patch.
  
  u32 FileSize;
  
  {
    FAT_FILE *FileHandle=FAT2_fopen_AliasForRead(pFilename);
    if(FileHandle==NULL){
      _consolePrintf("Can not open NDS file. [%s]\n",pFilename);
      StopFatalError(15001,"Can not open NDS file. [%s]\n",pFilename);
    }
    
    DFS_Init(FileHandle);
    FileSize=FAT2_GetFileSize(FileHandle);
    
    _consolePrint("FAT_fclose\n");
    FAT2_fclose(FileHandle);
  }
  
  {
    u32 header[0x40/4];
    DFS_SetOffset(0);
    DFS_Read16bit(header,0x40);
    {
      volatile TARMInfo *pai=&IPC6->ARMInfo7;
      pai->pCopyFrom=(void*)header[0x30/4];
      pai->pCopyTo=(void*)header[0x38/4];
      pai->CopySize=header[0x3c/4];
      pai->ExecAddr=header[0x34/4];
//      _consolePrintf("ARM7 CopyFrom=0x%08x, CopyTo=0x%08x, CopySize=%dbyte, ExecAddr=0x%08x.\n",pai->pCopyFrom,pai->pCopyTo,pai->CopySize,pai->ExecAddr);
      DFS_SetOffset((u32)pai->pCopyFrom);
      pai->pCopyFrom=malloc(pai->CopySize);
      if(pai->pCopyFrom==NULL) StopFatalError(0,"Large ARM7 binary size. %dbyte.",pai->CopySize);
      DFS_Read16bit(pai->pCopyFrom,pai->CopySize);
      _consolePrintf("ARM7 Loaded. ptr=0x%08x.\n",pai->pCopyFrom);
    }
    {
      volatile TARMInfo *pai=&IPC6->ARMInfo9;
      pai->pCopyFrom=(void*)header[0x20/4];
      pai->pCopyTo=(void*)header[0x28/4];
      pai->CopySize=header[0x2c/4];
      pai->ExecAddr=header[0x24/4];
//      _consolePrintf("ARM9 CopyFrom=0x%08x, CopyTo=0x%08x, CopySize=%dbyte, ExecAddr=0x%08x.\n",pai->pCopyFrom,pai->pCopyTo,pai->CopySize,pai->ExecAddr);
      DFS_SetOffset((u32)pai->pCopyFrom);
      pai->pCopyFrom=malloc(pai->CopySize);
      if(pai->pCopyFrom==NULL) StopFatalError(0,"Large ARM9 binary size. %dbyte.",pai->CopySize);
      DFS_Read16bit(pai->pCopyFrom,pai->CopySize);
      _consolePrintf("ARM9 Loaded. ptr=0x%08x.\n",pai->pCopyFrom);
    }
    {
      volatile TARMInfo *pai7=&IPC6->ARMInfo7;
      volatile TARMInfo *pai9=&IPC6->ARMInfo9;
      IPC6->RequestClearMemory=true;
      u32 ARM7CopyTo=(u32)pai7->pCopyTo;
      u32 ARM9CopyTo=(u32)pai9->pCopyTo;
      if((ARM7CopyTo<0x037f8000)||(0x0380f400<=ARM7CopyTo)) IPC6->RequestClearMemory=false;
      if((ARM9CopyTo<0x02000000)||(0x023ff000<=ARM9CopyTo)) IPC6->RequestClearMemory=false;
      if(IPC6->RequestClearMemory==true) _consolePrintf("Enable memory clean-up function.\n");
    }
  }
  
  DFS_Free();
  
  if((pDLDIBuf!=NULL)&&(DLDISize!=0)){
    volatile TARMInfo *pai=&IPC6->ARMInfo9;
    if(DLDIPatch_ARM9BIN((u8*)pai->pCopyFrom,pai->CopySize,pDLDIBuf,DLDISize)==false){
      _consolePrintf("DLDI patch error or not found DLDI chank.\n");
    }
  }
  
//  reboot(pFileBuf,FileSize);
  reboot();
  
  StopFatalError(15003,"Unknown error.\n");
}

void BootNDSROM(const char *pFilename)
{
	REG_IME = 0;
	REG_IE = 0;
	REG_IF = ~0;
	
	IPC6->RESET=1;
	_consolePrint("ARM7Wait: goto reboot function.");
	while(IPC6->RESET!=2){
	  for(vu32 w=0;w<0x100;w++);
	}
	
  IPC6->StartFlagARM7=0;
  IPC6->StartFlagARM9=0;
  
  DMA0_CR=0;
  DMA1_CR=0;
  DMA2_CR=0;
  DMA3_CR=0;
  
  RebootNDSHomeBrew(pFilename);
  
  StopFatalError(15006,"Unknown error.\n");
}
