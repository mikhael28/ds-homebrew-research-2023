#pragma once

// superclasses
#include "hpdisplay.h"
#include <screen.h>

// bitmaps
#include "hp11cscreen.h"

class HP11CDisplay : public HPDisplay
{
public:
	HP11CDisplay() : HPDisplay(hp11cscreenBitmap)
	{
	}
};
