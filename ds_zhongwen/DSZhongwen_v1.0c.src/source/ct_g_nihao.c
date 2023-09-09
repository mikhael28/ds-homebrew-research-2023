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
#include "ct_smiley.h"
#include "ct_header.h"
#include "ct_textscroll.h"
#include "ct_button.h"
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define NIHAO_OPENSCROLL 0
#define NIHAO_NORMAL 1
#define NIHAO_CLOSESCROLL 2

// Sprite-Related Info
#define LOGO_POS_X 16 
#define LOGO_POS_Y 40
#define BYRRC_POS_X 184
#define BYRRC_POS_Y 160

/* Variables - Internal State */
////////////////////////////////

// State
int nihao_state;
int iterate;
int actlanguage;

// Sprites
int up_titulopal; // titulo - Spanish Hi!
int up_titulo1000;
int up_titulo0100;
int up_titulo0001;
u16 up_titulo1000_gfx;
u16 up_titulo0100_gfx;
u16 up_titulo0010_gfx;
u16 up_titulo0001_gfx;

int dw_nihao_button_sp;
int dw_nihao_button_en;
int dw_nihao_button_credits;
int dw_nihao_button_start_p1;
int dw_nihao_button_start_p2;
int dw_nihao_button_start_p3;
int dw_nihao_button_start_p4;

/* Internal Functions */ 
////////////////////////

void button_state_ininormal() ;
void button_state_iniclosescroll();

// Function button_state_iniopenscroll() 
void button_state_iniopenscroll() 
{
	nihao_state = NIHAO_OPENSCROLL;
} // end of button_state_iniopenscroll() 

// Function button_state_openscroll() 
void button_state_openscroll() 
{
	// Cannot open? OK, it is opened, next state
	if (!ct_textscroll_openscroll(4)) {
		button_state_ininormal();
	}
} // end of button_state_openscroll() 

// Function button_state_ininormal() 
void button_state_ininormal() 
{
	nihao_state = NIHAO_NORMAL;
} // end of button_state_ininormal() 

// Function button_state_normal()
void button_state_normal() 
{
} // end of button_state_normal() 

// Function button_state_iniclosescroll() 
void button_state_iniclosescroll() 
{
   nihao_state = NIHAO_CLOSESCROLL;
} // end of button_state_iniclosescroll() 

// Function button_state_closescroll() 
void button_state_closescroll() 
{	
   if (ct_utillib_waitSemaphore() == 0){
      if (data_gameData[data_profile].active) {
         data_newuser = 0;
   		ct_state_assignState(GAME_MAINMENU);  // Next State - Config Menu
 		} else {
 		   data_newuser = 1;
 		   data_gameData[data_profile].active = 1;
 		   data_gameData[data_profile].hash = data_hashtitle;
 		   if (!Pad.Held.Select) {
 		   	ct_state_assignState(GAME_NEWUSER);  // Next State - New user
 			}   
 		   else
 		   	ct_state_assignState(GAME_MAINMENU);  // Next State - Bypass new user
 		}   
	}   
} // end of button_state_closescroll() 

/* Internal Handler Functions */ 
////////////////////////////////

void g_nihao_assignSpanish()
{
	ct_button_changetype(dw_nihao_button_sp,BTN_RED);
	ct_button_changetype(dw_nihao_button_en,BTN_GREY);
	actlanguage = LANG_SPA;
}

void g_nihao_assignEnglish()
{
	ct_button_changetype(dw_nihao_button_en,BTN_RED);
	ct_button_changetype(dw_nihao_button_sp,BTN_GREY);
	actlanguage = LANG_ENG;
}

void g_nihao_launchCredits()
{
	ct_state_assignState(GAME_CREDITS);  // Next State - Credits Menu
}

void g_nihao_mainMenu()
{
   int btn = ct_button_getHandler();
   
   // First! - Check if we are using the right library
   if ((data_gameData[ct_button_getVariable(btn)].active) && (data_gameData[ct_button_getVariable(btn)].hash != data_hashtitle))
   	return;

	// OK! Continue...
   data_profile = ct_button_getVariable(btn);
   
   ct_button_bh_blip(btn,90,1);
   if (btn != dw_nihao_button_start_p1)
   	ct_button_bh_move(dw_nihao_button_start_p1,256,-255,5,1);
   if (btn != dw_nihao_button_start_p2)
   	ct_button_bh_move(dw_nihao_button_start_p2,256,-255,5,1);
   if (btn != dw_nihao_button_start_p3)
   	ct_button_bh_move(dw_nihao_button_start_p3,256,-255,5,1);
   if (btn != dw_nihao_button_start_p4)
   	ct_button_bh_move(dw_nihao_button_start_p4,256,-255,5,1);

	button_state_iniclosescroll();
}

