/*
 * helper for saving/restoring persistent state - don't use it for structured files,
 * but it should be fine for things like preferences...
 *
 * load() and save() will call the object back on load_data() and save_data() respectively.  These
 * routines are expected to call appropriate read_xxx() and write_xxx() routines to move
 * fundamental data types to/from persistant storage, whatever that may be.
 */
#pragma once

class NDS_Filer
{
private:
public:
	// construction
	inline NDS_Filer() { }
	virtual inline ~NDS_Filer() { }

	// entry point - load object 
	inline void load(void) {
		if (prepare_load()) {
			load_data();
			cleanup_load();
		}
	};

	// entry point 
	inline void save(void) {
		if (prepare_save()) {
			save_data();
			cleanup_save();
		}
	};

protected:
	//
	// application subclass must implement load/save logic
	//
	virtual void load_data(void) = 0;
	virtual void save_data(void) = 0;

	//
	// media subclasses must implement single byte read/write
	//
	virtual void write_u8(uint8 value) = 0;
	virtual uint32 read_u8(void) = 0;

	//
	// media subclasses may implement these
	//
	virtual bool prepare_load() { return true;}
	virtual void cleanup_load() {}
	virtual bool prepare_save() { return true;}
	virtual void cleanup_save() {}

	//
	// we handle mapping complex data types onto single byte i/o
	//
	inline void write_u16(uint16 value) {
		write_u8(value & 0xFF);
		write_u8((value>>8) & 0xFF);
	};

	inline void write_u32(uint32 value) {
		write_u8(value & 0xFF);
		write_u8((value>>8) & 0xFF);
		write_u8((value>>16) & 0xFF);
		write_u8((value>>24) & 0xFF);
	};

	inline void write_bytes(const unsigned char *bytes, size_t len) {
		if (len) {
			do {
				write_u8(*bytes++);
			} while (len-- > 0);
		}
	};

	inline void write_cstring(const char *s) {
		size_t len = strlen(s);
		// write the number of significant bytes, then the bytes
		// themselves - note, we don't write the \0
		write_u32(len);
		write_bytes((uint8*)s,len);
	};

	inline uint16 read_u16(void) {
		uint16 v = read_u8();
		return v | read_u8();
	};

	inline uint32 read_u32(void) {
		uint32 v;
		v  = read_u8();
		v |= read_u8() << 8;
		v |= read_u8() << 16;
		v |= read_u8() << 24;
		return v;
	};

	inline void read_bytes(unsigned char *bytes, size_t len) {
		if (len) {
			do {
				*bytes++ = read_u8();
			} while (len-- > 0);
		}
	};

	inline uint32 read_cstring(char *buff, uint32 sz) {
		uint32 len;
		// read the number of characters written last time
		len = read_u32();

		// catch completely degenerate buffer
		if (sz) {
			// reserved space for the null
			sz--;
			// check if its too big for this buffer
			if (len > sz) {
				// and truncate
				size_t excess = len - sz;
				read_bytes((uint8*)buff,sz);
				buff[sz] = 0;
				while (excess--) read_u8();
			} else {
				// read the whole thing
				read_bytes((uint8*)buff,len);
				buff[len] = 0;
			}
		}
		return(len);
	};
};
