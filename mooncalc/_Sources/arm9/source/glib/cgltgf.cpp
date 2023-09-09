
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cgltgf.h"

#include "cglstream.h"

CglTGF::CglTGF(TMM *_pMM,const u8 *_buf,const int _size)
{
  pMM=_pMM;
  
  CglStream stream(_buf,_size);
  
  Width=stream.Readu16();
  Height=stream.Readu16();
  
  datasize=_size;
  
//  if(VerboseDebugLog==true) _consolePrintf("new CglTGF: w,h=%d,%d size=%d\n",Width,Height,datasize);
  
  int size=stream.GetSize()-stream.GetOffset();
  pdata=(u16*)safemalloc_chkmem(pMM,size);
  
  stream.ReadBuffer(pdata,size);
  
  ppLineOffsets=(u16**)safemalloc_chkmem(pMM,Height*4);
  
  u16 *_pdata=pdata;
  
  for(int y=0;y<Height;y++){
    ppLineOffsets[y]=_pdata;
    int x=0;
    while(x<Width){
      u16 curdata=*_pdata++;
      int alpha=curdata & 0xff;
      int len=curdata >> 8;
      
      x+=len;
      if(alpha!=31) _pdata+=len;
    }
  }
}

CglTGF::~CglTGF(void)
{
  safefree(pMM,pdata); pdata=NULL;
  safefree(pMM,ppLineOffsets); ppLineOffsets=NULL;
}

u32 CglTGF::GetDataSize(void) const
{
  return(datasize);
}

u16* CglTGF::GetData(void) const
{
  return(pdata);
}

int CglTGF::GetWidth(void) const
{
  return(Width);
}

int CglTGF::GetHeight(void) const
{
  return(Height);
}

