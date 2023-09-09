#pragma once


typedef struct {
  bool DetailsDebugLog;
} TiniSystem;

typedef struct {
  bool SlowDiskAccess;
} TiniDiskAdapter;

typedef struct {
  u32 DelayCount;
  u32 RateCount;
} TiniKeyRepeat;

typedef struct {
  TiniSystem System;
  TiniDiskAdapter DiskAdapter;
  TiniKeyRepeat KeyRepeat;
} TGlobalINI;

extern TGlobalINI GlobalINI;

extern void LoadINI(void);

