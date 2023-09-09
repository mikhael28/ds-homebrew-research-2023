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


/* Tool - Dictionary */
/*
	State_str = hanzi; (strlen / 4 = number of hanzi in the word)
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
#include "ct_hwpinyin.h"
#include "ct_hwhanzi.h"
#include "ct_linkedlist.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define G_DICT_ 0

#define G_DICT_LEN_CHAR 20

/* Variables - Internal State */
////////////////////////////////
int g_dict_state;
int g_dict_index;
char g_dict_word[20];

int g_dict_playsound;
int g_dict_delaysound;
char g_dict_soundtext[G_DICT_LEN_CHAR];

LList* g_dict_pinyinlist;

int dw_dict_button_imaback;
int dw_dict_button_imanext;
int dw_dict_button_image[6];
int dw_dict_button_back;
int dw_dict_button_next;
int dw_dict_button_search;
int dw_dict_button_pinyin;
int dw_dict_button_clean;
int dw_dict_button_anicheck;

char dw_dict_text[G_DICT_LEN_CHAR];
int dw_dict_textsize;
int dw_dict_searched;


/* Internal Functions */ 
////////////////////////


void g_dict_Back() {
	ct_state_assignState(GAME_CONFIG);
}

void g_dict_launchAni() {
   int len = strlen(g_dict_word) / 4;
   if (len > 0) {
		ct_state_assignState(GAME_LHANZI);      
	   ct_state_var_setStr(g_dict_word);
   	ct_state_var_setInt(0,0); // Individual hanzi
		ct_state_var_setInt(1,len); // Size of the word
		ct_state_var_setInt(2,1); // First one...
		ct_state_var_setInt(3,GAME_DICT); // Return to the dictionary...
	}	
}

void g_dict_Start() {
   int i;
   int max;
   
   // Clean the buttons, Rewrite
   ct_button_cleanContent(1);
   ct_button_Text(dw_dict_button_search,"+",4);
   ct_button_Text(dw_dict_button_clean,"-",4);
   ct_button_Text(dw_dict_button_pinyin,dw_dict_text,3);
   
   // Now, write the hanzi
   max = ct_linkedlist_len(g_dict_pinyinlist);
   
   for (i = g_dict_index; i < g_dict_index + 6; i++) {
		if (i < max) {
	   	ct_HALhanzi_paintHanzi32x32(0,
   										*((int *)ct_linkedlist_getPos(g_dict_pinyinlist,i)),
   										ct_button_getX(dw_dict_button_image[i - g_dict_index]),
   										ct_button_getY(dw_dict_button_image[i - g_dict_index])
											);
		}				
	}		
}   

void g_dict_Search() {
   int val;
   int *hanzi;
   char search_text[G_DICT_LEN_CHAR];
   
   // OK... First, clean variables, and mark others
   ct_linkedlist_finish(g_dict_pinyinlist);
	g_dict_pinyinlist = ct_linkedlist_init();
	g_dict_index = 0;
	dw_dict_searched = 1;
	
	// Now, search for the idx of the hanzi, based on the pinyin
   PA_CenterSmartText(1,0,0,256,32,"...LOADING...",1,3,0);	

	sprintf(search_text,"%s",dw_dict_text);
	ct_utillib_str_tolower(search_text);
	val = 0;
	val = ct_hwpinyin_searchPinyin(val,search_text);
	while (val != -1) {
	   // Check
	   if (!ct_hwhanzi_empty(val + 19968)) {
	   	// Store
	   	hanzi = malloc(sizeof(int));
	   	*hanzi = val + 19968;
	   	ct_linkedlist_addEnd(g_dict_pinyinlist,hanzi);
	 	}  	
	   // Next
	   val++;
	   val = ct_hwpinyin_searchPinyin(val,search_text);
	}   

   PA_CenterSmartText(1,0,0,256,32,"...LOADING...",0,3,0);	
		
	// Time to reset the images...
	g_dict_Start();
	
	// And time to play the actual pinyin...
   if (PA_SoundChannelIsBusy(0))
     	PA_StopSound(0);
   g_dict_delaysound = 15;
   g_dict_playsound = 1;
   strcpy(g_dict_soundtext,dw_dict_text);
}      

void g_dict_pinyinNext() {
   if ((g_dict_index + 6) < ct_linkedlist_len(g_dict_pinyinlist))
   	g_dict_index += 6;
   g_dict_Start();
}

void g_dict_pinyinBack() {
   if ((g_dict_index - 6) < 0)
   	g_dict_index = 0;
   else 
		g_dict_index -= 6;
   g_dict_Start();
}

