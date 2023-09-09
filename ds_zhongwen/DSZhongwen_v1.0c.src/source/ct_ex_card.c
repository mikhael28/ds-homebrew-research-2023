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


/* Card - Exams */
/*
	State[0] = TYPE - Type of Quiz. SUBGAME_EXCARDHANZI / PINYIN / MEAN
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
#include "ct_textzone.h"
#include "ct_hwmeaning.h"
#include "ct_hwsound.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define EX_CARD_ 0
#define EX_CARD_TEST 1
#define EX_CARD_YESNO 2
#define EX_CARD_RESULTS 3

#define EX_CARD_LEN_CHAR 20

/* Variables - Internal State */
////////////////////////////////
int ex_card_state;
int ex_card_actual;
int ex_card_size;
int ex_card_OK;
int ex_card_finished;

int ex_card_actsound;
int ex_card_playsound;
int ex_card_delaysound;

int dw_card_pal;
int dw_card_templwrite[4];
u16 dw_card_templ_gfx;

int dw_card_button_check;
int dw_card_button_yes;
int dw_card_button_no;
int dw_card_button_clean;
int dw_card_button_pinyin;

char dw_card_text[EX_CARD_LEN_CHAR];
int dw_card_textsize;

int dw_card_sealpal;
int dw_card_seal;
u16 dw_card_sealgfx;

int dw_card_pointpal;
int dw_card_point[2];
u16 dw_card_pointgfx[2];



/* Internal Functions */ 
////////////////////////

