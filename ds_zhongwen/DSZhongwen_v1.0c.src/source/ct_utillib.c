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
#include <sys/stat.h>
#include <ctype.h>
#include <stdint.h> /* for the hash function */

#include "ct_const.h"

#define DBG_SAVE 1

/* Variables */

int ini_sem;
char myres[5];
int counter;
int inner_nosave;
char res_str[255];

int utf16_BE = 1;

/* Functions */
///////////////


int ct_utillib_isVowel(char let) {
   if (
   		(let == 'a') || (let == 'A') || 
   		(let == 'e') || (let == 'E') || 
   		(let == 'i') || (let == 'I') || 
   		(let == 'o') || (let == 'O') || 
   		(let == 'u') || (let == 'U')
 		)
		return 1;
	else return 0;  	
}   

// Function ct_utillib_getPAsizeX(int x, int y)
int ct_utillib_getPAsizeX(int x, int y) {
	int alpha = (x / 8);
	x = alpha * 8; // Rounds x
	int beta = (y / 8);
	y = beta * 8; // Rounds y

	if ((x == 8) && (y == 8))
		return 0;
	if ((x == 16) && (y == 16))
		return 0;
	if ((x == 32) && (y == 32))
		return 0;		
	if ((x == 64) && (y == 64))
		return 0;		

	if ((x == 16) && (y == 8))
		return 1;
	if ((x == 32) && (y == 8))
		return 1;
	if ((x == 32) && (y == 16))
		return 1;
	if ((x == 64) && (y == 32))
		return 1;

	if ((x == 8) && (y == 16))
		return 1;
	if ((x == 8) && (y == 32))
		return 1;
	if ((x == 16) && (y == 32))
		return 1;
	if ((x == 32) && (y == 64))
		return 1;

	return 8; // otherwise		
} // end of ct_utillib_getPAsizeX(int x, int y)


// Function ct_utillib_getPAsizeY(int x, int y)
int ct_utillib_getPAsizeY(int x, int y) {
	int alpha = (x / 8);
	x = alpha * 8; // Rounds x
	int beta = (y / 8);
	y = beta * 8; // Rounds y

	if ((x == 8) && (y == 8))
		return 0;
	if ((x == 16) && (y == 16))
		return 1;
	if ((x == 32) && (y == 32))
		return 2;		
	if ((x == 64) && (y == 64))
		return 3;		

	if ((x == 16) && (y == 8))
		return 0;
	if ((x == 32) && (y == 8))
		return 1;
	if ((x == 32) && (y == 16))
		return 2;
	if ((x == 64) && (y == 32))
		return 3;

	if ((x == 8) && (y == 16))
		return 0;
	if ((x == 8) && (y == 32))
		return 1;
	if ((x == 16) && (y == 32))
		return 2;
	if ((x == 32) && (y == 64))
		return 3;

	return 0; // otherwise
} // end of ct_utillib_getPAsizeY(int x, int y)

/* Inits the game data */
int utillib_initProfileData(int step)
{
   int i;
   int j;
   int k;
   int l;
   int res = step + 1; // Actual/Next step
   
   for (i = 0; i < MAX_PROFILES; i++) {
      if (step == 0) {
         // Inits basic data
      	data_gameData[i].active = 0;
      	data_gameData[i].speed = CONF_SPEED;
      	data_gameData[i].hand = CONF_RIGHTHANDED;
      	if (PA_UserInfo.Language == 5)
      		data_gameData[i].language = LANG_SPA; // Spanish
      	else
      		data_gameData[i].language = LANG_ENG; // English
			PA_CopyText(data_gameData[i].name,PA_UserInfo.Name);
			for (j = 0; j < MAX_LESSONS; j++) {
			   for (k = 0; k < (MAX_LESSONS_NUMBER / 16); k++)
					data_gameData[i].lesson[j][k] = 0;
			}
			
			data_gameData[i].khanzi = 0;			
		}	
		else if ((step >= 1) && (step <= MAX_EXAMS)) {
		   l = step - 1;
			for (j = 0; j < MAX_LESSONS; j++)
			   for (k = 0; k < MAX_LESSONS_NUMBER; k++)
		   		data_gameData[i].lessongrades[j][k][l] = 0;
		}   
		else {
		   // Saves Hanzi array
		   l = step - MAX_EXAMS; 
			for (j = ((l - 1) * 20); j < (l * 20); j++) //(0..19, 20..39,...)
				if (j < (MAX_HANZI / 16))
					data_gameData[i].knownhanzi[j] = 0;
				else
					res = -1; // This is the end of the initialization process
		}		
	}   
	
	return res;
}

