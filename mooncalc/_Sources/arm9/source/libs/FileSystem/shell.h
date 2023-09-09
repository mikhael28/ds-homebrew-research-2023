
#pragma once

#include "unicode.h"
#include "fat2.h"
#include "memtool.h"

#define MaxFilenameLength (256)

#define DefaultDataPath "/MOONCALC"

#define DefaultRootPath DefaultDataPath

#define DefaultInternalPath DefaultDataPath "/INTERNAL"

#define INIFilename "MOONCALC.INI"

#define DataFilename "MOONCALC.DAT"
#define LogFilename "LOGBUF.TXT"
#define LanguageFilename "LANGUAGE.INI"

#define DLDIBodyBinFilename "DLDIBODY.BIN"

#define SndEffDatFilename "SNDEFF.DAT"

#define SkinPackFilename "SKINPACK.DAT"

// ------------------------------------------------------

bool Shell_FAT_fopen_isExists_Root(const char *fn);
FAT_FILE* Shell_FAT_fopen_Root(const char *fn);
FAT_FILE* Shell_FAT_fopen_Root_WithCheckExists(const char *fn);
FAT_FILE* Shell_FAT_fopenwrite_Root(const char *fn);

bool Shell_FAT_fopen_isExists_Internal(const char *fn);
FAT_FILE* Shell_FAT_fopen_Internal(const char *fn);
FAT_FILE* Shell_FAT_fopen_Internal_WithCheckExists(const char *fn);
FAT_FILE* Shell_FAT_fopenwrite_Internal(const char *fn);

FAT_FILE* Shell_FAT_fopen_FullPath(const UnicodeChar *pFullPathUnicode);
FAT_FILE* Shell_FAT_fopen_Split(const UnicodeChar *pFilePathUnicode,const UnicodeChar *pFileNameUnicode);

// ------------------------------------------------------

void Shell_FAT_fopen_LanguageInit(void);
bool Shell_isJPNmode(void);
void Shell_Set_isJPNmode(bool _isJPNmode);
const char *Shell_GetCodePageStr(void);
FAT_FILE* Shell_FAT_fopen_Language_chrglyph(void);
FAT_FILE* Shell_FAT_fopen_Language_messages(void);

// ------------------------------------------------------

bool Shell_FAT_ReadAlloc(TMM *pMM,const char *fn,void **pbuf,s32 *psize);

// ------------------------------------------------------

extern const char* ConvertFull_Unicode2Alias(const UnicodeChar *pBasePathUnicode,const UnicodeChar *pFilenameUnicode);
extern const char* ConvertPath_Unicode2Alias_CheckExists(const UnicodeChar *pBasePathUnicode);
extern const char* ConvertFullPath_Unicode2Alias(const UnicodeChar *pFullPathUnicode);
extern const char* ConvertFullPath_Ansi2Alias(const char *pFullPathAnsi);
extern bool forPLS_ConvertFull_Unicode2Alias(const UnicodeChar *pBasePathUnicode,const UnicodeChar *pFilenameUnicode,char *pPathAlias,char *pFilenameAlias);

// ------------------------------------------------------

extern bool FileExistsUnicode(const UnicodeChar *pBasePathUnicode,const UnicodeChar *pFilenameUnicode);
extern bool PathExistsUnicode(const UnicodeChar *pBasePathUnicode);

extern bool FullPath_FileExistsUnicode(const UnicodeChar *pFullPathUnicode);
extern bool FullPath_FileExistsAnsi(const char *pFullPathAnsi);

extern void SplitItemFromFullPathAlias(const char *pFullPathAlias,char *pPathAlias,char *pFilenameAlias);
extern void SplitItemFromFullPathUnicode(const UnicodeChar *pFullPathUnicode,UnicodeChar *pPathUnicode,UnicodeChar *pFilenameUnicode);

extern const UnicodeChar* FullPathUnicodeFromSplitItem(const UnicodeChar *pBasePathUnicode,const UnicodeChar *pFilenameUnicode);

// ------------------------------------------------------

const char* Shell_CreateNewFileUnicode(const UnicodeChar *pFilePathUnicode,const UnicodeChar *pFileNameUnicode);

// ------------------------------------------------------

extern bool isSwapFilenameUnicode_isEqual;
extern bool isSwapFilenameUnicode(const UnicodeChar *puc0,const UnicodeChar *puc1);

// ------------------------------------------------------

