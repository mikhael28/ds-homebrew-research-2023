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

/*
   ==========================================================
	DSZhongwen - Codename FR (First Release)
	Version 1.0b. Build 0.
	==========================================================
	
	TODO
	- Header change = Corrupt Memory (Free Gfx, Free Sprites, Free Pals?)
		(Maybe del sprite when changing system)
		(Possibly, same error in buttons)
		
	++CHANGE++
	- Changes To-DO
	
*/

// Includes
#include <PA9.h>       // Include for PA_Lib
#include <fat.h>
#include <string.h>
#include <sys/dir.h>

#include "ct_const.h" 
#include "ct_graphics.h" 
#include "ct_state.h"  
#include "ct_lesson.h"  
#include "ct_utillib.h"  
#include "ct_g_intro.h"  
#include "ct_g_nihao.h"
#include "ct_g_config.h"
#include "ct_g_mainmenu.h"
#include "ct_g_credits.h"
#include "ct_g_getlesson.h"
#include "ct_g_reviewhanzi.h"
#include "ct_g_lesson.h"
#include "ct_g_lhanzi.h"
#include "ct_g_llisten.h"
#include "ct_g_lgram.h"
#include "ct_g_ex.h"
#include "ct_g_grades.h"
#include "ct_g_todo.h"
#include "ct_ex_quiz.h"
#include "ct_ex_card.h"
#include "ct_ex_listen.h"
#include "ct_hwanihanzi.h"
#include "ct_g_newuser.h"
#include "ct_g_dict.h"

// Function: mainManageInput()
void mainManageInput() 
{
	switch (ct_state_getState()) {
		case GAME_:
			break;
		case GAME_INTRO :
			ct_g_intro_Input();
			break;
		case GAME_NIHAO :
			ct_g_nihao_Input();
			break;
		case GAME_CONFIG :
			ct_g_config_Input();	
			break;
		case GAME_CREDITS :
		   ct_g_credits_Input();
			break;
		case GAME_MAINMENU :
			ct_g_MainMenu_Input();	
			break;
		case GAME_GETLESSON :
			ct_g_getlesson_Input();	
			break;			
		case GAME_REVIEWHANZI :
			ct_g_reviewhanzi_Input();	
			break;			
		case GAME_LESSON :
			ct_g_lesson_Input();	
			break;			
		case GAME_LHANZI :
			ct_g_lhanzi_Input();	
			break;			
		case GAME_LGRAM :
			ct_g_lgram_Input();	
			break;						
		case GAME_LLISTEN :
			ct_g_llisten_Input();	
			break;			
		case GAME_EXAMS :
			ct_g_ex_Input();	
			break;						
		case GAME_EX_QUIZ :
			ct_ex_quiz_Input();	
			break;									
		case GAME_EX_CARD :
			ct_ex_card_Input();	
			break;			
		case GAME_EX_LISTEN :
			ct_ex_listen_Input();	
			break;			
		case GAME_GRADES :
			ct_g_grades_Input();	
			break;						
		case GAME_NEWUSER :
			ct_g_newuser_Input();	
			break;			
		case GAME_TODO :
			ct_g_todo_Input();	
			break;
		case GAME_DICT :
			ct_g_dict_Input();	
			break;
	}	
} // end of mainManageInput()

// Function: mainManageGameState()
void mainManageGameState() 
{
	switch (ct_state_getState()) {
		case GAME_:
			break;
		case GAME_INTRO :
			ct_g_intro_State();
			break;
		case GAME_NIHAO :
		   ct_g_nihao_State();
			break;
		case GAME_CONFIG :
			ct_g_config_State();
			break;
		case GAME_CREDITS :
		   ct_g_credits_State();
			break;
		case GAME_MAINMENU :
			ct_g_MainMenu_State();			
			break;
		case GAME_GETLESSON :
			ct_g_getlesson_State();	
			break;						
		case GAME_REVIEWHANZI :
			ct_g_reviewhanzi_State();	
			break;			
		case GAME_LESSON :
			ct_g_lesson_State();	
			break;			
		case GAME_LHANZI :
			ct_g_lhanzi_State();	
			break;			
		case GAME_LGRAM :
			ct_g_lgram_State();	
			break;									
		case GAME_LLISTEN :
			ct_g_llisten_State();	
			break;			
		case GAME_EXAMS :
			ct_g_ex_State();	
			break;									
		case GAME_EX_QUIZ :
			ct_ex_quiz_State();	
			break;		
		case GAME_EX_CARD :
			ct_ex_card_State();	
			break;																			
		case GAME_EX_LISTEN :
			ct_ex_listen_State();	
			break;		
		case GAME_GRADES :
			ct_g_grades_State();	
			break;						
		case GAME_NEWUSER :
			ct_g_newuser_State();	
			break;			
		case GAME_DICT :
			ct_g_dict_State();	
			break;
		case GAME_TODO :
			ct_g_todo_State();	
			break;
	}	
} // end of mainManageGameState()

