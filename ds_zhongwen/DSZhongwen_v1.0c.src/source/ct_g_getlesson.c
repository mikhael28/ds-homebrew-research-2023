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

#include "gfx/all_gfx.h" // PAGfx Include

/* Constants */
///////////////

// No internal state

/* Variables - Internal State */
////////////////////////////////

int dw_getlesson_selectedLesson;
int dw_getlesson_idxlesson;
int dw_getlesson_lesson;

int dw_getlesson_button_back;
int dw_getlesson_button_hanziattack;
int dw_getlesson_button_ideogram;
int dw_getlesson_button_grammar;
int dw_getlesson_button_listen;
int dw_getlesson_button_nihao;
int dw_getlesson_button_lesson[12];
int dw_getlesson_button_lessonback;
int dw_getlesson_button_lessonforth;
int dw_getlesson_button_lessonnumber;

int dw_getlesson_button_lessondbg[4]; // For preloading purposes

/* Internal Functions */ 
////////////////////////

void g_getlesson_changeColor(int idbtn, int val){
   if (val == 0) {
      ct_button_changetype(idbtn,BTN_GREY);
   } else if (val <= 3) {
      ct_button_changetype(idbtn,BTN_RED);
   } else if (val <= 6) {
      ct_button_changetype(idbtn,BTN_YELLOW);
   } else if (val <= 10) {
      ct_button_changetype(idbtn,BTN_GREEN);
   }       
}   

void g_getlesson_updateState() {
   int i;
   char num[20];
   int max;
	   
	// First...Hides the buttons!
	//for (i = 0; i < 12; i++) {
	//	ct_button_hide(dw_getlesson_button_lesson[i]);
	//	ct_button_Text(dw_getlesson_button_lesson[i],"",2);
	//}
	// Second... Show buttons depending on the actual lesson,...
	max = ct_lesson_count(dw_getlesson_selectedLesson);
	for (i = 0; i < 12; i++) {
	   if ((i + dw_getlesson_idxlesson) < max) {
			ct_button_show(dw_getlesson_button_lesson[i]);
			sprintf(num,"%d",dw_getlesson_idxlesson + i + 1);
			ct_button_Text(dw_getlesson_button_lesson[i],num,2);
			if (ct_utillib_bitOne16(data_gameData[data_profile].lesson[dw_getlesson_selectedLesson][(dw_getlesson_idxlesson + i) / 16],
											(dw_getlesson_idxlesson + i) % 16)) {
				g_getlesson_changeColor(dw_getlesson_button_lesson[i],
												ct_lesson_getMedExams(dw_getlesson_selectedLesson,(dw_getlesson_idxlesson + i + 1)));
			}	else {
			   ct_button_changetype(dw_getlesson_button_lesson[i],BTN_FIELDSEL);
			}	   
		} else {
			ct_button_hide(dw_getlesson_button_lesson[i]);
			ct_button_Text(dw_getlesson_button_lesson[i],"",2);
		}   
	}	
	sprintf(num,"%d/%d",
				(max == 0)? 0 : dw_getlesson_idxlesson + 1,
				max);
	ct_button_Text(dw_getlesson_button_lessonnumber,num,3);
	
	// Third... Highlight actual lesson - TODO - problems if (data_lessontype !=  LESSON_) not there (lesson=0)
	if ((data_lessontype == dw_getlesson_selectedLesson) && (data_lessontype !=  LESSON_))
		if ((data_lessonnumber > dw_getlesson_idxlesson) && (data_lessonnumber <= dw_getlesson_idxlesson + 12)) {
		   sprintf(num,"%d",data_lessonnumber);
			ct_button_Text(dw_getlesson_button_lesson[(data_lessonnumber - 1) % 12],num,4);
		}			
}   

void g_getlesson_launchBack(){
	ct_state_assignState(GAME_MAINMENU);
}

