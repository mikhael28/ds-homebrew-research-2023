
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglfont.h"

#include "cglstream.h"

static u32 FontWidthPadding;

CglFont::CglFont(TMM *_pMM,const u8 *_buf,const int _size)
{
  pMM=_pMM;
  
  CglStream stream(_buf,_size);
  
  u32 FontCount;
  
  if(_size<2048){
    FontCount=0x80;
    }else{
    FontCount=0x10000;
  }
  
  DataTable=(u16**)safemalloc_chkmem(pMM,FontCount*4);
  
  u32 DataSize=stream.GetSize()-(FontCount*1);
  Data=(u16*)safemalloc_chkmem(pMM,DataSize);
  
  u8 *pDiffTbl=(u8*)safemalloc_chkmem(&MM_Temp,FontCount*1);
  
  stream.ReadBuffer(pDiffTbl,FontCount*1);
  FontWidthPadding=pDiffTbl[0];
  pDiffTbl[0]=0;
  
  stream.ReadBuffer(Data,DataSize);
  
  u32 Height=Data[0];
  if(glFontHeight!=Height) StopFatalError(13004,"Not support font height. (%d!=%d)\n",glFontHeight,Height);
  
  u32 LastOffset=0;
  
  for(int idx=0;idx<FontCount;idx++){
    u32 diff=pDiffTbl[idx];
    if(diff==0){
      static u16 WidthZero=0;
      DataTable[idx]=&WidthZero;
      }else{
      LastOffset+=diff;
      DataTable[idx]=&Data[LastOffset/2];
    }
  }
  
  if(pDiffTbl!=NULL){
    safefree(&MM_Temp,pDiffTbl); pDiffTbl=NULL;
  }
  
  TextColor=RGB15(0,0,0)|BIT15;
}

CglFont::~CglFont(void)
{
  safefree(pMM,DataTable); DataTable=NULL;
  safefree(pMM,Data); Data=NULL;
}

u16* CglFont::GetBulkData(const TglUnicode uidx) const
{
  u16 *pBulkData=DataTable[uidx];
  if(*pBulkData==0) pBulkData=DataTable[(u32)'?'];
  PLD(pBulkData);
  return(pBulkData);
}

inline asm void DrawFont1bppUnder8pix(const u16 *pBulkData,u16 *pbuf,u32 bufwidth,u32 TextColor)
{
df1bu8_pBulkData RN r0
df1bu8_pbuf RN r1
df1bu8_bufwidth RN r2
df1bu8_TextColor RN r3
df1bu8_Height RN r4
df1bu8_BitImage RN lr

  PUSH {r4,lr}
  
  mov df1bu8_Height,#12/2
  
df1bu8_DrawFont1bpp_LoopYStart

  ldrh df1bu8_BitImage,[df1bu8_pBulkData],#2
  
  tsts df1bu8_BitImage,#1<<((8*0)+0)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*0]
  tsts df1bu8_BitImage,#1<<((8*0)+1)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*1]
  tsts df1bu8_BitImage,#1<<((8*0)+2)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*2]
  tsts df1bu8_BitImage,#1<<((8*0)+3)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*3]
  tsts df1bu8_BitImage,#1<<((8*0)+4)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*4]
  tsts df1bu8_BitImage,#1<<((8*0)+5)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*5]
  tsts df1bu8_BitImage,#1<<((8*0)+6)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*6]
  tsts df1bu8_BitImage,#1<<((8*0)+7)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*7]
  
  add df1bu8_pbuf,df1bu8_bufwidth
  
  tsts df1bu8_BitImage,#1<<((8*1)+0)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*0]
  tsts df1bu8_BitImage,#1<<((8*1)+1)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*1]
  tsts df1bu8_BitImage,#1<<((8*1)+2)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*2]
  tsts df1bu8_BitImage,#1<<((8*1)+3)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*3]
  tsts df1bu8_BitImage,#1<<((8*1)+4)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*4]
  tsts df1bu8_BitImage,#1<<((8*1)+5)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*5]
  tsts df1bu8_BitImage,#1<<((8*1)+6)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*6]
  tsts df1bu8_BitImage,#1<<((8*1)+7)
  strhne df1bu8_TextColor,[df1bu8_pbuf,#2*7]
  
  add df1bu8_pbuf,df1bu8_bufwidth
  
  subs df1bu8_Height,#1
  bne df1bu8_DrawFont1bpp_LoopYStart
  
  POP {r4,pc}
}

inline asm void DrawFont1bppUnder12pix(const u16 *pBulkData,u16 *pbuf,u32 bufwidth,u32 TextColor)
{
df1bu12_pBulkData RN r0
df1bu12_pbuf RN r1
df1bu12_bufwidth RN r2
df1bu12_TextColor RN r3
df1bu12_Height RN r4
df1bu12_BitImage RN lr

  PUSH {r4,lr}
  
  mov df1bu12_Height,#12
  
df1bu12_DrawFont1bpp_LoopYStart

  ldrh df1bu12_BitImage,[df1bu12_pBulkData],#2
  
  tsts df1bu12_BitImage,#1<<0
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*0]
  tsts df1bu12_BitImage,#1<<1
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*1]
  tsts df1bu12_BitImage,#1<<2
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*2]
  tsts df1bu12_BitImage,#1<<3
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*3]
  tsts df1bu12_BitImage,#1<<4
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*4]
  tsts df1bu12_BitImage,#1<<5
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*5]
  tsts df1bu12_BitImage,#1<<6
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*6]
  tsts df1bu12_BitImage,#1<<7
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*7]
  tsts df1bu12_BitImage,#1<<8
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*8]
  tsts df1bu12_BitImage,#1<<9
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*9]
  tsts df1bu12_BitImage,#1<<10
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*10]
  tsts df1bu12_BitImage,#1<<11
  strhne df1bu12_TextColor,[df1bu12_pbuf,#2*11]
  
  add df1bu12_pbuf,df1bu12_bufwidth
  
  subs df1bu12_Height,#1
  bne df1bu12_DrawFont1bpp_LoopYStart
  
  POP {r4,pc}
}

