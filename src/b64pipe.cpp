// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "b64pipe.h"
#include "base64.h"
#include <cstring>

// FUNCTION: 0x0042DDB0
int Base64Pipe::Put(void const * source, int slen)
{
	if (source == NULL || slen < 1) {
		return(BASECLASS::Put(source, slen));
	}

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

	if (Counter > 0) {
		int len = (slen < (fromsize-Counter)) ? slen : (fromsize-Counter);
		memmove(&from[Counter], source, len);
		Counter += len;
		slen -= len;
		source = ((char *)source) + len;

		if (Counter == fromsize) {
			int outcount;
			if (Control == ENCODE) {
				outcount = Base64_Encode(from, fromsize, to, tosize);
			} else {
				outcount = Base64_Decode(from, fromsize, to, tosize);
			}
			total += BASECLASS::Put(to, outcount);
			Counter = 0;
		}
	}

	while (slen >= fromsize) {
		int outcount;
		if (Control == ENCODE) {
			outcount = Base64_Encode(source, fromsize, to, tosize);
		} else {
			outcount = Base64_Decode(source, fromsize, to, tosize);
		}
		source = ((char *)source) + fromsize;
		total += BASECLASS::Put(to, outcount);
		slen -= fromsize;
	}

	if (slen > 0) {
		memmove(from, source, slen);
		Counter = slen;
	}

	return(total);
}

// FUNCTION: 0x0042DF40
int Base64Pipe::Flush(void)
{
	int len = 0;

	if (Counter) {
		if (Control == ENCODE) {
			int chars = Base64_Encode(PBuffer, Counter, CBuffer, sizeof(CBuffer));
			len += BASECLASS::Put(CBuffer, chars);
		} else {
			int chars = Base64_Decode(CBuffer, Counter, PBuffer, sizeof(PBuffer));
			len += BASECLASS::Put(PBuffer, chars);
		}
		Counter = 0;
	}
	len += BASECLASS::Flush();
	return(len);
}
