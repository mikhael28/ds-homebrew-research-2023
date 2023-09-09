/*
 * save state of the CPU to the Nintendo's SRAM
 */
#include "nds_fatfiler.h"
#include "hpcalc.h"

class HPSave : public NDS_FatFiler
{
public:
	// constructor 
	inline HPSave(struct nut_reg_t *cpu, const char *filename)
		: NDS_FatFiler(filename), _cpu(cpu)
	{
	};

private:
	struct nut_reg_t *_cpu;

	// write a register which is an array of digit_t which is uint8
	void write_reg_t(const reg_t &r)
	{
		int i;
		for (i=0;i<WSIZE;i++) write_u8((uint8)r[i]);
	}

	void read_reg_t(reg_t &r)
	{
		int i;
		for (i=0;i<WSIZE;i++) r[i] = read_u8();
	}

	// we are being asked to write ourselves to sram
	inline void save_data(void)
	{
		size_t i;

		// write our checkmark - if this is not present, we fail to load
		write_u8('H'); 
		write_u8('P');
		write_u8('1');
		write_u8('6');

		write_reg_t(_cpu->a);
		write_reg_t(_cpu->b);
		write_reg_t(_cpu->c);
		write_reg_t(_cpu->m);
		write_reg_t(_cpu->n);

		write_u8(_cpu->g[0]);
		write_u8(_cpu->g[1]);
		write_u8(_cpu->p);
		write_u8(_cpu->q);
		write_u8(_cpu->q_sel);
		write_u8(_cpu->fo);

		for (i=0; i<SSIZE; i++) {
			write_u8(_cpu->s[i]);
		}
		write_u16(_cpu->pc);
		for (i=0;i<STACK_DEPTH;i++) {
			write_u16(_cpu->stack[i]);
		}
		write_u8(_cpu->decimal);
		write_u8(_cpu->carry);
		write_u8(_cpu->awake);
		write_u8(_cpu->pf_addr);
		write_u16(_cpu->ram_addr);

		for (i=0;i<MAX_PAGE;i++) {
			write_u8(_cpu->active_bank[i]);
		}

		// should we write _cpu->max_ram ?

		for (i = 0; i < _cpu->max_ram; i++) {
			if (_cpu->ram_exists[i]) {
				write_reg_t( _cpu->ram[i] );
			}
		}
	};

	// we are being asked to read ourselves from sram
	inline void load_data(void)
	{
		size_t i;

		if (read_u8() != 'H') return;
		if (read_u8() != 'P') return;
		if (read_u8() != '1') return;
		if (read_u8() != '6') return;

		read_reg_t(_cpu->a);
		read_reg_t(_cpu->b);
		read_reg_t(_cpu->c);
		read_reg_t(_cpu->m);
		read_reg_t(_cpu->n);

		_cpu->g[0] = read_u8();
		_cpu->g[1] = read_u8();

		_cpu->p = read_u8();
		_cpu->q = read_u8();
		_cpu->q_sel = read_u8();
		_cpu->fo = read_u8();

		for (i=0; i<SSIZE; i++) {
			_cpu->s[i] = read_u8();
		}

		_cpu->pc = read_u16();
		for (i=0;i<STACK_DEPTH;i++) {
			_cpu->stack[i] = read_u16();
		}
		_cpu->decimal = read_u8();
		_cpu->carry = read_u8();
		_cpu->awake = read_u8();
		_cpu->pf_addr = read_u8();
		_cpu->ram_addr = read_u16();

		for (i=0;i<MAX_PAGE;i++) {
			_cpu->active_bank[i] = read_u8();
		}

		for (i = 0; i < _cpu->max_ram; i++) {
			if (_cpu->ram_exists[i]) {
				read_reg_t(_cpu->ram[i]);
			}
		}
	};
};
