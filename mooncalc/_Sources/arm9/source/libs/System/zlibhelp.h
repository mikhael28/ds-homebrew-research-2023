#pragma once


#include <NDS.h>

#define ZLIBTitle "ZLIB (C) 1995-2004 Jean-loup Gailly and Mark Adler"

typedef struct {
  u32 SrcSize;
  u8 *pSrcBuf;
  u32 DstSize;
  u8 *pDstBuf;
} TZLIBData;

//#define ZLIBCOMPRESS

#ifdef ZLIBCOMPRESS
extern bool zlibcompress(TZLIBData *pZLIBData,u32 LimitSize);
#endif

extern bool zlibdecompress(TZLIBData *pZLIBData);

