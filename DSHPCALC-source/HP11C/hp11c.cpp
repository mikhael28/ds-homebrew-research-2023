//
// ex:set ts=4 sw=4:
//

#include "hp11c.h"
#include "hp11cdisplay.h"
#include "hp11ckeyboard.h"
#include "hp11crom.h"

#define NNPR_OBJ        "11c"
#define NNPR_RAM        40
#define NNPR_CLOCK      215000.0
#define NNPR_WSIZE      56.0

//
// constructor
//
HP11C::HP11C(void) : HPCalculator()
{
}

HPDisplay *HP11C::init_display(
	HPCalculator *calc )
{
	return new HP11CDisplay();
}

HPKeyboard *HP11C::init_keyboard(
	HPCalculator *calc )
{
	return new HP11CKeyboard(dynamic_cast<HP11C*>(calc));
}
	

struct nut_reg_t *HP11C::init_cpu(
	HPDisplay *d,
	HPCalculator *calc )
{
	struct nut_reg_t *cpu = nut_new_processor(NNPR_RAM, (void *)d);
	nut_read_object_file(cpu, hp11crom, sizeof(hp11crom)/sizeof(hp11crom[0]));
	return cpu;
}

const char *HP11C::savefilename(void) 
{
	return "hp11c.ram";
}
