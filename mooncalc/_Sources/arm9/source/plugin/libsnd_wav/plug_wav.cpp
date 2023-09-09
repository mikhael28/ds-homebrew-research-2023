
#include <NDS.h>

#include <stdio.h>

#include "_console.h"
#include "fat2.h"
#include "memtool.h"
#include "arm9tcm.h"

#include "internaldrivers.h"

#ifdef ExceptWAVE

// -----------------

typedef struct {
  u16 wFormatTag;
  u16 nChannels;
  u32 nSamplesPerSec;
  u32 nAvgBytesPerSec;
  u16 nBlockAlign;
  u16 wBitsPerSample;
  u16 cbSize;
} WAVEFORMATEX;

DATA_IN_DTCM_WAVE static FAT_FILE *FileHandle;
DATA_IN_DTCM_WAVE static WAVEFORMATEX wfex;
DATA_IN_DTCM_WAVE static int DataTopOffset;
DATA_IN_DTCM_WAVE static int SampleCount;
DATA_IN_DTCM_WAVE static int SampleOffset;
DATA_IN_DTCM_WAVE static int BytePerSample;

DATA_IN_DTCM_WAVE static const int SamplePerFrame=512;
DATA_IN_DTCM_WAVE static u32 ReadBuffer[SamplePerFrame];

// --------------------------------------------------------------------

static bool WaveFile_ReadWaveChunk(void)
{
  FAT2_fseek(FileHandle,0x14,SEEK_SET);
  FAT2_fread(&wfex,sizeof(wfex),1,FileHandle);
  
  if(wfex.wFormatTag!=0x0001){
    _consolePrintf("Illigal CompressFormat Error. wFormatTag=0x%x\n",wfex.wFormatTag);
    return(false);
  }
  
  if((wfex.nChannels!=1)&&(wfex.nChannels!=2)){
    _consolePrintf("Channels Error. nChannels=%d\n",wfex.nChannels);
    return(false);
  }
  
  if((wfex.wBitsPerSample!=8)&&(wfex.wBitsPerSample!=16)){
    _consolePrintf("Bits/Sample Error. wBitsPerSample=%d\n",wfex.wBitsPerSample);
    return(false);
  }
  
  _consolePrintf("fmt:0x%x chns:%d\n",wfex.wFormatTag,wfex.nChannels);
  _consolePrintf("Smpls/Sec:%d AvgBPS:%d\n",wfex.nSamplesPerSec,wfex.nAvgBytesPerSec);
  _consolePrintf("BlockAlign:%d Bits/Smpl:%d\n",wfex.nBlockAlign,wfex.wBitsPerSample);
  
  return(true);
}

static bool WaveFile_SeekDataChunk(void)
{
  FAT2_fseek(FileHandle,0,SEEK_SET);
  
  // find "data"
  {
    char *readbuf=(char*)safemalloc(&MM_DLLSound,256);
    int size=0;
    int ofs=0;
    
    size=FAT2_fread(readbuf,1,256,FileHandle);
    if(size<4){
      safefree(&MM_DLLSound,readbuf);
      _consolePrintf("can not find data chunk.\n");
      return(false);
    }
    
    while(true){
      if(readbuf[ofs]=='d'){
        if((readbuf[ofs+1]=='a')&&(readbuf[ofs+2]=='t')&&(readbuf[ofs+3]=='a')){
          safefree(&MM_DLLSound,readbuf);
          FAT2_fseek(FileHandle,ofs+4,SEEK_SET);
          break;
        }
      }
      ofs++;
      if(ofs==(size-4)){
        safefree(&MM_DLLSound,readbuf);
        _consolePrintf("can not find data chunk.\n");
        return(false);
      }
    }
  }
  
  u32 DataSize;
  FAT2_fread(&DataSize,4,1,FileHandle);
  
  if(DataSize==0){
    _consolePrintf("DataSize is NULL\n");
    return(false);
  }
  
  BytePerSample=0;
  
  if((wfex.nChannels==1)&&(wfex.wBitsPerSample==8)) BytePerSample=1;
  if((wfex.nChannels==2)&&(wfex.wBitsPerSample==8)) BytePerSample=2;
  if((wfex.nChannels==1)&&(wfex.wBitsPerSample==16)) BytePerSample=2;
  if((wfex.nChannels==2)&&(wfex.wBitsPerSample==16)) BytePerSample=4;
  
  if(BytePerSample==0){
    _consolePrintf("Illigal Channels or Bits/Sample or no data\n");
    return(false);
  }
  
  SampleCount=DataSize/BytePerSample;
  
  DataTopOffset=FAT2_ftell(FileHandle);
  
  _consolePrintf("DataTop:%d DataSize:%dbyte\n",DataTopOffset,DataSize);
  
  return(true);
}

