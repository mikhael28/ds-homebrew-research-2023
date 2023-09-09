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
/* TODO
 - Sound
*/

// Includes
#include <PA9.h>       // Include for PA_Lib

#include "ct_const.h"
#include "ct_hwhanzi.h"
#include "ct_hwpinyin.h"
#include "ct_linkedlist.h"
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Variables - Internal State */
////////////////////////////////

#define PINYIN_SIZE 2;

LList *lhanzi;
char pinyin[255];
int HAL_pinyinsize = PINYIN_SIZE;
int HAL_hanzicolor = 1;
int HAL_hanzihide = -1;

/* Functions */ 
///////////////

/* Init the HAL layer */
void ct_HALhanzi_init()
{
  ct_hwhanzi_init();
  ct_hwpinyin_init(); 
  lhanzi = ct_linkedlist_init();
}

/* Preload hanzi information inside the HAL layer */
void ct_HALhanzi_preloadHanzi(u16 hanzi)
{
   
}

/* Deletes Preload information */
void ct_HALhanzi_reset(u16 hanzi)
{
   ct_linkedlist_finish(lhanzi);
   lhanzi = ct_linkedlist_init();
}

/* Gets a pointer to a certain sprite of a hanzi */
void* ct_HALhanzi_getHanzi(u16 hanzi)
{
   
}

/* Sets color of hanzi. -1 resets */
void ct_HALhanzi_setHanziColor(int c) 
{
   if (c == -1) {
   	HAL_hanzicolor = 1;
   } else {
      HAL_hanzicolor = c;
   }   
}

/* Paints a basic Hanzi onto the screen */
void ct_HALhanzi_paintHanzi16x16(int screen, u16 hanzi,int x, int y) 
{
   int *hanziIma;
   int i;
   
   hanziIma = (int *)ct_hwhanzi_getHanzi(hanzi);
   
   for (i = 0; i < 256; i++)
   	PA_Put8bitPixel(screen,x + (i % 16),y + (i / 16),(hanziIma[i]==1)?HAL_hanzicolor:0);
}

/* Paints a special Separator onto the screen */
void ct_HALhanzi_paintSep16x16(int screen, char sep,int x, int y) 
{
   int *hanziIma;
   int i;
   
   hanziIma = (int *)ct_hwhanzi_getHanziSep(sep);
   
   for (i = 0; i < 256; i++)
   	PA_Put8bitPixel(screen,x + (i % 16),y + (i / 16),(hanziIma[i]==1)?HAL_hanzicolor:0);
}      

/* Paints a Extended Hanzi onto the screen */
void ct_HALhanzi_paintHanzi32x32(int screen, u16 hanzi,int x, int y) 
{
   int *hanziIma;
   int i;
   
   hanziIma = (int *)ct_hwhanzi_getHanzi(hanzi);
   
   for (i = 0; i < 256; i++) {
   	PA_Put8bitPixel(screen,x + ((i % 16) * 2),y + ((i / 16) * 2),(hanziIma[i]==1)?HAL_hanzicolor:0);
   	PA_Put8bitPixel(screen,x + ((i % 16) * 2) + 1,y + ((i / 16) * 2),(hanziIma[i]==1)?HAL_hanzicolor:0);
   	PA_Put8bitPixel(screen,x + ((i % 16) * 2),y + ((i / 16) * 2) + 1,(hanziIma[i]==1)?HAL_hanzicolor:0);
   	PA_Put8bitPixel(screen,x + ((i % 16) * 2) + 1,y + ((i / 16) * 2) + 1,(hanziIma[i]==1)?HAL_hanzicolor:0);
 }  	
} 

/* Paints an Extra-Extended Hanzi onto the screen */
void ct_HALhanzi_paintHanzi64x64(int screen, u16 hanzi,int x, int y) 
{
   int *hanziIma;
   int i;
   int j;
   int k;
   
   hanziIma = (int *)ct_hwhanzi_getHanzi(hanzi);
   
   for (i = 0; i < 256; i++) {
     for (j = 0; j < 3; j++) {      
       for (k = 0; k < 3; k++) {
     	   PA_Put8bitPixel(screen,x + ((i % 16) * 3) + j,y + ((i / 16) * 3) + k,(hanziIma[i]==1)?HAL_hanzicolor:0);   	    	 
       } 	 
     }   
   }  	
}  

