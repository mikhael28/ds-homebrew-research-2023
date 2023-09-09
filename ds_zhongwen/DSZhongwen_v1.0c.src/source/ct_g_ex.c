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
#include "ct_lesson.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

#define EX_MAX_OPTIONS 4
#define EX_IS_QUIZ 0
#define EX_IS_CARDS 1
#define EX_IS_LISTEN 2

// No internal state

/* Variables - Internal State */
////////////////////////////////

int dw_ex_lesson_type;
int dw_ex_lesson_number;

int dw_ex_button_back;
int dw_ex_button_next;
int dw_ex_button_quiz;
int dw_ex_button_cards;
int dw_ex_button_listen;
int dw_ex_button_exams[EX_MAX_OPTIONS];

int dw_ex_button_lessondbg[2];

/* Internal Functions */ 
////////////////////////

void g_ex_updateState() {
   int i;
   
	// First...Hides the buttons!
	for (i = 0; i < EX_MAX_OPTIONS; i++) {
		ct_button_hide(dw_ex_button_exams[i]);
		ct_button_Text(dw_ex_button_exams[i],"",2);
		ct_button_setVariable(dw_ex_button_exams[i],GAME_);		
	}
	
	// Second, show (and fill, and highlight) buttons depending on actual exam
	switch (dw_ex_lesson_type) {
	   case EX_IS_QUIZ:
	      // Hanzi?
	      ct_button_show(dw_ex_button_exams[0]);
	      ct_button_setVariable(dw_ex_button_exams[0],SUBGAME_EXQUIZHANZI);
	      ct_button_Text(dw_ex_button_exams[0],"Hanzi?",
								(dw_ex_lesson_number == ct_button_getVariable(dw_ex_button_exams[0]))?3:2);
	      
	      // Pinyin?
	      ct_button_show(dw_ex_button_exams[1]);
	      ct_button_setVariable(dw_ex_button_exams[1],SUBGAME_EXQUIZPINYIN);	      
	      ct_button_Text(dw_ex_button_exams[1],"Pinyin?",
								(dw_ex_lesson_number == ct_button_getVariable(dw_ex_button_exams[1]))?3:2);
								
	      // Meaning?
	      ct_button_show(dw_ex_button_exams[2]);
	      ct_button_setVariable(dw_ex_button_exams[2],SUBGAME_EXQUIZMEAN);
	      ct_button_Text(dw_ex_button_exams[2],ct_const_getText(TXT_EXAM_MEANING,data_gameData[data_profile].language),
								(dw_ex_lesson_number == ct_button_getVariable(dw_ex_button_exams[2]))?3:2);
	      break;
	   case EX_IS_CARDS:
	      // Hanzi?
	      ct_button_show(dw_ex_button_exams[0]);
	      ct_button_setVariable(dw_ex_button_exams[0],SUBGAME_EXCARDHANZI);
	      ct_button_Text(dw_ex_button_exams[0],"Hanzi?",
								(dw_ex_lesson_number == ct_button_getVariable(dw_ex_button_exams[0]))?3:2);
	      
	      // Pinyin?
	      ct_button_show(dw_ex_button_exams[1]);
	      ct_button_setVariable(dw_ex_button_exams[1],SUBGAME_EXCARDPINYIN);	      
	      ct_button_Text(dw_ex_button_exams[1],"Pinyin?",
								(dw_ex_lesson_number == ct_button_getVariable(dw_ex_button_exams[1]))?3:2);
								
	      // Meaning?
	      ct_button_show(dw_ex_button_exams[2]);
	      ct_button_setVariable(dw_ex_button_exams[2],SUBGAME_EXCARDMEAN);
	      ct_button_Text(dw_ex_button_exams[2],ct_const_getText(TXT_EXAM_MEANING,data_gameData[data_profile].language),
								(dw_ex_lesson_number == ct_button_getVariable(dw_ex_button_exams[2]))?3:2);
	      break;
	   case EX_IS_LISTEN:
	      // Listen - Quiz?
	      ct_button_show(dw_ex_button_exams[0]);
	      ct_button_setVariable(dw_ex_button_exams[0],SUBGAME_EXLISTENQUIZ);
	      ct_button_Text(dw_ex_button_exams[0],"Quiz?",
								(dw_ex_lesson_number == ct_button_getVariable(dw_ex_button_exams[0]))?3:2);
	}   	
	
	// Update button colours
	ct_button_changetype(dw_ex_button_quiz,(dw_ex_lesson_type == EX_IS_QUIZ)?BTN_GREEN:BTN_GREY);
	ct_button_changetype(dw_ex_button_cards,(dw_ex_lesson_type == EX_IS_CARDS)?BTN_GREEN:BTN_GREY);
	ct_button_changetype(dw_ex_button_listen,(dw_ex_lesson_type == EX_IS_LISTEN)?BTN_GREEN:BTN_GREY);		
}   

