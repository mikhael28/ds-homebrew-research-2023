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
#include "ct_HALhanzi.h"
#include "ct_l_gram.h"
#include "ct_l_hanzi.h"
#include "ct_l_listen.h"
#include "ct_l_hanziattack.h"
#include "ct_l_hw.h"

#include "gfx/all_gfx.h" // PAGfx Include

gLesson *lesson;

/* init - Starts the lesson subsystem */
void ct_lesson_init()
{
   lesson = NULL;
}   


/* read - Get a certain lesson */
void ct_lesson_read(int type, int number)
{
   if (lesson != NULL)
   	ct_l_hw_destroyLesson(lesson);
   lesson = NULL;
   if (type != LESSON_) {
	   lesson = ct_l_hw_createLesson();
		switch(type) {
		   case LESSON_HANZI:
		      ct_l_hanzi_initTrans(number,data_gameData[data_profile].language);
		      ct_l_hanzi_get(number,lesson);
		      ct_l_hanzi_endTrans(number);
		      break;
		   case LESSON_GRAMM:
		      ct_l_gram_initTrans(number,data_gameData[data_profile].language);
		      ct_l_gram_get(number,lesson);
		      ct_l_gram_endTrans(number);
		      break;	      
		   case LESSON_LISTEN:
		      ct_l_listen_initTrans(number,data_gameData[data_profile].language);
		      ct_l_listen_get(number,lesson);
		      ct_l_listen_endTrans(number);
		      break;	      
		   case LESSON_HANZIATTACK:
		      ct_l_hanziattack_initTrans(number,data_gameData[data_profile].language);
		      ct_l_hanziattack_get(number,lesson);
		      ct_l_hanziattack_endTrans(number);
		      break;		      
		}
	}	
}

/* count - Returns the number of lessons of a certain type */
int ct_lesson_count(int type)
{
   switch (type) {
      case LESSON_HANZI : return ct_l_hanzi_getLessons();
      case LESSON_GRAMM : return ct_l_gram_getLessons();
      case LESSON_LISTEN : return ct_l_listen_getLessons();
      case LESSON_HANZIATTACK : return ct_l_hanziattack_getLessons();      
      case LESSON_ : return 0;
   }   
   return 0;
}

/* get a certain word from the database */
char *ct_lesson_askWord(int size, int number)
{
   if (lesson == NULL)
   	return "";
   
   if (ct_linkedlist_getPos(lesson->hanzi,size-1) == NULL)
   	return "";
   
   return ct_HALhanzi_phrase_getWord(
					ct_linkedlist_getPos(lesson->hanzi,size-1),
			   	number
				);
} 

/* get the number of distinct hanzi of a certain size in the database */
int ct_lesson_lenWords(int size)
{
   if (lesson == NULL) {
     	return 0;
	}   
   
   if (ct_linkedlist_getPos(lesson->hanzi,size-1) == NULL)
   	return 0;
   
   return ct_HALhanzi_phrase_lenWord(
					ct_linkedlist_getPos(lesson->hanzi,size-1)
				);
}

/* get a certain phrase */
char *ct_lesson_askPhrase(int which)
{
   if (lesson == NULL)
   	return "";

   if (ct_linkedlist_getPos(lesson->phrase,which-1) == NULL)
   	return "";
   
   return ct_linkedlist_getPos(lesson->phrase,which-1);
}

/* get the number of phrases, for the basic */
int ct_lesson_getNumPhraseBasic()
{
   if (lesson == NULL)
   	return "";
   
   return ct_linkedlist_len(lesson->phrase);
} 

/* get a translation of a certain phrase */
char *ct_lesson_askTranslationPhrase(int which)
{
   if (lesson == NULL)
   	return "";
   	
   if (ct_linkedlist_getPos(lesson->tphrase,which-1) == NULL)
   	return "";
   
   return ct_linkedlist_getPos(lesson->tphrase,which-1);
} 

/* Returns a string with the header of the actual lesson */
char *ct_lesson_strHeader()
{   
   if (data_lessontype != LESSON_) {
      return lesson->title;
   } else {
	   if (data_gameData[data_profile].language == LANG_ENG)
	   	return "General Lesson";
	   else return "Leccion General";
	}		   
}

/* Returns a string with the intro text of the actual lesson */
char *ct_lesson_strIntro(int i)
{   
   if (ct_linkedlist_getPos(lesson->intro,i) == NULL)
   	return "";
   else return ct_linkedlist_getPos(lesson->intro,i);
}

/* Returns a string with the text of the actual lesson */
char *ct_lesson_strText(int i)
{   
   if (ct_linkedlist_getPos(lesson->text,i) == NULL)
   	return "";
   else return ct_linkedlist_getPos(lesson->text,i);
}

/* Returns the number of texts used in the actual lesson (grammar) */
int ct_lesson_getNumText(int i)
{
   int *val;
   val = ct_linkedlist_getPos(lesson->ntext,i);
   if (val == NULL)
   	return -1;
   else return *val;
}   

/* Returns the idx of the text used in the actual lesson (grammar) */
int ct_lesson_getIdxText(int i)
{
   int idx = 0;
   int it;
   int val;
   
   for (it = 0; it < i; it++) {
      val = ct_lesson_getNumText(it);
      if (val != -1)
   		idx += val;
	}   
	
	return idx;
}   

/* Returns the number of phrases used in the actual lesson (grammar) */
int ct_lesson_getNumPhrase(int i)
{
   int *val;
   val = ct_linkedlist_getPos(lesson->nphrase,i);
   if (val == NULL)
   	return -1;
   else return *val;
}   

/* Returns the idx of the phrases used in the actual lesson (grammar) */
int ct_lesson_getIdxPhrase(int i)
{
   int idx = 0;
   int it;
   int val;
   
   for (it = 0; it < i; it++) {
      val = ct_lesson_getNumPhrase(it);
      if (val != -1)
   		idx += val;
	} 
	
	return idx;  
}

/* Returns the median of all the exams */
int ct_lesson_getMedExams(int ltype, int lnum) {
   int val = 0;
   int i;
   
   
   for (i = 0; i < SUBGAME_MAXREALEXAMS ; i++) {
	  val += data_gameData[data_profile].lessongrades[ltype][lnum][i];
	}	
	
	if ((val > 0) && (val < SUBGAME_MAXREALEXAMS))
		return 1; // I did, at least, an exam...
		
   val = val / SUBGAME_MAXREALEXAMS;
	return val;
}   