/* Gets a char * to the pinyin of a hanzi */
char* ct_HALhanzi_getPinyin(u16 hanzi)
{
   return ct_hwpinyin_getPinyin(hanzi);
}

/* Gets a char * to the meaning of a hanzi */
char* ct_HALhanzi_getMeaning(u16 hanzi)
{
   return ct_hwpinyin_getMeaning(hanzi);
}

/* Gets a certain hanzi number (starting from 1) from a hanzi word */
u16 ct_HALhanzi_word_getHanzi(char *str, int which)
{
   int res = 0;
   int j;
   char hex[4];
   
   if ((which * 4) > strlen(str))
   	return res;
   if (which <= 0)
   	return res;
   	
   for (j = ((which * 4) - 1); j >= ((which - 1) * 4); j--) {
      hex[j - ((which - 1) * 4)] = str[j];
   }   
   return ct_utillib_hex4int(hex);
}  

/* Gets how many hanzi are in the word */
u16 ct_HALhanzi_word_lenHanzi(char *str)
{
   return strlen(str) / 4;
}	 

// Check if the Hanzi pos. 'where' exists. 1...N. 0 if not. 
int HALhanzi_phrase_isHanzi(char *str, int which, int startpos, int startlen) 
{
   int pos = startpos;
   int len = startlen;
   int inword = 0;
   
   while (pos < strlen(str)) {
      // Advance a word
      if ((str[pos] == ' ') || (str[pos] == '!') || (str[pos] == ',') || (str[pos] == '.') || (str[pos] == '?')) { // No words? Advance, we are inside no word
      	pos++;
      	inword = 0;
   	}   
      else {
         if (!inword)
            inword = 1;
         len++;
         pos+=4;
      }   
      // Check
      if (len == which)
      	return pos;
   }   
   return 0;
}   

// Check if the Word pos. 'where' exists. returns number of hanzi inside the word (1...N). 0 if not. 
int HALhanzi_phrase_isWord(char *str, int which, int *pos, int *len) 
{
   int inword = 0;
	int lastword = 0;
	   
   while (*pos < strlen(str)) {
      // Advance a word
      if ((str[*pos] == ' ') || (str[*pos] == '!') || (str[*pos] == ',') || (str[*pos] == '.') || (str[*pos] == '?')) { // No words? Advance, we are inside no word
      	(*pos)++;
      	if (inword > 0) {
      		(*len)++;
      		lastword = inword;
     		}
      	inword = 0;
   	}   
      else {
			inword++;
         (*pos)+=4;
      }   
      // Check
      if (*len == which)
      	return lastword;
   }   
   // Last-time check
	if (inword > 0) {
   	(*len)++;
   	lastword = inword;
   }
   if (*len == which)
   	return lastword;
   	
   return 0;
}   

/* Gets how many hanzi are in the phrase */
u16 ct_HALhanzi_phrase_lenHanzi(char *str)
{
   int len = 0;
   int pos = 0;
   while ((pos = HALhanzi_phrase_isHanzi(str,len + 1,pos,len)) != 0)
   	len++;
   	
   return len;
}	 

/* Gets how many words are in the phrase */
u16 ct_HALhanzi_phrase_lenWord(char *str)
{
   int len = 0;
   int pos = 0;
   int word = 0;
   while ((HALhanzi_phrase_isWord(str,len + 1,&pos,&len)) != 0)
   	word++;
   	
   return word;
}

/* Gets how many special separators are in the phrase */
u16 ct_HALhanzi_phrase_lenSep(char *str)
{
   int len = 0;
   int pos = 0;
   while (pos < strlen(str)) {
      if ((str[pos] == '!') || (str[pos] == ',') || (str[pos] == '.') || (str[pos] == '?')) 
         // Special Separator
	   	len++;
      // Advance a word
	   pos++;
	}   	
   return len;
}

