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
#include "ct_graphics.h"
#include "ct_smiley.h"
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include


#define INIPOSX 24
#define INIPOSY 152
#define MINPOSX 8
#define MAXPOSX 40
#define MINPOSY 144
#define MAXPOSY 160


/* Variables - Internal State */
////////////////////////////////

int smiley_state; // The actual state of smiley
int smiley_nextState; // The next state of smiley
int smiley_talking; // Smiley is talking (1) or not (0)
int smiley_inner_state;  // Internal state of a particular smiley
int smiley_var1;
int smiley_var2;

char smiley_text[255];  // Internal text used by the smiley
int smiley_text_wrote; // Indicate if all the text have been wrote
int smiley_text_loc; // What characters i am currently writing
int smiley_text_steps; // Time to wait for writing the next character
int smiley_text_stepsMax; // Time to wait for writing the next character
int smiley_text_sem; // I have a semaphore associated with the smiley
int smiley_text_time; // Time to wait 
int smiley_text_timeMax; // Time to wait - Max value 
int smiley_text_moodtalk; // Mood while talking
int smiley_text_moodwait; // Mood when finished talking
int smiley_text_id;  // For the Extended system

u16 smiley_face1_gfx;
u16 smiley_face2_gfx;
u16 smiley_face3_gfx;
int smiley_sprite;
int smileypal;

/* Internal Functions */ 
////////////////////////

// Function ct_smiley_reset
void ct_smiley_reset()
{
	// Empty the gfxs
	PA_DeleteGfx(1,smiley_face1_gfx);
	PA_DeleteGfx(1,smiley_face2_gfx);
	PA_DeleteGfx(1,smiley_face3_gfx);
	
} // end of ct_smiley_reset

// Function ct_smiley_assignface(int state)
void ct_smiley_assignface()
{
	// Resets
	ct_smiley_reset();
	// And assigns
	switch (smiley_state) {
		case SMILEYMOOD_ :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);		
			break;
		case SMILEYMOOD_NORMAL :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_normal_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);		
			break;
		case SMILEYMOOD_NORMALMOVE :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_normal_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);		
			break;
		case SMILEYMOOD_NORMALHAPPY :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_normal_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley_happy_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);				
			break;
		case SMILEYMOOD_HAPPY :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_happy_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);				
			break;
		case SMILEYMOOD_HAPPYJUMP :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_happy_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);				
			break;
		case SMILEYMOOD_UNHAPPY :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_unhappy_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);				
			break;
		case SMILEYMOOD_AMUSED :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_bigeye_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley_bigeyes_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);						
			break;
		case SMILEYMOOD_BLINK :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_bigeyes_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley_thinking_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);						
			break;
		case SMILEYMOOD_WINK :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_normal_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley_wink_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);						
			break;
		case SMILEYMOOD_ZOMBIE :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_dead_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);								
			break;
		case SMILEYMOOD_WHAT :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_what_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);			
			break;
		case SMILEYMOOD_THINKING :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_thinking_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			break;
		case SMILEYMOOD_AIYA :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_aiya_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			break;
		case SMILEYMOOD_AIYAAAA :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_aiya_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			break;			
		case SMILEYMOOD_SUSPICIOUS :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_looking_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);		
			break;
		case SMILEYMOOD_TALKING :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_normal_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley_talking_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);		
			break;
		case SMILEYMOOD_APPEAR :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_normal_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley_happy_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);		
			break;
		case SMILEYMOOD_DISAPPEAR :
			smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley_normal_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley_happy_Sprite, OBJ_SIZE_64X32, 1);
			smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);		
			break;
	}
} // end of ct_smiley_reset

/* Functions */ 
///////////////

void ct_smiley_setState(int state);
void ct_smiley_Manage();

// Function ct_smiley_checkNextState()
void ct_smiley_checkNextState() {
	if (smiley_nextState != SMILEYMOOD_)
		ct_smiley_setState(smiley_nextState);
} // end of ct_smiley_checkNextState()

