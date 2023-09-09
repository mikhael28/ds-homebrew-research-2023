
static bool Shell_FAT_FullAlias_ReadFileAlloc(TMM *pMM,const char *pFullAlias,void **pbuf,s32 *psize)
{
  FAT_FILE *pf=FAT2_fopen_AliasForRead(pFullAlias);
  
  if(pf==NULL){
    _consolePrintf("Shell_FAT_ReadAlloc: (%s) File not found.\n",pFullAlias);
    return(false);
  }
  
  *psize=FAT2_GetFileSize(pf);
  
  if(*psize==0){
    _consolePrintf("Shell_FAT_ReadAlloc: (%s) File size == 0\n",pFullAlias);
    FAT2_fclose(pf);
    return(false);
  }
  
  *pbuf=safemalloc_chkmem(pMM,*psize);
  
  if(FAT2_fread(*pbuf,1,*psize,pf)!=*psize){
    safefree(pMM,pbuf);
    _consolePrintf("Shell_FAT_ReadAlloc: (%s) File size error.\n",pFullAlias);
    FAT2_fclose(pf);
    return(false);
  }
  
  FAT2_fclose(pf);
  
  return(true);
}

bool Shell_FAT_ReadAlloc(TMM *pMM,const char *fn,void **pbuf,s32 *psize)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultDataPath "/%s",fn);
  const char *pfullalias=ConvertFullPath_Ansi2Alias(Shell_FAT_fopen_fullfn);
  if(str_isEmpty(pfullalias)==true) return(false);
  return(Shell_FAT_FullAlias_ReadFileAlloc(pMM,pfullalias,pbuf,psize));
}

// ----------------------------------------

