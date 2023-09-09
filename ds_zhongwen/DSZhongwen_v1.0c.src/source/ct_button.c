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
#include "ct_utillib.h"

#include "gfx/all_gfx.h" // PAGfx Include

typedef struct {
	// Basic button structure
	int id;
	int type;
	int sizex;
	int sizey;
	int x;
	int y;
	int buttonpart[BTN_CONST_MAXSIZE];
	// "Content" of the button
	int hascontent;
	u16 content_gfx;
	u16 content_pal;
	int content; // id of the content 
	int xcontent; // Coordinates of the content
	int ycontent;
	int xscontent;// Size of the content
	int yscontent;
	// Text of the button
	char txt[255];
	int txtsize;
	// Variable of the button
	int var;
	// Handlers of the button
	VoidFunctionPointer fClick;
	// Written something over the button
	int txtover;
	// Button Management
	int behavior;
	int hidden;
	int b_time;
	int b_x;
	int b_y;
	int b_speed;
	int b_sem;
} abutton;

typedef struct {
	u16 buttonini_gfx;
	u16 buttonmid_gfx;
	u16 buttonend_gfx;
	int nsprites;
} btngfx;

/* Variables - Internal State */
////////////////////////////////

// Button list
void *buttonlist;
int buttonpal; // all 32buttons must have the same palette 
int button_txtpal; // palette used by buttons with text
int button_imapal; // palette used by buttons with images
int buttonfreeid = 0; // assign id to elements
btngfx buttongfx64[BTN_MAXT + 1];
btngfx buttongfx32[BTN_MAXT + 1];

// Internal "callback" (excuse! dunno how to implement a f(int) ^_- )
int idcallback;

/* Internal Functions */ 
////////////////////////

void *button_getSprite32(int type, int id)
{
	switch (type) { // no need of break statement, things of C :-P
		case BTN_GREY :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32grey100_Sprite;
				case 1 : return (void*)btn32grey010_Sprite;				
				case 2 : return (void*)btn32grey001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_RED :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32red100_Sprite;
				case 1 : return (void*)btn32red010_Sprite;				
				case 2 : return (void*)btn32red001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_BLUE :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32blue100_Sprite;
				case 1 : return (void*)btn32blue010_Sprite;				
				case 2 : return (void*)btn32blue001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_GREEN :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32green100_Sprite;
				case 1 : return (void*)btn32green010_Sprite;				
				case 2 : return (void*)btn32green001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_VIOLET :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32violet100_Sprite;
				case 1 : return (void*)btn32violet010_Sprite;				
				case 2 : return (void*)btn32violet001_Sprite;				
			}
			break; // if case is not 0-2			
		case BTN_YELLOW :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32yellow100_Sprite;
				case 1 : return (void*)btn32yellow010_Sprite;				
				case 2 : return (void*)btn32yellow001_Sprite;				
			}
			break; // if case is not 0-2			
		case BTN_ORANGE :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32orange100_Sprite;
				case 1 : return (void*)btn32orange010_Sprite;				
				case 2 : return (void*)btn32orange001_Sprite;				
			}
			break; // if case is not 0-2						
		case BTN_BACK :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32grey100_Sprite;
				case 1 : return (void*)btn32grey010_Sprite;				
				case 2 : return (void*)btn32grey001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_READ :
				switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32grey100_Sprite;
				case 1 : return (void*)btn32grey010_Sprite;				
				case 2 : return (void*)btn32grey001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_CONFIG :
				switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32grey100_Sprite;
				case 1 : return (void*)btn32grey010_Sprite;				
				case 2 : return (void*)btn32grey001_Sprite;				
			}
			break; // if case is not 0-2			
		case BTN_FIELD :
				switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32field100_Sprite;
				case 1 : return (void*)btn32field010_Sprite;				
				case 2 : return (void*)btn32field001_Sprite;				
			}
			break; // if case is not 0-2						
		case BTN_FIELDSEL :
				switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32fieldsel100_Sprite;
				case 1 : return (void*)btn32fieldsel010_Sprite;				
				case 2 : return (void*)btn32fieldsel001_Sprite;				
			}
			break; // if case is not 0-2						
	}
	return NULL; // -!- Please, fail so i can discover you ^_-
}

