#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "iv.h"
#include "draw.h"
#include "file.h"
#include "main.h"
#include "fb.h"

extern touchPosition touch, touch2;
extern bool dblclick;

u16* IV_image=NULL;
int IV_width, IV_height;
bool IV_reset=0;
int IV_ID;

bool IV_GoToMenu(){
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	if(down & KEY_START)return 1;
	return 0;
}

void IV_Init(){
	IV_ID = InitApp(IV_GoToMenu, IV_Act, "Image Viewer", "/DsOS/images/IV.png");
}

void IV_OpenImage(const char* filename){
	if(IV_image!=NULL)free(IV_image);
	if(strcmp(getExt(filename), "png")==0){
	IV_image = LoadPNG(filename, &IV_width, &IV_height);
	}else if(strcmp(getExt(filename), "jpg")==0){
	IV_image = LoadJPEG(filename, &IV_width, &IV_height);
	}else if(strcmp(getExt(filename), "jpeg")==0){
	IV_image = LoadJPEG(filename, &IV_width, &IV_height);
	}else if(strcmp(getExt(filename), "bmp")==0){
	IV_image = LoadBMP(filename, &IV_width, &IV_height);
	}
	IV_reset=1;
}

void IV_Act(){
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	static s32 startx=0, starty=0;
	static s32 oldsx=0, oldsy=0;
	static s32 sx=0, sy=0;
	static u8 fullscreen=0;
	if(IV_image==NULL){
	memmove(fb0, sfondo, 256*192*2);
	memmove(fb1, sfondo, 256*192*2);
	Print(1, 0, 96, ARGB16(1, 0, 0, 0), "Load an image through the file browser");
	Print(0, 0, 96, ARGB16(1, 0, 0, 0), "Load an image through the file browser");
	}else{
	if(IV_reset==1){
	startx=0;
	starty=0;
	oldsx=0;
	oldsy=0;
	sx=0;
	oldsy=0;
	fullscreen=0;
	IV_reset=0;
	}
	memset(fb1, 255, 256*192*2);
	memset(fb0, 255, 256*192*2);
	Print(1, 0, 0, ARGB16(1, 0, 0, 0), "Details:\n\tWidth: %d\n\tHeight: %d\n\nIf your image is bigger than the DS screen, you can scroll it using the touch screen or you can scale it to fit in the DS screen by pressing Select.\n\nScaled: %s\n\nPress X to close the image to get some free space on RAM(the File Browser will be opened)", IV_width, IV_height, fullscreen?"Yes" : "No");
	if(!fullscreen){
	if(down & KEY_TOUCH){
	sx=touch.px;
	sy=touch.py;
	oldsx=sx;
	oldsy=sy;
	}else if(held & KEY_TOUCH){
	sx=touch.px;
	sy=touch.py;
	startx-=sx-oldsx;
	starty-=sy-oldsy;
	oldsx=sx;
	oldsy=sy;
	}
	}
	if(startx>=IV_width-256)startx=IV_width-256-1;
	if(starty>=IV_height-192)starty=IV_height-192-1;
	if(startx<0)startx=0;
	if(starty<0)starty=0;
	if(IV_width<=256){
	startx=0;
	}
	if(IV_height<=192){
	starty=0;
	}
	if(down & KEY_SELECT){
	if(IV_width>256&&IV_height>192)fullscreen^=1;
	}
	if(fullscreen){
	int wj=IV_width/256;
	int hj=IV_height/192;
	for(int x=0;x<IV_width;x+=wj){
	for(int y=0;y<IV_height;y+=hj){
	PutPixelRGB(0, x/wj, y/hj, IV_image[x+y*IV_width]);
	}
	}
	}else{
	for(int x=0;x<256&&x+startx<IV_width;x++){
	for(int y=0;y<192&&y+starty<IV_height;y++){
	if(IV_width>256&&IV_height>192)PutPixelRGB(0, x, y, IV_image[x+startx+(y+starty)*IV_width]);
	else if(IV_width>256){
	PutPixelRGB(0, x, y+96-(IV_height/2), IV_image[x+startx+(y+starty)*IV_width]);
	}else if(IV_height>192){
	PutPixelRGB(0, x+128-(IV_width/2), y, IV_image[x+startx+(y+starty)*IV_width]);
	}else{
	PutPixelRGB(0, x+128-(IV_width/2), y+96-(IV_height/2), IV_image[x+startx+(y+starty)*IV_width]);
	}
	}
	}
	}
	if(down & KEY_X){
	free(IV_image);
	IV_image=NULL;
	SetCurApp(FB_ID);
	}
	}
}
