/*
 * helper for saving/restoring persistent state to disk file via libfat
 */
#pragma once

#include "nds_filer.h"

/* only need libfat if we aren't in SDL mode */
#if !defined(_SDL_H)
#include <fat.h>
#endif

/* in either case, we need stdio */
#include <stdio.h>

class NDS_FatFiler : public NDS_Filer
{
private:
	const char *_filename;
	FILE *_fp;
public:
	// construction
	inline NDS_FatFiler(const char *name) {
		_filename = name;
		_fp = NULL;
	}
	virtual inline ~NDS_FatFiler() {}

protected:
	virtual bool prepare_load(void) {
		_fp = fopen(_filename,"rb");
		return (_fp != NULL);
	}

	virtual void cleanup_load(void) {
		if (_fp) fclose(_fp);
	}

	virtual bool prepare_save(void) {
		_fp = fopen(_filename,"wb");
		return (_fp != NULL);
	}

	virtual void cleanup_save(void) {
		if (_fp) {
			fflush(_fp);
			fclose(_fp);
		}
	}

	void write_u8(uint8 value) {
		fputc(value,_fp);
	}

	inline uint32 read_u8(void) {
		uint32 v = fgetc(_fp);
		return v;
	}
};

