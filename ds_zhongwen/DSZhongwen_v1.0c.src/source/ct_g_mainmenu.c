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
#include "ct_ex_quiz.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define MAINMENU_BASIC 0


/* Variables - Internal State */
////////////////////////////////

int dw_mainmenu_button_back;
int dw_mainmenu_button_config;
int dw_mainmenu_button_contents;
int dw_mainmenu_button_lesson;
int dw_mainmenu_button_minigames;
int dw_mainmenu_button_review;
int dw_mainmenu_button_grades;

int mainmenu_state;
int mainmenu_substate;
int mainmenu_text;

/* Internal Functions */ 
////////////////////////

void g_mainmenu_launchBack(){
	ct_state_assignState(GAME_NIHAO);
	ct_utillib_saveSRAM(data_profile);
	ct_lesson_read(LESSON_,-1);
}

void g_mainmenu_launchConfig()
{
	ct_state_assignState(GAME_CONFIG);  // Next State - Configuration
}

void g_mainmenu_launchGetLesson()
{
	ct_state_assignState(GAME_GETLESSON);  // Next State - Configuration
}

void g_mainmenu_launchReviewHanzi() 
{
   if (data_lessontype == LESSON_) {
      if (data_gameData[data_profile].language == LANG_ENG)
		  ct_state_var_setStr("Review the hanzi of a lesson... if a lesson is selected. Go back and choose one!");
		else
		  ct_state_var_setStr("Aqui se pueden mirar los hanzi pertenecientes a una leccion... si tienes una leccion seleccionada.");
      ct_state_assignState(GAME_TODO);  // Next State - Configuration		  
		return;
   }   
   ct_state_assignState(GAME_REVIEWHANZI);  // Next State - Review Hanzi from the lessons
   ct_state_var_setInt(0,1); // [0]: Size (1)
   ct_state_var_setInt(1,0); // [1]: Index (0)
   
}

void g_mainmenu_launchLesson() 
{
   ct_state_assignState(GAME_LESSON); // Next state - launch generic lesson
}      

void g_mainmenu_launchExams()
{
   if (data_lessontype == LESSON_) {
      if (data_gameData[data_profile].language == LANG_ENG)
		  ct_state_var_setStr("Here you can ''play'' with the exams... but only if a lesson is selected. Go back and choose one!");
		else
		  ct_state_var_setStr("Aqui puedes probar tu nivel de chino respecto a una leccion... si tienes una leccion seleccionada.");
      ct_state_assignState(GAME_TODO);  // Next State - Configuration		  
		return;
   }   
   
	if (ct_utillib_bitOne16(data_gameData[data_profile].lesson[data_lessontype][(data_lessonnumber - 1) / 16],
									(data_lessonnumber - 1) % 16)) {
		ct_state_assignState(GAME_EXAMS);  // Next State - Choose an exam
	}	else {
   	   if (data_gameData[data_profile].language == LANG_ENG)
			  ct_state_var_setStr("Before doing an exam, you have to take the lesson first! So.... Go back.");
			else
			  ct_state_var_setStr("Antes de hacer un examen tienes que leer la leccion!. Vuelve atras y mirala.");
	      ct_state_assignState(GAME_TODO);  // Next State - Configuration			  
			return;
	}   
}

void g_mainmenu_launchGrades()
{
   if (data_lessontype == LESSON_) {
      if (data_gameData[data_profile].language == LANG_ENG)
		  ct_state_var_setStr("Check your grades in a certain lesson... but no lesson is selected. Go back and choose one!");
		else
		  ct_state_var_setStr("Comprueba tus notas de cada leccion... vuelve atras y selecciona una para verlas.");
      ct_state_assignState(GAME_TODO);  // Next State - Configuration		  
		return;
   }   
	ct_state_assignState(GAME_GRADES);  // Next State - Show the actual grades
}

/* Functions */ 
///////////////

