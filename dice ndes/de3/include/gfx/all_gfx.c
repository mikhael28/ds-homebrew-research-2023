//Gfx converted using Mollusk's PAGfx Converter

//This file contains all the .c, for easier inclusion in a project

#ifdef __cplusplus
extern "C" {
#endif

#include "all_gfx.h"


// Background files : 
#include "cinq.c"
#include "deux.c"
#include "dix.c"
#include "dixhuit.c"
#include "dixneuf.c"
#include "dixsept.c"
#include "douze.c"
#include "face.c"
#include "haut.c"
#include "huit.c"
#include "lancer.c"
#include "menu.c"
#include "neuf.c"
#include "onze.c"
#include "pile.c"
#include "quatorze.c"
#include "quatre.c"
#include "quinze.c"
#include "seize.c"
#include "sept.c"
#include "six.c"
#include "timer.c"
#include "toucher2.c"
#include "treize.c"
#include "trois.c"
#include "un.c"
#include "vingt.c"
#include "end.c"

// Palette files : 
#include "cinq.pal.c"
#include "deux.pal.c"
#include "dix.pal.c"
#include "dixhuit.pal.c"
#include "dixneuf.pal.c"
#include "dixsept.pal.c"
#include "douze.pal.c"
#include "face.pal.c"
#include "haut.pal.c"
#include "huit.pal.c"
#include "lancer.pal.c"
#include "menu.pal.c"
#include "neuf.pal.c"
#include "onze.pal.c"
#include "pile.pal.c"
#include "quatorze.pal.c"
#include "quatre.pal.c"
#include "quinze.pal.c"
#include "seize.pal.c"
#include "sept.pal.c"
#include "six.pal.c"
#include "timer.pal.c"
#include "toucher2.pal.c"
#include "treize.pal.c"
#include "trois.pal.c"
#include "un.pal.c"
#include "vingt.pal.c"
#include "end.pal.c"

// Background Pointers :
PAGfx_struct cinq = {(void*)cinq_Map, 768, (void*)cinq_Tiles, 11648, (void*)cinq_Pal, (int*)cinq_Info };
PAGfx_struct deux = {(void*)deux_Map, 768, (void*)deux_Tiles, 11584, (void*)deux_Pal, (int*)deux_Info };
PAGfx_struct dix = {(void*)dix_Map, 768, (void*)dix_Tiles, 12672, (void*)dix_Pal, (int*)dix_Info };
PAGfx_struct dixhuit = {(void*)dixhuit_Map, 768, (void*)dixhuit_Tiles, 12800, (void*)dixhuit_Pal, (int*)dixhuit_Info };
PAGfx_struct dixneuf = {(void*)dixneuf_Map, 768, (void*)dixneuf_Tiles, 12096, (void*)dixneuf_Pal, (int*)dixneuf_Info };
PAGfx_struct dixsept = {(void*)dixsept_Map, 768, (void*)dixsept_Tiles, 11712, (void*)dixsept_Pal, (int*)dixsept_Info };
PAGfx_struct douze = {(void*)douze_Map, 768, (void*)douze_Tiles, 12608, (void*)douze_Pal, (int*)douze_Info };
PAGfx_struct face = {(void*)face_Map, 768, (void*)face_Tiles, 9152, (void*)face_Pal, (int*)face_Info };
PAGfx_struct haut = {(void*)haut_Map, 768, (void*)haut_Tiles, 16448, (void*)haut_Pal, (int*)haut_Info };
PAGfx_struct huit = {(void*)huit_Map, 768, (void*)huit_Tiles, 11840, (void*)huit_Pal, (int*)huit_Info };
PAGfx_struct lancer = {(void*)lancer_Map, 768, (void*)lancer_Tiles, 23040, (void*)lancer_Pal, (int*)lancer_Info };
PAGfx_struct menu = {(void*)menu_Map, 768, (void*)menu_Tiles, 13760, (void*)menu_Pal, (int*)menu_Info };
PAGfx_struct neuf = {(void*)neuf_Map, 768, (void*)neuf_Tiles, 11520, (void*)neuf_Pal, (int*)neuf_Info };
PAGfx_struct onze = {(void*)onze_Map, 768, (void*)onze_Tiles, 10880, (void*)onze_Pal, (int*)onze_Info };
PAGfx_struct pile = {(void*)pile_Map, 768, (void*)pile_Tiles, 8448, (void*)pile_Pal, (int*)pile_Info };
PAGfx_struct quatorze = {(void*)quatorze_Map, 768, (void*)quatorze_Tiles, 11584, (void*)quatorze_Pal, (int*)quatorze_Info };
PAGfx_struct quatre = {(void*)quatre_Map, 768, (void*)quatre_Tiles, 11584, (void*)quatre_Pal, (int*)quatre_Info };
PAGfx_struct quinze = {(void*)quinze_Map, 768, (void*)quinze_Tiles, 12352, (void*)quinze_Pal, (int*)quinze_Info };
PAGfx_struct seize = {(void*)seize_Map, 768, (void*)seize_Tiles, 12160, (void*)seize_Pal, (int*)seize_Info };
PAGfx_struct sept = {(void*)sept_Map, 768, (void*)sept_Tiles, 11008, (void*)sept_Pal, (int*)sept_Info };
PAGfx_struct six = {(void*)six_Map, 768, (void*)six_Tiles, 11648, (void*)six_Pal, (int*)six_Info };
PAGfx_struct timer = {(void*)timer_Map, 768, (void*)timer_Tiles, 6464, (void*)timer_Pal, (int*)timer_Info };
PAGfx_struct toucher2 = {(void*)toucher2_Map, 768, (void*)toucher2_Tiles, 23680, (void*)toucher2_Pal, (int*)toucher2_Info };
PAGfx_struct treize = {(void*)treize_Map, 768, (void*)treize_Tiles, 12416, (void*)treize_Pal, (int*)treize_Info };
PAGfx_struct trois = {(void*)trois_Map, 768, (void*)trois_Tiles, 11584, (void*)trois_Pal, (int*)trois_Info };
PAGfx_struct un = {(void*)un_Map, 768, (void*)un_Tiles, 10752, (void*)un_Pal, (int*)un_Info };
PAGfx_struct vingt = {(void*)vingt_Map, 768, (void*)vingt_Tiles, 13376, (void*)vingt_Pal, (int*)vingt_Info };
PAGfx_struct end = {(void*)end_Map, 768, (void*)end_Tiles, 15168, (void*)end_Pal, (int*)end_Info };


#ifdef __cplusplus
}
#endif

