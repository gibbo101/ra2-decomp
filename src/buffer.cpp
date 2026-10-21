// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "buffer.h"
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E3E78;
extern const double DAT_007E3E80;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct Int3Init {
	int A, B, C;
	Int3Init() { C = 0; A = 0x80; B = 0x80; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// FUNCTION: 0x0043AEB0
BufferClass::BufferClass(void *buffer, int size)
{
	Size			= size;

	if (buffer) {
		Buffer			= (unsigned char *)buffer;
		Allocated		= false;
	} else {
		Buffer			= new unsigned char[Size];
		Allocated		= true;
	}
}

// FUNCTION: 0x0043AEF0
BufferClass::BufferClass(int size)
{
	Size				= size;
	Buffer			= new unsigned char[Size];
	Allocated		= true;
}

// FUNCTION: 0x0043AF10
BufferClass::BufferClass(void)
{
	Buffer		= 0;
	Size			= 0;
	Allocated	= false;
}

// FUNCTION: 0x0043AF20
BufferClass::~BufferClass(void)
{
	if (Allocated) {
		delete[] Buffer;
	}
}

// FUNCTION: 0x0043AF40 $E2
double DAT_0089C820 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0043AF70 $E5
double DAT_0089C880 = DAT_0089C820 / DAT_007E3E78;

// FUNCTION: 0x0043AF90 $E8
double DAT_0089C868 = DAT_007E1718 / DAT_0089C880;

// FUNCTION: 0x0043AFB0 $E11
double DAT_0089C860 = DAT_007E1720 * DAT_007E3E80;

// FUNCTION: 0x0043AFD0 $E14
double DAT_0089C830 = DAT_007E1728 * DAT_007E3E80;

// FUNCTION: 0x0043AFF0 $E17
double DAT_0089C7F8 = DAT_007E1730 * DAT_007E3E80;

// FUNCTION: 0x0043B010 $E20
int DAT_0089C888 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089C7F8 - DAT_0089C830) * DAT_0089C820));

// FUNCTION: 0x0043B050 $E23
double DAT_0089C840 = FUN_004CADE0(DAT_0089C888 * DAT_007E1740);

// FUNCTION: 0x0043B080 $E26
double DAT_0089C858 = FUN_004CADE0((DAT_0089C888 * 2) / DAT_0089C820);

// FUNCTION: 0x0043B0B0 $E29
int DAT_0089C87C = (int)(DAT_0089C888 * 4 + DAT_007E1738);

// FUNCTION: 0x0043B0E0 $E32
ShortPair DAT_0089C818;

// FUNCTION: 0x0043B0F0 $E35
IntTriple DAT_0089C848;

// FUNCTION: 0x0043B110 $E38
Int3Init DAT_0089C870;

// FUNCTION: 0x0043B130 $E41
IntQuad DAT_0089C808;
