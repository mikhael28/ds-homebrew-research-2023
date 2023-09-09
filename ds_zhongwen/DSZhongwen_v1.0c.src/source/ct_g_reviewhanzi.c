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

/* List the hanzi of a lesson */
/*
	State[0] = Size. 
	State[1] = Index. 
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
#include "ct_HALhanzi.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// No internal state

/* Variables - Internal State */
////////////////////////////////

int dw_reviewhanzi_button_back;
int dw_reviewhanzi_button_1;
int dw_reviewhanzi_button_2;
int dw_reviewhanzi_button_3;
int dw_reviewhanzi_button_4;
int dw_reviewhanzi_button_hanzi1[12];
int dw_reviewhanzi_button_hanzi2[6];
int dw_reviewhanzi_button_hanzi3[3];
int dw_reviewhanzi_button_hanzi4[3];
int dw_reviewhanzi_button_hanziback;
int dw_reviewhanzi_button_hanziforth;
int dw_reviewhanzi_button_hanzinumber;

int dw_reviewhanzi_button_lessondbg[2];

int dw_reviewhanzi_index;
int dw_reviewhanzi_size;

/* Internal Functions */ 
////////////////////////

void g_reviewhanzi_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
}

int g_reviewhanzi_hanziLetter(int size) {
   switch (size){
      case 1:
         return 12;
      case 2:
         return 6;
      case 3:
         return 3;
      case 4:
         return 3;
   }   
   return 0;
}   

void g_reviewhanzi_nextState(){
   
   char innerstr[21]; // 5 * 4 + 1
   
   PA_CopyText(innerstr,
			ct_lesson_askWord(
				dw_reviewhanzi_size,
				dw_reviewhanzi_index + ct_button_getVariable(ct_button_getHandler())
				)
			);
   if (!PA_CompareText("",innerstr)) {
	   ct_state_assignState(GAME_LHANZI);   
	   ct_state_var_setInt(0,0); // Hanzi:[0] - 0, we are NOT inside a lesson.
   	ct_state_var_setInt(1,dw_reviewhanzi_index + 
				ct_button_getVariable(ct_button_getHandler())
									); // Hanzi:[1] - "word number"
	   ct_state_var_setInt(2,dw_reviewhanzi_size); // Hanzi:[2] - "word length"
   	ct_state_var_setInt(3,GAME_REVIEWHANZI);  // Hanzi:[3] - Screen to return.	   
   	ct_state_var_setInt(4,dw_reviewhanzi_size);  // Hanzi:[4] - Size.	   
   	ct_state_var_setInt(5,dw_reviewhanzi_index);  // Hanzi:[5] - Index.	   		   	
      ct_state_var_setStr(ct_lesson_askWord(ct_state_var_getInt(2),ct_state_var_getInt(1)));
	}
}

