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
#include "ct_l_hw.h"

#include "gfx/all_gfx.h" // PAGfx Include

#ifdef DLDI

//----------------------------------------DLDI Section------------------------------------------

/* Variables - Internal State */
////////////////////////////////

int lang;
FILE *lhanzi_f_title;
FILE *lhanzi_f_intro;
FILE *lhanzi_f_phrase;
FILE *lhanzi_f_num;

/* Internal Functions */ 
////////////////////////

void l_hanzi_getTitle(char *instr,int number)
{
   fgets(instr, 254, lhanzi_f_title); // English, first line
   if (lang == LANG_ENG) {

   } else if (lang == LANG_SPA) {
      fgets(instr, 254, lhanzi_f_title); // Spanish, second line
   } else {
      sprintf(instr,"XXXX");
   } 
	ct_utillib_del_newline(instr);
}

int l_hanzi_getIntroCont(char *instr, int number, int it)
{
   char *res;
   
   res = fgets(instr, 254, lhanzi_f_intro); // English, first line
   if (res == NULL) {
   	sprintf(instr,"XXXX");
   	return 0;
	}   
	
	ct_utillib_del_newline(instr);
	return 1;
}   

int l_hanzi_getHanziCont(char *instr, int number, int size)
{
   char utf16[10];
   int end = 0;
   int res = 0;
   int val;

   while (!end) {
	   val = ct_utillib_getutf16file(lhanzi_f_phrase,utf16);
	   switch (val) {
	      case -1: // EOF
	         end = 1;
	         if (strlen(instr) == 0)
	         	res = 0;
				else res = 1; 
	      	break;
	      case 0: // Character
	         strncat(instr,utf16,4);
	      	break;
	      case 1: // CR/LF
	         end = 1;
	         res = 1;
	      	break;
	      case 2: // BOM
	      	break;	      	
	   }   
   }
   return res;
}   

int l_hanzi_getLessons()
{
   int val;
   char dir[255];
   
	sprintf(dir,"%s%s%s%s",DIR_MAIN,DIR_LHANZI,DIR_F_NUM,DIR_EXT);
	lhanzi_f_num = fopen (dir, "rb");
	
	fscanf(lhanzi_f_num,"%d",&val);
	
	fclose(lhanzi_f_num);
	
	return val;
}   

/* Functions */ 
///////////////   

/* Init the "HW" layer - Transaction */
void ct_l_hanzi_initTrans(int number, int actlang)
{
   char dir[255];
   lang = actlang;
   
	// File Access - open the files :-)   
	sprintf(dir,"%s%s/%d%s%s",DIR_MAIN,DIR_LHANZI,number,DIR_F_TITLE,DIR_EXT);
	lhanzi_f_title = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s%s",DIR_MAIN,DIR_LHANZI,number,DIR_F_INTRO,
				(lang == LANG_ENG)?"_eng":"_spa",
				DIR_EXT);
	lhanzi_f_intro = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s",DIR_MAIN,DIR_LHANZI,number,DIR_F_PHRASE,DIR_EXT);
	lhanzi_f_phrase = fopen (dir, "rb");
}

/* Gets a pointer to the contents of the lessson */
void ct_l_hanzi_get(int number, void *lesson)
{
	gLesson *inner = (gLesson *)lesson;
	int i;
	char *str;
	char strtmp[255];
		
	// Title
	l_hanzi_getTitle(strtmp,number);
	sprintf(inner->title,"%s %d:\n\n %s",ct_const_getText(TXT_MM_LESSON,lang),number,strtmp);
	
			
	// Introductory Text	
	i = 0;
	while (l_hanzi_getIntroCont(strtmp,number,i)) {
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->intro,(void *)str);	   
	   i++;
	}	
		
	// Hanzi
	PA_CopyText(strtmp,"");
	for (i = 1; i <= 4; i++) {
	   if (l_hanzi_getHanziCont(strtmp,number,i)) {
	   	str = malloc(strlen(strtmp)+1);
	   	PA_CopyText(str,strtmp);
	   } else {
	   	str = malloc(3);
	   	PA_CopyText(str,"");
	   }   
		ct_linkedlist_addEnd(inner->hanzi,(void *)str); 
		PA_CopyText(strtmp,"");
	} 
}

/* Finishes the "HW" layer - Transaction */
void ct_l_hanzi_endTrans(int number)
{
	// File Access - Close the files ;-)
	fclose(lhanzi_f_intro);   
	fclose(lhanzi_f_phrase);
	fclose(lhanzi_f_title);
}   

/* Gets the number of lessons of this type */
int ct_l_hanzi_getLessons()
{
   // ++CHANGE++ Put this on l_hanzi_getLessons()
   return l_hanzi_getLessons();
}

#else

//------------------------------------non-DLDI Section------------------------------------------

/* Variables - Internal State */
////////////////////////////////

int lang;

/* Internal Functions */ 
////////////////////////

char *l_hanzi_getTitle(int number)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: return "Numbers";
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: return "Numeros";
		   } 
   }   
   return "";
}

char *l_hanzi_getIntro(int number, int it)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "In this lesson we will learn the numbers (from 1 to 10) in chinese";
      		   if (it == 1)
				   	return "3...2...1... Go!";				   	
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "En esta leccion aprenderemos los numeros (del 1 al 10) en chino";
      		   if (it == 1)
				   	return "3...2...1... Adelante!";				   	
		   } 
   }   
   return "";
}

char *l_hanzi_getHanzi(int number, int size)
{
	switch (number) {
   	case 1: 
   	   if (size == 1)
		   	return "4E00 4E8C 4E09 56DB 4E94 516D 4E03 516B 4E5D 5341";
		   return "";
	}  
	return "";
}

int l_hanzi_getLessons()
{
   return 1;
}   

/* Functions */ 
///////////////   

/* Init the "HW" layer - Transaction */
void ct_l_hanzi_initTrans(int number, int actlang)
{
	// RAM Access - no need to apply transaction here :-)   
	lang = actlang;
}

/* Gets a pointer to the contents of the lessson */
void ct_l_hanzi_get(int number, void *lesson)
{
	gLesson *inner = (gLesson *)lesson;
	int i;
	char *str;
	
	// Title
	sprintf(inner->title,"%s %d:\n\n %s",ct_const_getText(TXT_MM_LESSON,lang),number,l_hanzi_getTitle(number));
	
	// Introductory Text
	for (i = 0; !PA_CompareText("",l_hanzi_getIntro(number,i)); i++) {
		str = malloc(strlen(l_hanzi_getIntro(number,i))+1);
		PA_CopyText(str,l_hanzi_getIntro(number,i));
		ct_linkedlist_addEnd(inner->intro,(void *)str);
	}		
	
	// Hanzi
	for (i = 1; i <= 4; i++) {
		if (!PA_CompareText("",l_hanzi_getHanzi(number,i))) {
	   	str = malloc(strlen(l_hanzi_getHanzi(number,i))+1);
	   	PA_CopyText(str,l_hanzi_getHanzi(number,i));
		} else {
	   	str = malloc(3);
	   	PA_CopyText(str,"");
		}
		ct_linkedlist_addEnd(inner->hanzi,(void *)str); 
	}		
}

/* Finishes the "HW" layer - Transaction */
void ct_l_hanzi_endTrans(int number)
{
	// RAM Access - no need to apply transaction here :-)   
}   

/* Gets the number of lessons of this type */
int ct_l_hanzi_getLessons()
{
   return l_hanzi_getLessons();
}

#endif

   
