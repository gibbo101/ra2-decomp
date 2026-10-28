// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rle.h"

// FUNCTION: GAMEMD 0x00661530
int RLEEngine::Compress(void const * source, void * dest, int length) const
{
	int outlen = 0;
	unsigned char const * sptr = (unsigned char const *) source;
	unsigned char * dptr = (unsigned char *)dest;
	while (length > 0) {
		if (*sptr == '\0') {
			int runcount = 0;
			while (sptr[runcount] == '\0' && runcount <= length) {
				runcount++;
			}

			runcount = MIN(runcount, 255);
			if (dptr != NULL) {
				*dptr++ = '\0';
				*dptr++ = (unsigned char)runcount;
			}
			outlen += 2;
			sptr += runcount;
			length -= runcount;
		} else {
			if (dptr != NULL) {
				*dptr++ = *sptr++;
			} else {
				sptr++;
			}
			outlen++;
			length--;
		}
	}

	return(outlen);
}

// FUNCTION: GAMEMD 0x00661590
int RLEEngine::Line_Compress(void const * source, void * dest, int length) const
{
	if (dest != NULL) {
		unsigned short * sizeptr = (unsigned short *)dest;
		int complen = Compress(source, sizeptr+1, length) + sizeof(short);
		*sizeptr = (unsigned short)complen;
		return(complen);
	}

	return(Compress(source, NULL, length) + sizeof(short));
}
