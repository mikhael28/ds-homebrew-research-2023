#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <dirent.h>
#include <sys/dir.h>
#include "fb.h"
#include "draw.h"
#include "file.h"
#include "main.h"
#include "iv.h"
#include "tv.h"

#include "nds_loader_arm9.h"

files file[512];
scrollbars scrollbar;

u32 totalfiles=0;
char curdir[MAXPATHLEN];
s32 curpos=0;
u16* folder=NULL;
u16* nds=NULL;
u16* img=NULL;
u16* txt=NULL;
u16* unknown=NULL;
u16* back=NULL;
s16 file_selected=-1;
const u8 posX[6]={16, 96, 16, 96, 16, 96};
const u8 posY[6]={0, 0, 64, 64, 128, 128};
extern touchPosition touch, touch2;
extern bool dblclick;

#define StylusInZone(x1, y1, x2, y2) ((touch.px>=x1)&&(touch.py>=y1)&&(touch.px<x2)&&(touch.py<y2))

int FB_ID;

bool FB_GoToMenu(){
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	if(down & KEY_START)return 1;
	return 0;
}

void FB_Init(){
	FB_ID = InitApp(FB_GoToMenu, FB_Act, "File Browser", "/DsOS/images/fb.png");
	sprintf(curdir, "/");
	int width, height;
	folder = LoadPNG("/DsOS/images/folder.png", &width, &height);
	nds = LoadPNG("/DsOS/images/NDS.png", &width, &height);
	img = LoadPNG("/DsOS/images/image.png", &width, &height);
	txt = LoadPNG("/DsOS/images/txt.png", &width, &height);
	unknown = LoadPNG("/DsOS/images/unknown.png", &width, &height);
	back = LoadPNG("/DsOS/images/back.png", &width, &height);
	for(int i=0;i<512;i++){
	file[i].filename = NULL;
	file[i].fileext = NULL;
	file[i].isdir=0;
	file[i].size=0;
	file[i].atime=0;
	file[i].mtime=0;
	file[i].ctime=0;
	}
	FB_ListFiles();
	scrollbar.color = ARGB16(1, 31, 31, 31);
}

void FB_ListFiles(){
	for(int i=0;i<512;i++){
	if(file[i].filename!=NULL)free(file[i].filename);
	file[i].filename = (char*)malloc(MAXPATHLEN);
	if(file[i].fileext!=NULL)free(file[i].fileext);
	file[i].fileext = (char*)malloc(32);
	file[i].isdir=0;
	file[i].size=0;
	file[i].atime=0;
	file[i].mtime=0;
	file[i].ctime=0;
	}
	int i=0;
	/*DIR *pdir;
	struct dirent *pent;
	struct stat statbuf;
	pdir=opendir(curdir);
	char path[256];
	while ((pent=readdir(pdir))!=NULL) {
	sprintf(path, "%s%s", curdir, pent->d_name);
	stat(path, &statbuf);
	if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)continue;
	if(statbuf.st_mode & S_IFDIR){
	sprintf(file[i].filename, "%s", pent->d_name);
	file[i].isdir = 1;
	}
	if(!(statbuf.st_mode & S_IFDIR)){
	sprintf(file[i].filename, "%s", pent->d_name);
	sprintf(file[i].fileext, "%s", getExt(pent->d_name));
	file[i].isdir = 0;
	file[i].size = statbuf.st_size;
	file[i].atime = statbuf.st_atime;
	file[i].mtime = statbuf.st_mtime;
	file[i].ctime = statbuf.st_ctime;
	}
	i++;
	}
	closedir(pdir);*/
	struct stat statbuf;
	char filename[MAXPATHLEN]; // always guaranteed to be enough to hold a filename
	DIR_ITER* dir;
	dir = diropen (curdir);
	while (dirnext(dir, filename, &statbuf) == 0) {
	if(strcmp(".", filename) == 0 || strcmp("..", filename) == 0)continue;
	if(statbuf.st_mode & S_IFDIR){
	sprintf(file[i].filename, "%s", filename);
	file[i].isdir = 1;
	}
	if(!(statbuf.st_mode & S_IFDIR)){
	sprintf(file[i].filename, "%s", filename);
	sprintf(file[i].fileext, "%s", getExt(filename));
	file[i].isdir = 0;
	file[i].size = statbuf.st_size;
	file[i].atime = statbuf.st_atime;
	file[i].mtime = statbuf.st_mtime;
	file[i].ctime = statbuf.st_ctime;
	}
	i++;
	}
	dirclose(dir);
	totalfiles=i;
	scrollbar.pos=0;
	scrollbar.aggfiles = totalfiles%6;
	//if(scrollbar.aggfiles>0)scrollbar.aggfiles=scrollbar.aggfiles%2;
	scrollbar.height=(160/((totalfiles+scrollbar.aggfiles)/2));
	if(scrollbar.height<16)scrollbar.height=16;
	if(totalfiles<=6)scrollbar.height=160;
	files temp;
	char tempname[2][256];
	bool redo=0;
	do{
	redo=0;
	for(int i=0;i<totalfiles-1;i++){
	strcpy(tempname[0], file[i].filename);
	strcpy(tempname[1], file[i+1].filename);
	strlwr(tempname[0]);
	strlwr(tempname[1]);
	if(strcmp(tempname[0], tempname[1])>0){
	temp = file[i];
	file[i] = file[i+1];
	file[i+1] = temp;
	redo=1;
	}
	}
	}while(redo);
	
	
}

