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


/* Hanzi - Pinyin */
/*
	State_str = hanzi; (strlen / 4 = number of hanzi in the word)
	State[0] = 0 - Individual Hanzi, 1 - Part of a lesson
	State[1] = Actual "Word"
	State[2] = Actual "Word Length"
	State[3] = Screen before this one
	State[4] = Size (for the GAME_REVIEWHANZI screen)
	State[5] = Index (for the GAME_REVIEWHANZI screen)
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
#include "ct_HALhanzi.h"
#include "ct_lesson.h"
#include "ct_textzone.h"
#include "ct_hwsound.h"
#include "ct_hwmeaning.h"
#include "ct_anihanzi.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define LHANZI_HANZI 0
#define LHANZI_PINYIN 1

/* Variables - Internal State */
////////////////////////////////
int lhanzi_state;
int lhanzi_actual;
int lhanzi_finished;

int lhanzi_actsound;
int lhanzi_playsound;
int lhanzi_delaysound;

int dw_lhanzi_button_back;
int dw_lhanzi_button_next;
int lhanzi_pal;
int lhanzi_templread;
int lhanzi_templwrite;
u16 lhanzi_templ_gfx;

int lhanzi_btn_11;
int lhanzi_btn_21;
int lhanzi_btn_22;
int lhanzi_btn_31;
int lhanzi_btn_32;
int lhanzi_btn_33;
int lhanzi_btn_41;
int lhanzi_btn_42;
int lhanzi_btn_43;
int lhanzi_btn_44;

int lhanzi_btn[4][4];

int dw_lhanzi_sealpal;
int dw_lhanzi_seal;
u16 dw_lhanzi_sealgfx;


/* Internal Functions */ 
////////////////////////

void g_lhanzi_launchBack(){
	ct_state_assignState(ct_state_var_getInt(3));
	if (ct_state_var_getInt(3) == GAME_REVIEWHANZI) {
	   ct_state_var_setInt(0,ct_state_var_getInt(4)); // [0] Size
	   ct_state_var_setInt(1,ct_state_var_getInt(5)); // [1] Index
	}   
}

void g_lhanzi_setField(int nlet) {
   int i;
   
   for (i = 0; i < nlet; i++) {
      if (i+1 == lhanzi_actual)
      	ct_button_changetype(lhanzi_btn[nlet-1][i],BTN_FIELDSEL);
      else ct_button_changetype(lhanzi_btn[nlet-1][i],BTN_FIELD);
   }   
}

void g_lhanzi_newHanzi();

void g_lhanzi_setType(int type, int nlet) {
   
   int i;
   int j;
   char textsmiley[255];
   
   /* Button Management */
   // First, hide everything! :-D
   for (i = 0; i < 4; i++) 
   	for (j = 0; j < 4; j++) {
   	   if (lhanzi_btn[i][j] != -1) {
   	      ct_button_hide(lhanzi_btn[i][j]);
   	   }   
   	}   
   	
   // Now, show only the important ones (also, add handlers -:) )
   for (i = 0; i < nlet; i++) {
      ct_button_show(lhanzi_btn[nlet-1][i]);
      ct_button_addHandler_Click(lhanzi_btn[nlet-1][i],g_lhanzi_newHanzi);
   }   
   
   /* Put the graphics in the button :-) */
	for (i = 1; i <= nlet; i++ ) {
	   ct_HALhanzi_word_getHanzi(ct_state_var_getStr(),i);
	   ct_HALhanzi_paintHanzi32x32(0,
	   									ct_HALhanzi_word_getHanzi(ct_state_var_getStr(),i),
	   									ct_button_getX(lhanzi_btn[nlet-1][i-1]),
	   									ct_button_getY(lhanzi_btn[nlet-1][i-1]));
	}
	
	/* Cleans textzone */
	ct_textzone_clear();		 
	
	/* Puts meaning and pinyin in the upper screen */
	sprintf(textsmiley,"@@2%s @@9 \n\n%s",ct_HALhanzi_phrase_getStrPinyin(ct_state_var_getStr()),
										ct_hwmeaning_getHanzi(ct_state_var_getStr(),data_gameData[data_profile].language)
				);
	ct_smiley_setText(textsmiley,-1,3,0);
	
	/* Load new hanzi */
	ct_anihanzi_load(ct_utillib_int4hex(ct_HALhanzi_phrase_getHanzi(ct_state_var_getStr(),lhanzi_actual)));	
	
	/* Update Fields */
	g_lhanzi_setField(nlet);
	
	/* Reinitializes sound system */
   if (PA_SoundChannelIsBusy(0))
     	PA_StopSound(0);
   lhanzi_actsound = 1;
   lhanzi_playsound = 1;
   lhanzi_delaysound = 20;
}

void g_lhanzi_newHanzi() {
   // Change hanzi, reload everything
   lhanzi_actual = ct_button_getVariable(ct_button_getHandler());
   g_lhanzi_setType(lhanzi_state, ct_HALhanzi_word_lenHanzi(ct_state_var_getStr()));
}	   
   

