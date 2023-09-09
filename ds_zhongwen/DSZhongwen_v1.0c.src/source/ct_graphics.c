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
#include "ct_const.h" // Constants of the game

#include "gfx/all_gfx.c" // PAGfx Include
#include "gfx/all_gfx.h" // PAGfx Include

int actualspriteidx[2];
int actualpaletteidx[2];

int graphics_uppermode;
int graphics_lowermode;

int dummypal;

// Function: ct_graphics_getSprite(int screen)
int ct_graphics_getSprite(int screen)
{
	// NO CHECK if it is greater than 128 :-P... should put some debug warning here, or hang the game...
	if (screen == 0)
		return actualspriteidx[0]++;
	else 
		return actualspriteidx[1]++;
}

// Function: ct_graphics_getPalette(int screen)
int ct_graphics_getPalette(int screen)
{	
	// NO CHECK if it is greater than 16 :-P... should put some debug warning here, or hang the game...
	int res;
	res = actualpaletteidx[screen];
	actualpaletteidx[screen] = actualpaletteidx[screen] + 1;
	return res;
}

// Function: ct_graphics_initGraphicMode(int mode)
void ct_graphics_initGraphicMode(int mode)
{	
   if (graphics_uppermode != SCREENGR_) {
	   PA_ClearTextBg(1);   
	   PA_Clear8bitBg(1);
	   PA_Clear8bitBg(0);
      
      if (actualspriteidx[0] != 0) 
			PA_ResetSpriteSysScreen(0);	   
      if (actualspriteidx[1] != 0) 
			PA_ResetSpriteSysScreen(1);	   

	}	
		
   actualspriteidx[0] = 0; // Reset the sprite numbering (TODO - 0,1?)
   actualspriteidx[1] = 0; // Reset the sprite numbering (TODO - 0,1?)
   actualpaletteidx[0] = 0; // Reset palette
   actualpaletteidx[1] = 0; // Reset palette   
   
   
	switch (mode) {
	  case SCREENMODE_INTRO:
	    graphics_uppermode = SCREENGR_8BIT;
	    graphics_lowermode = SCREENGR_8BIT;
	    break;
	  case SCREENMODE_TILE:
	    graphics_uppermode = SCREENGR_TEXT;
	    graphics_lowermode = SCREENGR_TEXT;
	    break;
	  case SCREENMODE_INPUT:
	    graphics_uppermode = SCREENGR_TEXT;
	    graphics_lowermode = SCREENGR_8BIT;
	    break;	    
	}
	
   // We'll change the back color to white, always
   PA_SetBgPalCol(1, 0, PA_RGB(31, 31, 31));   
   PA_SetBgPalCol(0, 0, PA_RGB(31, 31, 31));
   PA_SetBgPalCol(1, 1, PA_RGB(0, 0, 0));
   PA_SetBgPalCol(0, 1, PA_RGB(0, 0, 0));   
   PA_SetBgPalCol(1, 2, PA_RGB(0, 31, 0));
   PA_SetBgPalCol(0, 2, PA_RGB(0, 31, 0));   
   PA_SetBgPalCol(1, 3, PA_RGB(31, 0, 0));
   PA_SetBgPalCol(0, 3, PA_RGB(31, 0, 0));   
      
   // Dummy pal
   dummypal = ct_graphics_getPalette(1);
} // End of initGraphicMode(int mode)