void g_reviewhanzi_updateState(){
   
   int i;
   char innerstr[21]; // 5 * 4 + 1
   
   // Before that... clean all the pixels
   ct_button_cleanContent(0);
   
   // First, Hides all buttons
   for (i = 0 ; i < 12; i++) {
	   ct_button_hide(dw_reviewhanzi_button_hanzi1[i]);
	}		 
	for (i = 0 ; i < 6; i++) {
	   ct_button_hide(dw_reviewhanzi_button_hanzi2[i]);
	}            
	for (i = 0 ; i < 3; i++) {
	   ct_button_hide(dw_reviewhanzi_button_hanzi3[i]);
	   ct_button_hide(dw_reviewhanzi_button_hanzi4[i]);	   
	}            
   
   // Now, show only buttons from the actual number
   switch (dw_reviewhanzi_size) {
      case 1:
		   for (i = 0 ; i < 12; i++) {
	   		ct_button_show(dw_reviewhanzi_button_hanzi1[i]);
			}   
         break;
      case 2:
		   for (i = 0 ; i < 6; i++) {
	   		ct_button_show(dw_reviewhanzi_button_hanzi2[i]);
			}            
         break;
      case 3:
		   for (i = 0 ; i < 3; i++) {
	   		ct_button_show(dw_reviewhanzi_button_hanzi3[i]);
			}                     
         break;
      case 4:
		   for (i = 0 ; i < 3; i++) {
	   		ct_button_show(dw_reviewhanzi_button_hanzi4[i]);
			}         
         break;
	}	      
   
   // Draw the hanzi of the respective number
   switch (dw_reviewhanzi_size) {
      case 1:
		   for (i = 0 ; i < 12; i++) {
		      PA_CopyText(innerstr,ct_lesson_askWord(dw_reviewhanzi_size,dw_reviewhanzi_index + i + 1));
		      if (!PA_CompareText("",innerstr)) {
		         ct_HALhanzi_paintX(0,innerstr, 
											ct_button_getX(dw_reviewhanzi_button_hanzi1[i]) + 
												(ct_button_getSizeX(dw_reviewhanzi_button_hanzi1[i]) >> 1), 
											ct_button_getY(dw_reviewhanzi_button_hanzi1[i]) + 
												(ct_button_getSizeY(dw_reviewhanzi_button_hanzi1[i]) >> 1)
											);
					ct_button_addContentPix(dw_reviewhanzi_button_hanzi1[i]);
		      }   
			}   
         break;
      case 2:
		   for (i = 0 ; i < 6; i++) {
		      PA_CopyText(innerstr,ct_lesson_askWord(dw_reviewhanzi_size,dw_reviewhanzi_index + i + 1));
		      if (!PA_CompareText("",innerstr)) {
		         ct_HALhanzi_paintX(0,innerstr, 
											ct_button_getX(dw_reviewhanzi_button_hanzi2[i]) + 
												(ct_button_getSizeX(dw_reviewhanzi_button_hanzi2[i]) >> 1), 
											ct_button_getY(dw_reviewhanzi_button_hanzi2[i]) + 
												(ct_button_getSizeY(dw_reviewhanzi_button_hanzi2[i]) >> 1)
											);
					ct_button_addContentPix(dw_reviewhanzi_button_hanzi2[i]);											
		      }   
			} 
         break;
      case 3:
		   for (i = 0 ; i < 3; i++) {
		      PA_CopyText(innerstr,ct_lesson_askWord(dw_reviewhanzi_size,dw_reviewhanzi_index + i + 1));
		      if (!PA_CompareText("",innerstr)) {
		         ct_HALhanzi_paintX(0,innerstr, 
											ct_button_getX(dw_reviewhanzi_button_hanzi3[i]) + 
												(ct_button_getSizeX(dw_reviewhanzi_button_hanzi3[i]) >> 1), 
											ct_button_getY(dw_reviewhanzi_button_hanzi3[i]) + 
												(ct_button_getSizeY(dw_reviewhanzi_button_hanzi3[i]) >> 1)
											);
					ct_button_addContentPix(dw_reviewhanzi_button_hanzi3[i]);											
		      }   
			}          
         break;
      case 4:
		   for (i = 0 ; i < 3; i++) {
		      PA_CopyText(innerstr,ct_lesson_askWord(dw_reviewhanzi_size,dw_reviewhanzi_index + i + 1));
		      if (!PA_CompareText("",innerstr)) {
		         ct_HALhanzi_paintX(0,innerstr, 
											ct_button_getX(dw_reviewhanzi_button_hanzi4[i]) + 
												(ct_button_getSizeX(dw_reviewhanzi_button_hanzi4[i]) >> 1), 
											ct_button_getY(dw_reviewhanzi_button_hanzi4[i]) + 
												(ct_button_getSizeY(dw_reviewhanzi_button_hanzi4[i]) >> 1)
											);
					ct_button_addContentPix(dw_reviewhanzi_button_hanzi4[i]);											
		      }   
			}          
         break;
	}	       
   
   
   // Update buttons depending on actual lesson.
	sprintf(innerstr,"%d/%d",
				(ct_lesson_lenWords(dw_reviewhanzi_size) == 0)? 0 : dw_reviewhanzi_index + 1,
				ct_lesson_lenWords(dw_reviewhanzi_size));
	ct_button_Text(dw_reviewhanzi_button_hanzinumber,innerstr,3);
	
	ct_button_changetype(dw_reviewhanzi_button_1,(dw_reviewhanzi_size == 1)?BTN_GREEN:BTN_GREY);
	ct_button_changetype(dw_reviewhanzi_button_2,(dw_reviewhanzi_size == 2)?BTN_GREEN:BTN_GREY);
	ct_button_changetype(dw_reviewhanzi_button_3,(dw_reviewhanzi_size == 3)?BTN_GREEN:BTN_GREY);
	ct_button_changetype(dw_reviewhanzi_button_4,(dw_reviewhanzi_size == 4)?BTN_GREEN:BTN_GREY);			
}

