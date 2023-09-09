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


/* Button - Stores buttons of height 32/64 and width multiple of 16. Possible to add Sprite as a content */
#ifndef _H_BUTTON_
#define _H_BUTTON_

/* Creates the Button Subsystem*/
void ct_button_init();

/* Adds a button of a certain type (color or special type) and size into the Subsytem, returns id */
int ct_button_add(int type, int sizex, int sizey, int x, int y);

/* Changes the type of the button */
void ct_button_changetype(int id,int type);

/* Adds an asociated Text content to the button */
void ct_button_addContentTxt(int id,void *content, int x, int y, int xs, int ys);

/* Adds an asociated Image content to the button */
void ct_button_addContentIma(int id,void *content, int x, int y, int xs, int ys);

/* Indicates that there is an image over the button*/
void ct_button_addContentPix(int id);

/* Changes the internal variable of the button */
void ct_button_setVariable(int id, int var);

/* Gets the internal variable of the button */
int ct_button_getVariable(int id);

/* Gets the Coordinate X of the button */
int ct_button_getX(int id);

/* Gets the Coordinate Y of the button */
int ct_button_getY(int id);

/* Gets the size X of the button */
int ct_button_getSizeX(int id);

/* Gets the size Y of the button */
int ct_button_getSizeY(int id);

/* Adds a text to the button */
void ct_button_Text(int id, char txt[255], int size);

/* Adds a text ID to the button */
void ct_button_TextID(int id, int txtid, int language, int size);

/* Adds a handler for Clicks */
void ct_button_addHandler_Click(int id,VoidFunctionPointer handler);

/* Deletes the handler for Clicks */
void ct_button_addHandler_(int id);

/* Gets the handler for the click */
int ct_button_getHandler();

/* Returns the id of a clicked button, -1 otherwise */
int ct_button_checkClick(int x, int y);

/* Runs the "OnClick" handler */
void ct_button_exeHandler_Click(int id);

/* Cleans the Pixels inside the Buttons */
void ct_button_cleanContent(int force);

/* Finish the Button Subsystem, destroying its inhabitants */
void ct_button_finish();

/* "Blips" the content of the button */
void ct_button_bh_blipContentStart(int id);

/* "Unblips" the content of the button */
void ct_button_bh_blipContentStop(int id);

/* Forces the "Blip" State */
void ct_button_bh_blip(int id, int time, int sem);

/* Forces the "Move to" State */
void ct_button_bh_move(int id, int x, int y, int speed, int sem);

/* Hides a Button */
void ct_button_hide(int id);

/* "Reloads" a Button */
void ct_button_show(int id);

/* Manages the internal state of the buttons */
void ct_button_Manage();

#endif
