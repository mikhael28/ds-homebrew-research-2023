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

int dw_config_button_back;
int dw_config_button_speed;
int dw_config_button_speeditem[5];
int dw_config_button_hand;
int dw_config_button_handitem[2];
int dw_config_button_tool_dict;

/* Internal Functions */ 
////////////////////////

void g_config_reload(){
   int val;
   int i;
   
   // Clear fields from all buttons
	for (i = 0; i < 5; i++) { 
	   ct_button_changetype(dw_config_button_speeditem[i],BTN_FIELD);
	}   
	for (i = 0; i < 2; i++) { 
	   ct_button_changetype(dw_config_button_handitem[i],BTN_FIELD);
	}   		
	// Show selected buttons
	val = (data_gameData[data_profile].speed / 20) - 1;
	ct_button_changetype(dw_config_button_speeditem[val],BTN_FIELDSEL);
	val = data_gameData[data_profile].hand;
	ct_button_changetype(dw_config_button_handitem[val],BTN_FIELDSEL);
}

void g_config_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
}

void g_config_launchDict(){
	ct_state_assignState(GAME_DICT);
	ct_state_var_setStr("");  // Start at no word
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
}


void g_config_selectSpeed(){
   data_gameData[data_profile].speed = ct_button_getVariable(ct_button_getHandler());
   g_config_reload();
}

void g_config_selectHand(){
   data_gameData[data_profile].hand = ct_button_getVariable(ct_button_getHandler());
   g_config_reload();
}

void g_config_writeSpeed(){
  	ct_smiley_setText(ct_smiley_getText(SMILEY_CFG_SPEED,0,data_gameData[data_profile].language),-2,3,0);
}

void g_config_writeHand(){
  	ct_smiley_setText(ct_smiley_getText(SMILEY_CFG_HAND,0,data_gameData[data_profile].language),-2,3,0);
}

/* Functions */ 
///////////////

void ct_g_config_Start()
{
   int i;
   char num[6];
   
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
	ct_header_TextID(0,TXT_CFG_,data_gameData[data_profile].language);
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);
	ct_smiley_setText(ct_smiley_getText(SMILEY_CFG,0,data_gameData[data_profile].language),-2,3,0);

	dw_config_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_config_button_back,g_config_launchBack);
	ct_button_addContentIma(dw_config_button_back,(void *)arrow_b_Sprite,0,0,32,32);
	
	// Configuration items
	dw_config_button_speed = ct_button_add(BTN_GREY,64,BTN_32,0,32 + 12);
	ct_button_addHandler_Click(dw_config_button_speed,g_config_writeSpeed);
	ct_button_TextID(dw_config_button_speed,TXT_CFG_SPEED,data_gameData[data_profile].language,2);
	for (i = 0; i < 5; i++) { 
	   dw_config_button_speeditem[i] = ct_button_add(BTN_FIELD,32,BTN_32,70 + (i * 32) + (i * 4),32 + 12);
	   ct_button_setVariable(dw_config_button_speeditem[i],20 + (i * 20));
	   ct_button_addHandler_Click(dw_config_button_speeditem[i],g_config_selectSpeed);
	   sprintf(num,"%d",i+1);
	   ct_button_Text(dw_config_button_speeditem[i],num,3);
	}   
	
	dw_config_button_hand = ct_button_add(BTN_GREY,64,BTN_32,0,12 + 32 + 32 + 8);
	ct_button_addHandler_Click(dw_config_button_hand,g_config_writeHand);
	ct_button_TextID(dw_config_button_hand,TXT_CFG_HAND,data_gameData[data_profile].language,2);
	for (i = 0; i < 2; i++) { 
	   dw_config_button_handitem[i] = ct_button_add(BTN_FIELD,80,BTN_32,74 + (i * 80) + (i * 8),12 + 32 + 32 + 8);
	   ct_button_setVariable(dw_config_button_handitem[i],(i==0)?CONF_LEFTHANDED:CONF_RIGHTHANDED);
	   ct_button_addHandler_Click(dw_config_button_handitem[i],g_config_selectHand);
	   ct_button_TextID(dw_config_button_handitem[i],(i==0)?TXT_CFG_LHAND:TXT_CFG_RHAND,data_gameData[data_profile].language,2);
	}   		
	
	dw_config_button_tool_dict = ct_button_add(BTN_GREY,112,BTN_32,0,12 + 32 + 32 + 8 + 32 + 8);
	ct_button_addHandler_Click(dw_config_button_tool_dict,g_config_launchDict);
	ct_button_TextID(dw_config_button_tool_dict,TXT_DICT_TEXT,data_gameData[data_profile].language,2);

	/* Init next state - Unique state */
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
	//dw_button_back = ct_button_add(BTN32_GREEN,32,100,8);
	g_config_reload();
}

void ct_g_config_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();	
} 


void ct_g_config_Input()
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

void ct_g_config_State()
{
	// Elements state
	ct_smiley_Manage();
}