void g_ex_setParameters(int subgame) {
   switch (subgame) {
      case SUBGAME_EXQUIZHANZI:
         ct_state_assignState(GAME_EX_QUIZ);
         ct_state_var_resetArrInt();
         ct_state_var_setInt(0,SUBGAME_EXQUIZHANZI);
         ct_state_var_setInt(1,-1);
         ct_state_var_setInt(2,10);
         ct_state_var_setInt(3,10);
         ct_state_var_setInt(4,0);
         break;
      case SUBGAME_EXQUIZPINYIN:
         ct_state_assignState(GAME_EX_QUIZ);         
         ct_state_var_resetArrInt();
         ct_state_var_setInt(0,SUBGAME_EXQUIZPINYIN);
         ct_state_var_setInt(1,-1);
         ct_state_var_setInt(2,10);
         ct_state_var_setInt(3,10);
         ct_state_var_setInt(4,0);         
         break;
      case SUBGAME_EXQUIZMEAN:
         ct_state_assignState(GAME_EX_QUIZ);         
         ct_state_var_resetArrInt();
         ct_state_var_setInt(0,SUBGAME_EXQUIZMEAN);
         ct_state_var_setInt(1,-1);
         ct_state_var_setInt(2,10);
         ct_state_var_setInt(3,10);
         ct_state_var_setInt(4,0);         
         break;         
      case SUBGAME_EXCARDHANZI:
         ct_state_assignState(GAME_EX_CARD);
         ct_state_var_resetArrInt();
         ct_state_var_setInt(0,SUBGAME_EXCARDHANZI);
         ct_state_var_setInt(1,-1);
         ct_state_var_setInt(2,10);
         ct_state_var_setInt(3,10);
         ct_state_var_setInt(4,0);
         break;
      case SUBGAME_EXCARDPINYIN:
         ct_state_assignState(GAME_EX_CARD);         
         ct_state_var_resetArrInt();
         ct_state_var_setInt(0,SUBGAME_EXCARDPINYIN);
         ct_state_var_setInt(1,-1);
         ct_state_var_setInt(2,10);
         ct_state_var_setInt(3,10);
         ct_state_var_setInt(4,0);        
         break;         
      case SUBGAME_EXCARDMEAN:
         ct_state_assignState(GAME_EX_CARD);         
         ct_state_var_resetArrInt();
         ct_state_var_setInt(0,SUBGAME_EXCARDMEAN);
         ct_state_var_setInt(1,-1);
         ct_state_var_setInt(2,10);
         ct_state_var_setInt(3,10);
         ct_state_var_setInt(4,0);        
         break;                  
      case SUBGAME_EXLISTENQUIZ:
         if (data_lessontype == LESSON_LISTEN) {
         	ct_state_assignState(GAME_EX_LISTEN);
         	ct_state_var_resetArrInt();
         	ct_state_var_setInt(0,SUBGAME_EXLISTENQUIZ);
         	ct_state_var_setInt(1,-1);
         	ct_state_var_setInt(2,10);
         	ct_state_var_setInt(3,10);
         	ct_state_var_setInt(4,0);
         	break;
       	} else {
	         ct_state_assignState(GAME_EX_QUIZ);
   	      ct_state_var_resetArrInt();
      	   ct_state_var_setInt(0,SUBGAME_EXLISTENQUIZ);
         	ct_state_var_setInt(1,-1);
         	ct_state_var_setInt(2,10);
         	ct_state_var_setInt(3,10);
         	ct_state_var_setInt(4,0);
         	break;
			}			
   }   
}   

