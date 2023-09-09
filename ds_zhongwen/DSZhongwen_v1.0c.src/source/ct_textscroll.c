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

#include "gfx/all_gfx.h" // PAGfx Include

/* Variables - Internal State */
////////////////////////////////

#define TEXT_POS_XINI 0
#define TEXT_POS_XEND 160
#define TEXT_POS_Y 24

int up_textpal; // related to the textbox
int up_textscrolla;
int up_textscrollb;
int up_texta00000;
int up_textb00000;
int up_text0a0000;
int up_text0b0000;
int up_text00a000;
int up_text00b000;
int up_text000a00;
int up_text000b00;
int up_text0000a0;
int up_text0000b0;
int up_text00000a;
int up_text00000b;
u16 up_hidetext10_gfx;
u16 up_hidetext01_gfx;
u16 up_text10_gfx;
u16 up_text01_gfx;
u16 up_textscroll10_gfx;
u16 up_textscroll01_gfx;

/* Internal Functions */ 
////////////////////////

/* Functions */ 
///////////////

void ct_textscroll_init()
{
   // Load Text Gfxs
	up_textpal = ct_graphics_getPalette(1);
	PA_LoadSpritePal(1,up_textpal,(void*)up_text_Pal);
	up_hidetext10_gfx = PA_CreateGfx(1,(void*)up_hidetext10_Sprite, OBJ_SIZE_32X64, 1);
	up_hidetext01_gfx = PA_CreateGfx(1,(void*)up_hidetext01_Sprite, OBJ_SIZE_32X64, 1);
	up_text10_gfx = PA_CreateGfx(1,(void*)up_text10_Sprite, OBJ_SIZE_32X64, 1);
	up_text01_gfx = PA_CreateGfx(1,(void*)up_text01_Sprite, OBJ_SIZE_32X64, 1);
	up_textscroll10_gfx = PA_CreateGfx(1,(void*)up_textscroll10_Sprite, OBJ_SIZE_32X64, 1);
	up_textscroll01_gfx = PA_CreateGfx(1,(void*)up_textscroll01_Sprite, OBJ_SIZE_32X64, 1);

   // Load Text Sprites, in hidden state 
   up_textscrolla = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_textscrolla,up_textscroll10_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI,TEXT_POS_Y);		
	PA_SetSpritePrio(1,up_textscrolla,2);
   up_textscrollb = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_textscrollb,up_textscroll01_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI,TEXT_POS_Y + 64);									
	PA_SetSpritePrio(1,up_textscrollb,2);
	
   up_texta00000 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_texta00000,up_hidetext10_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI,TEXT_POS_Y);								
	PA_SetSpritePrio(1,up_texta00000,2);
   up_textb00000 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_textb00000,up_hidetext01_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI,TEXT_POS_Y + 64);								
	PA_SetSpritePrio(1,up_textb00000,2);
   up_text0a0000 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text0a0000,up_hidetext10_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+32,TEXT_POS_Y);								
	PA_SetSpritePrio(1,up_text0a0000,2);
   up_text0b0000 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text0b0000,up_hidetext01_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+32,TEXT_POS_Y + 64);								
	PA_SetSpritePrio(1,up_text0b0000,2);
   up_text00a000 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text00a000,up_hidetext10_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+64,TEXT_POS_Y);								
	PA_SetSpritePrio(1,up_text00a000,2);
   up_text00b000 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text00b000,up_hidetext01_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+64,TEXT_POS_Y + 64);								
	PA_SetSpritePrio(1,up_text00b000,2);
   up_text000a00 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text000a00,up_hidetext10_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+96,TEXT_POS_Y);								
	PA_SetSpritePrio(1,up_text000a00,2);
   up_text000b00 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text000b00,up_hidetext01_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+96,TEXT_POS_Y + 64);								
	PA_SetSpritePrio(1,up_text000b00,2);
   up_text0000a0 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text0000a0,up_hidetext10_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+128,TEXT_POS_Y);								
	PA_SetSpritePrio(1,up_text0000a0,2);
   up_text0000b0 = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text0000b0,up_hidetext01_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+128,TEXT_POS_Y + 64);								
	PA_SetSpritePrio(1,up_text0000b0,2);
   up_text00000a = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text00000a,up_hidetext10_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+160,TEXT_POS_Y);								
	PA_SetSpritePrio(1,up_text00000a,2);
   up_text00000b = ct_graphics_getSprite(1);
	PA_CreateSpriteFromGfx(1,up_text00000b,up_hidetext01_gfx,OBJ_SIZE_32X64,1,up_textpal,
									TEXT_POS_XINI+160,TEXT_POS_Y + 64);								
	PA_SetSpritePrio(1,up_text00000b,2);
}

