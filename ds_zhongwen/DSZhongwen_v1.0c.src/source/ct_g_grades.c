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

#define GRADES_MAX_OPTIONS 4
#define GRADES_IS_QUIZ 0
#define GRADES_IS_CARDS 1
#define GRADES_IS_LISTEN 2

// No internal state

/* Variables - Internal State */
////////////////////////////////

int dw_grades_lesson_type;
int dw_grades_lesson_number;

int dw_grades_button_back;
int dw_grades_button_quiz[2];
int dw_grades_button_cards[2];
int dw_grades_button_listen[2];
int dw_grades_button_exams[GRADES_MAX_OPTIONS];
int dw_grades_button_examsscore[GRADES_MAX_OPTIONS];

int dw_grades_button_lessondbg[3];

/* Internal Functions */ 
////////////////////////

void g_grades_changeColor(int idbtn, int val){
   if (val == 0) {
      ct_button_changetype(idbtn,BTN_GREY);
   } else if (val <= 3) {
      ct_button_changetype(idbtn,BTN_RED);
   } else if (val <= 6) {
      ct_button_changetype(idbtn,BTN_YELLOW);
   } else if (val <= 10) {
      ct_button_changetype(idbtn,BTN_GREEN);
   }       
}   

void g_grades_updateState() {
   int i;
   int val;
   char num[20];
   
	// First...Hides the buttons!
	for (i = 0; i < GRADES_MAX_OPTIONS; i++) {
	   // Exams
		ct_button_hide(dw_grades_button_exams[i]);
		ct_button_Text(dw_grades_button_exams[i],"",2);
		ct_button_setVariable(dw_grades_button_exams[i],GAME_);		
		// Score
		ct_button_hide(dw_grades_button_examsscore[i]);
		ct_button_Text(dw_grades_button_examsscore[i],"",2);
		ct_button_setVariable(dw_grades_button_examsscore[i],GAME_);				
	}
	
	// Second, show (and fill, and highlight) buttons depending on actual exam
	switch (dw_grades_lesson_type) {
	   case GRADES_IS_QUIZ:
	      // Hanzi?
	      ct_button_show(dw_grades_button_exams[0]);
	      ct_button_setVariable(dw_grades_button_exams[0],SUBGAME_EXQUIZHANZI);
	      ct_button_Text(dw_grades_button_exams[0],"Hanzi?",
								(dw_grades_lesson_number == ct_button_getVariable(dw_grades_button_exams[0]))?3:2);
	      // Hanzi? - Score
	      ct_button_show(dw_grades_button_examsscore[0]);
			val = data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXQUIZHANZI];
			sprintf(num,"%d",val);
			ct_button_Text(dw_grades_button_examsscore[0],num,2);
			g_grades_changeColor(dw_grades_button_examsscore[0],val);
				      
	      // Pinyin?
	      ct_button_show(dw_grades_button_exams[1]);
	      ct_button_setVariable(dw_grades_button_exams[1],SUBGAME_EXQUIZPINYIN);	      
	      ct_button_Text(dw_grades_button_exams[1],"Pinyin?",
								(dw_grades_lesson_number == ct_button_getVariable(dw_grades_button_exams[1]))?3:2);
			// Pinyin? - Score
	      ct_button_show(dw_grades_button_examsscore[1]);
			val = data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXQUIZPINYIN];
			sprintf(num,"%d",val);
			ct_button_Text(dw_grades_button_examsscore[1],num,2);
			g_grades_changeColor(dw_grades_button_examsscore[1],val);

	      // Meaning?
	      ct_button_show(dw_grades_button_exams[2]);
	      ct_button_setVariable(dw_grades_button_exams[2],SUBGAME_EXQUIZMEAN);	      
	      ct_button_Text(dw_grades_button_exams[2],ct_const_getText(TXT_EXAM_MEANING,data_gameData[data_profile].language),
								(dw_grades_lesson_number == ct_button_getVariable(dw_grades_button_exams[2]))?3:2);
			// Meaning? - Score
	      ct_button_show(dw_grades_button_examsscore[2]);
			val = data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXQUIZMEAN];
			sprintf(num,"%d",val);
			ct_button_Text(dw_grades_button_examsscore[2],num,2);
			g_grades_changeColor(dw_grades_button_examsscore[2],val);
						
	      break;
	   case GRADES_IS_CARDS:
	      // Hanzi?
	      ct_button_show(dw_grades_button_exams[0]);
	      ct_button_setVariable(dw_grades_button_exams[0],SUBGAME_EXCARDHANZI);
	      ct_button_Text(dw_grades_button_exams[0],"Hanzi?",
								(dw_grades_lesson_number == ct_button_getVariable(dw_grades_button_exams[0]))?3:2);
	      // Hanzi? - Score
	      ct_button_show(dw_grades_button_examsscore[0]);
			val = data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXCARDHANZI];
			sprintf(num,"%d",val);
			ct_button_Text(dw_grades_button_examsscore[0],num,2);
			g_grades_changeColor(dw_grades_button_examsscore[0],val);			
			
	      // Pinyin?
	      ct_button_show(dw_grades_button_exams[1]);
	      ct_button_setVariable(dw_grades_button_exams[1],SUBGAME_EXCARDPINYIN);	      
	      ct_button_Text(dw_grades_button_exams[1],"Pinyin?",
								(dw_grades_lesson_number == ct_button_getVariable(dw_grades_button_exams[1]))?3:2);
	      // Pinyin? - Score
	      ct_button_show(dw_grades_button_examsscore[1]);
			val = data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXCARDPINYIN];
			sprintf(num,"%d",val);
			ct_button_Text(dw_grades_button_examsscore[1],num,2);
			g_grades_changeColor(dw_grades_button_examsscore[1],val);

	      // Meaning?
	      ct_button_show(dw_grades_button_exams[2]);
	      ct_button_setVariable(dw_grades_button_exams[2],SUBGAME_EXCARDMEAN);	      
	      ct_button_Text(dw_grades_button_exams[2],ct_const_getText(TXT_EXAM_MEANING,data_gameData[data_profile].language),
								(dw_grades_lesson_number == ct_button_getVariable(dw_grades_button_exams[2]))?3:2);
			// Meaning? - Score
	      ct_button_show(dw_grades_button_examsscore[2]);
			val = data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXCARDMEAN];
			sprintf(num,"%d",val);
			ct_button_Text(dw_grades_button_examsscore[2],num,2);
			g_grades_changeColor(dw_grades_button_examsscore[2],val);
			
	      break;
	   case GRADES_IS_LISTEN:
	      // Quiz?
	      ct_button_show(dw_grades_button_exams[0]);
	      ct_button_setVariable(dw_grades_button_exams[0],SUBGAME_EXLISTENQUIZ);
	      ct_button_Text(dw_grades_button_exams[0],"Quiz?",
								(dw_grades_lesson_number == ct_button_getVariable(dw_grades_button_exams[0]))?3:2);
	      // Quiz? - Score
	      ct_button_show(dw_grades_button_examsscore[0]);
			val = data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXLISTENQUIZ];
			sprintf(num,"%d",val);
			ct_button_Text(dw_grades_button_examsscore[0],num,2);
			g_grades_changeColor(dw_grades_button_examsscore[0],val);			
			break;
	}   	
	
	// Third, Update main button colours and main score
	ct_button_changetype(dw_grades_button_quiz[0],(dw_grades_lesson_type == GRADES_IS_QUIZ)?BTN_GREEN:BTN_GREY);
	val = (data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXQUIZHANZI] + 
			data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXQUIZPINYIN] + 
			data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXQUIZMEAN]) / 3;
	sprintf(num,"%d",val);
	ct_button_Text(dw_grades_button_quiz[1],num,2);
	g_grades_changeColor(dw_grades_button_quiz[1],val);
	
	ct_button_changetype(dw_grades_button_cards[0],(dw_grades_lesson_type == GRADES_IS_CARDS)?BTN_GREEN:BTN_GREY);
	val = (data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXCARDHANZI] + 
			data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXCARDPINYIN] + 
			data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXCARDMEAN]) / 3;
	sprintf(num,"%d",val);
	ct_button_Text(dw_grades_button_cards[1],num,2);
	g_grades_changeColor(dw_grades_button_cards[1],val);

	ct_button_changetype(dw_grades_button_listen[0],(dw_grades_lesson_type == GRADES_IS_LISTEN)?BTN_GREEN:BTN_GREY);		
	val = (data_gameData[data_profile].lessongrades[data_lessontype][data_lessonnumber][SUBGAME_EXLISTENQUIZ]) / 1;
	sprintf(num,"%d",val);
	ct_button_Text(dw_grades_button_listen[1],num,2);
	g_grades_changeColor(dw_grades_button_listen[1],val);
}   