void ct_g_MainMenu_Start()
{
	// Inits the Graphic mode for the *
	ct_graphics_initGraphicMode(SCREENMODE_INPUT);

	// Inits the variable "number of times in the main menu"
	data_nummainmenu++;
	
	/* Graphics */
	// Inits header subsystem, button subsystem, textscroll subsystem
	ct_button_init();
	ct_textscroll_init();
	ct_textscroll_openscroll_full();
	ct_header_init();
	ct_utillib_init();
			
	// Load Header Sprite, Up and Down
	ct_header_add(1,data_lessontype);
	ct_header_add(0,data_lessontype);
	ct_header_TextID(0,TXT_MM_,data_gameData[data_profile].language);
	
	// Load Buttons - Back Button
	dw_mainmenu_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);
	ct_button_addHandler_Click(dw_mainmenu_button_back,g_mainmenu_launchBack);
	ct_button_addContentIma(dw_mainmenu_button_back,(void *)arrow_b_Sprite,0,0,32,32);

	dw_mainmenu_button_config = ct_button_add(BTN_CONFIG,32,BTN_32,224,8);
	ct_button_addHandler_Click(dw_mainmenu_button_config,g_mainmenu_launchConfig);
	ct_button_addContentIma(dw_mainmenu_button_config,(void *)config_Sprite,0,0,32,32);

	dw_mainmenu_button_contents = ct_button_add(BTN_GREY,208,BTN_64,24,44);
	ct_button_addHandler_Click(dw_mainmenu_button_contents,g_mainmenu_launchGetLesson);
	ct_button_Text(dw_mainmenu_button_contents,ct_lesson_strHeader(),3);

	dw_mainmenu_button_lesson = ct_button_add(BTN_GREY,96,BTN_32,24,114);
	ct_button_addHandler_Click(dw_mainmenu_button_lesson,g_mainmenu_launchLesson);
	ct_button_TextID(dw_mainmenu_button_lesson,TXT_MM_LESSON,data_gameData[data_profile].language,2);
	
	dw_mainmenu_button_minigames = ct_button_add(BTN_GREY,96,BTN_32,24,152);
	ct_button_addHandler_Click(dw_mainmenu_button_minigames,g_mainmenu_launchExams);
	ct_button_TextID(dw_mainmenu_button_minigames,TXT_MM_EXAMS,data_gameData[data_profile].language,2);
	
	dw_mainmenu_button_review = ct_button_add(BTN_GREY,96,BTN_32,136,114);
	ct_button_addHandler_Click(dw_mainmenu_button_review,g_mainmenu_launchReviewHanzi);
	ct_button_TextID(dw_mainmenu_button_review,TXT_MM_IDEOGRAMS,data_gameData[data_profile].language,2);
	
	dw_mainmenu_button_grades  = ct_button_add(BTN_GREY,96,BTN_32,136,152);
	ct_button_addHandler_Click(dw_mainmenu_button_grades,g_mainmenu_launchGrades);
	ct_button_TextID(dw_mainmenu_button_grades,TXT_MM_GRADES,data_gameData[data_profile].language,2);
					
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);
	
	/* Init next state */
	mainmenu_state = MAINMENU_BASIC;
	mainmenu_substate = 0;
	mainmenu_text = 0;
}

void ct_g_MainMenu_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
} 


void ct_g_MainMenu_Input()
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

void ct_g_MainMenu_State()
{  
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
	
	// State management
	if (mainmenu_state == MAINMENU_BASIC) {
	   
	   switch (mainmenu_substate) {
	      case 0:
	         if (data_nummainmenu == 1)
	         	mainmenu_substate = 1; // Hi, (new user / existent user)
	         else
	         	mainmenu_substate = 10;// Welcome Back (to the main screen)
	         break;
	      case 1:
	         if (data_newuser == 1)
	         	mainmenu_substate = 30; // Full Explanation
	         else {
	         	mainmenu_substate = 20; // Just a Hi!
	      	}   
	      	break;
	      case 10:
	         ct_smiley_setTextMood(SMILEYMOOD_HAPPYJUMP,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_HI_AGAIN,data_gameData[data_profile].language,400,3,0))
	         	mainmenu_substate = 29;
	         break;
	      case 20:
	         ct_smiley_setTextMood(SMILEYMOOD_HAPPYJUMP,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_HI_1,data_gameData[data_profile].language,-1,3,0))
	         	mainmenu_substate = 29;
	         break;
	      case 29:
	         ct_smiley_setTextMood(SMILEYMOOD_WHAT,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_HI_REC,data_gameData[data_profile].language,-1,3,0))
	         	mainmenu_substate++;
	         break;	         	         
	      case 30:
	         ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_HI_INTRO,data_gameData[data_profile].language,-1,3,0))
	         	mainmenu_substate++;
	         break;	         
	      case 31:
	         ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_HI_EXPLAIN,data_gameData[data_profile].language,-1,3,0))
	         	mainmenu_substate++;
	         break;	         	         
	      case 32:
	         ct_smiley_setTextMood(SMILEYMOOD_NORMAL,SMILEYMOOD_NORMAL);
	         if (!ct_smiley_setTextExt(SMILEY_HI_END,data_gameData[data_profile].language,-1,3,0))
	         	mainmenu_substate = -1;
	         break;	         	         	         
		}		
	}	
}

