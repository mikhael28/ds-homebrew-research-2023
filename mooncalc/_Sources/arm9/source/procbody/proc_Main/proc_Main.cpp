
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
#include "lngmsg.h"

// ------------------------------------------------------

static u32 JingleBGM_VSyncCount=0;

#include "Calc.h"

// ------------------------------------------------------

static char SysMsg_str[256];
static u32 SysMsg_Timeout;

static void SysMsg_Init(void)
{
  SysMsg_str[0]=0;
  SysMsg_Timeout=0;
}

static void SysMsg_SetMsgLang(const char *pstr)
{
  strcpy(SysMsg_str,pstr);
  SysMsg_Timeout=2*60;
  SetProcFadeEffect(EPFE_CrossFade);
}

static void SysMsg_UpdateVsync(const u32 VsyncCount)
{
  if(SysMsg_Timeout==0) return;
  
  if(SysMsg_Timeout<VsyncCount){
    SysMsg_Timeout=0;
    }else{
    SysMsg_Timeout-=VsyncCount;
  }
  
  if(SysMsg_Timeout==0) SetProcFadeEffect(EPFE_FastCrossFade);
}

static void SysMsg_Draw(CglCanvas *pcan)
{
  if(SysMsg_Timeout==0) return;
  
  const u32 tw=pcan->GetTextWidthLang(SysMsg_str);
  const u32 tx=(ScreenWidth-tw)/2;
  const u32 th=glCanvasTextHeight+8;
  const u32 ty=((ScreenHeight-th)/2);
  
  {
    u16 *pbuf=pcan->GetVRAMBuf();
    pbuf+=(ty-1)*ScreenWidth;
    for(u32 idx=0;idx<1*ScreenWidth;idx++){
      u16 col=*pbuf;
      col=((col&0x7BDE)>>1)+RGB15(0,0,0)|BIT15;
      *pbuf++=col;
    }
    for(u32 idx=0;idx<th*ScreenWidth;idx++){
      u16 col=*pbuf;
      col=((col&0x739C)>>2)+RGB15(22,20,20)|BIT15;
      *pbuf++=col;
    }
    for(u32 idx=0;idx<1*ScreenWidth;idx++){
      u16 col=*pbuf;
      col=((col&0x7BDE)>>1)+RGB15(0,0,0)|BIT15;
      *pbuf++=col;
    }
  }
  
  pcan->SetFontTextColor(RGB15(0,0,0)|BIT15);
  pcan->TextOutLang(tx,ty+4,SysMsg_str);
}

// ------------------------------------------------------

static void ExpMoveCur_Touch(u32 x);
static void ExpAddOne(const char ch);
static void ExpDeleteOne(void);
static void ExpDeleteAll(void);

#include "proc_Main_Component.h"

static CglB15 *pMathFontB15;
extern CFont *pMathFont;
CFont *pMathFont;

// ------------------------------------------------------

const u32 ExpOffsetX=2,ExpOffsetY=4;

static void ExpInit(void)
{
}

static void DrawOnlineHelp(CglCanvas *pcan);

static void ExpCalcAndDraw(void)
{
  CglCanvas *pcan=new CglCanvas(&MM_Temp,NULL,SCREEN_WIDTH,SCREEN_HEIGHT,pf15bit);
  pcan->SetCglFont(pCglFontDefault);
  
  pcan->FillFull(RGB15(31,31,31)|BIT15);
  if(Calc_Draw(pcan)==false){
    DrawOnlineHelp(pcan);
  }
  
  pcan->BitBltFullBeta(pScreenSub->pCanvas);
  
  if(pcan!=NULL){
    delete pcan; pcan=NULL;
  }
}

static void ExpMoveCur(s32 v)
{
  s32 cur=ProcState.Main.ExpCur;
  s32 len=strlen(ProcState.Main.ExpStr);
  
  switch(v){
    case -2: {
      cur=0;
    } break;
    case -1: {
      if(1<=cur) cur--;
    } break;
    case 0: {
    } break;
    case 1: {
      if(cur<=(len-1)) cur++;
    } break;
    case 2: {
      cur=len;
    } break;
  }
  
  if(ProcState.Main.ExpCur!=cur){
    ProcState.Main.ExpCur=cur;
    ProcState_RequestSave=true;
    SoundSE_Start(WAVFN_CurMove);
  }
}

