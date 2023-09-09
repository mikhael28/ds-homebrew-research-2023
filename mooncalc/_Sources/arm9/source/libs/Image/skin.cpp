
#pragma Ospace

#include <NDS.h>

#include <stdio.h>
#include <stdlib.h>

#include "_console.h"
#include "_consoleWriteLog.h"
#include "_const.h"

#include "skin.h"

#include "maindef.h"
#include "memtool.h"
#include "strtool.h"
#include "procstate.h"
#include "lang.h"
#include "inifile.h"

#include "cstream_fs.h"

#include "zlibhelp.h"

// ---------------------------------------------------------------------------------

#include "skin_SkinFile.h"
#include "skin_CustomBG.h"

// ---------------------------------------------------------------------------------

typedef struct {
  CglB15 *pbm;
} TstructB15;

typedef struct {
  CglTGF *pbm;
} TstructTGF;

#define ComponentSkinAlphaCount (ECSACount)
static TstructTGF ComponentSkinAlpha[ComponentSkinAlphaCount];

#define ScrollBarSkinAlphaCount (ESBSACount)
static TstructTGF ScrollBarSkinAlpha[ScrollBarSkinAlphaCount];

#define SetupSkinCount (ESSCount)
static TstructB15 SetupSkin[SetupSkinCount];
#define SetupSkinAlphaCount (ESSACount)
static TstructTGF SetupSkinAlpha[SetupSkinAlphaCount];

#define SysMenuSkinAlphaCount (ESMSACount)
static TstructTGF SysMenuSkinAlpha[SysMenuSkinAlphaCount];

#define MoviePlayerSkinCount (EMPSCount)
static TstructB15 MoviePlayerSkin[MoviePlayerSkinCount];
#define MoviePlayerSkinAlphaCount (EMPSACount)
static TstructTGF MoviePlayerSkinAlpha[MoviePlayerSkinAlphaCount];

#define ImageViewSkinCount (EIVSCount)
static TstructB15 ImageViewSkin[ImageViewSkinCount];

#define LaunchSkinCount (ELSCount)
static TstructB15 LaunchSkin[LaunchSkinCount];
#define LaunchSkinAlphaCount (ELSACount)
static TstructTGF LaunchSkinAlpha[LaunchSkinAlphaCount];

#define LongTapSkinAlphaCount (ELTSACount)
static TstructTGF LongTapSkinAlpha[LongTapSkinAlphaCount];

#define MP3CntSkinAlphaCount (EMP3SACount)
static TstructTGF MP3CntSkinAlpha[MP3CntSkinAlphaCount];

#define CustomSkinCount (ECSCount)
static TstructB15 CustomSkin[CustomSkinCount];

#define TextViewSkinCount (ETVCount)
static TstructB15 TextViewSkin[TextViewSkinCount];
#define TextViewSkinAlphaCount (ETVACount)
static TstructTGF TextViewSkinAlpha[TextViewSkinAlphaCount];

#define MemoEditSkinCount (EMECount)
static TstructB15 MemoEditSkin[MemoEditSkinCount];
#define MemoEditSkinAlphaCount (EMEACount)
static TstructTGF MemoEditSkinAlpha[MemoEditSkinAlphaCount];

#define MemoListSkinCount (EMLCount)
static TstructB15 MemoListSkin[MemoListSkinCount];

// ---------------------------------------------------------------------------------

