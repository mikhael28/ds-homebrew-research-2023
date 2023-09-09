/*
 * helper for fading the screen in/out
 *
 * handles SDL mode by nopping itself out
 */
#pragma once

#if !defined(_SDL_H)
#include <nds/arm9/video.h>
#endif

#include <woopsifuncs.h>		// for woopsiWaitVBL()

class NDS_Fader
{
private:
	int _faded;

public:
	// construction
	inline NDS_Fader()
	{
		_faded = 0;
	};
	
	// in our destructor, we make sure we faded back to normal
	// as it would be a bad thing to leave the screen blanked out
	// completely
	inline ~NDS_Fader()
	{
		if (_faded) {
			_set_registers(0);
		}
	};

	inline void _set_registers(int mode)
	{
#if !defined(_SDL_H)
		BRIGHTNESS =		mode;
		SUB_BRIGHTNESS =	mode;
#endif
	}

	// take 'ticks' vbls to fade to black
	inline void FadeToBlack(int ticks)
	{
		_faded = 2<<14;

		// BUG: compute appropriate deltas so we take 'ticks'
		int count = 0;
		while (count < 32) {
			_set_registers(_faded | count);
			count++;
			woopsiWaitVBL();
			woopsiWaitVBL();
		}
	};

	// take 'ticks' vbls to fade to white
	inline void FadeToWhite(int ticks)
	{
		_faded = 1<<14;
		// BUG: compute appropriate deltas so we take 'ticks'
		int count = 0;
		while (count < 32) {
			_set_registers(_faded | count);
			count++;
			woopsiWaitVBL();
			woopsiWaitVBL();
		}
	};

	// unfade (from wherever we are) taking ticks vbls to do it
	inline void Unfade(int ticks)
	{
		// BUG: compute appropriate deltas so we take 'ticks'
		int count = 31;
		while (count) {
			if (_faded) _set_registers(_faded | count);
			count--;
			woopsiWaitVBL();
			woopsiWaitVBL();
		}
	
		// one last cycle to ensure we are at normal visibility
		_faded = 0;
		_set_registers(0);
	};
};
