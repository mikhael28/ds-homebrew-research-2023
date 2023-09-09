#pragma once

// superclasses
#include "hpkeyboard.h"

// forwards
class HPCalculator;
class Screen;

class HP11CKeyboard : public HPKeyboard
{
public:
	HP11CKeyboard(HPCalculator*);
	~HP11CKeyboard();
};

