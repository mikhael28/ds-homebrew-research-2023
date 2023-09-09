
// ------------------------------------------------------------------------------------

bool Shell_FAT_fopen_isExists_Root(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultRootPath "/%s",fn);
  return(FullPath_FileExistsAnsi(Shell_FAT_fopen_fullfn));
}

FAT_FILE* Shell_FAT_fopen_Root(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultRootPath "/%s",fn);
  const char *pfullalias=ConvertFullPath_Ansi2Alias(Shell_FAT_fopen_fullfn);
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopen_Root=%s\n",pfullalias);
  return(FAT2_fopen_AliasForRead(pfullalias));
}

FAT_FILE* Shell_FAT_fopen_Root_WithCheckExists(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultRootPath "/%s",fn);
  if(FullPath_FileExistsAnsi(Shell_FAT_fopen_fullfn)==false) return(NULL);
  return(Shell_FAT_fopen_Root(fn));
}

FAT_FILE* Shell_FAT_fopenwrite_Root(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultRootPath "/%s",fn);
  const char *pfullalias=ConvertFullPath_Ansi2Alias(Shell_FAT_fopen_fullfn);
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopenwrite_Root=%s\n",pfullalias);
  if(str_isEmpty(pfullalias)==true) StopFatalError(13616,"Not found base file for write.\n");
  return(FAT2_fopen_AliasForWrite(pfullalias));
}

// ------------------------------------------------------------------------------------


bool Shell_FAT_fopen_isExists_Internal(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultInternalPath "/%s",fn);
  return(FullPath_FileExistsAnsi(Shell_FAT_fopen_fullfn));
}

FAT_FILE* Shell_FAT_fopen_Internal(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultInternalPath "/%s",fn);
  const char *pfullalias=ConvertFullPath_Ansi2Alias(Shell_FAT_fopen_fullfn);
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopen_Internal=%s\n",pfullalias);
  return(FAT2_fopen_AliasForRead(pfullalias));
}

FAT_FILE* Shell_FAT_fopen_Internal_WithCheckExists(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultInternalPath "/%s",fn);
  if(FullPath_FileExistsAnsi(Shell_FAT_fopen_fullfn)==false) return(NULL);
  return(Shell_FAT_fopen_Internal(fn));
}

FAT_FILE* Shell_FAT_fopenwrite_Internal(const char *fn)
{
  snprintf(Shell_FAT_fopen_fullfn,MaxFilenameLength,DefaultInternalPath "/%s",fn);
  const char *pfullalias=ConvertFullPath_Ansi2Alias(Shell_FAT_fopen_fullfn);
  if(VerboseDebugLog==true) _consolePrintf("Shell_FAT_fopenwrite_Internal=%s\n",pfullalias);
  if(str_isEmpty(pfullalias)==true) StopFatalError(13616,"Not found base file for write.\n");
  return(FAT2_fopen_AliasForWrite(pfullalias));
}

// ------------------------------------------------------------------------------------

