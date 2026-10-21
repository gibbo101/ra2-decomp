// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "unvqtblc.h"
#include "_vqa.h"
#include "assert.h"
#include "dsurface.h"

static bool Hicolor_Set_Color_Mode(int cmode);

bool Hicolor_Set_Color_Mode(int cmode)
{
	bool valid = false;

	int redshift, greenshift, blueshift;
	switch (cmode) {
	case COLORMODE_555:
		redshift = 10;
		greenshift = 5;
		blueshift = 0;
		valid = true;
		break;
	case COLORMODE_655:
		redshift = 11;
		greenshift = 5;
		blueshift = 0;
		valid = true;
		break;
	case COLORMODE_565:
		redshift = 11;
		greenshift = 6;
		blueshift = 0;
		valid = true;
		break;
	case COLORMODE_556:
		redshift = 11;
		greenshift = 6;
		blueshift = 1;
		valid = true;
		break;
	default:
		break;
	}

	if (valid) {
		for (int r = 0; r < 32; r++) {
			for (int g = 0; g < 32; g++) {
				for (int b = 0; b < 32; b++) {
					HicolorTable[(unsigned short)(32 * ((32 * r) | g)) | (unsigned short)b] = (r << redshift) | (g << greenshift) | (b << blueshift);
				}
			}
		}

		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00748220
bool Hicolor_Init_Table(int cmode)
{
	Hicolor_Clear_Table();

	HicolorTable = new unsigned short[65535 - 1];
	if (HicolorTable != NULL) {
		return(Hicolor_Set_Color_Mode(cmode));
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00748360
void Hicolor_Clear_Table(void)
{
	if (HicolorTable != NULL) {
		delete[] HicolorTable;
		HicolorTable = NULL;
	}
}

// FUNCTION: GAMEMD 0x00748380
void Hicolor_Translate(void * buffer, int size)
{
	unsigned short * buf = (unsigned short *)buffer;
	while (size > 0) {
		*buf = HicolorTable[*buf];
		buf++;
		size--;
	}
}