void g_getlesson_ChangeLessonInner(int ltype, int lnumber) {
   data_lessontype = ltype;
   data_lessonnumber = lnumber;
	dw_getlesson_lesson = data_lessonnumber;
	ct_lesson_read(data_lessontype,data_lessonnumber);
	
	ct_smiley_setTextMood(SMILEYMOOD_TALKING,SMILEYMOOD_NORMAL);
	ct_smiley_setText(ct_lesson_strHeader(),-1,3,0);
   
	ct_header_changetype(1, data_lessontype);
	ct_header_changetype(0, data_lessontype);
   g_getlesson_updateState();
}   

void g_getlesson_ChangeLesson(){
   g_getlesson_ChangeLessonInner(
			dw_getlesson_selectedLesson,
   		ct_button_getVariable(ct_button_getHandler()) + dw_getlesson_idxlesson + 1
   		);
}

void g_getlesson_Normal(){
   // Do what the other do...
	dw_getlesson_selectedLesson = LESSON_;
	dw_getlesson_idxlesson = 0;
	ct_header_changetype(0, LESSON_);
	g_getlesson_updateState();
	// ...But also updates automatically to the real lesson
	g_getlesson_ChangeLesson(dw_getlesson_selectedLesson,-1);
}

void g_getlesson_Gramm(){
	dw_getlesson_selectedLesson = LESSON_GRAMM;
	dw_getlesson_idxlesson = 0;
	ct_header_changetype(0, LESSON_GRAMM);
	g_getlesson_updateState();
}

void g_getlesson_Hanzi(){
	dw_getlesson_selectedLesson = LESSON_HANZI;
	dw_getlesson_idxlesson = 0;
	ct_header_changetype(0, LESSON_HANZI);
	g_getlesson_updateState();
}

void g_getlesson_Listen(){
	dw_getlesson_selectedLesson = LESSON_LISTEN;
	dw_getlesson_idxlesson = 0;
	ct_header_changetype(0, LESSON_LISTEN);
	g_getlesson_updateState();
}

void g_getlesson_HanziAttack(){
	dw_getlesson_selectedLesson = LESSON_HANZIATTACK;
	dw_getlesson_idxlesson = 0;
	ct_header_changetype(0, LESSON_HANZIATTACK);
	g_getlesson_updateState();
}

void g_getlesson_backLesson() {
   if (dw_getlesson_idxlesson < 12)
   	dw_getlesson_idxlesson = 0;
   else dw_getlesson_idxlesson -= 12;
   
   g_getlesson_updateState();
}   

void g_getlesson_forthLesson() {
   if ((dw_getlesson_idxlesson + 12) < ct_lesson_count(dw_getlesson_selectedLesson))
		dw_getlesson_idxlesson += 12;
	
	g_getlesson_updateState();
}

/* Functions */ 
///////////////

