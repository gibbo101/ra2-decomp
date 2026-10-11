// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "base64.h"
#include <cstddef>

static char const * const _pad = "=";

static char const * const _encoder = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int const PacketChars = 4;

typedef union {
	struct {
#ifdef BIG_ENDIAN
		unsigned char C1;
		unsigned char C2;
		unsigned char C3;
#else
		unsigned char C3;
		unsigned char C2;
		unsigned char C1;
#endif
		unsigned char pad;
	} Char;
	struct {
#ifdef BIG_ENDIAN
		unsigned O1:6;
		unsigned O2:6;
		unsigned O3:6;
		unsigned O4:6;
#else
		unsigned O4:6;
		unsigned O3:6;
		unsigned O2:6;
		unsigned O1:6;
#endif
		unsigned pad:8;
	} SubCode;
	unsigned int Raw;
}	PacketType;

// FUNCTION: 0x0042FD30
int Base64_Encode(void const * source, int slen, void * dest, int dlen)
{
	if (source == NULL || slen == 0 || dest == NULL || dlen == 0) {
		return(0);
	}

	int total = 0;
	unsigned char const * sptr = (unsigned char const *)source;
	unsigned char * dptr = (unsigned char *)dest;
	while (slen > 0 && dlen >= PacketChars) {
		PacketType packet;

		int pad = 0;
		packet.Raw = 0;
		packet.Char.C1 = *sptr++;
		slen--;
		if (slen) {
			packet.Char.C2 = *sptr++;
			slen--;
		} else {
			pad++;
		}
		if (slen) {
			packet.Char.C3 = *sptr++;
			slen--;
		} else {
			pad++;
		}

		*dptr++ = _encoder[packet.SubCode.O1];
		*dptr++ = _encoder[packet.SubCode.O2];
		if (pad < 2) {
			*dptr++ = _encoder[packet.SubCode.O3];
		} else {
			*dptr++ = _pad[0];
		}
		if (pad < 1) {
			*dptr++ = _encoder[packet.SubCode.O4];
		} else {
			*dptr++ = _pad[0];
		}

		dlen -= PacketChars;
		total += PacketChars;
	}

	if (dlen > 0) {
		*dptr = '\0';
	}

	return(total);
}