void *button_getSprite64(int type, int id)
{
	switch (type) { // no need of break statement, things of C :-P
		case BTN_GREY :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn64grey100_Sprite;
				case 1 : return (void*)btn64grey010_Sprite;				
				case 2 : return (void*)btn64grey001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_RED :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn64red100_Sprite;
				case 1 : return (void*)btn64red010_Sprite;				
				case 2 : return (void*)btn64red001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_BLUE :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn32blue100_Sprite;
				case 1 : return (void*)btn32blue010_Sprite;				
				case 2 : return (void*)btn32blue001_Sprite;				
			}
			break; // if case is not 0-2
		case BTN_GREEN :
			switch (id) { // no need of break statement, things of C :-P
				case 0 : return (void*)btn64green100_Sprite;
				case 1 : return (void*)btn64green010_Sprite;				
				case 2 : return (void*)btn64green001_Sprite;				
			}
			break; // if case is not 0-2
	}
	return NULL; // -!- Please, fail so i can discover you ^_-
}

void button_delGfx(int type,int sizey)
{
	if (sizey == BTN_32) {
		buttongfx32[type].nsprites--;
		if (buttongfx32[type].nsprites == 0) {
			PA_DeleteGfx(0,buttongfx32[type].buttonini_gfx);
			PA_DeleteGfx(0,buttongfx32[type].buttonmid_gfx);
			PA_DeleteGfx(0,buttongfx32[type].buttonend_gfx);
		} 
	}
	if (sizey == BTN_64) {
		buttongfx64[type].nsprites--;
		if (buttongfx64[type].nsprites == 0) {
			PA_DeleteGfx(0,buttongfx64[type].buttonini_gfx);
			PA_DeleteGfx(0,buttongfx64[type].buttonmid_gfx);
			PA_DeleteGfx(0,buttongfx64[type].buttonend_gfx);
		} 
	}
}

void button_addGfx(int type, int sizey)
{
	switch(sizey) {
		case BTN_32:
			if (buttongfx32[type].nsprites == 0) {
				buttongfx32[type].buttonini_gfx = PA_CreateGfx(0,button_getSprite32(type,0), OBJ_SIZE_16X32, 1);
				buttongfx32[type].buttonmid_gfx = PA_CreateGfx(0,button_getSprite32(type,1), OBJ_SIZE_16X32, 1);
				buttongfx32[type].buttonend_gfx = PA_CreateGfx(0,button_getSprite32(type,2), OBJ_SIZE_16X32, 1);
			} 
			buttongfx32[type].nsprites++;
			break;
		case BTN_64:
			if (buttongfx64[type].nsprites == 0) {
				buttongfx64[type].buttonini_gfx = PA_CreateGfx(0,button_getSprite64(type,0), OBJ_SIZE_32X64, 1);
				buttongfx64[type].buttonmid_gfx = PA_CreateGfx(0,button_getSprite64(type,1), OBJ_SIZE_32X64, 1);
				buttongfx64[type].buttonend_gfx = PA_CreateGfx(0,button_getSprite64(type,2), OBJ_SIZE_32X64, 1);
			} 
			buttongfx64[type].nsprites++;
			break;			
	}
}

void button_reloadGfx(abutton *node, int newbutton)
{
	int i;
	int steps = node->sizex / 16;	
	for (i = 0; i < steps; i++) {
		if (newbutton)
			node->buttonpart[i] = ct_graphics_getSprite(0);
		if (i == 0) {
			if (newbutton)
				if (node->sizey == BTN_32)
					PA_CreateSpriteFromGfx(0,node->buttonpart[i],buttongfx32[node->type].buttonini_gfx,OBJ_SIZE_16X32,1,buttonpal,
											node->x + (i * 16), node->y);
				if (node->sizey == BTN_64)
					PA_CreateSpriteFromGfx(0,node->buttonpart[i],buttongfx64[node->type].buttonini_gfx,OBJ_SIZE_32X64,1,buttonpal,
											node->x + (i * 16), node->y);				
			else 
				if (node->sizey == BTN_32)
					PA_SetSpriteGfx(0,node->buttonpart[i],buttongfx32[node->type].buttonini_gfx);
				if (node->sizey == BTN_64)
					PA_SetSpriteGfx(0,node->buttonpart[i],buttongfx64[node->type].buttonini_gfx);				
		} else if (i == (steps - 1)) {
			if (newbutton)
				if (node->sizey == BTN_32)			
					PA_CreateSpriteFromGfx(0,node->buttonpart[i],buttongfx32[node->type].buttonend_gfx,OBJ_SIZE_16X32,1,buttonpal,
											node->x + (i * 16), node->y);
				if (node->sizey == BTN_64)
					PA_CreateSpriteFromGfx(0,node->buttonpart[i],buttongfx64[node->type].buttonend_gfx,OBJ_SIZE_32X64,1,buttonpal,
											node->x + (i * 16), node->y);				
			else
				if (node->sizey == BTN_32)			
					PA_SetSpriteGfx(0,node->buttonpart[i],buttongfx32[node->type].buttonend_gfx);
				if (node->sizey == BTN_64)			
					PA_SetSpriteGfx(0,node->buttonpart[i],buttongfx64[node->type].buttonend_gfx);					
		} else {
			if (newbutton)
				if (node->sizey == BTN_32)
					PA_CreateSpriteFromGfx(0,node->buttonpart[i],buttongfx32[node->type].buttonmid_gfx,OBJ_SIZE_16X32,1,buttonpal,
											node->x + (i * 16), node->y);
				if (node->sizey == BTN_64)
					PA_CreateSpriteFromGfx(0,node->buttonpart[i],buttongfx64[node->type].buttonmid_gfx,OBJ_SIZE_32X64,1,buttonpal,
											node->x + (i * 16), node->y);				
			else
				if (node->sizey == BTN_32)
					PA_SetSpriteGfx(0,node->buttonpart[i],buttongfx32[node->type].buttonmid_gfx);
				if (node->sizey == BTN_64)
					PA_SetSpriteGfx(0,node->buttonpart[i],buttongfx64[node->type].buttonmid_gfx);
		}
		//if (newbutton) - Nop! Priorities must be reloaded
			PA_SetSpritePrio(0,node->buttonpart[i],2);
	}
}

