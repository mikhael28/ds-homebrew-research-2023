
static const char SkinFileID[]="Skin files package for MoonShell2 type.5\0\0";

static bool SkinFile_Check(const UnicodeChar *pSkinFilenameUnicode)
{
  if(VerboseDebugLog==true) _consolePrint("SkinFile_Check(...);\n");
  
  if(Unicode_isEmpty(pSkinFilenameUnicode)==true) return(false);
  
  if(FullPath_FileExistsUnicode(pSkinFilenameUnicode)==false){
    _consolePrintf("Skin file not found.\n");
    return(false);
  }
  
  FAT_FILE *pSkinFile=Shell_FAT_fopen_FullPath(pSkinFilenameUnicode);
  
  if(pSkinFile==NULL){
    _consolePrintf("Open file failed.\n");
    return(false);
  }
  
  CStreamFS *pSkinFileStream=new CStreamFS(pSkinFile);

  if(pSkinFileStream==NULL){
    _consolePrintf("Can not create file stream.\n");
    FAT2_fclose(pSkinFile);
    return(false);
  }
  
  {
    u32 IDLength=pSkinFileStream->Readu8();
    if(64<IDLength) IDLength=64;
    char chkid[64+1];
    pSkinFileStream->ReadBuffer(chkid,IDLength+1);
    
    if(isStrEqual(SkinFileID,chkid)==false){
      _consolePrintf("Illigal ID. [%s]\n",chkid);
      _consolePrintf("Request ID. [%s]\n",SkinFileID);
      delete pSkinFileStream; pSkinFileStream=NULL;
      FAT2_fclose(pSkinFile);
      return(false);
    }
  }
  
  delete pSkinFileStream; pSkinFileStream=NULL;
  FAT2_fclose(pSkinFile);
  
  return(true);
}

typedef struct {
  u32 FilenameOffset;
  u32 DecompSize;
  u32 CompSize;
  u32 CompDataOffset;
} TSkinFileInfo;

typedef struct {
  void *pData;
  u32 DataSize;
} TSkinFileCache;

static FAT_FILE *pSkinFile=NULL;
static CStreamFS *pSkinFileStream=NULL;
static u32 SkinFileCount=0;
static TSkinFileInfo *pSkinFileInfo;
static TSkinFileCache *pSkinFileCache;

static bool SkinFile_Open(const UnicodeChar *pSkinFilenameUnicode)
{
  if(VerboseDebugLog==true) _consolePrint("SkinFile_Open(...);\n");
  
  if(Unicode_isEmpty(pSkinFilenameUnicode)==true) return(false);
  
  if(FullPath_FileExistsUnicode(pSkinFilenameUnicode)==false){
    _consolePrintf("Skin file not found.\n");
    return(false);
  }
  
  pSkinFile=Shell_FAT_fopen_FullPath(pSkinFilenameUnicode);
  
  if(pSkinFile==NULL){
    _consolePrintf("Open file failed.\n");
    return(false);
  }
  
  _consolePrintf("Skin file size: %dbyte.\n",FAT2_GetFileSize(pSkinFile));
  
  pSkinFileStream=new CStreamFS(pSkinFile);

  if(pSkinFileStream==NULL){
    _consolePrintf("Can not create file stream.\n");
    FAT2_fclose(pSkinFile);
    return(false);
  }
  
  {
    u32 IDLength=pSkinFileStream->Readu8();
    if(64<IDLength) IDLength=64;
    char chkid[64+1];
    pSkinFileStream->ReadBuffer(chkid,IDLength+1);
    
    if(isStrEqual(SkinFileID,chkid)==false){
      _consolePrintf("Illigal ID. [%s]\n",chkid);
      _consolePrintf("Request ID. [%s]\n",SkinFileID);
      delete pSkinFileStream; pSkinFileStream=NULL;
      FAT2_fclose(pSkinFile);
      return(false);
    }
  }
  
  SkinFileCount=pSkinFileStream->Readu32();
  
  u32 HeaderSize=pSkinFileStream->Readu32();
  if(VerboseDebugLog==true) _consolePrintf("Skin header size=0x%x\n",HeaderSize);
  
  u32 ItemMaxCount=256;
  u32 HeaderMaxSize=(ItemMaxCount*sizeof(TSkinFileInfo))+2048; // with filename buffer.
  if(HeaderMaxSize<HeaderSize) StopFatalError(14401,"Skin header size too long. %d,%d\n",HeaderSize,HeaderMaxSize);
  pSkinFileInfo=(TSkinFileInfo*)safemalloc_chkmem(&MM_SystemAfter,HeaderMaxSize);
  pSkinFileCache=(TSkinFileCache*)safemalloc_chkmem(&MM_SystemAfter,ItemMaxCount*sizeof(TSkinFileCache));
  pSkinFileStream->ReadBuffer(pSkinFileInfo,HeaderSize);
  MemSet32CPU(0,pSkinFileCache,ItemMaxCount*sizeof(TSkinFileCache));
  
  return(true);
}

static void SkinFile_Close(void)
{
  if(VerboseDebugLog==true) _consolePrint("SkinFile_Close();\n");
  
  if(pSkinFileStream!=NULL){
    delete pSkinFileStream; pSkinFileStream=NULL;
  }
  
  if(pSkinFile!=NULL){
    FAT2_fclose(pSkinFile); pSkinFile=NULL;
  }
  
  if(pSkinFileInfo!=NULL){
    safefree(&MM_SystemAfter,pSkinFileInfo); pSkinFileInfo=NULL;
  }
  if(pSkinFileCache!=NULL){
    safefree(&MM_SystemAfter,pSkinFileCache); pSkinFileCache=NULL;
  }
  
  SkinFileCount=0;
}

