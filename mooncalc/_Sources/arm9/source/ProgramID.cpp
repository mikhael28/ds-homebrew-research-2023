
#include <nds.h>

#include "_const.h"
#include "maindef.h"

#include "__debug.h"

const char ROMTITLE[]="MoonCalc";
const char ROMVERSION[]="Ver 0.1";
const char ROMDATE[]=""__DATE__" "__TIME__" GMT+09:00";
const char ROMDATESHORT[]=""__DATE__" "__TIME__"";
const char ROMDATEVERYSHORT[]=""__DATE__"";
const char ROMENV[]="ARM RVCT3.1 [Build 794]";

const char OverlayHeader_ID[]="OVRC MoonShell2 OverlayCode "__DATE__" "__TIME__" GMT+09:00";
__attribute__ ((section ("OverlayHeader"))) const char OverlayHeader_ID_CanNotAccess[]="OVRC MoonShell2 OverlayCode "__DATE__" "__TIME__" GMT+09:00" "\0\0\0\0";
