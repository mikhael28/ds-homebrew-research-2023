#include "woopsi_splash.h"
#include <graphicsport.h>

//
// constructor
//
SplashScreen::SplashScreen(const unsigned short *bm)
	: Screen("splashscreen")
{
	_bm = bm;
}

//
// destructor
//
SplashScreen::~SplashScreen()
{
}

void SplashScreen::setBitmap(const unsigned short *bm)
{
	_bm = bm;
	Gadget::draw();
}

// Draw the screen
void SplashScreen::draw(Rect clipRect)
{
	GraphicsPort *port = newInternalGraphicsPort(clipRect);
	if (port) {
		port->drawBitmap(
			0, 0,			// x,y
			256, 192,		// width, height
			_bm,			// bitmap
			0, 0,			// x,y
			256, 192);		// width, height

		delete port;
	}
}