void ex_card_paintScore() {
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

int ex_card_noRepeat(int r) {
   char hanz[6 * 4]; // 4, max letters, 5 per letter
   
   // Last hanzi?
   if (r == ct_state_var_getInt(1))
   	return 1;
   	
   // Repeated value?
   if (ct_state_var_inArrInt(r))
   	return 1;

   // No meaning?
   if (ct_state_var_getInt(0) == SUBGAME_EXCARDMEAN) {
	   strcpy(hanz,"");
   	strcpy(hanz,ct_lesson_askWord(ex_card_size,ex_card_actual));
	   if (PA_CompareText("",ct_hwmeaning_getHanzi(hanz,LANG_ENG)))
   		return 1;
   } 		
   	
   return 0;
}   

void ex_card_Start() {
   int w[4];
   int maxprob;
   int r;
   int found;
   int sel_size;
   int sel_hanzi[4];
   int i;
   int x;
   int y;
   int size;
   int cont;
   int repeatedhanzi;
   
   // Inits the internal text system
   for (i = 0; i < EX_CARD_LEN_CHAR; i++)
   	dw_card_text[i] = '\0';
   dw_card_textsize = 0;
   
   // Always (end or not end), show the actual score ;-)
   ex_card_paintScore();

   
   // If it is the end... show the "end phase"
   if (ct_state_var_getInt(2) == 0) {
      ex_card_finished = 1;
  	   // "Finished" Seal and "Exam finished!!!" :-D - Lower && ...OK, no Upper
  	   for (i = 0; i < 1/*2*/; i++) {
			dw_card_sealpal = ct_graphics_getPalette(i);
			PA_LoadSpritePal(i,dw_card_sealpal,(void*)seal_Pal);
			dw_card_seal = ct_graphics_getSprite(i);
			dw_card_sealgfx = PA_CreateGfx(i,(void*)logo_seal_Sprite, OBJ_SIZE_64X64,1);
			PA_CreateSpriteFromGfx(i,dw_card_seal,dw_card_sealgfx,OBJ_SIZE_64X64,1,dw_card_sealpal,
										96,64);
	   	PA_SetSpritePrio(i,dw_card_seal,0);
		}
		ct_smiley_setState(SMILEYMOOD_HAPPYJUMP);
		// Show Results!!!! In hanzi, of course ^_^
		dw_card_pointpal = ct_graphics_getPalette(1);
		PA_LoadSpritePal(1,dw_card_pointpal,(void*)hanzi_Pal);
	   dw_card_point[0] = ct_graphics_getSprite(1);
		dw_card_pointgfx[0] = PA_CreateGfx(1,(void*)btn64yeah_Sprite, OBJ_SIZE_64X64,1);
		PA_CreateSpriteFromGfx(1,dw_card_point[0],dw_card_pointgfx[0],OBJ_SIZE_64X64,1,dw_card_pointpal,
										44,48);
		PA_SetSpritePrio(1,dw_card_point[0],3);
	   dw_card_point[1] = ct_graphics_getSprite(1);
		dw_card_pointgfx[1] = PA_CreateGfx(1,(void*)btn64nope_Sprite, OBJ_SIZE_64X64,1);
		PA_CreateSpriteFromGfx(1,dw_card_point[1],dw_card_pointgfx[1],OBJ_SIZE_64X64,1,dw_card_pointpal,
										44 + 64 + 48,48);
		PA_SetSpritePrio(1,dw_card_point[1],3);
				
		ct_HALhanzi_paintHanzi64x64(1,ct_HALhanzi_word_getHanzi(ct_utillib_getChineseNumber(ct_state_var_getInt(4)),1),
												52, 54); // GOOD!
		ct_HALhanzi_paintHanzi64x64(1,ct_HALhanzi_word_getHanzi(ct_utillib_getChineseNumber(10 - ct_state_var_getInt(4)),1),
												48 + 64 + 52, 54); // hmmmm bad :-P
		// Nothing more to do here...
		ex_card_state = EX_CARD_RESULTS;
		return;
   } 

	// Hide buttons Yes & No
   ct_button_hide(dw_card_button_yes);
   ct_button_hide(dw_card_button_no);	
   ct_button_Text(dw_card_button_clean,"[]",3);// ++CHANGE++ Change text for icon
	   
   // OK... choose a hanzi. But do not repeat the previous one!
   w[0] = ct_lesson_lenWords(1);
   w[1] = ct_lesson_lenWords(2);
   w[2] = ct_lesson_lenWords(3);
   w[3] = ct_lesson_lenWords(4);
   maxprob = w[0] + w[1] + w[2] + w[3]; // Get total number of hanzi

   repeatedhanzi = 1;
   while (repeatedhanzi) {
   	r = PA_RandMinMax(1,maxprob);

	   found = 0; // not found
   	sel_size = -1; // no size chosen
   	i = 0; // which size   
   	cont = 0; // upper limit
   	for(i = 0; !found; i++) {
			if (i > 3){
				found = 1; // No luck...      
			} 
			else { // Good, we can try to see if it is from this size  :-)
      		cont += w[i];
	      	if ((r <= cont) && (w[i] != 0)) {
   				sel_size = i+1;
   				sel_hanzi[0] = r - cont + w[i];
   				ex_card_size = sel_size;
	   			ex_card_actual = sel_hanzi[0];
   				found = 1;
  				} 		
	 		}
   	}   
   	repeatedhanzi = ex_card_noRepeat(r);
 	}  	
   ct_state_var_setInt(1,r);
	if (maxprob >= 10)
		ct_state_var_addArrInt(r); // Only if I have 10 or more, I can afford to not repeat values
	   
   // OK, now init the "input" system - hanzi or pinyin
	if (ct_state_var_getInt(0) == SUBGAME_EXCARDPINYIN) { // Draw the keyboard
      		PA_InitKeyboard(2); // Load the keyboard on background 2...
      		PA_ScrollKeyboardXY(24, 96); // Show the Keyboard...
   } else if ((ct_state_var_getInt(0) == SUBGAME_EXCARDHANZI) || (ct_state_var_getInt(0) == SUBGAME_EXCARDMEAN)) {
      		// First, just the GFX
				dw_card_pal = ct_graphics_getPalette(0);
				PA_LoadSpritePal(0,dw_card_pal,(void*)hanzi_Pal);
				dw_card_templ_gfx = PA_CreateGfx(0,(void*)hanzi_template_Sprite, OBJ_SIZE_64X64, 1);
				if (sel_size < 3)
					PA_SetRotset(0, 0, 0, 128, 128);
   			x = 96; y = 48; size = 128;
				for (i = 0; i < sel_size; i++) {
				   
				   switch (sel_size) {
				      case 1:
				         x = 64; y = 48; size = 128;
				         break;
				      case 2:
				         x = 0; y = 48; size = 128;
				         break;
				      case 3:
				         x = 32; y = 80; size = 64;
				         break;
				      case 4:
				         x = 0; y = 80; size = 64;
				         break;				         
				   }   
					dw_card_templwrite[i] = ct_graphics_getSprite(0);
					PA_CreateSpriteFromGfx(0,dw_card_templwrite[i],dw_card_templ_gfx,OBJ_SIZE_64X64,1,dw_card_pal,
													x + (i * size),y);
				   PA_SetSpritePrio(0,dw_card_templwrite[i],3);	
					if (sel_size < 3)	{
						PA_SetSpriteDblsize(0,dw_card_templwrite[i],1);
						PA_SetSpriteRotEnable(0,dw_card_templwrite[i],0);
					}							   
				}   
				ct_textzone_init(x,y,sel_size);
				ct_textzone_setPenSize((sel_size < 3)?3:1);      
	}	      			   
}   

void ex_card_PutOptions() {
   
   char hanz[6 * 4]; // 4, max letters, 5 per letter
   int i;
   
   strcpy(hanz,"");
   strcpy(hanz,ct_lesson_askWord(ex_card_size,ex_card_actual));
   
   // Depends on the type of quiz, write in the UPPER part
   switch (ct_state_var_getInt(0)) {
      case SUBGAME_EXCARDPINYIN :
			for (i = 1; i <= ex_card_size; i++ ) {
			   ct_HALhanzi_paintHanzi32x32(1,
	   									ct_HALhanzi_word_getHanzi(hanz,i),
	   									112 - ((ex_card_size-1) * 16) + ((i-1) * 32),
	   									16);
			}		 
         break;
      case SUBGAME_EXCARDHANZI :
         ct_HALhanzi_setPinyinSize(4);
         ct_HALhanzi_paintPinyin(1,hanz,56);
			break;         
      case SUBGAME_EXCARDMEAN :
		   strcpy(hanz,"");
		   strcpy(hanz,ct_lesson_askWord(ex_card_size,ex_card_actual));
		   PA_CenterSmartText(1,10 , 72, 
								250, 152, 
								ct_hwmeaning_getHanzi(hanz,LANG_ENG),
								1, 2, 0);
			break; 
   }   
   
   // Nothing to write on the lower part -:)
}   

void ex_card_PutResults() {
   int i;
   char hanz[6 * 4]; // 4, max letters, 5 per letter   

   strcpy(hanz,"");
   strcpy(hanz,ct_lesson_askWord(ex_card_size,ex_card_actual));
   
	// Write hanzi AND pinyin on the UPPER part
	for (i = 1; i <= ex_card_size; i++ ) {
	   ct_HALhanzi_paintHanzi32x32(1,
									ct_HALhanzi_word_getHanzi(hanz,i),
									112 - ((ex_card_size-1) * 16) + ((i-1) * 32),
									16);
	}		 
   ct_HALhanzi_setPinyinSize(4);
   ct_HALhanzi_paintPinyin(1,hanz,56);
	PA_CenterSmartText(1,10 , 72, 
								250, 152, 
								ct_hwmeaning_getHanzi(hanz,LANG_ENG),
								1, 2, 0);

   
}

int ex_card_Next() {
   if (ex_card_state == EX_CARD_RESULTS) {
		ct_state_var_setInt(2,ct_state_var_getInt(2) - 1); // Less Exams to do...      
		if (ct_state_var_getInt(2) >= 0){ 
		   // Next Exam
			ct_state_assignState(GAME_EX_CARD); 
		} else {
		   // Results - TODO
			ct_state_assignState(GAME_MAINMENU);
		}
		return 1;			
   }   
   return 0;
}   

void ex_card_Yes() {
   // Check for next screen
   if (ex_card_Next())
     return;
   
  	// Cool! - Let's make Smiley Jump!!! ^.^
	ct_smiley_setState(SMILEYMOOD_HAPPYJUMP);      
	// One point more for the score!!!!
	ct_state_var_setInt(4,ct_state_var_getInt(4) + 1);
	
	// Next substate
	ex_card_state = EX_CARD_RESULTS;	  
}

void ex_card_No() {
   // Check for next screen
   if (ex_card_Next())
     return;
   
   // Aiyaaaa... V_V
	ct_smiley_setState(SMILEYMOOD_AIYAAAA);            
	
	// Next substate
	ex_card_state = EX_CARD_RESULTS;	     
}   

void ex_card_Clean() {
   int i;
   
   if ((ct_state_var_getInt(0) == SUBGAME_EXCARDHANZI) || (ct_state_var_getInt(0) == SUBGAME_EXCARDMEAN))
   	ct_textzone_clear();
   else if (ct_state_var_getInt(0) == SUBGAME_EXCARDPINYIN) {
	   // Inits the internal text system
   	for (i = 0; i < EX_CARD_LEN_CHAR; i++)
   		dw_card_text[i] = '\0';
	   dw_card_textsize = 0;      
	   ct_button_Text(dw_card_button_pinyin,dw_card_text,3);
   }   
}   

void ex_card_Check() {
   // New Substate
   ex_card_PutResults();
   ex_card_state = EX_CARD_YESNO;	     
   
 	// Update sound system, talk a bit more.
   if (PA_SoundChannelIsBusy(0))
     	PA_StopSound(0);
	ex_card_actsound = 1;
	ex_card_playsound = 1;
	ex_card_delaysound = 20;

   
   // Set up things...
   ct_button_hide(dw_card_button_check);			                  
   ct_button_hide(dw_card_button_clean);
   ct_button_show(dw_card_button_yes);
   ct_button_show(dw_card_button_no);
}

void ex_card_ManageSound() {
	// Sound!
	if (ex_card_delaysound > 0) {
		ex_card_delaysound--;
	} else {
		if (ex_card_playsound) {
		   // OK, let's play this sound and only this sound
			ex_card_playsound = 0; 
			// Play
			if (ct_hwsound_preloadPinyin(ct_HALhanzi_getPinyin(ct_HALhanzi_phrase_getHanzi(ct_lesson_askWord(ex_card_size,ex_card_actual),ex_card_actsound)))) {
			   void *sound;
		   	sound = ct_hwsound_getPinyin();
			   if (sound != NULL) {
			   	PA_PlaySoundEx(0,sound,ct_hwsound_getPinyinSize(),96,11025,0);		
				}   
			}		   
		} else {
		   // Only if there are more sounds left...
	   	if (ex_card_actsound < ct_HALhanzi_phrase_lenHanzi(ct_lesson_askWord(ex_card_size,ex_card_actual))) {
		   	if (!PA_SoundChannelIsBusy(0)) {
		   		ex_card_actsound++; // Next!!!!!
		   		ex_card_playsound = 1;
	 			}  	
	 		}  	
		}   
	}		
}		   	   

/* Functions */ 
///////////////

void ct_ex_card_Start()
{
	// Internal state
	ex_card_state = EX_CARD_;
	ex_card_finished = 0;
	ex_card_actsound = 65535; // No sounds! ;-)
	ex_card_playsound = 0;
	ex_card_delaysound = 0;


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
	
	if (ct_state_var_getInt(2) != 0) {
		dw_card_button_check = ct_button_add(BTN_GREY,64,BTN_32,96,8);
		ct_button_addHandler_Click(dw_card_button_check,ex_card_Check);
		ct_button_Text(dw_card_button_check,"CHECK",2);
		
		dw_card_button_yes = ct_button_add(BTN_GREEN,32,BTN_32,0,8);
		ct_button_addHandler_Click(dw_card_button_yes,ex_card_Yes);
		ct_button_Text(dw_card_button_yes,"OK",2);
	
		dw_card_button_no = ct_button_add(BTN_RED,32,BTN_32,224,8);
		ct_button_addHandler_Click(dw_card_button_no,ex_card_No);
		ct_button_Text(dw_card_button_no,"NO",2);
		
		dw_card_button_clean = ct_button_add(BTN_GREY,32,BTN_32,224,8);
		ct_button_addHandler_Click(dw_card_button_clean,ex_card_Clean);
		ct_button_Text(dw_card_button_clean,"[]",3);
		
		dw_card_button_pinyin = ct_button_add(BTN_NONE,192,BTN_32,32,56);
	}			
	
	/* Init next state - Unique state */
	ex_card_Start();
}

void ct_ex_card_Finish()
{
	// Only if I get out from here...
	if ((ct_state_var_getInt(0) == SUBGAME_EXCARDPINYIN) && (ex_card_finished)) {
	   PA_KeyboardOut();
	}	

	// and maybe others...
	if (ex_card_finished) {
		PA_DeleteGfx(0,dw_card_sealgfx);
		
		// Update Score - TODO
		if (ct_state_var_getInt(4) > 
				data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][ct_state_var_getInt(0)])
			data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][ct_state_var_getInt(0)] = ct_state_var_getInt(4);
		
		// Save game
		ct_utillib_saveSRAM(data_profile);

	}	

   PA_WaitForVBL();
   
	// buttons...
	ct_button_finish();
	ct_smiley_finish();
	
} 