asm u32 CglTGF_BitBlt_Body(const u16 *pdata,const u16 *pBuf,const u32 Width)
{
REG_pdata RN r0
REG_pBuf RN r1
REG_Width RN r2

REG_pdataMaster RN r4
REG_alpha RN r5
REG_len RN r6
REG_src RN r7
REG_add RN r8
REG_tmp1 RN r9
REG_tmp2 RN r10
REG_tmp3 RN r11
REG_jumptable RN lr
  
  PUSH {r4,r5,r6,r7,r8,r9,r10,r11,lr}
  
  mov REG_pdataMaster,REG_pdata
  ldr REG_jumptable,=CglTGF_BitBlt_jumptable
  
CglTGF_BitBlt_Start
  ldrh REG_alpha,[REG_pdata],#2
  lsr REG_len,REG_alpha,#8
  and REG_alpha,#0xff
  
  sub REG_Width,REG_len ; Width-=len for interlock
  ldr pc,[REG_jumptable,REG_alpha,lsl #2]

  MACRO  
  CglTGF_BitBlt_TransEnd
  sub r0,REG_pdata,REG_pdataMaster
  POP {r4,r5,r6,r7,r8,r9,r10,r11,pc}
  MEND
  
CglTGF_BitBlt_Alpha0
CglTGF_BitBlt_Alpha0_Loop
  ldrh REG_add,[REG_pdata],#2
  subs REG_len,#1
  strh REG_add,[REG_pBuf],#2
  bne CglTGF_BitBlt_Alpha0_Loop
        
  cmp REG_Width,#0
  bne CglTGF_BitBlt_Start
  CglTGF_BitBlt_TransEnd

CglTGF_BitBlt_Alpha2
  ldr REG_tmp1,=((1<<4)<<0) | ((1<<4)<<5) | ((1<<4)<<10)
CglTGF_BitBlt_Alpha2_Loop
  ldrh REG_src,[REG_pBuf]
  ldrh REG_add,[REG_pdata],#2
  subs REG_len,#1
  and REG_src,REG_src,REG_tmp1
  add REG_add,REG_add,REG_src,lsr #4
  strh REG_add,[REG_pBuf],#2
  bne CglTGF_BitBlt_Alpha2_Loop
  
  cmp REG_Width,#0
  bne CglTGF_BitBlt_Start
  CglTGF_BitBlt_TransEnd

CglTGF_BitBlt_Alpha4
  ldr REG_tmp1,=((3<<3)<<0) | ((3<<3)<<5) | ((3<<3)<<10)
CglTGF_BitBlt_Alpha4_Loop
  ldrh REG_src,[REG_pBuf]
  ldrh REG_add,[REG_pdata],#2
  subs REG_len,#1
  and REG_src,REG_src,REG_tmp1
  add REG_add,REG_add,REG_src,lsr #3
  strh REG_add,[REG_pBuf],#2
  bne CglTGF_BitBlt_Alpha4_Loop
  
  cmp REG_Width,#0
  bne CglTGF_BitBlt_Start
  CglTGF_BitBlt_TransEnd

CglTGF_BitBlt_Alpha8
  ldr REG_tmp1,=((7<<2)<<0) | ((7<<2)<<5) | ((7<<2)<<10)
CglTGF_BitBlt_Alpha8_Loop
  ldrh REG_src,[REG_pBuf]
  ldrh REG_add,[REG_pdata],#2
  subs REG_len,#1
  and REG_src,REG_src,REG_tmp1
  add REG_add,REG_add,REG_src,lsr #2
  strh REG_add,[REG_pBuf],#2
  bne CglTGF_BitBlt_Alpha8_Loop
  
  cmp REG_Width,#0
  bne CglTGF_BitBlt_Start
  CglTGF_BitBlt_TransEnd

CglTGF_BitBlt_Alpha16
  ldr REG_tmp1,=((15<<1)<<0) | ((15<<1)<<5) | ((15<<1)<<10)
CglTGF_BitBlt_Alpha16_Loop
  ldrh REG_src,[REG_pBuf]
  ldrh REG_add,[REG_pdata],#2
  subs REG_len,#1
  and REG_src,REG_src,REG_tmp1
  add REG_add,REG_add,REG_src,lsr #1
  strh REG_add,[REG_pBuf],#2
  bne CglTGF_BitBlt_Alpha16_Loop
  
  cmp REG_Width,#0
  bne CglTGF_BitBlt_Start
  CglTGF_BitBlt_TransEnd

CglTGF_BitBlt_AlphaAny
  lsl REG_alpha,#11 ; alpha 5bit to 16bit
  
CglTGF_BitBlt_AlphaAny_Loop
  ldrh REG_src,[REG_pBuf]
  ldrh REG_add,[REG_pdata],#2
        
  and REG_tmp1,REG_src,#(0x1f<<0)
  smulwb REG_tmp1,REG_alpha,REG_tmp1
  and REG_tmp2,REG_src,#(0x1f<<5)
  add REG_add,REG_tmp1
  smulwb REG_tmp2,REG_alpha,REG_tmp2
  and REG_tmp3,REG_src,#(0x1f<<10)
  smulwb REG_tmp3,REG_alpha,REG_tmp3
  and REG_tmp2,REG_tmp2,#(0x1f<<5)
  add REG_add,REG_tmp2
  and REG_tmp3,REG_tmp3,#(0x1f<<10)
  add REG_add,REG_tmp3
        
  strh REG_add,[REG_pBuf],#2
        
  subs REG_len,#1
  bne CglTGF_BitBlt_AlphaAny_Loop
  
  cmp REG_Width,#0
  bne CglTGF_BitBlt_Start
  CglTGF_BitBlt_TransEnd

CglTGF_BitBlt_Alpha31
  add REG_pBuf,REG_len,lsl #1
  
  cmp REG_Width,#0
  bne CglTGF_BitBlt_Start
  CglTGF_BitBlt_TransEnd

CglTGF_BitBlt_jumptable
  DCD CglTGF_BitBlt_Alpha0,CglTGF_BitBlt_Alpha0,CglTGF_BitBlt_Alpha2,CglTGF_BitBlt_AlphaAny
  DCD CglTGF_BitBlt_Alpha4,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny
  DCD CglTGF_BitBlt_Alpha8,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny
  DCD CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny
  DCD CglTGF_BitBlt_Alpha16,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny
  DCD CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny
  DCD CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny
  DCD CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_AlphaAny,CglTGF_BitBlt_Alpha31
}

void CglTGF::BitBlt(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop) const
{
  u16 *_pdata=pdata; 
  u16 *pBuf=pDestCanvas->GetVRAMBuf();
  pBuf=&pBuf[(nDestTop*pDestCanvas->GetWidth())+nDestLeft];
  
  u32 DestWidth=pDestCanvas->GetWidth();
  
  for(int y=0;y<Height;y++){
    u32 srclen=CglTGF_BitBlt_Body(_pdata,pBuf,Width);
//    _consolePrintf("f%d/%d: %x,%x %d %d\n",y,Height,_pdata,pBuf,Width,srclen);
    _pdata+=srclen/2;
    pBuf+=DestWidth;
  }
}

void CglTGF::BitBltLimitY(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nHeight,const int nSrcTop) const
{
  u16 *_pdata=ppLineOffsets[nSrcTop];
  u16 *pBuf=pDestCanvas->GetVRAMBuf();
  pBuf=&pBuf[(nDestTop*pDestCanvas->GetWidth())+nDestLeft];
  
  u32 DestWidth=pDestCanvas->GetWidth();
  
  if(nSrcTop<0) return;
  if(Height<=nSrcTop) return;
  if(nHeight<=0) return;
  if(Height<nHeight) return;
  
  for(int y=0;y<nHeight;y++){
    u32 srclen=CglTGF_BitBlt_Body(_pdata,pBuf,Width);
//    _consolePrintf("y%d/%d: %x,%x %d %d\n",y,nHeight,_pdata,pBuf,Width,srclen);
    _pdata+=srclen/2;
    pBuf+=DestWidth;
  }
}

