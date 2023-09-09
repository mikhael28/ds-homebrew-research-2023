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


/* Smiley - your faithful companion ^_^ */
#ifndef _H_SMILEY_
#define _H_SMILEY_

/* Constants */
//-------------

#define SMILEY_NEWUSER_1 1
#define SMILEY_NEWUSER_2 2
#define SMILEY_NEWUSER_3 3
#define SMILEY_NEWUSER_4 4
#define SMILEY_NEWUSER_5 5
#define SMILEY_NEWUSER_6 6
#define SMILEY_HI_1 10
#define SMILEY_HI_2 11
#define SMILEY_HI_3 12
#define SMILEY_HI_INTRO 13
#define SMILEY_HI_REC 14
#define SMILEY_HI_EXPLAIN 15
#define SMILEY_HI_END 16
#define SMILEY_HI_AGAIN 17
#define SMILEY_NOLESSON_1 20
#define SMILEY_NOLESSON_2 21
#define SMILEY_NOLESSON_3 22
#define SMILEY_NOLESSON_4 23
#define SMILEY_NOLESSON_5 24
#define SMILEY_NOLESSON_6 25
#define SMILEY_CFG 30
#define SMILEY_CFG_SPEED 31
#define SMILEY_CFG_HAND 32


/* Functions */
//-------------

/* Creates ^_^, put him (it's a him, 'cause it's me :-P) "hidden" in the upper screen  */
void ct_smiley_init();

/* Finishes smiley, frees resources associated with him (bye, smiley :_( )*/
void ct_smiley_finish();

/* Gives Smiley a certain state, see the constants */
void ct_smiley_setState(int state);

/* Gets the actual state of Smiley */
int ct_smiley_getState();

/* Indicate what will be the next state of Smiley once he finishes the current state */
void ct_smiley_setNextState(int state);

/* Manages the State of Smiley */
void ct_smiley_Manage();

/* Assigns a Text to the Smiley to say */
void ct_smiley_setText(char txt[255], int time, int steps, int sem);

/* Ends the actual text said by Smiley */
void ct_smiley_endText();

/* Assigns a Mood for the smiley while talking */
void ct_smiley_setTextMood(int talking, int finished);

/* Returns if the text have ended */
int ct_smiley_getEndText();

/* Inits the Extended text system */
void ct_smiley_setTextExtIni();

/* Writes according to the Extended text system */   
int ct_smiley_setTextExt(int type, int lang, int time, int steps, int sem);

/* Writes according to the Extended text system - given a certain string */   
int ct_smiley_setTextExt2(char *str, int time, int steps, int sem);

/* Gets the internal text system value - for setTextExt2 */
int ct_smiley_getTextExt();

/* Get Text said by Smiley*/
char *ct_smiley_getText(int type, int id, int language);

/* - INTERNAL - Overrides text system, inputs debug */
void ct_smiley_debug(char *debugtext);
#endif
