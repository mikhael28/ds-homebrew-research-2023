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


/* "HW" Layer - Defines the structure of a lesson */
#ifndef _H_L_HW_
#define _H_L_HW_

#include "ct_linkedlist.h"

typedef struct slotLesson{
   char title[255];
	LList *hanzi;  // [str] hanzi contained in this lesson
	LList *intro;  // [str] introductory text
	LList *text;   // [str] lesson text
	LList *phrase; // [str] chinese phrases
	LList *tphrase;// [str] chinese phrase - translation
	LList *ntext;  // [int] text - phrase combination (Text)
	LList *nphrase;// [int] text - phrase combination (Phrase)
} gLesson;

/* Gets a Lesson */
void *ct_l_hw_createLesson();

/* Destroys a Lesson */
void ct_l_hw_destroyLesson(void *lesson);

/* Obtains the hanzi of a lesson, automatically */
void ct_l_hw_getHanzi(void *lesson);

#endif

