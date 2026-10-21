// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "blwstraw.h"
#include "assert.h"
#include <cstring>
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
extern const double DAT_007E3CD0;
extern const double DAT_007E3CD8;

// FUNCTION: 0x00438210
int BlowStraw::Get(void * source, int slen)
{
	if (source == NULL || slen <= 0) {
		return(0);
	}

	if (BF == NULL) {
		return(BASECLASS::Get(source, slen));
	}

	int total = 0;

	while (slen > 0) {
		if (Counter > 0) {
			int sublen = (slen < Counter) ? slen : Counter;
			memmove(source, &Buffer[sizeof(Buffer)-Counter], sublen);
			Counter -= sublen;
			source = ((char *)source) + sublen;
			slen -= sublen;
			total += sublen;
		}
		if (slen == 0) break;

		int incount = BASECLASS::Get(Buffer, sizeof(Buffer));
		if (incount == 0) break;

		if (incount == sizeof(Buffer)) {
			if (Control == DECRYPT) {
				BF->Decrypt(Buffer, incount, Buffer);
			} else {
				BF->Encrypt(Buffer, incount, Buffer);
			}
		} else {
			memmove(&Buffer[sizeof(Buffer)-incount], Buffer, incount);
		}
		Counter = incount;
	}

	return(total);
}

// FUNCTION: 0x00438300
void BlowStraw::Key(void const * key, int length)
{
	if (BF == NULL) {
		BF = new BlowfishEngine;
	}

	if (BF != NULL) {
		BF->Submit_Key(key, length);
	}
}

// FUNCTION: 0x00438340 $E2
double DAT_0089C6A0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00438370 $E5
double DAT_0089C6F0 = DAT_0089C6A0 / DAT_007E3CD0;

// FUNCTION: 0x00438390 $E8
double DAT_0089C6D8 = DAT_007E1718 / DAT_0089C6F0;

// FUNCTION: 0x004383B0 $E11
double DAT_0089C6D0 = DAT_007E1720 * DAT_007E3CD8;

// FUNCTION: 0x004383D0 $E14
double DAT_0089C6A8 = DAT_007E1728 * DAT_007E3CD8;

// FUNCTION: 0x004383F0 $E17
double DAT_0089C680 = DAT_007E1730 * DAT_007E3CD8;

// FUNCTION: 0x00438410 $E20
int DAT_0089C6F8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089C680 - DAT_0089C6A8) * DAT_0089C6A0));

// FUNCTION: 0x00438450 $E23
double DAT_0089C6B0 = FUN_004CADE0(DAT_0089C6F8 * DAT_007E1740);

// FUNCTION: 0x00438480 $E26
double DAT_0089C6C8 = FUN_004CADE0((DAT_0089C6F8 * 2) / DAT_0089C6A0);

// FUNCTION: 0x004384B0 $E29
int DAT_0089C6EC = (int)(DAT_0089C6F8 * 4 + DAT_007E1738);

struct ShortPair_89C698 {
	short X;
	short Y;
	ShortPair_89C698(void) : X(0), Y(0) {}
};

// FUNCTION: 0x004384E0 $E32
ShortPair_89C698 DAT_0089C698;

struct Triple_89C6B8 {
	int A;
	int B;
	int C;
	Triple_89C6B8(void) : A(0), B(0), C(0) {}
};

// FUNCTION: 0x004384F0 $E35
Triple_89C6B8 DAT_0089C6B8;

struct Triple_89C6E0 {
	int A;
	int B;
	int C;
	Triple_89C6E0(void) : A(128), B(128), C(0) {}
};

// FUNCTION: 0x00438510 $E38
Triple_89C6E0 DAT_0089C6E0;

struct Quad_89C688 {
	int A;
	int B;
	int C;
	int D;
	Quad_89C688(void) : A(0), B(0), C(0), D(0) {}
};

// FUNCTION: 0x00438530 $E41
Quad_89C688 DAT_0089C688;
