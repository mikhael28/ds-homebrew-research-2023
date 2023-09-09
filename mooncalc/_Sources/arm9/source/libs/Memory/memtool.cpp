
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_const.h"

#include "memtool.h"
#include "arm9tcm.h"

#define MEMCHK_COPY(align,p1,p2,p3) { \
  if((p1==NULL)||(p2==NULL)||(p3==0)||(((u32)p1&align)!=0)||(((u32)p2&align)!=0)){ \
    StopFatalError(13406,"COPY. Hooked memory address error. %s%d (%d) p1=0x%08x,p2=0x%08x,p3=0x%08x\n",__FILE__,__LINE__,align,p1,p2,p3); \
  } \
}

#define MEMCHK_SET(align,p1,p2,p3) { \
  if((p2==NULL)||(p3==0)||(((u32)p2&align)!=0)){ \
    StopFatalError(13407,"SET. Hooked memory address error. %s%d (%d) p1=0x%08x,p2=0x%08x,p3=0x%08x\n",__FILE__,__LINE__,align,p1,p2,p3); \
  } \
}

#undef MEMCHK_COPY
#define MEMCHK_COPY(align,p1,p2,p3)
#undef MEMCHK_SET
#define MEMCHK_SET(align,p1,p2,p3)

#define CACHE_LINE_SIZE (32)

void DCache_FlushRangeOverrun(const void *v,u32 size)
{
//  Flush up. (ダーティーデータをライトバックせずにキャッシュを無効化する）

  u32 va=(u32)v;
  if((va<0x02000000)||(0x02400000<=va)) return;
  
  va&=~(CACHE_LINE_SIZE-1);
  size+=CACHE_LINE_SIZE;
  
  size+=CACHE_LINE_SIZE-1;
  size&=~(CACHE_LINE_SIZE-1);
  
  if(size==0) return;

  while(size!=0){  
	  asm {
	    mcr p15, 0, va, c7, c6, 1
	  }
    va+=CACHE_LINE_SIZE;
    size-=CACHE_LINE_SIZE;
  }
  
  u32 zero;
  asm {
    mov zero,#0
    MCR p15, 0, zero, c7, c10, 4 // drain write buffer
  }
}

void DCache_CleanRangeOverrun(const void *v,u32 size)
{
// Clean up. (ダーティーデータをライトバッファに送ってクリアする。キャッシュは有効のまま）

  u32 va=(u32)v;
  if((va<0x02000000)||(0x02400000<=va)) return;

  va&=~(CACHE_LINE_SIZE-1);
  size+=CACHE_LINE_SIZE;
  
  size+=CACHE_LINE_SIZE-1;
  size&=~(CACHE_LINE_SIZE-1);
  
  if(size==0) return;

  while(size!=0){  
	  asm {
      mcr p15, 0, va, c7, c10, 1
	  }
    va+=CACHE_LINE_SIZE;
    size-=CACHE_LINE_SIZE;
	}
}

CODE_IN_ITCM_MemTool void MemCopy8CPU(const void *src,void *dst,u32 len)
{MEMCHK_COPY(0,src,dst,len);
  if((len&1)==0){
    if( (((u32)src&1)==0) && (((u32)dst&1)==0) ){
      MemCopy16CPU(src,dst,len);
      return;
    }
  }
  
  len>>=0;
  if(len==0) return;
  
  u8 *_src=(u8*)src;
  u8 *_dst=(u8*)dst;
  
  for(u32 idx=0;idx<len;idx++){
    _dst[idx]=_src[idx];
  }
}

CODE_IN_ITCM_MemTool void MemCopy16CPU(const void *src,void *dst,u32 len)
{MEMCHK_COPY(1,src,dst,len);
  if((len&3)==0){
    if( (((u32)src&3)==0) && (((u32)dst&3)==0) ){
      MemCopy32CPU(src,dst,len);
      return;
    }
  }
  
  len>>=1;
  if(len==0) return;
  
  u16 *_src=(u16*)src;
  u16 *_dst=(u16*)dst;
  
  for(u32 idx=0;idx<len;idx++){
    _dst[idx]=_src[idx];
  }
}

