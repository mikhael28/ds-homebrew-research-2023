//
// ex:set ts=4 sw=4:
//

#include "hp16c.h"
#include "hp16cdisplay.h"
#include "hp16ckeyboard.h"
#include "hp16crom.h"

#define NNPR_OBJ        "16c"
#define NNPR_RAM        40
#define NNPR_CLOCK      215000.0
#define NNPR_WSIZE      56.0

//
// constructor
//
HP16C::HP16C(void) : HPCalculator()
{
}

HPDisplay *HP16C::init_display(
	HPCalculator *calc )
{
	return new HP16CDisplay();
}

HPKeyboard *HP16C::init_keyboard(
	HPCalculator *calc )
{
	return new HP16CKeyboard(dynamic_cast<HP16C*>(calc));
}
	

struct nut_reg_t *HP16C::init_cpu(
	HPDisplay *d,
	HPCalculator *calc )
{
	struct nut_reg_t *cpu = nut_new_processor(NNPR_RAM, (void *)d);
	nut_read_object_file(cpu, hp16crom, sizeof(hp16crom)/sizeof(hp16crom[0]));
	return cpu;
}

const char *HP16C::savefilename(void) 
{
	return "hp16c.ram";
}
