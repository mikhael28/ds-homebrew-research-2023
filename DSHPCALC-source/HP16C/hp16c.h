#pragma once

#include "hpcalc.h"

class HP16C : public HPCalculator
{
public:
	// object construction
	HP16C();

	// mandatory methods we must implement
	HPDisplay *init_display( HPCalculator *calc );
	HPKeyboard *init_keyboard( HPCalculator *calc );
	struct nut_reg_t *init_cpu( HPDisplay *d, HPCalculator *calc );
	const char *savefilename(void);
};
