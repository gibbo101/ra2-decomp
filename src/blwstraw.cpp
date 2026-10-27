// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "blwstraw.h"
#include "assert.h"
#include <cstring>
#include <math.h>


// FUNCTION: GAMEMD 0x00438210
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

// FUNCTION: GAMEMD 0x00438300
void BlowStraw::Key(void const * key, int length)
{
	if (BF == NULL) {
		BF = new BlowfishEngine;
	}

	if (BF != NULL) {
		BF->Submit_Key(key, length);
	}
}

// SYNTHETIC: GAMEMD 0x00438340 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00438370 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00438390 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004383B0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004383D0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004383F0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00438410 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00438450 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00438480 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004384B0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004384E0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004384F0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00438510 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00438530 SYMBOL
// _$E41

