#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <fat.h>
#include <unistd.h> 
#include <malloc.h> 
#include <errno.h>

char* getExt(const char* filename){
	char* ext=(char*)malloc(32);
	u8 dotpos=0;
	u8 len = strlen(filename);
	for(int i=len-1;i>=0;i--){
	if(filename[i]=='.'){
	dotpos=i+1;
	break;
	}
	}
	if(dotpos==1)return NULL;
	for(int i=dotpos;i<len;i++){
	ext[i-dotpos] = filename[i];
	}
	strlwr(ext);
	ext[len-dotpos] = '\0';
	return ext;
}