void g_grades_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
}

void g_grades_ShowComment() {
	dw_grades_lesson_number = ct_button_getVariable(ct_button_getHandler()); 
	// TODO - Smiley Talking  
	g_grades_updateState();
}   

void g_grades_ChangeExam() {
   dw_grades_lesson_type = ct_button_getVariable(ct_button_getHandler());
   dw_grades_lesson_number = SUBGAME_;
   // TODO - Smiley Talking
   g_grades_updateState();
}   

/* Functions */ 
///////////////

void ct_g_grades_Start()
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
	ct_header_TextID(0,TXT_GRADES_,data_gameData[data_profile].language);
	
	// Load basic buttons
	dw_grades_button_quiz[0] = ct_button_add(BTN_GREY,80,BTN_32,-12,48);
	ct_button_TextID(dw_grades_button_quiz[0],TXT_EXAM_QUIZ,data_gameData[data_profile].language,2);
	ct_button_setVariable(dw_grades_button_quiz[0],GRADES_IS_QUIZ);
	ct_button_addHandler_Click(dw_grades_button_quiz[0],g_grades_ChangeExam);
	dw_grades_button_quiz[1] = ct_button_add(BTN_GREY,32,BTN_32,68,48);
	
	dw_grades_button_cards[0] = ct_button_add(BTN_GREY,80,BTN_32,-12,80);
	ct_button_TextID(dw_grades_button_cards[0],TXT_EXAM_CARDS,data_gameData[data_profile].language,2);
	ct_button_setVariable(dw_grades_button_cards[0],GRADES_IS_CARDS);	
	ct_button_addHandler_Click(dw_grades_button_cards[0],g_grades_ChangeExam);
	dw_grades_button_cards[1] = ct_button_add(BTN_GREY,32,BTN_32,68,80);
	
	dw_grades_button_listen[0] = ct_button_add(BTN_GREY,80,BTN_32,-12,112);
	ct_button_TextID(dw_grades_button_listen[0],TXT_EXAM_LISTEN,data_gameData[data_profile].language,2);
	ct_button_setVariable(dw_grades_button_listen[0],GRADES_IS_LISTEN);
	ct_button_addHandler_Click(dw_grades_button_listen[0],g_grades_ChangeExam);
	dw_grades_button_listen[1] = ct_button_add(BTN_GREY,32,BTN_32,68,112);
	
	// Load "Lesson" Buttons
	for (i = 0; i < GRADES_MAX_OPTIONS; i++) {
	   dw_grades_button_exams[i] = ct_button_add(BTN_GREY,96,BTN_32,
													112,
													48 + (i * 32));
		ct_button_addHandler_Click(dw_grades_button_exams[i],g_grades_ShowComment);
		dw_grades_button_examsscore[i] = ct_button_add(BTN_GREY,32,BTN_32,
													224,
													48 + (i * 32));		
	}
	
	// Load coloured "lesson" buttons (for preloading sprites purposes - TODO)
	dw_grades_button_lessondbg[0] = ct_button_add(BTN_GREEN,32,BTN_32,250,190);
	ct_button_hide(dw_grades_button_lessondbg[0]);
	dw_grades_button_lessondbg[1] = ct_button_add(BTN_RED,32,BTN_32,250,190);
	ct_button_hide(dw_grades_button_lessondbg[1]);
	dw_grades_button_lessondbg[2] = ct_button_add(BTN_BLUE,32,BTN_32,250,190);
	ct_button_hide(dw_grades_button_lessondbg[2]);
	   				
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_grades_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_grades_button_back,g_grades_launchBack);
	ct_button_addContentIma(dw_grades_button_back,(void *)arrow_b_Sprite,0,0,32,32);
		
	/* Init next state - Unique state */
	dw_grades_lesson_type = GRADES_IS_QUIZ;
	dw_grades_lesson_number = SUBGAME_;
	
	g_grades_updateState();
}

void ct_g_grades_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
} 


void ct_g_grades_Input()
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

void ct_g_grades_State()
{
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
}
