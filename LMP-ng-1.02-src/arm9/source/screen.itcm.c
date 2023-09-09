#include <nds.h>
#include <nds/arm9/console.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ipc2.h>

#define SCREEN_C

#include "playlist.h"
#include "screen.h"
#include "text.h"
#include "wheel.h"
#include "file.h"
#include "sound.h"
#include "input.h"

#include "ipod_top_bin.h"
#include "ipod_bottom_bin.h"

#include "batteryicons_bin.h"
#include "statusicons_bin.h"
#include "selected_bin.h"
#include "unknown_bin.h"
#include "progress_bin.h"
#include "sound_bin.h"
#include "shuffle_bin.h"
#include "seekbar_bin.h"
#include "seekpos_bin.h"

#include "myriad_lmp.h"

#define SMALL_SCREEN_X 23
#define SMALL_SCREEN_Y 24
#define SMALL_SCREEN_WIDTH 210
#define SMALL_SCREEN_HEIGHT 144

#define font font_myriad_web_9

#define batteryicon ((u16 *) SPRITE_GFX)
#define stateicon (((u16 *) SPRITE_GFX) + 32*32)
#define holdicon (((u16 *) SPRITE_GFX) + 32*32 + 16*16)
#define albumicon (((u16 *) SPRITE_GFX) + 32*32 + 16*16 + 16*16)
#define volumeicon (((u16 *) SPRITE_GFX) + 32*32 + 16*16 + 16*16 + 64*64)
#define shuffleicon (((u16 *) SPRITE_GFX) + 32*32 + 16*16 + 16*16 + 64*64 + 16*16)
#define seekposicon (((u16 *) SPRITE_GFX) + 32*32 + 16*16 + 16*16 + 64*64 + 16*16 + 16*16)
#define albumpic (((SpriteEntry*)OAM)[3])
#define volumepic (((SpriteEntry*)OAM)[4])
#define shufflepic (((SpriteEntry*)OAM)[5])
#define seekpospic (((SpriteEntry*)OAM)[6])


static uint16 * const main_screen = (uint16 *)0x06020000,
			 * const small_screen = (uint16 *) 0x06040000;
static uint16 *backbuffer, *frontbuffer;
static char *screen_menu_name;
static u8 progress_offset;

static struct screen_scrolling_text *screen_scroll_title = &screen_scrolling_text[0],
																		*screen_scroll_artist = &screen_scrolling_text[1],
																		*screen_scroll_album = &screen_scrolling_text[2];

static void screen_reset_backbuffer(void) {
	int i, j;

	for(i = SMALL_SCREEN_X; i < SMALL_SCREEN_WIDTH + SMALL_SCREEN_X; i++)
		for(j = SMALL_SCREEN_Y; j < SMALL_SCREEN_Y + 16 + SMALL_SCREEN_Y; j++)
			backbuffer[j * 256 + i] = ((u16 *) ipod_top_bin)[j * 256 + i];

	for(i = SMALL_SCREEN_X; i < SMALL_SCREEN_WIDTH + SMALL_SCREEN_X; i++)
		for(j = SMALL_SCREEN_Y + 16; j < SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y; j++)
			backbuffer[j * 256 + i] = RGB15(31,31,31) | BIT(15);
}

static void screen_switchbuffers(void) {
	uint16 *t;

	t = backbuffer;
	backbuffer = frontbuffer;
	frontbuffer = t;

	BG2_CR ^= BG_PRIORITY_1;
	BG2_CR ^= BG_PRIORITY_2;
	BG3_CR ^= BG_PRIORITY_1;
	BG3_CR ^= BG_PRIORITY_2;
}

