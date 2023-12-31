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
#include "ct_const.h" // Constants of the game

/* Variables */
int lc_state = GAME_;
int lc_newstate = GAME_;
char lc_str[255];
int lc_val[16];
int lc_arrval[16];
int lc_arrvallen;

/* Functions */

/* Resets Array - Arr. Int */
void ct_state_var_resetArrInt()
{
   int i;
   
	for (i = 0; i < 16; i++) {
		lc_arrval[i] = -1;
   }		
   lc_arrvallen = 0;
}   

void state_reset() {
	int i;
	PA_CopyText(lc_str,"");
	for (i = 0; i < 16; i++) {
		lc_val[i] = 0;
   }		
   ct_state_var_resetArrInt();
}   

void ct_state_init()
{
   state_reset();
}

// Function: ct_state_getState()
int ct_state_getState()
{
  return lc_state;
} // end of ct_state_getState() 

int ct_state_getNextState()
{
  return lc_newstate;
} // end of ct_state_getState() 

// Function: ct_state_assignState(int state)
void ct_state_assignState(int state)
{
  lc_newstate = state;
} // end of ct_state_assignState(int state)

// Function: ct_state_newState()
int ct_state_newState()
{
	if (lc_newstate == GAME_)
		return 0;
	else
		return 1;
} // end of ct_state_newState()

// Function: ct_state_updateState()
void ct_state_updateState()
{
  lc_state = lc_newstate;
  lc_newstate = GAME_;
} // end of ct_state_updateState()

/* Assigns var. - String */
void ct_state_var_setStr(char *str)
{
	PA_CopyText(lc_str,str);
}

/* Gets var. - String */
char *ct_state_var_getStr()
{
   return lc_str;
}   

/* Assigns var. - Int */
void ct_state_var_setInt(int who, int val)
{
   if ((who >=0) && (who < 16))
   	lc_val[who] = val;
}   

/* Gets var. - Int */
int ct_state_var_getInt(int who)
{
   if ((who >=0) && (who < 16))
   	return lc_val[who];
   else return -1;
}

/* Assigns var. - Arr. Int */
void ct_state_var_addArrInt(int val)
{
   if ((lc_arrvallen >=0) && (lc_arrvallen < 16)) {
   	lc_arrval[lc_arrvallen] = val;
   	lc_arrvallen++;
   }  	
}   

/* Gets var. - lenght Arr. Int */
int ct_state_var_lenArrInt()
{
   return lc_arrvallen;
}   

/* Gets var. - Arr. Int */
int ct_state_var_getArrInt(int pos)
{
   if ((pos >=0) && (pos < 16))
   	return lc_arrval[pos];
   else return -1;
}   

/* Test for a val - Arr. Int */
int ct_state_var_inArrInt(int val)
{
   int i;
   for (i=0;i<ct_state_var_lenArrInt();i++) {
      if (ct_state_var_getArrInt(i) == val)
      	return 1;
   }  
	return 0; 
}