static void ExpMoveCur_Touch(u32 TouchX)
{
  s32 cur=-1;
  s32 len=strlen(ProcState.Main.ExpStr);
  
  u32 x=ExpOffsetX;
  
  for(u32 idx=0;idx<=len;idx++){
    char text[16];
    if(idx<len){
      Calc_GetStrFromChar(text,ProcState.Main.ExpStr[idx]);
      }else{
      text[0]=0;
    }
    
    u32 tw=pMathFont->GetTextWidth(text);
    bool incx=true;
    if(ScreenWidth<(x+tw)) break;
    if(idx==cur) tw+=2;
    if((x<=TouchX)&&(TouchX<(x+tw))){
      cur=idx;
      break;
    }
    if(incx==true) x+=tw;
  }
  
  if(cur==-1) cur=len;
  
  if(ProcState.Main.ExpCur!=cur){
    ProcState.Main.ExpCur=cur;
    ProcState_RequestSave=true;
    SoundSE_Start(WAVFN_CurMove);
  }
}

static void ExpDraw(CglCanvas *pcan)
{
  u32 x=ExpOffsetX,y=ExpOffsetY;
  
  s32 cur=ProcState.Main.ExpCur;
  s32 len=strlen(ProcState.Main.ExpStr);
  
  u32 StartIndex=(u32)-1,EndIndex=(u32)-1;
  {
    s32 tmp=0;
    for(s32 idx=cur;idx>=0;idx--){
      if(ProcState.Main.ExpStr[idx]=='('){
        if(tmp==0){
          StartIndex=idx;
          break;
        }
        tmp++;
      }
      if((idx!=cur)&&(ProcState.Main.ExpStr[idx]==')')){
        tmp--;
        if(tmp>=0) break;
      }
    }
  }
  {
    u32 tmp=0;
    for(s32 idx=cur;idx<len;idx++){
      if((idx!=cur)&&(ProcState.Main.ExpStr[idx]=='(')){
        tmp++;
        if(tmp<=0) break;
      }
      if(ProcState.Main.ExpStr[idx]==')'){
        if(tmp==0){
          EndIndex=idx;
          break;
        }
        tmp--;
      }
    }
  }
  if((StartIndex!=(u32)-1)||(EndIndex!=(u32)-1)){
    if(StartIndex==(u32)-1) StartIndex=0;
    if(EndIndex==(u32)-1) EndIndex=len;
  }
  
  const u32 NestColorCount=4;
  const u16 NestColor[NestColorCount]={0,
                                       RGB15(24,31,24)|BIT15,
                                       RGB15(31,24,24)|BIT15,
                                       RGB15(24,24,31)|BIT15,
                                      };
  s32 NestLev=0;
  
  for(u32 idx=0;idx<=len;idx++){
    char text[16];
    if(idx<len){
      Calc_GetStrFromChar(text,ProcState.Main.ExpStr[idx]);
      }else{
      text[0]=0;
    }
    
    u32 tw=pMathFont->GetTextWidth(text);
    u32 th=glCanvasTextHeight+2;
    bool incx=true;
    if(ScreenWidth<(x+tw)){
      x=ExpOffsetX;
      y+=th;
      incx=false;
    }
    if(idx==cur) tw+=2;
    if(text[0]==')') NestLev--;
    if((0<=NestLev)&&(NestLev<NestColorCount)){
      const u16 BGCol=NestColor[NestLev];
      if(BGCol!=0){
        pcan->SetColor(BGCol);
        pcan->FillBox(x,y,tw,th);
      }
    }
    if(text[0]=='(') NestLev++;
    if((StartIndex!=(u32)-1)&&(EndIndex!=(u32)-1)&&(StartIndex<=idx)&&(idx<=EndIndex)){
      pcan->SetColor(RGB15(24,8,8)|BIT15);
      pcan->FillBox(x,y+th-1,tw,1);
    }
    if(idx==cur){
      pcan->SetColor(RGB15(0,0,0)|BIT15);
      pcan->FillBox(x,y+1,1,th-2);
      pMathFont->DrawText(pcan,x+2,y,text);
      }else{
      pMathFont->DrawText(pcan,x,y,text);
    }
    if(incx==true) x+=tw;
  }
}

static void ExpAddOne(const char ch)
{
  u32 cur=ProcState.Main.ExpCur;
  
  u32 len=strlen(ProcState.Main.ExpStr)+1;
  for(u32 idx=len-1;idx>=cur+1;idx--){
    ProcState.Main.ExpStr[idx]=ProcState.Main.ExpStr[idx-1];
  }
  ProcState.Main.ExpStr[len]=0;
  
  ProcState.Main.ExpStr[cur]=ch;
  ProcState.Main.ExpCur++;
  
  ProcState_RequestSave=true;
  
  ExpCalcAndDraw();
}

static void ExpDeleteOne(void)
{
  if(1<=ProcState.Main.ExpCur) ProcState.Main.ExpCur--;
  
  u32 cur=ProcState.Main.ExpCur;
  
  if(ProcState.Main.ExpStr[cur]==0) return;
  
  u32 len=strlen(ProcState.Main.ExpStr);
  for(u32 idx=cur;idx<len-1;idx++){
    ProcState.Main.ExpStr[idx]=ProcState.Main.ExpStr[idx+1];
  }
  ProcState.Main.ExpStr[len-1]=0;
  
  ProcState_RequestSave=true;
  
  ExpCalcAndDraw();
}