static void screen_initsprites(void) {
	SpriteEntry *spr_batt,
							*spr_state,
							*spr_hold,
							*spr_album,
							*spr_vol,
							*spr_shuffle,
							*spr_seekpos;

	spr_batt = &((SpriteEntry*)OAM)[0];
	spr_batt->attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (SMALL_SCREEN_Y - 1);
	spr_batt->attribute[1] = ATTR1_SIZE_32 | (256 - SMALL_SCREEN_X - 31);
	spr_batt->attribute[2] = ATTR2_ALPHA(1) | 0;

	spr_state = &((SpriteEntry*)OAM)[1];
	spr_state->attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (SMALL_SCREEN_Y - 1);
	spr_state->attribute[1] = ATTR1_SIZE_16 | (SMALL_SCREEN_X + 1);
	spr_state->attribute[2] = ATTR2_ALPHA(1) | 16;

	spr_hold = &((SpriteEntry*)OAM)[2];
	spr_hold->attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (SMALL_SCREEN_Y - 1);
	spr_hold->attribute[1] = ATTR1_SIZE_16 | (SMALL_SCREEN_X + 17);
	spr_hold->attribute[2] = ATTR2_ALPHA(1) | 20;

	spr_album = &((SpriteEntry*)OAM)[3];
	spr_album->attribute[0] = ATTR0_DISABLED;
	spr_album->attribute[1] = ATTR1_SIZE_64 | (SMALL_SCREEN_X + 8);
	spr_album->attribute[2] = ATTR2_ALPHA(1) | 24;

	spr_hold = &((SpriteEntry*)OAM)[4];
	spr_hold->attribute[0] = ATTR0_DISABLED;
	spr_hold->attribute[1] = ATTR1_SIZE_16 | (SMALL_SCREEN_X + 8);
	spr_hold->attribute[2] = ATTR2_ALPHA(1) | 88;

	spr_hold = &((SpriteEntry*)OAM)[5];
	spr_hold->attribute[0] = ATTR0_DISABLED;
	spr_hold->attribute[1] = ATTR1_SIZE_16 | (256 - SMALL_SCREEN_X - 17);
	spr_hold->attribute[2] = ATTR2_ALPHA(1) | 92;

	spr_seekpos = &((SpriteEntry*)OAM)[6];
	spr_seekpos->attribute[0] = ATTR0_DISABLED;
	spr_seekpos->attribute[2] = ATTR2_ALPHA(1) | 96;
}

void screen_initdisplays(void) {
	int i, j;
	const char *starting = "Starting...";

	/* setup main display */
	videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE
		| DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT | DISPLAY_SPR_1D_BMP);
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
	vramSetBankD(VRAM_D_MAIN_BG_0x06040000);
	vramSetBankE(VRAM_E_MAIN_SPRITE);

	/* console */
	BG0_CR = BG_MAP_BASE(31);
	BG_PALETTE[255] = RGB15(31,31,31);
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK(31), (u16*)CHAR_BASE_BLOCK(0), 16);

	/* background */
	BG2_CR = BG_BMP16_256x256 | BG_BMP_BASE(128 / 16) | BG_PRIORITY_1;
	BG2_XDX = 1 << 8;
	BG2_XDY = 0;
	BG2_YDX = 0;
	BG2_YDY = 1 << 8;
	BG2_CX = 0;
	BG2_CY = 0;

	/* screen */
	BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(256 / 16) | BG_PRIORITY_2;
	BG3_XDX = 1 << 8;
	BG3_XDY = 0;
	BG3_YDX = 0;
	BG3_YDY = 1 << 8;
	BG3_CX = 8;
	BG3_CY = 8;

	/* setup sub display */
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	vramSetBankC(VRAM_C_SUB_BG);
	SUB_BG3_CR = BG_BMP16_256x256;

	/* scaling and rotating */
	SUB_BG3_XDX = 1 << 8;
	SUB_BG3_XDY = 0;
	SUB_BG3_YDX = 0;
	SUB_BG3_YDY = 1 << 8;

	/* translation */
	SUB_BG3_CX = 0;
	SUB_BG3_CY = 0;

	screen_initsprites();

	backbuffer = small_screen;
	frontbuffer = main_screen;

	dmaCopy(ipod_bottom_bin, BG_GFX_SUB, 256*192*2);
	dmaCopy(ipod_top_bin, backbuffer, 256*192*2);
	dmaCopy(ipod_top_bin, frontbuffer, 256*192*2);

	for(i = SMALL_SCREEN_X; i < SMALL_SCREEN_WIDTH + SMALL_SCREEN_X; i++)
		for(j = SMALL_SCREEN_Y + 16; j < SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y; j++)
			frontbuffer[j * 256 + i] = backbuffer[j * 256 + i] = RGB15(31,31,31) | BIT(15);

	for(i = SMALL_SCREEN_X; i < SMALL_SCREEN_WIDTH + SMALL_SCREEN_X; i++)
		for(j = SMALL_SCREEN_Y; j < SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y; j++)
			small_screen[j * 256 + i] = main_screen[j * 256 + i];

	dispString(SMALL_SCREEN_X + SMALL_SCREEN_WIDTH/2 - getStringWidth(starting, font)/2,
		SMALL_SCREEN_Y + SMALL_SCREEN_HEIGHT/2 - getStringHeight(starting, font)/2 - 1,
		0,
		starting,
		backbuffer,
		font,
		SMALL_SCREEN_WIDTH,
		SMALL_SCREEN_HEIGHT,
		256);

	for(i = 0; i < 64; i++)
		for(j = 0; j < 64; j++)
			albumicon[i * 64 + j] = ((u16 *) unknown_bin)[i * 64 + j] | BIT(15);

	for(i = 0; i < 16; i++)
		for(j = 0; j < 16; j++)
			volumeicon[i * 16 + j] = ((u16 *) sound_bin)[i * 16 + j] | BIT(15);

	for(i = 0; i < 16; i++)
		for(j = 0; j < 16; j++)
			shuffleicon[i * 16 + j] = ((u16 *) shuffle_bin)[i * 16 + j] | BIT(15);

	for(i = 0; i < 16; i++)
		for(j = 0; j < 16; j++)
			seekposicon[i * 16 + j] = ((u16 *) seekpos_bin)[i * 16 + j];

	screen_switchbuffers();
}

