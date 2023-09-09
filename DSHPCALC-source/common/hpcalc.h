#pragma once
#include "woopsi_application.h"

// forwards to allow class definition without excessive #includes
class Screen;
class HPDisplay;
class HPKeyboard;
class HPSave;

extern "C" {
#include "proc_nut.h"
};

class HPCalculator :
	public WoopsiApplication
{
public:
	// object construction
	HPCalculator();
	virtual ~HPCalculator();

	// called at startup
	void Startup(void);

	// called at shutdown
	void Shutdown(void);

	// memory allocation vectors
	static void *alloc(size_t sz);

	// display updator
	static void display_callback(struct nut_reg_t *nv);

	// this gets called to service the CPU
	bool vbl();

	// the keyboard communicates with us via these
	void press_key(int k);
	void release_key(int k);

	// subclasses must implement these
	virtual HPDisplay *init_display( HPCalculator *calc ) = 0;
	virtual HPKeyboard *init_keyboard( HPCalculator *calc ) = 0;
	virtual const char *savefilename(void) = 0;
	virtual struct nut_reg_t *init_cpu( HPDisplay *d, HPCalculator *calc ) = 0;

protected:
	HPDisplay *_display;
	HPKeyboard *_keyboard;
	HPSave *_save;
	nut_reg_t *_cpu;
};
