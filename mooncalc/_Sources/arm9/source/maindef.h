#pragma once

#include "_console.h"

extern const char ROMTITLE[];
extern const char ROMVERSION[];
extern const char ROMDATE[];
extern const char ROMDATESHORT[];
extern const char ROMDATEVERYSHORT[];
extern const char ROMENV[];
extern const char OverlayHeader_ID[];
extern const char OverlayHeader_ID_CanNotAccess[];

#include "glib.h"

extern CglFont *pCglFontDefault;

extern void WaitForVBlank(void);

enum ETriggerType {ETT_AButton=0,ETT_LButton,ETT_RButton,ETT_PhoneSwitch};

typedef struct {
  void (*Start)(void);
  void (*VsyncUpdate)(u32 VsyncCount);
  void (*End)(void);
  void (*KeyPress)(u32 VsyncCount,u32 Keys,bool FirstFlag);
  void (*KeyLongPress)(u32 Keys);
  void (*KeySameLRDown)(void);
  void (*KeySameLRUp)(void);
  void (*MouseDown)(s32 x,s32 y);
  void (*MouseMove)(s32 x,s32 y);
  void (*MouseUp)(s32 x,s32 y);
  void (*strpcmRequestStop)(void);
  void (*PanelOpen)(void);
  void (*PanelClose)(void);
  void (*VBlankHandler)(void);
  void (*ExternalPowerAttach)(void);
  void (*ExternalPowerDetach)(void);
  void (*Trigger_ProcStart)(ETriggerType TriggerType);
  void (*Trigger_ProcEnd)(ETriggerType TriggerType);
  void (*Trigger_Down)(ETriggerType TriggerType);
  void (*Trigger_Up)(ETriggerType TriggerType);
  void (*Trigger_LongStart)(ETriggerType TriggerType);
  void (*Trigger_LongEnd)(ETriggerType TriggerType);
  void (*Trigger_SingleClick)(ETriggerType TriggerType);
  void (*Trigger_SingleLongStart)(ETriggerType TriggerType);
  void (*Trigger_SingleLongEnd)(ETriggerType TriggerType);
  void (*Trigger_DoubleClick)(ETriggerType TriggerType);
  void (*Trigger_DoubleLongStart)(ETriggerType TriggerType);
  void (*Trigger_DoubleLongEnd)(ETriggerType TriggerType);
  void (*Trigger_TripleClick)(ETriggerType TriggerType);
  void (*MWin_ProgressShow)(const char *TitleStr,s32 Max);
  void (*MWin_ProgressSetPos)(const char *pTitleStr,s32 pos,s32 max);
  void (*MWin_ProgressDraw)(const char *pstr0,const char *pstr1,s32 pos,s32 max);
  void (*MWin_ProgressHide)(void);
  void (*GetArtWork)(void *pFileHandle,u32 Offset,u32 Size);
  void (*ClearArtWork)(void);
  void (*SaveScreenCapture)(void);
  void (*ReqSoftReset)(void);
} TCallBack;

extern void CallBack_ExecuteVBlankHandler(void);
extern TCallBack* CallBack_GetPointer(void);

enum ENextProc {ENP_Loop,ENP_License,ENP_Main,ENP_Help,ENP_Settings};
enum EProcFadeEffect {EPFE_None,EPFE_LeftToRight,EPFE_RightToLeft,EPFE_UpToDown,EPFE_DownToUp,EPFE_CrossFade,EPFE_FastCrossFade};

static void SetNextProc(ENextProc _NextProc,EProcFadeEffect _ProcFadeEffect)
{
  extern ENextProc NextProc;
  extern EProcFadeEffect ProcFadeEffect;
  NextProc=_NextProc;
  ProcFadeEffect=_ProcFadeEffect;
}

static ENextProc GetNextProc(void)
{
  extern ENextProc NextProc;
  return(NextProc);
}

static void SetProcFadeEffect(EProcFadeEffect _ProcFadeEffect)
{
  extern EProcFadeEffect ProcFadeEffect;
  ProcFadeEffect=_ProcFadeEffect;
}

extern void ProcLicense_SetCallBack(TCallBack *pCallBack);
extern void ProcMain_SetCallBack(TCallBack *pCallBack);
extern void ProcHelp_SetCallBack(TCallBack *pCallBack);
extern void ProcSettings_SetCallBack(TCallBack *pCallBack);

extern void ScreenMain_Flip_ProcFadeEffect(void);

extern void DrawItemsListToScreenSub(bool ShowCursor);

#include "unicode.h"

#define MaxFilenameLength (256)

//#define EnableTriggerLog

static inline void CallBack_MWin_ProgressShow(const char *TitleStr,s32 Max)
{
  TCallBack *pcb=CallBack_GetPointer();
  if(pcb->MWin_ProgressShow!=NULL) pcb->MWin_ProgressShow(TitleStr,Max);
}

static inline void CallBack_MWin_ProgressSetPos(const char *pTitleStr,s32 pos,s32 max)
{
  TCallBack *pcb=CallBack_GetPointer();
  if(pcb->MWin_ProgressSetPos!=NULL) pcb->MWin_ProgressSetPos(pTitleStr,pos,max);
}

static inline void CallBack_MWin_ProgressDraw(const char *pstr0,const char *pstr1,s32 pos,s32 max)
{
  TCallBack *pcb=CallBack_GetPointer();
  if(pcb->MWin_ProgressDraw!=NULL) pcb->MWin_ProgressDraw(pstr0,pstr1,pos,max);
}

static inline void CallBack_MWin_ProgressHide(void)
{
  TCallBack *pcb=CallBack_GetPointer();
  if(pcb->MWin_ProgressHide!=NULL) pcb->MWin_ProgressHide();
}

// ---------------------------------------------------------

#define REGIME_Pause(); bool BackupIME=REG_IME; REG_IME=0;
#define REGIME_Resume(); REG_IME=BackupIME;