static void screen_update_battery(void) {
	static uint16 battery = 0x7fff, aux = 0x7fff;
	int i, j;

	u32 x, y;

	if(battery == IPC->battery && aux == IPC->aux)
		return;

	battery = IPC->battery;
	aux = IPC->aux;

	if(battery == 0 && aux == 0) {
		dmaCopy(batteryicons_bin, batteryicon, 32 * 16 * 2);
	} else if(battery == 0 && aux != 0)
		dmaCopy(batteryicons_bin + 2 * 2*32*16, batteryicon, 32 * 16 * 2);
	else
		dmaCopy(batteryicons_bin + 2 * 32*16, batteryicon, 32 * 16 * 2);
}

static void screen_update_state(void) {
	static u8 state = 0xff;

	if(state == IPC2->sound_state)
		return;

	state = IPC2->sound_state;

	switch(state) {
		case IPC2_PLAYING:
			dmaCopy(statusicons_bin, stateicon, 16*16*2);
			break;

		default:
		case IPC2_STOPPED:
			dmaCopy(statusicons_bin + 16*16*2, stateicon, 16*16*2);
			break;
	}
}

static void screen_update_hold(void) {
	static u8 hold = 0xff;

	if(hold == input_hold)
		return;

	hold = input_hold;

	switch(hold) {
		default:
		case INPUT_HOLD_OFF:
			memset(holdicon, 0, 16*16*2);
			break;

		case INPUT_HOLD_NORMAL:
			dmaCopy(statusicons_bin  + 2 * 16*16*2, holdicon, 16*16*2);
			break;

		case INPUT_HOLD_ALL:
			dmaCopy(statusicons_bin  + 3 * 16*16*2, holdicon, 16*16*2);
			break;
	}
}