void button_fillContent(abutton *node, void *element, int x, int y, int xs, int ys, int pal)
{
	// TODO: Check if there is content before
	node->hascontent = 1;
	node->xcontent = x; 
	node->ycontent = y;
	node->xscontent = xs;
	node->yscontent = ys;

	node->content_gfx = PA_CreateGfx(0,element, 
										ct_utillib_getPAsizeX(node->xscontent,node->yscontent),
										ct_utillib_getPAsizeY(node->xscontent,node->yscontent), 
										1); 
	node->content = ct_graphics_getSprite(0);
	PA_CreateSpriteFromGfx(0,node->content,node->content_gfx,
										ct_utillib_getPAsizeX(node->xscontent,node->yscontent),
										ct_utillib_getPAsizeY(node->xscontent,node->yscontent),
										1,pal,
										node->xcontent + node->x,
										node->ycontent + node->y);
	PA_SetSpritePrio(0,node->content,1);
}

void button_fillNode(abutton *node, int type, int sizex, int sizey, int x, int y)
{
	// First, Fills basic button fields 
   node->sizex = (sizex / 16) * 16;
   node->sizey = sizey;
   node->type = type;
   node->id = buttonfreeid++;
   node->x = x;
   node->y = y;
   PA_CopyText(node->txt,"");
   node->txtsize = 3;
		
	// Create basic GFX
	if (node->type != BTN_NONE) {
	   button_addGfx(type, sizey);
	}
				
	// Then, fill sprites with this GFX
	if (node->type != BTN_NONE) {
	   button_reloadGfx(node,1);
	}	
   
   // Node content
   node->hascontent = 0;
   
   // Node handlers
   node->fClick = NULL;
   
   node->txtover = 0;
   
   // Node Behavior
	node->behavior = BTN_BH_;
	node->hidden = 0;
	node->b_time = 0;
	node->b_speed = 0;
	node->b_x = node->x;
	node->b_y = node->y;
	node->b_sem = 0;
}

void button_rewrite(abutton *element, int col)
{
   PA_CenterSmartText(0,element->x , element->y, 
								element->x + element->sizex, element->y + element->sizey, 
								element->txt, col, element->txtsize, 0);	
}

int button_compare(void *element, int eleid)
{
	return ((((abutton *)element)->id) == eleid);
}

void button_hideContent(abutton *button, int hide)
{
	if (button->hascontent) {
		if (hide) {
	   	PA_SetSpriteX(0,button->content,300);
	      PA_SetSpriteY(0,button->content,0);		         
   	} else {
      	PA_SetSpriteX(0,button->content,button->xcontent + button->x);
         PA_SetSpriteY(0,button->content,button->ycontent + button->y);
		}			   
	}   
}

