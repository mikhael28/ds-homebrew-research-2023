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


/* Lesson Layer - Access the lesson subsystem */
#ifndef _H_LESSON_
#define _H_LESSON_

/* init - Starts the lesson subsystem */
void ct_lesson_init();

/* read - Get a certain lesson */
void ct_lesson_read(int type, int number);

/* count - Returns the number of lessons of a certain type */
int ct_lesson_count(int type);

/* get a certain word */
char *ct_lesson_askWord(int size, int number);

/* get the number of distinct hanzi of a certain size in the database */
int ct_lesson_lenWords(int size);

/* get a certain phrase */
char *ct_lesson_askPhrase(int which);

/* get the number of phrases, for the basic */
int ct_lesson_getNumPhraseBasic();

/* get a translation of a certain phrase */
char *ct_lesson_askTranslationPhrase(int which);

/* Returns a string with the header of the actual lesson */
char *ct_lesson_strHeader();

/* Returns a string with the intro text of the actual lesson */
char *ct_lesson_strIntro(int i);

/* Returns a string with the text of the actual lesson */
char *ct_lesson_strText(int i);

/* Returns the number of texts used in the actual lesson (grammar) */
int ct_lesson_getNumText(int i);

/* Returns the idx of the text used in the actual lesson (grammar) */
int ct_lesson_getIdxText(int i);

/* Returns the number of phrases used in the actual lesson (grammar) */
int ct_lesson_getNumPhrase(int i);

/* Returns the idx of the phrases used in the actual lesson (grammar) */
int ct_lesson_getIdxPhrase(int i);

/* Returns the median of all the exams */
int ct_lesson_getMedExams(int ltype, int lnum);

#endif