static int screen_drawmenu(const struct screen_menu_entry *m) {
	int i, j;
	struct screen_scrolling_text *s = &screen_scrolling_text[3];
	int t;

	if(screen_menu_name != NULL) {
		int w;

		w = getStringWidth(screen_menu_name, font);

		if(SMALL_SCREEN_WIDTH/2 - w/2 < 40)
			w = 40;
		else
			w = SMALL_SCREEN_WIDTH/2 - w/2;

		dispString2(SMALL_SCREEN_X + w,
			SMALL_SCREEN_Y,
			0, screen_menu_name, backbuffer, font,
			SMALL_SCREEN_WIDTH + SMALL_SCREEN_X - 40,
			SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y,
			256);
	}

	if(screen_menu_selected > screen_menu_startentry && screen_menu_selected - screen_menu_startentry >= 7)
		screen_menu_startentry = screen_menu_selected - 7;
	else if(screen_menu_startentry > screen_menu_selected)
		screen_menu_startentry = screen_menu_selected;

	for(i = screen_menu_startentry; i < screen_menu_items && i - screen_menu_startentry < 8; i++) {

		dispString2(SMALL_SCREEN_X + 3,
			SMALL_SCREEN_Y + 16 + (i - screen_menu_startentry) * 16, 0,
			m[i].label, backbuffer, font,
			SMALL_SCREEN_X + SMALL_SCREEN_WIDTH - 3 - 12, SMALL_SCREEN_Y + SMALL_SCREEN_HEIGHT,
			256);

		if(m[i].have_submenu == 1)
			dispString2(SMALL_SCREEN_X + SMALL_SCREEN_WIDTH - 12,
				SMALL_SCREEN_Y + 16 + (i - screen_menu_startentry) * 16, 0,
				">",
				backbuffer, font, SMALL_SCREEN_WIDTH + SMALL_SCREEN_X - 3, SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y, 256);
	}


	for(i = 0; i <16; i++)
		for(j = 0; j < 210; j++)
			backbuffer[(i + 16 * (screen_menu_selected - screen_menu_startentry + 1) + SMALL_SCREEN_Y) * 256 + SMALL_SCREEN_X + j] = ((u16 *) selected_bin)[i * 210 + j] | BIT(15);

	t = screen_menu_selected - screen_menu_startentry;

	if(s->data != NULL) {
		free(s->data);
		s->data = NULL;
	}
	s->animate = 0;


	s->width = getStringWidth(m[screen_menu_selected].label, font) + 1;
	s->height = getStringHeight(m[screen_menu_selected].label, font) + 1;

	if(s->width > SMALL_SCREEN_WIDTH - 3 - 3 - 12) {
		s->animate = 1;

		s->data = (u16 *) malloc(s->width * s->height * 2);
		for(i = 0; i < s->height; i++)
			for(j = 0; j < s->width; j++)
				s->data[i * s->width + j] = ((u16 *) selected_bin)[i * 210] | BIT(15);

		s->background = (u16 *) selected_bin;
		s->bg_width = 210;

		dispString(0, 0, 0xffff,
			m[screen_menu_selected].label, s->data, font,
			s->width, s->height,
			s->width);

		s->x_offset = 0;
		s->y = 16 + SMALL_SCREEN_Y + t * 16;
		s->x = 3 + SMALL_SCREEN_X;
		s->delay = 0;
		s->maxwidth = SMALL_SCREEN_X + SMALL_SCREEN_WIDTH - 3 - 12;

	} else {
		dispString(SMALL_SCREEN_X + 3, SMALL_SCREEN_Y + 16 *(screen_menu_selected - screen_menu_startentry + 1), 0xffff,
			m[screen_menu_selected].label,
			backbuffer, font, SMALL_SCREEN_WIDTH + SMALL_SCREEN_X, SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y, 256);
	}

	if(m[screen_menu_selected].have_submenu == 1)
		dispString(SMALL_SCREEN_X + SMALL_SCREEN_WIDTH - 12, SMALL_SCREEN_Y + 16 *(screen_menu_selected - screen_menu_startentry + 1), 0xffff,
			">",
			backbuffer, font, SMALL_SCREEN_WIDTH + SMALL_SCREEN_X, SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y, 256);

	screen_switchbuffers();
}

