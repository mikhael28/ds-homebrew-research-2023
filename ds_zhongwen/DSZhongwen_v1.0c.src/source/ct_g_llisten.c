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


/* Hanzi - Listen */
/*
	State_str = hanzi - phrase;
	State[0] = 0 - Individual phrase, 1 - Part of a lesson
	State[1] = n - Number of the phrase inside a lesson
	State[2] = 0 - Normal mode, 1 - "Play", infinite mode
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
#include "ct_hwsound.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define LLISTEN_ 0
#define LLISTEN_HEAR 1
#define LLISTEN_HEAREND 2
#define LLISTEN_END 3

/* Variables - Internal State */
////////////////////////////////
int llisten_state;
int llisten_actual;
int llisten_finished;

int dw_llisten_button_back;
int dw_llisten_button_next;
int dw_llisten_button_play;
int dw_llisten_button_playd1;
int dw_llisten_button_playd2;

int dw_llisten_sealpal;
int dw_llisten_seal;
u16 dw_llisten_sealgfx;


/* Internal Functions */ 
////////////////////////

void g_llisten_launchBack(){
   if (llisten_finished == 0)
   	llisten_finished = 1; // For finishing purposes
	ct_state_assignState(GAME_MAINMENU);
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
}

void g_llisten_launchNext(){
   // In "continuous mode", if the next one is empty, go back to the beginning!
   ct_state_var_setStr(ct_lesson_askPhrase(ct_state_var_getInt(1)));
   if (PA_CompareText("",ct_state_var_getStr())) {
      if (ct_state_var_getInt(2))
      	ct_state_var_setInt(1,1);
   }   

   // OK, launch!!!!
	ct_state_assignState(GAME_LLISTEN); // Himself! :-D
}

void g_llisten_Start() {
   // First, get the actual phrase
   int res = 0;
   int stop = 0;
   while ((!res) && (!stop)) {
      // Try to get the actual phrase
      ct_state_var_setStr(ct_lesson_askPhrase(ct_state_var_getInt(1)));
      if (PA_CompareText("",ct_state_var_getStr())) {
         // Finished in no more phrases...
         ct_state_var_setInt(1,ct_state_var_getInt(1) + 1);
         stop = 1;
      } else {
		   // Assign the actual phrase - done in ct_state_var_setStr() :-P
		   // OK, next phrase!
		   ct_state_var_setInt(1,ct_state_var_getInt(1) + 1);
		   res = 1; // end
		}			   
   }   
   
   // Now, Sets up the environment
   if ((stop) || (!ct_state_var_getInt(0))) {
      llisten_finished = 1;
      // No next button, no play button
      ct_button_hide(dw_llisten_button_next);
      ct_button_hide(dw_llisten_button_play);
      // "Finished" Seal :-D
		dw_llisten_sealpal = ct_graphics_getPalette(0);
		PA_LoadSpritePal(0,dw_llisten_sealpal,(void*)seal_Pal);
		dw_llisten_seal = ct_graphics_getSprite(0);
		dw_llisten_sealgfx = PA_CreateGfx(0,(void*)logo_seal_Sprite, OBJ_SIZE_64X64,1);
		PA_CreateSpriteFromGfx(0,dw_llisten_seal,dw_llisten_sealgfx,OBJ_SIZE_64X64,1,dw_llisten_sealpal,
										96,64);
   	PA_SetSpritePrio(0,dw_llisten_seal,0);
   	// Smiley says, hi!
   	ct_smiley_setState(SMILEYMOOD_HAPPYJUMP);   	
   	ct_smiley_setText(ct_const_getText(TXT_LESSON_END,data_gameData[data_profile].language),-2,3,0);
   	// Directly ends
   	llisten_state = LLISTEN_END;
   	// Flags lesson as finished 
		if (!ct_utillib_bitOne16(data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16],
									   (data_lessonnumber - 1) % 16)) {   	
	   	data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16] =
   			ct_utillib_bitSet16(
   				data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16],
	   			(data_lessonnumber - 1) % 16
				);
			llisten_finished = 2;
		}				
   }   
}