// ------------------------------------------------------------------------------------

static bool PlugStart(FAT_FILE *_FileHandle)
{
  FileHandle=_FileHandle;
  
  FAT2_fseek(FileHandle,0,SEEK_SET);
  
  u32 RIFFID;
  FAT2_fread(&RIFFID,4,1,FileHandle);
  
  if(RIFFID!=0x46464952){ // check "RIFF"
    _consolePrintf("no RIFFWAVEFILE error.");
    _consolePrintf("topdata:0x%04x\n",RIFFID);
    return(false);
  }
  
  if(WaveFile_ReadWaveChunk()==false) return(false);
  if(WaveFile_SeekDataChunk()==false) return(false);
  
  SampleOffset=0;
  
  return(true);
}

static void PlugFree(void)
{
}

static bool PlugisComplexDecoder(void)
{
  return(false);
}

static u32 PlugUpdate(u32 *plrbuf)
{
  int SampleCount=FAT2_fread(ReadBuffer,1,SamplePerFrame*BytePerSample,FileHandle)/BytePerSample;
  
  if(wfex.wBitsPerSample==8){
    s8 *readbuf=(s8*)ReadBuffer;
    if(wfex.nChannels==1){ // 8bit mono
      for(u32 idx=0;idx<SampleCount;idx++){
        s16 s=(((s16)(*readbuf++))-128)<<8;
        *plrbuf++=(s&0xffff)|(s<<16);
      }
      }else{ // 8bit stereo
      for(u32 idx=0;idx<SampleCount;idx++){
        s16 l=(((s16)(*readbuf++))-128)<<8;
        s16 r=(((s16)(*readbuf++))-128)<<8;
        *plrbuf++=(l&0xffff)|(r<<16);
      }
    }
    }else{
    if(wfex.nChannels==1){ // 16bit mono
      u16 *readbuf=(u16*)ReadBuffer;
      for(u32 idx=0;idx<SampleCount;idx++){
        u16 s=*readbuf++;
        *plrbuf++=s|(s<<16);
      }
      }else{ // 16bit stereo
      u32 *readbuf=(u32*)ReadBuffer;
      for(u32 idx=0;idx<SampleCount;idx++){
        *plrbuf++=*readbuf++;
      }
    }
  }
  
  SampleOffset+=SampleCount;
  
  return(SampleCount);
}

static s32 PlugGetPosMax(void)
{
  return(SampleCount);
}

static s32 PlugGetPosOffset(void)
{
  return(SampleOffset);
}

static void PlugSetPosOffset(s32 ofs)
{
  if(ofs<0) ofs=0;
  ofs&=~3;
  
  SampleOffset=ofs;
  
  FAT2_fseek(FileHandle,DataTopOffset+(SampleOffset*BytePerSample),SEEK_SET);
}

static u32 PlugGetChannelCount(void)
{
  return(wfex.nChannels);
}

static u32 PlugGetSampleRate(void)
{
  return(wfex.nSamplesPerSec);
}

static u32 PlugGetSamplePerFrame(void)
{
  return(SamplePerFrame);
}

static u32 PlugGetPlayTimeSec(void)
{
  u32 datasize=FAT2_GetFileSize(FileHandle)-DataTopOffset;
  u32 secs=datasize/wfex.nBlockAlign/wfex.nSamplesPerSec;
  return(secs);
}

// -----------------------------------------------------------

void PlugWAVE_GetInternal(Tlibsnd_Internal *plibsnd_Internal)
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
