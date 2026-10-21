// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "b64straw.h"
#include "base64.h"
#include <cstring>

// FUNCTION: GAMEMD 0x0042DF90
int Base64Straw::Get(void * source, int slen)
{
	int total = 0;

	char * from;
	int fromsize;
	char * to;
	int tosize;

	if (Control == ENCODE) {
		from = PBuffer;
		fromsize = sizeof(PBuffer);
		to = CBuffer;
		tosize = sizeof(CBuffer);
	} else {
		from = CBuffer;
		fromsize = sizeof(CBuffer);
		to = PBuffer;
		tosize = sizeof(PBuffer);
	}

	while (slen > 0) {
		if (Counter > 0) {
			int len = (slen < Counter) ? slen : Counter;
			memmove(source, &to[tosize-Counter], len);
			Counter -= len;
			slen -= len;
			source = ((char *)source) + len;
			total += len;
		}
		if (slen == 0) break;

		int incount = BASECLASS::Get(from, fromsize);
		if (Control == ENCODE) {
			Counter = Base64_Encode(from, incount, to, tosize);
		} else {
			Counter = Base64_Decode(from, incount, to, tosize);
		}
		if (Counter == 0) break;
	}

	return(total);
}
