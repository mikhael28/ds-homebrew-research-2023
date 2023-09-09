#pragma once


extern void Lang_Load(void);
extern void Lang_Free(void);

extern const char* Lang_GetUTF8_internal(const char *pItemName,const char *pErrorMsg);

//#define Lang_GetUTF8(name) Lang_GetUTF8_internal(name,"LangErr:" name)
#define Lang_GetUTF8(name) Lang_GetUTF8_internal(name,name)

#define Lang_GetPure(x) x

#include "lngmsg.h"

extern void Lang_GetENJPSplit(char *pdst,const char *pstr);

