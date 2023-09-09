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


/* Quiz - Exams */
/*
	State[0] = TYPE - Type of Quiz. SUBGAME_EXLISTENQUIZ / CARD
	State[1] = n - number of previous word
	State[2] = n - number of the exam (from State[2] to 1 - 0 is "end")
	State[3] = n - maximum number of exams, usually 10
	State[4] = score - Show the score
	StateArr = previous words. -1 if no word.
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
#include "ct_hwmeaning.h"
#include "ct_hwsound.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define EX_LISTEN_ 0
#define EX_LISTEN_TEST 1
#define EX_LISTEN_RESULTS 2

/* Variables - Internal State */
////////////////////////////////
int ex_listen_state;
int ex_listen_actual;
int ex_listen_size;
int ex_listen_phrase;
int ex_listen_word;
int ex_listen_OK;
int ex_listen_finished;

int ex_listen_maxbtn;

int ex_listen_actsound;
int ex_listen_playsound;
int ex_listen_delaysound;

int dw_listen_button_back;
int dw_listen_button_next;
int dw_listen_button_pinyin;

int dw_listen_button_items[4];

int dw_listen_sealpal;
int dw_listen_seal;
u16 dw_listen_sealgfx;

int dw_listen_pointpal;
int dw_listen_point[2];
u16 dw_listen_pointgfx[2];


/* Internal Functions */ 
////////////////////////

int _listen_testIn(int val,int arr[4], int max) {
   int i;
   int res = 0;
   if (max > 4)
   	max = 4;
   	
   for (i = 0; i < max; i++) {
   	if (arr[i] == val)
   		res = 1;
	}   
   	
   return res;
}

void ex_listen_paintScore() {
   int i;
   
   // Paint "OK!"Score
   for (i=0; i < ct_state_var_getInt(4); i++) {
      ct_utillib_drawRectangle(1,4 + (8 * i) + (4 * i),4,4 + (8 * i) + (4 * i) + 8,8,2);
      // PA_Put8bitPixel(s,x,y,c);
   }   
   // Paint "Nope..." Score
   for (i=0; i < ((10 - ct_state_var_getInt(2)) - ct_state_var_getInt(4)); i++) {
      ct_utillib_drawRectangle(1,244 - (8 * i) - (4 * i),4, 244 - (8 * i) - (4 * i) + 8,8,3);
   }      
}	   

int ex_listen_noRepeat(int r) {
   
   // Last phrase?
   if (r == ct_state_var_getInt(1))
   	return 1;
   	
   // Repeated value? - Not relevant for this type of lesson
   //if (ct_state_var_inArrInt(r))
   //	return 1;
   	
   return 0;
}   