static void ExpDeleteAll(void)
{
  ProcState.Main.ExpStr[0]=0;
  ProcState.Main.ExpCur=0;
  ProcState_RequestSave=true;
  
  ExpCalcAndDraw();
}

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
  
  if((Keys&KEY_SELECT)!=0) SetNextProc(ENP_Settings,EPFE_FastCrossFade);
    
  if((Keys&KEY_LEFT)!=0) ExpMoveCur(-1);
  if((Keys&KEY_RIGHT)!=0) ExpMoveCur(1);
}

static void DrawOnlineHelp(CglCanvas *pcan)
{
  pcan->SetCglFont(pCglFontDefault);
  
  u32 x=8;
  u32 h=glCanvasTextHeight+2;
  u32 y=ScreenHeight-(h*4);
  
//  pcan->FillFull(0);
  pcan->SetColor(RGB15(8,8,8)|BIT15);
  pcan->DrawLine(0,y-4-1,ScreenWidth,y-4-1);
  pcan->SetColor(RGB15(0,0,0)|BIT15);
  pcan->FillBox(0,(y-4),ScreenWidth,ScreenHeight-(y-4));
  
  for(u32 idx=0;idx<4;idx++){
    const char *pmsg=NULL;
    switch(idx){
      case 0: pmsg=LNGMSG_ProcRecorderHelp0; break;
      case 1: pmsg=LNGMSG_ProcRecorderHelp1; break;
      case 2: pmsg=LNGMSG_ProcRecorderHelp2; break;
      case 3: pmsg=LNGMSG_ProcRecorderHelp3; break;
    }
    if(pmsg!=NULL){
      pcan->SetFontTextColor(RGB15(0,0,2)|BIT15);
      pcan->TextOutLang(x+1,y+1,pmsg);
      pcan->SetFontTextColor(RGB15(16,16,18)|BIT15);
      pcan->TextOutLang(x+0,y+0,pmsg);
    }
    y+=h;
  }
}

#define CubeImgsCount (32)
typedef struct {
  CglTGF *pTGF;
} TCubeImg;
static TCubeImg CubeImgs[CubeImgsCount];
static u32 CubeImgIdx=0;

static void CubeImgLoad(void)
{
  for(u32 idx=0;idx<CubeImgsCount;idx++){
    ESkinFiles ESF=(ESkinFiles)-1;
    switch(idx){
      case  0: ESF=ESF_cube00_tgf; break;
      case  1: ESF=ESF_cube01_tgf; break;
      case  2: ESF=ESF_cube02_tgf; break;
      case  3: ESF=ESF_cube03_tgf; break;
      case  4: ESF=ESF_cube04_tgf; break;
      case  5: ESF=ESF_cube05_tgf; break;
      case  6: ESF=ESF_cube06_tgf; break;
      case  7: ESF=ESF_cube07_tgf; break;
      case  8: ESF=ESF_cube08_tgf; break;
      case  9: ESF=ESF_cube09_tgf; break;
      case 10: ESF=ESF_cube10_tgf; break;
      case 11: ESF=ESF_cube11_tgf; break;
      case 12: ESF=ESF_cube12_tgf; break;
      case 13: ESF=ESF_cube13_tgf; break;
      case 14: ESF=ESF_cube14_tgf; break;
      case 15: ESF=ESF_cube15_tgf; break;
      case 16: ESF=ESF_cube16_tgf; break;
      case 17: ESF=ESF_cube17_tgf; break;
      case 18: ESF=ESF_cube18_tgf; break;
      case 19: ESF=ESF_cube19_tgf; break;
      case 20: ESF=ESF_cube20_tgf; break;
      case 21: ESF=ESF_cube21_tgf; break;
      case 22: ESF=ESF_cube22_tgf; break;
      case 23: ESF=ESF_cube23_tgf; break;
      case 24: ESF=ESF_cube24_tgf; break;
      case 25: ESF=ESF_cube25_tgf; break;
      case 26: ESF=ESF_cube26_tgf; break;
      case 27: ESF=ESF_cube27_tgf; break;
      case 28: ESF=ESF_cube28_tgf; break;
      case 29: ESF=ESF_cube29_tgf; break;
      case 30: ESF=ESF_cube30_tgf; break;
      case 31: ESF=ESF_cube31_tgf; break;
    }
    CubeImgs[idx].pTGF=SkinPack_LoadTGF(ESF);
  }
}

static void CubeImgFree(void)
{
  for(u32 idx=0;idx<CubeImgsCount;idx++){
    if(CubeImgs[idx].pTGF!=NULL){
      delete CubeImgs[idx].pTGF; CubeImgs[idx].pTGF=NULL;
    }
  }
}

