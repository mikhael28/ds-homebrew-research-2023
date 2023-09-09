
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_const.h"
#include "__debug.h"

#include "inifile.h"
#include "strtool.h"
#include "memtool.h"
#include "shell.h"
#include "arm9tcm.h"

TGlobalINI GlobalINI;

void InitINI(void)
{
  {
    TiniSystem *ps=&GlobalINI.System;
    ps->DetailsDebugLog=false;
  }
  
  {
    TiniDiskAdapter *ps=&GlobalINI.DiskAdapter;
    ps->SlowDiskAccess=false;
  }
  
  {
    TiniKeyRepeat *ps=&GlobalINI.KeyRepeat;
    ps->DelayCount=40;
    ps->RateCount=10;
  }
  
}

static char section[128];
static u32 readline;

static inline void readsection(char *str)
{
  str++;
  
  u32 ofs;
  
  ofs=0;
  while(*str!=']'){
    if((128<=ofs)||(*str==0)) StopFatalError(0,"line%d error.\nThe section name doesn't end correctly.\n",readline);
    section[ofs]=*str;
    str++;
    ofs++;
  }
  section[ofs]=0;
}

static inline u16 GetColorCoord(const char *value)
{
  u32 v=0;
  
  while(1){
    char c=*value;
    if(c==0) break;
    
    bool use=false;
    
    if(('0'<=c)&&(c<='9')){
      use=true;
      v<<=4;
      v|=0x00+(c-'0');
    }
    if(('a'<=c)&&(c<='f')){
      use=true;
      v<<=4;
      v|=0x0a+(c-'a');
    }
    if(('A'<=c)&&(c<='F')){
      use=true;
      v<<=4;
      v|=0x0a+(c-'A');
    }
    
    if(use==false) break;
    
    value++;
  }
  
  u32 r,g,b;
  
  r=(v >> 16) & 0xff;
  g=(v >> 8) & 0xff;
  b=(v >> 0) & 0xff;
  
  return(RGB15(r/8,g/8,b/8) | BIT(15));
}

static inline void readkey(char *str)
{
  if(section[0]==0){
    _consolePrintf("line%d error.\nThere is a key ahead of the section name.\n",readline);
//    ShowLogHalt();
    return;
  }
  
  char key[128],value[128];
  
  u32 ofs;
  
  ofs=0;
  while(*str!='='){
    if((128<=ofs)||(*str==0)) StopFatalError(0,"line%d error.\nThe key name doesn't end correctly.\n",readline);
    key[ofs]=*str;
    str++;
    ofs++;
  }
  key[ofs]=0;
  
  str++;
  
  ofs=0;
  while(*str!=0){
    if(128<=ofs) StopFatalError(0,"line%d error.\nThe value doesn't end correctly.\n",readline);
    value[ofs]=*str;
    str++;
    ofs++;
  }
  value[ofs]=0;
  
  s32 ivalue=atoi(value);
  bool bvalue;
  
  if(ivalue==0){
    bvalue=false;
    }else{
    bvalue=true;
  }
  
  if(strcmp(section,"System")==0){
    TiniSystem *ps=&GlobalINI.System;
    
    if(strcmp(key,"DetailsDebugLog")==0){
      ps->DetailsDebugLog=bvalue;
      return;
    }
    
  }
  
  if(strcmp(section,"DiskAdapter")==0){
    TiniDiskAdapter *ps=&GlobalINI.DiskAdapter;
    
    if(strcmp(key,"SlowDiskAccess")==0){
      ps->SlowDiskAccess=bvalue;
      return;
    }
    
  }
  
  if(strcmp(section,"KeyRepeat")==0){
    TiniKeyRepeat *ps=&GlobalINI.KeyRepeat;
    
    if(strcmp(key,"DelayCount")==0){
      if(ivalue!=0) ps->DelayCount=ivalue;
      return;
    }
    if(strcmp(key,"RateCount")==0){
      if(ivalue!=0) ps->RateCount=ivalue;
      return;
    }
    
  }
  
  _consolePrintf("line%d error.\ncurrent section [%s] unknown key=%s value=%s\n",readline,section,key,value);
//  ShowLogHalt();
}

static inline void LoadINI_ins_loadbody(char *pini,u32 inisize)
{
  section[0]=0;
  readline=0;
  
  u32 iniofs=0;
  
  while(iniofs<inisize){
    
    readline++;
    
    u32 linelen=0;
    
    // Calc Line Length
    {
      char *s=&pini[iniofs];
      
      while(0x20<=*s){
        linelen++;
        s++;
        if(inisize<=(iniofs+linelen)) break;
      }
      *s=0;
    }
    
    if(linelen!=0){
      char c=pini[iniofs];
      if((c==';')||(c=='/')||(c=='!')){
        // comment line
        }else{
        if(c=='['){
          readsection(&pini[iniofs]);
          }else{
          readkey(&pini[iniofs]);
        }
      }
    }
    
    iniofs+=linelen;
    
    // skip NULL,CR,LF
    {
      char *s=&pini[iniofs];
      
      while(*s<0x20){
        iniofs++;
        s++;
        if(inisize<=iniofs) break;
      }
    }
    
  }
}

void LoadINI(void)
{
  InitINI();
  
  FAT_FILE *pf=Shell_FAT_fopen_Root_WithCheckExists(INIFilename);
  if(pf==NULL) return;
  
  u32 inisize=FAT2_GetFileSize(pf);
  char *pini=(char*)safemalloc_chkmem(&MM_Temp,inisize);
  
  FAT2_fread(pini,1,inisize,pf);
  
  FAT2_fclose(pf); pf=NULL;
  
  if(pini!=NULL){
    LoadINI_ins_loadbody(pini,inisize);
    safefree(&MM_Temp,pini); pini=NULL;
  }
  
#ifdef InternalDebugMode
  GlobalINI.System.DetailsDebugLog=true;
#endif

//  GlobalINI.System.Password=1;

//  GlobalINI.System.VRAMCacheEnabled=false;
}