/* Stores the game data */
void utillib_savedata(int i)
{
   FILE *fsave;
   char dir[255];
   
   // This is going to take a little...
   if (!inner_nosave) {
      int i;
      int j;
      for (j = 64; j < 128; j++)
      	for (i = 32; i < 224; i++)
				PA_Put8bitPixel(0,i,j,2);
  		      PA_CenterSmartText(0,32 , 64, 
				   					224, 128, 
					   				"SAVING GAME... \n GUARDANDO EL JUEGO...", 
						   			1, 2, 1);									
   }   
   
#ifdef DBG_SAVE
	// File Access - open the save file   
	sprintf(dir,"%s%s.%d",DIR_MAIN,DIR_SAVE,i);
	fsave = fopen (dir, "wb");
	if (fsave == NULL)
		return;
	
	// Save
	fwrite(&data_gameData[i],sizeof(gData),1,fsave);
	
	// Close
	fclose(fsave);
#endif
}

/* Get the game data */
void utillib_loaddata()
{
   FILE *fsave;
   char dir[255];
   int i;
   
  	for (i = 0; i < MAX_PROFILES; i++) {
		// File Access - open the save file   
		sprintf(dir,"%s%s.%d",DIR_MAIN,DIR_SAVE,i);
		fsave = fopen (dir, "rb");
		if (fsave == NULL)
			return;
	
		// Load
		fread(&data_gameData[i],sizeof(gData),1,fsave);
	
		// Close
		fclose(fsave);
	}		
}

/* Checks if there is a game saved on SRAM */
int ct_utillib_checkInSRAM()
{
   FILE *fsave;
   char dir[255];
   
	// File Access - open the save file   
	sprintf(dir,"%s%s.%d",DIR_MAIN,DIR_SAVE,0);
	fsave = fopen (dir, "rb");
	if (fsave == NULL)
		return 0;
	fclose(fsave);
	return 1;
}

/* Stores the game state on SRAM */
void ct_utillib_saveSRAM(int i)
{
   // Saves game data - Initialization process is supposed to had happened
   utillib_savedata(i);
}

/* Loads the game state from SRAM */
void ct_utillib_loadSRAM()
{
	if (ct_utillib_checkInSRAM())
	{ 
	   utillib_loaddata();
	}   
}

/* Resets the SRAM */
int ct_utillib_resetSRAM(int step)
{  	
   int i;
  	// Init and (when initialization process is finished) saves raw game data
  	step = utillib_initProfileData(step);
  	if (step == -1) {
  	   inner_nosave = 1;
  	   for (i = 0; i < MAX_PROFILES; i++)
			ct_utillib_saveSRAM(i);
		inner_nosave = 0;
	}		
		
	return step;
}

/* Inits subsystems- Semaphore, Counter */
void ct_utillib_init()
{
   ini_sem = 0;
   counter = 0;
   
   inner_nosave = 0;
}   

/* Send Value to Semaphore  */
void ct_utillib_setSemaphore()
{
   ini_sem+=1;
}   

/* Send Value to Semaphore  */
void ct_utillib_setSemaphoreVal(int val)
{
   ini_sem = val;
}   

/* Signals Semaphore  */
void ct_utillib_signalSemaphore()
{
   ini_sem--;
}   

/* Gets Semaphore. Returns 1 if wait, 0 otherwise */
int ct_utillib_waitSemaphore()
{
   return ini_sem;
}   

int hex2int(unsigned char hex) {
	hex = hex - '0';
   if (hex > 9) {
       hex = (hex + '0' - 1) | 0x20;
       hex = hex - 'a' + 11;
   }
   if (hex > 15)
       hex = 0xFF;
       
   return hex;
}

