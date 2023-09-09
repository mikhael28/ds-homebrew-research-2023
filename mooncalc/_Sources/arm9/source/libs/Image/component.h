#pragma once


#include <NDS.h>

#include "glib.h"

#include "rect.h"

extern void Component_Init(void);
extern void Component_SetFont(CglFont *pFont);

typedef struct {
  void (*CallBack_Click)(void *pComponentLabel);
  CglCanvas *pCanvas;
  TRect Rect;
  bool Visible;
  bool isTitle;
  bool Center;
  u16 TextColor;
  const char *pMsgUTF8;
} TComponentLabel;

extern void ComponentLabel_Init(TComponentLabel *pComponentLabel,CglCanvas *pCanvas);
extern void ComponentLabel_Draw(TComponentLabel *pComponentLabel);
extern s32 ComponentLabel_GetWidth(TComponentLabel *pComponentLabel);
extern s32 ComponentLabel_GetIndexFromPos(TComponentLabel *pComponentLabel,s32 mx,s32 my);
extern bool ComponentLabel_MouseUp(TComponentLabel *pComponentLabel,s32 mx,s32 my);

typedef struct {
  void (*CallBack_Click)(void *pComponentCheck);
  u32 UserData;
  CglCanvas *pCanvas;
  CglTGF *pOnIcon,*pOffIcon;
  bool Checked;
  TRect Rect;
  bool Visible;
  const char *pMsgUTF8;
  u16 TextColor;
} TComponentCheck;

extern void ComponentCheck_Init(TComponentCheck *pComponentCheck,CglCanvas *pCanvas);
extern void ComponentCheck_Draw(TComponentCheck *pComponentCheck);
extern s32 ComponentCheck_GetWidth(TComponentCheck *pComponentCheck);
extern bool ComponentCheck_MouseUp(TComponentCheck *pComponentCheck,s32 mx,s32 my);

typedef struct {
  CglCanvas *pCanvas;
  CglTGF *pSkin;
  CglTGF *pIcon;
  bool DrawFrame;
  bool Pressing;
  TRect Rect;
  const char *pMsgUTF8;
  u16 NormalTextColor,PressTextColor;
  bool Visible;
  void (*CallBack_Click)(void *pComponentButton);
  u32 UserData;
} TComponentButton;

extern void ComponentButton_Init(TComponentButton *pComponentButton,CglCanvas *pCanvas);
extern void ComponentButton_Draw(TComponentButton *pComponentButton);
extern s32 ComponentButton_GetIndexFromPos(TComponentButton *pComponentButton,s32 mx,s32 my);
extern bool ComponentButton_MouseUp(TComponentButton *pComponentButton,s32 mx,s32 my);

extern void Component_AutoLocation(u32 x,u32 y0,u32 y1,TComponentLabel *pc0,TComponentCheck *pc1,TComponentCheck *pc2,TComponentCheck *pc3,TComponentCheck *pc4);