void ct_graphics_init()
{
	graphics_uppermode = SCREENGR_;
	graphics_lowermode = SCREENGR_;
	
   PA_Init8bitBg(1, 1); // Top Screen: Image
   PA_InitCustomText(1,0,font_arial); // Top Screen: Text mode
   PA_SetTextTileCol(1, TEXT_BLACK);
   PA_Init8bitBg(0, 1); // Bottom Screen: Image	
   
   actualspriteidx[0] = 0; // Reset the sprite numbering (TODO - 0,1?)
   actualspriteidx[1] = 0; // Reset the sprite numbering (TODO - 0,1?)
   actualpaletteidx[0] = 0; // Reset palette
   actualpaletteidx[1] = 0; // Reset palette   
   
   // We'll change the back color to white, always
   PA_SetBgPalCol(1, 0, PA_RGB(31, 31, 31));   
   PA_SetBgPalCol(0, 0, PA_RGB(31, 31, 31));
   PA_SetBgPalCol(1, 1, PA_RGB(0, 0, 0));
   PA_SetBgPalCol(0, 1, PA_RGB(0, 0, 0));  
   PA_SetBgPalCol(1, 2, PA_RGB(0, 31, 0));
   PA_SetBgPalCol(0, 2, PA_RGB(0, 31, 0));   
}

/*void ct_graphics_initGraphicMode(int mode)
{
   //if (graphics_uppermode == SCREENGR_TEXT)
	//   PA_ResetSpriteSysScreen(1);
   //if (graphics_lowermode == SCREENGR_TEXT)
	//   PA_ResetSpriteSysScreen(0);	   
   if (graphics_uppermode == SCREENGR_8BIT)
	   PA_ResetBgSysScreen(1);
   if (graphics_lowermode == SCREENGR_8BIT)
	   PA_ResetBgSysScreen(0);	   
	PA_ResetSpriteSysScreen(1);
	PA_ResetSpriteSysScreen(0);
	
	PA_ResetBgSys();
	PA_ResetSpriteSys();	   
	
   actualspriteidx[0] = 1; // Reset the sprite numbering (TODO - 0,1?)
   actualspriteidx[1] = 1; // Reset the sprite numbering (TODO - 0,1?)
   actualpaletteidx[0] = 0; // Reset palette
   actualpaletteidx[1] = 0; // Reset palette   
	switch (mode) {
	  case SCREENMODE_INTRO:
	    //if (graphics_uppermode != SCREENGR_8BIT)
   	 	PA_Init8bitBg(1, 1); // Top Screen: Image
   	 //if (graphics_lowermode != SCREENGR_8BIT)
	    	PA_Init8bitBg(0, 1); // Bottom Screen: Image	    
	    graphics_uppermode = SCREENGR_8BIT;
	    graphics_lowermode = SCREENGR_8BIT;
	    break;
	  case SCREENMODE_TILE:
	    //if (graphics_uppermode != SCREENGR_TEXT)
   	 	PA_InitText(1, 0); // Top Screen: Text mode
   	 //if (graphics_lowermode != SCREENGR_TEXT)
	    	PA_InitText(0, 0); // Bottom Screen: Text mode	  
	    graphics_uppermode = SCREENGR_TEXT;
	    graphics_lowermode = SCREENGR_TEXT;
	    break;
	  case SCREENMODE_INPUT:
	  	 //if (graphics_uppermode != SCREENGR_TEXT)
   	 	PA_InitText(1, 0); // Top Screen: Text mode
   	 //if (graphics_lowermode != SCREENGR_8BIT)
	    	PA_Init8bitBg(0, 1); // Bottom Screen: Image
	    graphics_uppermode = SCREENGR_TEXT;
	    graphics_lowermode = SCREENGR_8BIT;
	    break;	    
	}
   // We'll change the back color to white, always
   PA_SetBgPalCol(1, 0, PA_RGB(31, 31, 31));   
   PA_SetBgPalCol(0, 0, PA_RGB(31, 31, 31));
   PA_SetBgPalCol(1, 1, PA_RGB(0, 0, 0));
   PA_SetBgPalCol(0, 1, PA_RGB(0, 0, 0));   
   
	PA_InitCustomText(1,0,font_arial); //font name   
	PA_SetTextTileCol(1, TEXT_BLACK);
   
   // Dummy pal
   dummypal = ct_graphics_getPalette(1);
} // End of initGraphicMode(int mode)
*/
