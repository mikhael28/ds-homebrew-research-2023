
#include <stdio.h>

#include <NDS.h>

#include "_const.h"
#include "_console.h"
#include "_consoleWriteLog.h"
#include "memtool.h"

#include "fat2.h"

// -----------------

#define BI_RGB (0)
#define BI_RLE8 (1)
#define BI_RLE4 (2)
#define BI_Bitfields (3)

typedef struct {
  u8 bfType[2];
  u32 bfSize;
  u16 bfReserved1;
  u16 bfReserved2;
  u32 bfOffset;
  u32 biSize;
  u32 biWidth;
  u32 biHeight;
  u16 biPlanes;
  u16 biBitCount;
  u32 biCopmression;
  u32 biSizeImage;
  u32 biXPixPerMeter;
  u32 biYPixPerMeter;
  u32 biClrUsed;
  u32 biCirImportant;
  
  u32 Palette32[256];
  u16 Palette16[256];
  
  u32 DataWidth;
} TBMPHeader;

static TBMPHeader *pBMPHeader=NULL;

static FAT_FILE *FileHandle;
static u8 *pReadBuf=NULL;

// ------------------------------------------------------------------------------------

static u8 Get8bit(void)
{
  u8 res;
  FAT2_fread(&res,1,1,FileHandle);
  return(res);
}

static u16 Get16bit(void)
{
  u16 res;
  FAT2_fread(&res,2,1,FileHandle);
  return(res);
}

static u32 Get32bit(void)
{
  u32 res;
  FAT2_fread(&res,4,1,FileHandle);
  return(res);
}

static bool GetBMPHeader(TBMPHeader *pBMPHeader)
{
  FAT2_fseek(FileHandle,0,SEEK_SET);
  
  pBMPHeader->bfType[0]=Get8bit();
  pBMPHeader->bfType[1]=Get8bit();
  pBMPHeader->bfSize=Get32bit();
  pBMPHeader->bfReserved1=Get16bit();
  pBMPHeader->bfReserved2=Get16bit();
  pBMPHeader->bfOffset=Get32bit();
  pBMPHeader->biSize=Get32bit();
  pBMPHeader->biWidth=Get32bit();
  pBMPHeader->biHeight=Get32bit();
  pBMPHeader->biPlanes=Get16bit();
  pBMPHeader->biBitCount=Get16bit();
  pBMPHeader->biCopmression=Get32bit();
  pBMPHeader->biSizeImage=Get32bit();
  pBMPHeader->biXPixPerMeter=Get32bit();
  pBMPHeader->biYPixPerMeter=Get32bit();
  pBMPHeader->biClrUsed=Get32bit();
  pBMPHeader->biCirImportant=Get32bit();
  
  if((pBMPHeader->biBitCount==4)||(pBMPHeader->biBitCount==8)){
    for(int idx=0;idx<256;idx++){
      u32 pal32=Get32bit();
      pBMPHeader->Palette32[idx]=pal32;
      u32 r=(pal32>>16)&0xff;
      u32 g=(pal32>>8)&0xff;
      u32 b=(pal32>>0)&0xff;
      pBMPHeader->Palette16[idx]=RGB15(r>>3,g>>3,b>>3)|BIT15;
    }
    }else{
    for(int idx=0;idx<256;idx++){
      pBMPHeader->Palette32[idx]=0;
      pBMPHeader->Palette16[idx]=0;
    }
  }
  
  if((pBMPHeader->bfType[0]!='B')||(pBMPHeader->bfType[1]!='M')){
    _consolePrintf("Error MagicID!=BM");
    return(false);
  }
  
  if(pBMPHeader->biCopmression!=BI_RGB){
    _consolePrintf("Error notsupport Compression");
    return(false);
  }
  
  if(pBMPHeader->biHeight>=0x80000000){
    _consolePrintf("Error notsupport OS/2 format");
    return(false);
  }
  
  if(pBMPHeader->biPlanes!=1){
    _consolePrintf("Error notsupport Planes!=1");
    return(false);
  }
  
  pBMPHeader->DataWidth=0;
  
  switch(pBMPHeader->biBitCount){
    case 1:
      _consolePrintf("Error notsupport 1bitcolor.");
      return(false);
      break;
    case 4:
      _consolePrintf("Error notsupport 4bitcolor.");
      return(false);
      break;
    case 8:
      pBMPHeader->DataWidth=pBMPHeader->biWidth*1;
      break;
    case 16:
      _consolePrintf("Error notsupport 16bitcolor.");
      return(false);
    case 24:
      pBMPHeader->DataWidth=pBMPHeader->biWidth*3;
      break;
    case 32:
      _consolePrintf("Error notsupport 32bitcolor.");
      return(false);
      break;
    default:
      _consolePrintf("Error Unknown %dbitcolor.",pBMPHeader->biBitCount);
      return(false);
      break;
  }
  
  pBMPHeader->DataWidth=(pBMPHeader->DataWidth+3)&~3;
  
  return(true);
}