asm CODE_IN_ITCM_MemTool void MemCopy32CPU(const void *src,void *dst,u32 len)
{/* MEMCHK_COPY(3,src,dst,len); */
c32psrc RN r0
c32pdst RN r1
c32size RN r2
  
  cmp c32size,#0
  bxeq lr
  
  PUSH {r4,r5,r6,r7,r8,r9,r10}
  
  cmp c32size,#4*8
  blo c32set32x1
    
c32set32x8
  ldmia c32psrc!,{r3,r4,r5,r6,r7,r8,r9,r10}
  stmia c32pdst!,{r3,r4,r5,r6,r7,r8,r9,r10}
  subs c32size,c32size,#4*8
  cmp c32size,#4*8
  bhs c32set32x8
      
  cmp c32size,#0
  beq c32setend
   
c32set32x1
  ldr r3,[c32psrc],#4
  subs c32size,c32size,#4
  str r3,[c32pdst],#4
  bne c32set32x1

c32setend
  POP {r4,r5,r6,r7,r8,r9,r10}
  bx lr      
}

CODE_IN_ITCM_MemTool void MemSet8CPU(u8 v,void *dst,u32 len)
{MEMCHK_SET(0,v,dst,len);
  len>>=0;
  if(len==0) return;
  
  u8 *_dst=(u8*)dst;
  
  for(u32 cnt=0;cnt<len;cnt++){
    _dst[cnt]=v;
  }
}

CODE_IN_ITCM_MemTool void MemSet16CPU(u16 v,void *dst,u32 len)
{MEMCHK_SET(1,v,dst,len);
  len>>=1;
  if(len==0) return;

  u16 *_dst=(u16*)dst;
  
  for(u32 idx=0;idx<len;idx++){
    _dst[idx]=v;
  }
}

asm CODE_IN_ITCM_MemTool void MemSet32CPU(u32 v,void *dst,u32 len)
{/* MEMCHK_SET(3,v,dst,len); */
s32data RN r0
s32pbuf RN r1
s32size RN r2
  
  cmp s32size,#0
  bxeq lr
  
  PUSH {r4,r5,r6,r7,r8,r9}
  
  mov r3,s32data
  mov r4,s32data
  mov r5,s32data
  mov r6,s32data
  mov r7,s32data
  mov r8,s32data
  mov r9,s32data
  
  cmp s32size,#4*8
  blo s32set32x1
    
s32set32x8
  stmia s32pbuf!,{s32data,r3,r4,r5,r6,r7,r8,r9}
  subs s32size,s32size,#4*8
  cmp s32size,#4*8
  bhs s32set32x8
      
  cmp s32size,#0
  beq s32setend
   
s32set32x1
  str s32data,[s32pbuf],#4
  subs s32size,s32size,#4
  bne s32set32x1

s32setend
  POP {r4,r5,r6,r7,r8,r9}
  bx lr      
}

// ----------------------------------------------

bool (*safemalloc_CallBack_RequestFreeMemory_PlugSound)(void)=NULL;
bool (*safemalloc_CallBack_RequestFreeMemory_PlugImage)(void)=NULL;
bool (*safemalloc_CallBack_RequestFreeMemory_NDSFiles)(void)=NULL;

static bool CallRequestFree(void)
{
  bool retry=false;
  
  static bool rn=false;
  
  if(rn==false){
    rn=true;
    if((retry==false)&&(safemalloc_CallBack_RequestFreeMemory_PlugSound!=NULL)){
      if(safemalloc_CallBack_RequestFreeMemory_PlugSound()==true) retry=true;
    }
    if((retry==false)&&(safemalloc_CallBack_RequestFreeMemory_PlugImage!=NULL)){
      if(safemalloc_CallBack_RequestFreeMemory_PlugImage()==true) retry=true;
    }
    }else{
    rn=false;
    if((retry==false)&&(safemalloc_CallBack_RequestFreeMemory_PlugImage!=NULL)){
      if(safemalloc_CallBack_RequestFreeMemory_PlugImage()==true) retry=true;
    }
    if((retry==false)&&(safemalloc_CallBack_RequestFreeMemory_PlugSound!=NULL)){
      if(safemalloc_CallBack_RequestFreeMemory_PlugSound()==true) retry=true;
    }
  }
  
  if((retry==false)&&(safemalloc_CallBack_RequestFreeMemory_NDSFiles!=NULL)){
    if(safemalloc_CallBack_RequestFreeMemory_NDSFiles()==true) retry=true;
  }
  
  return(retry);
}

