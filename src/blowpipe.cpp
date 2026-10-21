// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "blowpipe.h"
#include "assert.h"
#include <cstring>

// FUNCTION: GAMEMD 0x00438060
int BlowPipe::Flush(void)
{
	int total = 0;
	if (Counter > 0 && BF != NULL) {
		total += BASECLASS::Put(Buffer, Counter);
	}
	Counter = 0;
	total += BASECLASS::Flush();
	return(total);
}

// FUNCTION: GAMEMD 0x004380A0
int BlowPipe::Put(void const * source, int slen)
{
	if (source == NULL || slen < 1) {
		return(BASECLASS::Put(source, slen));
	}

	if (BF == NULL) {
		return(BASECLASS::Put(source, slen));
	}

	int total = 0;

	if (Counter) {
		int sublen = ((int)sizeof(Buffer)-Counter < slen) ? (sizeof(Buffer)-Counter) : slen;
		memmove(&Buffer[Counter], source, sublen);
		Counter += sublen;
		source = ((char *)source) + sublen;
		slen -= sublen;

		if (Counter == sizeof(Buffer)) {
			if (Control == DECRYPT) {
				BF->Decrypt(Buffer, sizeof(Buffer), Buffer);
			} else {
				BF->Encrypt(Buffer, sizeof(Buffer), Buffer);
			}
			total += BASECLASS::Put(Buffer, sizeof(Buffer));
			Counter = 0;
		}
	}

	while (slen >= sizeof(Buffer)) {
		if (Control == DECRYPT) {
			BF->Decrypt(source, sizeof(Buffer), Buffer);
		} else {
			BF->Encrypt(source, sizeof(Buffer), Buffer);
		}
		total += BASECLASS::Put(Buffer, sizeof(Buffer));
		source = ((char *)source) + sizeof(Buffer);
		slen -= sizeof(Buffer);
	}

	if (slen > 0) {
		memmove(Buffer, source, slen);
		Counter = slen;
	}

	return(total);
}