void g_lhanzi_Start() {
   // First, get the actual word
   int res = 0;
   int stop = 0;
   while ((!res) && (!stop)) {
      // Try to get the actual word - depends on call mode!
      if (ct_state_var_getInt(0))
      	ct_state_var_setStr(ct_lesson_askWord(ct_state_var_getInt(2),ct_state_var_getInt(1)));
      // else ... No, str is already passed as a parameter :-)
      if (PA_CompareText("",ct_state_var_getStr())) {
         // Next phrase... starting in 0!!!!
         ct_state_var_setInt(2,ct_state_var_getInt(2) + 1);
         ct_state_var_setInt(1,1);
         // No more phrases?
         if ((ct_state_var_getInt(2) > 4) || (!ct_state_var_getInt(0)))
         	stop = 1;
      } else {
		   // Assign the actual word - done in ct_state_var_setStr() :-P
		   // OK, next word!
		   ct_state_var_setInt(1,ct_state_var_getInt(1) + 1);
		   res = 1; // end
		}			   
   }   
   
   // Now, Sets up the environment
   if ((stop) || (!ct_state_var_getInt(0))) {
      // No next button
      ct_button_hide(dw_lhanzi_button_next);
      if (ct_state_var_getInt(0)) { // Part of a lesson? Finished! :-D
      	lhanzi_finished = 1;
	      // "Finished" Seal :-D
			dw_lhanzi_sealpal = ct_graphics_getPalette(0);
			PA_LoadSpritePal(0,dw_lhanzi_sealpal,(void*)seal_Pal);
			dw_lhanzi_seal = ct_graphics_getSprite(0);
			dw_lhanzi_sealgfx = PA_CreateGfx(0,(void*)logo_seal_Sprite, OBJ_SIZE_64X64,1);
			PA_CreateSpriteFromGfx(0,dw_lhanzi_seal,dw_lhanzi_sealgfx,OBJ_SIZE_64X64,1,dw_lhanzi_sealpal,
											96,64);
	   	PA_SetSpritePrio(0,dw_lhanzi_seal,0);
   		// Smiley says, hi!
   		ct_smiley_setState(SMILEYMOOD_HAPPYJUMP);   	
	   	ct_smiley_setText(ct_const_getText(TXT_LESSON_END,data_gameData[data_profile].language),-2,3,0);
	   	// Flags lesson as finished 
			if (!ct_utillib_bitOne16(data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16],
										   (data_lessonnumber - 1) % 16)) {			
				data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16] =	   	
		   			ct_utillib_bitSet16(
	   					data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16],
	   					(data_lessonnumber - 1) % 16
						);
				lhanzi_finished = 2;
			}			
 		}  	
   }   
}

void g_lhanzi_launchNext(){
	ct_state_assignState(GAME_LHANZI); // Himself! :-D
}   

/* Functions */ 
///////////////