// ----------------------------------------------

#include "memtool_MM.h"

// ----------------------------------------------

//#define StrangeCheckOverRange

void *__safemalloc__(TMM *pMM,const char *filename,int linenum,const char *funcname,int size)
{
// _consolePrintf("safemalloc(%d);\n",size);

#ifdef StrangeCheckOverRange
  MM_CheckOverRange();
#endif
  if(pMM==NULL) __StopFatalError__(filename,linenum,funcname,106,"MemMgr is NULL.");
  
  if(size<=0) return(NULL);
  
  void *ptr=NULL;
  
  while(ptr==NULL){
    ptr=malloc(size+(8*2)); // 先頭直前と終端直後に検査コードを入れる
    if(ptr!=NULL) break;
    
    bool retry=CallRequestFree();
    if(retry==false){
      _consolePrintf("safemalloc(%d) failed allocate error. not use auto allocator.\ncalled from %s:%d %s.\n",size,filename,linenum,funcname);
      return(NULL);
    }
  }
  
  ptr=(void*)((u32)ptr+8);
  
//  _consolePrintf("safemalloc(%d)=0x%08x\n",size,ptr);

  MM_Set(pMM,filename,linenum,funcname,(u32)ptr,size);
  
  u32 *pbuf32=(u32*)ptr;
  pbuf32[-2]=0xa5a6a7a8;
  pbuf32[-1]=0xa1a2a3a4;
  
  u8 *pbuf8=(u8*)ptr;
  pbuf8+=size;
  pbuf8[0]=0xb0;
  pbuf8[1]=0xb1;
  pbuf8[2]=0xb2;
  pbuf8[3]=0xb3;
  pbuf8[4]=0xb4;
  pbuf8[5]=0xb5;
  pbuf8[6]=0xb6;
  pbuf8[7]=0xb7;
  
//  _consolePrintf("%s:%d %s: safemalloc(%d)==0x%08x\n",filename,linenum,funcname,size,ptr);
  
  return(ptr);
}

void *__safemalloc_chkmem__(TMM *pMM,const char *filename,int linenum,const char *funcname,int size)
{
#ifdef StrangeCheckOverRange
  MM_CheckOverRange();
#endif
  if(pMM==NULL) __StopFatalError__(filename,linenum,funcname,107,"MemMgr is NULL.");
  
  void *p=__safemalloc__(pMM,filename,linenum,funcname,size);
  
  if(p==NULL){
    MM_ShowAllocated(&MM_Temp);
    MM_ShowAllocated(&MM_System);
    MM_ShowAllocated(&MM_SystemAfter);
    MM_ShowAllocated(&MM_Skin);
    MM_ShowAllocated(&MM_DLLImage);
    MM_ShowAllocated(&MM_DLLSound);
    MM_ShowAllocated(&MM_DLLDPG);
    MM_ShowAllocated(&MM_SKKOSK);
    MM_ShowAllocated(&MM_Process);
    PrintFreeMem_Accuracy_Already();
    __StopFatalError__(filename,linenum,funcname,101,"Insufficient memory. malloc(%d)\n",size);
  }
  
//  _consolePrintf("%s:%d %s: safemalloc_chkmem(%d)==0x%08x\n",filename,linenum,funcname,size,p);
  
  return(p);
}