// Function ct_smiley_init()
void ct_smiley_init()
{
	// TODO - Does not manage issue of double init.
	// Creates the basic graphics needed for smiley... sprite, gfxs done later
	smiley_face1_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
	smiley_face2_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
	smiley_face3_gfx = PA_CreateGfx(1,(void*)smiley__Sprite, OBJ_SIZE_64X32, 1);
	smileypal = ct_graphics_getPalette(1);
	PA_LoadSpritePal(1,smileypal,(void*)smiley_Pal);
	smiley_sprite = ct_graphics_getSprite(1);	
	PA_CreateSpriteFromGfx(1,smiley_sprite,smiley_face1_gfx,OBJ_SIZE_64X32,1,smileypal,
									INIPOSX,INIPOSY+64);
   PA_SetSpritePrio(1,smiley_sprite,3);
   
   // state
   ct_smiley_setState(SMILEYMOOD_); // Mood
	ct_smiley_setTextExtIni();			// Text & InnerMood
	
	// Empty the text things
	PA_CopyText(smiley_text,"");
	smiley_text_wrote = 0;
	smiley_text_loc = 0;
	smiley_text_sem = 0;
	smiley_text_time = 0;
	smiley_text_timeMax = 0;
} // end of ct_smiley_init(int screen)

/* Finishes smiley, frees resources associated with him (bye, smiley :_( )*/
void ct_smiley_finish()
{
   ct_smiley_reset();
}   

/* Gives Smiley a certain state, see the constants */
void ct_smiley_setState(int state)
{
	smiley_state = state;
	smiley_nextState = SMILEYMOOD_;
	smiley_inner_state = 0;
	ct_smiley_assignface();
	//ct_smiley_Manage(); // for initialization purposes
}

/* Gets the actual state of Smiley */
int ct_smiley_getState()
{
	return smiley_state;
}

/* Indicate what will be the next state of Smiley once he finishes the current state */
void ct_smiley_setNextState(int state)
{
	smiley_nextState = state;
}

