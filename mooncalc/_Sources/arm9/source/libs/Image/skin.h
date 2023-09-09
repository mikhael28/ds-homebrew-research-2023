#pragma once


#include <NDS.h>
#include "unicode.h"
#include "glib.h"
#include "procstate.h"

#define CustomBG_Height (512)

extern bool Skin_SetFilename(const UnicodeChar *pFullPath);
extern void Skin_CloseFile(void);

extern void Skin_Load_License(void);
extern void Skin_Load_Main(void);
extern void Skin_Load_Skin(void);
extern void Skin_Load_Settings(void);

extern void Skin_Free(void);

// -----------------------------

enum EComponentSkinAlpha {ECSA_BoxMin,ECSA_BoxPlus,
                          ECSA_CheckOff,ECSA_CheckOn,
                          ECSA_RadioOff,ECSA_RadioOn,
                          ECSA_Ok,ECSA_Cancel,ECSACount};

extern CglTGF* ComponentAlpha_GetSkin(EComponentSkinAlpha ComponentSkinAlpha);

// -----------------------------

enum EScrollBarSkinAlpha {ESBSA_BG,
                          ESBSA_DownBtn_Normal,ESBSA_DownBtn_Press,ESBSA_UpBtn_Normal,ESBSA_UpBtn_Press,
                          ESBSA_GripBG_Normal,ESBSA_GripBG_Press,
                          ESBSA_GripBottom_Normal,ESBSA_GripBottom_Press,ESBSA_GripTop_Normal,ESBSA_GripTop_Press,
                          ESBSACount};

extern CglTGF* ScrollBarAlpha_GetSkin(EScrollBarSkinAlpha ScrollBarSkinAlpha);

// -----------------------------

enum ESetupSkin {ESS_BG_Top,ESS_BG_Bottom,
                 ESSCount};

extern CglB15* Setup_GetSkin(ESetupSkin SetupSkin);

enum ESetupSkinAlpha {ESSA_ChkOverlayOn,ESSA_RadioOverlayOn,
                      ESSACount};

extern CglTGF* SetupAlpha_GetSkin(ESetupSkinAlpha SetupSkinAlpha);

// -----------------------------

enum ESysMenuSkinAlpha {ESMSA_BG,ESMSA_SelectBar,
                        ESMSACount};

extern CglTGF* SysMenuAlpha_GetSkin(ESysMenuSkinAlpha SysMenuSkinAlpha);

// -----------------------------

enum EMoviePlayerSkin {EMPS_bg,
                       EMPS_seekbar_off,EMPS_seekbar_on,
                       EMPSCount};

extern CglB15* MoviePlayer_GetSkin(EMoviePlayerSkin MoviePlayerSkin);

enum EMoviePlayerSkinAlpha {EMPSA_mode_repeat,EMPSA_mode_allrep,EMPSA_mode_random,
                            EMPSA_play,EMPSA_pause,EMPSA_stop,EMPSA_prev,EMPSA_next,
                            EMPSA_volbar_off,EMPSA_volbar_on,
                            EMPSA_backlight,
                            EMPSA_seekbargrip,
                            EMPSA_digits,
                            EMPSACount};

extern CglTGF* MoviePlayerAlpha_GetSkin(EMoviePlayerSkinAlpha MoviePlayerSkinAlpha);

// -----------------------------

enum EImageViewSkin {EIVS_OverlayBG,EIVS_prgbar_off,EIVS_prgbar_on,
                     EIVSCount};

extern CglB15* ImageView_GetSkin(EImageViewSkin ImageViewSkin);

// -----------------------------

enum ELaunchSkin {ELS_BGTop,ELS_BGBottom,
                  ELSCount};

extern CglB15* Launch_GetSkin(ELaunchSkin LaunchSkin);

enum ELaunchSkinAlpha {ELSA_Item_ClearBG,ELSA_Item_SelectBG,
                       ELSA_Tab0_Launch,ELSA_Tab1_NDS,
                       ELSA_FileInfoFrame,
                       ELSACount};

extern CglTGF* LaunchAlpha_GetSkin(ELaunchSkinAlpha LaunchSkinAlpha);

// -----------------------------

enum ELongTapSkinAlpha {ELTSA_f0,ELTSA_f1,ELTSA_f2,ELTSA_f3,ELTSA_f4,ELTSA_f5,ELTSA_f6,ELTSA_f7,
                        ELTSACount};

extern CglTGF* LongTapAlpha_GetSkin(ELongTapSkinAlpha LongTapSkinAlpha);

// -----------------------------

enum EMP3CntSkinAlpha {EMP3SA_p0,
                       EMP3SA_p1_prev,
                       EMP3SA_p2_stop,EMP3SA_p2_play,EMP3SA_p2_pause,
                       EMP3SA_p3_next,
                       EMP3SA_p4_repeat,EMP3SA_p4_allrep,EMP3SA_p4_shuffle,
                       EMP3SACount};

extern CglTGF* MP3CntAlpha_GetSkin(EMP3CntSkinAlpha MP3CntSkinAlpha);

// -----------------------------

enum ECustomSkin {ECS_TopMsg,ECS_BG,
                  ECSCount};

extern CglB15* Custom_GetSkin(ECustomSkin CustomSkin);

// -----------------------------

enum ETextViewSkin {ETV_PageBG,ETV_Bookmark_LoadBG,ETV_Bookmark_SaveBG,ETV_Bookmark_PreviewBG,
                    ETVCount};

extern CglB15* TextView_GetSkin(ETextViewSkin TextViewSkin);

enum ETextViewSkinAlpha {ETVA_Bookmark_Cursor,ETVA_Bookmark_Clear,ETVA_Bookmark_Slot0Icon,ETVA_Bookmark_Slot1Icon,ETVA_Bookmark_Slot2Icon,ETVA_Bookmark_Slot3Icon,
                         ETVACount};

