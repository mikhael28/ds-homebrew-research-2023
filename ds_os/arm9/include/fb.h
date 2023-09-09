#ifndef _FB_H_
#define _FB_H_

typedef struct{
char* filename;
char* fileext;
bool isdir;
u32 size;
time_t atime; //accesso
time_t mtime; //modifica
time_t ctime; //modifica inode
}files;

typedef struct{
s16 pos;
u8 height;
u16 color;
u8 aggfiles;
}scrollbars;

extern int FB_ID;

void FB_Init();
void FB_ListFiles();
void FB_ShowFiles();
void FB_ShowScrollbar();
void FB_MoveScrollbar();
void FB_Act();
#endif