static struct screen_menu_entry *screen_playlist_menu;
void screen_playlistmenu(struct playlist *p) {
	int i;

	if(screen_playlist_menu != NULL) {
		free(screen_playlist_menu);
		screen_playlist_menu = NULL;
	}

	screen_playlist_menu = (struct screen_menu_entry *) malloc(p->size * sizeof(struct screen_menu_entry));

	for(i = 0; i < p->size; i++) {
		screen_playlist_menu[i].label = p->list[i]->title;
		screen_playlist_menu[i].have_submenu = 0;
		screen_playlist_menu[i].callback = input_playlist;
	}

	screen_type = MENU;
	screen_menu = LIST;

	if(p->name != NULL)
		screen_menu_name = p->name;
	else
		screen_menu_name = "";

	screen_menu_items = p->size;
	if(screen_menu_startentry < 0 || screen_menu_startentry > p->size)
		screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = playlist_input_p;
	currentmenu = screen_playlist_menu;
}

static struct screen_menu_entry *screen_playlists_menu;
void screen_playlistsmenu(void) {
	int i;
	int size;

	if(screen_playlists_menu != NULL) {
		free(screen_playlists_menu);
		screen_playlists_menu = NULL;
	}

	screen_playlists_menu = (struct screen_menu_entry *) malloc(playlists.size * sizeof(struct screen_menu_entry));

	size = 0;
	for(i = 0; i < playlists.size; i++) {
		if(playlists.p[i]->name == NULL)
			continue;

		screen_playlists_menu[size].label = playlists.p[i]->name;
		screen_playlists_menu[size].have_submenu = 0;
		screen_playlists_menu[size].callback = input_playlists;
		size++;
	}

	screen_type = MENU;
	screen_menu = LISTS;

	screen_menu_name = "Playlists";

	screen_menu_items = size;
	if(screen_menu_startentry < 0 || screen_menu_startentry > size)
		screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = playlists_input_p;
	currentmenu = screen_playlists_menu;
}

static struct screen_menu_entry music_menu[] = {
	{ "Songs", 1, input_songs_setup },
	{ "Playlists", 1, input_playlists_setup },
	{ "Now Playing", 1, input_nowplaying_setup }
};
static char music_menu_name[] = "Music";
void screen_musicmenu(void) {
	screen_type = MENU;
	screen_menu = MUSIC;

	screen_menu_name = music_menu_name;

	screen_menu_items = 3;
	screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = music_input_p;
	currentmenu = music_menu;
}

static struct screen_menu_entry main_menu[] = {
	{ "Music", 1, input_musicmenu_setup },
	{ "Shuffle Songs", 0, input_shuffle_setup },
//	{ "Photos", 1, NULL },
//	{ "Videos", 1, NULL },
//	{ "Settings", 0, NULL },
	{ "Now Playing", 1, input_nowplaying_setup }
};

void screen_mainmenu(void) {
	screen_type = MENU;
	screen_menu = MAIN;

	screen_menu_name = NULL;

	screen_menu_items = 3;
	screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = menu_input_p;
	currentmenu = main_menu;
}

