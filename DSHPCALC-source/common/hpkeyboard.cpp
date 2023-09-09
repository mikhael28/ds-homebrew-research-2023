#include "hpkeyboard.h"
#include "hpcalc.h"
#include <graphicsport.h>

//
// constructor
//
HPKeyboard::HPKeyboard(
	HPCalculator *cpu,
	const u16 *bitmap,
	const imagemap_entry *keymap)
	: Gadget(0,0,256,192,GADGET_BORDERLESS)
{
	_keydown = 0;		// populated during drag()
	_inkey = false;
	_cpu = cpu;
	_keymap = keymap;
	_bitmap = bitmap; 
}

//
// destructor
//
HPKeyboard::~HPKeyboard()
{
}

// Draw the keyboard
void HPKeyboard::draw(Rect clipRect)
{
	GraphicsPort *port = newInternalGraphicsPort(clipRect);
	if (port) {
		port->drawBitmap(
			0, 0,			// x,y
			256, 192,		// width, height
			_bitmap,		// bitmap
			0, 0,			// x,y
			256, 192);		// width, height

		// if a button is down, draw it
		if (_inkey) {
			const imagemap_entry *i = _keydown;
			u16 outline = woopsiRGB(8,8,8);
			// would like to inverse the rectangle here but thats expensive
			// so for the time being we draw a ring instead
			port->drawRect( i->x,	i->y,	i->w,	i->h,	outline);
			port->drawRect( i->x+1, i->y+1, i->w-2, i->h-2, outline);
		}
		delete port;
	}
}

const imagemap_entry *HPKeyboard::xytokey(s16 x, s16 y)
{
	const imagemap_entry *i = _keymap;
	while (i->id) {
		if (
			x >= i->x
			&&
			x <= i->x + i->w
			&&
			y >= i->y
			&&
			y <= i->y + i->h
		) {
			return i;
		}
		i++;
	}
	return 0;
}

bool HPKeyboard::click(s16 x, s16 y)
{
	bool clicked = Gadget::click(x,y);
	if (clicked) {
		const imagemap_entry *k = xytokey(x,y);
		if (k) {
			_keydown = k;
			_cpu->press_key(_keydown->id);
			setDragging(x,y);
		}
	}
	return clicked;
}

// this gets called as the stylus is dragged around the screen.  we
// watch for enter/exit events
bool HPKeyboard::drag(s16 x, s16 y, s16 vX, s16 vY)
{
	const imagemap_entry *k = xytokey(x,y);
	if (_keydown == k) {
		_inkey = _keydown != 0;
	} else {
		_inkey = 0;
	}
	Gadget::draw();
	return true;
}

bool HPKeyboard::release(s16 x, s16 y)
{
	bool released = Gadget::release(x,y);
	_cpu->release_key(-1);
	if (released) {
		if (_inkey) _inkey = 0;
		Gadget::draw();
	}
	return released;
}
