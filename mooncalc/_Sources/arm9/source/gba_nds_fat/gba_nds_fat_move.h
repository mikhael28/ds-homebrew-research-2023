#pragma once

bool FAT_Move(const char *pSrcPath,const char *pDstPath)
{
  _consolePrintf("FAT_Move: Get source directory entry.\n");
  DIR_ENT SrcDirEntry=FAT_DirEntFromPath(pSrcPath);
  
  _consolePrintf("FAT_Move: Check source name.\n");
  if(SrcDirEntry.name[0]==FILE_FREE) StopFatalError(12701,"FAT_Move: Source is removed.\n");
  
  _consolePrintf("FAT_Move: Check source file attribute.\n");
  // Only delete directories if the directory is entry
  if((SrcDirEntry.attrib&ATTRIB_DIR)!=0) StopFatalError(12702,"FAT_Move: Can not move source folder.\n");
  
  _consolePrintf("FAT_Move: Get dist directory entry.\n");
  DIR_ENT DstDirEntry=FAT_DirEntFromPath(pDstPath);

  _consolePrintf("FAT_Move: Check dist name.\n");
  if(DstDirEntry.name[0]==FILE_FREE) StopFatalError(12703,"FAT_Move: Dist is removed.\n");
  
  _consolePrintf("FAT_Move: Check dist file attribute.\n");
  // Only delete directories if the directory is entry
  if((DstDirEntry.attrib&ATTRIB_DIR)!=0) StopFatalError(12704,"FAT_Move: Can not move dist folder.\n");
  
  if(DstDirEntry.fileSize!=0){
    _consolePrintf("FAT_Move: Clear dist cluster links.\n");
    FAT_ClearLinks (DstDirEntry.startCluster | (DstDirEntry.startClusterHigh << 16));
    DstDirEntry.startCluster=0;
    DstDirEntry.startClusterHigh=0;
    DstDirEntry.fileSize=0;
  }
  
  _consolePrintf("FAT_Move: Override alias name and ext.\n");
  for(u32 idx=0;idx<8;idx++){
    SrcDirEntry.name[idx]=DstDirEntry.name[idx];
  }
  for(u32 idx=0;idx<3;idx++){
    SrcDirEntry.ext[idx]=DstDirEntry.ext[idx];
  }
  
  _consolePrintf("FAT_Move: Write to dist directory entry.\n");
  disc_SystemReadSector ( (wrkDirCluster == FAT16_ROOT_DIR_CLUSTER ? filesysRootDir : FAT_ClustToSect(wrkDirCluster)) + wrkDirSector , globalBuffer);
  ((DIR_ENT*)globalBuffer)[wrkDirOffset]=SrcDirEntry;
  disc_SystemWriteSector ( (wrkDirCluster == FAT16_ROOT_DIR_CLUSTER ? filesysRootDir : FAT_ClustToSect(wrkDirCluster)) + wrkDirSector , globalBuffer);
    
  _consolePrintf("FAT_Move: Flush dist directory entry sectors in disc cache\n");
  disc_CacheFlush();
  
  // ---------------------------------
  
  _consolePrintf("FAT_Move: Get source LFN directory entry.\n");
  SrcDirEntry=FAT_DirEntFromPath_LFNTopDirEntry(pSrcPath);
  if(SrcDirEntry.name[0]==FILE_FREE){
    _consolePrintf("FAT_Move: Get source alias directory entry.\n");
    SrcDirEntry=FAT_DirEntFromPath(pSrcPath);
    if(SrcDirEntry.name[0]==FILE_FREE) StopFatalError(12705,"FAT_Move: Not found source directory entry.\n");
  }

  _consolePrintf("FAT_Move: Remove source LFN and alias directory entry.\n");
  while(SrcDirEntry.name[0]!=FILE_FREE){
    disc_SystemReadSector ( (wrkDirCluster == FAT16_ROOT_DIR_CLUSTER ? filesysRootDir : FAT_ClustToSect(wrkDirCluster)) + wrkDirSector , globalBuffer);
    ((DIR_ENT*)globalBuffer)[wrkDirOffset].name[0] = FILE_FREE;
    disc_SystemWriteSector ( (wrkDirCluster == FAT16_ROOT_DIR_CLUSTER ? filesysRootDir : FAT_ClustToSect(wrkDirCluster)) + wrkDirSector , globalBuffer);
    _consolePrintf("Removed 0x%02x, ",SrcDirEntry.attrib);
    if((SrcDirEntry.attrib & ATTRIB_LFN)!=ATTRIB_LFN){
      _consolePrintf("Alias %.8s.%.3s\n",SrcDirEntry.name,SrcDirEntry.ext);
      }else{
      _consolePrintf("LFN entry.\n");
    }
    if((SrcDirEntry.attrib & ATTRIB_LFN)!=ATTRIB_LFN) break;
    DIR_ENT *pDirEntry=FAT_GetAllDirEntry((u32)-1, 1, SEEK_CUR);
    SrcDirEntry=*pDirEntry;
  }
    
  _consolePrintf("FAT_Move: Flush source directory entry sectors in disc cache\n");
  disc_CacheFlush();
  
  return(true);
}
