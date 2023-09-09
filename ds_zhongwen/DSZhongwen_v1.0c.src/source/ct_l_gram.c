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
FILE *lgram_f_title;
FILE *lgram_f_intro;
FILE *lgram_f_phrase;
FILE *lgram_f_text;
FILE *lgram_f_num;


/* Internal Functions */ 
////////////////////////

void l_gram_getTitle(char *instr,int number)
{
   fgets(instr, 254, lgram_f_title); // English, first line
   if (lang == LANG_ENG) {

   } else if (lang == LANG_SPA) {
      fgets(instr, 254, lgram_f_title); // Spanish, second line
   } else {
      sprintf(instr,"XXXX");
   } 
	ct_utillib_del_newline(instr);
}

int l_gram_getIntroCont(char *instr, int number, int it)
{
   char *res;
   
   res = fgets(instr, 254, lgram_f_intro); 
   if (res == NULL) {
   	sprintf(instr,"XXXX");
   	return 0;
	}   
	
	ct_utillib_del_newline(instr);
	return 1;
}   

int l_gram_getPhraseCont(char *instr, int number, int which)
{
   char utf16[10];
   int end = 0;
   int res = 0;
   int val;

   while (!end) {
	   val = ct_utillib_getutf16file(lgram_f_phrase,utf16);
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

int l_gram_getTextCont(char *instr, int *val, int number, int which)
{
   char *res;
   char tmp[255];
   
   // Get
   res = fgets(tmp, 254, lgram_f_text); 
   if (res == NULL) {
   	sprintf(instr,"XXXX");
   	return 0;
	}   
	
	ct_utillib_del_newline(tmp);
	
	// Format
	sscanf(tmp,"%d %[^\x7F]",val,instr);
	
	return 1;
}   


int l_gram_getLessons()
{
   return 1;
}   

void l_gram_saveTextPhrase(gLesson *lesson, int text, int phrase)
{
   int *vala;
   int *valb;
   
   vala = malloc(sizeof(int));
   valb = malloc(sizeof(int));	  
	*vala = text;
	*valb = phrase;
	ct_linkedlist_addEnd(lesson->ntext,(void *)vala);
	ct_linkedlist_addEnd(lesson->nphrase,(void *)valb);
}   

/* Functions */ 
///////////////   

/* Init the "HW" layer - Transaction */
void ct_l_gram_initTrans(int number, int actlang)
{
   char dir[255];
   lang = actlang;
   
	// File Access - open the files :-)   
	sprintf(dir,"%s%s/%d%s%s",DIR_MAIN,DIR_LGRAM,number,DIR_F_TITLE,DIR_EXT);
	lgram_f_title = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s%s",DIR_MAIN,DIR_LGRAM,number,DIR_F_INTRO,
				(lang == LANG_ENG)?"_eng":"_spa",
				DIR_EXT);
	lgram_f_intro = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s",DIR_MAIN,DIR_LGRAM,number,DIR_F_PHRASE,DIR_EXT);
	lgram_f_phrase = fopen (dir, "rb");
	sprintf(dir,"%s%s/%d%s%s%s",DIR_MAIN,DIR_LGRAM,number,DIR_F_TEXT,
				(lang == LANG_ENG)?"_eng":"_spa",
				DIR_EXT);
	lgram_f_text = fopen (dir, "rb");
}

/* Gets a pointer to the contents of the lessson */
void ct_l_gram_get(int number, void *lesson)
{
	gLesson *inner = (gLesson *)lesson;
	int i;
	char *str;
	char strtmp[255];
	int val;
	int ntext;
	int oldval;
	int preval;

	
	// Title
	l_gram_getTitle(strtmp,number);
	sprintf(inner->title,"%s %d:\n\n %s",ct_const_getText(TXT_MM_LESSON,lang),number,strtmp);
	
	// Introductory Text	
	i = 0;
	while (l_gram_getIntroCont(strtmp,number,i)) {
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->intro,(void *)str);	   
	   i++;
	}	
	
	// Phrases 
	i = 0;
	PA_CopyText(strtmp,"");
	while (l_gram_getPhraseCont(strtmp,number,i)) {
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->phrase,(void *)str);
		
		PA_CopyText(strtmp,"");
	   i++;
	}	
	
	// Lesson Text & Sections
	i = 0;
	ntext = 0;
	oldval = 0;
	preval = -1;
	while (l_gram_getTextCont(strtmp,&val,number,i)) {
	   // Copy Text
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->text,(void *)str);	  
				
		// Check for new "sections"
		if (preval == -1) {
		   // First phrase, init system
		   preval = val;
		   ntext++;
		} else if (preval == val) {
		   // Same section
		   ntext++;
		} else if (preval != val) {
		   // New section, save
			l_gram_saveTextPhrase(inner,ntext,preval - oldval);
			// Update
			oldval = preval; // Phrases from the previous section
			preval = val; // we are in a new section
			ntext = 1; // and we just read the first phrase of the new section
		}			   
		
		// Next
	   i++;				
	}	
	if (ntext != 0) {
	   // Final save
	   l_gram_saveTextPhrase(inner,ntext,preval - oldval);
	}   

	
	// Hanzi- TODO
	ct_l_hw_getHanzi(inner);
}

/* Finishes the "HW" layer - Transaction */
void ct_l_gram_endTrans(int number)
{
	// File Access - Close the files ;-)
	fclose(lgram_f_title);
	fclose(lgram_f_intro);   
	fclose(lgram_f_phrase);
	fclose(lgram_f_text);
}   

/* Gets the number of lessons of this type */
int ct_l_gram_getLessons()
{
   return l_gram_getLessons();
}

#else

//------------------------------------non-DLDI Section------------------------------------------

