#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <fat.h>
#include <unistd.h> 
#include <malloc.h> 
#include <errno.h>
#include "draw.h"
#include "fb.h"
#include "file.h"
#include "sett.h"
#include "main.h"
#include "iv.h"
#include "tv.h"

#include "nds_loader_arm9.h"

#include "palibs_bin.h"
#include "neosplash_bin.h"

extern u8 __end__[];        // end of static code and data 
extern u8 __eheap_end[];    // farthest point to which the heap will grow 
u8* getHeapStart() { 
   return __end__; 
} 
u8* getHeapEnd() { 
   return (u8*)sbrk(0); 
} 
u8* getHeapLimit() { 
   return __eheap_end; 
} 
int getMemUsed() { 
   struct mallinfo mi = mallinfo(); 
   return mi.uordblks; 
} 
int getMemFree() { 
   struct mallinfo mi = mallinfo(); 
   return mi.fordblks + (getHeapLimit() - getHeapEnd()); 
}

touchPosition touch, touch2;
bool dblclick=0;
u8 ndita = 0;
#define StylusInZone(x1, y1, x2, y2) ((touch.px>=x1)&&(touch.py>=y1)&&(touch.px<x2)&&(touch.py<y2))

APP apps[10];
int napp=0;
int curapp=0;
u16* screen0=(u16*)malloc(256*192*2);
u16* screen1=(u16*)malloc(256*192*2);
int InitApp(gotomenu go, Actapp Act, const char* name, const char* icon){
	int width, height;
	apps[napp].name = (char*)malloc(32);
	sprintf(apps[napp].name, "%s", name);
	apps[napp].icon = LoadPNG(icon, &width, &height);
	apps[napp].go = go;
	apps[napp].Act = Act;
	napp++;
	return napp-1;
}

void PrintAppname(bool screen, s16 x1, s16 y1, u16 color, const char* name){
	int len = strlen(name);
	s16 x=0, y=0;
	s16 x2=0;
	if(len<8)x=(64-(len*8))/2;
	else if(len<16)x2=(64-((len-8)*8))/2;
	for(int i=0;i<len&&i<16;i++){
	PrintChar(screen, x+x1, y+y1, name[i], color);
	x+=8;
	if(i==7){
	x=x2;
	y+=8;
	}
	}
}

