// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "crc.h"
#include <cctype>
#include <cstring>

// FUNCTION: 0x005F9E80
int Obfuscate(char const * string)
{
	char buffer[128];

	if (!string) return(0);
	memset(buffer, '\xA5', sizeof(buffer));

	strncpy(buffer, string, sizeof(buffer));
	buffer[sizeof(buffer)-1] = '\0';
	int length = strlen(buffer);

	strupr(buffer);

	for (int index = 0; index < length; index++) {
		if (!isgraph(buffer[index])) {
			buffer[index] = (char)('A' + (index%26));
		}
	}

	if (length < 16 || (length & 0x03)) {
		int maxlen = 16;
		if (((length+3) & 0x00FC) > maxlen) {
			maxlen = ((length+3) & 0x00FC);
		}
		int index;
		for (index = length; index < maxlen; index++) {
			buffer[index] = (char)('A' + ((('?' ^ buffer[index-length]) + index) % 26));
		}
		length = index;
		buffer[length] = '\0';
	}

	int code = CRCEngine()(buffer, length);

	int copy = code;

	strrev(buffer);
	code ^= CRCEngine()(buffer, length);

	code = code ^ copy;

	strrev(buffer);
	for (int index2 = 0; index2 < length; index2++) {
		code ^= (unsigned char)buffer[index2];
		unsigned char temp = (unsigned char)code;
		buffer[index2] ^= temp;
		code >>= 8;
		code |= (((int)temp)<<24);
	}

	for (int index3 = 0; index3 < length; index3++) {
		static unsigned char _lossbits[] = {0x00,0x08,0x00,0x20,0x00,0x04,0x10,0x00};
		static unsigned char _addbits[] = {0x10,0x00,0x00,0x80,0x40,0x00,0x00,0x04};

		buffer[index3] |= _addbits[index3 % (sizeof(_addbits)/sizeof(_addbits[0]))];
		buffer[index3] &= (char)(~_lossbits[index3 % (sizeof(_lossbits)/sizeof(_lossbits[0]))]);
	}

	for (int index4 = 0; index4 < length; index4 += 4) {
		short key1 = buffer[index4];
		short key2 = buffer[index4+1];
		short key3 = buffer[index4+2];
		short key4 = buffer[index4+3];
		short val1 = key1;
		short val2 = key2;
		short val3 = key3;
		short val4 = key4;

		val1 *= key1;
		val2 += key2;
		val3 += key3;
		val4 *= key4;

		short s3 = val3;
		val3 ^= val1;
		val3 *= key1;
		short s2 = val2;
		val2 ^= val4;
		val2 += val3;
		val2 *= key3;
		val3 += val2;

		val1 ^= val2;
		val4 ^= val3;

		val2 ^= s3;
		val3 ^= s2;

		buffer[index4] = val1;
		buffer[index4+1] = val2;
		buffer[index4+2] = val3;
		buffer[index4+3] = val4;
	}

	code = CRCEngine()(buffer, length);

	return(code);
}

// FUNCTION: 0x005FA0D0 $E2
// FUNCTION: 0x005FA100 $E5
// FUNCTION: 0x005FA120 $E8
// FUNCTION: 0x005FA140 $E11
// FUNCTION: 0x005FA160 $E14
// FUNCTION: 0x005FA180 $E17
// FUNCTION: 0x005FA1A0 $E20
// FUNCTION: 0x005FA1E0 $E23
// FUNCTION: 0x005FA210 $E26
// FUNCTION: 0x005FA240 $E29
// FUNCTION: 0x005FA270 $E32
// FUNCTION: 0x005FA280 $E35
