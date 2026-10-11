// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include <string.h>

typedef unsigned char uint8_t;

typedef unsigned short uint16_t;

typedef unsigned int uint32_t;

// FUNCTION: 0x007C3D60
void __cdecl UnVQ1_C4_4x4(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	bufwidth *= 2u;
	uint32_t block_row_stride = bufwidth * 4u;

	uint8_t * end = (uint8_t *)buffer + ((uint32_t)numrows * bufwidth * 4u);
	uint8_t * dst = (uint8_t *)buffer;

	uint8_t * row_base = (uint8_t *)buffer;
	uint8_t * row_end = row_base + ((uint32_t)blocksperrow * 8u);

	uint16_t * src = (uint16_t *)pointers;

	while (dst < end) {
		uint32_t command = (*src & 0xE000);
		uint16_t cb_index = (*src & 0x1FFF);

		src += 1;

		switch (command) {
		case 0x0000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 5);

			for (uint32_t r = 0; r < 4; r++) {
				uint32_t lo;
				uint32_t hi;

				lo = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 0) = lo;

				hi = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 4) = hi;

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x2000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 5);

			for (uint32_t r = 0; r < 4; r++) {
				uint8_t * p = dst;

				for (uint32_t c = 0; c < 4; c++) {
					uint16_t v = *(const uint16_t *)cb;

					if (v != 0x8000u) {
						*(uint16_t *)p = v;
					}

					p += 2;
					cb += 2;
				}

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x4000u: {
			dst += 8;
		} break;

		default:
			break;
		}

		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + ((uint32_t)blocksperrow * 8u);
		}
	}
}

// FUNCTION: 0x007C4260
void __cdecl UnVQ1_C4_4x2(unsigned char * codebook, unsigned char * pointers, unsigned char * buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth)
{
	bufwidth *= 2u;
	uint32_t block_row_stride = bufwidth * 2u;

	uint8_t * end = (uint8_t *)buffer + ((uint32_t)numrows * bufwidth * 2u);
	uint8_t * dst = (uint8_t *)buffer;

	uint8_t * row_base = (uint8_t *)buffer;
	uint8_t * row_end = row_base + ((uint32_t)blocksperrow * 8u);

	uint16_t * src = (uint16_t *)pointers;

	while (dst < end) {
		uint32_t command = (*src & 0xE000);
		uint16_t cb_index = (*src & 0x1FFF);

		src += 1;

		switch (command) {
		case 0x0000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 4);

			for (uint32_t r = 0; r < 2; r++) {
				uint32_t lo;
				uint32_t hi;

				lo = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 0) = lo;

				hi = ((uint32_t)*(const uint16_t *)(cb + 2) << 16) | *(const uint16_t *)(cb + 0);
				cb += 4;
				*(uint32_t *)(dst + 4) = hi;

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x2000u: {
			const uint8_t * cb = (const uint8_t *)codebook + (cb_index << 4);

			for (uint32_t r = 0; r < 2; r++) {
				uint8_t * p = dst;

				for (uint32_t c = 0; c < 4; c++) {
					uint16_t v = *(const uint16_t *)cb;

					if (v != 0x8000u) {
						*(uint16_t *)p = v;
					}

					p += 2;
					cb += 2;
				}

				dst += bufwidth;
			}

			dst += 8u - block_row_stride;
		} break;

		case 0x4000u: {
			dst += 8;
		} break;

		default:
			break;
		}

		if (dst == row_end) {
			row_base += block_row_stride;
			dst = row_base;
			row_end = row_base + ((uint32_t)blocksperrow * 8u);
		}
	}
}