void ex_listen_Start() {
   int w[4];
   int maxprob;
   int maxphrase;
   int rphr;
   int rword;
   int r;
   int found;
   int sel_size;
   int sel_hanzi[4];
   int i;
   int cont;
   int repeatedhanzi;
   char hanz[6 * 4]; // 4, max letters, 5 per letter
   
   // Always (end or not end), show the actual score ;-)
   ex_listen_paintScore();
   
   // If it is the end... show the "end phase"
   if (ct_state_var_getInt(2) == 0) {
      ex_listen_finished = 1;
  	   // "Finished" Seal and "Exam finished!!!" :-D - Lower && ...OK, no Upper
  	   for (i = 0; i < 1/*2*/; i++) {
			dw_listen_sealpal = ct_graphics_getPalette(i);
			PA_LoadSpritePal(i,dw_listen_sealpal,(void*)seal_Pal);
			dw_listen_seal = ct_graphics_getSprite(i);
			dw_listen_sealgfx = PA_CreateGfx(i,(void*)logo_seal_Sprite, OBJ_SIZE_64X64,1);
			PA_CreateSpriteFromGfx(i,dw_listen_seal,dw_listen_sealgfx,OBJ_SIZE_64X64,1,dw_listen_sealpal,
										96,64);
	   	PA_SetSpritePrio(i,dw_listen_seal,0);
		}
		// Hide Buttons! Make Smiley Dance! ^.^
		for (i = 0; i < 4; i++) {   
		   ct_button_hide(dw_listen_button_items[i]);
		}   
		ct_smiley_setState(SMILEYMOOD_HAPPYJUMP);
		// Show Results!!!! In hanzi, of course ^_^
		dw_listen_pointpal = ct_graphics_getPalette(1);
		PA_LoadSpritePal(1,dw_listen_pointpal,(void*)hanzi_Pal);
	   dw_listen_point[0] = ct_graphics_getSprite(1);
		dw_listen_pointgfx[0] = PA_CreateGfx(1,(void*)btn64yeah_Sprite, OBJ_SIZE_64X64,1);
		PA_CreateSpriteFromGfx(1,dw_listen_point[0],dw_listen_pointgfx[0],OBJ_SIZE_64X64,1,dw_listen_pointpal,
										44,48);
		PA_SetSpritePrio(1,dw_listen_point[0],3);
	   dw_listen_point[1] = ct_graphics_getSprite(1);
		dw_listen_pointgfx[1] = PA_CreateGfx(1,(void*)btn64nope_Sprite, OBJ_SIZE_64X64,1);
		PA_CreateSpriteFromGfx(1,dw_listen_point[1],dw_listen_pointgfx[1],OBJ_SIZE_64X64,1,dw_listen_pointpal,
										44 + 64 + 48,48);
		PA_SetSpritePrio(1,dw_listen_point[1],3);
				
		ct_HALhanzi_paintHanzi64x64(1,ct_HALhanzi_word_getHanzi(ct_utillib_getChineseNumber(ct_state_var_getInt(4)),1),
												52, 54); // GOOD!
		ct_HALhanzi_paintHanzi64x64(1,ct_HALhanzi_word_getHanzi(ct_utillib_getChineseNumber(10 - ct_state_var_getInt(4)),1),
												48 + 64 + 52, 54); // hmmmm bad :-P
		// Nothing more to do here...
		ex_listen_state = EX_LISTEN_RESULTS;
		return;
   }
	
	// Choose a hanzi... from the "pool" of all the "listen" lessons.
	// For simplicity, "no repeat" gets limited to just the phrase
   repeatedhanzi = 1;
   while (repeatedhanzi) {
		maxphrase = ct_lesson_getNumPhraseBasic();
		rphr = PA_RandMinMax(1,maxphrase);
		if (maxphrase > 1)
			repeatedhanzi = ex_listen_noRepeat(rphr);
		else
			repeatedhanzi = 0;
	}
	ct_state_var_setInt(1,rphr);	
	ct_state_var_setStr(ct_lesson_askPhrase(rphr));
	maxprob = ct_HALhanzi_phrase_lenWord(ct_state_var_getStr());
	rword = PA_RandMinMax(1,maxprob); 
	PA_CopyText(hanz,ct_HALhanzi_phrase_getWord(ct_state_var_getStr(), rword));
	sel_size = ct_HALhanzi_word_lenHanzi(hanz);
	
	// Discover the actual position of the selected word
   w[0] = 0;
   w[1] = ct_lesson_lenWords(1);
   w[2] = w[1] + ct_lesson_lenWords(2);
   w[3] = w[2] + ct_lesson_lenWords(3);
	r = -1;
	for (i = 1; i <= ct_lesson_lenWords(sel_size); i++) {
	   if (PA_CompareText(hanz,ct_lesson_askWord(sel_size,i))) {
	   	r = i;// + w[sel_size - 1];
	   	break;
	 	}  	
	}
	sel_hanzi[0] = r;
	
	// Mark the variables that will be used later
	ex_listen_phrase = rphr;
	ex_listen_word = rword;
	ex_listen_size = sel_size;
	ex_listen_actual = sel_hanzi[0];
	
   // Now, choose the others - Special Case
   if (ct_lesson_lenWords(sel_size) < 4) {
      ex_listen_maxbtn = ct_lesson_lenWords(sel_size); // Only these options...
      // Aiyaaa.... ok, show only the options located in this lesson
      for (i = 0; i < ex_listen_maxbtn; i++) {
         if ((ex_listen_actual + i) > ex_listen_maxbtn)
            sel_hanzi[i] = ex_listen_actual + i - ex_listen_maxbtn;
         else
         	sel_hanzi[i] = ex_listen_actual + i;
      }
	} else {
	   ex_listen_maxbtn = 4; // Max options!!!
	   // Good! "Jumping" Algorithm.
	   for (i = 1; i < 4; i++) {
	      r = PA_RandMinMax(1,ct_lesson_lenWords(sel_size) - 1); // Avoid return to same position
	      cont = ( (sel_hanzi[i-1] + r - 1) % ct_lesson_lenWords(sel_size) ) + 1; // Cycle - (cont + r) MOD size [1..n]
	      if (_listen_testIn(cont,sel_hanzi,i)) {
	        // Repeated position... scan for "clear" position
	        found = 0; 
	        while (!found) { // No infinite loop, since for size = 4 it will end sometime :-)
	           cont = (cont % ct_lesson_lenWords(sel_size)) + 1; // Increment in a cycle
	           if (!_listen_testIn(cont,sel_hanzi,i))
	           		found = 1;
	        }   
	     	}  
			sel_hanzi[i] = cont; 
	   }   
	}   
   
   // And fill the buttons -:)
   r = PA_RandMinMax(0,ex_listen_maxbtn - 1);
   ex_listen_OK = dw_listen_button_items[r]; // The ID of the right button!
   for (i = 0; i < ex_listen_maxbtn; i++) {
      cont = (r + i) % ex_listen_maxbtn;
      ct_button_setVariable(dw_listen_button_items[cont],sel_hanzi[i]);
   }   
   
   // Now... hide buttons if there is a problem with the quiz
   for (i = ex_listen_maxbtn; i < 4; i++) {
      ct_button_hide(dw_listen_button_items[i]);
   }   
   
   // Debug Purposes
   /*char debugstr[255];
   strcpy(debugstr,"");
   for (i = 0; i < 4; i++) {
      sprintf(debugstr,"[%s]%d(%d)",
						(ex_listen_OK==i)?"Y":"N",
						ct_button_getVariable(dw_listen_button_items[i]),
						sel_size);
	   ct_button_Text(dw_listen_button_items[i],debugstr,3);
	}*/
}   

