#include "hp16c.h"
#include "hp16ckeyboard.h"
#include "hp16ckeys.h"

// image map is hard-wired to image at present
static const imagemap_entry imagemap[] = {

	{ 0x13, 2,  16, 22, 27 },		// A
	{ 0x33, 27, 16, 22, 27 },		// B
	{ 0x73, 54, 16, 22, 27 },		// C
	{ 0xC3, 80, 16, 22, 27 },		// D
	{ 0x83, 106,16, 22, 27 },		// E
	{ 0x82, 132,16, 22, 27 },		// F
	{ 0xC2, 158,16, 22, 27 },		// 7
	{ 0x72, 183,16, 22, 27 },		// 8
	{ 0x32, 208,16, 22, 27 },		// 9
	{ 0x12, 233,16, 22, 27 },		// DIVIDE

	{ 0x10, 2,  63, 22, 27 },		// GSB
	{ 0x30, 27, 63, 22, 27 },		// GTO
	{ 0x70, 54, 63, 22, 27 },		// HEX
	{ 0xC0, 80, 63, 22, 27 },		// DEC
	{ 0x80, 106,63, 22, 27 },		// OCT
	{ 0x87, 132,63, 22, 27 },		// BIN
	{ 0xC7, 158,63, 22, 27 },		// 4
	{ 0x77, 183,63, 22, 27 },		// 5
	{ 0x37, 208,63, 22, 27 },		// 6
	{ 0x17, 233,63, 22, 27 },		// TIMES

	{ 0x11,  2, 116, 22, 27 },		// R/S
	{ 0x31, 27, 116, 22, 27 },		// SST
	{ 0x71, 54, 116, 22, 27 },		// ROLLDOWN
	{ 0xC1, 80, 116, 22, 27 },		// X><Y
	{ 0x81, 106,116, 22, 27 },		// BSP

	{ 0x84, 131,116, 22, 75 },		// ENTER

	{ 0xC4, 158,116, 22, 27 },		// 1
	{ 0x74, 183,116, 22, 27 },		// 2
	{ 0x34, 208,116, 22, 27 },		// 3
	{ 0x14, 233,116, 22, 27 },		// MINUS

	{ 0x18, 2,  162, 22, 27 },		// ON
	{ 0x38, 27, 162, 22, 27 },		// f
	{ 0x78, 54, 162, 22, 27 },		// g
	{ 0xC8, 80, 162, 22, 27 },		// STO
	{ 0x88, 106,162, 22, 27 },		// RCL

	{ 0xC5, 158,162, 22, 27 },		// 0
	{ 0x75, 183,162, 22, 27 },		// .
	{ 0x35, 208,162, 22, 27 },		// CHS
	{ 0x15, 233,162, 22, 27 },		// PLUS

	// sentinel
	{ 0,0,0,0,0 }
};

//
// constructor
//
HP16CKeyboard::HP16CKeyboard(HPCalculator *cpu)
	: HPKeyboard(cpu,hp16ckeysBitmap,imagemap)
{
}

//
// destructor
//
HP16CKeyboard::~HP16CKeyboard()
{
}