static void CB_Start(void)
{
  deskmf=false;
  pPressingButton=NULL;
  deskmf_EditBox=false;
  
  CubeImgLoad();
  
  CompsInit();
  CompsSetPage(0);
  
  ExpInit();
  
  SysMsg_Init();
  
  pMathFontB15=SkinPack_LoadB15(ESF_mathfont_b15);
  pMathFont=new CFont(CFont::EBM_B15,pMathFontB15,NULL,DefaultImplantCharsCount,DefaultImplantChars);
  
  ExpCalcAndDraw();
  
  static bool FirstStart=true; // ‚ß‚ñ‚Ç‚¢‚Ì‚Åglobal•Ï”
  if(FirstStart==true){
    FirstStart=false;
    SoundBGM_Start(WAVFN_StartJingle,false);
    JingleBGM_VSyncCount=SoundBGM_GetCurrentPlayTimePerVsync();
    }else{
    JingleBGM_VSyncCount=0;
    SoundBGM_Start(WAVFN_BGM,true);
  }
}

static void CB_VsyncUpdate(u32 VsyncCount)
{
  if(8<VsyncCount) VsyncCount=8;
  SysMsg_UpdateVsync(VsyncCount);
  
  if(VsyncCount<4){
    static u32 ProcState_SaveInterval=0;
    if(ProcState_SaveInterval<=2){
      ProcState_SaveInterval++;
      }else{
      ProcState_SaveInterval=0;
      ProcState_Save();
    }
  }
  
  if(deskmf==false){
    u16 Keys=(~REG_KEYINPUT)&0x3ff;
    if(Keys&(KEY_L|KEY_R)){
      CompsSetPage(1);
      }else{
      CompsSetPage(0);
    }
  }
  
  if(deskmf==true){
    if((pPressingButton==&CompButtons[ECBS_HelpBtn])||(pPressingButton==&CompButtons[ECBS_ClearBtn])||(pPressingButton==&CompButtons[ECBS_SettingsBtn])){
      if((deskmf_PressCount<10)&&(10<=(deskmf_PressCount+VsyncCount))){
        CB_MouseDown_PlaySE(pPressingButton);
      }
      deskmf_PressCount+=VsyncCount;
    }
  }
  
  CglCanvas *pcan=pScreenMain->pBackCanvas;
  pcan->SetCglFont(pCglFontDefault);
  
  Setting_Redraw(pcan);
  SysMsg_Draw(pcan);
  
  switch(CompsGetPage()){
    case 0: {
      CubeImgIdx+=VsyncCount;
      while((CubeImgsCount*2)<=CubeImgIdx){
        CubeImgIdx-=CubeImgsCount*2;
      }
      const TComponentButton *pcb=&CompButtons[ECBS_SettingsBtn];
      const TRect *pRect=&pcb->Rect;
      CubeImgs[CubeImgIdx/2].pTGF->BitBlt(pcan,pRect->x,pRect->y);
    } break;
    case 1: {
    } break;
  }
  
  ExpDraw(pcan);
  
  ScreenMain_Flip_ProcFadeEffect();
    
  if(JingleBGM_VSyncCount!=0){
    if(JingleBGM_VSyncCount<VsyncCount){
      JingleBGM_VSyncCount=0;
      }else{
      JingleBGM_VSyncCount-=VsyncCount;
    }
    if(JingleBGM_VSyncCount==0) SoundBGM_Start(WAVFN_BGM,true);
  }
}

static void CB_ReqSoftReset(void)
{
  pScreenMain->pBackCanvas->FillFull(RGB15(0,0,0)|BIT15);
  SetProcFadeEffect(EPFE_UpToDown);
  ScreenMain_Flip_ProcFadeEffect();
  pScreenMain->pBackCanvas->FillFull(RGB15(0,0,0)|BIT15);
  
  extern bool ReqSoftResetFlag;
  ReqSoftResetFlag=true;
}

static void CB_End(void)
{
  CompsFree();
  CubeImgFree();
  
  if(pMathFont!=NULL){
    delete pMathFont; pMathFont=NULL;
  }
  if(pMathFontB15!=NULL){
    delete pMathFontB15; pMathFontB15=NULL;
  }
  
  SoundSE_Stop();
  SoundBGM_Stop();
  
  ProcState_Save();
}

void ProcMain_SetCallBack(TCallBack *pCallBack)
{
  pCallBack->Start=CB_Start;
  pCallBack->VsyncUpdate=CB_VsyncUpdate;
  pCallBack->End=CB_End;
  pCallBack->KeyPress=CB_KeyPress;
  pCallBack->MouseDown=CB_MouseDown;
  pCallBack->MouseMove=CB_MouseMove;
  pCallBack->MouseUp=CB_MouseUp;
  pCallBack->ReqSoftReset=CB_ReqSoftReset;
}

