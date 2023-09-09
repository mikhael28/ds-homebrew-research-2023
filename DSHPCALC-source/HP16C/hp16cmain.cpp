#include "hp16c.h"

int main(int argc, char *argv[])
{
	// Create woopsi application
	HP16C theApp;
	theApp.Startup();		// start it up
	theApp.draw();			// ensure physical screen is up to date
	theApp.RunLoop();		// let the event loop run
	theApp.Shutdown();		// and we are done

	return 0;
}