// Check if there is a special separator in this position
char HALhanzi_phrase_isSeparator(char *str, int pos) 
{
   if (pos < strlen(str))
		if ((str[pos] == '!') || (str[pos] == ',') || (str[pos] == '.') || (str[pos] == '?'))  // Separator! Great :-D
   		return str[pos];
   		
   return ' ';
}   
	 	 

/* Get the nth hanzi inside a phrase */
u16 ct_HALhanzi_phrase_getHanzi(char *str, int which)
{
   char hz[4];
   int poshz;
   int j; 
   int i;
   
   if (which > ct_HALhanzi_phrase_lenHanzi(str))
   	return 0;
   	
   poshz = HALhanzi_phrase_isHanzi(str,which,0,0);
   j = 3;
   for (i = poshz - 1; i >= poshz - 4; i--) {
      hz[j] = str[i];
      j--;
   }   
   
   return ct_utillib_hex4int(hz);
}

/* Get the nth word inside a phrase */
char *ct_HALhanzi_phrase_getWord(char *str, int which)
{
   int inpos = 0;
   int inlen = 0;
   int word = 1;
   int actword;
   int i;
   
   PA_CopyText(pinyin,"");
   
   actword = HALhanzi_phrase_isWord(str,inlen + 1,&inpos,&inlen);
   while (actword != 0) {
      if (inlen == which) {
      	for (i = 0; i < actword; i++) {
         	strcat(pinyin,
						ct_utillib_int4hex(ct_HALhanzi_phrase_getHanzi(str,i + word))
						);
	      }  
	      return pinyin;
   	}    		
 		word += actword;
   	actword = HALhanzi_phrase_isWord(str,inlen + 1,&inpos,&inlen);
 	}  	
 	return "";
}   

// Get the hanzi/separator of a phrase. To use on writing the phrase including separators
int HALhanzi_phrase_getHanziSep(char *str, int *pos, int *len, char *res) 
{
   int inword = 0;
   int i;
   
   while ((*pos) < strlen(str)) {
      // Advance a word
		if ((str[*pos] == ' ') || (str[*pos] == '!') || (str[*pos] == ',') || (str[*pos] == '.') || (str[*pos] == '?')) { // Real Separator
      	res[0] = str[*pos];
      	res[1] = '\0';
      	(*len)++;
      	(*pos)++;      	
      	inword = 1;
   	} else { // Word AAAA
   		for (i = (*pos); i <= (*pos) + 3; i++) {
      		res[i - (*pos)] = str[i];
		   }      	   
		   res[4] = '\0';
         (*len)++;
         (*pos)+=4;
         inword = 1;
      }   
     	return (*pos);
   }   
   return 0;
}  

/* Special: Hides hanzi i from the phrase. -1 resets */
void ct_HALhanzi_paintHide(int h)
{
   HAL_hanzihide = h;
}   