/* Variables - Internal State */
////////////////////////////////

int lang;

/* Internal Functions */ 
////////////////////////

char *l_gram_getTitle(int number)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: return "I, You, He/She, ...";
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: return "Yo, Tu, El, ...";
		   } 
   }   
   return "";
}

char *l_gram_getIntro(int number, int it)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "In this lesson we will learn the personal pronouns in chinese.";
      		   if (it == 1)
				   	return "They are easy to understand! (but difficult to write...)";				   	
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "En esta leccion aprenderemos los pronombres personales en chino.";
      		   if (it == 1)
				   	return "Son faciles de entender, pero dificiles de escribir :-).";				   	
		   } 
   }   
   return "";
}

char *l_gram_getText(int number, int it)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "These are the personal pronouns in Chinese.";
      		   if (it == 1)
				   	return "I, You, He/She, We, You, They.";				   	
      		   if (it == 2)
				   	return "Actually... He/She have the same pronunciation, but they are written differently.";				   					   	
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "Estos son los pronombres personales en Chino.";
      		   if (it == 1)
				   	return "Yo, Tu, El, Nosotros, Vosotros, Ellos.";				   	
      		   if (it == 2)
				   	return "El femenino no existe en chino, salvo en el caso de El y Ella.";				   					   	
      		   if (it == 3)
				   	return "En realidad, El y Ella se diferencian en la escritura, pero se pronuncian de igual forma.";				   					   	
		   } 
   }   
   return "";
}

char *l_gram_getPhrase(int number, int it)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "6211";
      		   if (it == 1)
				   	return "4F60";				   	
      		   if (it == 2)
				   	return "4ED6";				   					   	
      		   if (it == 3)
				   	return "62114EEC";
      		   if (it == 4)
				   	return "4F604EEC";				   	
      		   if (it == 5)
				   	return "4ED64EEC";				   					   	
      		   if (it == 6)
				   	return "4ED6";
      		   if (it == 7)
				   	return "5979";				   	
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return "6211";
      		   if (it == 1)
				   	return "4F60";				   	
      		   if (it == 2)
				   	return "4ED6";				   					   	
      		   if (it == 3)
				   	return "62114EEC";
      		   if (it == 4)
				   	return "4F604EEC";				   	
      		   if (it == 5)
				   	return "4ED64EEC";				   					   	
      		   if (it == 6)
				   	return "4ED6";
      		   if (it == 7)
				   	return "5979";				   	
		   } 
   }   
   return "";
}

int l_gram_getNumText(int number, int it)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return 2;
      		   if (it == 1)
				   	return 1;				   	
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return 3;
      		   if (it == 1)
				   	return 1;				   	
		   } 
   }   
   return -1;
}

int l_gram_getNumPhrase(int number, int it)
{
   switch (lang) {
      case LANG_ENG:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return 6;
      		   if (it == 1)
				   	return 2;				   	
		   }    
      case LANG_SPA:
		   switch (number) {
      		case 1: 
      		   if (it == 0)
				   	return 6;
      		   if (it == 1)
				   	return 2;				   	
		   } 
   }   
   return -1;
}

int l_gram_getLessons()
{
   return 1;
}   

/* Functions */ 
///////////////   

/* Init the "HW" layer - Transaction */
void ct_l_gram_initTrans(int number, int actlang)
{
	// RAM Access - no need to apply transaction here :-)   
	lang = actlang;
}

/* Gets a pointer to the contents of the lessson */
void ct_l_gram_get(int number, void *lesson)
{
	gLesson *inner = (gLesson *)lesson;
	int i;
	char *str;
	int *vala;
	int *valb;	
	
	// Title
	sprintf(inner->title,"%s %d:\n\n %s",ct_const_getText(TXT_MM_LESSON,lang),number,l_gram_getTitle(number));
	
	// Introductory Text
	for (i = 0; !PA_CompareText("",l_gram_getIntro(number,i)); i++) {
		str = malloc(strlen(l_gram_getIntro(number,i))+2);
		PA_CopyText(str,l_gram_getIntro(number,i));
		ct_linkedlist_addEnd(inner->intro,(void *)str);
	}		
	
	// Lesson Text
	for (i = 0; !PA_CompareText("",l_gram_getText(number,i)); i++) {
		str = malloc(strlen(l_gram_getText(number,i))+2);
		PA_CopyText(str,l_gram_getText(number,i));
		ct_linkedlist_addEnd(inner->text,(void *)str);
	}		

	// Lesson Phrases
	for (i = 0; !PA_CompareText("",l_gram_getPhrase(number,i)); i++) {
		str = malloc(strlen(l_gram_getPhrase(number,i))+2);
		PA_CopyText(str,l_gram_getPhrase(number,i));
		ct_linkedlist_addEnd(inner->phrase,(void *)str);
	}		
	
	// Lesson "Sections"
	for (i = 0; l_gram_getNumText(number,i) != -1; i++) {
	   vala = malloc(sizeof(int));
	   valb = malloc(sizeof(int));	  
		*vala = l_gram_getNumText(number,i);
		*valb = l_gram_getNumPhrase(number,i);
		ct_linkedlist_addEnd(inner->ntext,(void *)vala);
		ct_linkedlist_addEnd(inner->nphrase,(void *)valb);		
	}   
	
	// Hanzi- TODO
	ct_l_hw_getHanzi(inner);
}

/* Finishes the "HW" layer - Transaction */
void ct_l_gram_endTrans(int number)
{
	// RAM Access - no need to apply transaction here :-)   
}   

/* Gets the number of lessons of this type */
int ct_l_gram_getLessons()
{
   return l_gram_getLessons();
}

#endif
