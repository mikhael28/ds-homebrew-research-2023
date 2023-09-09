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

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define CREDITS_CLOSESCROLL 0
#define CREDITS_ROLL 1


/* Variables - Internal State */
////////////////////////////////

int up_titulopal; // titulo - Spanish Hi!
int up_titulo1000;
int up_titulo0100;
int up_titulo0001;
u16 up_titulo1000_gfx;
u16 up_titulo0100_gfx;
u16 up_titulo0001_gfx;

int credits_state;
int credits_substate;
int credits_time;

int dw_credits_button_back;
int dw_credits_button_text;

/* Internal Functions */ 
////////////////////////

void g_credits_launchBack(){
	ct_state_assignState(GAME_NIHAO);
}

void g_credits_advancestate(){
	credits_time = 0;
}

/* Functions */ 
///////////////

void ct_g_credits_Start()
{   	
	// Inits the Graphic mode for the credits
	ct_graphics_initGraphicMode(SCREENMODE_INPUT);
	
	// Graphics 
	// Headers, buttons, textscroll
	ct_header_init();
	ct_button_init();
	ct_textscroll_init();
	ct_textscroll_openscroll_full();
	ct_utillib_init();
	
	// Load Header Sprite, Up and Down
	// BUG - TODO - But will leave it "as is"... wanna finish it :-)
	ct_header_add(1,data_lessontype);
	ct_header_add(0,data_lessontype);
	ct_header_TextID(0,TXT_MADEBY_,LANG_ENG);
	
	// Logo, just for the first state
	// Load Logo Sprite
	up_titulopal = ct_graphics_getPalette(0);
	PA_LoadSpritePal(0,up_titulopal,(void*)titulo_Pal);
	up_titulo1000 = ct_graphics_getSprite(0);
	up_titulo1000_gfx = PA_CreateGfx(0,(void*)up_titzh1000_Sprite, OBJ_SIZE_64X64, 1);
	PA_CreateSpriteFromGfx(0,up_titulo1000,up_titulo1000_gfx,OBJ_SIZE_64X64,1,up_titulopal,
									64,64);
   PA_SetSpritePrio(0,up_titulo1000,0);						
   up_titulo0100 = ct_graphics_getSprite(0);
   up_titulo0100_gfx = PA_CreateGfx(0,(void*)up_titzh0100_Sprite, OBJ_SIZE_64X64, 1);
	PA_CreateSpriteFromGfx(0,up_titulo0100,up_titulo0100_gfx,OBJ_SIZE_64X64,1,up_titulopal,
									64+64,64);
   PA_SetSpritePrio(0,up_titulo0100,0);
   up_titulo0001 = ct_graphics_getSprite(0);
   up_titulo0001_gfx = PA_CreateGfx(0,(void*)up_titulo0001_Sprite, OBJ_SIZE_64X32, 1);
	PA_CreateSpriteFromGfx(0,up_titulo0001,up_titulo0001_gfx,OBJ_SIZE_64X32,1,up_titulopal,
									192,160);
   PA_SetSpritePrio(0,up_titulo0001,0);	
					
	// Variables
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);
	ct_smiley_setNextState(SMILEYMOOD_DISAPPEAR);

	dw_credits_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_credits_button_back,g_credits_launchBack);
	ct_button_addContentIma(dw_credits_button_back,(void *)arrow_b_Sprite,0,0,32,32);
	
	dw_credits_button_text = ct_button_add(BTN_NONE,208,BTN_64,24,72);
	ct_button_addHandler_Click(dw_credits_button_text,g_credits_advancestate);
	
	/* Init next state  */
	credits_state = CREDITS_CLOSESCROLL;
}

void ct_g_credits_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
	
	// Delete Gfxs
	PA_DeleteGfx(1,up_titulo1000_gfx);
	PA_DeleteGfx(1,up_titulo0100_gfx);
	PA_DeleteGfx(1,up_titulo0001_gfx);
} 


void ct_g_credits_Input()
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
		// Screen Management ... same as button
		g_credits_advancestate();
	}
}

void ct_g_credits_State()
{
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
	
	// Close Textscroll if it was opened
	if (credits_state == CREDITS_CLOSESCROLL) {
	   if (!ct_textscroll_closescroll(4)) {
	      // Advance State
	   	credits_state = CREDITS_ROLL;
	   	credits_substate = 0;
	   	credits_time = 0;
		}   
	} else if (credits_state == CREDITS_ROLL) {
	   credits_time--;
	   if (credits_time <= 0) {
	      switch (credits_substate) {
	         case 0:
	            credits_time = 180;
	            break;
	         case 1:	      		
	            PA_SetSpriteX(0,up_titulo1000,256);
	            PA_SetSpriteX(0,up_titulo0100,257);
	            PA_SetSpriteX(0,up_titulo0001,256);
	            credits_time = 240;
	            ct_button_Text(dw_credits_button_text,"Idea Original / Original Idea \n\n RRC2Software",2);
	            break;
	         case 2:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Diseño / Game Design \n\n Rodrigo Roman \n\n (RRC2Software)",2);
	            break;	            
	         case 3:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Programacion / Game Programming \n\n Rodrigo Roman \n\n (RRC2Software)",2);
	            break;	      
	         case 4:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Pruebas / Playtesting \n\n Ramon Roman \n\n (RRC2Software)",2);
	            break;	      	            
	         case 5:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Hanzi Graphics / Ideogramas \n\n CNewsRead by drhu",2);
	            break;	      	            	            
	         case 6:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Animated Hanzi / Ideogramas Animados \n\n OCRAT",2);
	            break;	      	            	            
	         case 7:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Pinyin Phonetics / Pronunciacion Pinyin \n\n OCRAT - Dr. John Jinghua Yin",2);
	            break;	      	            	            	            
	         case 8:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Translations / Traducciones \n\n CEDICT (MDBG) \n\n http://www.mdbg.net/chindict/chindict.php",2);
	            break;	      	            	            	            
	         case 9:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Agradecimientos / Thank You \n\n Ping Xiao, Ruan Yi, \n\n Weichuan Chu Huang",2);
	            break;	            	             					      	             
	         case 10:
	            credits_time = 300;
	            ct_button_Text(dw_credits_button_text,"Thanks for Playing! Gracias por jugar!",1);
	            PA_SetSpriteX(0,up_titulo1000,64);
	            PA_SetSpriteY(0,up_titulo1000,32);	            
	            PA_SetSpriteX(0,up_titulo0100,64+64);
	            PA_SetSpriteY(0,up_titulo0100,32);	            
	            PA_SetSpriteX(0,up_titulo0001,192);
	            break;	            	             	            
	      } 
			credits_substate++;   
	   }   
	} 
}