/* Write a hanzi phrase, centered */
void HALhanzi_paint(int screen,char *str, int x, int y)
{
   int inpos;
   int inlen;
   int word;
   int lastword;
   int actpos;
   char hanzi[10];
   int sentences = 0;
   int senlen[16];
   int i;
   int j;
   int xini;
   int yini;
   int paintsep;
   int oldsep;
   
   // Get lenght for separating
	word = 0;
	inpos = 0;
	inlen = 0;
	lastword = 0;
	actpos = HALhanzi_phrase_getHanziSep(str,&inpos,&inlen,hanzi);
	while (actpos != 0) {
	   if (HALhanzi_phrase_isSeparator(hanzi,0) != ' ') {
	      // This is a special separator? - Advance
	      word += 1;
	   	lastword = word;
	   } else if (hanzi[0] == ' ') {
	      // Space - new word... but no advance
	      lastword = word;
		} else {
		   // Advance
		   word += 1;
		}			 
		// New phrase?
		if (word > 14) {
		   senlen[sentences] = lastword;
		   word = word - lastword;
		   lastword = 0;
		   sentences++;
		}   
		// Advance
		actpos = HALhanzi_phrase_getHanziSep(str,&inpos,&inlen,hanzi);
	}  
	if (word > 0) {
		senlen[sentences] = word; 	
		sentences++;
	}	
	   
   // Second... Write ;-)
   inpos = 0; inlen = 0; paintsep = 0; word = 0; oldsep = 1;
   yini = y - ((sentences * 17) >> 1);
   for (i = 0; i < sentences; i++) {
     	xini = x - ((senlen[i] * 17) >> 1);
     	j = 1;
     	while (j <= senlen[i]) {
     	   // Get phrase
     	   actpos = HALhanzi_phrase_getHanziSep(str,&inpos,&inlen,hanzi);
     	   // Nothing? End :-)
     	   if (actpos == 0) {
     	      paintsep = -1;
     	      oldsep = 0; // possible new word
     	      j = senlen[i] + 1;
     	   }   
     	   // Separator? Paint
     	   if (HALhanzi_phrase_isSeparator(hanzi,0) != ' ') {
     	      paintsep = 1;
     	      oldsep = 1;
     	      j++;
     	   } else if (hanzi[0] == ' ') {
     	      // IGNORE this word
     	      paintsep = -1;
     	      oldsep = 1;
     	   } else {
			   paintsep = 0;
			   if (oldsep) {
			      word += 1; // new word
			      oldsep = 0;
			   }   
			   j++;
			}	     
     	 	// Paint - separator, or normal
     	 	if (paintsep == 1) {
     	 	   ct_HALhanzi_paintSep16x16(screen,hanzi[0],xini,yini);
     	 	   xini += 17;
     	 	} else if (paintsep == 0) {   
     	 	   // Is this the "hide" word? - If not, paint :-)
     	 	   if (word != HAL_hanzihide)
     	   		ct_HALhanzi_paintHanzi16x16(screen,ct_utillib_hex4int(hanzi),xini,yini);
     	   	else {
     	   	   // HIDE! - Paint in red, hidden...
     	   	   ct_HALhanzi_setHanziColor(3);
     	   	   ct_HALhanzi_paintSep16x16(screen,'X',xini,yini);
     	   	   ct_HALhanzi_setHanziColor(1);
     	   	}   
     	   	xini += 17;
     	 	}  	
     	}   
     	yini += 17;
   }  	      
}

/* Returns a String containing the Pinyin of this phrase - including commas */
char *ct_HALhanzi_phrase_getStrPinyin(char *str)
{
   int inpos = 0;
   int inlen = 0;
   int word = 0;
   int actword;
   int i;
   int j = 2;
   
   PA_CopyText(pinyin,"");

	word = 1;
   actword = HALhanzi_phrase_isWord(str,inlen + 1,&inpos,&inlen);
   while (actword != 0) {
      for (i = 0; i < actword; i++) {
         strcat(pinyin,ct_hwpinyin_getPinyin(ct_HALhanzi_phrase_getHanzi(str,i + word)));
         j++;
      }  
      if (str[inpos - 1] == ',')
			strcat(pinyin,", "); 
      if (str[inpos - 1] == '!')
			strcat(pinyin,"! "); 			
      if (str[inpos - 1] == '?')
			strcat(pinyin,"? "); 			
      if (str[inpos - 1] == '.')
			strcat(pinyin,". "); 
      if (str[inpos - 1] == ' ')
			strcat(pinyin,"  "); 			
			
 		word += actword;
 		
   	actword = HALhanzi_phrase_isWord(str,inlen + 1,&inpos,&inlen);
 	}  	
 	
 	return pinyin;
}

