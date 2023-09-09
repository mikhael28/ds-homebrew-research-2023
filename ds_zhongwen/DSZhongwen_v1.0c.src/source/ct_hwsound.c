/*
Copyright (c) 2007 Rodrigo Roman and Ramon Roman (rrc2soft)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of "rrc2soft" nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// Includes
#include <PA9.h>       // Include for PA_Lib
#include <ctype.h>

#include "ct_const.h"
#include "ct_linkedlist.h"
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include

char *listeningSnd; 
u32 listeningSize;
char *pinyinSnd;
int pinyinSize;

int hwsound_LoadOffset(char *pinyin, int *offset, int *tam) {
   char dir[255];
   char act_pinyin[20];
   char param_pinyin[20];
   char *res;
   FILE *fidx;
   int end;
	   
   *offset = 0;
   *tam = 0;
   sprintf(param_pinyin,"%s",pinyin);
   ct_utillib_str_toupper(param_pinyin);
   
   // Open idx file
	sprintf(dir,"%s%s/%c%s",DIR_MAIN,DIR_PINYIN,tolower(param_pinyin[0]),DIR_IDX);   
	fidx = fopen(dir,"rb");
	if (fidx != NULL) {
	   // Check all the entries
	   end = 0;
	   while (!end) {
	   	res = fgets(dir,254,fidx);
	   	if (res == NULL) {
	   	   // No more lines...
	      	end = 1;
	      	*offset = 0;
	      	*tam = 0;
		   } else {
		      // Let's compare the pinyin...
		      sscanf(dir,"%s %d %d",act_pinyin,offset,tam);
		      ct_utillib_str_toupper(act_pinyin);
		      ct_utillib_str_toupper(param_pinyin);
		      if (strcmp(act_pinyin,param_pinyin) == 0) {
		         // Found!!!!!!
		         end = 1;
		      }   
		   }   
		}   
	}   
	
	if ((*tam) == 0)
		return 0;
	else return 1;
}   

int hwsound_LoadPAK(char **buffer, char *pinyin, int offset, int tam) {   
   char dir[255];
   int res = 0;

   FILE *fraw;
      
   // Open raw file
	sprintf(dir,"%s%s/%c%s",DIR_MAIN,DIR_PINYIN,tolower(pinyin[0]),DIR_EXTSND);   
	fraw = fopen(dir,"rb");
	if (fraw != NULL) {
	   fseek(fraw,offset,SEEK_SET);
	   *buffer = (char*) malloc (sizeof(char)*tam);	   
	   if (buffer != NULL) {
	   	fread ((void *)*buffer,1,tam,fraw);
	   	res = 1;
		}   
	   fclose (fraw);
	}
	return res;   
}

int hwsound_Load(char *dir, char **buffer, u32 *bufferSize) {   
   FILE *fsound;
   int res = 0;
   
	fsound = fopen (dir, "rb");
	if (fsound != NULL) {
		// obtain file size:
	   fseek (fsound , 0 , SEEK_END);
	   *bufferSize = ftell (fsound);
	   rewind (fsound);
		
	   *buffer = (char*) malloc (sizeof(char)*(*bufferSize));
	   if (buffer != NULL) {
	   	fread ((void *)*buffer,1,*bufferSize,fsound);
	   	res = 1;
		}   
	   fclose (fsound);
	}  
	return res;
}   

/* Init the HW layer */
void ct_hwsound_init() {
   listeningSnd = NULL;
   pinyinSnd = NULL;
}   

/* Preloads a certain sound of a pinyin */
int ct_hwsound_preloadPinyin(char *pinyin) {
   int offset;
   int tam;
   
   if (pinyinSnd != NULL)
   	free(pinyinSnd);
   pinyinSnd = NULL;
   
   // Read Offset of the sound
   if (hwsound_LoadOffset(pinyin, &offset, &tam)) {
      pinyinSize = tam;
      return hwsound_LoadPAK(&pinyinSnd, pinyin, offset, tam);
   } else {
      return 0;
   }   
}   

/* Gets a void * to the sound of a pinyin */
void* ct_hwsound_getPinyin() {
   return pinyinSnd;
}

/* Gets the size of the sound of a pinyin */
u32 ct_hwsound_getPinyinSize() {
   return pinyinSize;
}   

/* Preloads a certain sound of a lesson */
int ct_hwsound_preloadListening(int lesson, int part) {
   char dir[255];
   
   if (listeningSnd != NULL)
   	free(listeningSnd);
   listeningSnd = NULL;
   
   // Read sound
	sprintf(dir,"%s%s/%d%s%d_%d%s",DIR_MAIN,DIR_LLISTEN,lesson,DIR_F_SNDLST,lesson,part,DIR_EXTSND);
	return hwsound_Load(dir,&listeningSnd,&listeningSize);
}   

/* Gets the sound of a lesson */
void *ct_hwsound_getListening() {
   return listeningSnd;
}   

/* Gets the size of the sound of a lesson */
u32 ct_hwsound_getListeningSize() {
   return listeningSize;
}
