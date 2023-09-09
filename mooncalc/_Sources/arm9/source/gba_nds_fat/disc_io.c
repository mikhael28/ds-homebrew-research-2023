/*

	disc_io.c

	uniformed io-interface to work with Chishm's FAT library

	Written by MightyMax
  
	Modified by Chishm:
	2005-11-06
		* Added WAIT_CR modifications for NDS

	Modified by www.neoflash.com:
	2006-02-03
		* Added SUPPORT_* defines, comment out any of the SUPPORT_* defines in disc_io.h to remove support
		  for the given interface and stop code being linked to the binary

	    * Added support for MK2 MMC interface

		* Added disc_Cache* functions

	Modified by Chishm:
	2006-02-05
		* Added Supercard SD support
*/

#include <nds.h>
#include <stdio.h>

#include "disc_io.h"

#include "maindef.h"
#include "memtool.h"
#include "_console.h"

// Include known io-interfaces:
#include "io_dldi.h"

// Keep a pointer to the active interface
LPIO_INTERFACE active_interface = NULL;


bool disc_isGBACartridge;

/*

	Hardware level disc funtions

*/

#include "_console.h"
#include "_const.h"

#include "mediatype.h"

u32 DIMediaType=DIMT_NONE;
const char *DIMediaName="No adapter.";
char DIMediaID[5]="NONE";

#define SystemCacheCount (64)
//#define SystemCacheCount (512)
#define SystemCacheCountMask (SystemCacheCount-1)

typedef struct {
  u32 RingIndex;
  u32 SectorIndex[SystemCacheCount];
  u32 SectorData[(512/4)*SystemCacheCount];
} TSystemCache;

static TSystemCache SystemCache;

#define GetSectorDataPointer(secidx) (&SystemCache.SectorData[(512/4)*secidx])

static void disc_SystemCache_ClearAll(void)
{
  _consolePrintf("Clear system cache.\n");
  
  TSystemCache *psc=&SystemCache;
  psc->RingIndex=0;
  for(u32 idx=0;idx<SystemCacheCount;idx++){
    psc->SectorIndex[idx]=(u32)-1;
  }
  MemSet32CPU(0,psc->SectorData,512*SystemCacheCount);
}

void DISCIO_ShowAdapterInfo(void)
{
  if(active_interface==NULL) StopFatalError(11801,"DISCIO_ShowAdapterInfo: Not ready active interface.\n");
  _consolePrintf("DLDI: active_interface= 0x%08x, ID= %s, Name= %s.\n",active_interface,DIMediaID,DIMediaName);
  _consolePrintf("active_interface->ul_Features= 0x%02x.\n",active_interface->ul_Features);
  _consolePrintf("BlockWriteLimitSectorsCount=%d.\n",BlockWriteLimitSectorsCount);
}

#include "maindef.h"
bool disc_Init(void) 
{
  disc_SystemCache_ClearAll();
  
  if(active_interface!=NULL) return(true);
  
  REG_EXMEMCNT &= ~(ARM7_OWNS_ROM | ARM7_OWNS_CARD);
  
  active_interface=DLDI_GetInterface();
  if(active_interface==NULL) StopFatalError(0,"could not find a working IO Interface");
  
  DIMediaType=active_interface->ul_ioType;
  DIMediaName=DLDI_GetAdapterName();
  
  {
    char *p=(char*)&active_interface->ul_ioType;
    DIMediaID[0]=p[0];
    DIMediaID[1]=p[1];
    DIMediaID[2]=p[2];
    DIMediaID[3]=p[3];
    DIMediaID[4]=0;
  }
  
  if((active_interface->ul_Features&FEATURE_SLOT_GBA)!=0){
    disc_isGBACartridge=false;
    }else{
    disc_isGBACartridge=true;
  }
  
  DISCIO_ShowAdapterInfo();
  
  _consolePrintf("DLDI driver stack check.\n");
  DTCM_StackCheck(1);
  if(active_interface->fn_StartUp()==false) StopFatalError(0,"DLDI start-up failed.");
  DTCM_StackCheck(2);
  u32 buf[512/4];
  active_interface->fn_ReadSectors(8192,1,buf);
  DTCM_StackCheck(3);
  
  return(true);
} 

bool disc_IsInserted(void) 
{
  if(active_interface==NULL) return(false);
  return(active_interface->fn_IsInserted());
} 

//#define saferw

#ifdef saferw
static u32 saferwtmp[512/4];
#endif

bool disc_ReadSectors(u32 sector, u8 numSecs, void* buffer) 
{
//  _consolePrintf("RSs:%d,%d\n",sector,numSecs);
  if(active_interface==NULL) return(false);
  
#ifdef saferw
  {
    u8 *ptmp=(u8*)saferwtmp;
    u8 *pbuf=(u8*)buffer;
    while(numSecs!=0){
      active_interface->fn_ReadSectors(sector,1,ptmp);
      for(u32 idx=0;idx<512;idx++){
        pbuf[idx]=ptmp[idx];
      }
      pbuf+=512;
      sector++;
      numSecs--;
    }
    return(true);
  }
#endif
  
  return(active_interface->fn_ReadSectors(sector,numSecs,buffer));
} 

bool disc_WriteSectors(u32 sector, u8 numSecs, const void* buffer) 
{
//  _consolePrintf("WSs:%d,%d\n",sector,numSecs);
  if(active_interface==NULL) return(false);
  
#ifdef saferw
  {
    u8 *ptmp=(u8*)saferwtmp;
    u8 *pbuf=(u8*)buffer;
    while(numSecs!=0){
      for(u32 idx=0;idx<512;idx++){
        ptmp[idx]=pbuf[idx];
      }
      active_interface->fn_WriteSectors(sector,1,ptmp);
      pbuf+=512;
      sector++;
      numSecs--;
    }
    return(true);
  }
#endif
  
  return(active_interface->fn_WriteSectors(sector,numSecs,buffer));
} 