void button_hide(abutton *button, int hide)
{
   int steps;
   int i;
   
   // Button...only if there is an sprite
   if (button->type != BTN_NONE) {
		if (hide) {
			steps = button->sizex / 16;	
			for (i = 0; i < steps; i++) {
				PA_SetSpriteX(0,button->buttonpart[i],300 + (i * 16));
				PA_SetSpriteY(0,button->buttonpart[i],0);
			}		         
		} else {
			steps = button->sizex / 16;	
			for (i = 0; i < steps; i++) {
				PA_SetSpriteX(0,button->buttonpart[i],button->x + (i * 16));
				PA_SetSpriteY(0,button->buttonpart[i],button->y);
			}		         
		}
	}		
	
	// Content
	button_hideContent(button,hide);
	
	// Text
	button_rewrite(button,(hide)?0:1);
}

/* Functions */ 
///////////////

// Function ct_button_init()
void ct_button_init()
{
	int i;
	buttonlist = ct_linkedlist_init();
	buttonpal = ct_graphics_getPalette(0);
	PA_LoadSpritePal(0,buttonpal,(void*)btn_Pal);
	button_txtpal = ct_graphics_getPalette(0);
	PA_LoadSpritePal(0,button_txtpal,(void*)btntxt_Pal);	
	button_imapal = ct_graphics_getPalette(0);
	PA_LoadSpritePal(0,button_imapal,(void*)btnima_Pal);	
	for (i = 0; i<=BTN_MAXT; i++) {
		buttongfx32[i].buttonini_gfx = 65535;
		buttongfx32[i].buttonmid_gfx = 65535;
		buttongfx32[i].buttonend_gfx = 65535;
		buttongfx32[i].nsprites = 0;
		
		buttongfx64[i].buttonini_gfx = 65535;
		buttongfx64[i].buttonmid_gfx = 65535;
		buttongfx64[i].buttonend_gfx = 65535;
		buttongfx64[i].nsprites = 0;		
	}
} // end of ct_button_init()

// Function ct_button_add(int type, int sizex, int sizey, int x, int y)
int ct_button_add(int type, int sizex, int sizey, int x, int y)
{
   // Safety
	if (sizex < BTN_CONST_MINSIZE)
		sizex = BTN_CONST_MINSIZE;
	if (sizex > BTN_CONST_MAXSIZE)
		sizex = BTN_CONST_MAXSIZE;
	if ((sizey != BTN_32) && (sizey != BTN_64))
		sizey = BTN_32;
	if ((type > BTN_MAXT) && (type != BTN_NONE))
		type = BTN_RED;
	if ((type == BTN_BACK) || (type == BTN_READ) || (type == BTN_CONFIG)) {
		sizex = 32;
		sizey = BTN_32;
	}
	
	// Create	
	abutton *button = malloc(sizeof(abutton));
	if (button == NULL) {
	   return -1;
	   // ++CHANGE++ The null button must die! :-P
	   //PA_OutputSimpleText(1,0,0,"NULLLLLLLL!!!!!"); // Alarm :-)
	}   
	button_fillNode(button,type,sizex,sizey,x,y);
	ct_linkedlist_addEnd(buttonlist,button);
	
	return button->id;
} // end of ct_button_add(int type, int size, int x, int y)

// Function ct_button_changetype(int id,int type)
void ct_button_changetype(int id,int type)
{
	if ((type == BTN_BACK) || (type == BTN_READ) || (type == BTN_CONFIG) || (type == BTN_NONE))
		return;
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;
		
	if (type == button->type)		
		return;
	button_delGfx(button->type,button->sizey);
	button->type = type;
	button_addGfx(button->type,button->sizey);
	button_reloadGfx(button,0);
} // end of ct_button_changetype(int id,int type)

// Function ct_button_addContentTxt(int id,void *content, int x, int y, int xs, int ys)
void ct_button_addContentTxt(int id,void *content, int x, int y, int xs, int ys)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL){
	   PA_OutputText(1,0,0,"%d!!!!!",id);
		return;
	}		
		
	button_fillContent(button, content, x, y, xs, ys, button_txtpal);
} // end of ct_button_addContentTxt(int id,void *content, int x, int y, int xs, int ys)

// Function ct_button_addContentPix(int id)
void ct_button_addContentPix(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL){
	   PA_OutputText(1,0,0,"%d!!!!!",id);
		return;
	}		

	button->txtover = 1;		
} // end of ct_button_addContentPix(int id)

// Function ct_button_addContentIma(int id,void *content, int x, int y, int xs, int ys)
void ct_button_addContentIma(int id,void *content, int x, int y, int xs, int ys)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL) {
	   PA_OutputText(1,0,1,"%d!!!!!",id);
		return;
	}		
		
	button_fillContent(button, content, x, y, xs, ys, button_imapal);
} // end of ct_button_addContentIma(int id,void *content, int x, int y, int xs, int ys)