void FB_PrintFilename(bool screen, s16 x1, s16 y1, u16 color, const char* filename){
	int len = strlen(filename);
	s16 x=0, y=0;
	s16 x2=0;
	if(len<8)x=(64-(len*8))/2;
	else if(len<16)x2=(64-((len-8)*8))/2;
	for(int i=0;i<len&&i<16;i++){
	PrintChar(screen, x+x1, y+y1, filename[i], color);
	x+=8;
	if(i==7){
	x=x2;
	y+=8;
	}
	}
}

void FB_ShowScrollbar(){
	DrawLine(0, 176, 0, 176, 191, ARGB16(1, 0, 0, 0));
	DrawLine(0, 191, 0, 191, 191, ARGB16(1, 0, 0, 0));
	DrawLine(0, 176, 15, 191, 15, ARGB16(1, 0, 0, 0));
	DrawLine(0, 176, 176, 191, 176, ARGB16(1, 0, 0, 0));
	for(int x=177;x<191;x++)for(int y=scrollbar.pos+16;y<scrollbar.pos+scrollbar.height+16;y++)PutPixelRGB(0, x, y, scrollbar.color);
}

void FB_ShowFiles(){
	for(s32 i=curpos;i<curpos+6&&i<totalfiles;i++){
	if(file[i].isdir==1){
	DrawImage(0, posX[i-curpos], posY[i-curpos], folder, 32, 32);
	}else if(strcmp(file[i].fileext, "nds")==0){
	DrawImage(0, posX[i-curpos], posY[i-curpos], nds, 32, 32);
	}else if(strcmp(file[i].fileext, "jpg")==0){
	DrawImage(0, posX[i-curpos], posY[i-curpos], img, 32, 32);
	}else if(strcmp(file[i].fileext, "jpeg")==0){
	DrawImage(0, posX[i-curpos], posY[i-curpos], img, 32, 32);
	}else if(strcmp(file[i].fileext, "png")==0){
	DrawImage(0, posX[i-curpos], posY[i-curpos], img, 32, 32);
	}else if(strcmp(file[i].fileext, "bmp")==0){
	DrawImage(0, posX[i-curpos], posY[i-curpos], img, 32, 32);
	}else if(strcmp(file[i].fileext, "txt")==0){
	DrawImage(0, posX[i-curpos], posY[i-curpos], txt, 32, 32);
	}else{
	DrawImage(0, posX[i-curpos], posY[i-curpos], unknown, 32, 32);
	}
	FB_PrintFilename(0, posX[i-curpos]-16, posY[i-curpos]+40, ARGB16(1, 0, 0, 0), file[i].filename);
	}
	if(strcmp(curdir, "/")!=0){
	DrawImage(0, 224, 160, back, 32, 32);
	}
}

void FB_MoveScrollbar(){
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	static s16 starty;
	static u8 moving = 0;
	if((down & KEY_TOUCH) && touch.px>=177 && touch.px<192 && touch.py>=(scrollbar.pos+16) && touch.py<(scrollbar.pos+16+scrollbar.height)){
	starty = touch.py-scrollbar.pos;
	moving = 1;
	}
	if(moving && (held & KEY_TOUCH)){
	scrollbar.pos = touch.py-starty;
	}
	if(moving && (up & KEY_TOUCH)){
	scrollbar.pos = touch.py-starty;
	moving = 0;
	}
	if(scrollbar.pos<0)scrollbar.pos=0;
	if(scrollbar.pos+scrollbar.height>160)scrollbar.pos=160-scrollbar.height;
	curpos = 2*(scrollbar.pos*((totalfiles+scrollbar.aggfiles)/2)/(160-scrollbar.height));
}