int g_nihao_getColorActive(int profile) {
   int col = BTN_GREY;
   if (data_gameData[profile].active) // Active
   	if (data_gameData[profile].hash != data_hashtitle) // User not from this library
   		col = BTN_RED;
   return col;
}   

/* Functions */ 
///////////////

// Function ct_g_nihao_Start()
void ct_g_nihao_Start()
{
   char text[255];
   
   // Resets the Configuration of the game data - This is the top-most screen
   ct_utillib_initData();
   
   // Inits the Graphic mode for saying Hi!!
	ct_graphics_initGraphicMode(SCREENMODE_INPUT);
		
	/* Graphics */				
     
	// Inits header subsystem, button subsystem, textscroll subsystem
	ct_button_init();
	ct_textscroll_init();
	ct_header_init();
	ct_utillib_init();

			
	// Load Header/Banner Sprite, Up and Down
	ct_header_add(1,data_lessontype);
	ct_header_add(0,data_lessontype);
	
	// Load Logo Sprite
	up_titulopal = ct_graphics_getPalette(1);
	PA_LoadSpritePal(1,up_titulopal,(void*)titulo_Pal);
	up_titulo1000 = ct_graphics_getSprite(1);
	up_titulo1000_gfx = PA_CreateGfx(1,(void*)up_titzh1000_Sprite, OBJ_SIZE_64X64, 1);
	PA_CreateSpriteFromGfx(1,up_titulo1000,up_titulo1000_gfx,OBJ_SIZE_64X64,1,up_titulopal,
									LOGO_POS_X,LOGO_POS_Y);
   PA_SetSpritePrio(1,up_titulo1000,3);						
   up_titulo0100 = ct_graphics_getSprite(1);
   up_titulo0100_gfx = PA_CreateGfx(1,(void*)up_titzh0100_Sprite, OBJ_SIZE_64X64, 1);
	PA_CreateSpriteFromGfx(1,up_titulo0100,up_titulo0100_gfx,OBJ_SIZE_64X64,1,up_titulopal,
									LOGO_POS_X+64,LOGO_POS_Y);
   PA_SetSpritePrio(1,up_titulo0100,3);
   up_titulo0001 = ct_graphics_getSprite(1);
   up_titulo0001_gfx = PA_CreateGfx(1,(void*)up_titulo0001_Sprite, OBJ_SIZE_64X32, 1);
	PA_CreateSpriteFromGfx(1,up_titulo0001,up_titulo0001_gfx,OBJ_SIZE_64X32,1,up_titulopal,
									LOGO_POS_X+64,LOGO_POS_Y+64);
   PA_SetSpritePrio(1,up_titulo0001,3);
	   
   
   // Load Text Gfxs
   // (Already done in initialization)
	
	// Load Buttons
	dw_nihao_button_en = ct_button_add(BTN_GREY,80,BTN_32,-12,64);
	ct_button_TextID(dw_nihao_button_en,TXT_INI_ENGLISH,LANG_ENG,3);
	ct_button_addHandler_Click(dw_nihao_button_en,g_nihao_assignEnglish);
	
	dw_nihao_button_sp = ct_button_add(BTN_GREY,80,BTN_32,-12,128);
	ct_button_TextID(dw_nihao_button_sp,TXT_INI_SPANISH,LANG_ENG,3);
	ct_button_addHandler_Click(dw_nihao_button_sp,g_nihao_assignSpanish);
	
	dw_nihao_button_credits = ct_button_add(BTN_CONFIG,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_nihao_button_credits,g_nihao_launchCredits);
	ct_button_addContentIma(dw_nihao_button_credits,(void *)logo_rrc2_Sprite,0,0,32,32);
	//ct_button_addContentTxt(dw_button_config,(void *)txt_ideograma_sp_Sprite,12,0,64,32);

	dw_nihao_button_start_p1 = ct_button_add(g_nihao_getColorActive(0),128,BTN_32,112,48);
	ct_button_addHandler_Click(dw_nihao_button_start_p1,g_nihao_mainMenu);
	ct_button_setVariable(dw_nihao_button_start_p1,0);
	if (!data_gameData[0].active)
		sprintf(text,"-----");
	else sprintf(text,"%s",data_gameData[0].name);
	ct_button_Text(dw_nihao_button_start_p1,text,3);
	
	dw_nihao_button_start_p2 = ct_button_add(g_nihao_getColorActive(1),128,BTN_32,112,84);
	ct_button_addHandler_Click(dw_nihao_button_start_p2,g_nihao_mainMenu);
	ct_button_setVariable(dw_nihao_button_start_p2,1);
	if (!data_gameData[1].active)
		sprintf(text,"-----");
	else sprintf(text,"%s",data_gameData[1].name);
	ct_button_Text(dw_nihao_button_start_p2,text,3);

	dw_nihao_button_start_p3 = ct_button_add(g_nihao_getColorActive(2),128,BTN_32,112,120);
	ct_button_addHandler_Click(dw_nihao_button_start_p3,g_nihao_mainMenu);
	ct_button_setVariable(dw_nihao_button_start_p3,2);
	if (!data_gameData[2].active)
		sprintf(text,"-----");
	else sprintf(text,"%s",data_gameData[2].name);
	ct_button_Text(dw_nihao_button_start_p3,text,3);

	dw_nihao_button_start_p4 = ct_button_add(g_nihao_getColorActive(3),128,BTN_32,112,156);
	ct_button_addHandler_Click(dw_nihao_button_start_p4,g_nihao_mainMenu);
	ct_button_setVariable(dw_nihao_button_start_p4,3);
	if (!data_gameData[3].active)
		sprintf(text,"-----");
	else sprintf(text,"%s",data_gameData[3].name);
	ct_button_Text(dw_nihao_button_start_p4,text,3);

	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_APPEAR);
	ct_smiley_setNextState(SMILEYMOOD_NORMAL);
	
	/* Others */
	if (PA_UserInfo.Language == 5)
		g_nihao_assignSpanish();
	else g_nihao_assignEnglish();
	
	// Library Title
   PA_CenterSmartText(1,100 , 160, 
								256, 192, 
								ct_utillib_getTitle(), 1, 2, 0);	
	
								
	// Version number
   PA_CenterSmartText(0,0 , 180, 
								48, 192, 
								DSZHONGWEN_VERSION, 1, 2, 0);	

	
	/* Init next state */
   button_state_iniopenscroll();
} // end of ct_g_nihao_Start()