extern CglTGF* TextViewAlpha_GetSkin(ETextViewSkinAlpha TextViewSkinAlpha);

// -----------------------------

enum EMemoEditSkin {EME_BGTop,EME_BGBottom,
                    EMECount};

extern CglB15* MemoEdit_GetSkin(EMemoEditSkin MemoEditSkin);

enum EMemoEditSkinAlpha {EMEA_FileInfoBG,
                         EMEACount};

extern CglTGF* MemoEditAlpha_GetSkin(EMemoEditSkinAlpha MemoEditSkinAlpha);

// -----------------------------

enum EMemoListSkin {EML_BG,
                    EMLCount};

extern CglB15* MemoList_GetSkin(EMemoListSkin MemoListSkin);

// -----------------------------

extern void DrawSkin(CglB15 *psrcbm,CglCanvas *pdstbm,s32 x,s32 y);
extern void DrawSkinAlpha(CglTGF *psrcbm,CglCanvas *pdstbm,s32 x,s32 y);

// -----------------------------

extern bool Skin_BGPack_SSApply;
extern bool Skin_BGPack_SSUseFilter;
extern bool Skin_BGPack_SSNotDrawDays;
extern u32 Skin_BGPack_AutoChangeTimeSec;

typedef struct {
  bool Setup_Top;
  bool Setup_Bottom;
  bool BookmarkMenu;
  bool FileList_Top;
  bool Custom_Top;
  bool DisableParticles;
} TSkin_OwnerDrawText;

extern TSkin_OwnerDrawText Skin_OwnerDrawText;

typedef struct {
  bool Enabled;
  u32 PosX,PosY;
} TSkin_Calender;

extern TSkin_Calender Skin_Calender;

typedef struct {
  u16 HelpTop_Text;
  u16 HelpBody_Text;
  u16 Label_Text;
  u16 Check_Text;
  u16 Button_NormalText;
  u16 Button_PressText;
} TColorTable_Setup;

typedef struct {
  u16 HelpTop_Text;
  u16 HelpBody_Text;
  u16 Label_Text;
  u16 TitleLabel_Text;
  u16 Check_Text;
  u16 Button_NormalHighlight;
  u16 Button_NormalShadow;
  u16 Button_NormalBG;
  u16 Button_NormalText;
  u16 Button_PressHighlight;
  u16 Button_PressShadow;
  u16 Button_PressBG;
  u16 Button_PressText;
  u16 SoftwareScrollBar_Frame;
  u16 SoftwareScrollBar_BG;
} TColorTable_Component;

typedef struct {
  u16 MessageText;
} TColorTable_ErrorDialog;

typedef struct {
  u16 HelpTop_Text;
  u16 HelpBody_Text;
  u16 DeleteFileDialog_Title_Text;
  u16 DeleteFileDialog_Body_Text;
  u16 FolderNameText;
  u16 FileNameText;
  u16 SelectText;
  u16 PopupBG;
  u16 PopupFrame;
  u16 PopupText;
  u16 ID3TagText;
  u16 ID3TagProgressBarAdd;
} TColorTable_FileList;

typedef struct {
  u16 BG;
  u16 Label_Text;
  u16 TitleLabel_Text;
  u16 Check_Text;
} TColorTable_ScreenSaverCustom;

typedef struct {
  u16 EmptyText;
  u16 FileNameText;
  u16 NDSFileInfoText;
  u16 SaveFileInfoText;
} TColorTable_Launch;

typedef struct {
  u16 ItemText;
  u16 ItemSelectText;
} TColorTable_SystemMenu;

typedef struct {
  u16 InitBG;
  u16 FilenameShadow;
  u16 FilenameText;
  u16 VolumeShadow;
  u16 VolumeText;
  u16 VolumeMaxShadow;
  u16 VolumeMaxText;
  u16 InfoBG;
  u16 InfoText;
  u16 FrameCacheOff;
  u16 FrameCacheOn;
  u16 FrameCacheWarn;
} TColorTable_Video;

typedef struct {
  u16 MainText;
  u16 ScrollBar_Frame;
  u16 ScrollBar_Inside;
  u16 BookmarkMenu_TitleText;
  u16 BookmarkMenu_Empty;
  u16 BookmarkMenu_Exists;
  u16 PreviewText;
  u16 Line;
  u16 Bookmark_FillBox[4];
} TColorTable_TextView;

typedef struct {
  u16 BG_Sunday;
  u16 BG_Satday;
  u16 BG_Today;
  u16 BG_Normal;
  u16 FrameOutColor;
  u16 FrameInColor;
} TColorTable_Calender;

typedef struct {
  u16 TitleText;
  u16 FileNumText;
  u16 HelpText_Normal;
  u16 HelpText_Shadow;
  u16 FilenameText;
  u16 SelectText;
} TColorTable_MemoList;

typedef struct {
  u16 HelpText_Normal;
  u16 HelpText_Shadow;
  u16 FileInfo_Normal;
  u16 FileInfo_Shadow;
} TColorTable_MemoEdit;

typedef struct {
  TColorTable_Setup Setup;
  TColorTable_Component Component;
  TColorTable_ErrorDialog ErrorDialog;
  TColorTable_FileList FileList;
  TColorTable_ScreenSaverCustom ScreenSaverCustom;
  TColorTable_Launch Launch;
  TColorTable_SystemMenu SystemMenu;
  TColorTable_Video Video;
  TColorTable_TextView TextView;
  TColorTable_Calender Calender_Normal,Calender_Digital,Calender_Extend;
  TColorTable_MemoList MemoList;
  TColorTable_MemoEdit MemoEdit;
} TColorTable;

extern TColorTable ColorTable;

