#pragma once

#include "__debug.h"

#define ExceptWAVE
#define ExceptG721

#include "nds.h"
#include "fat2.h"

typedef struct {
  bool (*Start)(FAT_FILE *pFileHandle);
  void (*Free)(void);
  bool (*isComplexDecoder)(void);
  
  u32 (*Update)(u32 *plrbuf);
  
  s32 (*GetPosMax)(void);
  s32 (*GetPosOffset)(void);
  void (*SetPosOffset)(s32 ofs);
  u32 (*GetChannelCount)(void);
  u32 (*GetSampleRate)(void);
  u32 (*GetSamplePerFrame)(void);
  u32 (*GetPlayTimeSec)(void);
} Tlibsnd_Internal;

#define DATA_IN_DTCM_WAVE
#define DATA_IN_DTCM_G721
