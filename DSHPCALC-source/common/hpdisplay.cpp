#include "hpcalc.h"
#include "hpdisplay.h"
#include <graphicsport.h>

#if !defined(_SDL_H)
#include <nds/arm9/cache.h>
#endif

	// This is a 16-bit bitmap which is 18 pixels wide, and 30 pixels tall.  It contains
	// the complete LCD array
#include "lcd_all.h"

	// These are 8-bit bitmaps that have been RLE encoded.  They were originally the
	// same as lcd_all (18x30) but are now packed into pairs of the form (len,value)
#include "segment_a.h"
#include "segment_b.h"
#include "segment_c.h"
#include "segment_d.h"
#include "segment_e.h"
#include "segment_f.h"
#include "segment_g.h"
#include "segment_h.h"
#include "segment_i.h"

	// our indicators are all in the bitmap 'hpindicators', which 
	// we *know* to be 129x11 16b pixels - though 'git' splays them
	// into 130x11
#include "hpindicators.h"
#define INDICATORBMHEIGHT	11
#define	INDICATORBMWIDTH	130

//
// constructor
//
HPDisplay::HPDisplay(const u16 *bitmap)
	: Gadget(0,0,256,192,GADGET_BORDERLESS),
	  _bitmap(bitmap)
{
	// reset what we think is on the screen
	for (int i=0;i<MAX_DIGIT_POSITION;i++) _segs[i] = 0;
}

//
// destructor
//
HPDisplay::~HPDisplay()
{
}

//
// Draw the screen
//
void HPDisplay::draw(Rect clipRect)
{
	GraphicsPort *port = newInternalGraphicsPort(clipRect);
	if (port) {
		// first do the background
		port->drawBitmap(
			0, 0,			// x,y
			256, 192,		// width, height
			_bitmap,		// bitmap
			0, 0,			// x,y
			256, 192);		// width, height

		// then the digits
		for (int i=0;i<11;i++) {
			draw_digit(port, _segs[i], 19+20*i, 138);
		}

		// and the indicators - these are scattered through the digits
		if (_segs[1] & 1<<17) draw_indicator(port,0, 18, 7);	// *
		if (_segs[2] & 1<<17) draw_indicator(port,7, 43, 21);	// USER
		if (_segs[3] & 1<<17) draw_indicator(port,28,76, 8);	// f
		if (_segs[4] & 1<<17) draw_indicator(port,35,90, 6);	// g
		if (_segs[5] & 1<<17) draw_indicator(port,41,105, 24);	// BEGIN
		if (_segs[6] & 1<<17) draw_indicator(port,65,150, 5);	// G
		if (_segs[7] & 1<<17) draw_indicator(port,71,156, 15);	// RAD
		if (_segs[8] & 1<<17) draw_indicator(port,86,172, 18);	// D.MY
		if (_segs[9] & 1<<17) draw_indicator(port,104,199, 6);	// C
		if (_segs[10]& 1<<17) draw_indicator(port,110,220, 19);	// PRGM

		delete port;
	}
}

//
// draw an indicator - we know they are all on the same line, and are the same height,
// all we need are the offset into the SOURCE, the offset into the DESTINATION and
// the width in pixels
//
void HPDisplay::draw_indicator(GraphicsPort * port, int srcx, int dstx, int w)
{
	// unfortunately, Woopsi doesn't have a function to draw a portion of a bitmap
	// so we construct another copy and blit that - because calling alloc() and
	// free() seems like a bad thing, we allocate a buffer thats big enough
	// for all possible indicators - that and DMA hates stack frames
	static u16 temp[24*INDICATORBMHEIGHT];

	u16 *src = ((u16*)hpindicatorsBitmap)+srcx;
	u16 *dst = temp;
	for (int row=0;row<INDICATORBMHEIGHT;row++) {
		while (DMA_Active());
		DMA_Copy(src, dst, w, DMA_16NOW);
		src += INDICATORBMWIDTH;
		dst += w;
	}

	// might need to flush because drawBitmap uses dma
#if !defined(_SDL_H)
	DC_FlushRange(temp,24*INDICATORBMHEIGHT*sizeof(temp[0]));
#endif
	port->drawBitmap(
		dstx,172,		// coords of destination rectangle (in graphic port)
		w,INDICATORBMHEIGHT,	// width, height of info to copy
		temp,			// bitmap we just created
		0, 0,			// its origin
		w,INDICATORBMHEIGHT );	// and size
}

static void copy_masked_rle(
	u16 *bits,
	const u16 *master,
	const u8  *rle )
{
	// the RLE data consists of a set of (n,value) pairs where
	// n indicates the number of value's appear in the data.  the end of
	// the data is marked by an entry with n==0
	while (*rle) {
		int n = *rle++;
		int v = *rle++;
		if (v) {
			while (DMA_Active());
			DMA_Copy((u16*)master,bits,n,DMA_16NOW);
		}
		bits += n;
		master += n;
	}
}

void HPDisplay::draw_digit(GraphicsPort * port, segment_bitmap_t s, int x, int y)
{
	// this must be statically allocated - if you use malloc(), it appears
	// to return memory that isn't quite compatible with drawBitmap() and
	// memory on the stack frame is right out...
	static u16 _digit[18*30];

	// background colour is the top-left pixel of lcd_all
	u16 bg = lcd_allBitmap[0];

	// allocate a new bitmap that we can draw into,
	// initialise to our background colour???
	for (int i=0;i<18*30;i++) _digit[i] = bg;

	if (s & 0x001)	copy_masked_rle( _digit, lcd_allBitmap, segment_a_rle );
	if (s & 0x002)	copy_masked_rle( _digit, lcd_allBitmap, segment_b_rle );
	if (s & 0x004)	copy_masked_rle( _digit, lcd_allBitmap, segment_c_rle );
	if (s & 0x008)	copy_masked_rle( _digit, lcd_allBitmap, segment_d_rle );
	if (s & 0x010)	copy_masked_rle( _digit, lcd_allBitmap, segment_e_rle );
	if (s & 0x020)	copy_masked_rle( _digit, lcd_allBitmap, segment_f_rle );
	if (s & 0x040)	copy_masked_rle( _digit, lcd_allBitmap, segment_g_rle );
	if (s & 0x080)	copy_masked_rle( _digit, lcd_allBitmap, segment_h_rle );
	if (s & 0x100)	copy_masked_rle( _digit, lcd_allBitmap, segment_i_rle );

	// flush all changes to the temporary bitmap so dma will work
#if !defined(_SDL_H)
	DC_FlushRange(_digit,18*30*sizeof(_digit[0]));
#endif
	port->drawBitmap(
		x, y, 18, 30,	// x, y, w, h
		_digit,		// bitmap
		0, 0, 18, 30 );
}

void HPDisplay::set_segments(const segment_bitmap_t *segs)
{
	int i;
	_dirty = 0;
	for (i=0;i<MAX_DIGIT_POSITION;i++) {
		if (segs[i] != _segs[i]) {
			_dirty = 1;
			_segs[i] = segs[i];
		}
	}

	if (_dirty) {
		// could just clip to the digit space ?
		Gadget::draw();
	}
}
