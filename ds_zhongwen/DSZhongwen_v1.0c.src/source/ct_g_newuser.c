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
#include "ct_state.h"
#include "ct_button.h"
#include "ct_smiley.h"
#include "ct_header.h"
#include "ct_textscroll.h"
#include "ct_utillib.h"
#include "ct_lesson.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// No internal state

/* Variables - Internal State */
////////////////////////////////

int dw_newuser_button_next;
int dw_newuser_button_text;
int dw_newuser_button_left;
int dw_newuser_button_right;
int dw_newuser_button_hidden;
int newuser_state;
int newuser_substate;

char newuser_text[MAX_USERNAME];
int newuser_textsize;

/* Internal Functions */ 
////////////////////////

void g_newuser_nextText(){
   if (ct_utillib_waitSemaphore()) {
      ct_button_bh_blipContentStop(dw_newuser_button_next);
   	ct_utillib_signalSemaphore();
   	ct_smiley_endText(data_gameData[data_profile].name,newuser_text);
	}   
}

void g_newuser_nextHand(){
   // Next State
	newuser_substate++;
	ct_button_addHandler_Click(dw_newuser_button_next,g_newuser_nextText);   
	// Manage buttons
   ct_button_hide(dw_newuser_button_left);
	ct_button_hide(dw_newuser_button_right);
	// No more blips
	ct_button_bh_blipContentStop(dw_newuser_button_next);
}

void g_newuser_nextKeyboard(){
   // Next State
	newuser_substate++;
	ct_button_addHandler_Click(dw_newuser_button_next,g_newuser_nextText);
	// No more blip
   ct_button_bh_blipContentStop(dw_newuser_button_next);
   // Do
	PA_CopyText(data_gameData[data_profile].name, newuser_text);
	ct_button_Text(dw_newuser_button_text,"",3);
	// Keyboard out
	PA_KeyboardOut();
}

void g_newuser_selectHand(){
   // Hide
   ct_button_changetype(dw_newuser_button_left,BTN_FIELD);
   ct_button_changetype(dw_newuser_button_right,BTN_FIELD);
   // Show
	if (ct_button_getVariable(ct_button_getHandler()) == CONF_LEFTHANDED) {
	   ct_button_changetype(dw_newuser_button_left,BTN_FIELDSEL);
	   
	} else {
	   ct_button_changetype(dw_newuser_button_right,BTN_FIELDSEL);
	}	
	// Assign
	data_gameData[data_profile].hand = ct_button_getVariable(ct_button_getHandler());	   	
}

/* Functions */ 
///////////////

void ct_g_newuser_Start()
{
	// Inits the Graphic mode
	ct_graphics_initGraphicMode(SCREENMODE_INPUT);
	
	/* Graphics */
	// Headers, buttons, textscroll
	ct_header_init();
	ct_button_init();
	ct_textscroll_init();
	ct_textscroll_openscroll_full();
	ct_utillib_init();
	
	// Load Header Sprite, Up and Down
	ct_header_add(1,data_lessontype);
	ct_header_add(0,data_lessontype);
	ct_header_TextID(0,TXT_MADEBY_,data_gameData[data_profile].language);
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_newuser_button_next = ct_button_add(BTN_CONFIG,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_newuser_button_next,g_newuser_nextText);
	ct_button_addContentIma(dw_newuser_button_next,(void *)arrow_r_Sprite,0,0,32,32);
	
	dw_newuser_button_left = ct_button_add(BTN_FIELD,192,BTN_32,32,64);
	ct_button_addHandler_Click(dw_newuser_button_left,g_newuser_selectHand);
	ct_button_setVariable(dw_newuser_button_left,CONF_LEFTHANDED);
	ct_button_TextID(dw_newuser_button_left,TXT_CFG_LHAND,data_gameData[data_profile].language,3);
	ct_button_hide(dw_newuser_button_left);

	dw_newuser_button_right = ct_button_add(BTN_FIELD,192,BTN_32,32,128);
	ct_button_addHandler_Click(dw_newuser_button_right,g_newuser_selectHand);
	ct_button_setVariable(dw_newuser_button_right,CONF_RIGHTHANDED);
	ct_button_TextID(dw_newuser_button_right,TXT_CFG_RHAND,data_gameData[data_profile].language,3);
	ct_button_hide(dw_newuser_button_right);
	
	dw_newuser_button_hidden = ct_button_add(BTN_FIELDSEL,32,BTN_32,250,190);
	ct_button_hide(dw_newuser_button_hidden);
	
	if (data_gameData[data_profile].hand == CONF_LEFTHANDED) {
	   ct_button_changetype(dw_newuser_button_left,BTN_FIELDSEL);
	} else {
	   ct_button_changetype(dw_newuser_button_right,BTN_FIELDSEL);
	}		   
	
	dw_newuser_button_text = ct_button_add(BTN_NONE,192,BTN_32,32,56);
	
	/* Init next state - Unique state */
	newuser_substate = 0;
}

