
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
#include "skinpack.h"

static const u32 HelpPageCount=4;
static u32 HelpPageCurrent;

static void DrawHelpPage(EProcFadeEffect ProcFadeEffect)
{
  ESkinFiles ESF=ESF_helpj0_b15;
  switch(HelpPageCurrent){
    case 0: ESF=ESF_helpj0_b15; break;
    case 1: ESF=ESF_helpj1_b15; break;
    case 2: ESF=ESF_helpj2_b15; break;
    case 3: ESF=ESF_helpj3_b15; break;
  }
  
  CglB15 *pB15=SkinPack_LoadB15(ESF);
  pB15->pCanvas->BitBltFullBeta(pScreenMain->pBackCanvas);
  if(pB15!=NULL){
    delete pB15; pB15=NULL;
  }
  
  SetProcFadeEffect(ProcFadeEffect);
  ScreenMain_Flip_ProcFadeEffect();
}

static void CB_KeyPress(u32 VsyncCount,u32 Keys,bool FirstFlag)
{
  if(Keys&(KEY_START|KEY_SELECT)){
    SetNextProc(ENP_Main,EPFE_CrossFade);
    return;
  }
  
  if(FirstFlag==false) return;
  
  if(Keys&KEY_TOUCH) return;
  
  if(Keys&KEY_R) Keys|=KEY_L;
  if(Keys&KEY_X) Keys|=KEY_UP;
  if(Keys&KEY_B) Keys|=KEY_DOWN;
  if(Keys&KEY_Y) Keys|=KEY_LEFT;
  if(Keys&KEY_A) Keys|=KEY_RIGHT;
  
  if(Keys&KEY_LEFT){
    if(0<HelpPageCurrent){
      HelpPageCurrent--;
      DrawHelpPage(EPFE_LeftToRight);
      return;
    }
  }
  
  if(Keys&KEY_RIGHT){
    if((HelpPageCurrent+1)<HelpPageCount){
      HelpPageCurrent++;
      DrawHelpPage(EPFE_RightToLeft);
    }
    return;
  }
  
  SetNextProc(ENP_Main,EPFE_CrossFade);
}

static bool deskmf;

static void CB_MouseDown(s32 x,s32 y)
{
  deskmf=false;
  
  const u32 s=48;
  
  if((x<s)&&((ScreenHeight-s)<=y)){
    deskmf=true;
    if(0<HelpPageCurrent){
      HelpPageCurrent--;
      DrawHelpPage(EPFE_LeftToRight);
      return;
    }
  }
  
  if(((ScreenWidth-s)<=x)&&((ScreenHeight-s)<=y)){
    deskmf=true;
    if((HelpPageCurrent+1)<HelpPageCount){
      HelpPageCurrent++;
      DrawHelpPage(EPFE_RightToLeft);
    }
    return;
  }
  
  SetNextProc(ENP_Main,EPFE_CrossFade);
}

static void CB_MouseMove(s32 x,s32 y)
{
  if(deskmf==false) return;
}

static void CB_MouseUp(s32 x,s32 y)
{
  if(deskmf==false) return;
  
  deskmf=false;
}

static void CB_Start(void)
{
  deskmf=false;
  
  pScreenSub->pCanvas->FillFull(0);
  
  SoundBGM_Start(WAVFN_HelpBGM,true);
//  Sound_Start(WAVFN_Help);
  
  HelpPageCurrent=0;
  DrawHelpPage(EPFE_CrossFade);
}

static void CB_VsyncUpdate(u32 VsyncCount)
{
}

static void CB_End(void)
{
}

void ProcHelp_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
}

