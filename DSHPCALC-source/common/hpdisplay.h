#pragma once
#include <screen.h>

#include "display.h"

class GraphicsPort;

class HPDisplay :
	public Gadget
{
public:
	HPDisplay(const u16 *bitmap);
	~HPDisplay();
	void draw(Rect clipRect);
	void draw_digit(GraphicsPort * port, segment_bitmap_t s, int x, int y);
	void draw_indicator(GraphicsPort * port, int srcx, int dstx, int w);
	void set_segments(const segment_bitmap_t *segs);

private:
	bool _dirty;
	const u16 *_bitmap;
	segment_bitmap_t _segs[MAX_DIGIT_POSITION];
};
