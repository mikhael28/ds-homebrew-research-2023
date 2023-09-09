#pragma once

// superclasses
#include "hpdisplay.h"
#include <screen.h>

// bitmaps
#include "hp16cscreen.h"

class HP16CDisplay : public HPDisplay
{
public:
	HP16CDisplay() : HPDisplay(hp16cscreenBitmap)
	{
	}
};
