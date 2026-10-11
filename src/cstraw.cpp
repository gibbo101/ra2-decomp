// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cstraw.h"
#include <cstring>

// FUNCTION: 0x004A2780
int CacheStraw::Get(void * source, int slen)
{
	int total = 0;

	if (Is_Valid() && source != NULL && slen > 0) {
		while (slen > 0) {
			if (Length > 0) {
				int tocopy = (Length < slen) ? Length : slen;
				memmove(source, ((char *)BufferPtr.Get_Buffer()) + Index, tocopy);
				slen -= tocopy;
				Index += tocopy;
				total += tocopy;
				Length -= tocopy;
				source = (char*)source + tocopy;
			}
			if (slen == 0) break;

			Length = BASECLASS::Get(BufferPtr, BufferPtr.Get_Size());
			Index = 0;
			if (Length == 0) break;
		}
	}
	return(total);
}