static bool SystemWriteCache_Enabled=false;
static u32 SystemWriteCache_TargetSectorIndex=(u32)-1;
static u32 SystemWriteCache_Data[512/4];

void disc_SystemReadSector(u32 sector, void* buffer)
{
//  _consolePrintf("SR:%d ",sector);
  REGIME_Pause();
  
  if(active_interface==NULL) StopFatalError(0,"Not startupped.");
  
  if(SystemWriteCache_Enabled==true){
    if(SystemWriteCache_TargetSectorIndex==sector){
//      _consolePrintf("[ReadCacheFromWriteCache:%d]",sector);
      u32 *pdst=(u32*)buffer;
      for(u32 idx=0;idx<512/4;idx++){
        pdst[idx]=SystemWriteCache_Data[idx];
      }
      REGIME_Resume();
      return;
    }
  }
  
  TSystemCache *psc=&SystemCache;
  
  for(u32 idx=0;idx<SystemCacheCount;idx++){
    if(sector==psc->SectorIndex[idx]){
//      _consolePrintf("[ReadCacheHit:%d,%d]",idx,sector);
      MemCopy32CPU(GetSectorDataPointer(idx),buffer,512);
      REGIME_Resume();
      return;
    }
  }
  
//  _consolePrintf("[ReadCacheMiss:%d,%d]",psc->RingIndex,sector);
  psc->SectorIndex[psc->RingIndex]=sector;
  active_interface->fn_ReadSectors(sector,1,buffer);
  MemCopy32CPU(buffer,GetSectorDataPointer(psc->RingIndex),512);
  psc->RingIndex=(psc->RingIndex+1)&SystemCacheCountMask;
  
  REGIME_Resume();
}

static void disc_SystemWriteSector_Body(u32 sector, const void* buffer)
{
  if(active_interface==NULL) StopFatalError(0,"Not startupped.");
  
  TSystemCache *psc=&SystemCache;
  
  for(u32 idx=0;idx<SystemCacheCount;idx++){
    if(sector==psc->SectorIndex[idx]){
      u32 *pbuf1=(u32*)buffer;
      u32 *pbuf2=(u32*)GetSectorDataPointer(idx);
      active_interface->fn_WriteSectors(sector,1,pbuf1);
      active_interface->fn_ReadSectors(sector,1,pbuf2);
      for(u32 idx=0;idx<512/4;idx++){
        if(pbuf1[idx]!=pbuf2[idx]) StopFatalError(11802,"System disk write verify error on cache. 0x%08x, 0x%08x, %d\n",pbuf1,pbuf2,idx);
      }
      return;
    }
  }
  
  psc->SectorIndex[psc->RingIndex]=sector;
  
  {
    u32 *pbuf1=(u32*)buffer;
    u32 *pbuf2=(u32*)GetSectorDataPointer(psc->RingIndex);
    active_interface->fn_WriteSectors(sector,1,pbuf1);
    active_interface->fn_ReadSectors(sector,1,pbuf2);
    for(u32 idx=0;idx<512/4;idx++){
      if(pbuf1[idx]!=pbuf2[idx]) StopFatalError(11803,"System disk write verify error on direct.\n");
    }
  }
  
  psc->RingIndex=(psc->RingIndex+1)&SystemCacheCountMask;
}

void disc_SystemWriteSector_SetWriteCache(bool Enabled)
{
  REGIME_Pause();
  
  SystemWriteCache_Enabled=Enabled;
  
  if(SystemWriteCache_Enabled==true){
    _consolePrint("Start system write-back cache.\n");
    }else{
    _consolePrint("End system write-back cache.\n");
  }
  
  if(SystemWriteCache_Enabled==false){
    if(SystemWriteCache_TargetSectorIndex!=(u32)-1){
//      _consolePrintf("[FlashWriteCache:%d]",SystemWriteCache_TargetSectorIndex);
      disc_SystemWriteSector_Body(SystemWriteCache_TargetSectorIndex,SystemWriteCache_Data);
      SystemWriteCache_TargetSectorIndex=(u32)-1;
    }
  }
  
  REGIME_Resume();
}

void disc_SystemWriteSector(u32 sector, const void* buffer)
{
//  _consolePrintf("SW:%d ",sector);
  REGIME_Pause();
  
  if(SystemWriteCache_Enabled==false){
    disc_SystemWriteSector_Body(sector,buffer);
    REGIME_Resume();
    return;
  }
  
  if(SystemWriteCache_TargetSectorIndex!=(u32)-1){
    if(SystemWriteCache_TargetSectorIndex!=sector){
      disc_SystemWriteSector_Body(SystemWriteCache_TargetSectorIndex,SystemWriteCache_Data);
      SystemWriteCache_TargetSectorIndex=(u32)-1;
    }
  }
  
  const u32 *psrc=(u32*)buffer;
  
  for(u32 idx=0;idx<512/4;idx++){
    SystemWriteCache_Data[idx]=psrc[idx];
  }
  
  SystemWriteCache_TargetSectorIndex=sector;
  
  REGIME_Resume();
}

bool disc_ClearStatus(void) 
{
  if(active_interface==NULL) return(false);
  return(active_interface->fn_ClearStatus());
} 

bool disc_Shutdown(void) 
{
  if(active_interface==NULL) return(true);
  active_interface->fn_Shutdown();
  active_interface=NULL;
  return(true);
} 

u32	disc_HostType (void)
{
  if(active_interface==NULL) return(0);
  return(active_interface->ul_ioType);
}