void ct_ex_card_Input()
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
		ex_card_Next();
	}
	
	// Check stylus painting
	if ((ex_card_state == EX_CARD_TEST) && 
		 ((ct_state_var_getInt(0) == SUBGAME_EXCARDHANZI) || (ct_state_var_getInt(0) == SUBGAME_EXCARDMEAN))) {
		if (Stylus.Held) {
	   	if (ct_textzone_inArea(Stylus.X,Stylus.Y))
	   		ct_textzone_paint(Stylus.X,Stylus.Y);
	 	} else {
	 	   ct_textzone_reset();
	 	}   
	}   
	
	// Check Keyboard pressed
	int letter;
	int nletter = 0;
	if ((ex_card_state == EX_CARD_TEST) && 
		 (ct_state_var_getInt(0) == SUBGAME_EXCARDPINYIN)) {
		    
		letter = PA_CheckKeyboard();
		
		if (letter > 31) { // New letter?
			if (dw_card_textsize < (EX_CARD_LEN_CHAR - 2)) {
				dw_card_text[dw_card_textsize] = letter;
				dw_card_text[dw_card_textsize + 1] = '\0';
				dw_card_textsize++;
				nletter = 1;
			}			
		} else if (letter == PA_BACKSPACE) { // Backspace pressed
			if (dw_card_textsize > 0) {
				dw_card_textsize--;			   
				dw_card_text[dw_card_textsize] = ' '; // Erase the last letter			   
				dw_card_text[dw_card_textsize + 1] = '\0';
			} else
				dw_card_text[dw_card_textsize] = ' '; // Erase the last letter
			nletter = 1;
		}
		if (nletter)
			ct_button_Text(dw_card_button_pinyin,dw_card_text,3);
	}	
}

void ct_ex_card_State()
{  
	// Elements state
	ct_button_Manage();
	ct_smiley_Manage();
		
	switch (ex_card_state) {
	   case EX_CARD_:
	      // Now, fill the Contents...
	      ex_card_PutOptions();
	      ex_card_state = EX_CARD_TEST;
	      if ((ct_state_var_getInt(0) == SUBGAME_EXCARDHANZI) || (ct_state_var_getInt(0) == SUBGAME_EXCARDMEAN)) {
	         ex_card_actsound = 1;
	         ex_card_playsound = 1;
	         ex_card_delaysound = 20;
	      } 
	      break;
	   case EX_CARD_TEST:
	      // Wait for input...spell the pinyin...
	      ex_card_ManageSound();
	      break;
	   case EX_CARD_YESNO:
	      // Wait the user to push yes or no...
	      ex_card_ManageSound();
	      break;	      
	   case EX_CARD_RESULTS:
	      // Nothing to do here...except to shut up!!!
		   if (PA_SoundChannelIsBusy(0))
     			PA_StopSound(0);
	      break;	      
	}   
}
