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


/* Constant file with constants used in the game // Global variables of the system */
#ifndef _H_CONST_
#define _H_CONST_

/* Progran DEFINEs */
#define DLDI

/* Constants */
//-------------

/* Version */
#define DSZHONGWEN_VERSION "v1.0c"

/* Directories */
#define DIR_MAIN "/data/DSZhongwen"
#define DIR_SAVE "/DSZhongwen.sav"
#define DIR_TITLE "/title.txt"
#define DIR_LHANZI "/lhanzi"
#define DIR_LHANZIATTACK "/lhanziattack"
#define DIR_LGRAM "/lgram"
#define DIR_LLISTEN "/llisten"
#define DIR_TRANS "/trans"
#define DIR_PINYIN "/pinyin"
#define DIR_F_NUM "/num"
#define DIR_F_TITLE "/title"
#define DIR_F_SNDLST "/l"
#define DIR_F_INTRO "/intro"
#define DIR_F_PHRASE "/phrase"
#define DIR_F_TPHRASE "/tphrase"
#define DIR_F_TEXT "/text"
#define DIR_EXT ".txt"
#define DIR_DAT ".dat"
#define DIR_IDX ".idx"
#define DIR_PAK ".pak"
#define DIR_EXTSND ".raw"

/* screen mode */
#define SCREENMODE_INTRO 0
#define SCREENMODE_TILE 2
#define SCREENMODE_INPUT 3

#define SCREENGR_ -1
#define SCREENGR_8BIT 0
#define SCREENGR_TILE 1
#define SCREENGR_TEXT 2

/* game state */
#define GAME_ -1
#define GAME_INTRO 0
#define GAME_NIHAO 1
#define GAME_CONFIG 2
#define GAME_CREDITS 3
#define GAME_MAINMENU 4
#define GAME_GETLESSON 5
#define GAME_REVIEWHANZI 6
#define GAME_EXAMS 7
#define GAME_GRADES 8
#define GAME_LESSON 30
#define GAME_LHANZI 31
#define GAME_LLISTEN 32
#define GAME_LGRAM 33
#define GAME_EX_QUIZ 50
#define GAME_EX_CARD 51
#define GAME_EX_LISTEN 52
#define GAME_DICT 60
#define GAME_NEWUSER 100
#define GAME_TODO 255

/* Certain substates */
#define SUBGAME_ -1
#define SUBGAME_EXQUIZHANZI 0
#define SUBGAME_EXQUIZPINYIN 1
#define SUBGAME_EXCARDHANZI 2
#define SUBGAME_EXCARDPINYIN 3
#define SUBGAME_EXQUIZMEAN 4
#define SUBGAME_EXCARDMEAN 5
#define SUBGAME_EXLISTENQUIZ 6

#define SUBGAME_MAXREALEXAMS 7

/* game time */
#define GTIME_INTRO 3 // 4 seconds

/* Smiley state */

#define SMILEYMOOD_ -1
#define SMILEYMOOD_NORMAL 0
#define SMILEYMOOD_NORMALMOVE 1
#define SMILEYMOOD_NORMALHAPPY 2
#define SMILEYMOOD_HAPPY 3
#define SMILEYMOOD_HAPPYJUMP 4
#define SMILEYMOOD_UNHAPPY 5
#define SMILEYMOOD_AMUSED 6
#define SMILEYMOOD_BLINK 7
#define SMILEYMOOD_WINK 8
#define SMILEYMOOD_ZOMBIE 9
#define SMILEYMOOD_WHAT 10
#define SMILEYMOOD_THINKING 11
#define SMILEYMOOD_AIYA 12
#define SMILEYMOOD_AIYAAAA 13
#define SMILEYMOOD_SUSPICIOUS 14
#define SMILEYMOOD_TALKING 253
#define SMILEYMOOD_APPEAR 254
#define SMILEYMOOD_DISAPPEAR 255

/* Buttons */