void ct_g_newuser_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
} 


void ct_g_newuser_Input()
{
	// Check the stylus presses :
	int button;
	if (Stylus.Newpress) {
		// Records Position
		button = ct_button_checkClick(Stylus.X,Stylus.Y);
		// Button Management
		if (button != -1) {
			ct_button_exeHandler_Click(button);
			return; // Managed button
		}
	}
	
	// Check Keyboard pressed
	int letter;
	int nletter = 0;
	if (newuser_substate == 2) {
		    
		letter = PA_CheckKeyboard();
		
		if (letter > 31) { // New letter?
			if (newuser_textsize < (MAX_USERNAME - 1)) {
				newuser_text[newuser_textsize] = letter;
				newuser_textsize++;
				nletter = 1;
			}			
		} else if (letter == PA_BACKSPACE) { // Backspace pressed
			if (newuser_textsize > 0) {
				newuser_textsize--;			   
				newuser_text[newuser_textsize] = ' '; // Erase the last letter			   
			} else
				newuser_text[newuser_textsize] = ' '; // Erase the last letter
			nletter = 1;
		}
		if (nletter)
			ct_button_Text(dw_newuser_button_text,newuser_text,3);
	}	
}

void ct_g_newuser_State()
{
   int val;
   
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
	
	// State of the lesson
   switch (newuser_substate) {
      case 0:
         ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMAL);
         val = ct_smiley_setTextExt(SMILEY_NEWUSER_1,data_gameData[data_profile].language,-2,3,1);
         if (!val)
         	newuser_substate++;
         else if (val == 3)
         	ct_button_bh_blipContentStart(dw_newuser_button_next);
         break;         	
      case 1:
         ct_smiley_setTextMood(SMILEYMOOD_WHAT,SMILEYMOOD_WHAT);
         val = ct_smiley_setTextExt(SMILEY_NEWUSER_2,data_gameData[data_profile].language,-2,3,1);
         if (!val) {
         	newuser_substate++;
         	// Different "next" handler
         	ct_button_addHandler_Click(dw_newuser_button_next,g_newuser_nextKeyboard);
         	// Text
         	PA_CopyText(newuser_text, data_gameData[data_profile].name);
         	newuser_textsize = strlen(data_gameData[data_profile].name);
         	ct_button_Text(dw_newuser_button_text,newuser_text,3);
         	// Keyboard
				PA_InitKeyboard(2); // Load the keyboard on background 2...
      		PA_ScrollKeyboardXY(24, 96); // Show the Keyboard...         	
      		// Blip button
      		ct_button_bh_blipContentStart(dw_newuser_button_next);
       	}  	
         else if (val == 3)
         	ct_button_bh_blipContentStart(dw_newuser_button_next);
         break;         	
      case 2:
         break;
      case 3:
         ct_smiley_setTextMood(SMILEYMOOD_HAPPYJUMP,SMILEYMOOD_HAPPYJUMP);
         val = ct_smiley_setTextExt(SMILEY_NEWUSER_3,data_gameData[data_profile].language,-2,3,1);
         if (!val)
         	newuser_substate++;
         else if (val == 3)
         	ct_button_bh_blipContentStart(dw_newuser_button_next);
         break;         	
      case 4:
         ct_smiley_setTextMood(SMILEYMOOD_WHAT,SMILEYMOOD_WHAT);
         val = ct_smiley_setTextExt(SMILEY_NEWUSER_4,data_gameData[data_profile].language,-2,3,1);
         if (!val) {
         	newuser_substate++;
         	// Different "next" handler
         	ct_button_addHandler_Click(dw_newuser_button_next,g_newuser_nextHand);
         	// Show buttons
         	ct_button_show(dw_newuser_button_left);
         	ct_button_show(dw_newuser_button_right);
         	// Blip button
         	ct_button_bh_blipContentStart(dw_newuser_button_next);
      	}   
         else if (val == 3)
         	ct_button_bh_blipContentStart(dw_newuser_button_next);
         break;         	
      case 5:
         break;         
      case 6:
         ct_smiley_setTextMood(SMILEYMOOD_HAPPYJUMP,SMILEYMOOD_HAPPYJUMP);
         val = ct_smiley_setTextExt(SMILEY_NEWUSER_5,data_gameData[data_profile].language,-2,3,1);
         if (!val)
         	newuser_substate++;
         else if (val == 3)
         	ct_button_bh_blipContentStart(dw_newuser_button_next);
         break;         	
      case 7:
         ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMALHAPPY);
         val = ct_smiley_setTextExt(SMILEY_NEWUSER_6,data_gameData[data_profile].language,-2,3,1);
         if (!val)
         	newuser_substate++;
         else if (val == 3)
         	ct_button_bh_blipContentStart(dw_newuser_button_next);
         break;         	
      case 8:
         ct_state_assignState(GAME_MAINMENU);
         break;
	}
}