static u32 SkinFile_GetFileIndexFromFilename(const char *pFilename)
{
  u32 FilenameLength=strlen(pFilename);
  
  for(u32 idx=0;idx<SkinFileCount;idx++){
    TSkinFileInfo *psfi=&pSkinFileInfo[idx];
    
    char *pfn=(char*)pSkinFileInfo;
    pfn=&pfn[psfi->FilenameOffset];
    
    u32 fnlen=(u8)*pfn;
    pfn++;
    
    if(FilenameLength==fnlen){
      if(isStrEqual(pFilename,pfn)==true) return(idx);
    }
  }
  
  return((u32)-1);
}

static void SkinFile_LoadFileAllocate(u32 SkinFileIndex,void **pbuf,s32 *psize)
{
  TSkinFileInfo *psfi=&pSkinFileInfo[SkinFileIndex];

  void *pdummy=safemalloc_chkmem(&MM_Temp,psfi->DecompSize*2);
  
  if(0){
    u32 tmpbufsize=psfi->DecompSize;
    u8 *ptmpbuf=(u8*)safemalloc_chkmem(&MM_Temp,tmpbufsize);
    
    safefree(&MM_Temp,pdummy); pdummy=NULL;
    
    pSkinFileStream->SetOffset(psfi->CompDataOffset);  
    pSkinFileStream->ReadBuffer(ptmpbuf,tmpbufsize);
    
    *pbuf=ptmpbuf;
    *psize=tmpbufsize;
    return;
  }
  
  TZLIBData zd;
  
  zd.DstSize=psfi->DecompSize;
  zd.pDstBuf=(u8*)safemalloc_chkmem(&MM_Temp,zd.DstSize);
  zd.SrcSize=psfi->CompSize;
  zd.pSrcBuf=(u8*)safemalloc_chkmem(&MM_Temp,zd.SrcSize);
  
  safefree(&MM_Temp,pdummy); pdummy=NULL;
  
  pSkinFileStream->SetOffset(psfi->CompDataOffset);  
  pSkinFileStream->ReadBuffer(zd.pSrcBuf,zd.SrcSize);
  
  if((zd.pSrcBuf[0]==0x78)&&(zd.pSrcBuf[1]==0x01)){
    if(zlibdecompress(&zd)==false) StopFatalError(14404,"ZLIB decompress error.\n");
    }else{
    MemCopy32CPU(zd.pSrcBuf,zd.pDstBuf,zd.DstSize);
  }
  
  zd.SrcSize=0;
  safefree(&MM_Temp,zd.pSrcBuf); zd.pSrcBuf=NULL;
  
  *pbuf=zd.pDstBuf;
  *psize=zd.DstSize;
}

static void SkinFile_LoadB15(const char *pFilename,CglB15 **ppB15)
{
  if(VerboseDebugLog==true) _consolePrintf("Load skin %s.\n",pFilename);
  
  u32 fidx=SkinFile_GetFileIndexFromFilename(pFilename);
  
  if(fidx==(u32)-1) StopFatalError(14405,"Skin file B15 '%s' not found.\n",pFilename);
  
  TSkinFileCache *psfc=&pSkinFileCache[fidx];
  
  if(psfc->pData!=NULL){
    *ppB15=new CglB15(&MM_Skin,(u8*)psfc->pData,psfc->DataSize);
    return;
  }
  
  u8 *pbuf=NULL;
  s32 bufsize=0;
  
  SkinFile_LoadFileAllocate(fidx,(void**)&pbuf,&bufsize);
  *ppB15=new CglB15(&MM_Skin,pbuf,bufsize);
  
  psfc->pData=NULL;
  
  safefree(&MM_Temp,pbuf); pbuf=NULL;
}

static void SkinFile_LoadTGF(const char *pFilename,CglTGF **ppTGF)
{
  if(VerboseDebugLog==true) _consolePrintf("Load skin %s.\n",pFilename);
  
  u32 fidx=SkinFile_GetFileIndexFromFilename(pFilename);
  
  if(fidx==(u32)-1) StopFatalError(14406,"Skin file TGF '%s' not found.\n",pFilename);
  
  TSkinFileCache *psfc=&pSkinFileCache[fidx];
  
  if(psfc->pData!=NULL){
    if(VerboseDebugLog==true){
      u8 *p=(u8*)psfc->pData;
      u32 crc=0;
      for(u32 idx=0;idx<psfc->DataSize;idx++){
        crc+=p[idx];
      }
      _consolePrintf("from cache %x %x,%x,%x %x\n",psfc->pData,p[0],p[1],p[2],crc);
    }
    *ppTGF=new CglTGF(&MM_Skin,(u8*)psfc->pData,psfc->DataSize);
    return;
  }
  
  u8 *pbuf=NULL;
  s32 bufsize=0;
  
  SkinFile_LoadFileAllocate(fidx,(void**)&pbuf,&bufsize);
  *ppTGF=new CglTGF(&MM_Skin,pbuf,bufsize);
  
  psfc->pData=NULL;
  
  safefree(&MM_Temp,pbuf); pbuf=NULL;
}

