
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
#include "skinpack.h"
#include "strtool.h"

#include "fat2.h"
#include "shell.h"
#include "sndeff.h"
#include "procstate.h"
#include "strpcm.h"
#include "cfont.h"

// ------------------------------------------------------

static TProcState BackupProcState;

static void MoveNextProc(bool ReturnFlag)
{
  if(ReturnFlag==false){
    SetNextProc(ENP_Settings,EPFE_None);
    }else{
    SetNextProc(ENP_Main,EPFE_None);
  }
}

static const char* GetLangMsg(const char *pstr)
{
  if(Shell_isJPNmode()==true){
    const char *pjpn=pstr;
    while(1){
      const char ch=*pjpn++;
      if(ch==0) break;
      if(ch=='|'){
        pstr=pjpn;
        break;
      }
    }
  }
  return(pstr);
}

#include "proc_Settings_Component.h"

// ------------------------------------------------------

static void CB_KeyPress(u32 VsyncCount,u32 Keys,bool FirstFlag)
{
  if(Keys&KEY_R) Keys|=KEY_L;
  if(Keys&KEY_X) Keys|=KEY_UP;
  if(Keys&KEY_B) Keys|=KEY_DOWN;
  if(Keys&KEY_Y) Keys|=KEY_LEFT;
  if(Keys&KEY_A) Keys|=KEY_RIGHT;
  
  if((Keys&KEY_L)!=0){
    if((Keys&(KEY_X|KEY_Y))!=0){
      if((Keys&KEY_X)!=0) ChangeNextBacklightLevel();
      if((Keys&KEY_Y)!=0) ChangePrevBacklightLevel();
      return;
    }
    return;
  }
  
  if((Keys&(KEY_DOWN|KEY_SELECT))!=0) CB_CancelBtn_Click(NULL);
  if((Keys&KEY_RIGHT)!=0) CB_OkBtn_Click(NULL);
}

static void CB_Start(void)
{
  BackupProcState=ProcState;
  
  deskmf=false;
  pPressingButton=NULL;
  
  CompsInit();
  
  SoundBGM_Start(WAVFN_HelpBGM,true);
}

static void CB_VsyncUpdate(u32 VsyncCount)
{
  if(8<VsyncCount) VsyncCount=8;
  
  pScreenSub->pCanvas->FillFull(0);
  
  CglCanvas *pcan=pScreenMain->pBackCanvas;
  
  Setting_Redraw(pcan);
  
  ScreenMain_Flip_ProcFadeEffect();
}

static void CB_End(void)
{
  ProcState_Save();
  
  CompsFree();
  
  SoundSE_Stop();
  SoundBGM_Stop();
}

void ProcSettings_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
}

