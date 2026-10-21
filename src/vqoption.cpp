// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "vqoption.h"

unsigned char VQOptions[OPTION_SLOTS];

// FUNCTION: GAMEMD 0x0075A790
void Initialize_Options(void)
{
	for (int flag = 0; flag < VQ_OPTION_COUNT; flag++) {
		Clear_Option(flag);
	}
	Set_Option(OPTION_6);
}

// FUNCTION: GAMEMD 0x0075A7D0
void Set_Option(int flag)
{
	int v = (1 << (unsigned char)flag % (unsigned)OPTION_SLOTS);
	int pos = flag >> OPTION_SHIFT;
	VQOptions[pos] |= v;
}

// FUNCTION: GAMEMD 0x0075A7F0
void Clear_Option(int flag)
{
	int v = (1 << (unsigned char)flag % (unsigned)OPTION_SLOTS);
	int pos = flag >> OPTION_SHIFT;
	VQOptions[pos] &= ~v;
}

// FUNCTION: GAMEMD 0x0075A810
bool Get_Option(int flag)
{
	int v = (1 << (unsigned char)flag % (unsigned)OPTION_SLOTS);
	int pos = flag >> OPTION_SHIFT;
	if (VQOptions[pos] & v) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0075A830
int Toggle_Option(int flag)
{
	if (Get_Option(flag)) {
		Clear_Option(flag);
		return(false);
	}
	Set_Option(flag);
	return(true);
}