// Function: mainChangeGameState()
void mainChangeGameState()
{
	// Finishes SubSystem
	switch (ct_state_getState()) {
		case GAME_:
			break;
		case GAME_INTRO :
			ct_g_intro_Finish();
			break;
		case GAME_NIHAO :
			ct_g_nihao_Finish();
			break;
		case GAME_CONFIG :
			ct_g_config_Finish();
			break;
		case GAME_CREDITS :
		   ct_g_credits_Finish();
			break;
		case GAME_MAINMENU :
			ct_g_MainMenu_Finish();			
			break;
		case GAME_GETLESSON :
			ct_g_getlesson_Finish();	
			break;						
		case GAME_REVIEWHANZI :
			ct_g_reviewhanzi_Finish();	
			break;
		case GAME_LESSON :
			ct_g_lesson_Finish();	
			break;			
		case GAME_LHANZI :
			ct_g_lhanzi_Finish();	
			break;						
		case GAME_LLISTEN :
			ct_g_llisten_Finish();	
			break;
		case GAME_LGRAM :
			ct_g_lgram_Finish();	
			break;												
		case GAME_EXAMS :
			ct_g_ex_Finish();	
			break;									
		case GAME_EX_QUIZ :
			ct_ex_quiz_Finish();	
			break;			
		case GAME_EX_CARD :
			ct_ex_card_Finish();	
			break;
		case GAME_EX_LISTEN :
			ct_ex_listen_Finish();	
			break;												
		case GAME_GRADES :
			ct_g_grades_Finish();	
			break;	
		case GAME_NEWUSER :
			ct_g_newuser_Finish();	
			break;						
		case GAME_DICT :
			ct_g_dict_Finish();	
			break;
		case GAME_TODO :
			ct_g_todo_Finish();	
			break;
	}
  ct_state_updateState();
  // Starts new System
	switch (ct_state_getState()) {
		case GAME_:
			break;
		case GAME_INTRO :
			ct_g_intro_Start();
			break;
		case GAME_NIHAO :
			ct_g_nihao_Start();
			break;
		case GAME_CONFIG :
			ct_g_config_Start();
			break;
		case GAME_CREDITS :
		   ct_g_credits_Start();
			break;
		case GAME_MAINMENU :
			ct_g_MainMenu_Start();			
			break;
		case GAME_GETLESSON :
			ct_g_getlesson_Start();	
			break;		
		case GAME_REVIEWHANZI :
			ct_g_reviewhanzi_Start();	
			break;		
		case GAME_LESSON :
			ct_g_lesson_Start();	
			break;					
		case GAME_LHANZI :
			ct_g_lhanzi_Start();	
			break;			
		case GAME_LLISTEN :
			ct_g_llisten_Start();	
			break;			
		case GAME_LGRAM :
			ct_g_lgram_Start();	
			break;									
		case GAME_EXAMS :
			ct_g_ex_Start();	
			break;									
		case GAME_EX_QUIZ :
			ct_ex_quiz_Start();	
			break;			
		case GAME_EX_CARD :
			ct_ex_card_Start();	
			break;
		case GAME_EX_LISTEN :
			ct_ex_listen_Start();	
			break;								
		case GAME_GRADES :
			ct_g_grades_Start();	
			break;
		case GAME_NEWUSER :
			ct_g_newuser_Start();	
			break;						
		case GAME_DICT :
			ct_g_dict_Start();	
			break;
		case GAME_TODO :
			ct_g_todo_Start();	
			break;
	}
} // end of mainChangeGameState()

