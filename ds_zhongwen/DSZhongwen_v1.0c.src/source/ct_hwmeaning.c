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
#include <ctype.h>
#include <string.h>

#include "ct_const.h"
#include "ct_linkedlist.h"
#include "ct_utillib.h"
#include "ct_l_hw.h"


#include "gfx/all_gfx.h" // PAGfx Include

#ifdef DLDI

// Variables
FILE *lmeaning_db;
char act_pinyin[80];
char act_meaning[255];

//----------------------------------------DLDI Section------------------------------------------

int hwmeaning_getTextCont(char *meaning, char *pinyin, char *pinyin4, int number)
{
   char *res;
   char tmp[255];
   char tmpstr1[10];
   
   // Get
   res = fgets(tmp, 254, lmeaning_db); 
   if (res == NULL) {
   	sprintf(meaning,"XXXX");
   	return 0;
	}   
	
	ct_utillib_del_newline(tmp);
	
	// Format
	sscanf(tmp,"%s %[^\x7F]",pinyin,meaning);
	
	// Check
   strncpy(tmpstr1,pinyin,4);
   tmpstr1[4] = '\0';
   ct_utillib_str_toupper(tmpstr1);	
   
	if (strcmp(tmpstr1,pinyin4) != 0)
		return 0; // Not our pinyin anymore

	return 1;
}

void hwmeaning_addCRLF(char *meaning) {
   int i;
   int count;
   int lastspace;
   
   count = 0;
   lastspace = 0;
   i = 0;
   while (i < strlen(meaning)) {
      // Count this letter...
      count++;
      // More than 40 words? Change sentence
      if (count > 40) {
         // Insert a \n in the last space
         meaning[lastspace] = '\n';
			// Continue
			count = count - lastspace;
      }   
      // Space
      if (meaning[i] == ' ')
      	lastspace = i;
      // Next
      i++;
   }   
}   

/* Init the HW layer */
void ct_hwmeaning_init()
{
   // Nothing to init here
}

int hwmeaning_LoadOffset(char *pinyin, int *offset, int lang) {
   char dir[255];
   char act_pinyin[20];
   char param_pinyin[20];
   char tmpstr1[10];
   char *res;
   FILE *fidx;
   int end;
   int tam;
	   
   *offset = 0;
   tam = 0;
   
   strncpy(param_pinyin,pinyin,4);
   param_pinyin[4] = '\0';
   ct_utillib_str_toupper(param_pinyin);
   
   strncpy(tmpstr1,pinyin,2);
   tmpstr1[2] = '\0';
   ct_utillib_str_toupper(tmpstr1);

   
   // Open idx file
	sprintf(dir,"%s%s%s/%s%s",DIR_MAIN,DIR_TRANS,
				(lang == LANG_ENG)?"/eng":"/spa",
				tmpstr1,DIR_IDX);
	fidx = fopen(dir,"rb");
	if (fidx != NULL) {
	   // Check all the entries
	   end = 0;
	   while (!end) {
	   	res = fgets(dir,254,fidx);
	   	if (res == NULL) {
	   	   // No more lines...
	      	end = 1;
	      	*offset = 0;
	      	tam = 0;
		   } else {
		      // Let's compare the "pinyin"...
		      sscanf(dir,"%s %d",act_pinyin,offset);
		      ct_utillib_str_toupper(act_pinyin);
		      if (strcmp(act_pinyin,param_pinyin) == 0) {
		         // Found!!!!!!
		         end = 1;
		         tam = 1;
		      }   
		   }   
		}   
	}   
	
	if (tam == 0)
		return 0;
	else return 1;
}      

/* Gets a string with the meaning of a certain pinyin phrase */
char* ct_hwmeaning_getHanzi(char *pinyin, int lang)
{
   char dir[255];
   char tmpstr1[10];
   char tmpstr2[10];
   int i;
   int offset;

	// Pre-details...
   ct_utillib_str_toupper(pinyin);   

	// First, get the associated idx and offset
   if (!hwmeaning_LoadOffset(pinyin,&offset,lang))
   	return "";
   
   // Second, try to open the file, and move the offset
   strncpy(tmpstr1,pinyin,2);
   tmpstr1[2] = '\0';
   ct_utillib_str_toupper(tmpstr1);
   strncpy(tmpstr2,pinyin,4);
   tmpstr2[4] = '\0';
   ct_utillib_str_toupper(tmpstr2);   
	sprintf(dir,"%s%s%s/%s%s",DIR_MAIN,DIR_TRANS,
				(lang == LANG_ENG)?"/eng":"/spa",
				tmpstr1,DIR_EXT);
	lmeaning_db = fopen (dir, "rb");
   
   if (lmeaning_db == NULL)
   	return "";
   fseek(lmeaning_db,offset,SEEK_SET);	
   	
   // Now, try to get the exact meaning
   i = 0;
   while (hwmeaning_getTextCont(act_meaning,act_pinyin,tmpstr2,i)) {
      // Compare meanings...
      if (strcmp(act_pinyin,pinyin) == 0) {
         // This one!!!! - Good, give it to the user
         fclose(lmeaning_db);
         // But... first, add \n where it is necessary!!!!
         hwmeaning_addCRLF(act_meaning);
         // Good! Return it now :-D
         return act_meaning;
      }   
      // Next one...
      i++;
   }   
   // No meaning?... :-(
   return "";
}

#else

//----------------------------------------non-DLDI Section--------------------------------------

/* Init the HW layer */
void ct_hwmeaning_init()
{
}   

/* Gets a string with the meaning of a certain pinyin phrase */
char* ct_hwmeaning_getHanzi(char *pinyin)
{
   return "";
}

#endif   

