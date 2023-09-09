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


/* Hanzi - Gram */
/*
	State[0] = n - Section number of the lesson
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

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define LGRAM_ 0       // Shows "buttons" and "text"
#define LGRAM_LESSON 1 // Let smiley do his work
#define LGRAM_END 2 // Finished...

/* Variables - Internal State */
////////////////////////////////
int lgram_state;
int lgram_substate;
int lgram_finished;
int lgram_actualtext;
int lgram_maxtext;
int lgram_actualphrase;
int lgram_maxphrase;

int dw_lgram_button_back;
int dw_lgram_button_next;
int dw_lgram_button_gram[8];

int dw_lgram_sealpal;
int dw_lgram_seal;
u16 dw_lgram_sealgfx;


/* Internal Functions */ 
////////////////////////

void g_lgram_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
}

void g_lgram_launchNext(){
   // Finishes managing the semaphore
   if (ct_utillib_waitSemaphore()) {
      ct_button_bh_blipContentStop(dw_lgram_button_next);
   	ct_utillib_signalSemaphore();
   	ct_smiley_endText();
	}   

   // Smiley talking?
   lgram_actualtext++;
   lgram_maxtext--;
   
   // Next state?
   if (lgram_maxtext <= 0) {
		ct_state_assignState(GAME_LGRAM); // Himself! :-D
	   ct_state_var_setInt(0,ct_state_var_getInt(0) + 1);
   }   
}

/*void g_lesson_nextText(){
}*/

void g_lgram_Start() {
   int ntext;
   int i;
   
   // Check if this is the end of the lesson
   ntext = ct_lesson_getNumText(ct_state_var_getInt(0));
   
   // Lesson Finished! Indicate it
   if (ntext == -1) {
      lgram_finished = 1;
      // No next button
      ct_button_hide(dw_lgram_button_next);
      // "Finished" Seal :-D
		dw_lgram_sealpal = ct_graphics_getPalette(0);
		PA_LoadSpritePal(0,dw_lgram_sealpal,(void*)seal_Pal);
		dw_lgram_seal = ct_graphics_getSprite(0);
		dw_lgram_sealgfx = PA_CreateGfx(0,(void*)logo_seal_Sprite, OBJ_SIZE_64X64,1);
		PA_CreateSpriteFromGfx(0,dw_lgram_seal,dw_lgram_sealgfx,OBJ_SIZE_64X64,1,dw_lgram_sealpal,
										96,64);
   	PA_SetSpritePrio(0,dw_lgram_seal,0);
   	// Smiley says, hi!
   	ct_smiley_setState(SMILEYMOOD_HAPPYJUMP);   	
   	ct_smiley_setText(ct_const_getText(TXT_LESSON_END,data_gameData[data_profile].language),-2,3,0);
   	// Directly ends
   	lgram_state = LGRAM_END;
   	// Flags lesson as finished 
		if (!ct_utillib_bitOne16(data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16],
									   (data_lessonnumber - 1) % 16)) {		
	   	data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16] =
   			ct_utillib_bitSet16(
   				data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16],
   				(data_lessonnumber - 1) % 16
				);
			lgram_finished = 2;
		}			
   }
	   
   // OK... create and draw the buttons and contents - TODO 5 & 7
   lgram_maxphrase = ct_lesson_getNumPhrase(ct_state_var_getInt(0));   
   lgram_actualphrase = ct_lesson_getIdxPhrase(ct_state_var_getInt(0));
   if (lgram_maxphrase)
	for (i = 0; i < lgram_maxphrase; i++) {
	   // "Normal" cases
	   int x;
	   int y;
	   int ii;
	   if (lgram_maxphrase > 3) {
	     x = (i < (lgram_maxphrase >> 1))?32:160;
	     ii = i % (lgram_maxphrase >> 1);
	     y = 32 + // Offset
		  		(ii * (160 / (lgram_maxphrase >> 1)) ) + // Sector of this button
				((160 / (lgram_maxphrase >> 1)) >> 1) - // Centering button
				16; // Button size
		} else {
		  x = 96;
	     y = 32 + // Offset
		  		(i * (160 / lgram_maxphrase) ) + // Sector of this button
				((160 / lgram_maxphrase) >> 1) - // Centering button
				16; // Button size
		}     
	   dw_lgram_button_gram[i] = ct_button_add(BTN_NONE,64,BTN_32,x,y);
	   
	}   
	// Buttons created, time to draw the contents
	for (i = 0; i < lgram_maxphrase; i++) {	
	   // Debug - ct_button_Text(dw_lgram_button_gram[i],ct_utillib_int4hex(lgram_actualphrase + i + 1),3);
		ct_HALhanzi_paintX(0,ct_lesson_askPhrase(lgram_actualphrase + i + 1),
									ct_button_getX(dw_lgram_button_gram[i]) + (ct_button_getSizeX(dw_lgram_button_gram[i]) >> 1), 
									ct_button_getY(dw_lgram_button_gram[i]) + (ct_button_getSizeY(dw_lgram_button_gram[i]) >> 2));
		ct_HALhanzi_paintPinyinX(0,ct_lesson_askPhrase(lgram_actualphrase + i + 1),
									ct_button_getX(dw_lgram_button_gram[i]) + (ct_button_getSizeX(dw_lgram_button_gram[i]) >> 1), 
									ct_button_getY(dw_lgram_button_gram[i]) + (16 + (ct_button_getSizeY(dw_lgram_button_gram[i]) >> 1)));
	}		
   
   // And now, put the first words in the mounth of smiley (in its place)
   lgram_maxtext = ntext;
   lgram_actualtext = ct_lesson_getIdxText(ct_state_var_getInt(0));
   
}   