unsigned char int2hex(int num) {
   
   unsigned char hex = num + '0';
   
	if (num > 9) {
	   switch (num) {
	      // Too bored for making the calculations ;-)
	      case 10: hex = 'a'; break;
	      case 11: hex = 'b'; break;
	      case 12: hex = 'c'; break;
	      case 13: hex = 'd'; break;
	      case 14: hex = 'e'; break;
	      case 15: hex = 'f'; break;
	   }   
	}	
   if (num > 15)
       hex = 'x';
       
   return hex;
}

/* Transforms a 4-letter digit into a int */
int ct_utillib_hex4int(char *hex) {
   int i;
   int res = 0;
   
   for (i = 0; i < 4; i++) {
      res += hex2int(hex[3 - i]) * (1 << (i * 4));
   }   
   return res;
}   

/* Transforms a integer into a 4-letter */
char *ct_utillib_int4hex(int hex) {
   int i;
   myres[4] = '\0';
   for (i = 0; i < 4; i++) {
   	myres[i] = int2hex((hex >> ((3 - i) * 4)) & 0x000F);
	}  
	
	return myres; 
}

/* Gets the color associated to the lesson */
int ct_utillib_lessonColor(int lesson) {
   switch (lesson) {
      case LESSON_: 
		   return BTN_RED;
      case LESSON_GRAMM: 
		   return BTN_GREEN;
      case LESSON_HANZI: 
		   return BTN_VIOLET;
      case LESSON_LISTEN: 
		   return BTN_BLUE;
      case LESSON_HANZIATTACK: 
		   return BTN_ORANGE;		   
   }  
	return BTN_GREY; 
}

/* Return if a char is a letter or approved symbol - seems that a bug is floating around... */
int ct_utillib_isASymbol(char which) {
   return (
				((which >= 32) && (which <= 254)) || (which == 10) // Text, Return
			);
}

/* gets the counter that updates every iteration */
int ct_utillib_getCounter() {
   return counter;
}

/* To execute in every iteration */
int ct_utillib_iterate() {
   counter++;
   return counter;
}

/* Check if a bit is 1 - 8 bit */	 
int ct_utillib_bitOne8(u8 num, u8 pos)
{
   return ((num >> pos) & 1);
}

/* Check if a bit is 1 - 16 bit */	 
int ct_utillib_bitOne16(u16 num, u8 pos)
{
   return ((num >> pos) & 1);
}

/* Puts a bit in 1 - 8 bit */	 
u8 ct_utillib_bitSet8(u8 num, u8 pos)
{
   return num | (1 << pos);
}

/* Puts a bit in 1 - 16 bit */	 
u16 ct_utillib_bitSet16(u16 num, u8 pos)
{
   return num | (1 << pos);
}

/* Deletes the CR/LF from a string */
void ct_utillib_del_newline(char *str) {
   char *p;
   
   if ((p = strchr(str, '\n')) != NULL) {
      *p = '\0';
	}   
   if ((p = strchr(str, '\r')) != NULL) {
      *p = '\0';
	}   
}

/* Puts an string on uppercase */
void ct_utillib_str_toupper(char *str)
{
   int i;
   for (i = 0; str[i] != 0; i++ )
   	str[i] = toupper( str[i] );
}

/* Puts an string on lowercase */
void ct_utillib_str_tolower(char *str)
{
   int i;
   for (i = 0; str[i] != 0; i++ )
   	str[i] = tolower( str[i] );
}      

/* Get the size of a file */
long ct_utillib_fsize(const char *const name)
{
	struct stat stbuf;
	
	if(stat(name, &stbuf) == -1)
		return -1; // The file could not be accessed.
	return stbuf.st_size;
}

