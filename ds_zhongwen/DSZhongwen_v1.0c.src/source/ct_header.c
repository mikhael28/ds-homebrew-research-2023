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
#include "ct_linkedlist.h"

#include "gfx/all_gfx.h" // PAGfx Include

#define BANNER_POS_X 192 
#define BANNER_POS_Y 0

typedef struct {
	int id;
	int type;
	int screen;
	// Header
	int up_header1000;
	int up_header0100;
	int up_header0010;
	int up_header0001;
	u16 up_header_gfx;	
	// Banner
	int up_banner10;
	int up_banner01;
	u16 up_banner10_gfx;
	u16 up_banner01_gfx;	
	// Text
	char txt[255];
} aheader;


/* Variables - Internal State */
////////////////////////////////

LList *headerlist;
int headerfreeid = 0; // assign id to elements
int headerpal0;
int bannerpal0;
int headerpal1;
int bannerpal1;
	
/* Internal Functions */ 
////////////////////////

void *getSpriteHeader(int type)
{
	switch (type) { // no need of break statement, things of C :-P
		case LESSON_ :
			return (void*)up_headernihao_Sprite;
		case LESSON_HANZI :
			return (void*)up_headerhanzi_Sprite;
		case LESSON_GRAMM :
			return (void*)up_headergramm_Sprite;
		case LESSON_LISTEN :
			return (void*)up_headerlisten_Sprite;			
		case LESSON_HANZIATTACK :
			return (void*)up_headerhanziattack_Sprite;			
		break; // if case is not 0-3
	}
	return NULL; // -!- Please, fail so i can discover you ^_-
}

void *getSpriteBanner(int type, int who)
{
   if ((who < 0) || (who > 1))
   	return NULL; // -!- Please, fail so i can discover you ^_-
   	
	switch (type) { // no need of break statement, things of C :-P
		case LESSON_ :
		   if (who == 0)
				return (void*)up_bannernihao10_Sprite;
			else
				return (void*)up_bannernihao01_Sprite;
		case LESSON_HANZI :
		   if (who == 0)
				return (void*)up_bannerhanzi10_Sprite;
			else
				return (void*)up_bannerhanzi01_Sprite;
		case LESSON_GRAMM :
		   if (who == 0)
				return (void*)up_bannergramm10_Sprite;
			else
				return (void*)up_bannergramm01_Sprite;
		case LESSON_LISTEN :
		   if (who == 0)
				return (void*)up_bannerlisten10_Sprite;			
			else
				return (void*)up_bannerlisten01_Sprite;
		case LESSON_HANZIATTACK :
		   if (who == 0)
				return (void*)up_bannerhanziattack10_Sprite;
			else
				return (void*)up_bannerhanziattack01_Sprite;				
		break; // if case is not 0-3
	}
	return NULL; // -!- Please, fail so i can discover you ^_-
}

int getPalBanner(int type, int screen) 
{
   if (screen == 0)
   	return bannerpal0;
   else return bannerpal1;
}

int getPalHeader(int type, int screen) 
{
   if (screen == 0)
   	return headerpal0;
   else return headerpal1;
}

void header_delGfx(int screen,aheader *node)
{
   PA_DeleteGfx(screen,node->up_header_gfx);
  	if (screen == 1) {
		PA_DeleteGfx(screen,node->up_banner01_gfx);
		PA_DeleteGfx(screen,node->up_banner10_gfx);
	}	
}

void header_addGfx(int screen,aheader *node, int type)
{
   node->up_header_gfx = PA_CreateGfx(screen,getSpriteHeader(type), OBJ_SIZE_64X32, 1);
	if (screen == 1) {
	   node->up_banner10_gfx = PA_CreateGfx(screen,getSpriteBanner(type,0), OBJ_SIZE_64X64, 1);
	   node->up_banner01_gfx = PA_CreateGfx(screen,getSpriteBanner(type,1), OBJ_SIZE_64X32, 1);
	}   
}   

