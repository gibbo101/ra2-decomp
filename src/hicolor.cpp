// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define PIXEL_RGB555(r,g,b) 		( (((r) & 0xF8) << 7) | (((g) & 0xF8) << 2) | (((b) & 0xF8) >> 3) )

// FUNCTION: GAMEMD 0x004F4DA0
void rgb24_clamp(unsigned char *buf, int len)
{
	for (unsigned i = 0; i < 3 * (unsigned)len; i++) {
		*buf &= 0xF8;
		buf++;
	}
}

// FUNCTION: GAMEMD 0x004F4DC0
void rgb32_to_rgb24_clamped(unsigned char *buf, int len)
{
	for (int i = 0; i < len; i++) {
		*buf &= 0xF8;
		buf++;

		*buf &= 0xF8;
		buf++;

		*buf &= 0xF8;
		buf++;

		buf++;
	}
}

// FUNCTION: GAMEMD 0x004F4EB0
void rgb32_to_rgb24(unsigned char *buf, int rows, int columns, int count)
{
	unsigned len = rows * columns * count;

	unsigned char *src = buf;
	for (unsigned i = 0; i < len; i++) {
		src++;

		*buf= *src;
		src++;
		buf++;

		*buf= *src;
		src++;
		buf++;

		*buf= *src;
		src++;
		buf++;
	}
}