/* Manages the State of Smiley - Internal State */
void smiley_manageState()
{
   int invar1;
   
	switch (smiley_state) {
		case SMILEYMOOD_ :
			break;
		case SMILEYMOOD_NORMAL :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load normal Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Final state... NextStateCheck
				ct_smiley_checkNextState();
			}		
			break;
		case SMILEYMOOD_NORMALMOVE :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load normal Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Choose to move left or right
				smiley_var1 = PA_RandMinMax(0,1);
				if (smiley_var1 == 0) 
					smiley_var1 = -1;
				else smiley_var1 = 1;
				// next state
				smiley_inner_state = 2;
			} else if (smiley_inner_state == 2) {
				// Move
				if (smiley_var1 == -1) {
				   if (PA_GetSpriteX(1,smiley_sprite) > MINPOSX)
				   	PA_SetSpriteX(1,smiley_sprite,PA_GetSpriteX(1,smiley_sprite) - 1);
				   else smiley_var1 = 1;
				} else {
				   if (PA_GetSpriteX(1,smiley_sprite) < MAXPOSX)
				   	PA_SetSpriteX(1,smiley_sprite,PA_GetSpriteX(1,smiley_sprite) + 1);
				   else smiley_var1 = -1;
				}
				// Test next state			   
				ct_smiley_checkNextState();
			}			   
			break;
		case SMILEYMOOD_NORMALHAPPY :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load normal Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Choose to move left or right
				smiley_var1 = PA_RandMinMax(0,1);
				if (smiley_var1 == 0) 
					smiley_var1 = -1;
				else smiley_var1 = 1;
				// next state
				smiley_inner_state = 2;
			} else if (smiley_inner_state == 2) {
				// Move ... or Jump?
				invar1 = PA_RandMinMax(1,100);
				if (invar1 == 1)  { // 1% chances of jumping
					PA_SetSpriteGfx(1,smiley_sprite,smiley_face2_gfx);
					smiley_var2 = -1;
					smiley_inner_state = 3;
				} else {			
				   // Normal move
					if (smiley_var1 == -1) {
				   	if (PA_GetSpriteX(1,smiley_sprite) > MINPOSX)
				   		PA_SetSpriteX(1,smiley_sprite,PA_GetSpriteX(1,smiley_sprite) - 1);
				   	else smiley_var1 = 1;
					} else {
				   	if (PA_GetSpriteX(1,smiley_sprite) < MAXPOSX)
				   		PA_SetSpriteX(1,smiley_sprite,PA_GetSpriteX(1,smiley_sprite) + 1);
				   	else smiley_var1 = -1;
					}
				}			
				// Test next state, anyways
				ct_smiley_checkNextState();
			} else if (smiley_inner_state == 3) { 		   		   
				if (smiley_var2 == -1) {
				  	if (PA_GetSpriteY(1,smiley_sprite) > MINPOSY)
				  		PA_SetSpriteY(1,smiley_sprite,PA_GetSpriteY(1,smiley_sprite) - 1);
				  	else smiley_var2 = 1;
				} else {
				  	if (PA_GetSpriteY(1,smiley_sprite) < INIPOSY)
				  		PA_SetSpriteY(1,smiley_sprite,PA_GetSpriteY(1,smiley_sprite) + 1);
				 	else {
					   smiley_inner_state = 2; // Move again
					   PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);
					}			
				}
				// Test next state, anyways
				ct_smiley_checkNextState();				
			}   
			break;
		case SMILEYMOOD_HAPPY :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load happy Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Final state... NextStateCheck
				ct_smiley_checkNextState();
			}				   
			break;
		case SMILEYMOOD_HAPPYJUMP :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load happy Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				smiley_var1 = -1;
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				if (smiley_var1 == -1) {
				  	if (PA_GetSpriteY(1,smiley_sprite) > MINPOSY)
				  		PA_SetSpriteY(1,smiley_sprite,PA_GetSpriteY(1,smiley_sprite) - 1);
				  	else smiley_var1 = 1;
				} else {
				  	if (PA_GetSpriteY(1,smiley_sprite) < INIPOSY)
				  		PA_SetSpriteY(1,smiley_sprite,PA_GetSpriteY(1,smiley_sprite) + 1);
				 	else 
					 	smiley_var1 = -1;
				}
				// Test next state, anyways
				ct_smiley_checkNextState();				
			}				   
		   
			break;
		case SMILEYMOOD_UNHAPPY :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load unhappy Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Final state... NextStateCheck
				ct_smiley_checkNextState();
			}				   		   
			break;
		case SMILEYMOOD_AMUSED :
			break;
		case SMILEYMOOD_BLINK :
			break;
		case SMILEYMOOD_WINK :
			break;
		case SMILEYMOOD_ZOMBIE :
			break;
		case SMILEYMOOD_WHAT :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load what Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Final state... NextStateCheck
				ct_smiley_checkNextState();
			}				   		   
			break;
		case SMILEYMOOD_THINKING :
			break;
		case SMILEYMOOD_AIYA :
			break;
		case SMILEYMOOD_AIYAAAA :
		      // Tremble!!!
		      invar1 = PA_RandMinMax(0,3);
				PA_SetSpriteX(1,smiley_sprite,INIPOSX - 2 + invar1);
		      invar1 = PA_RandMinMax(0,3);				
				PA_SetSpriteY(1,smiley_sprite,INIPOSY - 2 + invar1);						   
			break;
		case SMILEYMOOD_SUSPICIOUS :
			break;
		case SMILEYMOOD_TALKING :
			if (smiley_inner_state == 0) {
				// Puts him in the normal position, load normal Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);				
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);				
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
			   // Move lips (or not)
			   if (!PA_CompareText("",smiley_text)) {
			      if (smiley_text_loc < strlen(smiley_text)) {
			         // "Speak?" Open lips only in vowels
			         if (ct_utillib_isVowel(smiley_text[smiley_text_loc]))
			         	PA_SetSpriteGfx(1,smiley_sprite,smiley_face2_gfx);
			         else PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);
			      }   
			   }   
				// Test next state, anyways
				ct_smiley_checkNextState();
			}				   		   		   
			break;
		case SMILEYMOOD_APPEAR :	
			if (smiley_inner_state == 0) {
				// Puts him outside, load normal Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face2_gfx);	
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);							
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);
				PA_SetSpriteY(1,smiley_sprite,
					PA_GetSpriteY(1,smiley_sprite) + 10 + PA_GetSpriteLy(1,smiley_sprite) - 1);
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Jumps!
				if (PA_GetSpriteY(1,smiley_sprite) > MINPOSY) {
					PA_SetSpriteY(1,smiley_sprite,
						PA_GetSpriteY(1,smiley_sprite) - 1);
				} else {
					smiley_inner_state = 2;
				}
			} else if (smiley_inner_state == 2) {
				// Gravity makes its job...
				if (PA_GetSpriteY(1,smiley_sprite) < INIPOSY) {
					PA_SetSpriteY(1,smiley_sprite,
						PA_GetSpriteY(1,smiley_sprite) + 1);
				} else {
					// Puts new face, next state
					PA_SetSpriteGfx(1,smiley_sprite,smiley_face1_gfx);
					smiley_inner_state = 3;
				}			
			} else if (smiley_inner_state == 3) {
				// Final state... NextStateCheck
				ct_smiley_checkNextState();		
			}
			break;
		case SMILEYMOOD_DISAPPEAR :	
			if (smiley_inner_state == 0) {
				// Puts him inside, load normal Gfx
				PA_SetSpriteGfx(1,smiley_sprite,smiley_face2_gfx);	
				PA_SetSpriteX(1,smiley_sprite,INIPOSX);							
				PA_SetSpriteY(1,smiley_sprite,INIPOSY);
				// next state
				smiley_inner_state = 1;
			} else if (smiley_inner_state == 1) {
				// Jumps!... Down
				if (PA_GetSpriteY(1,smiley_sprite) < (INIPOSY + 10 + PA_GetSpriteLy(1,smiley_sprite) - 1)) {
					PA_SetSpriteY(1,smiley_sprite,
						PA_GetSpriteY(1,smiley_sprite) + 1);
				} else {
					smiley_inner_state = 2;
				}
			} else if (smiley_inner_state == 2) {
				// Final state... NextStateCheck
				ct_smiley_checkNextState();		
			}
			break;
	}
}

