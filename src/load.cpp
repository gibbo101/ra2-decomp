// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "iff.h"
#include "lcw.h"
#include <cstring>

// FUNCTION: GAMEMD 0x005584F0
unsigned int Uncompress_Data(void const *src, void *dst)
{
	unsigned int					skip;
	CompressionType	method;
	unsigned int					uncomp_size=NULL;

	if (!src || !dst) return(NULL);

	uncomp_size = ((CompHeaderType*)src)->Size;
	#if (AMIGA)
		uncomp_size = Reverse_Long(uncomp_size);
	#endif
	skip = ((CompHeaderType*)src)->Skip;
	#if (AMIGA)
		skip = Reverse_Word(skip);
	#endif
	method = (CompressionType) ((CompHeaderType*)src)->Method;
	src = ((char*)src) + (int)sizeof(CompHeaderType) + (int)skip;

	switch (method) {
		default:
		case NOCOMPRESS:
			memmove(dst, (void *) src, uncomp_size);
			break;

		case HORIZONTAL:
#if LIB_EXTERNS_RESOLVED
			RLE_Uncompress((void *) src, dst, uncomp_size);
#endif
			break;

		case LCW:
			LCW_Uncomp((void *) src, (void *) dst, (unsigned int) uncomp_size);
			break;
	}

	return(uncomp_size);
}
