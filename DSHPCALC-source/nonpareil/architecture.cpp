/*
 * This file was never part of baseline nonpareil - it is an implementation
 * of the mandatory architecture services that nonpareil exports ports to provide.
 */
#include "hpcalc.h"
#include "architecture.h"
#include <stdarg.h>

//
// C-bridges across to C++
//
void *a_alloc (size_t size)
{
	return HPCalculator::alloc(size);
}

//
// update the display to match the segments in the CPU
//
void a_display(struct nut_reg_t *nv)
{
	return HPCalculator::display_callback(nv);
}

//
// general text printer - debug messages only
//
void a_printf(const char *fmt, ...)
{
	char buff[1024];
	va_list args;
	va_start(args,fmt);
	vsnprintf(buff,sizeof(buff)-1,fmt,args);
	va_end(args);
	buff[sizeof(buff)-1] = 0;
//	Debug::printf("%s",buff);
}
