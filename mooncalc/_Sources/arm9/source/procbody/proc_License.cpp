
#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "lang.h"
#include "datetime.h"
#include "inifile.h"

#include "glib.h"
#include "skin.h"
#include "strtool.h"

#include "fat2.h"
#include "shell.h"
#include "sndeff.h"
#include "procstate.h"
#include "strpcm.h"
#include "skinpack.h"

static void MoveNextProc(void)
{
  if(ProcState.System.FirstStart==true){
    ProcState.System.FirstStart=false;
    SetNextProc(ENP_Help,EPFE_RightToLeft);
    }else{
    SetNextProc(ENP_Main,EPFE_DownToUp);
  }
}

static void CB_KeyPress(u32 VsyncCount,u32 Keys,bool FirstFlag)
{
  MoveNextProc();
}

static u32 BGM_VSyncCount;

static void CB_Start(void)
{
  pScreenSub->pCanvas->FillFull(0);
  
  CglB15 *pB15=SkinPack_LoadB15(ESF_license_b15);
  pB15->pCanvas->BitBltFullBeta(pScreenMain->pBackCanvas);
  if(pB15!=NULL){
    delete pB15; pB15=NULL;
  }
  
  SoundBGM_Start(WAVFN_License,false);
  BGM_VSyncCount=SoundBGM_GetCurrentPlayTimePerVsync()+60;
  
  ScreenMain_Flip_ProcFadeEffect();
}

static void CB_VsyncUpdate(u32 VsyncCount)
{
  if(BGM_VSyncCount!=0){
    if(BGM_VSyncCount<VsyncCount){
      BGM_VSyncCount=0;
      }else{
      BGM_VSyncCount-=VsyncCount;
    }
    if(BGM_VSyncCount==0) MoveNextProc();
  }
}

static void CB_End(void)
{
}

void ProcLicense_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
}