// Function ct_g_nihao_Finish()
void ct_g_nihao_Finish()
{
   PA_WaitForVBL();
	// Finish the header, button subsystem, textscroll subsystem
	ct_button_finish();
	ct_header_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
	
	// Delete Gfxs
	PA_DeleteGfx(1,up_titulo1000_gfx);
	PA_DeleteGfx(1,up_titulo0100_gfx);
	PA_DeleteGfx(1,up_titulo0010_gfx);
	PA_DeleteGfx(1,up_titulo0001_gfx);
		
	// Assigns the language to the selected profile
	data_gameData[data_profile].language = actlanguage;
} // end of ct_g_nihao_Finish()

// Function ct_g_nihao_Input()
void ct_g_nihao_Input()
{
	int button;
	if (nihao_state == NIHAO_NORMAL) {
		if (Stylus.Newpress) {
			// Records Position
			button = ct_button_checkClick(Stylus.X,Stylus.Y);
			// Button Management
			if (button != -1) {
				ct_button_exeHandler_Click(button);
				return; // Managed button
			}
			// Screen Management
			// ...
		}
	}
} // end of ct_g_nihao_Input()

// Function ct_g_nihao_State()
void ct_g_nihao_State()
{
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
	
	// System state
	switch (nihao_state) {
		case NIHAO_OPENSCROLL:
		   button_state_openscroll();
			break;
		case NIHAO_NORMAL:
		   button_state_normal();
			break;
		case NIHAO_CLOSESCROLL:
		   button_state_closescroll();
			break;			
	}
} // end of ct_g_nihao_State()
