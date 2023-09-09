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
#include "ct_HALhanzi.h"

#include "gfx/all_gfx.h" // PAGfx Include

#ifdef DLDI

//----------------------------------------DLDI Section------------------------------------------

/* Variables - Internal State */
////////////////////////////////

int lang;
FILE *llisten_f_title;
FILE *llisten_f_intro;
FILE *llisten_f_phrase;
FILE *llisten_f_tphrase;
FILE *llisten_f_num;


/* Internal Functions */ 
////////////////////////

void l_listen_getTitle(char *instr,int number)
{
   fgets(instr, 254, llisten_f_title); // English, first line
   if (lang == LANG_ENG) {

   } else if (lang == LANG_SPA) {
      fgets(instr, 254, llisten_f_title); // Spanish, second line
   } else {
      sprintf(instr,"XXXX");
   } 
	ct_utillib_del_newline(instr);
}

int l_listen_getIntroCont(char *instr, int number, int it)
{
   char *res;
   
   res = fgets(instr, 254, llisten_f_intro); 
   if (res == NULL) {
   	sprintf(instr,"XXXX");
   	return 0;
	}   
	
	ct_utillib_del_newline(instr);
	return 1;
}   

int l_listen_getPhraseCont(char *instr, int number, int which)
{
   char utf16[10];
   int end = 0;
   int res = 0;
   int val;

   while (!end) {
	   val = ct_utillib_getutf16file(llisten_f_phrase,utf16);
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

int l_listen_getTPhraseCont(char *instr, int number, int which)
{
   char *res;
   
   res = fgets(instr, 254, llisten_f_tphrase); 
   if (res == NULL) {
   	sprintf(instr,"XXXX");
   	return 0;
	}   
	
	ct_utillib_del_newline(instr);
	return 1;
}   

int l_listen_getLessons()
{
   int val;
   char dir[255];
   
	sprintf(dir,"%s%s%s%s",DIR_MAIN,DIR_LLISTEN,DIR_F_NUM,DIR_EXT);
	llisten_f_num = fopen (dir, "rb");
	
	fscanf(llisten_f_num,"%d",&val);
	
	fclose(llisten_f_num);
	
	return val;
}   

/* Functions */ 
///////////////   

/* Init the "HW" layer - Transaction */
void ct_l_listen_initTrans(int number, int actlang)
{
   char dir[255];
   lang = actlang;
   
	// File Access - open the files :-)   
	sprintf(dir,"%s%s/%d%s%s",DIR_MAIN,DIR_LLISTEN,number,DIR_F_TITLE,DIR_EXT);
	llisten_f_title = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s%s",DIR_MAIN,DIR_LLISTEN,number,DIR_F_INTRO,
				(lang == LANG_ENG)?"_eng":"_spa",
				DIR_EXT);
	llisten_f_intro = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s",DIR_MAIN,DIR_LLISTEN,number,DIR_F_PHRASE,DIR_EXT);
	llisten_f_phrase = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s%s",DIR_MAIN,DIR_LLISTEN,number,DIR_F_TPHRASE,
				(lang == LANG_ENG)?"_eng":"_spa",
				DIR_EXT);
	llisten_f_tphrase = fopen (dir, "rb");
}

/* Gets a pointer to the contents of the lessson */
void ct_l_listen_get(int number, void *lesson)
{   
	gLesson *inner = (gLesson *)lesson;
	int i;
	char *str;
	char strtmp[255];
	
	// Title
	l_listen_getTitle(strtmp,number);
	sprintf(inner->title,"%s %d:\n\n %s",ct_const_getText(TXT_MM_LESSON,lang),number,strtmp);
			
	// Introductory Text	
	i = 0;
	while (l_listen_getIntroCont(strtmp,number,i)) {
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->intro,(void *)str);	   
	   i++;
	}	
		
	// Phrases & translations
	i = 0;
	PA_CopyText(strtmp,"");
	while (l_listen_getPhraseCont(strtmp,number,i)) {

		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->phrase,(void *)str);	   
		
		l_listen_getTPhraseCont(strtmp,number,i);
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->tphrase,(void *)str);	   
		
		PA_CopyText(strtmp,"");
	   i++;
	}	
	
	// Hanzi Calcultation - TODO
	ct_l_hw_getHanzi(inner);
}

