
#include <NDS.h>

#include <stdio.h>

#include "_console.h"
#include "_consoleWriteLog.h"
#include "maindef.h"
#include "fat2.h"
#include "memtool.h"
#include "arm9tcm.h"
#include "inifile.h"

#include "internaldrivers.h"

#ifdef ExceptG721

#include "g721dec.h"

// ------------------------------------------------------------------------------------

#define SamplesPerFrame (256)
#define BlockDivCount (64)

DATA_IN_DTCM_G721 static FAT_FILE *FileHandle;

DATA_IN_DTCM_G721 static u8 *pCodeBuf;
DATA_IN_DTCM_G721 static u32 CodeSize,CodeReadPos;

DATA_IN_DTCM_G721 TG721State State;

DATA_IN_DTCM_G721 static u32 HeaderSize;
DATA_IN_DTCM_G721 static u32 Freq,Chs;

DATA_IN_DTCM_G721 static u32 BlockDataSize;

DATA_IN_DTCM_G721 static u32 BlocksCount;
DATA_IN_DTCM_G721 static u32 BlocksPos;

// ------------------------------------------------------------------------------------

typedef struct {
  u32 IDTag_VREC;
  u32 IDTag_G721;
  u16 Freq;
  u16 Channels;
} TG721Header;

static bool PlugStart(FAT_FILE *_FileHandle)
{
  FileHandle=_FileHandle;
  
  pCodeBuf=NULL;
  CodeSize=0;
  CodeReadPos=0;
  
  {
    TG721Header wh;
    FAT2_fread(&wh,1,sizeof(TG721Header),FileHandle);
    
#define IDTag(x) ( ((u8)(x[0])<<0) | ((u8)(x[1])<<8) | ((u8)(x[2])<<16) | ((u8)(x[3])<<24) )
    bool f=true;
    if(wh.IDTag_VREC!=IDTag("VREC")) f=false;
    if(wh.IDTag_G721!=IDTag("G721")) f=false;
    Freq=wh.Freq;
    Chs=wh.Channels;
    if(Chs!=1) f=false;
    if(f==false){
      _consolePrintf("Invalid file format.\n");
      return(false);
    }
#undef IDTag
  }
  
  HeaderSize=FAT2_ftell(FileHandle);
  
  BlockDataSize=4+sizeof(TG721State)+(Freq/2);
  
  BlocksCount=((FAT2_GetFileSize(FileHandle)-HeaderSize)+(BlockDataSize-1))/BlockDataSize;
  BlocksPos=0;
  
  pCodeBuf=(u8*)safemalloc_chkmem(&MM_DLLSound,Freq/2);
  
  _consolePrintf("%dsecs. %dHz. %dchs.\n",BlocksCount,Freq,Chs);
  
  return(true);
}

static void PlugFree(void)
{
  if(pCodeBuf!=NULL){
    safefree(&MM_DLLSound,pCodeBuf); pCodeBuf=NULL;
  }
  
  FileHandle=NULL;
}

static bool PlugisComplexDecoder(void)
{
  return(false);
}

static bool UpdateCodeBuf(void)
{
  if(CodeReadPos<CodeSize) return(true);
  
  CodeReadPos=0;
  
  if(BlocksPos==BlocksCount) return(false);
  BlocksPos++;
  
  {
#define IDTag(x) ( ((u8)(x[0])<<0) | ((u8)(x[1])<<8) | ((u8)(x[2])<<16) | ((u8)(x[3])<<24) )
    u32 ID;
    FAT2_fread(&ID,4,1,FileHandle);
    if(ID!=IDTag("G721")) StopFatalError(0,"Invalid block ID.");
  }
  
  {
    u32 hsize=sizeof(TG721State);
    if(hsize!=52) StopFatalError(0,"Internal sizeof error. %d!=%d",hsize,52);
    FAT2_fread(&State,1,hsize,FileHandle);
  }
  
  CodeSize=FAT2_fread(pCodeBuf,1,Freq/2,FileHandle);
  
  return(true);
}

