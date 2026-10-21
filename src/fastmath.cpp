// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "fastmath.h"
#include "dbgprint.h"
#include "win.h"

#define MOST_SIG_OFFSET 0
#define SQRT_BITS 13
#define EXP_BIAS   127
#define EXP_SHIFTS 23
#define EXP_LSB	(1<<EXP_SHIFTS)
#define MANT_MASK  (EXP_LSB-1)
#define MANT_SHIFTS (23-SQRT_BITS)
#define SIN_TABLE_SIZE   8192
#define SIN_TABLE_EXTENT (SIN_TABLE_SIZE + SIN_TABLE_SIZE / 4)
#define TAN_TABLE_SIZE   4096
#define ASIN_TABLE_SIZE  4096
#define ATAN_TABLE_SIZE  4096

extern "C" {
	extern float sin_tab[];
	extern float asin_tab[];
	extern float tan_tab[];
	extern float atan_tab[];
	extern int sqrt_tab[];
}

static void Sabotage_Routine(LPVOID funcptr)
{
	MEMORY_BASIC_INFORMATION info;
	memset(&info, 0, sizeof(info));

	DWORD * numptr = (DWORD*)funcptr;
	if (VirtualQuery(numptr, &info, sizeof(info))) {
		PVOID addr = info.BaseAddress;
		SIZE_T regsize = info.RegionSize;
		DWORD flOldProtect = 0;
		if (!VirtualProtect(addr, regsize, PAGE_WRITECOPY, &flOldProtect)) {
			DebugString("VirtualProtect failed with error code %d - %s\n", GetLastError(), Last_Error_Text());
			return;
		}

		*numptr = 0;
		if (!VirtualProtect(addr, regsize, PAGE_EXECUTE, &flOldProtect)) {
			DebugString("VirtualProtect failed with error code %d - %s\n", GetLastError(), Last_Error_Text());
		}
		FlushInstructionCache(GetCurrentProcess(), 0, 0);
	}
}

// FUNCTION: GAMEMD 0x004CAC40
double __cdecl fastmath::sqrt(double f)
{
	return((float)(fastmath::sqrtf(f)));
}

// FUNCTION: GAMEMD 0x004CACB0
double __cdecl fastmath::sin(double f)
{
	return((float)(fastmath::sinf(f)));
}

// FUNCTION: GAMEMD 0x004CAD00
double __cdecl fastmath::cos(double f)
{
	return((float)(fastmath::cosf(f)));
}

// FUNCTION: GAMEMD 0x004CAD50
double __cdecl fastmath::tan(double f)
{
	return((float)(fastmath::tanf(f)));
}

// FUNCTION: GAMEMD 0x004CAD80
double __cdecl fastmath::asin(double f)
{
	return((float)(fastmath::asinf(f)));
}

// FUNCTION: GAMEMD 0x004CADB0
double __cdecl fastmath::acos(double f)
{
	return((float)(fastmath::acosf(f)));
}

// FUNCTION: GAMEMD 0x004CADE0
double __cdecl fastmath::atan(double f)
{
	return((float)(fastmath::atan2(f, 1)));
}

// FUNCTION: GAMEMD 0x004CAE30
double __cdecl fastmath::atan2(double y, double x)
{
	return((float)(fastmath::atan2f(y, x)));
}

// FUNCTION: GAMEMD 0x004CAEF0
void FUN_004CAEF0(void)
{
}

// FUNCTION: GAMEMD 0x004CAF00
void FUN_004CAF00(void)
{
}

// FUNCTION: GAMEMD 0x004CAF10
void FUN_004CAF10(void)
{
}

// FUNCTION: GAMEMD 0x004CB060
float fastmath::sqrtf(float f)
{
	short expo;
	float x = f;

	unsigned int  *fi = (unsigned int *) &x + MOST_SIG_OFFSET;

	if (f == 0) {
		return(0);
	}

	x = (x <= 0) ? x * -1.0 : x;

	expo = (*fi >> EXP_SHIFTS) - EXP_BIAS;

	*fi &= MANT_MASK;

	if (expo & 0x01) {
		*fi |= EXP_LSB;
	}

	expo >>= 1;

	*fi = (sqrt_tab[*fi >> MANT_SHIFTS]) + ((expo + EXP_BIAS) << EXP_SHIFTS);

	return(x);
}