void ex_listen_PutOptions() {
   
   char hanz[6 * 4]; // 4, max letters, 5 per letter
   int i;
   
   strcpy(hanz,"");
   strcpy(hanz,ct_lesson_askWord(ex_listen_size,ex_listen_actual));
   
   // Depends on the type of Quiz, write in the UPPER part
   switch (ct_state_var_getInt(0)) {
      case SUBGAME_EXLISTENQUIZ :
         ct_HALhanzi_paintHide(ex_listen_word);
			ct_HALhanzi_paint(1,ct_state_var_getStr(),50);
			ct_HALhanzi_paintHide(-1);
         break;
   }   
   
   // And now, writes in the LOWER part
   for (i = 0; i < ex_listen_maxbtn; i++) {
	   switch (ct_state_var_getInt(0)) {
	      case SUBGAME_EXLISTENQUIZ : // Paint the hanzi
	      	PA_CopyText(hanz,
								ct_lesson_askWord(ex_listen_size,
														ct_button_getVariable(dw_listen_button_items[i])
														)
								);
				ct_HALhanzi_paintX(0,hanz, 
											ct_button_getX(dw_listen_button_items[i]) + 
												(ct_button_getSizeX(dw_listen_button_items[i]) >> 1), 
											ct_button_getY(dw_listen_button_items[i]) + 
												(ct_button_getSizeY(dw_listen_button_items[i]) >> 1)
											);	      
				break;         
	   }         
   }      
}   

void ex_listen_PutResults() {
   int i;
   char hanz[6 * 4]; // 4, max letters, 5 per letter   
   
	// First, clean button contents
	ct_button_cleanContent(1);

	// Now, write hanzi AND pinyin
   for (i = 0; i < ex_listen_maxbtn; i++) {
     	PA_CopyText(hanz,
						ct_lesson_askWord(ex_listen_size,
												ct_button_getVariable(dw_listen_button_items[i])
												)
						);
		ct_HALhanzi_paintX(0,hanz, 
									ct_button_getX(dw_listen_button_items[i]) + 
										(ct_button_getSizeX(dw_listen_button_items[i]) >> 1), 
									ct_button_getY(dw_listen_button_items[i]) + 
										(ct_button_getSizeY(dw_listen_button_items[i]) >> 2) + 
										(ct_button_getSizeY(dw_listen_button_items[i]) >> 3)
								);
		ct_HALhanzi_setPinyinSize((ex_listen_size < 3)?3:2);
		ct_HALhanzi_paintPinyinX(0,hanz,
									ct_button_getX(dw_listen_button_items[i]) + 
										(ct_button_getSizeX(dw_listen_button_items[i]) >> 1), 
									ct_button_getY(dw_listen_button_items[i]) + 
										(ct_button_getSizeY(dw_listen_button_items[i]) >> 1) + 
										(ct_button_getSizeY(dw_listen_button_items[i]) >> 3)
								);   	      								
	}			
	// Also, write...ok, write he pinyin in the upper part ;-)
	ct_HALhanzi_paintPinyin(1,ct_state_var_getStr(), 110);
}

void ex_listen_Next() {
   if (ex_listen_state == EX_LISTEN_RESULTS) {
      // Shut up -:)
	   if (PA_SoundChannelIsBusy(0))
      	PA_StopSound(0);
		// OK, continue
		ct_state_var_setInt(2,ct_state_var_getInt(2) - 1); // Less Exams to do...		
		if (ct_state_var_getInt(2) >= 0){ 
		   // Next Exam
			ct_state_assignState(GAME_EX_LISTEN); 
		} else {
		   // Results - TODO
			ct_state_assignState(GAME_MAINMENU);
		}	

   }   
}   

