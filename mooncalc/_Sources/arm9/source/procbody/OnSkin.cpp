
#include <nds.h>

#include "_const.h"
#include "maindef.h"
#include "_console.h"
#include "_consoleWriteLog.h"

#include "memtool.h"
#undef safemalloc
#define safemalloc safemalloc_chkmem
#include "strtool.h"
#include "unicode.h"
#include "fat2.h"
#include "datetime.h"
#include "shell.h"
#include "glib.h"

#include "BMPReader.h"

#include "onskin.h"

CglCanvas *pSkinBG;

void OnSkin_Exclusive_LoadBMP(CglCanvas *pcan,const char *pfn)
{
  _consolePrintf("LoadBMP: %s.\n",pfn);
  
  char afn[16];
  
  u32 idx=0;
  while(1){
    char c=pfn[idx];
    if(c==0) break;
    if(('a'<=c)&&(c<='z')) c='A'+(c-'a');
    afn[idx]=c;
    idx++;
  }
  afn[idx]=0;
  
  FAT_FILE *pf=Shell_FAT_fopen_Skin(afn);
  if(pf==NULL) StopFatalError(0,"Not found skin file. %s, %s",pfn,afn);
  
  if(BMPReader_Start(pf,false)==false) StopFatalError(0,"Can not open BMP file. %s, %s",pfn,afn);
  
  const u32 Width=BMPReader_GetWidth();
  const u32 Height=BMPReader_GetHeight();
  
  pcan->SetVRAMBuf(NULL,Width,Height,pf15bit);
  
  for(u32 y=0;y<Height;y++){
    u16 *pbuf=pcan->GetScanLine(y);
    BMPReader_GetBitmap15(y,pbuf);
  }
  
  BMPReader_Free();
  
  FAT2_fclose(pf); pf=NULL;
}

static void LoadTGF(CglTGF **pptgf,const char *pfn)
{
  _consolePrintf("LoadTGF: %s.\n",pfn);
  
  char afn[16];
  
  u32 idx=0;
  while(1){
    char c=pfn[idx];
    if(c==0) break;
    if(('a'<=c)&&(c<='z')) c='A'+(c-'a');
    afn[idx]=c;
    idx++;
  }
  afn[idx]=0;
  
  FAT_FILE *pf=Shell_FAT_fopen_Skin(afn);
  if(pf==NULL) StopFatalError(0,"Not found skin file. %s, %s",pfn,afn);
  
  u32 bufsize=FAT2_GetFileSize(pf);
  u8 *pbuf=(u8*)safemalloc(&MM_Temp,bufsize);
  FAT2_fread(pbuf,1,bufsize,pf);
  
  FAT2_fclose(pf); pf=NULL;
  
  *pptgf=new CglTGF(&MM_SystemAfter,pbuf,bufsize);
  
  if(pbuf!=NULL){
    safefree(&MM_Temp,pbuf); pbuf=NULL;
  }
}

void OnSkin_Init(const char *pbgfn)
{
  pSkinBG=new CglCanvas(&MM_SystemAfter,NULL,1,1,pf15bit);
  OnSkin_Exclusive_LoadBMP(pSkinBG,pbgfn);
}

void OnSkin_Free(void)
{
  if(pSkinBG!=NULL){
    delete pSkinBG; pSkinBG=NULL;
  }
}

void OnSkin_FillBox25per(CglCanvas *pcan,u32 x,u32 y,u32 w,u32 h,u16 Color)
{
  if((w==0)||(h==0)) return;
  
  u32 sr=(Color>>0)&0x1f;
  u32 sg=(Color>>5)&0x1f;
  u32 sb=(Color>>10)&0x1f;
  
  for(u32 py=y;py<y+h;py++){
    u16 *pbuf=pcan->GetScanLine(py);
    u32 tmpw=w;
    pbuf+=x;
    if((x&1)!=0){
      u32 col=*pbuf;
      u32 r=(col>>0)&0x1f;
      u32 g=(col>>5)&0x1f;
      u32 b=(col>>10)&0x1f;
      r=((r*3)+sr)>>2;
      g=((g*3)+sg)>>2;
      b=((b*3)+sb)>>2;
      *pbuf++=RGB15(r,g,b)|BIT15;
      tmpw--;
    }
    while(2<=tmpw){
      u32 *pbuf32=(u32*)pbuf;
      u32 col=*pbuf32;
      u32 r0=(col>>0)&0x1f;
      u32 g0=(col>>5)&0x1f;
      u32 b0=(col>>10)&0x1f;
      u32 r1=(col>>(16+0))&0x1f;
      u32 g1=(col>>(16+5))&0x1f;
      u32 b1=(col>>(16+10))&0x1f;
      r0=((r0*3)+sr)>>2;
      g0=((g0*3)+sg)>>2;
      b0=((b0*3)+sb)>>2;
      r1=((r1*3)+sr)>>2;
      g1=((g1*3)+sg)>>2;
      b1=((b1*3)+sb)>>2;
      col=(RGB15(r0,g0,b0)|BIT15)|((RGB15(r1,g1,b1)|BIT15)<<16);
      *pbuf32++=col;
      pbuf+=2;
      tmpw-=2;
    }
    if(tmpw!=0){
      u32 col=*pbuf;
      u32 r=(col>>0)&0x1f;
      u32 g=(col>>5)&0x1f;
      u32 b=(col>>10)&0x1f;
      r=((r*3)+sr)>>2;
      g=((g*3)+sg)>>2;
      b=((b*3)+sb)>>2;
      *pbuf++=RGB15(r,g,b)|BIT15;
    }
  }
}

