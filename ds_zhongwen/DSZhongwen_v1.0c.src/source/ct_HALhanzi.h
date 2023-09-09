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


/* HAL Layer - Access the hanzi graphics and its associated information 
   regardless of the HW layer below this library */
#ifndef _H_HALHANZI_
#define _H_HALHANZI_

/* Init the HAL layer */
void ct_HALhanzi_init();

/* Preload hanzi information inside the HAL layer */
void ct_HALhanzi_preloadHanzi(u16 hanzi);

/* Deletes Preload information */
void ct_HALhanzi_reset(u16 hanzi);

/* Gets a pointer to a certain sprite of a hanzi */
void* ct_HALhanzi_getHanzi(u16 hanzi);

/* Sets color of hanzi. -1 resets */
void ct_HALhanzi_setHanziColor(int c);

/* Paints a basic Hanzi onto the screen */
void ct_HALhanzi_paintHanzi16x16(int screen, u16 hanzi,int x, int y);

/* Paints a extended Hanzi onto the screen */
void ct_HALhanzi_paintHanzi32x32(int screen, u16 hanzi,int x, int y);

/* Paints an Extra-extended Hanzi onto the screen */
void ct_HALhanzi_paintHanzi64x64(int screen, u16 hanzi,int x, int y);

/* Gets a char * to the pinyin of a hanzi */
char* ct_HALhanzi_getPinyin(u16 hanzi);

/* Gets a char * to the meaning of a hanzi */
char* ct_HALhanzi_getMeaning(u16 hanzi);

/* Gets a certain hanzi number from a hanzi string */
u16 ct_HALhanzi_word_getHanzi(char *str, int which);

/* Gets how many hanzi are in the word */
u16 ct_HALhanzi_word_lenHanzi(char *str);

/* Gets how many hanzi are in the phrase */
u16 ct_HALhanzi_phrase_lenHanzi(char *str);

/* Gets how many words are in the phrase */
u16 ct_HALhanzi_phrase_lenWord(char *str);

/* Get the nth hanzi inside a phrase */
u16 ct_HALhanzi_phrase_getHanzi(char *str, int which);

/* Get the nth word inside a phrase */
char *ct_HALhanzi_phrase_getWord(char *str, int which);

/* Special: Hides hanzi i from the phrase. -1 resets */
void ct_HALhanzi_paintHide(int h);

/* Write a hanzi phrase, centered */
void ct_HALhanzi_paint(int screen,char *str, int y);
void ct_HALhanzi_paintX(int screen,char *str, int x, int y);

/* Returns a String containing the Pinyin of this phrase */
char *ct_HALhanzi_phrase_getStrPinyin(char *str);

/* Write a pinyin phrase, centered */
void ct_HALhanzi_paintPinyin(int screen,char *str, int y);
void ct_HALhanzi_paintPinyinX(int screen,char *str, int x, int y);

/* Set the size of the pinyin for the next phrase */
void ct_HALhanzi_setPinyinSize(int size);

#endif