void ex_listen_Button() {
   
   // First, Test if we have to go to the next lesson :-)
   ex_listen_Next();
   
   if (ex_listen_state == EX_LISTEN_TEST) {
	   // This is the right button?
   	if (ex_listen_actual == ct_button_getVariable(ct_button_getHandler())) {
      	// Cool! - Let's make Smiley Jump!!! ^.^
			ct_smiley_setState(SMILEYMOOD_HAPPYJUMP);      
			// One point more for the score!!!!
			ct_state_var_setInt(4,ct_state_var_getInt(4) + 1);
	   } else {
   	   // Aiyaaaa... V_V
			ct_smiley_setState(SMILEYMOOD_AIYAAAA);            
			// Mark this button as red
			ct_button_changetype(ct_button_getHandler(),BTN_RED);
   	}   
	   // Set the correct button green ^.^
   	ct_button_changetype(ex_listen_OK,BTN_GREEN);
   
	   // And now, write results, and advance state
   	ex_listen_PutResults();
   	ex_listen_state = EX_LISTEN_RESULTS;
   	
   	// Update sound system, talk a bit more.
	   if (PA_SoundChannelIsBusy(0))
      	PA_StopSound(0);
		ex_listen_actsound = 1;
		ex_listen_playsound = 1;
		ex_listen_delaysound = 20;
	}   
}

void ex_listen_ManageSound() {
   void *sound;
	// Sound!
	if (ex_listen_delaysound > 0) {
		ex_listen_delaysound--;
	} else {
		if (ex_listen_playsound) {
		   // OK, let's play this sound and only this sound
			ex_listen_playsound = 0; 
			// Play
	   	ct_hwsound_preloadListening(data_lessonnumber,ex_listen_phrase);
	   	sound = ct_hwsound_getListening();
	   	if (sound != NULL)
	   		PA_PlaySoundEx(0,sound,ct_hwsound_getListeningSize(),127,11025,0);		
			}		   
	}		
}		   

/* Functions */ 
///////////////

void ct_ex_listen_Start()
{
   int i;

	// Internal state
	ex_listen_state = EX_LISTEN_;
	ex_listen_finished = 0;
	ex_listen_actsound = 65535; // No sounds! ;-)
	ex_listen_playsound = 0;
	ex_listen_delaysound = 0;

	// Inits the Graphic mode for the listening
	ct_graphics_initGraphicMode(SCREENMODE_INTRO);
	
	/* Graphics */
	// Just buttons, and smiley
	ct_button_init();	
	ct_utillib_init();
	
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMALMOVE);
	
	// Elements of the screen
					
	/* Variables*/

	for (i = 0; i < 4; i++) {
	   dw_listen_button_items[i] = ct_button_add(BTN_GREY,112,BTN_64,
											(i < 2)?8:136,
											(i % 2 == 0)?(32+8):(32+80+8));
		ct_button_addHandler_Click(dw_listen_button_items[i],ex_listen_Button);	
	}   
	//dw_listen_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	//ct_button_addHandler_Click(dw_listen_button_back,g_quiz_launchBack);
	//ct_button_addContentIma(dw_listen_button_back,arrow_b_Sprite,0,0,32,32);
	
	/* Init next state - Unique state */
	ex_listen_Start();
}

void ct_ex_listen_Finish()
{
	// Gfx
	if (ex_listen_finished) {
		PA_DeleteGfx(0,dw_listen_sealgfx);
		
		// Update Score - TODO
		if (ct_state_var_getInt(4) > 
				data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][ct_state_var_getInt(0)])
			data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][ct_state_var_getInt(0)] = ct_state_var_getInt(4);
		
		// Save game
		ct_utillib_saveSRAM(data_profile);
	}			

   PA_WaitForVBL();
	// Just buttons
	ct_button_finish();
	ct_smiley_finish();	
} 


void ct_ex_listen_Input()
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
		// Screen Management ... next lesson?
		ex_listen_Next();
	}
}

void ct_ex_listen_State()
{   
	// Elements state
	ct_button_Manage();
	ct_smiley_Manage();
		
	switch (ex_listen_state) {
	   case EX_LISTEN_:
	      // Now, fill the Contents...
	      ex_listen_PutOptions();
	      ex_listen_state = EX_LISTEN_TEST;
	      if (ct_state_var_getInt(0) == SUBGAME_EXLISTENQUIZ) {
	         // Read the phrase
	         ex_listen_actsound = 1;
	         ex_listen_playsound = 1;
	         ex_listen_delaysound = 20;
	      }   
	      break;
	   case EX_LISTEN_TEST:
	      // Wait for input...and manage sounds...
	      ex_listen_ManageSound();
	      break;
	   case EX_LISTEN_RESULTS:
	      // Spell the pinyin, if sound has to be managed
	      ex_listen_ManageSound();
	      break;	      
	}   
}