void ct_button_setVariable(int id, int var)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL){
	   PA_OutputText(1,0,2,"%d!!!!!",id);
		return;
	}		
	button->var = var;
}

int ct_button_getVariable(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return -1;

	return button->var;
}  

/* Gets the Coordinate X of the button */
int ct_button_getX(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return -1;

	return button->x;
}  

/* Gets the Coordinate Y of the button */
int ct_button_getY(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return -1;

	return button->y;
}

/* Gets the size X of the button */
int ct_button_getSizeX(int id) 
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return -1;

	return button->sizex;
}  

/* Gets the size Y of the button */
int ct_button_getSizeY(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return -1;

	return button->sizey;
}  

// Function ct_button_Text(int id,void *content, int x, char txt[255])
void ct_button_Text(int id, char txt[255], int size)
{
   // TODO Check Range size
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	button_rewrite(button,0);
		
	button->txtsize = size;
	PA_CopyText(button->txt,txt);
	button_rewrite(button,1);
} // end of ct_button_Text(int id,void *content, int x, char txt[255])

void ct_button_TextID(int id, int txtid, int language, int size)
{
   // TODO Check Range size
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	button_rewrite(button,0);	
	
	button->txtsize = size;
	PA_CopyText(button->txt,ct_const_getText(txtid,language));
	button_rewrite(button,1);
} 

// Function ct_button_addHandler_Click(int id,VoidFunctionPointer handler)
void ct_button_addHandler_Click(int id,VoidFunctionPointer handler)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL){
	   PA_OutputText(1,0,3,"%d!!!!!",id);
		return;
	}		
 
	button->fClick = handler;
} // end of ct_button_addHandler_Click(int id,VoidFunctionPointer handler) 

// Function ct_button_addHandler_(int id)
void ct_button_addHandler_(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;
 
	button->fClick = NULL;
} // end of ct_button_addHandler_(int id) 

// Function ct_button_exeHandler_Click(int id)
void ct_button_exeHandler_Click(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	idcallback = id;
	if (button->fClick != NULL)
		button->fClick();
} // end of ct_button_exeHandler_Click(int id) 

// Function ct_button_getHandler()
int ct_button_getHandler()
{
   return idcallback;
} // end of ct_button_getHandler()


// Function ct_button_checkClick(int x, int y)
int ct_button_checkClick(int x, int y)
{
	int pos = 0;
	abutton *button;
	void *dev;
	while ((dev = ct_linkedlist_getPos(buttonlist,pos)) != NULL)
	{
		button = (abutton *)dev;
		//PA_OutputText(1,0,pos,"(%d)%d - x:%d,y:%d",pos,button->id,button->x,button->y);
		if (!button->hidden) {
			if ((x >= button->x) && (x <= (button->x + button->sizex)) &&
				 (y >= button->y) && (y <= (button->y + button->sizey)))
				return button->id; 
		}
		pos++;
	}
	return -1;
} // end of ct_button_checkClick(int x, int y)

// Function void ct_button_cleanContent()
void ct_button_cleanContent(int force)
{
   int i;
   int j;
	int pos = 0;
	abutton *button;
	void *dev;
	while ((dev = ct_linkedlist_getPos(buttonlist,pos)) != NULL)
	{
		button = (abutton *)dev;
		//PA_OutputText(1,0,pos,"(%d)%d - x:%d,y:%d",pos,button->id,button->x,button->y);
		if ((!button->hidden) && ((button->txtover) || force)) {
		   button->txtover = 0;
		   for (i = button->x; i < (button->x + button->sizex); i++) {
		      for (j = button->y; j < (button->y + button->sizey); j++) {
		         PA_Put8bitPixel(0,i,j,0);
		      }   
		   }   
		}
		pos++;
	}
} // end of ct_button_cleanContent()

// Function ct_button_finish()
void ct_button_finish()
{
	// Deletes all Gfxs and Sprites
	int pos = 0;
	abutton *button;
	while ((button = ct_linkedlist_getPos(buttonlist,pos)) != NULL)
	{
	   if (button->type != BTN_NONE) {
			button_delGfx(button->type,button->sizey);
			//for (i = 0; i < (button->sizex / 16); i++)
			//	PA_DeleteSprite(0,button->buttonpart[i]);
		}	
		pos++;	
	}
	
	// Finishes the List
	ct_linkedlist_finish(buttonlist);
	buttonfreeid = 0;
} // end of ct_button_finish()