static void ComponentAlpha_Init(void)
{
  for(u32 idx=0;idx<ComponentSkinAlphaCount;idx++){
    TstructTGF *ptag=&ComponentSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EComponentSkinAlpha)idx){
      case ECSA_BoxMin: pfn="cmps_BoxMin.tgf"; break;
      case ECSA_BoxPlus: pfn="cmps_BoxPlus.tgf"; break;
      case ECSA_CheckOff: pfn="cmps_ChkOff.tgf"; break;
      case ECSA_CheckOn: pfn="cmps_ChkOn.tgf"; break;
      case ECSA_RadioOff: pfn="cmps_RadioOff.tgf"; break;
      case ECSA_RadioOn: pfn="cmps_RadioOn.tgf"; break;
      case ECSA_Ok: pfn="cmps_Ok.tgf"; break;
      case ECSA_Cancel: pfn="cmps_Cancel.tgf"; break;
      case ECSACount: default: StopFatalError(13701,"ComponentAlpha: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void ComponentAlpha_Free(void)
{
  for(u32 idx=0;idx<ComponentSkinAlphaCount;idx++){
    TstructTGF *ptag=&ComponentSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* ComponentAlpha_GetSkin(EComponentSkinAlpha idx)
{
  TstructTGF *ptag=&ComponentSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void ScrollBarAlpha_Init(void)
{
  for(u32 idx=0;idx<ScrollBarSkinAlphaCount;idx++){
    TstructTGF *ptag=&ScrollBarSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EScrollBarSkinAlpha)idx){
      case ESBSA_BG: pfn="SB_BG.tgf"; break;
      case ESBSA_DownBtn_Normal: pfn="SB_DownBtn_Normal.tgf"; break;
      case ESBSA_DownBtn_Press: pfn="SB_DownBtn_Press.tgf"; break;
      case ESBSA_UpBtn_Normal: pfn="SB_UpBtn_Normal.tgf"; break;
      case ESBSA_UpBtn_Press: pfn="SB_UpBtn_Press.tgf"; break;
      case ESBSA_GripBG_Normal: pfn="SB_GripBG_Normal.tgf"; break;
      case ESBSA_GripBG_Press: pfn="SB_GripBG_Press.tgf"; break;
      case ESBSA_GripBottom_Normal: pfn="SB_GripBottom_Normal.tgf"; break;
      case ESBSA_GripBottom_Press: pfn="SB_GripBottom_Press.tgf"; break;
      case ESBSA_GripTop_Normal: pfn="SB_GripTop_Normal.tgf"; break;
      case ESBSA_GripTop_Press: pfn="SB_GripTop_Press.tgf"; break;
      case ESBSACount: default: StopFatalError(13705,"ScrollBarAlpha: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void ScrollBarAlpha_Free(void)
{
  for(u32 idx=0;idx<ScrollBarSkinAlphaCount;idx++){
    TstructTGF *ptag=&ScrollBarSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* ScrollBarAlpha_GetSkin(EScrollBarSkinAlpha idx)
{
  TstructTGF *ptag=&ScrollBarSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void Setup_Init(void)
{
  for(u32 idx=0;idx<SetupSkinCount;idx++){
    TstructB15 *ptag=&SetupSkin[(u32)idx];
    const char *pfn=NULL;
    switch((ESetupSkin)idx){
      case ESS_BG_Top: pfn="Setup_BG_Top.b15"; break;
      case ESS_BG_Bottom: pfn="Setup_BG_Bottom.b15"; break;
      case ESSCount: default: StopFatalError(13706,"Setup: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void Setup_Free(void)
{
  for(u32 idx=0;idx<SetupSkinCount;idx++){
    TstructB15 *ptag=&SetupSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* Setup_GetSkin(ESetupSkin idx)
{
  TstructB15 *ptag=&SetupSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void SetupAlpha_Init(void)
{
  for(u32 idx=0;idx<SetupSkinAlphaCount;idx++){
    TstructTGF *ptag=&SetupSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ESetupSkinAlpha)idx){
      case ESSA_ChkOverlayOn: pfn="Setup_ChkOverlayOn.tgf"; break;
      case ESSA_RadioOverlayOn: pfn="Setup_RadioOverlayOn.tgf"; break;
      case ESSACount: default: StopFatalError(13707,"SetupAlpha: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void SetupAlpha_Free(void)
{
  for(u32 idx=0;idx<SetupSkinAlphaCount;idx++){
    TstructTGF *ptag=&SetupSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* SetupAlpha_GetSkin(ESetupSkinAlpha idx)
{
  TstructTGF *ptag=&SetupSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void SysMenuAlpha_Init(void)
{
  for(u32 idx=0;idx<SysMenuSkinAlphaCount;idx++){
    TstructTGF *ptag=&SysMenuSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ESysMenuSkinAlpha)idx){
      case ESMSA_BG: pfn="SysMenu_BG.tgf"; break;
      case ESMSA_SelectBar: pfn="SysMenu_SelectBar.tgf"; break;
      case ESMSACount: default: StopFatalError(13708,"SysMenuAlpha: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void SysMenuAlpha_Free(void)
{
  for(u32 idx=0;idx<SysMenuSkinAlphaCount;idx++){
    TstructTGF *ptag=&SysMenuSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* SysMenuAlpha_GetSkin(ESysMenuSkinAlpha idx)
{
  TstructTGF *ptag=&SysMenuSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void MoviePlayer_Init(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinCount;idx++){
    TstructB15 *ptag=&MoviePlayerSkin[(u32)idx];
    const char *pfn=NULL;
    switch((EMoviePlayerSkin)idx){
      case EMPS_bg: pfn="mp_bg.b15"; break;
      case EMPS_seekbar_off: pfn="mp_seekbar_off.b15"; break;
      case EMPS_seekbar_on: pfn="mp_seekbar_on.b15"; break;
      case EMPSCount: default: StopFatalError(13709,"MoviePlayer: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void MoviePlayer_Free(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinCount;idx++){
    TstructB15 *ptag=&MoviePlayerSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* MoviePlayer_GetSkin(EMoviePlayerSkin idx)
{
  TstructB15 *ptag=&MoviePlayerSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void MoviePlayerAlpha_Init(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinAlphaCount;idx++){
    TstructTGF *ptag=&MoviePlayerSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EMoviePlayerSkinAlpha)idx){
      case EMPSA_mode_repeat: pfn="mp_mode_repeat.tgf"; break;
      case EMPSA_mode_allrep: pfn="mp_mode_allrep.tgf"; break;
      case EMPSA_mode_random: pfn="mp_mode_random.tgf"; break;
      case EMPSA_play: pfn="mp_play.tgf"; break;
      case EMPSA_pause: pfn="mp_pause.tgf"; break;
      case EMPSA_stop: pfn="mp_stop.tgf"; break;
      case EMPSA_prev: pfn="mp_prev.tgf"; break;
      case EMPSA_next: pfn="mp_next.tgf"; break;
      case EMPSA_volbar_off: pfn="mp_volbar_off.tgf"; break;
      case EMPSA_volbar_on: pfn="mp_volbar_on.tgf"; break;
      case EMPSA_backlight: pfn="mp_backlight.tgf"; break;
      case EMPSA_seekbargrip: pfn="mp_seekbargrip.tgf"; break;
      case EMPSA_digits: pfn="mp_digits.tgf"; break;
      case EMPSACount: default: StopFatalError(13710,"MoviePlayerAlpha: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void MoviePlayerAlpha_Free(void)
{
  for(u32 idx=0;idx<MoviePlayerSkinAlphaCount;idx++){
    TstructTGF *ptag=&MoviePlayerSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* MoviePlayerAlpha_GetSkin(EMoviePlayerSkinAlpha idx)
{
  TstructTGF *ptag=&MoviePlayerSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void ImageView_Init(void)
{
  for(u32 idx=0;idx<ImageViewSkinCount;idx++){
    TstructB15 *ptag=&ImageViewSkin[(u32)idx];
    const char *pfn=NULL;
    switch((EImageViewSkin)idx){
      case EIVS_OverlayBG: pfn="iv_OverlayBG.b15"; break;
      case EIVS_prgbar_off: pfn="iv_prgbar_off.b15"; break;
      case EIVS_prgbar_on: pfn="iv_prgbar_on.b15"; break;
      case EIVSCount: default: StopFatalError(13711,"ImageView: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void ImageView_Free(void)
{
  for(u32 idx=0;idx<ImageViewSkinCount;idx++){
    TstructB15 *ptag=&ImageViewSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* ImageView_GetSkin(EImageViewSkin idx)
{
  TstructB15 *ptag=&ImageViewSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void Custom_Init(void)
{
  for(u32 idx=0;idx<CustomSkinCount;idx++){
    TstructB15 *ptag=&CustomSkin[(u32)idx];
    const char *pfn=NULL;
    switch((ECustomSkin)idx){
      case ECS_TopMsg: pfn="Custom_TopMsg.b15"; break;
      case ECS_BG: pfn="Custom_BG.b15"; break;
      case ECSCount: default: StopFatalError(13716,"Custom: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void Custom_Free(void)
{
  for(u32 idx=0;idx<CustomSkinCount;idx++){
    TstructB15 *ptag=&CustomSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* Custom_GetSkin(ECustomSkin idx)
{
  TstructB15 *ptag=&CustomSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void TextView_Init(void)
{
  for(u32 idx=0;idx<TextViewSkinCount;idx++){
    TstructB15 *ptag=&TextViewSkin[(u32)idx];
    const char *pfn=NULL;
    switch((ETextViewSkin)idx){
      case ETV_PageBG: pfn="TV_PageBG.b15"; break;
      case ETV_Bookmark_LoadBG: pfn="TV_Bookmark_LoadBG.b15"; break;
      case ETV_Bookmark_SaveBG: pfn="TV_Bookmark_SaveBG.b15"; break;
      case ETV_Bookmark_PreviewBG: pfn="TV_Bookmark_PreviewBG.b15"; break;
      case ETVCount: default: StopFatalError(13719,"TextView: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void TextView_Free(void)
{
  for(u32 idx=0;idx<TextViewSkinCount;idx++){
    TstructB15 *ptag=&TextViewSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* TextView_GetSkin(ETextViewSkin idx)
{
  TstructB15 *ptag=&TextViewSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void TextViewAlpha_Init(void)
{
  for(u32 idx=0;idx<TextViewSkinAlphaCount;idx++){
    TstructTGF *ptag=&TextViewSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((ETextViewSkinAlpha)idx){
      case ETVA_Bookmark_Cursor: pfn="TV_Bookmark_Cursor.tgf"; break;
      case ETVA_Bookmark_Clear: pfn="TV_Bookmark_Clear.tgf"; break;
      case ETVA_Bookmark_Slot0Icon: pfn="TV_Bookmark_Slot0Icon.tgf"; break;
      case ETVA_Bookmark_Slot1Icon: pfn="TV_Bookmark_Slot1Icon.tgf"; break;
      case ETVA_Bookmark_Slot2Icon: pfn="TV_Bookmark_Slot2Icon.tgf"; break;
      case ETVA_Bookmark_Slot3Icon: pfn="TV_Bookmark_Slot3Icon.tgf"; break;
      case ETVACount: default: StopFatalError(13720,"TextViewAlpha: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void TextViewAlpha_Free(void)
{
  for(u32 idx=0;idx<TextViewSkinAlphaCount;idx++){
    TstructTGF *ptag=&TextViewSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* TextViewAlpha_GetSkin(ETextViewSkinAlpha idx)
{
  TstructTGF *ptag=&TextViewSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void MemoEdit_Init(void)
{
  for(u32 idx=0;idx<MemoEditSkinCount;idx++){
    TstructB15 *ptag=&MemoEditSkin[(u32)idx];
    const char *pfn=NULL;
    switch((EMemoEditSkin)idx){
      case EME_BGTop: pfn="MemoEdit_BGTop.b15"; break;
      case EME_BGBottom: pfn="MemoEdit_BGBottom.b15"; break;
      case EMECount: default: StopFatalError(13721,"MemoEdit: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void MemoEdit_Free(void)
{
  for(u32 idx=0;idx<MemoEditSkinCount;idx++){
    TstructB15 *ptag=&MemoEditSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* MemoEdit_GetSkin(EMemoEditSkin idx)
{
  TstructB15 *ptag=&MemoEditSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void MemoEditAlpha_Init(void)
{
  for(u32 idx=0;idx<MemoEditSkinAlphaCount;idx++){
    TstructTGF *ptag=&MemoEditSkinAlpha[(u32)idx];
    const char *pfn=NULL;
    switch((EMemoEditSkinAlpha)idx){
      case EMEA_FileInfoBG: pfn="MemoEdit_FileInfoBG.tgf"; break;
      case EMEACount: default: StopFatalError(13722,"MemoEditAlpha: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadTGF(pfn,&ptag->pbm);
  }
}

static void MemoEditAlpha_Free(void)
{
  for(u32 idx=0;idx<MemoEditSkinAlphaCount;idx++){
    TstructTGF *ptag=&MemoEditSkinAlpha[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglTGF* MemoEditAlpha_GetSkin(EMemoEditSkinAlpha idx)
{
  TstructTGF *ptag=&MemoEditSkinAlpha[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

static void MemoList_Init(void)
{
  for(u32 idx=0;idx<MemoListSkinCount;idx++){
    TstructB15 *ptag=&MemoListSkin[(u32)idx];
    const char *pfn=NULL;
    switch((EMemoListSkin)idx){
      case EML_BG: pfn="MemoList_BG.b15"; break;
      case EMLCount: default: StopFatalError(0,"MemoList: Unknown type. %d\n",idx); break;
    }
    SkinFile_LoadB15(pfn,&ptag->pbm);
  }
}

static void MemoList_Free(void)
{
  for(u32 idx=0;idx<MemoListSkinCount;idx++){
    TstructB15 *ptag=&MemoListSkin[(u32)idx];
    if(ptag->pbm!=NULL){
      delete ptag->pbm; ptag->pbm=NULL;
    }
  }
}

CglB15* MemoList_GetSkin(EMemoListSkin idx)
{
  TstructB15 *ptag=&MemoListSkin[(u32)idx];
  return(ptag->pbm);
}

// ---------------------------------------------------------------------------------

TSkin_OwnerDrawText Skin_OwnerDrawText;
TSkin_Calender Skin_Calender;
TColorTable ColorTable;

#include "skin_colortbl.h"

// ---------------------------------------------------------------------------------

bool Skin_SetFilename(const UnicodeChar *pFullPath)
{
  static UnicodeChar DefaultFilenameW[64];
  StrConvert_Ank2Unicode(DefaultInternalPath "/default.skn",DefaultFilenameW);
  if(Unicode_isEqual_NoCaseSensitive(pFullPath,DefaultFilenameW)==true){
    char fna[64];
    UnicodeChar fnw[64];
    snprintf(fna,64-1,DefaultInternalPath "/def_%s.skn",Shell_GetCodePageStr());
    StrConvert_Ank2Unicode(fna,fnw);
    if(FullPath_FileExistsUnicode(fnw)==false){
      _consolePrintf("Not found alias skin file '%s'.\n",fna);
      }else{
      _consolePrintf("Default skin alias to '%s'.\n",fna);
      Unicode_Copy(DefaultFilenameW,fnw);
      pFullPath=DefaultFilenameW;
    }
  }
  
  if(SkinFile_Check(pFullPath)==false){
    _consolePrintf("Skin file check failed.\n");
    return(false);
  }
  
  SkinFile_Close();
  
  if(SkinFile_Open(pFullPath)==false){
    _consolePrintf("Skin file open failed.\n");
    return(false);
  }
  
  LoadColorTable_colortbl_ini();
  
  return(true);
}

void Skin_CloseFile(void)
{
  SkinFile_Close();
}

void Skin_Load_ChkDsk(void)
{
}

void Skin_Load_License(void)
{
}

void Skin_Load_Main(void)
{
  ComponentAlpha_Init();
}

void Skin_Load_Help(void)
{
}

void Skin_Load_Settings(void)
{
  ComponentAlpha_Init();
}

void Skin_Free(void)
{
  ComponentAlpha_Free();
  ScrollBarAlpha_Free();
  Setup_Free();
  SetupAlpha_Free();
  SysMenuAlpha_Free();
  MoviePlayer_Free();
  MoviePlayerAlpha_Free();
  ImageView_Free();
  Custom_Free();
  TextView_Free();
  TextViewAlpha_Free();
  MemoEdit_Free();
  MemoEditAlpha_Free();
  MemoList_Free();
}

void DrawSkin(CglB15 *psrcbm,CglCanvas *pdstbm,s32 x,s32 y)
{
  psrcbm->pCanvas->BitBlt(pdstbm,x,y,psrcbm->GetWidth(),psrcbm->GetHeight(),0,0,false);
}

void DrawSkinAlpha(CglTGF *psrcbm,CglCanvas *pdstbm,s32 x,s32 y)
{
  psrcbm->BitBlt(pdstbm,x,y);
}

//-----------------------------------------------------------------------------------------