void screen_nowplaying(void) {
	char *playing = "Now Playing";
	char buffer[128];
	int i, j;

	screen_type = NOW_PLAYING;

	currenthandle = nowplaying_input_p;

	dispString(SMALL_SCREEN_X + SMALL_SCREEN_WIDTH/2 - getStringWidth(playing, font)/2,
		SMALL_SCREEN_Y,
		0, playing, backbuffer, font,
		SMALL_SCREEN_WIDTH + SMALL_SCREEN_X,
		SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y,
		256);

	albumpic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (SMALL_SCREEN_Y + 16 + 20);

	if(playlists.p[playlists.current] == shufflelist)
		shufflepic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (SMALL_SCREEN_Y + 16 + 3);

	sprintf(buffer, "%d of %d", PLAYLIST->current + 1, PLAYLIST->size);
	dispString(SMALL_SCREEN_X + 8,
		SMALL_SCREEN_Y + 16 + 3,
		0, buffer, backbuffer, font,
		SMALL_SCREEN_WIDTH + SMALL_SCREEN_X,
		SMALL_SCREEN_HEIGHT + SMALL_SCREEN_Y,
		256);

	if(currentmedia->title != NULL) {
		if(screen_scroll_title->data != NULL) {
			free(screen_scroll_title->data);
			screen_scroll_title->data = NULL;
		}

		screen_scroll_title->width = getStringWidth(currentmedia->title, font) + 1;
		screen_scroll_title->height = getStringHeight(currentmedia->title, font) + 1;

		if(screen_scroll_title->width > SMALL_SCREEN_WIDTH - 80) {
			screen_scroll_title->animate = 1;

			screen_scroll_title->data = (u16 *) malloc(screen_scroll_title->width * screen_scroll_title->height * 2);
			for(i = 0; i < screen_scroll_title->height; i++)
				for(j = 0; j < screen_scroll_title->width; j++)
					screen_scroll_title->data[i * screen_scroll_title->width + j] = RGB15(31,31,31) | BIT(15);

			dispString(0, 0, 0,
				currentmedia->title, screen_scroll_title->data, font,
				screen_scroll_title->width, screen_scroll_title->height,
				screen_scroll_title->width);

			screen_scroll_title->x_offset = 0;
			screen_scroll_title->y = 16 + 16 + SMALL_SCREEN_Y;
			screen_scroll_title->x = 80 + SMALL_SCREEN_X;
			screen_scroll_title->delay = 0;
			screen_scroll_title->maxwidth = SMALL_SCREEN_X + SMALL_SCREEN_WIDTH - 3;

		} else {
			screen_scroll_title->animate = 0;

			dispString(SMALL_SCREEN_X + 80, SMALL_SCREEN_Y + 16 + 16, 0,
				currentmedia->title, backbuffer, font,
				SMALL_SCREEN_X + SMALL_SCREEN_WIDTH, SMALL_SCREEN_Y + SMALL_SCREEN_HEIGHT,
				256);
		}
	}

	if(currentmedia->artist != NULL) {
		if(screen_scroll_artist->data != NULL) {
			free(screen_scroll_artist->data);
			screen_scroll_artist->data = NULL;
		}

		screen_scroll_artist->width = getStringWidth(currentmedia->artist, font) + 1;
		screen_scroll_artist->height = getStringHeight(currentmedia->artist, font) + 1;

		if(screen_scroll_artist->width > SMALL_SCREEN_WIDTH - 80) {
			screen_scroll_artist->animate = 1;

			screen_scroll_artist->data = (u16 *) malloc(screen_scroll_artist->width * screen_scroll_artist->height * 2);
			for(i = 0; i < screen_scroll_artist->height; i++)
				for(j = 0; j < screen_scroll_artist->width; j++)
					screen_scroll_artist->data[i * screen_scroll_artist->width + j] = RGB15(31,31,31) | BIT(15);

			dispString(0, 0, 0,
				currentmedia->artist, screen_scroll_artist->data, font,
				screen_scroll_artist->width, screen_scroll_artist->height,
				screen_scroll_artist->width);

			screen_scroll_artist->x_offset = 0;
			screen_scroll_artist->y = 16 + 40 + SMALL_SCREEN_Y;
			screen_scroll_artist->x = 80 + SMALL_SCREEN_X;
			screen_scroll_artist->delay = 0;
			screen_scroll_artist->maxwidth = SMALL_SCREEN_X + SMALL_SCREEN_WIDTH - 3;

		} else {
			screen_scroll_artist->animate = 0;

			dispString(SMALL_SCREEN_X + 80, SMALL_SCREEN_Y + 16 + 40, 0,
				currentmedia->artist, backbuffer, font,
				SMALL_SCREEN_X + SMALL_SCREEN_WIDTH, SMALL_SCREEN_Y + SMALL_SCREEN_HEIGHT,
				256);
		}
	}

	if(currentmedia->album != NULL) {
		if(screen_scroll_album->data != NULL) {
			free(screen_scroll_album->data);
			screen_scroll_album->data = NULL;
		}

		screen_scroll_album->width = getStringWidth(currentmedia->album, font) + 1;
		screen_scroll_album->height = getStringHeight(currentmedia->album, font) + 1;

		if(screen_scroll_album->width > SMALL_SCREEN_WIDTH - 80) {

			screen_scroll_album->animate = 1;

			screen_scroll_album->data = (u16 *) malloc(screen_scroll_album->width * screen_scroll_album->height * 2);
			for(i = 0; i < screen_scroll_album->height; i++)
				for(j = 0; j < screen_scroll_album->width; j++)
					screen_scroll_album->data[i * screen_scroll_album->width + j] = RGB15(31,31,31) | BIT(15);

			dispString(0, 0, 0,
				currentmedia->album, screen_scroll_album->data, font,
				screen_scroll_album->width, screen_scroll_album->height,
				screen_scroll_album->width);

			screen_scroll_album->x_offset = 0;
			screen_scroll_album->y = 16 + 64 + SMALL_SCREEN_Y;
			screen_scroll_album->x = 80 + SMALL_SCREEN_X;
			screen_scroll_album->delay = 0;
			screen_scroll_album->maxwidth = SMALL_SCREEN_X + SMALL_SCREEN_WIDTH - 3;

		} else {
			screen_scroll_album->animate = 0;

			dispString(SMALL_SCREEN_X + 80, SMALL_SCREEN_Y + 16 + 64, 0,
				currentmedia->album, backbuffer, font,
				SMALL_SCREEN_X + SMALL_SCREEN_WIDTH, SMALL_SCREEN_Y + SMALL_SCREEN_HEIGHT,
				256);
		}
	}

	screen_switchbuffers();
}