void FB_GoBackDir(){
	if(strcmp(curdir, "/")!=0){
	int slashpos = 0;
	int len = strlen(curdir);
	for(int i=len-2;i>=0;i--){
	if(curdir[i]=='/'){
	slashpos = i+1;
	break;
	}
	}
	for(int i=slashpos;i<len;i++){
	curdir[i]=' ';
	}
	curdir[slashpos]='\0';
	FB_ListFiles();
	}
}



void FB_Act(){
	memmove(fb0, sfondo, 256*192*2);
	memmove(fb1, sfondo, 256*192*2);
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	char path[256];
	static u32 ur=0, ug=0, ub=0;
	if(down & KEY_TOUCH && StylusInZone(224, 160, 256, 192)){
	FB_GoBackDir();
	}
	FB_MoveScrollbar();
	FB_ShowFiles();
	FB_ShowScrollbar();
	if(!StylusInZone(0, 0, 144, 192)){
	file_selected = -1;
	}else{
	for(int i=0;i<6;i++){
	if((down & KEY_TOUCH) && StylusInZone(posX[i]-16, posY[i], posX[i]+48, posY[i]+64) && (file_selected!=i+curpos)){
	file_selected = i+curpos;
	ur=0;
	ug=0;
	ub=0;
	}else if((down & KEY_TOUCH) && StylusInZone(posX[i]-16, posY[i], posX[i]+48, posY[i]+64) && (file_selected==i+curpos)){
	if(file[i+curpos].isdir){
	sprintf(curdir, "%s%s/", curdir, file[i+curpos].filename);
	FB_ListFiles();
	FB_MoveScrollbar();
	FB_ShowFiles();
	FB_ShowScrollbar();
	file_selected=-1;
	}else if(strcmp(file[i+curpos].fileext, "nds")==0){
	char* arg[1];
	sprintf(arg[0], "%s%s", curdir, file[i+curpos].filename);
	runNdsFile (arg[0], 1, (const char**)arg);
	}else if(strcmp(file[i+curpos].fileext, "png")==0){
	sprintf(path, "%s%s", curdir, file[i+curpos].filename);
	IV_OpenImage(path);
	SetCurApp(IV_ID);
	}else if(strcmp(file[i+curpos].fileext, "bmp")==0){
	sprintf(path, "%s%s", curdir, file[i+curpos].filename);
	IV_OpenImage(path);
	SetCurApp(IV_ID);
	}else if(strcmp(file[i+curpos].fileext, "jpg")==0){
	sprintf(path, "%s%s", curdir, file[i+curpos].filename);
	IV_OpenImage(path);
	SetCurApp(IV_ID);
	}else if(strcmp(file[i+curpos].fileext, "jpeg")==0){
	sprintf(path, "%s%s", curdir, file[i+curpos].filename);
	IV_OpenImage(path);
	SetCurApp(IV_ID);
	}else if(strcmp(file[i+curpos].fileext, "txt")==0){
	sprintf(path, "%s%s", curdir, file[i+curpos].filename);
	TV_OpenText(path);
	SetCurApp(TV_ID);
	}
	}
	}
	}
	if(file_selected!=-1 && file_selected<totalfiles){
	for(int x=posX[file_selected-curpos]-16;x<posX[file_selected-curpos]+48;x++){
	for(int y=posY[file_selected-curpos];y<posY[file_selected-curpos]+64;y++){
	u8 r = GetPixelR(0, x, y);
	u8 g = GetPixelG(0, x, y);
	u8 b = GetPixelB(0, x, y);
	r = (r+(ur>>9))/2;
	g = (g+(ug>>9))/2;
	b = (b+(ub>>9))/2;
	if(ur<(31<<9))ur++;
	if(ug<(31<<9))ug++;
	if(ub<(31<<9))ub++;
	PutPixelRGB(0, x, y, ARGB16(1, r, g, b));
	}
	}
	Print(1, 0, 0, ARGB16(1, 0, 0, 0), "Filename:\n\t%s\nFile type:\n\t%s\nfilesize:\n\t%d %s", file[file_selected].filename, file[file_selected].isdir?"directory" : file[file_selected].fileext, (file[file_selected].size>=1024)? file[file_selected].size/1024 : file[file_selected].size, (file[file_selected].size>=1024)?"KBytes" : "Bytes");
	}
}