void g_llisten_playLesson(int play) {
   void *sound;
   
   if (!play) {
	   if (PA_SoundChannelIsBusy(0))
      	PA_StopSound(0);
   }
	if (play) {
	   ct_hwsound_preloadListening(data_lessonnumber,llisten_actual);
	   sound = ct_hwsound_getListening();
	   if (sound != NULL)
	   	PA_PlaySoundEx(0,sound,ct_hwsound_getListeningSize(),127,11025,0);		
	}		   
}   

void g_llisten_setContinuous() {
   // Basic state
   ct_state_var_setInt(2,ct_state_var_getInt(2) ^ 1);
   ct_button_changetype(ct_button_getHandler(),ct_state_var_getInt(2)?BTN_FIELDSEL:BTN_FIELD);
   
   // Setting up...
   if (ct_state_var_getInt(2)) {
      data_ignorelid = 1; // No pausing if lid is closed
      llisten_state = LLISTEN_HEAREND; // End this if the phrase finishes
   } else {
      data_ignorelid = 0;
      data_lidopen = 1;
      llisten_state = LLISTEN_HEAR; // "Normal" system
   }   
}   


/* Functions */ 
///////////////

void ct_g_llisten_Start()
{

	// Internal state
	llisten_state = LLISTEN_;
	llisten_actual = ct_state_var_getInt(1);
	llisten_finished = 0;

	// Inits the Graphic mode for the listening
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
	ct_header_TextID(0,TXT_LESSON_LISTEN_,data_gameData[data_profile].language);
	
	// Elements of the screen
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_llisten_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_llisten_button_back,g_llisten_launchBack);
	ct_button_addContentIma(dw_llisten_button_back,(void *)arrow_b_Sprite,0,0,32,32);
		
	dw_llisten_button_next = ct_button_add(BTN_GREY,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_llisten_button_next,g_llisten_launchNext);
	ct_button_addContentIma(dw_llisten_button_next,(void *)arrow_r_Sprite,0,0,32,32);

	// Dummy buttons
	dw_llisten_button_playd1 = ct_button_add(BTN_FIELD,32,BTN_32,256,192);
	dw_llisten_button_playd2 = ct_button_add(BTN_FIELDSEL,32,BTN_32,256,192);

	dw_llisten_button_play = ct_button_add(ct_state_var_getInt(2)?BTN_FIELDSEL:BTN_FIELD,32,BTN_32,256 - 32,192 - 32);
	ct_button_addContentIma(dw_llisten_button_play,(void *)arrow_play_Sprite,0,0,32,32);
	ct_button_addHandler_Click(dw_llisten_button_play,g_llisten_setContinuous);

	
	/* Init next state - Unique state */
	g_llisten_Start();
}

void ct_g_llisten_Finish()
{
	// Finish gfx, game
	if (llisten_finished != 0) {
		PA_DeleteGfx(0,dw_llisten_sealgfx);
		
		// Save game (only if...)
		if (llisten_finished == 2) {									      
			ct_utillib_saveSRAM(data_profile);
		}			
	}	

   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
	
	// Finish sound
	g_llisten_playLesson(0);
	
	// Cleans "No close" variable.
	if (llisten_finished != 0) {
		data_ignorelid = 0;
		data_lidopen = 1;
	}	
} 


void ct_g_llisten_Input()
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
		// Screen Management ... Replays sound - TODO
		g_llisten_playLesson(1);
	}
}

void ct_g_llisten_State()
{
   
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
		
	switch (llisten_state) {
	   case LLISTEN_:
			ct_HALhanzi_paint(0,ct_state_var_getStr(),70);
			ct_HALhanzi_paintPinyin(0,ct_state_var_getStr(), 130);
			ct_smiley_setText(ct_lesson_askTranslationPhrase(ct_state_var_getInt(1)-1),-1,3,0);
			g_llisten_playLesson(1);
			if (ct_state_var_getInt(2))
		      llisten_state = LLISTEN_HEAREND;
		   else llisten_state = LLISTEN_HEAR;
	      break;
	   case LLISTEN_HEAR:
	      break;
	   case LLISTEN_HEAREND:
	      // No sound, next!!!!!!
	      if (!PA_SoundChannelIsBusy(0))
	      	g_llisten_launchNext();
	      break;	      
	   case LLISTEN_END:
	      break;	      
	}   
}