void ct_g_lhanzi_Start()
{
   int i;
   int j;
   int xread;
   int yread;
   int xwrite;
   int ywrite;

	// Internal state
	lhanzi_state = LHANZI_HANZI;
	lhanzi_actual = 1; // The first one is always the selected
	lhanzi_finished = 0;
	if (data_gameData[data_profile].hand == CONF_LEFTHANDED) {
	   xread = 128;
		yread = 64;
		xwrite = 0;
		ywrite = 64; 
	} else {
	   xread = 0;
		yread = 64;
		xwrite = 128;
		ywrite = 64; 
	}      

	// Inits the Graphic mode for the input of hanzi
	ct_graphics_initGraphicMode(SCREENMODE_INPUT);
	
	/* Graphics */
	// Headers, buttons, textscroll
	ct_header_init();
	ct_button_init();
	ct_textscroll_init();
	ct_textscroll_openscroll_full();
	ct_utillib_init();
	ct_anihanzi_init(0,xread + 8, yread + 8,data_gameData[data_profile].speed,1);
	
	// Load Header Sprite, Up and Down - only if...
	if (ct_state_var_getInt(3) != GAME_DICT) {
		ct_header_add(1,data_lessontype);
		ct_header_add(0,data_lessontype);
		ct_header_TextID(0,TXT_LESSON_HANZI_,data_gameData[data_profile].language);
	}	
	
	// Elements of the screen
	lhanzi_pal = ct_graphics_getPalette(0);
	PA_LoadSpritePal(0,lhanzi_pal,(void*)hanzi_Pal);
	lhanzi_templ_gfx = PA_CreateGfx(0,(void*)hanzi_template_Sprite, OBJ_SIZE_64X64, 1);
	PA_SetRotset(0, 0, 0, 128, 128);

	lhanzi_templread = ct_graphics_getSprite(0);
	PA_CreateSpriteFromGfx(0,lhanzi_templread,lhanzi_templ_gfx,OBJ_SIZE_64X64,1,lhanzi_pal,
									xread,yread);
   PA_SetSpritePrio(0,lhanzi_templread,3);						
	PA_SetSpriteDblsize(0,lhanzi_templread,1);
	PA_SetSpriteRotEnable(0,lhanzi_templread,0);

	lhanzi_templwrite = ct_graphics_getSprite(0);
	PA_CreateSpriteFromGfx(0,lhanzi_templwrite,lhanzi_templ_gfx,OBJ_SIZE_64X64,1,lhanzi_pal,
									xwrite,ywrite);
   PA_SetSpritePrio(0,lhanzi_templwrite,3);						
	PA_SetSpriteDblsize(0,lhanzi_templwrite,1);
	PA_SetSpriteRotEnable(0,lhanzi_templwrite,0);
	
	ct_textzone_init(xwrite,ywrite,1);
	ct_textzone_setPenSize(3);
	
	// Buttons
   for (i = 0; i < 4; i++) 
   	for (j = 0; j < 4; j++) {
   	   lhanzi_btn[i][j] = -1;
   	}   
	lhanzi_btn[0][0] = ct_button_add(BTN_FIELD,BTN_32,32,112,32);
	ct_button_setVariable(lhanzi_btn[0][0],1);
	lhanzi_btn[1][0] = ct_button_add(BTN_FIELD,BTN_32,32,96,32);
	ct_button_setVariable(lhanzi_btn[1][0],1);
	lhanzi_btn[1][1] = ct_button_add(BTN_FIELD,BTN_32,32,128,32);
	ct_button_setVariable(lhanzi_btn[1][1],2);
	lhanzi_btn[2][0] = ct_button_add(BTN_FIELD,BTN_32,32,80,32);
	ct_button_setVariable(lhanzi_btn[2][0],1);
	lhanzi_btn[2][1] = ct_button_add(BTN_FIELD,BTN_32,32,112,32);
	ct_button_setVariable(lhanzi_btn[2][1],2);
	lhanzi_btn[2][2] = ct_button_add(BTN_FIELD,BTN_32,32,144,32);
	ct_button_setVariable(lhanzi_btn[2][2],3);
	lhanzi_btn[3][0] = ct_button_add(BTN_FIELD,BTN_32,32,64,32);
	ct_button_setVariable(lhanzi_btn[3][0],1);
	lhanzi_btn[3][1] = ct_button_add(BTN_FIELD,BTN_32,32,96,32);
	ct_button_setVariable(lhanzi_btn[3][1],2);
	lhanzi_btn[3][2] = ct_button_add(BTN_FIELD,BTN_32,32,128,32);
	ct_button_setVariable(lhanzi_btn[3][2],3);
	lhanzi_btn[3][3] = ct_button_add(BTN_FIELD,BTN_32,32,160,32);
	ct_button_setVariable(lhanzi_btn[3][3],4);
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_lhanzi_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_lhanzi_button_back,g_lhanzi_launchBack);
	ct_button_addContentIma(dw_lhanzi_button_back,(void *)arrow_b_Sprite,0,0,32,32);

	dw_lhanzi_button_next = ct_button_add(BTN_GREY,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_lhanzi_button_next,g_lhanzi_launchNext);
	ct_button_addContentIma(dw_lhanzi_button_next,(void *)arrow_r_Sprite,0,0,32,32);

	
	/* Init next state - Unique state */
	g_lhanzi_Start();
	if (lhanzi_finished == 0)
		g_lhanzi_setType(lhanzi_state, ct_HALhanzi_word_lenHanzi(ct_state_var_getStr()));
}

void ct_g_lhanzi_Finish()
{
	// Finish gfx
	PA_DeleteGfx(0,lhanzi_templ_gfx);
	if (lhanzi_finished != 0) {
		PA_DeleteGfx(0,dw_lhanzi_sealgfx);
	
		// Save game (if...)
		if (lhanzi_finished == 2) {									      
			ct_utillib_saveSRAM(data_profile);
		}			
	}		

   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
	
} 


void ct_g_lhanzi_Input()
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
	
	// Check stylus painting
	if (Stylus.Held) {
   	if (ct_textzone_inArea(Stylus.X,Stylus.Y))
   		ct_textzone_paint(Stylus.X,Stylus.Y);
 	} else {
 	   ct_textzone_reset();
 	}   
}

void ct_g_lhanzi_State()
{
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
	ct_anihanzi_manage();
	
	// Sound!
	if (lhanzi_delaysound > 0) {
		lhanzi_delaysound--;
	} else {
		if (lhanzi_playsound) {
		   // OK, let's play this sound and only this sound
			lhanzi_playsound = 0; 
			// Play
			if (ct_hwsound_preloadPinyin(ct_HALhanzi_getPinyin(ct_HALhanzi_phrase_getHanzi(ct_state_var_getStr(),lhanzi_actsound)))) {
			   void *sound;
			   sound = ct_hwsound_getPinyin();
			   if (sound != NULL) {
		   		PA_PlaySoundEx(0,sound,ct_hwsound_getPinyinSize(),(lhanzi_actual == lhanzi_actsound)?127:96,11025,0);		
				}   
			}		   
		} else {
		   // Only if there are more sounds...
	   	if (lhanzi_actsound < ct_HALhanzi_phrase_lenHanzi(ct_state_var_getStr())) {
			   if (!PA_SoundChannelIsBusy(0)) {
		   		lhanzi_actsound++; // Next!!!!!
	   			lhanzi_playsound = 1;
		 		}  	
 			}  	
		}
	}		   
}
