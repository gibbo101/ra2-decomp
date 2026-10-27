// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "_mono.h"
#include "conquer.h"
#include "mono.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "coord.h"
#include "globals.h"

// SYNTHETIC: GAMEMD 0x0054A8A0 SYMBOL
// _$E2

struct DebugPage;
DebugPage * __fastcall FUN_005BC450(int index);
void __cdecl FUN_005BD1F0(DebugPage * page, char const * format, ...);
void __fastcall FUN_006BEC50(int code);

// FUNCTION: GAMEMD 0x0054A8C0
void __cdecl Fatal(char const * message, ...)
{
	va_list	va;

	va_start(va, message);
	Emergency_Exit();
	vfprintf(stderr, message, va);
	FUN_005BD1F0(FUN_005BC450(4), message);
	FUN_006BEC50(EXIT_FAILURE);
}

// FUNCTION: GAMEMD 0x0054A900
void Set_Bit(void * array, int bit, int value)
{
	unsigned char mask = (unsigned char)(1 << (bit % 8));

	if (value != 0) {
		*((unsigned char *)array + (bit/8)) |= mask;
	} else {
		*((unsigned char *)array + (bit/8)) &= (unsigned char)~mask;
	}
}

// FUNCTION: GAMEMD 0x0054A950
int Get_Bit(void const * array, int bit)
{
	unsigned char mask = (unsigned char)(1 << (bit % 8));
	return((*((unsigned char *)array + (bit/8)) & mask) != 0);
}

// FUNCTION: GAMEMD 0x0054A990
int First_True_Bit(void const * array)
{
	int index = 0;
	while (*((unsigned char *)array) == 0) {
		index++;
		array = ((char*)array) + 1;
	}
	int subindex;
	for (subindex = 0; subindex < 8; subindex++) {
		if (Get_Bit(array, subindex)) break;
	}
	return(index * 8 + subindex);
}

// FUNCTION: GAMEMD 0x0054A9E0
int First_False_Bit(void const * array)
{
	int index = 0;
	while (*((unsigned char *)array) == 0xFF) {
		index++;
		array = ((char*)array) + 1;
	}
	int subindex;
	for (subindex = 0; subindex < 8; subindex++) {
		if (!Get_Bit(array, subindex)) break;
	}
	return(index * 8 + subindex);
}
