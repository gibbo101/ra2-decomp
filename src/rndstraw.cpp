// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rndstraw.h"
#include "sha.h"
#include <climits>
#include <cstring>

// FUNCTION: GAMEMD 0x00661730
void RandomStraw::Reset(void)
{
	SeedBits = 0;
	Current = 0;
	memset(Random, '\0', sizeof(Random));
}

// FUNCTION: GAMEMD 0x00661750
int RandomStraw::Seed_Bits_Needed(void) const
{
	const int total = sizeof(Random) * CHAR_BIT;
	if (SeedBits < total) {
		return(total - SeedBits);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x00661770
void RandomStraw::Seed_Bit(int seed)
{
	char * ptr = ((char *)&Random[0]) + ((SeedBits / CHAR_BIT) % sizeof(Random));
	char frac = (char)(1 << (SeedBits & (CHAR_BIT-1)));

	if (seed & 0x01) {
		*ptr ^= frac;
	}
	SeedBits++;

	if (SeedBits == (sizeof(Random) * CHAR_BIT)) {
		Scramble_Seed();
	}
}

// FUNCTION: GAMEMD 0x00661850
void RandomStraw::Seed_Byte(char seed)
{
	for (int index = 0; index < CHAR_BIT; index++) {
		Seed_Bit(seed);
		seed >>= 1;
	}
}

// FUNCTION: GAMEMD 0x00661960
void RandomStraw::Seed_Short(short seed)
{
	for (int index = 0; index < (sizeof(seed)*CHAR_BIT); index++) {
		Seed_Bit(seed);
		seed >>= 1;
	}
}

// FUNCTION: GAMEMD 0x00661A60
void RandomStraw::Seed_Long(int seed)
{
	for (int index = 0; index < (sizeof(seed)*CHAR_BIT); index++) {
		Seed_Bit(seed);
		seed >>= 1;
	}
}

// FUNCTION: GAMEMD 0x00661B70
void RandomStraw::Scramble_Seed(void)
{
	SHAEngine sha;

	for (int index = 0; index < sizeof(Random); index++) {
		char digest[20];

		sha.Hash(&Random[0], sizeof(Random));
		sha.Result(digest);

		int tocopy = sizeof(digest) < (sizeof(Random)-index) ? sizeof(digest) : (sizeof(Random)-index);
		memmove(((char *)&Random[0]) + index, digest, tocopy);
	}
}

// FUNCTION: GAMEMD 0x00661C10
int RandomStraw::Get(void * source, int slen)
{
	if (source == NULL || slen < 1) {
		return(BASECLASS::Get(source, slen));
	}

	int total = 0;
	while (slen > 0) {
		*(char *)source = (char)(Random[Current++]());
		Current = Current % (sizeof(Random) / sizeof(Random[0]));
		source = (char*)source + sizeof(char);
		slen--;
		total++;
	}
	return(total);
}

// SYNTHETIC: GAMEMD 0x00661C70
// ??_GRandomStraw@@UAEPAXI@Z
RandomStraw::~RandomStraw(void)
{
	Reset();
}