/* Manages the State of Smiley - Text State */
void smiley_manageText()
{
   if (!PA_CompareText("",smiley_text)) {
      //PA_OutputText(1,0,0,"%s - %d/%d",smiley_text,smiley_text_loc,strlen(smiley_text));
   	if (!ct_utillib_isASymbol(smiley_text[smiley_text_loc - 1]))
   		smiley_text_wrote = 1;      
      if (!smiley_text_wrote)
   		PA_BoxText(1, 0, 5, 18, 15, smiley_text, smiley_text_loc);
   	if (strlen(smiley_text) == smiley_text_loc) {
   	   // We have written the entire text
   	   smiley_text_wrote = 1;
      	// Check if new smiley-mood must be launch
      	if (smiley_text_time == smiley_text_timeMax) {
      	   if (smiley_text_moodwait != SMILEYMOOD_)
         		ct_smiley_setState(smiley_text_moodwait);
      	}   
      	// Decrease time
   		if (smiley_text_time > 0) {   	   
			   smiley_text_time--;
			   //if ((smiley_text_time == 0) && (smiley_text_sem == 1))
			   //	ct_utillib_signalSemaphore(); - Nope Others have to signal me!
			}			   
		} else {
	   	// Advance to the next letter
		   smiley_text_steps++;
		   if (smiley_text_steps == smiley_text_stepsMax) {
	   	   smiley_text_steps = 0;
	   		smiley_text_loc++;
		 	}  	
		}   
	}
}


char *smiley_createEmpty()
{
   int i;
   static char Text[255];
   PA_CopyText(Text,"");
   for (i = 0; i < 18; i++) {
      strcat(Text,"          ");
   }
	return Text;   
}

void smiley_formatText(char *Text)
{
   int len = strlen(Text);
   int i;
   int state = 0;
   for (i = 0; i < len; i++) {
      if ((state == 0) && (Text[i] == '@'))
      	state++;
      else if ((state == 1) && (Text[i] == '@'))
      	state++;
      else if ((state == 2) && ((Text[i] >= '1') || (Text[i] <= '0'))) {
         state = 0;
         Text[i-2] = '%';
         Text[i-1] = 'c';
   	} else state = 0;   
   }   
}      

/* Manages the State of Smiley */
void ct_smiley_Manage()
{
   smiley_manageState();
   smiley_manageText();
}   

/* Assigns a Text for the Smiley to say */
void ct_smiley_setText(char txt[255], int time, int steps, int sem)
{
   // Clean
   PA_BoxText(1, 0, 5, 18, 15, smiley_createEmpty(), 180); // (15 - 5) * (18 - 0)
   // Do!
   PA_CopyText(smiley_text,txt);
   smiley_formatText(smiley_text);
   smiley_text_loc = 1;
   
   if (time == -1)
   	smiley_text_time = strlen(smiley_text) * 3; // Negative? Time depends on text
   else if (time < -1)
   	smiley_text_time = 1; // Directly "ends" (1 for allowing to put the next smiley state)
   else smiley_text_time = time; // ...Including 0, although 0 is buggy- TODO

	smiley_text_wrote = 0;   
   smiley_text_timeMax = smiley_text_time;
   smiley_text_stepsMax = steps;
   smiley_text_steps = 0;
	smiley_text_sem = sem;
	if (smiley_text_sem == 1)
		ct_utillib_setSemaphore(); // Inc(1) The Semaphore
}

/* Ends the actual text said by Smiley */
void ct_smiley_endText()
{
	smiley_text_loc = strlen(smiley_text);
	smiley_text_time = smiley_text_timeMax;
}

