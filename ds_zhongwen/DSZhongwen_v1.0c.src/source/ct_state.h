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


/* Game State - Stores the Actual State of the Game */
#ifndef _H_STATE_
#define _H_STATE_

/* Inits the state system */
void ct_state_init();

/* Get the actual state of the Game */
int ct_state_getState();

/* Get the next state of the Game. Mostly for debug purposes */
int ct_state_getNextState();

/* Assign the actual state of the game, but does not change it */
void ct_state_assignState(int state);

/* Returns 1 if the state has changed. 0 otherwise */
int ct_state_newState();

/* Updates the state for the new, previously assigned state */
void ct_state_updateState();

/* Assigns var. - String */
void ct_state_var_setStr(char *str);

/* Gets var. - String */
char *ct_state_var_getStr();

/* Assigns var. - Int */
void ct_state_var_setInt(int who, int val);

/* Gets var. - Int */
int ct_state_var_getInt(int who);

/* Resets Array - Arr. Int */
void ct_state_var_resetArrInt();

/* Assigns var. - Arr. Int */
void ct_state_var_addArrInt(int val);   

/* Gets var. - lenght Arr. Int */
int ct_state_var_lenArrInt();

/* Gets var. - Arr. Int */
int ct_state_var_getArrInt(int pos);

/* Test for a val - Arr. Int */
int ct_state_var_inArrInt(int val);

#endif