/* Functions */ 
///////////////

void ct_g_lgram_Start()
{
	// Internal state
	lgram_state = LGRAM_;
	lgram_substate = 0;
	lgram_actualtext = 0;
	lgram_maxtext = 0;	
	lgram_finished = 0;

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
	ct_header_TextID(0,TXT_LESSON_GRAM_,data_gameData[data_profile].language);
	
	// Elements of the screen
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_lgram_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_lgram_button_back,g_lgram_launchBack);
	ct_button_addContentIma(dw_lgram_button_back,(void *)arrow_b_Sprite,0,0,32,32);
		
	dw_lgram_button_next = ct_button_add(BTN_GREY,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_lgram_button_next,g_lgram_launchNext);
	ct_button_addContentIma(dw_lgram_button_next,(void *)arrow_r_Sprite,0,0,32,32);
	
	/* Init next state - Unique state */
	g_lgram_Start();
}

void ct_g_lgram_Finish()
{
	// Del Gfx
	if (lgram_finished != 0) {
		PA_DeleteGfx(0,dw_lgram_sealgfx);
	
		// Save game (if...)
		if (lgram_finished == 2) {
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


void ct_g_lgram_Input()
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

void ct_g_lgram_State()
{
	int val;
   
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
		
	switch (lgram_state) {
	   case LGRAM_:
			//ct_HALhanzi_paint(0,ct_state_var_getStr(),70);
			//ct_HALhanzi_paintPinyin(0,ct_state_var_getStr(), 130);
			//ct_smiley_setText(ct_lesson_askTranslationPhrase(ct_state_var_getInt(1)-1),-1,3,0);
	      lgram_state = LGRAM_LESSON;
	      break;
	   case LGRAM_LESSON:
	      ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMAL);
	      val = ct_smiley_setTextExt2(ct_lesson_strText(lgram_actualtext),
													-2,3,1);
			if (val == 3)
				ct_button_bh_blipContentStart(dw_lgram_button_next);
	      break;
	   case LGRAM_END:
		   break;	      
	}   
}
