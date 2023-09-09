
#pragma once

#include "glib.h"

#define FCH_SIN 'a'
#define FCH_COS 'b'
#define FCH_TAN 'c'
#define FCH_EXP 'd'
#define FCH_LOG 'e'
#define FCH_PI 'f'

#define FSTR_SIN "sin"
#define FSTR_COS "cos"
#define FSTR_TAN "tan"
#define FSTR_EXP "exp"
#define FSTR_LOG "log"
#define FSTR_PI "PI"

extern bool Calc_Draw(CglCanvas *pcan);

static void Calc_GetStrFromChar(char *pstr,const char ch)
{
  pstr[0]=0;
  switch(ch){
    case FCH_SIN: { strcpy(pstr,FSTR_SIN); } break;
    case FCH_COS: { strcpy(pstr,FSTR_COS); } break;
    case FCH_TAN: { strcpy(pstr,FSTR_TAN); } break;
    case FCH_EXP: { strcpy(pstr,FSTR_EXP); } break;
    case FCH_LOG: { strcpy(pstr,FSTR_LOG); } break;
    case FCH_PI: { strcpy(pstr,FSTR_PI); } break;
    default: {
      pstr[0]=ch;
      pstr[1]=0;
    } break;
  }
}

