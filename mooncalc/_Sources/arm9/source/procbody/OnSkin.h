
#pragma once

#include "glib.h"

extern void OnSkin_Exclusive_LoadBMP(CglCanvas *pcan,const char *pfn);

extern void OnSkin_Init(const char *pbgfn);
extern void OnSkin_Free(void);

extern CglCanvas *pSkinBG;

extern void OnSkin_FillBox25per(CglCanvas *pcan,u32 x,u32 y,u32 w,u32 h,u16 Color);
extern void OnSkin_FillBox50per(CglCanvas *pcan,u32 x,u32 y,u32 w,u32 h,u16 Color);
extern void OnSkin_FillBox75per(CglCanvas *pcan,u32 x,u32 y,u32 w,u32 h,u16 Color);