/* Write a pinyin phrase, centered */
void HALhanzi_paintPinyin(int screen,char *str, int x, int y)
{
   // NOTE: For sizes != 2, the "sentences" system will fail :-)
	char strpinyin[255];
	char pinyinphrase[6][43];
	int sentences;
	int letter;
	int i;
	int j;
	int lastspace;
	int lastspaceglobal;
	int xini;
	int yini;
	
	// First, Get the pinyin
	PA_CopyText(strpinyin,ct_HALhanzi_phrase_getStrPinyin(str));
	
	// Now, time to get the phrases
	sentences = 0;
	letter = 0;
	lastspace = 0;
	lastspaceglobal = 0;
	for (i = 0; i < strlen(strpinyin); i++) {
	   if ((strpinyin[i] == ' ') || (strpinyin[i] == '!') || (strpinyin[i] == ',') || (strpinyin[i] == '.') || (strpinyin[i] == '?')) {
	      lastspace = letter;
	      lastspaceglobal = i;
	   }
	   pinyinphrase[sentences][letter] = strpinyin[i];
	   letter++;
	   if (letter == 43) {
	   	for (j = lastspace; j < letter; j++)
	   		pinyinphrase[sentences][j] ='\0';
	   	
	   	i = lastspaceglobal;
	   	sentences++;
	   	letter = 0;
	   	lastspace = 0;
	   }   
	}  
	if (letter < 43) 
		pinyinphrase[sentences][letter] = '\0';
	
	// Writing time!!!!
	yini = y - ((sentences * 9) >> 1);
	for (i = 0; i <= sentences; i++) {
	   xini = x - (( strlen(pinyinphrase[i]) * 6) >> 1);
	   
	   PA_CenterSmartText(screen,xini,yini, 
								xini + (strlen(pinyinphrase[i]) * 6),
								yini + 8, 
								pinyinphrase[i], 1, HAL_pinyinsize, 0);									
		yini += 10;
	}   
	
	// Return to old text size...
	HAL_pinyinsize = PINYIN_SIZE;
}   

/* Writes a hanzi phrase, centered */
void ct_HALhanzi_paintX(int screen,char *str, int x, int y)
{
   HALhanzi_paint(screen,str,x,y);   
}

void ct_HALhanzi_paint(int screen,char *str, int y)
{
	HALhanzi_paint(screen,str,128,y);   
}

/* Write a pinyin phrase, centered */
void ct_HALhanzi_paintPinyin(int screen,char *str, int y)
{
   HALhanzi_paintPinyin(screen,str,128,y);
}

void ct_HALhanzi_paintPinyinX(int screen,char *str, int x, int y)
{
   HALhanzi_paintPinyin(screen,str,x,y);
}   

/* Set the size of the pinyin for the next phrase */
void ct_HALhanzi_setPinyinSize(int size) {
   HAL_pinyinsize = size;
}   

/* Write a hanzi phrase, centered  - old version
void HALhanzi_paint(int screen,char *str, int x, int y)
{
   int inpos = 0;
   int inlen = 0;
   int word = 0;
   int actword;
   int totlen = ct_HALhanzi_phrase_lenHanzi(str);
   int sentences = 1;
   int senlen[16];
   int i;
   int j;
   int xini;
   int yini;
   
   if (totlen > 15) {
      // First: Check number of sentences and get hanzi per line
      actword = HALhanzi_phrase_isWord(str,inlen + 1,&inpos,&inlen);
      while (actword != 0) {
      	word += actword;
      	if (word > 15) {
      	   senlen[sentences] = (word - actword);
      	   word = word  - senlen[sentences];
      	   sentences++;
      	}   
   		actword = HALhanzi_phrase_isWord(str,inlen + 1,&inpos,&inlen);
 		}  	
 		if (word != 0)
 			senlen[sentences] = word;
	} else
	   senlen[1] = totlen;
	   
   // Second... Write ;-)
   actword = 1;
   yini = y - ((sentences * 17) >> 1);
   for (i = 1; i <= sentences; i++) {
     	// Debug - PA_OutputText(1,0,i,"Sen. %d Words %d",i,senlen[i]);
     	xini = x - ((senlen[i] * 17) >> 1);
     	for (j = 1; j <= senlen[i]; j++) {
     	   ct_HALhanzi_paintHanzi16x16(screen,ct_HALhanzi_phrase_getHanzi(str, actword),xini,yini);
     	   xini += 17;
     	   actword++;
     	}   
     	yini += 17;
   }  	      
}
*/