/* Assigns a Mood for the smiley while talking */
void ct_smiley_setTextMood(int talking, int finished)
{
   smiley_text_moodtalk = talking;
   smiley_text_moodwait = finished;   
}   

/* Inits the Extended text system */
void ct_smiley_setTextExtIni()
{
   smiley_text_id = 0;
   smiley_text_moodtalk = smiley_state;
   smiley_text_moodwait = smiley_state;   
}

/* Writes according to the Extended text system */   
int ct_smiley_setTextExt(int type, int lang, int time, int steps, int sem) 
{
   if (!ct_smiley_getEndText()) {
   	return 1; // Writing Text...
   } else {
      if (  (!smiley_text_sem) || // No semaphore
				((smiley_text_sem) && (!ct_utillib_waitSemaphore())) // Semaphore, but no need to wait
			) {
   		if (PA_CompareText("", ct_smiley_getText(type, smiley_text_id, lang))) {
				smiley_text_id = 0;   	   
   			return 0; // No more Text!
	   	} else {
   		   ct_smiley_setState(smiley_text_moodtalk);
      		ct_smiley_setText(ct_smiley_getText(type, smiley_text_id, lang), time, steps, sem);
      		smiley_text_id++;
      		return 2; // New Text written
	   	}   
 		} else 
			return 3; // Waiting for the semaphore...
   }   
}   

/* Writes according to the Extended text system - given a certain string */   
int ct_smiley_setTextExt2(char *str, int time, int steps, int sem) 
{
   if (!ct_smiley_getEndText()) {
   	return 1; // Writing Text...
   } else {
      if (  (!smiley_text_sem) || // No semaphore
				((smiley_text_sem) && (!ct_utillib_waitSemaphore())) // Semaphore, but no need to wait
			) {      
	   	if (PA_CompareText("", str)) {
				smiley_text_id = 0;   	   
   			return 0; // No more Text!
	   	} else {
   		   ct_smiley_setState(smiley_text_moodtalk);
      		ct_smiley_setText(str, time, steps, sem);
      		smiley_text_id++;
	      	return 2; // New Text written
   		}   
  		} else 
			return 3; // Waiting for the semaphore...  		
   }   
}

/* Gets the internal text system value - for setTextExt2 */
int ct_smiley_getTextExt()
{
   return smiley_text_id;
}   

/* Returns if the text have ended */
int ct_smiley_getEndText()
{
   if ((strlen(smiley_text) == smiley_text_loc) && (smiley_text_time == 0))
   	return 1;
   else return 0;
}      