void ct_textscroll_finish() {
	PA_DeleteGfx(1,up_hidetext10_gfx);
	PA_DeleteGfx(1,up_hidetext01_gfx);
	PA_DeleteGfx(1,up_text10_gfx);
	PA_DeleteGfx(1,up_text01_gfx);
	PA_DeleteGfx(1,up_textscroll10_gfx);
	PA_DeleteGfx(1,up_textscroll01_gfx);
}

int ct_textscroll_openscroll(int pix) 
{
	if (PA_GetSpriteX(1,up_textscrolla) < TEXT_POS_XEND) {
		if ((PA_GetSpriteX(1,up_textscrolla) % 32) == 0) {
			switch (PA_GetSpriteX(1,up_textscrolla) / 32) {
				case 0:
					PA_SetSpriteGfx(1,up_texta00000,up_text10_gfx);
					PA_SetSpriteGfx(1,up_textb00000,up_text01_gfx);					
					break;
				case 1:
					PA_SetSpriteGfx(1,up_text0a0000,up_text10_gfx);
					PA_SetSpriteGfx(1,up_text0b0000,up_text01_gfx);					
					break;
				case 2:
					PA_SetSpriteGfx(1,up_text00a000,up_text10_gfx);
					PA_SetSpriteGfx(1,up_text00b000,up_text01_gfx);					
					break;
				case 3:
					PA_SetSpriteGfx(1,up_text000a00,up_text10_gfx);
					PA_SetSpriteGfx(1,up_text000b00,up_text01_gfx);					
					break;
				case 4:
					PA_SetSpriteGfx(1,up_text0000a0,up_text10_gfx);
					PA_SetSpriteGfx(1,up_text0000b0,up_text01_gfx);					
					break;
				case 5:
					PA_SetSpriteGfx(1,up_text00000a,up_text10_gfx);
					PA_SetSpriteGfx(1,up_text00000b,up_text01_gfx);					
					break;					
			}
		}	
		PA_SetSpriteX(1,up_textscrolla,PA_GetSpriteX(1,up_textscrolla)+pix);
		PA_SetSpriteX(1,up_textscrollb,PA_GetSpriteX(1,up_textscrollb)+pix);
		if (PA_GetSpriteX(1,up_textscrolla) > TEXT_POS_XEND) {
			PA_SetSpriteX(1,up_textscrolla,TEXT_POS_XEND);
			PA_SetSpriteX(1,up_textscrollb,TEXT_POS_XEND);			
		}
		return 1; // OK, scroll opened a bit
	} else{
		return 0; // Ups, could not open scroll - it is already opened :-)
	}
}

