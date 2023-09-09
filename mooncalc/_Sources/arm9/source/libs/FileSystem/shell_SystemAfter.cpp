
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_const.h"
#include "_console.h"
#include "shell.h"
#include "strtool.h"
#include "procstate.h"
#include "unicode.h"

#include "memtool.h"
#include "arm9tcm.h"

#include "lang.h"

extern char Shell_FAT_fopen_fullfn[MaxFilenameLength+1];

// ----------------------------------------

FAT_FILE* Shell_FAT_fopen_FullPath(const UnicodeChar *pFullPathUnicode)
{
  const char *pfullalias=ConvertFullPath_Unicode2Alias(pFullPathUnicode);
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopen_FullPath=%s\n",pfullalias);
  return(FAT2_fopen_AliasForRead(pfullalias));
}

FAT_FILE* Shell_FAT_fopen_Split(const UnicodeChar *pFilePathUnicode,const UnicodeChar *pFileNameUnicode)
{
  const char *pfullalias=ConvertFull_Unicode2Alias(pFilePathUnicode,pFileNameUnicode);
  if(str_isEmpty(pfullalias)==true) StopFatalError(13619,"Not found base file for split read.\n");
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopen_Split=%s\n",pfullalias);
  return(FAT2_fopen_AliasForRead(pfullalias));
}

// ------------------------------------------------

#define DefaultLanguageSetFullPathFilename "/MOONSHL2/LANGUAGE.SET"
#define DefaultLanguageDataPath DefaultDataPath "/LANGUAGE"

static char CodePageStr[4]={0,0,0,0};
static bool isJPNmode;

void Shell_FAT_fopen_LanguageInit(void)
{
  FAT_FILE *pf=FAT2_fopen_AliasForRead(DefaultLanguageSetFullPathFilename);
  
  if(pf==NULL){
    StrCopy("932",CodePageStr);
    }else{
    FAT2_fread(CodePageStr,1,3,pf);
    CodePageStr[3]=0;
    FAT2_fclose(pf);
  }
  
  _consolePrintf("Setup default code page is '%s'.\n",CodePageStr);
  
  isJPNmode=false;
  if((strcmp("932",CodePageStr)==0)||(strcmp("933",CodePageStr)==0)) isJPNmode=true;

  if(isJPNmode==false){
    CodePageStr[0]='0';
    CodePageStr[1]='0';
    CodePageStr[2]='0';
    CodePageStr[3]=0;
  }
}

bool Shell_isJPNmode(void)
{
  return(isJPNmode);
}

void Shell_Set_isJPNmode(bool _isJPNmode)
{
  isJPNmode=_isJPNmode;
}

const char *Shell_GetCodePageStr(void)
{
  return(CodePageStr);
}

FAT_FILE* Shell_FAT_fopen_Language_chrglyph(void)
{
  if(CodePageStr[0]=='0'){
    snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultLanguageDataPath "/chrglyph.000");
    }else{
    snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultLanguageDataPath "/chrglyph.%s",CodePageStr);
  }
  _consolePrintf("%s\n",Shell_FAT_fopen_fullfn);
  const char *pfullalias=ConvertFullPath_Ansi2Alias(Shell_FAT_fopen_fullfn);
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopen_Language_chrglyph=%s\n",pfullalias);
  return(FAT2_fopen_AliasForRead(pfullalias));
}

FAT_FILE* Shell_FAT_fopen_Language_messages(void)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultLanguageDataPath "/messages.%s",CodePageStr);
  const char *pfullalias=ConvertFullPath_Ansi2Alias(Shell_FAT_fopen_fullfn);
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopen_Language_messages=%s\n",pfullalias);
  return(FAT2_fopen_AliasForRead(pfullalias));
}

// ----------------------------------------

#include "shell_CreateNewFile.h"

// ----------------------------------------

bool isSwapFilenameUnicode_isEqual;

bool isSwapFilenameUnicode(const UnicodeChar *puc0,const UnicodeChar *puc1)
{
  isSwapFilenameUnicode_isEqual=false;
  
  if(puc0==puc1){
    isSwapFilenameUnicode_isEqual=true;
    return(false);
  }
  
  while(1){
    u32 uc0=*puc0;
    u32 uc1=*puc1;
    if(((u32)'A'<=uc0)&&(uc0<=(u32)'Z')) uc0+=0x20;
    if(((u32)'A'<=uc1)&&(uc1<=(u32)'Z')) uc1+=0x20;
    
    if(uc0==uc1){
      if(uc0==0){
        isSwapFilenameUnicode_isEqual=true;
        return(false);
      }
      }else{
      // ファイル名長さチェック
      if(uc0==0) return(false);
      if(uc1==0) return(true);
      if(uc0==(u32)'.') return(false);
      if(uc1==(u32)'.') return(true);
      // 文字比較
      if(uc0<uc1){
        return(false);
        }else{
        return(true);
      }
    }
    
    puc0++; puc1++;
  }
}


// ------------------------------------------------------