#define BTN_CONST_MINSIZE 32
#define BTN_CONST_MAXSIZE 256
#define BTN_GREY 0
#define BTN_RED 1
#define BTN_BLUE 2
#define BTN_GREEN 3
#define BTN_VIOLET 4
#define BTN_YELLOW 5
#define BTN_ORANGE 6
#define BTN_BACK 7
#define BTN_READ 8
#define BTN_CONFIG 9
#define BTN_FIELD 10
#define BTN_FIELDSEL 11
#define BTN_NONE 255
#define BTN_MAXT 11 // = biggest "visible" Btn32 type
#define BTN_32 32
#define BTN_64 64
#define BTN_BH_ 0
#define BTN_BH_BLIP 1
#define BTN_BH_MOVE 2
#define BTN_BH_BLIPCTN 3

/* Lessons */
#define LESSON_ 0
#define LESSON_GRAMM 1
#define LESSON_HANZI 2
#define LESSON_LISTEN 3
#define LESSON_HANZIATTACK 4

/* Language */
#define LANG_SPA 0
#define LANG_ENG 1

/* Text System */
#define TXT_INI_ENGLISH 1
#define TXT_INI_SPANISH 2
#define TXT_INI_PROFILE 3
#define TXT_MADEBY_ 25
#define TXT_CFG_ 50
#define TXT_CFG_SPEED 51
#define TXT_CFG_HAND 52
#define TXT_CFG_LHAND 53
#define TXT_CFG_RHAND 54
#define TXT_MM_ 100
#define TXT_MM_LESSON 101
#define TXT_MM_IDEOGRAMS 102
#define TXT_MM_EXAMS 103
#define TXT_MM_GRADES 104
#define TXT_GETLESS_ 200
#define TXT_GETLESS_IDEOGRAM 201
#define TXT_GETLESS_GRAMMAR 202
#define TXT_GETLESS_LISTEN 203
#define TXT_GETLESS_NIHAO 204
#define TXT_GETLESS_HANZIATTACK 205
#define TXT_REVIEWHANZI_ 300
#define TXT_LESSON_ 400
#define TXT_LESSON_GO 401
#define TXT_LESSON_NO 402
#define TXT_LESSON_END 403
#define TXT_LESSON_GRAM_ 430
#define TXT_LESSON_HANZI_ 440
#define TXT_LESSON_LISTEN_ 450
#define TXT_EXAM_ 500
#define TXT_EXAM_QUIZ 501
#define TXT_EXAM_CARDS 502
#define TXT_EXAM_LISTEN 503
#define TXT_EXAM_MEANING 510
#define TXT_GRADES_ 550
#define TXT_DICT_ 600
#define TXT_DICT_TEXT 601

/* Configuration values */
#define CONF_SPEED 60
#define CONF_LEFTHANDED 0
#define CONF_RIGHTHANDED 1

/* Max. lessons - hanzi */
#define MAX_HANZI 19360 //(0x4e00 - 0x9999) [19360 - 1210]
#define MAX_LESSONS 5
#define MAX_EXAMS 10
#define MAX_LESSONS_NUMBER 256
#define MAX_USERNAME 15

#define MAX_PROFILES 4

/* Structures */
//--------------

typedef struct slotGameData{
   int active;
   u32 hash;
	int language;
	int hand;
	int speed;
	char name[MAX_USERNAME];
	u16 lesson[MAX_LESSONS][MAX_LESSONS_NUMBER / 16];
	u8 lessongrades[MAX_LESSONS][MAX_LESSONS_NUMBER][MAX_EXAMS]; // ++CHANGE++ put MAX_LESSONS_NUMBER
	u16 knownhanzi[MAX_HANZI / 16];
	int khanzi;
} gData;

/* Variables */
//-------------

gData data_gameData[MAX_PROFILES]; // Space for 4 profiles
int data_hashtitle;   // Hash of the library which is being actually played
int data_profile;     // Actual profile played in the game
int data_lessontype;  // Actual lesson type being played
int data_lessonnumber;  // Actual lesson number being played
int data_nummainmenu;  // Number of times entered in the main menu
int data_newuser; // 1 if the user is new, 0 if not.
int data_ignorelid; // 1 if the DS should be working even if the lid is closed.
int data_lidopen; // Stores the state of the lid, used for the ignorelid case

/* Functions */
//-------------

/* Get System Text */
char *ct_const_getText(int id, int language);

#endif

/*
INTERNAL STRUCTURE


*/
