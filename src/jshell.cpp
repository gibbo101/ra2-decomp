// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "_mono.h"
#include "conquer.h"
#include "mono.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "coord.h"
#include "globals.h"

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

const double ONE_RAD = M_PI / 180.0;
const double ISO_TILE_SIZE = 60.0;

// SYNTHETIC: GAMEMD 0x0054AA30 SYMBOL
// _$E4
const double CELL_LEPTON_DIAG = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);
// SYNTHETIC: GAMEMD 0x0054AA60 SYMBOL
// _$E7
const double LEPTONS_PER_PIXEL = CELL_LEPTON_DIAG / ISO_TILE_SIZE;
// SYNTHETIC: GAMEMD 0x0054AA80 SYMBOL
// _$E10
const double PIXELS_PER_LEPTON = 1.0 / LEPTONS_PER_PIXEL;
// SYNTHETIC: GAMEMD 0x0054AAA0 SYMBOL
// _$E13
const double RAD_45 = ONE_RAD * 45;
// SYNTHETIC: GAMEMD 0x0054AAC0 SYMBOL
// _$E16
const double RAD_60 = ONE_RAD * 60;
// SYNTHETIC: GAMEMD 0x0054AAE0 SYMBOL
// _$E19
const double RAD_90 = ONE_RAD * 90;
// SYNTHETIC: GAMEMD 0x0054AB00 SYMBOL
// _$E22
const int LEVEL_LEPTON_H = (int)(fastmath::tan(RAD_90 - RAD_60) * CELL_LEPTON_DIAG * 0.5);
// SYNTHETIC: GAMEMD 0x0054AB40 SYMBOL
// _$E25
const double CELL_SLOPE_ANGLE = fastmath::atan(LEVEL_LEPTON_H * (1 / (double)CELL_LEPTON_H));
// SYNTHETIC: GAMEMD 0x0054AB70 SYMBOL
// _$E28
const double CELL_DIAG_SLOPE_ANGLE = fastmath::atan((LEVEL_LEPTON_H * 2) / CELL_LEPTON_DIAG);
// SYNTHETIC: GAMEMD 0x0054ABA0 SYMBOL
// _$E31
const int BRIDGE_LEPTON_HEIGHT = (int)((LEVEL_LEPTON_H * BRIDGE_CELL_HEIGHT) + 0.5);
// SYNTHETIC: GAMEMD 0x0054ABD0 SYMBOL
// _$E34
Cell const CELL_NONE(0,0);

struct Triple {
	int X, Y, Z;
	Triple() : X(0), Y(0), Z(0) {}
	Triple(int x, int y, int z) : X(x), Y(y), Z(z) {}
};
struct Quad {
	int A, B, C, D;
	Quad() : A(0), B(0), C(0), D(0) {}
};

// SYNTHETIC: GAMEMD 0x0054ABE0 SYMBOL
// _$E37
Triple DAT_00ABC5A8;

// SYNTHETIC: GAMEMD 0x0054AC00 SYMBOL
// _$E40
Triple DAT_00ABC5D0(128, 128, 0);

// SYNTHETIC: GAMEMD 0x0054AC20 SYMBOL
// _$E43
Quad DAT_00ABC578;