void ct_g_getlesson_Start()
{
	int i;
	int j;
	int k;
	
	// Inits the Graphic mode for the getlesson
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
	ct_header_TextID(0,TXT_GETLESS_,data_gameData[data_profile].language);
	
	// Load basic buttons
	dw_getlesson_button_hanziattack = ct_button_add(ct_utillib_lessonColor(LESSON_HANZIATTACK),96,BTN_32,-8,32);
	ct_button_TextID(dw_getlesson_button_hanziattack,TXT_GETLESS_HANZIATTACK,data_gameData[data_profile].language,2);
	ct_button_addHandler_Click(dw_getlesson_button_hanziattack,g_getlesson_HanziAttack);
	dw_getlesson_button_ideogram = ct_button_add(ct_utillib_lessonColor(LESSON_HANZI),96,BTN_32,-8,32 + 32);
	ct_button_TextID(dw_getlesson_button_ideogram,TXT_GETLESS_IDEOGRAM,data_gameData[data_profile].language,2);
	ct_button_addHandler_Click(dw_getlesson_button_ideogram,g_getlesson_Hanzi);
	dw_getlesson_button_grammar = ct_button_add(ct_utillib_lessonColor(LESSON_GRAMM),96,BTN_32,-8,32 + 32 + 32);
	ct_button_TextID(dw_getlesson_button_grammar,TXT_GETLESS_GRAMMAR,data_gameData[data_profile].language,2);
	ct_button_addHandler_Click(dw_getlesson_button_grammar,g_getlesson_Gramm);
	dw_getlesson_button_listen = ct_button_add(ct_utillib_lessonColor(LESSON_LISTEN),96,BTN_32,-8,32 + 32 + 32 + 32);
	ct_button_TextID(dw_getlesson_button_listen,TXT_GETLESS_LISTEN,data_gameData[data_profile].language,2);
	ct_button_addHandler_Click(dw_getlesson_button_listen,g_getlesson_Listen);
	dw_getlesson_button_nihao = ct_button_add(ct_utillib_lessonColor(LESSON_),96,BTN_32,-8,32 + 32 + 32 + 32 + 32);
	ct_button_TextID(dw_getlesson_button_nihao,TXT_GETLESS_NIHAO,data_gameData[data_profile].language,2);
	ct_button_addHandler_Click(dw_getlesson_button_nihao,g_getlesson_Normal);
	
	// Load "Lesson" Buttons
	for (j = 0; j < 3; j++) {
	   for (i = 0; i < 4; i++) {
	      k = i + (j*4);
	      dw_getlesson_button_lesson[k] = ct_button_add(BTN_GREY,32,BTN_32,
													96 + ((i) * 40),
													36 + ((j) * 40));
			ct_button_addHandler_Click(dw_getlesson_button_lesson[k],g_getlesson_ChangeLesson);
			ct_button_setVariable(dw_getlesson_button_lesson[k],k);													
	   }   
	}
	dw_getlesson_button_lessonback = ct_button_add(BTN_GREY,32,BTN_32,96,156);
	ct_button_addHandler_Click(dw_getlesson_button_lessonback,g_getlesson_backLesson);
	ct_button_addContentIma(dw_getlesson_button_lessonback,(void *)arrow_l_Sprite,0,0,32,32);
	dw_getlesson_button_lessonforth = ct_button_add(BTN_GREY,32,BTN_32,216,156);
	ct_button_addHandler_Click(dw_getlesson_button_lessonforth,g_getlesson_forthLesson);	
	ct_button_addContentIma(dw_getlesson_button_lessonforth,(void *)arrow_r_Sprite,0,0,32,32);
	dw_getlesson_button_lessonnumber = ct_button_add(BTN_NONE,80,BTN_32,132,156);
	
	// Load coloured "lesson" buttons (for preloading sprites purposes - TODO)
	dw_getlesson_button_lessondbg[0] = ct_button_add(BTN_GREEN,32,BTN_32,250,190);
	ct_button_hide(dw_getlesson_button_lessondbg[0]);
	dw_getlesson_button_lessondbg[1] = ct_button_add(BTN_YELLOW,32,BTN_32,250,190);
	ct_button_hide(dw_getlesson_button_lessondbg[1]);	
	dw_getlesson_button_lessondbg[2] = ct_button_add(BTN_RED,32,BTN_32,250,190);
	ct_button_hide(dw_getlesson_button_lessondbg[2]);
	dw_getlesson_button_lessondbg[3] = ct_button_add(BTN_FIELDSEL,32,BTN_32,250,190);
	ct_button_hide(dw_getlesson_button_lessondbg[3]);	
	   				
	/* Variables*/
	ct_smiley_init();
	ct_smiley_setState(SMILEYMOOD_NORMAL);

	dw_getlesson_button_back = ct_button_add(BTN_GREY,32,BTN_32,0,0);
	ct_button_addHandler_Click(dw_getlesson_button_back,g_getlesson_launchBack);
	ct_button_addContentIma(dw_getlesson_button_back,(void *)arrow_b_Sprite,0,0,32,32);
	
	/* Init next state - Unique state */
	dw_getlesson_selectedLesson = data_lessontype;
	dw_getlesson_idxlesson = 0;
	dw_getlesson_lesson = data_lessonnumber;
	g_getlesson_updateState();
}

void ct_g_getlesson_Finish()
{
   PA_WaitForVBL();
	// Headers, buttons, textscroll
	ct_header_finish();
	ct_button_finish();
	ct_textscroll_finish();
	ct_smiley_finish();
} 


void ct_g_getlesson_Input()
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

void ct_g_getlesson_State()
{
	// Elements state
	ct_smiley_Manage();
	ct_button_Manage();
}