void g_dict_PaintUpper() {
   int i;
   char hanz[6 * 4]; // 4, max letters, 5 per letter   
   int max;
   
	// First, clean upper screen
	PA_Clear8bitBg(1);

	// Now, write hanzi AND pinyin
	max = ct_HALhanzi_word_lenHanzi(g_dict_word);
   for (i = 0; i < ct_HALhanzi_word_lenHanzi(g_dict_word); i++) {
      // Hanzi
		ct_HALhanzi_paintHanzi32x32(1,ct_HALhanzi_word_getHanzi(g_dict_word,i + 1), 
									128 - (16 * max) + (i * 32), 
									32
								);
		// Pinyin
      ct_HALhanzi_setPinyinSize(4);
      ct_HALhanzi_paintPinyin(1,g_dict_word,80);
	}			
	// Also, write meaning in the upper part ;-)
	if (ct_HALhanzi_word_lenHanzi(g_dict_word) > 0)
	   PA_CenterSmartText(1,10 , 96, 
									250, 144, 
									ct_hwmeaning_getHanzi(g_dict_word,LANG_ENG),
									1, 2, 0);
}	      

void g_dict_newWord() {
   int val;
   int pos;
   // If there is something inside
   pos = g_dict_index + ct_button_getVariable(ct_button_getHandler());
   if ( pos < ct_linkedlist_len(g_dict_pinyinlist)) 
	{
	   // No more than 4 words...
	   if (ct_HALhanzi_word_lenHanzi(g_dict_word) < 4) {
			// Add this word to the system
			val = *((int *)ct_linkedlist_getPos(g_dict_pinyinlist,pos));
			strncat(g_dict_word,ct_utillib_int4hex(val),4);
		
			// Paint the upper part
			g_dict_PaintUpper();
		}			
	}   
}

void g_dict_cleanWord() {
   int i;
   char hanzi[6 * 4];
   int len;
   sprintf(hanzi,"%s","");
   len = ct_HALhanzi_word_lenHanzi(g_dict_word);
   if (len > 1) {
      strncpy(hanzi,g_dict_word,(len - 1) * 4);
      hanzi[(len - 1) * 4] = '\0';
   } else {
   	sprintf(hanzi,"%s","");
	}
	strcpy(g_dict_word,hanzi);
	g_dict_word[(len - 1) * 4] = '\0';
	g_dict_PaintUpper();
}	   

void g_dict_manageSound() {
	// Sound!
	if (g_dict_delaysound > 0) {
		g_dict_delaysound--;
	} else {
		if (g_dict_playsound) {
		   // OK, let's play this sound and only this sound
			g_dict_playsound = 0; 
			// Play
			if (ct_hwsound_preloadPinyin(g_dict_soundtext)) {
			   void *sound;
		   	sound = ct_hwsound_getPinyin();
			   if (sound != NULL) {
			   	PA_PlaySoundEx(0,sound,ct_hwsound_getPinyinSize(),96,11025,0);		
				}   
			}		   
			// End
			strcpy(g_dict_soundtext,"");
		}			
	}		
}	   	   

/* Functions */ 
///////////////