void __safefree__(TMM *pMM,const char *filename,int linenum,const char *funcname,const void *ptr)
{
#ifdef StrangeCheckOverRange
  MM_CheckOverRange();
#endif
  if(pMM==NULL) __StopFatalError__(filename,linenum,funcname,108,"MemMgr is NULL.");
  
  if(ptr==NULL) __StopFatalError__(filename,linenum,funcname,102,"Request free null pointer in safefree.\n");
  
//  _consolePrintf("%s:%d %s: safefree(0x%08x)==%dbyte.\n",filename,linenum,funcname,ptr,MM_GetSize(pMM,(u32)ptr));
  
  {
    TMM_List *pList=NULL;
    for(u32 idx=0;idx<pMM->ListCount;idx++){
      TMM_List *_pList=&pMM->pLists[idx];
      if(_pList->adr==(u32)ptr) pList=_pList;
    }
    if(pList==NULL) __StopFatalError__(filename,linenum,funcname,104,"Not found target ptr in MM->pLists. [0x%08x]\n",ptr);
    
    u32 size=pList->size;
    u8 *pbuf=(u8*)ptr;
    
    if((pbuf[-8]!=0xa8)||(pbuf[-7]!=0xa7)||(pbuf[-6]!=0xa6)||(pbuf[-5]!=0xa5)||(pbuf[-4]!=0xa4)||(pbuf[-3]!=0xa3)||(pbuf[-2]!=0xa2)||(pbuf[-1]!=0xa1)||
       (pbuf[size+0]!=0xb0)||(pbuf[size+1]!=0xb1)||(pbuf[size+2]!=0xb2)||(pbuf[size+3]!=0xb3)||(pbuf[size+4]!=0xb4)||(pbuf[size+5]!=0xb5)||(pbuf[size+6]!=0xb6)||(pbuf[size+7]!=0xb7)){
      MM_ShowAllocated(&MM_Temp);
      MM_ShowAllocated(&MM_System);
      MM_ShowAllocated(&MM_SystemAfter);
      MM_ShowAllocated(&MM_Skin);
      MM_ShowAllocated(&MM_DLLImage);
      MM_ShowAllocated(&MM_DLLSound);
      MM_ShowAllocated(&MM_DLLDPG);
      MM_ShowAllocated(&MM_SKKOSK);
      MM_ShowAllocated(&MM_Process);
      _consolePrint("\n");
      _consolePrintf("adr=0x%08x size=%d %s:%d %s\n",pbuf,size,pList->filename,pList->linenum,pList->funcname);
      for(u32 idx=8;idx>0;idx--){
        _consolePrintf("%02x,",pbuf[-idx]);
      }
      _consolePrint("\n");
      for(u32 idx=0;idx<24;idx++){
        _consolePrintf("%02x,",pbuf[size+idx-8]);
      }
      _consolePrint("\n");
      __StopFatalError__(filename,linenum,funcname,105,"Memory check error. Ignore writing code?\n");
    }
    
  }
  
  MM_Clear(pMM,(u32)ptr);
  
  ptr=(void*)((u32)ptr-8);
  free((void*)ptr);
}

void *__safecalloc__(TMM *pMM,const char *filename,int linenum,const char *funcname,int nmemb, int size)
{
#ifdef StrangeCheckOverRange
  MM_CheckOverRange();
#endif
  if(pMM==NULL) __StopFatalError__(filename,linenum,funcname,109,"MemMgr is NULL.");
  
  size*=nmemb;
  
// _consolePrintf("safecalloc(%d);\n",size);

  if(size<=0) return(NULL);
  
  void *ptr=NULL;
  
  while(ptr==NULL){
    ptr=malloc(size+(8*2)); // 先頭直前と終端直後に検査コードを入れる
    if(ptr!=NULL) break;
    
    bool retry=CallRequestFree();
    if(retry==false){
      _consolePrintf("safemalloc(%d) failed allocate error. not use auto allocator.\n",size);
      return(NULL);
    }
  }
  
  ptr=(void*)((u32)ptr+8);
  
//  _consolePrintf("safecalloc(%d)=0x%08x\n",size,ptr);

  MM_Set(pMM,filename,linenum,funcname,(u32)ptr,size);
  
  u32 *pbuf32=(u32*)ptr;
  pbuf32[-2]=0xa5a6a7a8;
  pbuf32[-1]=0xa1a2a3a4;
  
  u8 *pbuf8=(u8*)ptr;
  pbuf8+=size;
  pbuf8[0]=0xb0;
  pbuf8[1]=0xb1;
  pbuf8[2]=0xb2;
  pbuf8[3]=0xb3;
  pbuf8[4]=0xb4;
  pbuf8[5]=0xb5;
  pbuf8[6]=0xb6;
  pbuf8[7]=0xb7;
  
  MemSet8CPU(0,ptr,size);
  
  return(ptr);
}

