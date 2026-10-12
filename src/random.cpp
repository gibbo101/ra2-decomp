// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "random.h"

// FUNCTION: 0x0065C640
int RandomClass::operator ()(void)
{
	Seed = (Seed * MULT_CONSTANT) + ADD_CONSTANT;

	return((Seed >> THROW_AWAY_BITS) & (~((~0) << SIGNIFICANT_BITS)));
}

// FUNCTION: 0x0065C660
int RandomClass::operator() (int minval, int maxval)
{
	return(Pick_Random_Number(*this, minval, maxval));
}

// FUNCTION: 0x0065C890
Random3Class::Random3Class(unsigned seed1, unsigned seed2) :
	Seed(seed1),
	Index(seed2)
{
}

// FUNCTION: 0x0065C8B0
int Random3Class::operator() (void)
{
	int loword = Seed;
	int hiword = Index++;
	for (int i = 0; i < 4; i++) {
		int hihold  = hiword;
		int temp    = hihold ^  Mix1[i];
		int itmpl   = temp   &  0xffff;
		int itmph   = temp   >> 16;
		temp    = itmpl * itmpl + ~(itmph * itmph);
		temp    = (temp >> 16) | (temp << 16);
		hiword  = loword ^ ((temp ^ Mix2[i]) + itmpl * itmph);
		loword  = hihold;
	}
	return(hiword);
}

// FUNCTION: 0x0065C910
int Random3Class::operator() (int minval, int maxval)
{
	return(Pick_Random_Number(*this, minval, maxval));
}

// FUNCTION: 0x0065C6D0
Random2Class::Random2Class(unsigned seed) :
	Index1(0),
	Index2(103)
{
	Random3Class random(seed);

	for (int index = 0; index < TABLE_SIZE; index++) {
		Table[index] = random;
	}
	unknown_00 = false;
}

// FUNCTION: 0x0065C780
int Random2Class::operator() (void)
{
	if (unknown_00) return(0);

	Table[Index1] ^= Table[Index2];
	int val = Table[Index1];

	Index1++;
	Index2++;

	if (Index1 >= TABLE_SIZE) Index1 = 0;
	if (Index2 >= TABLE_SIZE) Index2 = 0;

	return(val);
}

// FUNCTION: 0x0065C7E0
int Random2Class::operator() (int minval, int maxval)
{
	return(Pick_Random_Number(*this, minval, maxval));
}