void ct_g_dict_Start()
{
   int i;
   int key_hand;
   int btn_hand;
   
	// Internal state
	g_dict_pinyinlist = ct_linkedlist_init();
	g_dict_index = 0;
	dw_dict_textsize = 0;
	dw_dict_text[0] = '\0';
	dw_dict_searched = 0;
	
	sprintf(g_dict_word,"%s",ct_state_var_getStr());
	
	if (data_gameData[data_profile].hand == CONF_LEFTHANDED) {
	   key_hand = 0;
	   btn_hand = 208 + 8;
	} else {
	   key_hand = 256 - 208;
	   btn_hand = 8;
	}		   
	
	g_dict_delaysound = 0;
	g_dict_playsound = 0;
	strcpy(g_dict_soundtext,"");

	// Inits the Graphic mode for the listening
	ct_graphics_initGraphicMode(SCREENMODE_INTRO);
	
	/* Graphics */
	// Just buttons, and smiley
	ct_button_init();	
	ct_utillib_init();
	
	ct_smiley_init();
	
	// Elements of the screen
					
	/* Variables*/
	for (i = 0; i < 6; i++) {
	   dw_dict_button_image[i] = ct_button_add(BTN_NONE,32,BTN_32,32 + (i*32),16);
	   ct_button_setVariable(dw_dict_button_image[i],i);
	   ct_button_addHandler_Click(dw_dict_button_image[i],g_dict_newWord);
	}   
	dw_dict_button_imaback = ct_button_add(BTN_GREY,32,BTN_32,0,16);
	ct_button_addContentIma(dw_dict_button_imaback,(void *)arrow_l_Sprite,0,0,32,32);	
	ct_button_addHandler_Click(dw_dict_button_imaback,g_dict_pinyinBack);
	
	dw_dict_button_imanext = ct_button_add(BTN_GREY,32,BTN_32,256 - 32,16);
   ct_button_addContentIma(dw_dict_button_imanext,(void *)arrow_r_Sprite,0,0,32,32);			
	ct_button_addHandler_Click(dw_dict_button_imanext,g_dict_pinyinNext);
   
	dw_dict_button_back = ct_button_add(BTN_GREY,32,BTN_32,btn_hand,96 - 32);
	ct_button_addContentIma(dw_dict_button_back,(void *)arrow_b_Sprite,0,0,32,32);	
	ct_button_addHandler_Click(dw_dict_button_back,g_dict_Back);

	dw_dict_button_search = ct_button_add(BTN_GREY,32,BTN_32,btn_hand,96);
	//ct_button_addContentIma(dw_dict_button_search,(void *)arrow_b_Sprite,0,0,32,32);	
	ct_button_Text(dw_dict_button_search,"+",4);
	ct_button_addHandler_Click(dw_dict_button_search,g_dict_Search);

	dw_dict_button_clean = ct_button_add(BTN_GREY,32,BTN_32,btn_hand,96 + 32);
	//ct_button_addContentIma(dw_dict_button_search,(void *)arrow_b_Sprite,0,0,32,32);	
	ct_button_Text(dw_dict_button_clean,"-",4);
	ct_button_addHandler_Click(dw_dict_button_clean,g_dict_cleanWord);
	
	dw_dict_button_anicheck = ct_button_add(BTN_GREY,32,BTN_32,btn_hand,96 + 32 + 32);
	ct_button_addContentIma(dw_dict_button_anicheck,(void *)arrow_hanzi_Sprite,0,0,32,32);	
	ct_button_addHandler_Click(dw_dict_button_anicheck,g_dict_launchAni);
	
	dw_dict_button_pinyin = ct_button_add(BTN_NONE,192,BTN_32,key_hand,56);
   
   /* Keyboard */
   PA_InitKeyboard(2); // Load the keyboard on background 2...
   PA_ScrollKeyboardXY(key_hand, 96); // Show the Keyboard...
   
	
	/* Init next state - Unique state */
	g_dict_Start();
	g_dict_PaintUpper();
}

void ct_g_dict_Finish()
{
	// Only if I get out from here...
	PA_KeyboardOut();

   PA_WaitForVBL();
   
	// buttons...
	ct_button_finish();
	ct_smiley_finish();
	
	// State....
	ct_linkedlist_finish(g_dict_pinyinlist);
} 


void ct_g_dict_Input()
{
   int i;
   
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
	}
	
	// Check Keyboard pressed
	int letter;
	int nletter = 0;
	if (1) {
		    
		letter = PA_CheckKeyboard();
		
		
		if ((letter != 0) && (dw_dict_searched)) {
		   // Clean text
		   for (i = 0; i < dw_dict_textsize; i++)
		   	dw_dict_text[i] = '\0';
			dw_dict_textsize = 0;
			dw_dict_searched = 0;
		}   
		if (letter > 31) { // New letter?
			if (dw_dict_textsize < (G_DICT_LEN_CHAR - 2)) {
				dw_dict_text[dw_dict_textsize] = letter;
				dw_dict_text[dw_dict_textsize + 1] = '\0';
				dw_dict_textsize++;
				nletter = 1;
			}			
		} else if (letter == PA_BACKSPACE) { // Backspace pressed
			if (dw_dict_textsize > 0) {
				dw_dict_textsize--;			   
				dw_dict_text[dw_dict_textsize] = ' '; // Erase the last letter			   
				dw_dict_text[dw_dict_textsize + 1] = '\0';
			} /*else
				dw_dict_text[dw_dict_textsize] = ' '; // Erase the last letter*/
			nletter = 1;
		}
		if (nletter)
			ct_button_Text(dw_dict_button_pinyin,dw_dict_text,3);
	}	
}

void ct_g_dict_State()
{  
	// Elements state
	ct_button_Manage();
	ct_smiley_Manage();		
	
	// Other's state - Music!
	g_dict_manageSound();
}