void fillNode(aheader *node, int type, int screen, int newheader)
{
   // Gets the Gfx of this system
   header_addGfx(screen,node,type);
   
   // Fills Header
   if (newheader) {
		node->up_header1000 = ct_graphics_getSprite(screen);
		PA_CreateSpriteFromGfx(screen,node->up_header1000,node->up_header_gfx,OBJ_SIZE_64X32,1,getPalHeader(type,screen),
										0,0);
	} else {
	   PA_SetSpriteGfx(screen,node->up_header1000,node->up_header_gfx);
	}   
   PA_SetSpritePrio(screen,node->up_header1000,3);		

   if (newheader) {		
   	node->up_header0100 = ct_graphics_getSprite(screen);
		PA_CreateSpriteFromGfx(screen,node->up_header0100,node->up_header_gfx,OBJ_SIZE_64X32,1,getPalHeader(type,screen),
									64,0);
	} else {	
	   PA_SetSpriteGfx(screen,node->up_header0100,node->up_header_gfx);
	}   
   PA_SetSpritePrio(screen,node->up_header0100,3);
   
   if (newheader) {
   	node->up_header0010 = ct_graphics_getSprite(screen);
		PA_CreateSpriteFromGfx(screen,node->up_header0010,node->up_header_gfx,OBJ_SIZE_64X32,1,getPalHeader(type,screen),
									128,0);
	} else {	
	   PA_SetSpriteGfx(screen,node->up_header0010,node->up_header_gfx);
	}   
   PA_SetSpritePrio(screen,node->up_header0010,3);
   
   if (newheader) {
   	node->up_header0001 = ct_graphics_getSprite(screen);
		PA_CreateSpriteFromGfx(screen,node->up_header0001,node->up_header_gfx,OBJ_SIZE_64X32,1,getPalHeader(type,screen),
									192,0);
	} else {	
	   PA_SetSpriteGfx(screen,node->up_header0001,node->up_header_gfx);   	
	}  
	PA_SetSpritePrio(screen,node->up_header0001,3); 
   
   // Fills Banner
	if (screen == 1) {		
	   if (newheader){
			node->up_banner10 = ct_graphics_getSprite(screen);	
			PA_CreateSpriteFromGfx(screen,node->up_banner10,node->up_banner10_gfx,OBJ_SIZE_64X64,1,getPalBanner(type,screen),
										BANNER_POS_X,BANNER_POS_Y);
		} else {
		   PA_SetSpriteGfx(screen,node->up_banner10,node->up_banner10_gfx); 
		}					
   	PA_SetSpritePrio(screen,node->up_banner10,2);		
					
		if (newheader) {
   		node->up_banner01 = ct_graphics_getSprite(screen);	
			PA_CreateSpriteFromGfx(screen,node->up_banner01,node->up_banner01_gfx,OBJ_SIZE_64X32,1,getPalBanner(type,screen),
										BANNER_POS_X,BANNER_POS_Y+64);
		} else {
		   PA_SetSpriteGfx(screen,node->up_banner01,node->up_banner01_gfx); 
		}					
   	PA_SetSpritePrio(screen,node->up_banner01,2);  
	}
	   
   // Fills Other things
   node->screen = screen;
   node->type = type;
   if (newheader)
   	node->id = headerfreeid++;
}

void header_rewrite(aheader *element)
{
   if (element->screen == 0) {
   	PA_CenterSmartText(0,
								0,0, 
								255, 24, 
								element->txt, 
								1, 3, 0);	
	} else {
	   // TODO - Text in Upper header
	}				
}

int header_compare(void *element, int screen)
{
	return ((((aheader *)element)->screen) == screen);
}

/* Functions */ 
///////////////

// Function ct_header_init()
void ct_header_init()
{
	headerlist = ct_linkedlist_init();
	
	headerpal0 = ct_graphics_getPalette(0);
	PA_LoadSpritePal(0,headerpal0,(void *)header_Pal);
	bannerpal0 = ct_graphics_getPalette(0);
	PA_LoadSpritePal(0,bannerpal0,(void *)banner_Pal);
	headerpal1 = ct_graphics_getPalette(1);
	PA_LoadSpritePal(1,headerpal1,(void *)header_Pal);
	bannerpal1 = ct_graphics_getPalette(1);
	PA_LoadSpritePal(1,bannerpal1,(void *)banner_Pal);
} // end of ct_header_init()

// Function ct_header_add(int type, int screen)
void ct_header_add(int screen, int type)
{
	aheader *header = malloc(sizeof(aheader));
	fillNode(header,type,screen,1);
	PA_CopyText(header->txt,"");
	ct_linkedlist_add(headerlist,header);
} // end of ct_header_add(int type, int screen)


void ct_header_changetype(int screen, int type)
{
	aheader *header = ct_linkedlist_get(headerlist,screen,header_compare);
	if (header == NULL)
		return;
		
	header_delGfx(screen,header);	
	header->type = type;
	fillNode(header,type,screen,0); // Gfx will be added inside
}

void ct_header_Text(int screen, char txt[255])
{
	aheader *header = ct_linkedlist_get(headerlist,screen,header_compare);
	if (header == NULL)
		return;
	
	PA_CopyText(header->txt,txt);
	header_rewrite(header);
}

void ct_header_TextID(int screen, int txtid, int language)
{
	aheader *header = ct_linkedlist_get(headerlist,screen,header_compare);
	if (header == NULL)
		return;
	
	PA_CopyText(header->txt,ct_const_getText(txtid,language));
	header_rewrite(header);
} 

   
// Function ct_header_finish()
void ct_header_finish()
{
	// Deletes all Gfxs and Sprites
	int pos = 0;
	aheader *header;
	while ((header = ct_linkedlist_getPos(headerlist,pos)) != NULL)
	{
		//PA_DeleteSprite(header->screen,header->up_header1000);
		//PA_DeleteSprite(header->screen,header->up_header0100);
		//PA_DeleteSprite(header->screen,header->up_header0010);
		//PA_DeleteSprite(header->screen,header->up_header0001);						
		//if (header->screen == 1){
			//PA_DeleteSprite(header->screen,header->up_banner01);
			//PA_DeleteSprite(header->screen,header->up_banner10);
		//}		
		header_delGfx(header->screen,header);
		pos++;
	}
	
	// Finishes the List
	ct_linkedlist_finish(headerlist);
	headerlist = NULL;
	headerfreeid = 0;
} // end of ct_header_finish()

