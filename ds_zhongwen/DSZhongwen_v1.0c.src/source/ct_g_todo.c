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


// State_str: Str que debe decir el smiley

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

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// No internal state

/* Variables - Internal State */
////////////////////////////////

int dw_todo_button_back;
int dw_todo_button_text;

/* Internal Functions */ 
////////////////////////

void g_todo_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
}


/* Functions */ 
///////////////

void ct_g_todo_Start()
{
   
	// Inits the Graphic mode for the configuration
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
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);
	ct_smiley_setText(ct_state_var_getStr(),-2,3,0);

	dw_todo_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_todo_button_back,g_todo_launchBack);
	ct_button_addContentIma(dw_todo_button_back,(void *)arrow_b_Sprite,0,0,32,32);

	dw_todo_button_text = ct_button_add(BTN_NONE,208,BTN_64,24,72);
	ct_button_addHandler_Click(dw_todo_button_text,g_todo_launchBack);
	ct_button_Text(dw_todo_button_text,ct_const_getText(TXT_LESSON_NO,data_gameData[data_profile].language),3);
	
	/* Init next state - Unique state */
}

void ct_g_todo_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();	
} 


void ct_g_todo_Input()
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
		// Screen Management ... nothing
	}
}

void ct_g_todo_State()
{
	// Elements state
	ct_smiley_Manage();
}