void g_ex_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
}

void g_ex_startExam(){
   if (dw_ex_lesson_number == SUBGAME_) {
      // No Exam selected!
   } else {
      // Start new exam!!!
	   g_ex_setParameters(dw_ex_lesson_number);
   } 
}

void g_ex_ChangeLesson() {
	dw_ex_lesson_number = ct_button_getVariable(ct_button_getHandler()); 
	// TODO - Smiley Talking  
	g_ex_updateState();
}   

void g_ex_ChangeExam() {
   dw_ex_lesson_type = ct_button_getVariable(ct_button_getHandler());
   dw_ex_lesson_number = SUBGAME_;
   // TODO - Smiley Talking
   g_ex_updateState();
}   

/* Functions */ 
///////////////

void ct_g_ex_Start()
{
	int i;
	
	// Inits the Graphic mode for the exam selection
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
	ct_header_TextID(0,TXT_EXAM_,data_gameData[data_profile].language);
	
	// Load basic buttons
	dw_ex_button_quiz = ct_button_add(BTN_GREY,96,BTN_32,-12,48);
	ct_button_TextID(dw_ex_button_quiz,TXT_EXAM_QUIZ,data_gameData[data_profile].language,2);
	ct_button_setVariable(dw_ex_button_quiz,EX_IS_QUIZ);
	ct_button_addHandler_Click(dw_ex_button_quiz,g_ex_ChangeExam);
	
	dw_ex_button_cards = ct_button_add(BTN_GREY,96,BTN_32,-12,80);
	ct_button_TextID(dw_ex_button_cards,TXT_EXAM_CARDS,data_gameData[data_profile].language,2);
	ct_button_setVariable(dw_ex_button_cards,EX_IS_CARDS);	
	ct_button_addHandler_Click(dw_ex_button_cards,g_ex_ChangeExam);
	
	dw_ex_button_listen = ct_button_add(BTN_GREY,96,BTN_32,-12,112);
	ct_button_TextID(dw_ex_button_listen,TXT_EXAM_LISTEN,data_gameData[data_profile].language,2);
	ct_button_setVariable(dw_ex_button_listen,EX_IS_LISTEN);
	ct_button_addHandler_Click(dw_ex_button_listen,g_ex_ChangeExam);
	
	// Load "Lesson" Buttons
	for (i = 0; i < EX_MAX_OPTIONS; i++) {
	   dw_ex_button_exams[i] = ct_button_add(BTN_GREY,144,BTN_32,
													96,
													48 + (i * 32));
		ct_button_addHandler_Click(dw_ex_button_exams[i],g_ex_ChangeLesson);
	}
	
	// Load coloured "lesson" buttons (for preloading sprites purposes - TODO)
	dw_ex_button_lessondbg[0] = ct_button_add(BTN_GREEN,32,BTN_32,250,190);
	ct_button_hide(dw_ex_button_lessondbg[0]);
	dw_ex_button_lessondbg[1] = ct_button_add(BTN_RED,32,BTN_32,250,190);
	ct_button_hide(dw_ex_button_lessondbg[1]);
	   				
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_ex_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_ex_button_back,g_ex_launchBack);
	ct_button_addContentIma(dw_ex_button_back,(void *)arrow_b_Sprite,0,0,32,32);
	
	dw_ex_button_next = ct_button_add(BTN_CONFIG,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_ex_button_next,g_ex_startExam);
	ct_button_addContentIma(dw_ex_button_next,(void *)arrow_r_Sprite,0,0,32,32);
	
	/* Init next state - Unique state */
	dw_ex_lesson_type = EX_IS_QUIZ;
	dw_ex_lesson_number = SUBGAME_;
	
	g_ex_updateState();
}

void ct_g_ex_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
} 


void ct_g_ex_Input()
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

void ct_g_ex_State()
{
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
}