// Show error "No FAT" message, hangs
void mainNoFAT() {
   PA_CenterSmartText(1,0 , 0, 
								256, 192, 
								"NO DLDI", 
								1, 4, 0);	

   PA_CenterSmartText(0,0 , 0, 
								256, 64, 
								"DLDI Unavailable, Error while reading. \n Please patch this ROM with the DLDI tool.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 64, 
								256, 128, 
								"DLDI No disponible, error al leer. \n Por favor parchee esta ROM con \n la herramienta DLDI.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 128, 
								256, 192, 
								"http://chishm.drunkencoders.com/DLDI/", 
								1, 2, 0);									
								
   while (1) {
      // Hangs.... :-(
      PA_WaitForVBL();
   }   
}

// Show error "No DSZhongwen" message, hangs
void mainNoDSZhongwen() {
   PA_CenterSmartText(1,0 , 0, 
								256, 192, 
								"NO /data/DSZhongwen", 
								1, 4, 0);	

   PA_CenterSmartText(0,0 , 0, 
								256, 64, 
								"Directory /data/DSZhongwen not found. \n Please download it and copy it \n to the flash cart.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 64, 
								256, 128, 
								"Directorio /data/DSZhongwen no existe. \n Por favor descarguelo y copielo \n en su flash cart.", 
								1, 2, 0);	
   PA_CenterSmartText(0,0 , 128, 
								256, 192, 
								"http://www.rrc2soft.com", 
								1, 2, 0);									
								
   while (1) {
      // Hangs.... :-(
      PA_WaitForVBL();
   }   
}   

// Function: main()
int main(int argc, char ** argv)
{
	PA_Init();    // Initializes PA_Lib
	PA_InitVBL(); // Initializes a standard VBL
	ct_graphics_init(); // Initializes the graphic subsystem
	PA_InitSound(); // Initializes the sound subsystem
	PA_InitRand(); // Initializes the random subsystem
	
	PA_WaitForVBL();  PA_WaitForVBL();  PA_WaitForVBL();  // wait a few VBLs	
	if (!fatInitDefault()) { //Initialize fat library
		// No fat?
		mainNoFAT();
	}	
	if (diropen(DIR_MAIN) == NULL) {
	   mainNoDSZhongwen();
	}   
	PA_WaitForVBL();  
	
	ct_state_init();	
	ct_state_assignState(GAME_INTRO);  // Alert the system that the first state is the intro
	
	ct_lesson_init(); // Starts the lesson subsystem
	
	//++CHANGE++ See where to put this, and where to put the other hw inits (i.e. meaning)
	ct_hwanihanzi_init();	

	// For "solving a bug" (null in a malloc of having too many buttons)
	// ...have to check whether i am losing memory somewhere... :-(
	//ct_g_reviewhanzi_Start();
	//ct_g_reviewhanzi_Finish();

	// Infinite loop to keep the program running
	while (1)
	{
	   // Change the actual State of the Game
	   if (ct_state_newState()) {
	     mainChangeGameState();
		}   
	   
	   // Manage the input of the user in this State
      mainManageInput();
      
      // Manage the state of the game itself
      if (!ct_state_newState())
      	mainManageGameState();	
      	
      // Checks if lid is closed, manage cases where the ds should not be paused
      if (!data_ignorelid) {
	      PA_CheckLid();
	   } else {
		   if (PA_LidClosed()) {
		      if (data_lidopen) {
		   		PA_SetScreenLight(0,0);
		   		PA_SetScreenLight(1,0);
		 		}  
				data_lidopen = 0;	
		 	} else {
		 	   if (!data_lidopen) {
		   		PA_SetScreenLight(0,1);
		   		PA_SetScreenLight(1,1);
		  		} 		
		  		data_lidopen = 1;
			}		   	
		}			   
      
      // Waits for the vertical retrace
      ct_utillib_iterate();
		PA_WaitForVBL();
	}
	
	return 0;
} // End of main()
