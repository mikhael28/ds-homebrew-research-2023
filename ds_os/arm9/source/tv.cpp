#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tv.h"
#include "draw.h"
#include "file.h"
#include "main.h"

extern touchPosition touch, touch2;
extern bool dblclick;

int TV_ID;
int TV_nrighe;
char* TV_testo;
bool TV_reset=0;

bool TV_GoToMenu(){
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	if(down & KEY_START)return 1;
	return 0;
}

void TV_Init(){
	TV_ID = InitApp(TV_GoToMenu, TV_Act, "Text Viewer", "/DsOS/images/te.png");
}

int TV_NumeroParole(char*text){
	int len = strlen(text);
	int n=0;
	for(int i=0;i<len;i++){
	if(text[i]==' ' || text[i]=='\n' || text[i]=='\t')n++;
	}
	if(n==0&&len>0)n++;
	if(text[len-1]!=' ' && text[len-1]!='\n' && text[len-1]!='\t')n++;
	return n;
}

int TV_NumeroRighe(char*text){
	int len = strlen(text);
	/*if(len%32>0)return (len/32)+1;
	else return len/32;*/
	int nc=0;
	int n=0;
	for(int i=0;i<len;i++){
	if(text[i]=='\n'){
	nc=0;
	n++;
	}else if(text[i]=='\r'){
	}else if(text[i]=='\t'){
	nc+=4;
	if(nc>=32){
	nc=nc-32;
	n++;
	}
	}else{
	nc++;
	if(nc==32){
	nc=0;
	n++;
	}
	}
	}
	if(nc%31>0 && nc!=0)n++;
	return n;
}

void TV_PrintFixed(char* text, int starty){
	int len = strlen(text);
	int n=0;
	int x=0;
	int y=0;
	int nc=0;
	for(int i=0;i<len;i++){
	if(n>=starty && n<starty+24){
	if(text[i]=='\n'){
	x=0;
	y++;
	}else if(text[i]=='\r'){
	}else if(text[i]=='\t'){
	x+=4;
	if(x>=32){
	x=x-32;
	y++;
	}
	}else{
	PrintChar(1, x*8, y*8, text[i], ARGB16(1, 0, 0, 0));
	x++;
	if(x==32){
	x=0;
	y++;
	}
	}
	}
	if(text[i]=='\n'){
	nc=0;
	n++;
	}else if(text[i]=='\r'){
	}else if(text[i]=='\t'){
	nc+=4;
	if(nc>=32){
	nc=nc-32;
	n++;
	}
	}else{
	nc++;
	if(nc==32){
	nc=0;
	n++;
	}
	}
	if(n>=starty+24)break;
	}
}

void TV_OpenText(const char* filename){
	if(TV_testo!=NULL)free(TV_testo);
	FILE* fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	TV_testo = (char*)malloc(size);
	fread(TV_testo, 1, size, fp);
	fclose(fp);
	TV_testo[size] = '\0';
	TV_nrighe = TV_NumeroRighe(TV_testo);
	TV_reset=1;
}

void TV_Act(){
	int held = keysHeld();
	int down = keysDown();
	int up = keysUp();
	static s32 starty=0;
	static u8 scroll_vbl=0;
	if(TV_testo==NULL){
	memmove(fb0, sfondo, 256*192*2);
	memmove(fb1, sfondo, 256*192*2);
	Print(1, 0, 96, ARGB16(1, 0, 0, 0), "Load a text through the file browser");
	Print(0, 0, 96, ARGB16(1, 0, 0, 0), "Load a text through the file browser");
	}else{
	if(TV_reset==1){
	starty=0;
	scroll_vbl=0;
	TV_reset=0;
	}
	memmove(fb0, sfondo, 256*192*2);
	memset(fb1, 255, 256*192*2);
	Print(0, 0, 0, ARGB16(1, 0, 0, 0), "Details:\n\tLines: %d\n\nCurrent Line: %d", TV_nrighe, starty+1);
	if(down & KEY_DOWN){
	scroll_vbl=0;
	starty++;
	}
	if(up & KEY_UP){
	scroll_vbl=0;
	starty--;
	}
	if(down & KEY_RIGHT){
	scroll_vbl=0;
	starty+=5;
	}
	if(down & KEY_LEFT){
	scroll_vbl=0;
	starty-=5;
	}
	if(held & KEY_DOWN){
	scroll_vbl++;
	if(scroll_vbl==8){
	scroll_vbl=7;
	starty++;
	}
	}
	if(held & KEY_UP){
	scroll_vbl++;
	if(scroll_vbl==8){
	scroll_vbl=7;
	starty--;
	}
	}
	if(held & KEY_RIGHT){
	scroll_vbl++;
	if(scroll_vbl==8){
	scroll_vbl=7;
	starty+=5;
	}
	}
	if(held & KEY_LEFT){
	scroll_vbl++;
	if(scroll_vbl==8){
	scroll_vbl=7;
	starty-=5;
	}
	}
	if(up & KEY_DOWN)scroll_vbl=0;
	if(up & KEY_UP)scroll_vbl=0;
	if(up & KEY_RIGHT)scroll_vbl=0;
	if(up & KEY_LEFT)scroll_vbl=0;
	if(starty<0)starty=0;
	if(starty>=TV_nrighe)starty=TV_nrighe-1;
	TV_PrintFixed(TV_testo, starty);
	}
}