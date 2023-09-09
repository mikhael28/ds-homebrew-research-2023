#pragma once


#include "unicode.h"
#include "shell.h"

extern void ProcState_Init(void);
extern void ProcState_Free(void);
extern void ProcState_Clear(void);
extern void ProcState_Load(void);
extern void ProcState_Save(void);

typedef struct {
  enum ELang {EL_ENG,EL_JPN};
  ELang Lang;
  bool ClickSound;
  bool EnableFadeEffect;
  u32 AudioVolume64;
  u32 BacklightLevel;
  UnicodeChar SkinFilenameUnicode[MaxFilenameLength];
  enum ELRKeyLock {ELRKL_AlwayOff,ELRKL_RelationalPanel,ELRKL_AlwayOn};
  ELRKeyLock LRKeyLock;
  bool Use24hFormat;
  bool FirstStart;
  enum EVolume {EV_Mute,EV_Low,EV_Mid,EV_High};
  EVolume VolBGM,VolSE;
} TProcState_System;

typedef struct {
  bool ShowBin;
  enum EShowPrec {ESP_D1,ESP_D2,ESP_D3};
  EShowPrec ShowPrec;
  char ExpStr[256];
  u32 ExpCur;
} TProcState_Main;

typedef struct {
  u32 Version;
  TProcState_System System;
  TProcState_Main Main;
  u8 PaddingForSectorAlign[512];
} TProcState;

extern TProcState ProcState;

extern bool ProcState_RequestSave;

extern void ApplyCurrentBacklightLevel(void);
extern void ChangePrevBacklightLevel(void);
extern void ChangeNextBacklightLevel(void);

enum EBGBMPType {EBGBT_None=0,EBGBT_8bit=1,EBGBT_15bit=2};

extern u32 GetBGMVolume128(void);
extern u32 GetSEVolume128(void);
