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


/* UtilLib - Tools useful for the overall game */
#ifndef _H_UTILLIB_
#define _H_UTILLIB_

#include <stdint.h> /* for the hash function */

/* Gets the size constant of PALib */
int ct_utillib_getPAsizeX(int x, int y);
int ct_utillib_getPAsizeY(int x, int y);

/* Checks if a character is a vowel */
int ct_utillib_isVowel(char let);

/* Checks if there is a game saved on SRAM */
int ct_utillib_checkInSRAM();

/* Resets the SRAM */
int ct_utillib_resetSRAM(int step);

/* Stores the game state on SRAM */
void ct_utillib_saveSRAM(int i);

/* Loads the game state from SRAM */
void ct_utillib_loadSRAM();

/* Inits basic game data configuration */
void ct_utillib_initData();

/* Inits subsystems */
void ct_utillib_init();

/* Send Value Val to Semaphore */
void ct_utillib_setSemaphoreVal(int val);

/* Send Value to Semaphore */
void ct_utillib_setSemaphore();

/* Signals Semaphore  */
void ct_utillib_signalSemaphore();

/* Gets Semaphore. Returns 1 if wait, 0 otherwise */
int ct_utillib_waitSemaphore();

/* Transforms a 4-letter digit into a int */
int ct_utillib_hex4int(char *hex);

/* Transforms a integer into a 4-letter */
char *ct_utillib_int4hex(int hex);

/* Gets the color associated to the lesson */
int ct_utillib_lessonColor(int lesson);

/* Return if a char is a letter or approved symbol - seems that a bug is floating around... */
int ct_utillib_isASymbol(char which);

/* gets the counter that updates every iteration */
int ct_utillib_getCounter();

/* To execute in every iteration */
int ct_utillib_iterate();

/* Check if a bit is 1 - 8 bit */	 
int ct_utillib_bitOne8(u8 num, u8 pos);

/* Check if a bit is 1 - 16 bit */	 
int ct_utillib_bitOne16(u16 num, u8 pos);

/* Puts a bit in 1 - 8 bit */	 
u8 ct_utillib_bitSet8(u8 num, u8 pos);

/* Puts a bit in 1 - 16 bit */	 
u16 ct_utillib_bitSet16(u16 num, u8 pos);

/* Deletes the CR/LF from a string */
void ct_utillib_del_newline(char *str);

/* Puts an string on uppercase */
void ct_utillib_str_toupper(char *str);

/* Puts an string on lowercase */
void ct_utillib_str_tolower(char *str);

/* Get the size of a file */
long ct_utillib_fsize(const char *const name);

/* Get a utf16 character (transformed) from a file */
int ct_utillib_getutf16file(FILE *f, char *utf16);

/* Get the Title of the program */
char *ct_utillib_getTitle();

/* Draws a Rectangle on the screen */ 
void ct_utillib_drawRectangle(int s, int x0, int y0, int x1, int y1, int c);

/* Returns the string with the number in chinese */ 
char *ct_utillib_getChineseNumber(int i);

/* Hash function by Paul Hsieh. (C) 2004 Paul Hsieh - http://www.azillionmonkeys.com/qed/ */
uint32_t Hsieh_SuperFastHash (const char * data, int len);

#endif
