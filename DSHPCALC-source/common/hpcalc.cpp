//
// ex:set ts=4 sw=4:
//
#include "woopsi.h"
#include "nds_fader.h"

#include "hpcalc.h"
#include "hpdisplay.h"
#include "hpkeyboard.h"
#include "hpsave.h"

#include "woopsi_splash.h"
#include "tristeronds.h"
#include "woopsilogo.h"

#include "architecture.h"
extern "C" {
//#include "proc_nut.h"
};

#include <fat.h>			// fatInitDefault()
#include <stdarg.h>			// va_list,etc
#include <unistd.h>			// chdir()

static HPDisplay *theDisplay;

//
// constructor
//
HPCalculator::HPCalculator()
	: WoopsiApplication(),
	_display(NULL),
	_keyboard(NULL)
{
	fatInitDefault();

	// if the /data directory exists, switch in there now
	chdir("/data");
}

//
// destructor
//
HPCalculator::~HPCalculator()
{
}

//
// real initialiser - seperate so we can throw safely
//
void HPCalculator::Startup(void)
{
	NDS_Fader fader;
	Screen *ts, *bs;

	// do whatever our superclass wants
	WoopsiApplication::Startup();

	{
		int count;
		fader.FadeToBlack(5);
		SplashScreen *ss = new SplashScreen(TristeroNDSBitmap);
		addGadget(ss);
		ss->Gadget::draw();
		fader.Unfade(5);
		for (count=0;count<8;count++) woopsiWaitVBL();
		fader.FadeToBlack(5);

		ss->setBitmap(woopsilogoBitmap);
		fader.Unfade(5);
		for (count=0;count<8;count++) woopsiWaitVBL();
		fader.FadeToBlack(5);

		ss->close();
	}

	// Create our two screens.
	ts = new Screen("Display");
	addGadget(ts);
	ts->flipToTopScreen();

	bs = new Screen("Keyboard");
	addGadget(bs);

	// now create the display and keyboard controllers
	_display = init_display(this);
	ts->addGadget(_display);

	_keyboard = init_keyboard(this);
	bs->addGadget(_keyboard);

	// initialise our stuff
	theDisplay = _display;

	// and now the CPU
	_cpu = init_cpu(_display, this);

	// create a new SRAM, giving it a pointer to the CPU
	_save = new HPSave(_cpu, savefilename());
	_save->load();

	Woopsi::registerForVBL(this);

	ts->draw();
	bs->draw();

	// and make it all visible
	fader.Unfade(5);
}

void HPCalculator::Shutdown(void)
{
	Woopsi::unregisterFromVBL(this);
	WoopsiApplication::Shutdown();
}

//
// allocate a block of memory for the emulator to use
//
void *HPCalculator::alloc(size_t size)
{
	void *p;
	p = calloc (1, size);
	if (! p) perror ("Memory allocation failed\n");
	return (p);
}

//
// update the display to reflect the current segment array
//
void HPCalculator::display_callback(struct nut_reg_t *nv)
{
	if (theDisplay) theDisplay->set_segments(nv->display_segments);
}

//
// called by the keyboard to let us know whats happening
//
void HPCalculator::press_key(int key)
{
	if (_cpu) {
		// grab display state *before* key press because pressing OFF
		int enable = _cpu->display_chip->enable;
		nut_press_key(_cpu, key);
		if (key == 0x18) {
			if (enable) {
				// display was on, lets save *now*
				_save->save();
			}
		}
	}
}

void HPCalculator::release_key(int key)
{
	if (_cpu) nut_release_key(_cpu);
}

//
// this is called on every vertical blank (1/60 sec?)
// we take this opportunity to pump the cpu
//
bool HPCalculator::vbl()
{
	if (_cpu) {
		// the OSX port of nonpareil installed a task that would be called back 30 times
		// per second.  on each call, it would:
		// a) look for a keystroke in the queue
		// b) execute a cycle
		// each cycle executes liked to execute 3839.000000 instructions per second since
		//							round(NNPR_CLOCK / NNPR_WSIZE));
		// last time - this tends to be about 128 instructions when idle
		// we assume our VBL is 60/second and thus we'll do 64 instructions - probably
		// way out of sync with reality.
/***
http://nocash.emubase.de/gbatek.htm
DS Display Dimensions / Timings
Dot clock = 5.585664 MHz (=33.513982 MHz / 6)
H-Timing: 256 dots visible, 99 dots blanking, 355 dots total (15.7343KHz)
V-Timing: 192 lines visible, 71 lines blanking, 263 lines total (59.8261 Hz)
The V-Blank cycle for the 3D Engine consists of the 23 lines, 191..213.
Screen size 62.5mm x 47.0mm (each) (256x192 pixels)
Vertical space between screens 22mm (equivalent to 90 pixels)
***/

		size_t i;
		for (i=0;i<128;i++) {
			nut_execute_instruction(_cpu);
		}
	}

	return (true);
}