void g_reviewhanzi_nexthanzi(){
   if ((dw_reviewhanzi_index + g_reviewhanzi_hanziLetter(dw_reviewhanzi_size)) 
		  < ct_lesson_lenWords(dw_reviewhanzi_size)) {
		dw_reviewhanzi_index += g_reviewhanzi_hanziLetter(dw_reviewhanzi_size);	
   	g_reviewhanzi_updateState();
	}   
}

void g_reviewhanzi_previoushanzi(){
   if (dw_reviewhanzi_index > 0) {
		dw_reviewhanzi_index -= g_reviewhanzi_hanziLetter(dw_reviewhanzi_size);
   	g_reviewhanzi_updateState();
 	} 
}

void g_reviewhanzi_Letters(){
   dw_reviewhanzi_size = ct_button_getVariable(ct_button_getHandler());
   dw_reviewhanzi_index = 0;
   g_reviewhanzi_updateState();
}

/* Functions */ 
///////////////

void ct_g_reviewhanzi_Start()
{
	int i;
	int j;
	int k;
	
	// Inits the Graphic mode for the reviewhanzi
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
	ct_header_TextID(0,TXT_REVIEWHANZI_,data_gameData[data_profile].language);
	
	// Load basic buttons
	dw_reviewhanzi_button_1 = ct_button_add(BTN_GREY,64,BTN_32,-12,48);
	ct_button_addHandler_Click(dw_reviewhanzi_button_1,g_reviewhanzi_Letters);
	ct_button_setVariable(dw_reviewhanzi_button_1,1);
	ct_button_Text(dw_reviewhanzi_button_1,"1",3);
	dw_reviewhanzi_button_2 = ct_button_add(BTN_GREY,64,BTN_32,-12,80);
	ct_button_addHandler_Click(dw_reviewhanzi_button_2,g_reviewhanzi_Letters);
	ct_button_setVariable(dw_reviewhanzi_button_2,2);	
	ct_button_Text(dw_reviewhanzi_button_2,"2",3);	
	dw_reviewhanzi_button_3 = ct_button_add(BTN_GREY,64,BTN_32,-12,112);
	ct_button_addHandler_Click(dw_reviewhanzi_button_3,g_reviewhanzi_Letters);
	ct_button_setVariable(dw_reviewhanzi_button_3,3);	
	ct_button_Text(dw_reviewhanzi_button_3,"3",3);	
	dw_reviewhanzi_button_4 = ct_button_add(BTN_GREY,64,BTN_32,-12,144);
	ct_button_addHandler_Click(dw_reviewhanzi_button_4,g_reviewhanzi_Letters);
	ct_button_setVariable(dw_reviewhanzi_button_4,4);
	ct_button_Text(dw_reviewhanzi_button_4,"4",3);	
	
	// Load "Hanzi" Buttons
	for (j = 0; j < 3; j++) {
	   for (i = 0; i < 4; i++) {
	      k = (i + (j*4));
	      dw_reviewhanzi_button_hanzi1[k] = ct_button_add(BTN_NONE,32,BTN_32,
													64 + ((i) * 40),
													36 + ((j) * 40));	      
			ct_button_setVariable(dw_reviewhanzi_button_hanzi1[k],k + 1);			
			ct_button_addHandler_Click(dw_reviewhanzi_button_hanzi1[k],g_reviewhanzi_nextState);
		}
	}				
	for (j = 0; j < 3; j++) {
	   for (i = 0; i < 2; i++) {
	      k = (i + (j*2));
	      dw_reviewhanzi_button_hanzi2[k] = ct_button_add(BTN_NONE,64,BTN_32,
													72 + ((i) * 70),
													36 + ((j) * 40));	      
			ct_button_setVariable(dw_reviewhanzi_button_hanzi2[k],k + 1);			
			ct_button_addHandler_Click(dw_reviewhanzi_button_hanzi2[k],g_reviewhanzi_nextState);
		}
	}				
   for (i = 0; i < 3; i++) {
      k = i;
      dw_reviewhanzi_button_hanzi3[k] = ct_button_add(BTN_NONE,96,BTN_32,
												92,
												36 + ((i) * 40));	      
		ct_button_setVariable(dw_reviewhanzi_button_hanzi3[k],k + 1);			
		ct_button_addHandler_Click(dw_reviewhanzi_button_hanzi3[k],g_reviewhanzi_nextState);
	}				
   for (i = 0; i < 3; i++) {
      k = i;
      dw_reviewhanzi_button_hanzi4[k] = ct_button_add(BTN_NONE,128,BTN_32,
												78,
												36 + ((i) * 40));	      
		ct_button_setVariable(dw_reviewhanzi_button_hanzi4[k],k + 1);			
		ct_button_addHandler_Click(dw_reviewhanzi_button_hanzi4[k],g_reviewhanzi_nextState);
	}				

	dw_reviewhanzi_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,8);	
	ct_button_addHandler_Click(dw_reviewhanzi_button_back,g_reviewhanzi_launchBack);
	ct_button_addContentIma(dw_reviewhanzi_button_back,(void *)arrow_b_Sprite,0,0,32,32);

	
	dw_reviewhanzi_button_hanziforth = ct_button_add(BTN_GREY,32,BTN_32,184,156);
	ct_button_addContentIma(dw_reviewhanzi_button_hanziforth,(void *)arrow_r_Sprite,0,0,32,32);		
	ct_button_addHandler_Click(dw_reviewhanzi_button_hanziforth,g_reviewhanzi_nexthanzi);
	dw_reviewhanzi_button_hanziback = ct_button_add(BTN_GREY,32,BTN_32,64,156);
	ct_button_addContentIma(dw_reviewhanzi_button_hanziback,(void *)arrow_l_Sprite,0,0,32,32);	
	ct_button_addHandler_Click(dw_reviewhanzi_button_hanziback,g_reviewhanzi_previoushanzi);	
	dw_reviewhanzi_button_hanzinumber = ct_button_add(BTN_NONE,80,BTN_32,100,156);

	// Load coloured "lesson" buttons (for preloading sprites purposes - TODO)
	dw_reviewhanzi_button_lessondbg[0] = ct_button_add(BTN_GREEN,32,BTN_32,250,190);
	ct_button_hide(dw_reviewhanzi_button_lessondbg[0]);
	dw_reviewhanzi_button_lessondbg[1] = ct_button_add(BTN_RED,32,BTN_32,250,190);
	ct_button_hide(dw_reviewhanzi_button_lessondbg[1]);
	
	
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);
	
	/* Init next state - Unique state */
	dw_reviewhanzi_size = ct_state_var_getInt(0);	
	dw_reviewhanzi_index = ct_state_var_getInt(1);
	g_reviewhanzi_updateState();
	//ct_button_changetype(dw_button_back,BTN32_GREEN);
	//dw_button_back = ct_button_add(BTN32_GREEN,32,100,8);

}

void ct_g_reviewhanzi_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
} 


void ct_g_reviewhanzi_Input()
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

void ct_g_reviewhanzi_State()
{
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
}
