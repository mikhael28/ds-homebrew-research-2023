
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "shell.h"

#include "lang.h"

typedef struct {
  char *pItem;
  u32 ItemHash;
  char *pValue;
} TLangData;

#define LangDataMaxCount (1024)
static u32 LangDataCount;
static TLangData *pLangData;

#include "lang_TextPool.h"

// ---------------------------------------------------------

static u32 CalcItemHash(const char *pItem)
{
  u32 hash=0;
  while(*pItem!=0){
    hash+=*pItem++;
  }
  return(hash);
}

// ---------------------------------------------------------

static bool isStrEqual(const char *s1,const char *s2)
{
  if((s1==0)&&(s2==0)) return(true);
  if((s1==0)||(s2==0)) return(false);
  
  while(1){
    char c1=*s1++;
    char c2=*s2++;
    
    if((c1==0)||(c2==0)){
      if((c1==0)&&(c2==0)){
        return(true);
        }else{
        return(false);
      }
    }
    
    if(c1!=c2) return(false);
  }
  
  return(false);
}

// ---------------------------------------------------------

void Lang_Load(void)
{
  FAT_FILE *pf=Shell_FAT_fopen_Language_messages();
  if(pf==NULL) StopFatalError(13201,"Not found language file.\n");
  
  u32 bufsize=FAT2_GetFileSize(pf);
  u8 *pbuf=(u8*)safemalloc_chkmem(&MM_Temp,bufsize);
  
  FAT2_fread(pbuf,1,bufsize,pf);
  FAT2_fclose(pf);
  
  LangDataCount=0;
  pLangData=(TLangData*)safemalloc_chkmem(&MM_System,sizeof(TLangData)*LangDataMaxCount);
  
  InitTextPool();
  
  char linebuf[512+1];
  u32 linelen=0;
  linebuf[linelen]=0;
  
  s32 ofs=0;
  
  while(ofs<bufsize){
    char ch=pbuf[ofs++];
    if(ch==0) break;
    
    if(ch<0x20){
      if((ch==0x0d)||(ch==0x0a)){
        linebuf[linelen]=0;
        
        if((linelen!=0)&&(linebuf[0]!=';')&&(linebuf[0]!='#')){
          u32 eqofs=0;
          for(u32 idx=0;idx<linelen;idx++){
            if(linebuf[idx]=='='){
              eqofs=idx;
              break;
            }
          }
          if(eqofs!=0){
            linebuf[eqofs]=0;
            
            if(LangDataCount==LangDataMaxCount) StopFatalError(13202,"Language data buffer overflow.\n");
            
            TLangData *pld=&pLangData[LangDataCount];
            pld->pItem=TextPoolChar_AllocateCopy(&linebuf[0]);
            pld->ItemHash=CalcItemHash(pld->pItem);
            pld->pValue=TextPoolChar_AllocateCopy(&linebuf[eqofs+1]);
//            _consolePrintf("%s,%s,%x\n",pld->pItem,pld->pValue,pld->ItemHash);
            LangDataCount++;
          }
        }
        
        linelen=0;
      }
      }else{
      if((linelen+1)<512) linebuf[linelen++]=ch;
    }
  }
  
  EndTextPool();
  
  pLangData=(TLangData*)saferealloc(&MM_System,pLangData,sizeof(TLangData)*LangDataCount);
  
  if(pbuf!=NULL){
    safefree(&MM_Temp,pbuf); pbuf=NULL;
  }
  
  _consolePrintf("Language materials count= %d\n",LangDataCount);
}

void Lang_Free(void)
{
  if(pLangData!=NULL){
    for(u32 idx=0;idx<LangDataCount;idx++){
      TLangData *pld=&pLangData[idx];
      pld->pItem=NULL;
      pld->ItemHash=0;
      pld->pValue=NULL;
    }
    safefree(&MM_System,pLangData); pLangData=NULL;
  }
  
  LangDataCount=0;
  
  FreeTextPool();
}

const char* Lang_GetUTF8_internal(const char *pItemName,const char *pErrorMsg)
{
  if(LangDataCount==0) StopFatalError(13203,"Lang_GetUTF8: Language file not loaded.\n");
  
  u32 hash=CalcItemHash(pItemName);
  
  for(u32 idx=0;idx<LangDataCount;idx++){
    TLangData *pld=&pLangData[idx];
    if(hash==pld->ItemHash){
      if(isStrEqual(pItemName,pld->pItem)==true) return(pld->pValue);
    }
  }
  
  _consolePrintf("Lang_GetUTF8: Can not found language item. [%s]\n",pItemName);
  return(pErrorMsg);
}

// --------------------

#include "strtool.h"

void Lang_GetENJPSplit(char *pdst,const char *pstr)
{
  u32 pos=(u32)-1;
  u32 idx=0;
  while(pstr[idx]!=0){
    if(pstr[idx]=='|') pos=idx;
    idx++;
  }
  if(pos==(u32)-1) StopFatalError(0,"Not found sepalator.");
  
  if(Shell_isJPNmode()==false){
    StrCopy(pstr,pdst);
    pdst[pos]=0;
    }else{
    StrCopy(&pstr[pos+1],pdst);
  }
}

