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
#define G_LESSON_NO 0
#define G_LESSON_INTRO 1

/* Variables - Internal State */
////////////////////////////////

int dw_lesson_button_back;
int dw_lesson_button_next;
int dw_lesson_button_text;
int lesson_state;
int lesson_substate;

/* Internal Functions */ 
////////////////////////

void g_lesson_startLesson(){
   // Depending on what lesson we had chosen, we start a new state
   switch (data_lessontype) {
      case LESSON_HANZI:
         ct_state_assignState(GAME_LHANZI);
         ct_state_var_setInt(0,1); // Hanzi:[0] - 1, we are inside a lesson.
         ct_state_var_setInt(1,1); // Hanzi:[1] - 1, word number 1.
         ct_state_var_setInt(2,1); // Hanzi:[2] - 1, "word length" 1.
         ct_state_var_setInt(3,GAME_MAINMENU);  // Hanzi:[3] - Screen to return.
         break;
      case LESSON_LISTEN:
         ct_state_assignState(GAME_LLISTEN);
         ct_state_var_setInt(0,1); // Listen:[0] - 1, we are inside a lesson.
         ct_state_var_setInt(1,1); // Listen:[1] - 1, phrase number 1. 
			ct_state_var_setInt(2,0); // Listen:[2] - 0, non-continuous mode.         
         break;
      case LESSON_GRAMM:
			ct_state_assignState(GAME_LGRAM);         
         ct_state_var_setInt(0,0); // Gramm:[0] - 0, Start in subset number one
         break;         
      case LESSON_HANZIATTACK:
         ct_state_assignState(GAME_LHANZI);
         ct_state_var_setInt(0,1); // Hanzi:[0] - 1, we are inside a lesson.
         ct_state_var_setInt(1,1); // Hanzi:[1] - 1, word number 1.
         ct_state_var_setInt(2,1); // Hanzi:[2] - 1, "word length" 1.
         ct_state_var_setInt(3,GAME_MAINMENU);  // Hanzi:[3] - Screen to return.
         break;         
   }   
}   

void g_lesson_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
}

void g_lesson_launchNext(){
		if (data_lessontype == LESSON_)
			g_lesson_launchBack();
		else g_lesson_startLesson();
}

void g_lesson_nextText(){
   if (ct_utillib_waitSemaphore()) {
      ct_button_bh_blipContentStop(dw_lesson_button_next);
   	ct_utillib_signalSemaphore();
   	ct_smiley_endText();
	}   
}

/* Functions */ 
///////////////

void ct_g_lesson_Start()
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
	ct_header_TextID(0,TXT_LESSON_,data_gameData[data_profile].language);
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_lesson_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_lesson_button_back,g_lesson_launchBack);
	ct_button_addContentIma(dw_lesson_button_back,(void *)arrow_b_Sprite,0,0,32,32);

	dw_lesson_button_next = ct_button_add(BTN_CONFIG,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_lesson_button_next,g_lesson_nextText);
	ct_button_addContentIma(dw_lesson_button_next,(void *)arrow_r_Sprite,0,0,32,32);

	dw_lesson_button_text = ct_button_add(BTN_NONE,208,BTN_64,24,72);
	ct_button_addHandler_Click(dw_lesson_button_text,g_lesson_launchNext);
	//ct_button_addHandler_Click(dw_lesson_button_text,g_credits_advancestate);

	
	/* Init next state - Unique state */
	if (data_lessontype == LESSON_)
		lesson_state = G_LESSON_NO;
	else lesson_state = G_LESSON_INTRO;
	lesson_substate = 0;
}

void ct_g_lesson_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
} 


void ct_g_lesson_Input()
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
		// Screen Management ... Directly start the lesson :-)
	}
}

void ct_g_lesson_State()
{
   int val;
   
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
	
	// State of the lesson
	if (lesson_state == G_LESSON_NO) {
	   
	   switch (lesson_substate) {
	      case 0:
	         ct_button_Text(dw_lesson_button_text,ct_const_getText(TXT_LESSON_NO,data_gameData[data_profile].language),3);
	         lesson_substate++;
	   	   break;
	      case 1:
	         ct_smiley_setTextMood(SMILEYMOOD_HAPPYJUMP,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_NOLESSON_1,data_gameData[data_profile].language,-1,3,0))
	         	lesson_substate++;
	         break;
	      case 2:
	         ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_NOLESSON_2,data_gameData[data_profile].language,-1,3,0))
	         	lesson_substate++;
	         break;
	      case 3:
	         ct_smiley_setTextMood(SMILEYMOOD_UNHAPPY,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_NOLESSON_3,data_gameData[data_profile].language,-1,3,0))
	         	lesson_substate++;
	         break;
	      case 4:
	         ct_smiley_setTextMood(SMILEYMOOD_HAPPYJUMP,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_NOLESSON_4,data_gameData[data_profile].language,600,3,0))
	         	lesson_substate++;
	         break;	         
	      case 5:
	         ct_smiley_setTextMood(SMILEYMOOD_AMUSED,SMILEYMOOD_AMUSED);
	         if (!ct_smiley_setTextExt(SMILEY_NOLESSON_5,data_gameData[data_profile].language,-1,3,0))
	         	lesson_substate++;
	         break;	         
	      case 6:
	         ct_smiley_setTextMood(SMILEYMOOD_HAPPYJUMP,SMILEYMOOD_HAPPYJUMP);
	         if (!ct_smiley_setTextExt(SMILEY_NOLESSON_6,data_gameData[data_profile].language,-1,3,0))
	         	lesson_substate++;
	         break;	         	         
		}
	}					
	if (lesson_state == G_LESSON_INTRO) {
	   switch (lesson_substate) {
	      case 0:
	         ct_button_Text(dw_lesson_button_text,ct_const_getText(TXT_LESSON_GO,data_gameData[data_profile].language),3);
	         lesson_substate++;
	   	   break;	      
	      case 1:
	         ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMAL);
	         val = ct_smiley_setTextExt2(ct_lesson_strIntro(ct_smiley_getTextExt()),
													-2,3,1);
	         if (!val)
	         	lesson_substate++;
	         else if (val == 3)
	         	ct_button_bh_blipContentStart(dw_lesson_button_next);
	         break;
	      case 2:
	         	g_lesson_startLesson();
	         break;
	   }   
	}   
}