static u32 PlugUpdate(u32 *plrbuf)
{
  if(UpdateCodeBuf()==false) return(0);
  
  u32 DecCount=SamplesPerFrame;
  if(((CodeSize-CodeReadPos)*2)<DecCount) DecCount=(CodeSize-CodeReadPos)*2;
  
  s16 *pbuf=(s16*)safemalloc_chkmem(&MM_Temp,DecCount*2);
  g721_blkdecoder(&pCodeBuf[CodeReadPos],pbuf,DecCount, &State);
  CodeReadPos+=DecCount/2;
  
  for(u32 idx=0;idx<DecCount;idx++){
    s32 s=pbuf[idx];
    s<<=2;
    if(s<-0x8000) s=-0x8000;
    if(0x7fff<s) s=0x7fff;
    s&=0xffff;
    *plrbuf++=s|(s<<16);
  }
  
  if(pbuf!=NULL){
    safefree(&MM_Temp,pbuf); pbuf=NULL;
  }
  
  return(DecCount);
}

static s32 PlugGetPosMax(void)
{
  return(BlocksCount*BlockDivCount);
}

static s32 PlugGetPosOffset(void)
{
  float frac=0;
  if(CodeSize!=0) frac=(CodeReadPos*1.0)/CodeSize;
  u32 basepos=0;
  if(BlocksPos!=0) basepos=(BlocksPos-1)*BlockDivCount;
  return(basepos+(frac*BlockDivCount));
}

static void PlugSetPosOffset(s32 ofs)
{
  u32 seekcnt=ofs&(BlockDivCount-1);
  ofs/=BlockDivCount;
  
  CodeSize=0;
  CodeReadPos=0;
  
  _consolePrintf("Seek to %d from %d.\n",ofs,BlocksPos);
  
  BlocksPos=ofs;
  if(BlocksPos==BlocksCount) BlocksPos--;
  
  u32 fofs=HeaderSize+(BlocksPos*BlockDataSize);
  FAT2_fseek(FileHandle,fofs,SEEK_SET);
  
  CodeReadPos=0;
  CodeSize=0;
  
  if(UpdateCodeBuf()==false) return;
  
  for(u32 idx=0;idx<seekcnt;idx++){
    u32 DecCount=SamplesPerFrame;
    if(((CodeSize-CodeReadPos)*2)<DecCount) break;
    
    s16 *pbuf=(s16*)safemalloc_chkmem(&MM_Temp,DecCount*2);
    g721_blkdecoder(&pCodeBuf[CodeReadPos],pbuf,DecCount, &State);
    CodeReadPos+=DecCount/2;
    
    if(pbuf!=NULL){
      safefree(&MM_Temp,pbuf); pbuf=NULL;
    }
  }
}

static u32 PlugGetChannelCount(void)
{
  return(Chs);
}

static u32 PlugGetSampleRate(void)
{
  return(Freq);
}

static u32 PlugGetSamplePerFrame(void)
{
  return(SamplesPerFrame);
}

static u32 PlugGetPlayTimeSec(void)
{
  return(BlocksCount);
}

// -----------------------------------------------------------

void PlugG721_GetInternal(Tlibsnd_Internal *plibsnd_Internal)
{
  Tlibsnd_Internal *pi=plibsnd_Internal;
  
  pi->Start=PlugStart;
  pi->Free=PlugFree;
  pi->isComplexDecoder=PlugisComplexDecoder;
  
  pi->Update=PlugUpdate;
  
  pi->GetPosMax=PlugGetPosMax;
  pi->GetPosOffset=PlugGetPosOffset;
  pi->SetPosOffset=PlugSetPosOffset;
  pi->GetChannelCount=PlugGetChannelCount;
  pi->GetSampleRate=PlugGetSampleRate;
  pi->GetSamplePerFrame=PlugGetSamplePerFrame;
  pi->GetPlayTimeSec=PlugGetPlayTimeSec;
}

#endif