void HandleApps(){
	if(apps[curapp].go()){
	u8 r=0, g=0, b=0;
	u32 ur=0, ug=0, ub=0;
	for(int x=0;x<256;x++){
	for(int y=0;y<192;y++){
	r = GetPixelR(0, x, y);
	g = GetPixelG(0, x, y);
	b = GetPixelB(0, x, y);
	screen0[x+y*256] = ARGB16(1, (r)/2, (g)/2, (b)/2);
	}
	}
	for(int x=0;x<256;x++){
	for(int y=0;y<192;y++){
	r = GetPixelR(1, x, y);
	g = GetPixelG(1, x, y);
	b = GetPixelB(1, x, y);
	screen1[x+y*256] = ARGB16(1, (r)/2, (g)/2, (b)/2);
	}
	}
	for(int x=32;x<224;x++){
	for(int y=32;y<160;y++){
	screen0[x+y*256] = ARGB16(1, 25, 25, 25);
	}
	}
	bool exit=0;
	int app_selected=-1;
	while(!exit){
	touchRead(&touch2);
	scanKeys();
	int down = keysDown();
	int held = keysHeld();
	if(held & KEY_TOUCH)touch=touch2;
	if(down & KEY_START)exit=1;
	ResetDrawSystem();
	memmove(fb1, screen1, 256*192*2);
	memmove(fb0, screen0, 256*192*2);
	for(int i=0;i<napp;i++){
	DrawImage(0, 48+i*64, 32+(i/3)*64, apps[i].icon, 32, 32);
	PrintAppname(0, 32+i*64, 72+(i/3)*64, ARGB16(1, 0, 0, 0), apps[i].name);
	}
	for(int i=0;i<napp;i++){
	if((down & KEY_TOUCH) && StylusInZone(32+i*64, 32+(i/3)*64, 32+i*64+64, 32+(i/3)*64+64) && (app_selected!=i)){
	app_selected=i;
	ur=0;
	ug=0;
	ub=0;
	}else if((down & KEY_TOUCH) && StylusInZone(32+i*64, 32+(i/3)*64, 32+i*64+64, 32+(i/3)*64+64) && (app_selected==i)){
	curapp=i;
	exit=1;
	}
	}
	if(app_selected!=-1){
	for(int x=32+app_selected*64;x<32+app_selected*64+64;x++){
	for(int y=32+(app_selected/3)*64;y<32+(app_selected/3)*64+64;y++){
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
	}
	DrawSystemUpdate();
	swiWaitForVBlank();
	}
	}
}

void SetCurApp(int cur){
	curapp=cur;
}

int vblcount=0;
int count=0;
int FPS=0;

void Vblank(){
	if(vblcount>=60){
	FPS=count;
	vblcount=0;
	count=0;
	}
	vblcount++;
}

u16* sfondo;

//#define DEBUG

int main(void) {
	if(!fatInitDefault()){
	consoleDemoInit();
	printf("Error: can't init fat");
	while(1);
	}
	irqSet(IRQ_VBLANK, Vblank);
	irqEnable(IRQ_VBLANK);
	InitDrawSystem();
	scanKeys();
	bool exit=0;
	int exit_vbl=160;
	while(!exit){
	setBrightness(3, exit_vbl/5-16);
	if(keysDown() & KEY_TOUCH)exit=1;
	if(exit_vbl<=0)exit=1;
	memmove(fb0, palibs_bin, palibs_bin_size);
	memmove(fb1, neosplash_bin, neosplash_bin_size);
	DrawSystemUpdate();
	swiWaitForVBlank();
	scanKeys();
	exit_vbl--;
	}
	setBrightness(3, 0);
	fifoSendValue32(FIFO_PM, PM_REQ_SLEEP_DISABLE);
	LoadFont("/DsOS/images/font/text.png");
	int width, height;
	sfondo = LoadPNG("/DsOS/images/background.png", &width, &height);
	FB_Init();
	IV_Init();
	TV_Init();
	u32 downtime=0;
	u32 uptime=0;
	while(1) {
		//Init
		touchRead(&touch2);
		scanKeys();
		int held = keysHeld();
		int down = keysDown();
		int up = keysUp();
		if((down & KEY_TOUCH) && (downtime+uptime < 45))dblclick=1;
		else dblclick=0;
		if(down & KEY_TOUCH)downtime=0; // = 0 if newpress
		if(held & KEY_TOUCH)downtime++;
		if(up & KEY_TOUCH)uptime=0; // = 0 when released
		if(!(held & KEY_TOUCH))uptime++;
		if(held & KEY_TOUCH)touch=touch2;
		s32 pressure = (touch2.rawx * touch2.z2) / (touch2.z1) - touch2.rawx;
		if(down & KEY_LID){
		powerOff((PM_Bits )0b00001100);  //disabilita i monitor 
		}else if(up & KEY_LID){
		powerOn((PM_Bits )0b00001100);  //abilita i monitor 
		}
		if(held & KEY_TOUCH){
		if(pressure>=2000){
		ndita = 1;
		}else if(pressure<2000&&pressure>1000){
		ndita = 2;
		}else{
		ndita = 3;
		}
		}else ndita = 0;
		apps[curapp].Act();
		/*for(int x=0;x<256;x++){
		for(int y=80;y<112;y++){
		u32 r=GetPixelR(1, x, y);
		u32 g=GetPixelG(1, x, y);
		u32 b=GetPixelB(1, x, y);
		r=(r+(x>>4)*31)/((x>>4)+1);
		g=(g+(x>>4)*31)/((x>>4)+1);
		b=(b+(x>>4)*31)/((x>>4)+1);
		PutPixelRGB(1, x, y, ARGB16(1, r, g, b));
		}
		}*/
		#ifdef DEBUG
		Print(1, 0, 168, ARGB16(1, 0, 0, 0), "Free: %d", getMemFree());
		Print(1, 0, 176, ARGB16(1, 0, 0, 0), "Used: %d", getMemUsed());
		Print(1, 0, 184, ARGB16(1, 0, 0, 0), "FPS: %d", FPS);
		#endif
		DrawSystemUpdate();
		HandleApps();
		count++;
		//swiWaitForVBlank();
	}
	return 0;
}

