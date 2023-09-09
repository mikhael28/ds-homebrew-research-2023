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

#include "ct_l_hw.h"
#include "ct_HALhanzi.h"
#include "ct_linkedlist.h"


/* Gets a Lesson */
void *ct_l_hw_createLesson() 
{
	gLesson *lesson = malloc(sizeof(gLesson));
	
	PA_CopyText(lesson->title,"");
	lesson->hanzi = ct_linkedlist_init();
	lesson->intro = ct_linkedlist_init();
	lesson->text = ct_linkedlist_init();
	lesson->phrase = ct_linkedlist_init();
	lesson->tphrase = ct_linkedlist_init();
	lesson->ntext = ct_linkedlist_init();
	lesson->nphrase = ct_linkedlist_init();	
	
	return lesson;
}   

/* Destroys a Lesson */
void ct_l_hw_destroyLesson(void *lesson) 
{
	gLesson *inner;
	inner = (gLesson *)lesson;
	
	ct_linkedlist_finish(inner->hanzi);
	ct_linkedlist_finish(inner->intro);
	ct_linkedlist_finish(inner->text);
	ct_linkedlist_finish(inner->phrase);
	ct_linkedlist_finish(inner->tphrase);
	ct_linkedlist_finish(inner->ntext);
	ct_linkedlist_finish(inner->nphrase);	
	
	free(inner);
}  

/* Obtains the hanzi of a lesson, automatically */
void ct_l_hw_getHanzi(void *lesson)
{
  	char *str;
	char *strword;
	char *str1;
	char *str2;
	char *str3;
	char *str4;
	int i1 = 0;
	int i2 = 0;
	int i3 = 0;
	int i4 = 0;
	int i;
	int j;
	gLesson *inner;
	inner = (gLesson *)lesson;
		
	// Hanzi Calcultation - TODO
	str1 = malloc(256);
	PA_CopyText(str1,"");
	str2 = malloc(256);
	PA_CopyText(str2,"");	
	str3 = malloc(256);
	PA_CopyText(str3,"");	
	str4 = malloc(256);
	PA_CopyText(str4,"");	
	for (i = 0; ct_linkedlist_getPos(inner->phrase,i) != NULL; i++) {
	   // For every phrase in the lesson...
	   str = ct_linkedlist_getPos(inner->phrase,i);
		for (j = 1; !PA_CompareText("",ct_HALhanzi_phrase_getWord(str,j)); j++) {
		   // For every word in the phrase, add it to its database
		   strword = ct_HALhanzi_phrase_getWord(str,j);
		   switch (ct_HALhanzi_word_lenHanzi(strword)) {
		      case 1:
		         if (strstr(str1,strword) == NULL) {
		            if (i1)
		            	strcat(str1," "); // 1st word
		            i1 = 1;
		         	strcat(str1,strword);
		      	}   
		         break;
		      case 2:
		         if (strstr(str2,strword) == NULL) {
		            if (i2)
		            	strcat(str2," "); // 1st word
		            i2 = 1;		            
		         	strcat(str2,strword);
		       	}  	
		         break;
		      case 3:
		         if (strstr(str3,strword) == NULL) {		         
		            if (i3)
		            	strcat(str3," "); // 1st word
		            i3 = 1;		            
		         	strcat(str3,strword);
		      	}   
		         break;
		      case 4:
		         if (strstr(str4,strword) == NULL) {
		            if (i4)
		            	strcat(str4," "); // 1st word
		            i4 = 1;		            
		         	strcat(str4,strword);
		       	}  	
		         break;		         
		   } 
		}   
	}			
	ct_linkedlist_addEnd(inner->hanzi,(void *)str1); 
	ct_linkedlist_addEnd(inner->hanzi,(void *)str2); 		
	ct_linkedlist_addEnd(inner->hanzi,(void *)str3); 		
	ct_linkedlist_addEnd(inner->hanzi,(void *)str4); 
}   


