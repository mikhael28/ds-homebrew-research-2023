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
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// Internal State
#define FADING 0
#define SHOW 1
#define UNFADING 2

/* Variables - Internal State */
////////////////////////////////

int state;
int iterate;
int sram_int;

/* Internal Functions */ 
////////////////////////

void g_state_inishow() ;
void g_state_iniunfading();

// Function g_state_inifading() 
void g_state_inifading() 
{
	state = FADING;
	iterate = 32; // Counter - for fading
} // end of g_state_inifading() 

// Function g_state_fading() 
void g_state_fading() 
{
   if (iterate >= 0) {
   	iterate--;
   } else {
      g_state_inishow();
   }
} // end of g_state_fading() 

// Function g_state_inishow() 
void g_state_inishow() 
{
   // Actual status
	state = SHOW;
	iterate = GTIME_INTRO * 60; // Counter - for waiting a few seconds  :-)
	
	// SRAM operations
   if (!ct_utillib_checkInSRAM()) {
   	sram_int = 0;
      PA_CenterSmartText(0,0 , 148, 
								 256, 192, 
								"(Generating save data...) \n (Generando datos de la partida...)", 
								1, 2, 0);
 	}  	
	else {
	   sram_int = -1;
		ct_utillib_loadSRAM();
	}	
} // end of g_state_inishow() 

// Function g_state_show()
void g_state_show() 
{
   if (iterate >= 0) {
      // Wait...
   	iterate--;
   	// And also init the data :-)
   	if (sram_int != -1) 
   	   sram_int = ct_utillib_resetSRAM(sram_int);
   	if ((sram_int != -1) && (iterate == 0))
   		iterate = 1;
   } else {
      g_state_iniunfading();
   }
} // end of g_state_show() 

// Function g_state_iniunfading() 
void g_state_iniunfading() 
{
   state = UNFADING;
	iterate = 0; // Counter for fading
} // end of g_state_iniunfading() 

// Function g_state_unfading() 
void g_state_unfading() 
{
   if (iterate <= 32) {
   	//PA_WindowFade(1, 1, iterate); 
   	//PA_WindowFade(0, 1, iterate); 
   	iterate++;
   } else {
		ct_state_assignState(GAME_NIHAO); // Next State - Intro Menu
   }
} // end of g_state_unfading() 

/* Functions */ 
///////////////

// Function ct_g_intro_Start()
void ct_g_intro_Start()
{
   // Inits the Graphic mode for the Splash Screen
	ct_graphics_initGraphicMode(SCREENMODE_INTRO);
	
	/* Graphics */
	// Load Both Splash Screens
   PA_LoadPal(PAL_BG1,logo_palib_Pal);
   PA_Load8bitBitmap(1, // top screen
					logo_palib_Bitmap); // image
   PA_LoadPal(PAL_BG0,logo_rrc_Pal);
   PA_Load8bitBitmap(0, // bottom screen
					logo_rrc_Bitmap); // image					
					
	/* Variables*/
	
	/* Init next state */
	g_state_inifading();
} // end of ct_g_intro_Start()

// Function ct_g_intro_Finish()
void ct_g_intro_Finish()
{
	// Nothing here...
} // end of ct_g_intro_Finish()

// Function ct_g_intro_Input()
void ct_g_intro_Input()
{
	// if stylus released in state show, advance show
	if ((Stylus.Released) && (state == SHOW))
		g_state_iniunfading();
} // end of ct_g_intro_Input()

// Function ct_g_intro_State()
void ct_g_intro_State()
{
	switch (state) {
		case FADING:
		   g_state_fading();
			break;
		case SHOW:
		   g_state_show();
			break;
		case UNFADING:
		   g_state_unfading();
			break;			
	}
} // end of ct_g_intro_State()

