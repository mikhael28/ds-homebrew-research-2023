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
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

#define TZ_SIZE 128

/* Variables - Internal State */
////////////////////////////////

int dw_textzone_x;
int dw_textzone_y;
int dw_textzone_oldx;
int dw_textzone_oldy;
int dw_textzone_sx;
int dw_textzone_sy;
int dw_textzone_size;

/* Internal Functions */ 
////////////////////////

/* Functions */ 
///////////////

/* Creates the TextZone system in a certain area*/
void ct_textzone_init(int x, int y, int size)
{
   if (size < 3) {
   	dw_textzone_sx = TZ_SIZE * size;
   	dw_textzone_sy = TZ_SIZE;   
	} else {
   	dw_textzone_sx = (TZ_SIZE >> 1) * size;
   	dw_textzone_sy = TZ_SIZE >> 1;   	   
	}   
   dw_textzone_x = x;
   dw_textzone_y = y;
   dw_textzone_oldx = -1;
   dw_textzone_oldy = -1;   
   dw_textzone_size = 1;
}

/* Set the pencil size */
void ct_textzone_setPenSize(int size)
{
   dw_textzone_size = size;
}


/* Returns 1 if the coordinate is inside the textarea */
int ct_textzone_inArea(int x,int y)
{
   if ((x >= dw_textzone_x) && (x <= dw_textzone_x + dw_textzone_sx) &&
   			(y >= dw_textzone_y) && (y <= dw_textzone_y + dw_textzone_sy))
   	return 1;
   else {
      dw_textzone_oldx = -1;
      dw_textzone_oldy = -1;      
	   return 0;
	}   
}		

/* Resets the textzone system, should be called when the stylus is not held */
void ct_textzone_reset() {
   dw_textzone_oldx = -1;
   dw_textzone_oldy = -1;      
}		

/* Clears the textzone */
void ct_textzone_clear() 
{
   int tx;
   int ty;
 	for (tx = dw_textzone_x; tx <= dw_textzone_x + dw_textzone_sx; tx++)
		for (ty = dw_textzone_y; ty <= dw_textzone_y + dw_textzone_sy; ty++)
			PA_Put8bitPixel(0,tx,ty,0);

}   


/* Writes a pixel on the coordinate indicated as a parameter - Always screen 0 */
void ct_textzone_paint(int x,int y)
{
   int tx, ty;
   
   if (dw_textzone_oldx == -1) {
      dw_textzone_oldx = x;
      dw_textzone_oldy = y;
	   if (dw_textzone_size == 1) {
   		PA_Put8bitPixel(0,x,y,1);
   		return;
	 	} else {
		 	for (tx = (x - (dw_textzone_size >> 1)); tx <= (x + (dw_textzone_size >> 1)); tx++)
 	 			for (ty = (y - (dw_textzone_size >> 1)); ty <= (y + (dw_textzone_size >> 1)); ty++)
 	 				PA_Put8bitPixel(0,tx,ty,1);
 		}	 	
   } else {
	   if (dw_textzone_size == 1) {
	      PA_Draw8bitLine(0,dw_textzone_oldx,dw_textzone_oldy,x,y,1);
	 	} else {
	 	   PA_Draw8bitLineEx(0,dw_textzone_oldx,dw_textzone_oldy,x,y,1,dw_textzone_size);
 		}	 	
      dw_textzone_oldx = x;
      dw_textzone_oldy = y; 		
   }   
   
}   
