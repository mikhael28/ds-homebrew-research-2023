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

/* Variables - Internal State */
////////////////////////////////

int lang;
FILE *lhanziattack_f_phrase;
FILE *lhanziattack_f_num;

/* Internal Functions */ 
////////////////////////

int l_hanziattack_getHanziCont(char *instr, int number, int size)
{
   char utf16[10];
   int end = 0;
   int res = 0;
   int val;

   while (!end) {
	   val = ct_utillib_getutf16file(lhanziattack_f_phrase,utf16);
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

int l_hanziattack_getLessons()
{
   int val;
   char dir[255];
   
	sprintf(dir,"%s%s%s%s",DIR_MAIN,DIR_LHANZIATTACK,DIR_F_NUM,DIR_EXT);
	lhanziattack_f_num = fopen (dir, "rb");
	
	fscanf(lhanziattack_f_num,"%d",&val);
	
	fclose(lhanziattack_f_num);
	
	return val;
}   

/* Functions */ 
///////////////   

/* Init the "HW" layer - Transaction */
void ct_l_hanziattack_initTrans(int number, int actlang)
{
   char dir[255];
   lang = actlang;
   
	// File Access - open the files :-)   
	sprintf(dir,"%s%s/%d%s",DIR_MAIN,DIR_LHANZIATTACK,number,DIR_EXT);
	lhanziattack_f_phrase = fopen (dir, "rb");
}

/* Gets a pointer to the contents of the lessson */
void ct_l_hanziattack_get(int number, void *lesson)
{
	gLesson *inner = (gLesson *)lesson;
	int i;
	char *str;
	char strtmp[255];
		
	// Title
	sprintf(inner->title,"%s %d:\n\n %d - %d",ct_const_getText(TXT_MM_LESSON,lang),number,((number - 1) * 15) + 1,number * 15);
	
	// Introductory Text	
	if (lang == LANG_ENG) {
		sprintf(strtmp,"In this lesson, we have the hanzi from %d to %d!!!!",((number - 1) * 15) + 1,number * 15);
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->intro,(void *)str);	   
		sprintf(strtmp,"Ready for your lesson? Go!!!!!");
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->intro,(void *)str);	   		
	} else {
		sprintf(strtmp,"En esta leccion, tenemos los hanzi desde el %d hasta el %d!!!!",((number - 1) * 15) + 1,number * 15);
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->intro,(void *)str);	   	
		sprintf(strtmp,"Preparado para tu leccion? Adelante!!!!!");
		str = malloc(strlen(strtmp)+1);
		PA_CopyText(str,strtmp);
		ct_linkedlist_addEnd(inner->intro,(void *)str);
	}   
		
	// ...Get!!!
	i = 1;
	PA_CopyText(strtmp,"");	
	if (l_hanziattack_getHanziCont(strtmp,number,i)) {
	  	str = malloc(strlen(strtmp)+1);
	  	PA_CopyText(str,strtmp);
	} else {
	  	str = malloc(3);
	  	PA_CopyText(str,"");
	}   
	ct_linkedlist_addEnd(inner->hanzi,(void *)str); 
}

/* Finishes the "HW" layer - Transaction */
void ct_l_hanziattack_endTrans(int number)
{
	// File Access - Close the files ;-)
	fclose(lhanziattack_f_phrase);
}   

/* Gets the number of lessons of this type */
int ct_l_hanziattack_getLessons()
{
   // ++CHANGE++ Put this on l_hanziattack_getLessons()
   return l_hanziattack_getLessons();
}
