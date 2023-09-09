#pragma once

// superclasses
#include "hpkeyboard.h"

// forwards
class HPCalculator;
class Screen;

class HP16CKeyboard : public HPKeyboard
{
public:
	HP16CKeyboard(HPCalculator*);
	~HP16CKeyboard();
};

