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
#include "ct_linkedlist.h"
#include "ct_utillib.h"
#include "ct_hwanihanzi.h"

#include "gfx/all_gfx.h" // PAGfx Include

// Constants

// Variables
int ani_screen;
int ani_x;
int ani_y;
int ani_delay;
int ani_delaymax;
int ani_frame;
int ani_framemax;
int ani_double;

// Clears the screen
void anihanzi_clear() {
   int i;
   int j;
   int mi = (!ani_double)?56:112;
   int mj = (!ani_double)?56:112;
   
   for (i = 0; i < mi; i++)
   	for (j = 0; j < mj; j++)
   		PA_Put8bitPixel(ani_screen,i + ani_x,j + ani_y,0);
}   

/* Init the anihanzi system */
void ct_anihanzi_init(int screen, int x, int y, int speed, int x2) {
   ani_screen = screen;
   ani_x = x;
   ani_y = y;
   ani_delay = speed;
   ani_delaymax = speed;
	ani_frame = -1;	
   ani_framemax = -1;
   if (x2 == 1)
		ani_double = 1;
	else ani_double = 0;
}   

/* Load an animated hanzi into the system */
void ct_anihanzi_load(char *pinyin) {
	ct_hwanihanzi_load(pinyin);
	ani_framemax = ct_hwanihanzi_getFrames();
	if (ani_framemax <= 0) {
		ani_framemax = -1;
		anihanzi_clear();
		return;
	}
	ani_frame = 0;	
	anihanzi_clear();
	ani_delay = ani_delaymax;
}   

/* Manages the anihanzi, including painting when is the moment */
void ct_anihanzi_manage() {
   if (ani_framemax == -1)
   	return;
   	
   ani_delay--;
	if (ani_delay == 0) {
	   // Return
	   ani_delay = ani_delaymax;
	   
	   // Paint
	   if (ani_frame == -1) {
	      anihanzi_clear();
	   } else {
	      if (ani_double)
		  		ct_hwanihanzi_draw2x(ani_screen,ani_x,ani_y,ani_frame);
		  	else 
		  	   ct_hwanihanzi_draw(ani_screen,ani_x,ani_y,ani_frame);
		}
		ani_frame++;
		if (ani_frame == ani_framemax)
			ani_frame = -1;
	} 
}   