void OnSkin_FillBox50per(CglCanvas *pcan,u32 x,u32 y,u32 w,u32 h,u16 Color)
{
  if((w==0)||(h==0)) return;
  
  u32 sr=(Color>>0)&0x1f;
  u32 sg=(Color>>5)&0x1f;
  u32 sb=(Color>>10)&0x1f;
  
  for(u32 py=y;py<y+h;py++){
    u16 *pbuf=pcan->GetScanLine(py);
    u32 tmpw=w;
    pbuf+=x;
    if((x&1)!=0){
      u32 col=*pbuf;
      u32 r=(col>>0)&0x1f;
      u32 g=(col>>5)&0x1f;
      u32 b=(col>>10)&0x1f;
      r=(r+sr)>>1;
      g=(g+sg)>>1;
      b=(b+sb)>>1;
      *pbuf++=RGB15(r,g,b)|BIT15;
      tmpw--;
    }
    while(2<=tmpw){
      u32 *pbuf32=(u32*)pbuf;
      u32 col=*pbuf32;
      u32 r0=(col>>0)&0x1f;
      u32 g0=(col>>5)&0x1f;
      u32 b0=(col>>10)&0x1f;
      u32 r1=(col>>(16+0))&0x1f;
      u32 g1=(col>>(16+5))&0x1f;
      u32 b1=(col>>(16+10))&0x1f;
      r0=(r0+sr)>>1;
      g0=(g0+sg)>>1;
      b0=(b0+sb)>>1;
      r1=(r1+sr)>>1;
      g1=(g1+sg)>>1;
      b1=(b1+sb)>>1;
      col=(RGB15(r0,g0,b0)|BIT15)|((RGB15(r1,g1,b1)|BIT15)<<16);
      *pbuf32++=col;
      pbuf+=2;
      tmpw-=2;
    }
    if(tmpw!=0){
      u32 col=*pbuf;
      u32 r=(col>>0)&0x1f;
      u32 g=(col>>5)&0x1f;
      u32 b=(col>>10)&0x1f;
      r=(r+sr)>>1;
      g=(g+sg)>>1;
      b=(b+sb)>>1;
      *pbuf++=RGB15(r,g,b)|BIT15;
    }
  }
}

void OnSkin_FillBox75per(CglCanvas *pcan,u32 x,u32 y,u32 w,u32 h,u16 Color)
{
  if((w==0)||(h==0)) return;
  
  u32 sr=(Color>>0)&0x1f;
  u32 sg=(Color>>5)&0x1f;
  u32 sb=(Color>>10)&0x1f;
  sr*=3;
  sg*=3;
  sb*=3;
  
  for(u32 py=y;py<y+h;py++){
    u16 *pbuf=pcan->GetScanLine(py);
    u32 tmpw=w;
    pbuf+=x;
    if((x&1)!=0){
      u32 col=*pbuf;
      u32 r=(col>>0)&0x1f;
      u32 g=(col>>5)&0x1f;
      u32 b=(col>>10)&0x1f;
      r=(r+sr)>>2;
      g=(g+sg)>>2;
      b=(b+sb)>>2;
      *pbuf++=RGB15(r,g,b)|BIT15;
      tmpw--;
    }
    while(2<=tmpw){
      u32 *pbuf32=(u32*)pbuf;
      u32 col=*pbuf32;
      u32 r0=(col>>0)&0x1f;
      u32 g0=(col>>5)&0x1f;
      u32 b0=(col>>10)&0x1f;
      u32 r1=(col>>(16+0))&0x1f;
      u32 g1=(col>>(16+5))&0x1f;
      u32 b1=(col>>(16+10))&0x1f;
      r0=(r0+sr)>>2;
      g0=(g0+sg)>>2;
      b0=(b0+sb)>>2;
      r1=(r1+sr)>>2;
      g1=(g1+sg)>>2;
      b1=(b1+sb)>>2;
      col=(RGB15(r0,g0,b0)|BIT15)|((RGB15(r1,g1,b1)|BIT15)<<16);
      *pbuf32++=col;
      pbuf+=2;
      tmpw-=2;
    }
    if(tmpw!=0){
      u32 col=*pbuf;
      u32 r=(col>>0)&0x1f;
      u32 g=(col>>5)&0x1f;
      u32 b=(col>>10)&0x1f;
      r=(r+sr)>>2;
      g=(g+sg)>>2;
      b=(b+sb)>>2;
      *pbuf++=RGB15(r,g,b)|BIT15;
    }
  }
}

