#pragma once


static inline void VRAMWriteCache_Enable(void)
{
  if(GlobalINI.System.VRAMCacheEnabled==false) return;
  int r0=7;
  __asm {
    mcr p15, 0, r0, c3, c0, 0
  }
}

static inline void VRAMWriteCache_Disable(void)
{
  if(GlobalINI.System.VRAMCacheEnabled==false) return;
  int r0=6;
  __asm {
    mcr p15, 0, r0, c3, c0, 0
  }
}

