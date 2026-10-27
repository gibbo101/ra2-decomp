// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