int ct_textscroll_closescroll(int pix) 
{
	if (PA_GetSpriteX(1,up_textscrolla) > TEXT_POS_XINI) {
		if ((PA_GetSpriteX(1,up_textscrolla) % 32) == 0) {
			switch (PA_GetSpriteX(1,up_textscrolla) / 32) {
				case 0:
					PA_SetSpriteGfx(1,up_texta00000,up_hidetext10_gfx);
					PA_SetSpriteGfx(1,up_textb00000,up_hidetext01_gfx);					
					break;
				case 1:
					PA_SetSpriteGfx(1,up_text0a0000,up_hidetext10_gfx);
					PA_SetSpriteGfx(1,up_text0b0000,up_hidetext01_gfx);					
					break;
				case 2:
					PA_SetSpriteGfx(1,up_text00a000,up_hidetext10_gfx);
					PA_SetSpriteGfx(1,up_text00b000,up_hidetext01_gfx);					
					break;
				case 3:
					PA_SetSpriteGfx(1,up_text000a00,up_hidetext10_gfx);
					PA_SetSpriteGfx(1,up_text000b00,up_hidetext01_gfx);					
					break;
				case 4:
					PA_SetSpriteGfx(1,up_text0000a0,up_hidetext10_gfx);
					PA_SetSpriteGfx(1,up_text0000b0,up_hidetext01_gfx);					
					break;
				case 5:
					PA_SetSpriteGfx(1,up_text00000a,up_hidetext10_gfx);
					PA_SetSpriteGfx(1,up_text00000b,up_hidetext01_gfx);					
					break;					
			}
		}	
		PA_SetSpriteX(1,up_textscrolla,PA_GetSpriteX(1,up_textscrolla)-2);
		PA_SetSpriteX(1,up_textscrollb,PA_GetSpriteX(1,up_textscrollb)-2);
		if (PA_GetSpriteX(1,up_textscrolla) < TEXT_POS_XINI) {
			PA_SetSpriteX(1,up_textscrolla,TEXT_POS_XINI);
			PA_SetSpriteX(1,up_textscrollb,TEXT_POS_XINI);			
		}		
		return 1; // OK, scroll closed a bit
	} else{
		return 0; // Ups, could not close scroll - it is already closed :-)
	}
}

void ct_textscroll_openscroll_full() 
{
	// Hides "hidden" scroll layers
	PA_SetSpriteGfx(1,up_texta00000,up_text10_gfx);
	PA_SetSpriteGfx(1,up_textb00000,up_text01_gfx);					
	PA_SetSpriteGfx(1,up_text0a0000,up_text10_gfx);
	PA_SetSpriteGfx(1,up_text0b0000,up_text01_gfx);					
	PA_SetSpriteGfx(1,up_text00a000,up_text10_gfx);
	PA_SetSpriteGfx(1,up_text00b000,up_text01_gfx);					
	PA_SetSpriteGfx(1,up_text000a00,up_text10_gfx);
	PA_SetSpriteGfx(1,up_text000b00,up_text01_gfx);					
	PA_SetSpriteGfx(1,up_text0000a0,up_text10_gfx);
	PA_SetSpriteGfx(1,up_text0000b0,up_text01_gfx);					
	PA_SetSpriteGfx(1,up_text00000a,up_text10_gfx);
	PA_SetSpriteGfx(1,up_text00000b,up_text01_gfx);					
	// Puts the scroll in the end
	PA_SetSpriteX(1,up_textscrolla,TEXT_POS_XEND);
	PA_SetSpriteX(1,up_textscrollb,TEXT_POS_XEND);
}

void ct_textscroll_closescroll_full() 
{
	// Activates "hidden" scroll layers
	PA_SetSpriteGfx(1,up_texta00000,up_hidetext10_gfx);
	PA_SetSpriteGfx(1,up_textb00000,up_hidetext01_gfx);					
	PA_SetSpriteGfx(1,up_text0a0000,up_hidetext10_gfx);
	PA_SetSpriteGfx(1,up_text0b0000,up_hidetext01_gfx);					
	PA_SetSpriteGfx(1,up_text00a000,up_hidetext10_gfx);
	PA_SetSpriteGfx(1,up_text00b000,up_hidetext01_gfx);					
	PA_SetSpriteGfx(1,up_text000a00,up_hidetext10_gfx);
	PA_SetSpriteGfx(1,up_text000b00,up_hidetext01_gfx);					
	PA_SetSpriteGfx(1,up_text0000a0,up_hidetext10_gfx);
	PA_SetSpriteGfx(1,up_text0000b0,up_hidetext01_gfx);					
	PA_SetSpriteGfx(1,up_text00000a,up_hidetext10_gfx);
	PA_SetSpriteGfx(1,up_text00000b,up_hidetext01_gfx);					
	// Puts the scroll in the start
	PA_SetSpriteX(1,up_textscrolla,TEXT_POS_XINI);
	PA_SetSpriteX(1,up_textscrollb,TEXT_POS_XINI);
}