void screen_nowplaying_reset(void) {
	if(screen_scroll_title != NULL)
		screen_scroll_title->animate = 0;
	if(screen_scroll_artist != NULL)
		screen_scroll_artist->animate = 0;
	if(screen_scroll_album != NULL)
		screen_scroll_album->animate = 0;

	screen_reset_backbuffer();
	screen_nowplaying();
	screen_reset_backbuffer();
	screen_nowplaying();
}

static void screen_animate_update(void) {
	int i, j, k;
	int offset;
	struct screen_scrolling_text *s = screen_scrolling_text;

	for(k = 0; k < SCREEN_MAX_SCROLLING_TEXT; k++) {
		if(s[k].animate == 0)
			continue;

		offset = s[k].x_offset/2;

		if(offset == 0 && s[k].delay == 0)
			s[k].delay = 60;

		if(s[k].delay != 0)
			s[k].delay--;

		if(s[k].delay == 0)
			s[k].x_offset++;

		if(s[k].x_offset >= 2 * s[k].width)
			s[k].x_offset = 0;

		for(i = 0; i < s[k].height; i++)
			for(j = 0; j + offset < s[k].width && s[k].x + j < s[k].maxwidth; j++)
				backbuffer[(i + s[k].y) * 256 + s[k].x + j] = s[k].data[i * s[k].width + j + offset];

		if(s[k].background == NULL)
			for(i = 0; i < s[k].height; i++)
				for(j = s[k].width - offset; j < s[k].width && s[k].x + j < s[k].maxwidth; j++)
					backbuffer[(i + s[k].y) * 256 + s[k].x + j] = RGB15(31,31,31) | BIT(15);
		else
			for(i = 0; i < s[k].height; i++)
				for(j = s[k].width - offset; j < s[k].width && s[k].x + j < s[k].maxwidth; j++)
					backbuffer[(i + s[k].y) * 256 + s[k].x + j] = s[k].background[i * s[k].bg_width] | BIT(15);

	}

	/* the nice progressbar */
	if(screen_type == NOW_PLAYING) {
		float pos;

		if(nowplaying_mode == NOWPLAYING_SEEK
			|| nowplaying_mode == NOWPLAYING_VOLUME) {

			if(nowplaying_mode == NOWPLAYING_SEEK && nowplaying_delay > 0)
				nowplaying_delay--;
			else
				nowplaying_mode = NOWPLAYING_VOLUME;

			if(state == PLAYING)
				progress_offset++;

			pos = ((float) sound_position() * 194.) / (float) sound_size();
			volumepic.attribute[0] = ATTR0_DISABLED;
			seekpospic.attribute[0] = ATTR0_DISABLED;
		}
		else if(nowplaying_mode == NOWPLAYING_VOLUME_CHANGING) {
			progress_offset = 0;
			if(nowplaying_delay > 0)
				nowplaying_delay--;
			else
				nowplaying_mode = NOWPLAYING_VOLUME;

			pos = ((float) IPC2->sound_volume * 194.) / (float) 0xffff;
			volumepic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (SMALL_SCREEN_Y + 108 + 12);
			seekpospic.attribute[0] = ATTR0_DISABLED;
		}

		if(nowplaying_mode == NOWPLAYING_VOLUME
			|| nowplaying_mode == NOWPLAYING_VOLUME_CHANGING
			|| (nowplaying_mode == NOWPLAYING_SEEK && nowplaying_delay == 0)) {
			for(i = 0; i < 10; i++)
				for(j = 0; j < pos; j++)
					backbuffer[(i + SMALL_SCREEN_Y + 108) * 256 + j + SMALL_SCREEN_X + 8] = ((u16 *) progress_bin)[i * 194 + (j + progress_offset) % 8 + 1940] | BIT(15);

			for(i = 0; i < 10; i++)
				for(j = pos; j < 194; j++)
					backbuffer[(i + SMALL_SCREEN_Y + 108) * 256 + j + SMALL_SCREEN_X + 8] = ((u16 *) progress_bin)[i * 194 + j] | BIT(15);
		} else if(nowplaying_mode == NOWPLAYING_SEEK && nowplaying_delay > 0) {
			for(i = 0; i < 10; i++)
				for(j = 0; j < 194; j++)
					backbuffer[(i + SMALL_SCREEN_Y + 108) * 256 + j + SMALL_SCREEN_X + 8] = RGB15(31,31,31) | BIT(15);

			for(i = 0; i < 4; i++)
				for(j = 0; j < 194; j++)
					backbuffer[(i + SMALL_SCREEN_Y + 111) * 256 + j + SMALL_SCREEN_X + 8] = ((u16 *) seekbar_bin)[i % 4] | BIT(15);

			seekpospic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (SMALL_SCREEN_Y + 111 - 3);
			seekpospic.attribute[1] = ATTR1_SIZE_16 | (SMALL_SCREEN_X + 8 - 4 + (int) pos);
		}
	}

	screen_switchbuffers();
}