inline asm void DrawFont1bppUnder16pix(const u16 *pBulkData,u16 *pbuf,u32 bufwidth,u32 TextColor)
{
df1bu16_pBulkData RN r0
df1bu16_pbuf RN r1
df1bu16_bufwidth RN r2
df1bu16_TextColor RN r3
df1bu16_Height RN r4
df1bu16_BitImage RN lr

  PUSH {r4,lr}
  
  mov df1bu16_Height,#12
  
df1bu16_DrawFont1bpp_LoopYStart

  ldrh df1bu16_BitImage,[df1bu16_pBulkData],#2
  
  tsts df1bu16_BitImage,#1<<0
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*0]
  tsts df1bu16_BitImage,#1<<1
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*1]
  tsts df1bu16_BitImage,#1<<2
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*2]
  tsts df1bu16_BitImage,#1<<3
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*3]
  tsts df1bu16_BitImage,#1<<4
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*4]
  tsts df1bu16_BitImage,#1<<5
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*5]
  tsts df1bu16_BitImage,#1<<6
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*6]
  tsts df1bu16_BitImage,#1<<7
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*7]
  tsts df1bu16_BitImage,#1<<8
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*8]
  tsts df1bu16_BitImage,#1<<9
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*9]
  tsts df1bu16_BitImage,#1<<10
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*10]
  tsts df1bu16_BitImage,#1<<11
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*11]
  tsts df1bu16_BitImage,#1<<12
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*12]
  tsts df1bu16_BitImage,#1<<13
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*13]
  tsts df1bu16_BitImage,#1<<14
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*14]
  tsts df1bu16_BitImage,#1<<15
  strhne df1bu16_TextColor,[df1bu16_pbuf,#2*15]
  
  add df1bu16_pbuf,df1bu16_bufwidth
  
  subs df1bu16_Height,#1
  bne df1bu16_DrawFont1bpp_LoopYStart
  
  POP {r4,pc}
}

void CglFont::DrawFont(CglCanvas *pCanvas,const int x,const int y,const TglUnicode uidx) const
{
  const u16 *pBulkData=GetBulkData(uidx);
  
  int Width=*pBulkData++;
  if(Width==0) return;
  
  u16 *pbuf=pCanvas->GetVRAMBuf();
  u32 bufwidth=pCanvas->GetWidth();
  
  pbuf=&pbuf[(y*bufwidth)+x];
  
  if(Width<=8){
    DrawFont1bppUnder8pix(pBulkData,pbuf,bufwidth*2,TextColor);
    }else{
    if(Width<=12){
      DrawFont1bppUnder12pix(pBulkData,pbuf,bufwidth*2,TextColor);
      }else{
      DrawFont1bppUnder16pix(pBulkData,pbuf,bufwidth*2,TextColor);
    }
  }
}

static bool isUnicodeArabic(u32 uc)
{
  if(uc<0x0600) return(false);
  
  if((0x0600<=uc)&&(uc<0x0700)) return(true); // UNICODE Arabic
  if((0xfb50<=uc)&&(uc<0xfe00)) return(true); // UNICODE Arabic Presentation Forms-A
  if((0xfe70<=uc)&&(uc<0xff00)) return(true); // UNICODE Arabic Presentation Forms-B
  return(false);
}

int CglFont::GetFontWidth(const TglUnicode uidx) const
{
  u16 *BulkData=GetBulkData(uidx);
  
  int Width=*BulkData;
  
  if(isUnicodeArabic(uidx)==true) Width--;
  
  return(Width+FontWidthPadding);
}

void CglFont::SetTextColor(const u16 Color)
{
  TextColor=Color;
}

u16 CglFont::GetTextColor(void)
{
  return(TextColor);
}

bool CglFont::isExists(const TglUnicode uidx) const
{
  if(*DataTable[uidx]!=0) return(true);
  return(false);
}

