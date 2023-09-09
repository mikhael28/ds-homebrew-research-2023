#pragma once


extern bool BMPReader_Start(FAT_FILE *_FileHandle,bool ReadHeaderOnly);
extern void BMPReader_Free(void);
extern void BMPReader_GetBitmap32(u32 LineY,u32 *pBM);
extern void BMPReader_GetBitmap15(u32 LineY,u16 *pBM);
extern s32 BMPReader_GetWidth(void);
extern s32 BMPReader_GetHeight(void);

