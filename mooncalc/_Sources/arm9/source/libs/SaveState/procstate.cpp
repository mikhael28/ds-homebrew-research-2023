
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "_const.h"
#include "memtool.h"
#include "fat2.h"
#include "lang.h"
#include "zlibhelp.h"

#include "procstate.h"

#include "shell.h"
#include "disc_io.h"
#include "ipc6.h"

#define CurrentProcStateVersion (0x00020005)

TProcState ProcState;
bool ProcState_RequestSave;

static void DataInit(void)
{
  TProcState *pstate=&ProcState;
  
  pstate->Version=CurrentProcStateVersion;
  
  {
    TProcState_System *psbody=&pstate->System;
    if(Shell_isJPNmode()==false){
      psbody->Lang=TProcState_System::EL_ENG;
      }else{
      psbody->Lang=TProcState_System::EL_JPN;
    }
    psbody->ClickSound=true;
    psbody->EnableFadeEffect=true;
    
    psbody->AudioVolume64=64;
    psbody->BacklightLevel=IPC6->DefaultBrightness;
    psbody->SkinFilenameUnicode[0]=0;
    psbody->LRKeyLock=TProcState_System::ELRKL_RelationalPanel;
    psbody->Use24hFormat=false;
    psbody->FirstStart=true;
    psbody->VolBGM=TProcState_System::EV_Mid;
    psbody->VolSE=TProcState_System::EV_High;
  }
  
  {
    TProcState_Main *psbody=&pstate->Main;
    psbody->ShowBin=false;
    psbody->ShowBin=true;
    psbody->ShowPrec=TProcState_Main::ESP_D3;
    strcpy(ProcState.Main.ExpStr,"");
//    strcpy(ProcState.Main.ExpStr,"80-(a(f/2)+(1+2*4))");
    psbody->ExpCur=strlen(ProcState.Main.ExpStr);
  }
  
}

#include "procstate_dfs.h"

#define ProcState_ReadWriteSize (2048)

void ProcState_Init(void)
{
  DataInit();
  ProcState_RequestSave=false;
  
  pBurstList=NULL;
  
  if(VerboseDebugLog==true) _consolePrintf("Open setting file. [%s]\n",DataFilename);
  FAT_FILE *pf=Shell_FAT_fopen_Internal(DataFilename);
  if(pf!=NULL){
    u32 filesize=FAT2_GetFileSize(pf);
    if(filesize==ProcState_ReadWriteSize) CreateBurstList(pf);
    FAT2_fclose(pf);
  }
  
  const u32 size=sizeof(ProcState)-512;
  if(VerboseDebugLog==true) _consolePrintf("ProcState: size= %dbyte, R/W size= %dbyte.\n",size,ProcState_ReadWriteSize);
  if((pBurstList==NULL)||(ProcState_ReadWriteSize<size)) StopFatalError(13501,"File not found or size error. [%s]\n",DataFilename);
}

void ProcState_Free(void)
{
  ProcState_DFS_Free();
}

void ProcState_Clear(void)
{
  DataInit();
  
  ProcState_RequestSave=true;
}

static void fbuf_LoadFromDisk(void)
{
  if(pBurstList==NULL) StopFatalError(13502,"Data sector index is 0 for load.\n");
  ProcState_DFS_Read32bit((u8*)&ProcState,ProcState_ReadWriteSize/SectorSize);
}

static void fbuf_SaveToDisk(void)
{
  if(pBurstList==NULL) StopFatalError(13503,"Data sector index is 0 for save.\n");
  ProcState_DFS_Write32bit((u8*)&ProcState,ProcState_ReadWriteSize/SectorSize);
}

void ProcState_Load(void)
{
  if(VerboseDebugLog==true) _consolePrintf("Load settings.\n");
  
  fbuf_LoadFromDisk();
  
  if(ProcState.Version!=CurrentProcStateVersion){
    _consolePrint("This file is old version setting. load default.\n");
    DataInit();
  }
  
  ProcState_RequestSave=false;
}

void ProcState_Save(void)
{
  if(ProcState_RequestSave==false) return;
  ProcState_RequestSave=false;
  
  if(VerboseDebugLog==true) _consolePrint("Save settings.\n");
  
  fbuf_SaveToDisk();
  
  if(VerboseDebugLog==true) _consolePrintf("Saved setting.\n");
}

void ApplyCurrentBacklightLevel(void)
{
  TProcState_System *psys=&ProcState.System;
  IPC6->Brightness=psys->BacklightLevel;
  if(VerboseDebugLog==true) _consolePrintf("Backlight set to %d.\n",psys->BacklightLevel);
}

void ChangePrevBacklightLevel(void)
{
  TProcState_System *psys=&ProcState.System;
  
  if(psys->BacklightLevel==0){
    psys->BacklightLevel=3;
    }else{
    psys->BacklightLevel--;
  }
  
  ProcState_RequestSave=true;
  
  ApplyCurrentBacklightLevel();
}

void ChangeNextBacklightLevel(void)
{
  TProcState_System *psys=&ProcState.System;
  
  psys->BacklightLevel++;
  if(psys->BacklightLevel==4) psys->BacklightLevel=0;
  
  ProcState_RequestSave=true;
  
  ApplyCurrentBacklightLevel();
}

static u32 GetVolume128(TProcState_System::EVolume vol)
{
  u32 vol128=0;
  const u32 base=32;
  switch(vol){
    case TProcState_System::EV_Mute: vol128=base*0; break;
    case TProcState_System::EV_Low:  vol128=base*1; break;
    case TProcState_System::EV_Mid:  vol128=base*2; break;
    case TProcState_System::EV_High: vol128=base*3; break;
  }
  return(vol128);
}

u32 GetBGMVolume128(void)
{
  return(GetVolume128(ProcState.System.VolBGM));
}

u32 GetSEVolume128(void)
{
  return(GetVolume128(ProcState.System.VolSE));
}