void ct_button_bh_blip(int id, int time, int sem)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	button->behavior = BTN_BH_BLIP;
	button->b_time = time;
	button->b_sem = sem;
	if (button->b_sem == 1)
		ct_utillib_setSemaphore(); 
}

void ct_button_bh_blipContentStart(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	if (button->behavior != BTN_BH_BLIPCTN) {
		button->behavior = BTN_BH_BLIPCTN;
		button->b_time = 99;
		button->b_sem = 0;
	}	
}

void ct_button_bh_blipContentStop(int id)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	if (button->behavior == BTN_BH_BLIPCTN)
		button->b_time = 0;
}

void ct_button_bh_move(int id, int x, int y, int speed, int sem)
{
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	button->behavior = BTN_BH_MOVE;
	button->b_time = 1;
	if (x == -255)
		button->b_x = button->x;
	else button->b_x = x;
	if (y == -255)
		button->b_y = button->y;
	else button->b_y = y;
	button->b_speed = speed;
	button->b_sem = sem;
	button_rewrite(button,0);
	if (button->b_sem == 1)
		ct_utillib_setSemaphore(); 
}

/* Hides a Button */
void ct_button_hide(int id) 
{   
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	if (!button->hidden) {
		button_hide(button,1);
		button->hidden = 1;
	}		
}   

/* "Reloads" a Button */
void ct_button_show(int id)
{  
	abutton *button = ct_linkedlist_get(buttonlist,id,button_compare);
	if (button == NULL)
		return;

	if (button->hidden) {
		button_hide(button,0);
		button->hidden = 0;
	}		
}   
   

void ct_button_Manage()
{
   int i;
   int steps;
	int pos = 0;
	int stepx;
	int stepy;
	void *dev;
	abutton *button;
	while ((dev = ct_linkedlist_getPos(buttonlist,pos)) != NULL)
	{
	   // Get
		button = (abutton *)dev;
		
		// Manage
		switch (button->behavior) {
		   case BTN_BH_BLIP :
		      if (((button->b_time / 10) & 1) == 1) {
		         button_hide(button,1); // Hide
		      } else {
		         button_hide(button,0); // Show - BTW, can save the if here, but want to have readability :-P
				}
				button->b_time--;			   
		      break;
		   case BTN_BH_BLIPCTN:
			   if (((button->b_time / 10) & 1) == 1) {
			      button_hideContent(button,1); // Hide
		   	} else {
		   	   button_hideContent(button,0); // Show - BTW, can save the if here, but want to have readability :-P
				}
				button->b_time--;			   
				if (button->b_time == 0) // Normal Case - Cycle
					button->b_time = 99;  // Cycle
				if (button->b_time == -1) // Special Case - End of everything
					button->b_time = 0;   // Ends
		      break;		      
		   case BTN_BH_MOVE :
		      // Move where + Safety check
		      if (button->x < button->b_x)
		      	stepx = ((button->x+button->b_speed) <= button->b_x)?button->b_speed:(button->b_x-button->x);
		      else if (button->x > button->b_x)
					stepx = ((button->x-button->b_speed) >= button->b_x)?(-button->b_speed):(button->x-button->b_x);
				else stepx = 0;
		      if (button->y < button->b_y)
		      	stepy = ((button->y+button->b_speed) <= button->b_y)?button->b_speed:(button->b_y-button->y);
		      else if (button->y > button->b_y)
					stepy = ((button->y-button->b_speed) >= button->b_y)?(-button->b_speed):(button->y-button->b_y);
				else stepy = 0;								
				// Move 
				if ((stepx != 0) || (stepy != 0)) {
				   button->x += stepx;
				   button->y += stepy;
					steps = button->sizex / 16;	
					for (i = 0; i < steps; i++) {
						PA_SetSpriteX(0,button->buttonpart[i],button->x + (i * 16));
						PA_SetSpriteY(0,button->buttonpart[i],button->y);
					}		         
				} else {
					button->b_time = 0;   
					// TODO: Write if inside the system
				}			
		      break;
		}   
		if (button->behavior != BTN_BH_) {
		   if (button->b_time == 0) {
		      // No behavior
		      button->behavior = BTN_BH_;
		      // Signals semaphore b_sem
		      if (button->b_sem == 1) {
		         ct_utillib_signalSemaphore();
		         button->b_sem = 0;
		   	}   
		   }   
		}   
		// Next
		pos++;
	}
   
}   
