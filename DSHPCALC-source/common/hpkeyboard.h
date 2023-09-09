#pragma once
#include <screen.h>
//#include <eventhandler.h>

class HPCalculator;

typedef struct {
	u16	id;
	u8	x,y,w,h;
} imagemap_entry;

class HPKeyboard :
	public Gadget
{
public:
	HPKeyboard(HPCalculator*, const u16 *, const imagemap_entry *);
	~HPKeyboard();
	void draw(Rect clipRect);
	bool click(s16 x, s16 y);
	bool drag(s16 x, s16 y, s16 vX, s16 vY);
	bool release(s16 x, s16 y);
private:
	const imagemap_entry *xytokey(s16 x, s16 y);
	const u16 *_bitmap;
	const imagemap_entry *_keymap;
	const imagemap_entry *_keydown;
	bool _inkey;
	HPCalculator *_cpu;
};