/* Get a utf16 character (transformed) from a file */
int ct_utillib_getutf16file(FILE *f, char *utf16) {
  // Remember... UTF-16, Big Endian. No problem is there is a BOM.
  int cup;
  int cdown;
  
  // Get Unicode of this part
  cup = fgetc(f);
  if (feof(f))
    return -1; // EOF
  cdown = fgetc(f);
  
  // Copy the Unicode
  if (utf16_BE)
  		sprintf(utf16,"%s",ct_utillib_int4hex((cup << 8) + cdown));
  else
		sprintf(utf16,"%s",ct_utillib_int4hex((cdown << 8) + cup));
  ct_utillib_str_toupper(utf16);
  
  // Check special characters
  if (strcmp(utf16,"FF0C") == 0) // ","
    sprintf(utf16,"%s",",");
  else if (strcmp(utf16,"3002") == 0) // "."
    sprintf(utf16,"%s",".");
  else if (strcmp(utf16,"FF1F") == 0) // "?"
    sprintf(utf16,"%s","?");
  else if (strcmp(utf16,"FF01") == 0) // "!"
    sprintf(utf16,"%s","!");
  else if (strcmp(utf16,"0020") == 0) // " "
    sprintf(utf16,"%s"," ");    
  else if (strcmp(utf16,"000D") == 0) { // "CR-LF!!!"
    cup = fgetc(f);
    cdown = fgetc(f);
    return 1; // CR/LF
  } else if (strcmp(utf16,"FEFF") == 0) { // BOM!!! - Big Endian
    utf16_BE = 1;
    return 2; // BOM
  } else if (strcmp(utf16,"FFFE") == 0) { // BOM!!! - Little Endian
  	 utf16_BE = 0;
    return 2; // BOM
  }   
   
  return 0; // Character
}  

/* Get the Title of the program */
char *ct_utillib_getTitle() {
   char dir[255];
   char *res;
   FILE *file;
   
	sprintf(dir,"%s%s",DIR_MAIN,DIR_TITLE);
	file = fopen (dir, "rb");
   res = fgets(res_str, 254, file); 
   if (res == NULL) {
   	strcpy(res_str,"XXXX");
   	fclose(file);
   	return res_str;
	}   
	ct_utillib_del_newline(res_str);
	fclose(file);
	return res_str;
}

/* Returns the string with the number in chinese */ 
char *ct_utillib_getChineseNumber(int i) {
   // if i not in [0..10]... returns 1
 	strcpy(res_str,"4e00");
 	
 	switch (i) {
 	   case 1:
 	      strcpy(res_str,"4e00");break;
 	   case 2:
 	      strcpy(res_str,"4e8c");break;
 	   case 3:
 	      strcpy(res_str,"4e09");break;
 	   case 4:
 	      strcpy(res_str,"56db");break;
 	   case 5:
 	      strcpy(res_str,"4e94");break;
 	   case 6:
 	      strcpy(res_str,"516d");break;
 	   case 7:
 	      strcpy(res_str,"4e03");break;
 	   case 8:
 	      strcpy(res_str,"516b");break;
 	   case 9:
 	      strcpy(res_str,"4e5d");break;
 	   case 10:
 	      strcpy(res_str,"5341");break;
 	   case 0:
 	      strcpy(res_str,"96f6");break;
 	}   
 	
 	return res_str;
} 	

/* Draws a Rectangle on the screen */ 
void ct_utillib_drawRectangle(int s, int x0, int y0, int x1, int y1, int c) {
   int x;
   int y;
   
   for (x = x0; x <= x1; x++) {
     for (y = y0; y <= y1; y++) {
        PA_Put8bitPixel(s,x,y,c);
     }   
   }   
}   


/* Hash function by Paul Hsieh. (C) 2004 Paul Hsieh - http://www.azillionmonkeys.com/qed/ */

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

uint32_t Hsieh_SuperFastHash (const char * data, int len) {
uint32_t hash = len, tmp;
int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (uint16_t)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}  

/* Inits basic game data configuration */
void ct_utillib_initData()
{
   char title[255];
   data_lessonnumber = -1;
   data_lessontype = LESSON_;
   data_profile = -1;
   data_nummainmenu = 0;
   data_newuser = 0;
   data_ignorelid = 0;
   data_lidopen = 1;
   strcpy(title,ct_utillib_getTitle());
   data_hashtitle = Hsieh_SuperFastHash(title,strlen(title));
}

