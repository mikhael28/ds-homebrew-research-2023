#pragma once
#include <Woopsi.h>
#include <woopsifuncs.h>

class WoopsiApplication : public Woopsi
{
public:

	// object construction
	inline WoopsiApplication()
	{
		initWoopsiGfxMode();
	}

	virtual inline ~WoopsiApplication()
	{
	}

	// called once at application startup 
	virtual inline void Startup(void) {}

	// main application loop - runs forever, or until somethinng
	// makes it stop
	virtual inline void RunLoop(void) {
		// Infinite loop to keep the program running
		while (1) {
			ProcessOneVBL();
		}
	}

	// single event loop - allows modal screens to pump messages without
	// giving up complete control
	virtual inline void ProcessOneVBL(void) {
		this->play();
		woopsiWaitVBL();
	}

	// called once at application shutdown
	virtual inline void Shutdown(void)
	{
	}
};