void screen_update(void) {
	static struct screen_menu_entry *lastmenu = NULL;
	static u32 lastselected = 0xffffffff;
	static enum screen_type lasttype = OTHER;

	screen_update_battery();
	screen_update_hold();
	screen_update_state();

	if(screen_type != lasttype) {
		IPC2->sound_control = IPC2_SOUND_CLICK;

		if(lasttype == NOW_PLAYING) {
			albumpic.attribute[0] = ATTR0_DISABLED;
			volumepic.attribute[0] = ATTR0_DISABLED;
			shufflepic.attribute[0] = ATTR0_DISABLED;
			seekpospic.attribute[0] = ATTR0_DISABLED;

			if(screen_scroll_title)
				screen_scroll_title->animate = 0;
			if(screen_scroll_artist)
				screen_scroll_artist->animate = 0;
			if(screen_scroll_album)
				screen_scroll_album->animate = 0;

			nowplaying_delay = 0;
		}

		if(lasttype == MENU) {
			screen_scrolling_text[3].animate = 0;
		}

		lasttype = screen_type;

		if(screen_type == NOW_PLAYING) {
			lastselected = 0xffffffff;

			screen_nowplaying_reset();
		}
	}

	if(lasttype == MENU) {
		if(lastmenu != currentmenu || lastselected != screen_menu_selected) {
			IPC2->sound_control = IPC2_SOUND_CLICK;

			screen_reset_backbuffer();
			screen_drawmenu(currentmenu);
			screen_reset_backbuffer();
			screen_drawmenu(currentmenu);

			lastmenu = currentmenu;
			lastselected = screen_menu_selected;
		}
	}

	screen_animate_update();

	input_handleinput();
}