void *__saferealloc__(TMM *pMM,const char *filename,int linenum,const char *funcname,void *ptr, int size)
{
#ifdef StrangeCheckOverRange
  MM_CheckOverRange();
#endif
  if(pMM==NULL) __StopFatalError__(filename,linenum,funcname,110,"MemMgr is NULL.");
  
  if(ptr==NULL) ptr=safemalloc(pMM,size);
  if(ptr==NULL) __StopFatalError__(filename,linenum,funcname,103,"Insufficient memory. realloc(%d)\n",size);
  
  MM_Clear(pMM,(u32)ptr);
  
  ptr=(void*)((u32)ptr-8);
  ptr=realloc(ptr,size+(8*2));
  ptr=(void*)((u32)ptr+8);

  MM_Set(pMM,filename,linenum,funcname,(u32)ptr,size);
  
  u32 *pbuf32=(u32*)ptr;
  pbuf32[-2]=0xa5a6a7a8;
  pbuf32[-1]=0xa1a2a3a4;
  
  u8 *pbuf8=(u8*)ptr;
  pbuf8+=size;
  pbuf8[0]=0xb0;
  pbuf8[1]=0xb1;
  pbuf8[2]=0xb2;
  pbuf8[3]=0xb3;
  pbuf8[4]=0xb4;
  pbuf8[5]=0xb5;
  pbuf8[6]=0xb6;
  pbuf8[7]=0xb7;
  
  return(ptr);
}

// ---------------------------------------------------------------------------------

void PrintFreeMem_Simple(void)
{
  const u32 FreeMemSeg=2*1024;
  
  for(u32 i=1*FreeMemSeg;i<4*1024*1024;i+=FreeMemSeg){
    void *ptr=malloc(i);
    if(ptr==NULL){
      _consolePrintf("FreeMem=%dbyte\n",i-FreeMemSeg);
      break;
    }
    free(ptr); ptr=NULL;
  }
}

void PrintFreeMem_Accuracy(void)
{
  if(VerboseDebugLog==false) return;
  PrintFreeMem_Accuracy_Already();
}

void PrintFreeMem_Accuracy_Already(void)
{
  const u32 maxsize=4*1024*1024;
  const u32 segsize=1*1024;
  const u32 count=maxsize/segsize;
  u32 *pptrs=(u32*)malloc(count*4);
  
  if(pptrs==NULL){
    _consolePrintf("PrintFreeMem_Accuracy: Investigation was interrupted. Very low free area.\n");
    return;
  }
  
  u32 FreeMemSize=0;
  u32 MaxBlockSize=0;
  
  for(u32 idx=0;idx<count;idx++){
    u32 size=maxsize-(segsize*idx);
    pptrs[idx]=(u32)malloc(size);
    if(pptrs[idx]!=0){
      FreeMemSize+=size;
      if(MaxBlockSize<size) MaxBlockSize=size;
    }
  }
  
  _consolePrintf("AccuracyFreeMem=%dbyte (MaxBlockSize=%dbyte)\n",FreeMemSize,MaxBlockSize);
  
  for(u32 idx=0;idx<count;idx++){
    if(pptrs[idx]!=0){
      free((void*)pptrs[idx]); pptrs[idx]=0;
    }
  }
  
  free(pptrs); pptrs=NULL;
}

void PrintFreeMem(void)
{
  if(VerboseDebugLog==false) return;
  
  PrintFreeMem_Simple(); return;
  
  PrintFreeMem_Accuracy();
}

u32 GetMaxMemoryBlockSize(void)
{
  const u32 FreeMemSeg=64*1024;
  for(u32 i=4*1024*1024;i!=FreeMemSeg;i-=FreeMemSeg){
    void *ptr=malloc(i+(8*2));
    if(ptr!=NULL){
      free(ptr); ptr=NULL;
      return(i);
    }
  }
  
  // 自動メモリ解放を使って試みる。
  void *ptr=safemalloc(&MM_Temp,FreeMemSeg);
  if(ptr!=NULL){
    safefree(&MM_Temp,ptr); ptr=NULL;
    return(FreeMemSeg);
  }
  
  return(0);
}

