#pragma once


#include "unicode.h"

#include "glib.h"

#define DefaultImplantCharsCount (71)
#define DefaultImplantChars "0123456789.,()+-/*=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ "

class CFont
{
public:
  enum EBitmapMode {EBM_B15,EBM_TGF};
private:
  EBitmapMode BitmapMode;
  
  CglB15 *pB15;
  CglTGF *pTGF;
  
  u32 ImplantCharsCount;
  const char *pImplantChars;
  
  u32 CharHeight,CharWidth;
  
  u32 CharWidths[128];
  s32 CharPads[128];
  
  u32 SpaceWidth;
  
  CFont(const CFont&);
  CFont& operator=(const CFont&);
protected:
public:
  CFont(EBitmapMode _BitmapMode,CglB15 *_pB15,CglTGF *_pTGF,u32 _ImplantCharsCount,const char *_pImplantChars);
  ~CFont(void);
  u32 DrawChar(CglCanvas *pCanvas,u32 x,u32 y,char c);
  u32 DrawCharToBlack(CglCanvas *pCanvas,u32 x,u32 y,char c);
  u32 DrawText(CglCanvas *pCanvas,u32 x,u32 y,const char *pstr);
  u32 GetTextWidth(const char *pstr);
  u32 GetTextHeight(void);
};