/* Finishes the "HW" layer - Transaction */
void ct_l_listen_endTrans(int number)
{
	// File Access - Close the files ;-)
	fclose(llisten_f_title);
	fclose(llisten_f_intro);   
	fclose(llisten_f_phrase);
	fclose(llisten_f_tphrase);
}   

/* Gets the number of lessons of this type */
int ct_l_listen_getLessons()
{
   return l_listen_getLessons();
}

#else

//------------------------------------non-DLDI Section------------------------------------------

/* Variables - Internal State */
////////////////////////////////

int lang;

/* Internal Functions */ 
////////////////////////

char *l_listen_getTitle(int number)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: return "To Be, Hi, Bye";
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: return "Verbo Ser, Hola, Adios";
		   } 
   }   
   return "";
}

char *l_listen_getIntro(int number, int it)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "In this lesson we will listen to a basic hello conversation. Ready?";
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "En esta leccion escucharemos una conversacion de hola y adios muy basica. Preparado?";
		   } 
   }   
   return "";
}

char *l_listen_getPhrase(int number, int which)
{
	switch (number) {
   	case 1: 
   	   if (which == 1)
		   	return "4F60597D,6211 662F 4E3D4E3D,6211 662F 4E2D56FD 4EBA";
   	   if (which == 2)
		   	return "4F60597D,6211 662F 5C0F660E,6211 4E5F 662F 4E2D56FD 4EBA";
   	   if (which == 3)
		   	return "5979 662F 5B895A1C,5979 662F 897F73ED7259 4EBA.5B895A1C 662F 897F73ED7259 4EBA";		   	
		   return "";
	}  
	return "";
}

char *l_listen_getTrans(int number, int which)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
				   if (which == 1)
						return "Hi, I am Lili, I am chinese";
				   if (which == 2)
						return "Hi, I am Xiaoming, I am also chinese";
				   if (which == 3)
						return "She is Ana, She is spaniard. Ana is spaniard";						
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
				   if (which == 1)
						return "Hola, yo soy Lili, yo soy china";
				   if (which == 2)
						return "Hola, yo soy Xiaoming, yo tambien soy chino";
				   if (which == 3)
						return "Ella es Ana, ella es espanyola. Ana es espanyola";
		   } 
   }   
   return "";
}

int l_listen_getLessons()
{
   return 1;
}   

/* Functions */ 
///////////////   

/* Init the "HW" layer - Transaction */
void ct_l_listen_initTrans(int number, int actlang)
{
	// RAM Access - no need to apply transaction here :-)   
	lang = actlang;
}

/* Gets a pointer to the contents of the lessson */
void ct_l_listen_get(int number, void *lesson)
{
	gLesson *inner = (gLesson *)lesson;
	int i;
	char *str;
	
	// Title
	sprintf(inner->title,"%s %d:\n\n %s",ct_const_getText(TXT_MM_LESSON,lang),number,l_listen_getTitle(number));
	
	// Introductory Text
	for (i = 0; !PA_CompareText("",l_listen_getIntro(number,i)); i++) {
		str = malloc(strlen(l_listen_getIntro(number,i))+2);
		PA_CopyText(str,l_listen_getIntro(number,i));
		ct_linkedlist_addEnd(inner->intro,(void *)str);
	}		
	
	// Phrases & Translations
	for (i = 1; !PA_CompareText("",l_listen_getPhrase(number,i)); i++) {
		str = malloc(strlen(l_listen_getPhrase(number,i))+2);
	   PA_CopyText(str,l_listen_getPhrase(number,i));
		ct_linkedlist_addEnd(inner->phrase,(void *)str); 
	}		
	for (i = 1; !PA_CompareText("",l_listen_getTrans(number,i)); i++) {
		str = malloc(strlen(l_listen_getTrans(number,i))+2);
	   PA_CopyText(str,l_listen_getTrans(number,i));
		ct_linkedlist_addEnd(inner->tphrase,(void *)str); 
	}		
	
	// Hanzi Calcultation - TODO
	ct_l_hw_getHanzi(inner);
}

/* Finishes the "HW" layer - Transaction */
void ct_l_listen_endTrans(int number)
{
	// RAM Access - no need to apply transaction here :-)   
}   

/* Gets the number of lessons of this type */
int ct_l_listen_getLessons()
{
   return l_listen_getLessons();
}

#endif
