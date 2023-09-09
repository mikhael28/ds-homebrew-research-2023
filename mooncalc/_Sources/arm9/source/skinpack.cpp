
#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "inifile.h"

#include "glib.h"
#include "strtool.h"

#include "fat2.h"
#include "shell.h"
#include "procstate.h"

#include "skinpack.h"

typedef struct {
  u32 Offset;
  u32 Size;
} TFileItem;

static TFileItem Files[ESF_Count];

static FAT_FILE *pf;

void SkinPack_Init(void)
{
  pf=Shell_FAT_fopen_Internal(SkinPackFilename);
  
  FAT2_fread_fast(Files,sizeof(TFileItem),ESF_Count,pf);
}

void SkinPack_Free(void)
{
  if(pf!=NULL){
    FAT2_fclose(pf); pf=NULL;
  }
}

CglB15* SkinPack_LoadB15(ESkinFiles ESF)
{
  TFileItem *pfi=&Files[(u32)ESF];
  
  const u32 offset=pfi->Offset;
  const u32 bufsize=pfi->Size;
//  _consolePrintf("SkinPack_LoadB15(%d): %x,%d\n",(u32)ESF,offset,bufsize);
  
  const u32 memsize=(bufsize+3)&~3;
  u8 *pbuf=(u8*)safemalloc(&MM_Temp,memsize);
  
  FAT2_fseek(pf,offset,SEEK_SET);
  FAT2_fread_fast(pbuf,1,memsize,pf);
  
  CglB15 *pB15=new CglB15(&MM_Skin,pbuf,bufsize);
  
  if(pbuf!=NULL){
    safefree(&MM_Temp,pbuf); pbuf=NULL;
  }
  
  return(pB15);
}

CglTGF* SkinPack_LoadTGF(ESkinFiles ESF)
{
  TFileItem *pfi=&Files[(u32)ESF];
  
  const u32 offset=pfi->Offset;
  const u32 bufsize=pfi->Size;
//  _consolePrintf("SkinPack_LoadTGF(%d): %x,%d\n",(u32)ESF,offset,bufsize);
  
  const u32 memsize=(bufsize+3)&~3;
  u8 *pbuf=(u8*)safemalloc(&MM_Temp,memsize);
  
  FAT2_fseek(pf,offset,SEEK_SET);
  FAT2_fread_fast(pbuf,1,memsize,pf);
  
  CglTGF *pTGF=new CglTGF(&MM_Skin,pbuf,bufsize);
  
  if(pbuf!=NULL){
    safefree(&MM_Temp,pbuf); pbuf=NULL;
  }
  
  return(pTGF);
}