static void BMP_ShowFrameInfo(TBMPHeader *pBMPHeader)
{
  _consolePrintf("FileSize=%d\n",pBMPHeader->bfSize);
  _consolePrintf("Size=(%d,%d) %dpixel\n",pBMPHeader->biWidth,pBMPHeader->biHeight,pBMPHeader->biWidth*pBMPHeader->biHeight);
  _consolePrintf("Planes=%d\n",pBMPHeader->biPlanes);
  _consolePrintf("BitCount=%d\n",pBMPHeader->biBitCount);
  _consolePrintf("Compression=%d\n",pBMPHeader->biCopmression);
  _consolePrintf("BitmapOffset=0x%x\n",pBMPHeader->bfOffset);
  _consolePrintf("DataWidth=%d\n",pBMPHeader->DataWidth);
}

// ------------------------------------------------------------------------------------

bool BMPReader_Start(FAT_FILE *_FileHandle,bool ReadHeaderOnly)
{
  if(_FileHandle==NULL){
    _consolePrintf("FileHandle is NULL\n");
    return(false);
  }
  
  FileHandle=_FileHandle;
  
  pBMPHeader=(TBMPHeader*)safemalloc_chkmem(&MM_Temp,sizeof(TBMPHeader));
  MemSet8CPU(0,pBMPHeader,sizeof(TBMPHeader));
  
  if(GetBMPHeader(pBMPHeader)==false){
    _consolePrintf("BMP LoadError.\n");
    return(false);
  }
  
//  BMP_ShowFrameInfo(pBMPHeader);
  
  if(ReadHeaderOnly==true){
    pReadBuf=NULL;
    }else{
    pReadBuf=(u8*)safemalloc_chkmem(&MM_Temp,pBMPHeader->DataWidth*pBMPHeader->biHeight);
    u32 ofs=pBMPHeader->bfOffset;
    if(FAT2_ftell(FileHandle)!=ofs) FAT2_fseek(FileHandle,ofs,SEEK_SET);
    FAT2_fread(pReadBuf,1,pBMPHeader->DataWidth*pBMPHeader->biHeight,FileHandle);
  }
  
  return(true);
}

void BMPReader_Free(void)
{
  if(pReadBuf!=NULL){
    safefree(&MM_Temp,pReadBuf); pReadBuf=NULL;
  }
  if(pBMPHeader!=NULL){
    safefree(&MM_Temp,pBMPHeader); pBMPHeader=NULL;
  }
  
  FileHandle=0;
}

void BMPReader_GetBitmap32(u32 LineY,u32 *pBM)
{
  if(pBMPHeader->biHeight<=LineY) return;
  
  u32 ofs=(pBMPHeader->biHeight-LineY-1)*pBMPHeader->DataWidth;
  u8 *pbuf=&pReadBuf[ofs];
  
  u32 Width=pBMPHeader->biWidth;
  
  switch(pBMPHeader->biBitCount){
    case 1: {
    } break;
    case 4: {
    } break;
    case 8: {
      for(u32 x=0;x<Width;x++){
        u32 col32=pBMPHeader->Palette32[pbuf[x]];
        u32 r=(col32>>16)&0xff;
        u32 g=(col32>>8)&0xff;
        u32 b=(col32>>0)&0xff;
        pBM[x]=b|(g<<8)|(r<<16);
      }
    } break;
    case 24: {
      for(u32 x=0;x<Width;x++){
        u32 r=pbuf[x*3+2];
        u32 g=pbuf[x*3+1];
        u32 b=pbuf[x*3+0];
        pBM[x]=b|(g<<8)|(r<<16);
      }
    } break;
  }
}

void BMPReader_GetBitmap15(u32 LineY,u16 *pBM)
{
  if(pBMPHeader->biHeight<=LineY) return;
  
  u32 ofs=(pBMPHeader->biHeight-LineY-1)*pBMPHeader->DataWidth;
  u8 *pbuf=&pReadBuf[ofs];
  
  u32 Width=pBMPHeader->biWidth;
  
  switch(pBMPHeader->biBitCount){
    case 1: {
    } break;
    case 4: {
    } break;
    case 8: {
      for(u32 x=0;x<Width;x++){
        u32 col15=pBMPHeader->Palette16[pbuf[x]];
        pBM[x]=col15;
      }
    } break;
    case 24: {
      for(u32 x=0;x<Width;x++){
        u32 r=pbuf[x*3+2];
        u32 g=pbuf[x*3+1];
        u32 b=pbuf[x*3+0];
        pBM[x]=RGB15(r>>3,g>>3,b>>3)|BIT15;
      }
    } break;
  }
}

s32 BMPReader_GetWidth(void)
{
  return(pBMPHeader->biWidth);
}

s32 BMPReader_GetHeight(void)
{
  return(pBMPHeader->biHeight);
}