/* Get Text said by Smiley*/
char *ct_smiley_getText(int type, int id, int language)
{
   static char res[255];
   PA_CopyText(res,"");
   switch(language) {
      case LANG_ENG:
         switch(type) {
            case SMILEY_NEWUSER_1:
               if (id==0)
               	sprintf(res,"HELLO!.");
               if (id==1)
               	sprintf(res,"MY NAME IS LUO RUIMENG, AND I WELCOME YOU TO DSZHONGWEN!");
               if (id==2)
               	sprintf(res,"THIS IS A SMALL HOMEBREW APPLICATION/GAME FOR LEARNING (A BIT OF) MANDARIN CHINESE - BOTH SPOKEN AND WRITTEN.");
               if (id==3)
               	sprintf(res,"THIS IS NOT A COURSE BY ITSELF - CONSIDER THIS AS A TOOL FOR AIDING YOU IN LEARNING THE BASICS OF CHINESE.");
               break;            
            case SMILEY_NEWUSER_2:
               if (id==0)
               	sprintf(res,"BY THE WAY, WHAT IS YOUR NAME?");
               break;                           
            case SMILEY_NEWUSER_3:
               if (id==0)
               	sprintf(res,"%s... OK!",data_gameData[data_profile].name);
               break;                                          
            case SMILEY_NEWUSER_4:
               if (id==0)
               	sprintf(res,"ALSO... ARE YOU LEFT-HANDED OR RIGHT-HANDED?");
               break;                                                         
            case SMILEY_NEWUSER_5:
               if (id==0)
               	sprintf(res,"GOT IT!");
               break;
            case SMILEY_NEWUSER_6:
               if (id==0)
               	sprintf(res,"(BY THE WAY, NOT ALL HANZI/PINYIN ARE AVAILABLE, AND EXPECT THE DICTIONARY TO NOT BE PERFECT. WARNED YOU, MY CREATORS ARE LAZY :-) )");
               if (id==1)
               	sprintf(res,"NOW, IT IS TIME FOR YOU TO ENJOY THIS LITTLE 'GAME'!!!\n AND IF YOU ARE CHINESE, HOPE YOU WILL LEARN SOME ENGLISH!");
               break;                                                         
            case SMILEY_HI_1:
               if (id==0)
               	sprintf(res,"HI, @@1%s! @@9GLAD TO SEE YOU AGAIN!",data_gameData[data_profile].name);
               break;
            case SMILEY_HI_2:
               if (id==0)
               	sprintf(res,"HELLO, @@1%s! @@9READY TO LEARN (A BIT OF) CHINESE?",data_gameData[data_profile].name);
               break;
            case SMILEY_HI_3:
               if (id==0)
               	sprintf(res,"WELCOME BACK, @@1%s! @@9 HOPE YOU WILL ENJOY THIS LITTLE GAME!",data_gameData[data_profile].name);
               break;               
            case SMILEY_HI_INTRO:
               if (id==0)
               	sprintf(res,"THIS IS THE MAIN SCREEN. IN THIS SCREEN, YOU CAN CHOOSE WHATEVER LESSON YOU WANT TO TAKE,...");
               if (id==1)
               	sprintf(res,"...AND YOU CAN READ (LISTEN) TO THE CONTENTS OF THE LESSON, REVIEW THE IDEOGRAMS, AND CHECK YOUR GRADES TOO!.");               	
               break;          
            case SMILEY_HI_REC:					                    
               if (id==0)
               	sprintf(res,"YOU DO NOT KNOW WHAT TO DO? I'LL TELL YOU AGAIN WHAT YOU CAN DO IN THIS SCREEN:");  
					break;             
            case SMILEY_HI_EXPLAIN:
               if (id==0)
               	sprintf(res,"TOUCH THE BIG BUTTON FOR CHOOSING WHICHEVER LESSON YOU WANT TO TAKE,...");               
               if (id==1)
               	sprintf(res,"...TOUCH @@2%s @@9 IN ORDER TO READ (OR LISTEN!) THE ACTUAL LESSON,...",ct_const_getText(TXT_MM_LESSON, LANG_ENG));
               if (id==2)
               	sprintf(res,"...TOUCH @@2%s @@9 FOR 'PLAYING' WITH THE MINIGAMES... ER... EXAMS,...",ct_const_getText(TXT_MM_EXAMS, LANG_ENG));
               if (id==3)
               	sprintf(res,"...TOUCH @@2%s @@9 FOR READING THE IDEOGRAMS IN THE ACTUAL LESSON,...",ct_const_getText(TXT_MM_IDEOGRAMS, LANG_ENG));
               if (id==4)
               	sprintf(res,"...AND TOUCH @@2%s @@9 TO READ YOUR GRADES IN THE ACTUAL LESSON.",ct_const_getText(TXT_MM_GRADES, LANG_ENG));               	
               if (id==5)
               	sprintf(res,"AH! THERE IS A LITTLE DICTIONARY - PRONUNCIATION GUIDE IN THE CONF. SCREEN. CHECK IT!");               	               	
               break;                              
            case SMILEY_HI_END:
               if (id==0)
               	sprintf(res,"THAT'S ALL. ENJOY!");   
					break;                           
            case SMILEY_HI_AGAIN:
               if (id==0)
               	sprintf(res,"WELCOME BACK TO THE MAIN SCREEN, @@2%s!",data_gameData[data_profile].name);                              
               break;
            case SMILEY_NOLESSON_1:
               if (id==0)
               	sprintf(res,"HI, @@2%s! @@9 THIS IS THE LESSON SCREEN!",data_gameData[data_profile].name);                              
               break;
            case SMILEY_NOLESSON_2:               
               if (id==0)
               	sprintf(res,"IN THIS SCREEN, YOU CAN BE ABLE TO LEARN A BIT OF CHINESE - GRAMMAR, PRONUNCIATION, AND THE IDEOGRAMS.");
               break;               
            case SMILEY_NOLESSON_3:
               if (id==0)
               	sprintf(res,"HOWEVER, YOU HAVE NOT SELECTED ANY LESSON, THUS, I CANNOT TEACH YOU ANYTHING... YET.");                              
               	break;
            case SMILEY_NOLESSON_4:
               if (id==0)
               	sprintf(res,"SO... GO BACK! SELECT A LESSON USING THE BIG BUTTON, AND RETURN HERE!.");                              
               	break;            
            case SMILEY_NOLESSON_5:
               if (id==0)
               	sprintf(res,"WHAT ARE YOU WAITING FOR? AN EASTER EGG?.");                              
               	break;      
            case SMILEY_NOLESSON_6:
               if (id==0)
               	sprintf(res,"NO EASTER EGGS HERE! GO BACK!.");                              
               	break;     												   	
            case SMILEY_CFG:
               if (id==0)
               	sprintf(res,"IN THIS SCREEN YOU CAN CHANGE THE CONFIGURATION OF DSZHONGWEN.");                              
               	break;     												   	
            case SMILEY_CFG_SPEED:
               if (id==0)
               	sprintf(res,"CHOOSE THE SPEED OF THE ANIMATED IDEOGRAMS: FROM 1 (FAST) TO 5 (SLOW).");                              
               	break;     												   	
            case SMILEY_CFG_HAND:
               if (id==0)
               	sprintf(res,"CHOOSE THE HAND YOU USE FOR WRITING THE IDEOGRAMS.");                              
               	break;     												   	

         }   
         smiley_formatText(res);
         return res; // no need to break ;-)
      case LANG_SPA:
         switch(type) {
            case SMILEY_NEWUSER_1:
               if (id==0)
               	sprintf(res,"HOLA!.");
               if (id==1)
               	sprintf(res,"MI NOMBRE ES LUO RUIMENG, Y TE DOY LA BIENVENIDA A DSZHONGWEN!");
               if (id==2)
               	sprintf(res,"ESTA ES UNA PEQUENYA APLICACION HOMEBREW PARA APRENDER (UN POCO DE) CHINO MANDARIN - TANTO HABLADO COMO ESCRITO.");
               if (id==3)
               	sprintf(res,"ESTO NO ES UN CURSO DE POR SI - CONSIDERA ESTO COMO UNA HERRAMIENTA PARA AYUDARTE A APRENDER LOS CONCEPTOS BASICOS DE CHINO.");
               break;            
            case SMILEY_NEWUSER_2:
               if (id==0)
               	sprintf(res,"POR CIERTO, COMO TE LLAMAS?");
               break;                           
            case SMILEY_NEWUSER_3:
               if (id==0)
               	sprintf(res,"%s... OK!",data_gameData[data_profile].name);
               break;                                          
            case SMILEY_NEWUSER_4:
               if (id==0)
               	sprintf(res,"TAMBIEN... ERES DIESTRO O ZURDO?");
               break;                                                         
            case SMILEY_NEWUSER_5:
               if (id==0)
               	sprintf(res,"DE ACUERDO!");
               break;
            case SMILEY_NEWUSER_6:
               if (id==0)
               	sprintf(res,"(POR CIERTO, NO TODOS LOS HANZI/PINYIN ESTAN DISPONIBLES, Y EL DICCIONARIO ESTA EN INGLES. DISCULPA, MIS CREADORES SON UN POCO VAGOS :-) )");               
               if (id==1)
               	sprintf(res,"AHORA, ESPERO QUE DISFRUTES CON ESTE 'JUEGO'!!!\n Y SI ERES CHINO, ESPERO QUE TE SEA UTIL PARA APRENDER UN POCO DE ESPANYOL!");
               break;                                                         
            case SMILEY_HI_1:
               if (id==0)
               	sprintf(res,"HOLA, @@1%s! @@9ME ALEGRA VERTE OTRA VEZ!",data_gameData[data_profile].name);
               break;
            case SMILEY_HI_2:
               if (id==0)
               	sprintf(res,"HOLA, @@1%s! @@9LISTO PARA APRENDER (UN POCO DE) CHINO?",data_gameData[data_profile].name);
               break;
            case SMILEY_HI_3:
               if (id==0)
               	sprintf(res,"BIENVENIDO, @@1%s! @@9 ESPERO QUE DISFRUTES DE ESTE PEQUENYO JUEGO!",data_gameData[data_profile].name);
               break;               
            case SMILEY_HI_INTRO:
               if (id==0)
               	sprintf(res,"ESTE ES EL MENU PRINCIPAL. AQUI, PUEDES ELEGIR LA LECCION QUE QUIERAS APRENDER,...");
               if (id==1)
               	sprintf(res,"...Y PUEDES LEER (ESCUCHAR) LOS CONTENIDOS DE LA LECCION, REVISAR LOS IDEOGRAMAS, Y MIRAR TUS NOTAS!.");               	
               break;          
            case SMILEY_HI_REC:					                    
               if (id==0)
               	sprintf(res,"NO SABES QUE HACER? VENGA, TE LO EXPLICARE OTRA VEZ...");  
					break;             
            case SMILEY_HI_EXPLAIN:
               if (id==0)
               	sprintf(res,"TOCA EN EL BOTON GRANDE PARA ELEGIR LA LECCION QUE QUIERAS APRENDER,...");               
               if (id==1)
               	sprintf(res,"...TOCA @@2%s @@9 PARA LEER (O ESCUCHAR!) LA LECCION ACTUAL,...",ct_const_getText(TXT_MM_LESSON, LANG_SPA));
               if (id==2)
               	sprintf(res,"...TOCA @@2%s @@9 PARA 'JUGAR' CON LOS MINIJUEGOS... ESTO... EXAMENES,...",ct_const_getText(TXT_MM_EXAMS, LANG_SPA));
               if (id==3)
               	sprintf(res,"...TOCA @@2%s @@9 PARA REVISAR LOS IDEOGRAMAS DE LA LECCION ACTUAL,...",ct_const_getText(TXT_MM_IDEOGRAMS, LANG_SPA));
               if (id==4)
               	sprintf(res,"...Y TOCA @@2%s @@9 PARA VER TUS NOTAS DE LOS EXAMENES DE LA LECCION ACTUAL.",ct_const_getText(TXT_MM_GRADES, LANG_SPA));               	
               if (id==5)
               	sprintf(res,"AH! HAY UN PEQUENYO DICCIONARIO - GUIA DE PRONUNCIACION EN EL MENU DE CONFIGURACION. PRUEBALO!");               	               	               	
               break;                              
            case SMILEY_HI_END:
               if (id==0)
               	sprintf(res,"ESO ES TODO. DISFRUTA!");   
					break;                           
            case SMILEY_HI_AGAIN:
               if (id==0)
               	sprintf(res,"BIENVENIDO OTRA VEZ AL MENU PRINCIPAL, @@2%s!",data_gameData[data_profile].name);                              
               break;
            case SMILEY_NOLESSON_1:
               if (id==0)
               	sprintf(res,"HOLA, @@2%s! @@9 ESTA ES LA PANTALLA DE LA LECCION!",data_gameData[data_profile].name);                              
               break;
            case SMILEY_NOLESSON_2:               
               if (id==0)
               	sprintf(res,"AQUI, PUEDES APRENDER UN POCO DE CHINO - GRAMATICA, PRONUNCIACION, Y LOS IDEOGRAMAS.");
               break;               
            case SMILEY_NOLESSON_3:
               if (id==0)
               	sprintf(res,"SIN EMBARGO, NO HAS SELECCIONADO NINGUNA LECCION, POR LO QUE NO PUEDO ENSENYARTE NADA... AUN.");                              
               	break;
            case SMILEY_NOLESSON_4:
               if (id==0)
               	sprintf(res,"ASI PUES... VUELVE AL MENU PRINCIPAL! ELIGE UNA LECCION TOCANDO EL BOTON GRANDE, Y VUELVE AQUI!.");                              
               	break;            
            case SMILEY_NOLESSON_5:
               if (id==0)
               	sprintf(res,"A QUE ESTAS ESPERANDO... A UN EASTER EGG?.");                              
               	break;      
            case SMILEY_NOLESSON_6:
               if (id==0)
               	sprintf(res,"AQUI NO HAY 'HUEVOS DE PASCUA'! VUELVE AL MENU PRINCIPAL!.");                              
               	break;     												   	
            case SMILEY_CFG:
               if (id==0)
               	sprintf(res,"AQUI PUEDES MODIFICAR LA CONFIGURACION DE DSZHONGWEN.");                              
               	break;     												   	
            case SMILEY_CFG_SPEED:
               if (id==0)
               	sprintf(res,"ELIGE LA VELOCIDAD DE LOS IDEOGRAMAS ANIMADOS: DE 1 (RAPIDO) A 5(LENTO).");                              
               	break;     												   	
            case SMILEY_CFG_HAND:
               if (id==0)
               	sprintf(res,"ELIGE SI ERES DIESTRO O ZURDO, PARA ASI ESCRIBIR LOS IDEOGRAMAS MAS COMODAMENTE.");                              
               	break;     												   	               	
         }   
         smiley_formatText(res);
         return res; // no need to break ;-)
   }   
   return res; // No Spanish, no english? O_o
}

void ct_smiley_debug(char *debugtext)
{
	PA_BoxText(1, 0, 5, 18, 15, debugtext, 255);
}