// FUNCTION: GAMEMD 0x004CB0E0
void init_sin_tab(void)
{
	double half = SIN_TABLE_SIZE/2;

	for (int i = 0; i != SIN_TABLE_EXTENT + 1; i++) {
		float angle = i * (M_PI / half);
		sin_tab[i] = fastmath::sinf(angle);
	}
}

// FUNCTION: GAMEMD 0x004CB150
float fastmath::sinf(float f)
{
	static float _s = SIN_TABLE_SIZE / M_PI;

	int scaled = f * _s;
	bool odd = scaled & 1;
	int index = (scaled / 2) % SIN_TABLE_SIZE;

	if (index < 0) {
		index += SIN_TABLE_SIZE;
	}

	if (odd && index < (SIN_TABLE_SIZE - 1)) {
		index++;
	}
	return(sin_tab[index]);
}

// FUNCTION: GAMEMD 0x004CB1A0
float fastmath::cosf(float f)
{
	static float _s = SIN_TABLE_SIZE / M_PI;

	int scaled = f * _s;
	bool odd = scaled & 1;
	int index = (scaled / 2) % SIN_TABLE_SIZE;

	if (index < 0) {
		index += SIN_TABLE_EXTENT;
	} else {
		index += SIN_TABLE_SIZE / 4;
	}

	if (odd && index < (SIN_TABLE_EXTENT - 1)) {
		index++;
	}

	return(sin_tab[index]);
}

// FUNCTION: GAMEMD 0x004CB200
void init_asin_tab(void)
{
	double half = ASIN_TABLE_SIZE/2;

	for (int i = 0; i != ASIN_TABLE_SIZE + 1; i++) {
		float value = i / half - 1;
		asin_tab[i] = fastmath::asinf(value);
	}
}

// FUNCTION: GAMEMD 0x004CB260
float fastmath::asinf(float f)
{
	int i = (int)((f + 1) * (ASIN_TABLE_SIZE / 2));

	return(asin_tab[i]);
}

// FUNCTION: GAMEMD 0x004CB290
float fastmath::acosf(float f)
{
	return(DEG_TO_RAD(90) - fastmath::asinf(f));
}

// FUNCTION: GAMEMD 0x004CB2C0
void init_tan_tab(void)
{
	double half = TAN_TABLE_SIZE/2;

	for (int i = 0; i != TAN_TABLE_SIZE + 1; i++) {
		float angle = i * (M_PI/half);
		tan_tab[i] = fastmath::tanf(angle);
	}
}

// FUNCTION: GAMEMD 0x004CB320
float fastmath::tanf(float f)
{
	int i = int(f * (TAN_TABLE_SIZE / 2 / M_PI)) % TAN_TABLE_SIZE;
	if (i < 0) {
		i += TAN_TABLE_SIZE;
	}
	return(tan_tab[i]);
}

// FUNCTION: GAMEMD 0x004CB3D0
float fastmath::atan2f(float y, float x)
{
	if (x == 0) {
		if (y == 0) {
			return(0);
		}

		if (y > 0) {
			return(M_PI/2);
		}
		return(-M_PI/2);
	}

	float result;
	float z = y / x;
	int i = abs((int)(z / atan_tab[ATAN_TABLE_SIZE + 1]));

	if (i < ATAN_TABLE_SIZE + 1) {
		result = atan_tab[i];
	} else  {
		result = M_PI/2;
	}

	if (x < 0) {
		result = M_PI - result;
	}

	if (y < 0) {
		result = -result;
	}

	return(result);
}

// FUNCTION: GAMEMD 0x004CB480
float fastmath::atanf(float f)
{
	return((float)(fastmath::atan2f(f, 1)));
}

// FUNCTION: GAMEMD 0x004CB4C0
void Sabotage_Sine(void)
{
	Sabotage_Routine(fastmath::sin);
}
