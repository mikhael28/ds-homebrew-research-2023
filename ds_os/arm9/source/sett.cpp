#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "draw.h"
#include "fb.h"

extern touchPosition touch, touch2;
extern bool dblclick;
extern scrollbars scrollbar;

#define StylusInZone(x1, y1, x2, y2) ((touch.px>=x1)&&(touch.py>=y1)&&(touch.px<x2)&&(touch.py<y2))
void SETT_Act(){
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	static u8 r=31, g=31, b=31;
	static u8 modr=0, modg=0, modb=0;
	if(down & KEY_TOUCH){
	if(StylusInZone(1, 17, 33, 23)){
	modr=1;
	}else if(StylusInZone(1, 33, 33, 39)){
	modg=1;
	}else if(StylusInZone(1, 29, 33, 55)){
	modb=1;
	}
	}
	if(held & KEY_TOUCH){
	if(modr==1)r = touch.px-1;
	else if(modb==1)b = touch.px-1;
	else if(modg==1)g = touch.px-1;
	}
	if(up & KEY_TOUCH){
	modr=0;
	modg=0;
	modb=0;
	}
	if(r<0)r=0;
	if(r>31)r=31;
	if(b<0)b=0;
	if(b>31)b=31;
	if(g<0)g=0;
	if(g>31)g=31;
	Print(0, 0, 0, ARGB16(1, 0, 0, 0), "Here you can set scrollbar RGB\nR:\n\nG:\n\nB:\n\nColor: ");
	DrawRect(0, 0, 16, 33, 24, ARGB16(1, 31, 31, 31));
	Print(0, 33, 16, ARGB16(1, 0, 0, 0), "%d/31", r);
	for(int x=1;x<r+1;x++){
	for(int y=17;y<23;y++){
	PutPixelRGB(0, x, y, ARGB16(1, 31, 0, 0));
	}
	}
	DrawRect(0, 0, 32, 33, 40, ARGB16(1, 31, 31, 31));
	Print(0, 33, 32, ARGB16(1, 0, 0, 0), "%d/31", g);
	for(int x=1;x<g+1;x++){
	for(int y=33;y<39;y++){
	PutPixelRGB(0, x, y, ARGB16(1, 0, 31, 0));
	}
	}
	DrawRect(0, 0, 48, 33, 56, ARGB16(1, 31, 31, 31));
	Print(0, 33, 48, ARGB16(1, 0, 0, 0), "%d/31", b);
	for(int x=1;x<b+1;x++){
	for(int y=49;y<55;y++){
	PutPixelRGB(0, x, y, ARGB16(1, 0, 0, 31));
	}
	}
	DrawRect(0, 0, 64, 16, 80, ARGB16(1, 31, 31, 31));
	for(int x=1;x<15;x++){
	for(int y=65;y<79;y++){
	PutPixelRGB(0, x, y, ARGB16(1, r, g, b));
	}
	}
	scrollbar.color = ARGB16(1, r, g, b);
	
}