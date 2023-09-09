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

#include "ct_const.h"
#include "ct_linkedlist.h"
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include

typedef char tAniHanzi[56 * 56];
typedef char tAniHanziHDD[(56 * 56) / 8];

// Variables
FILE *lanihanzi;

LList *aniHanzi;
int aniHanziLen;

tAniHanzi aAniHanzi;
tAniHanzi *aAniHanziMem;
tAniHanziHDD aAniHanziHDD;

/* Init this HW layer */
void ct_hwanihanzi_init() {
  aniHanzi = NULL;   
  aniHanziLen = 0;
}   

int hwanihanzi_LoadOffset(char *pinyin, int *offset, int *tam) {
   char dir[255];
   char act_pinyin[20];
   char param_pinyin[20];
   char tmpstr1[10];
   char *res;
   FILE *fidx;
   int end;
	   
   *offset = 0;
   *tam = 0;
   sprintf(param_pinyin,"%s",pinyin);
   ct_utillib_str_toupper(param_pinyin);
   
   strncpy(tmpstr1,pinyin,2);
   tmpstr1[2] = '\0';
   ct_utillib_str_toupper(tmpstr1);

   
   // Open idx file
	sprintf(dir,"%s%s/%s%s",DIR_MAIN,DIR_PINYIN,tmpstr1,DIR_IDX);   
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
		      // Let's compare the "pinyin"...
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

/* Load an animated hanzi into the system */
void ct_hwanihanzi_load(char *pinyin) {
   char dir[255];
   char tmpstr1[10];
   int i,j;
   int offset, size;

  // Destroys previous anihanzi
  if (aniHanzi != NULL) {
    ct_linkedlist_finish(aniHanzi);   
    aniHanziLen = 0;
  }   
  aniHanzi = ct_linkedlist_init();
  
  // Let's get the new one...
  hwanihanzi_LoadOffset(pinyin,&offset,&size);
  if (size > 0) {
		// Loads new one - open file
		strncpy(tmpstr1,pinyin,2);
		tmpstr1[2] = '\0';
		ct_utillib_str_toupper(tmpstr1);
		sprintf(dir,"%s%s/%s%s",DIR_MAIN,DIR_PINYIN,tmpstr1,DIR_PAK);
		lanihanzi = fopen (dir, "rb");
		if (lanihanzi == NULL)
			return;
		// Load AniHanzi Frames
		fseek(lanihanzi,offset,SEEK_SET);
		aniHanziLen = size / ((56 * 56) / 8);
		for (i = 0; i < aniHanziLen; i++) {
		  // Load the compressed frame
		  fread(&aAniHanziHDD,sizeof(tAniHanziHDD),1,lanihanzi);
		  
		  // Transforms the compressed frame into an uncompressed frame
		  aAniHanziMem = malloc(sizeof(tAniHanzi));
		  for (j = 0; j < ((56 * 56) - 1); j++) {
		     (*aAniHanziMem)[j] = ct_utillib_bitOne8(aAniHanziHDD[j / 8],j % 8);
		  }   
		  
		  // Stores uncompressed frame into the list
		  ct_linkedlist_addEnd(aniHanzi,(void *)aAniHanziMem);
		}   
			
		// Close file
		fclose(lanihanzi);
	}	
}

/* Draws the animated hanzi into the screen */
void ct_hwanihanzi_draw(int screen, int x, int y, int frame) {
   
   int i;
   int actframe;
   
	if (aniHanziLen <= 0)
		return;
	actframe = ((frame >= 0) && (frame <= aniHanziLen-1))?frame:aniHanziLen - 1;
	
	aAniHanziMem = ct_linkedlist_getPos(aniHanzi,actframe);
   
   for (i = 0; i < ((56 * 56) - 1); i++)
   	PA_Put8bitPixel(screen,x + (i % 56),y + (i / 56),(*aAniHanziMem)[i]);
}   

/* Draws the animated hanzi into the screen, x2 */
void ct_hwanihanzi_draw2x(int screen, int x, int y, int frame) {
   
   int i;
   int actframe;
   
	if (aniHanziLen <= 0)
		return;
	actframe = ((frame >= 0) && (frame <= aniHanziLen-1))?frame:aniHanziLen - 1;
	
	aAniHanziMem = ct_linkedlist_getPos(aniHanzi,actframe);
   
   for (i = 0; i < ((56 * 56) - 1); i++) {
   	PA_Put8bitPixel(screen,x + ((i % 56) * 2),y + ((i / 56) * 2),(*aAniHanziMem)[i]);
   	PA_Put8bitPixel(screen,x + ((i % 56) * 2) + 1,y + ((i / 56) * 2),(*aAniHanziMem)[i]);
   	PA_Put8bitPixel(screen,x + ((i % 56) * 2),y + ((i / 56) * 2) + 1,(*aAniHanziMem)[i]);
   	PA_Put8bitPixel(screen,x + ((i % 56) * 2) + 1,y + ((i / 56) * 2) + 1,(*aAniHanziMem)[i]);
	}   
}   


/* Get the number of frames */
int ct_hwanihanzi_getFrames(){
  return aniHanziLen;  
}   

