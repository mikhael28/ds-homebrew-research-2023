
static __attribute__ ((noinline)) FAT_FILE* openwrite(const char *pprefix)
{
  for(u32 idx=0;idx<100;idx++){
    char fn[64];
    snprintf(fn,64,"/%02d_%s.BMP",idx,pprefix);
    FAT_FILE *pf=FAT2_fopen_AliasForRead(fn);
    if(pf==NULL) return(FAT2_fopen_AliasForWrite(fn));
    FAT2_fclose(pf);
  }
  StopFatalError(18701,"can not open file for write.\n");
  return(NULL);
}

static __attribute__ ((noinline)) void main_SavePreviewAndHalt(void)
{
  {
    u32 size;
    u8 *pbmp=pScreenMain->pViewCanvas->CreateBMPImage(&size);
    if(pbmp!=NULL){
      FAT_FILE *pf=openwrite("PMAIN");
      if(pf!=NULL){
        FAT2_fwrite(pbmp,1,size,pf);
        FAT2_fclose(pf);
        _consolePrint("/00_pmain.bmp saved.\n");
      }
      safefree(&MM_Temp,pbmp); pbmp=NULL;
    }
  }
  {
    u32 size;
    u8 *pbmp=pScreenMainOverlay->pCanvas->CreateBMPImage(&size);
    if(pbmp!=NULL){
      FAT_FILE *pf=openwrite("POVER");
      if(pf!=NULL){
        FAT2_fwrite(pbmp,1,size,pf);
        FAT2_fclose(pf);
        _consolePrint("00_pover.bmp saved.\n");
      }
      safefree(&MM_Temp,pbmp); pbmp=NULL;
    }
  }
  {
    u32 size;
    u8 *pbmp=pScreenSub->pCanvas->CreateBMPImage(&size);
    if(pbmp!=NULL){
      FAT_FILE *pf=openwrite("PSUB");
      if(pf!=NULL){
        FAT2_fwrite(pbmp,1,size,pf);
        FAT2_fclose(pf);
        _consolePrint("00_psub.bmp saved.\n");
      }
      safefree(&MM_Temp,pbmp); pbmp=NULL;
    }
  }
}

#if 0

static inline void SaveSkinBG(void)
{
  for(u32 idx=0;idx<20;idx++){
    extern CglB15* getskinbg(u32 idx);
    CglB15 *pbg=getskinbg(idx);
    u32 size;
    pbg->pCanvas->BitBltFullBeta(pScreenMainOverlay->pCanvas);
    u8 *pbmp=pScreenMainOverlay->pCanvas->CreateBMPImage(&size);
    if(pbmp!=NULL){
      FAT_FILE *pf=openwrite("BGBMP");
      if(pf!=NULL){
        FAT2_fwrite(pbmp,1,size,pf);
        FAT2_fclose(pf);
        _consolePrint("??_BGBMP.BMP saved.\n");
      }
      safefree(&MM_Temp,pbmp); pbmp=NULL;
    }
  }
  StopFatalError((u32)-1,"BG bmp files saved.\n");
}

#endif
