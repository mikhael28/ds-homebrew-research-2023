#pragma once

#include "tglunicode.h"
#include "memtool.h"

typedef u16 UnicodeChar;

extern bool Unicode_isEqual(const UnicodeChar *s1,const UnicodeChar *s2);
extern bool Unicode_isEqual_NoCaseSensitive(const UnicodeChar *s1,const UnicodeChar *s2);
extern void Unicode_Add(UnicodeChar *s1,const UnicodeChar *s2);
extern void Unicode_Copy(UnicodeChar *tag,const UnicodeChar *src);
extern u32 Unicode_GetLength(const UnicodeChar *s);

extern UnicodeChar* Unicode_AllocateCopy(TMM *pMM,const UnicodeChar *src);
extern UnicodeChar* Unicode_AllocateCopyFromUTF8(TMM *pMM,const char *psrcstr);

extern void StrConvert_Ank2Unicode(const char *srcstr,UnicodeChar *dststr);
extern void StrConvert_UTF82Unicode(const char *srcstr,UnicodeChar *dststr);

static inline bool Unicode_isEmpty(const UnicodeChar *psrc)
{
  if(psrc==NULL) return(true);
  if(psrc[0]==0) return(true);
  return(false);
}

extern const char* StrConvert_Unicode2Ank_Test(const UnicodeChar *srcstr);

