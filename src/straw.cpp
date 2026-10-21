// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "straw.h"
#include "subtitle.h"
#include <cstddef>
#include <cstring>

// FUNCTION: 0x006C9850
Straw::~Straw(void)
{
	if (ChainTo != NULL) {
		ChainTo->ChainFrom = ChainFrom;
	}
	if (ChainFrom != NULL) {
		ChainFrom->Get_From(ChainTo);
	}

	ChainFrom = NULL;
	ChainTo = NULL;
}

// FUNCTION: 0x006C9890
void Straw::Get_From(Straw * straw)
{
	if (ChainTo != straw) {
		if (straw != NULL && straw->ChainFrom != NULL) {
			straw->ChainFrom->Get_From(NULL);
			straw->ChainFrom = NULL;
		}

		if (ChainTo != NULL) {
			ChainTo->ChainFrom = NULL;
		}

		ChainTo = straw;
		if (ChainTo != NULL) {
			ChainTo->ChainFrom = this;
		}
	}
}

// FUNCTION: 0x006C98E0
int Straw::Get(void * source, int slen)
{
	if (ChainTo != NULL) {
		return(ChainTo->Get(source, slen));
	}
	return(0);
}

// FUNCTION: 0x006C9900
SubTitleClass::SubTitleClass()
	:
	mTimeStamp(0),
	mDuration(20 * 60),
	mRGBColor(0x00FFFFFF),
	mLinePosition(15),
	mAlignment(Center),
	mCaption(NULL)
{
}

// FUNCTION: 0x006C9930
SubTitleClass::~SubTitleClass()
{
	if (mCaption != NULL) {
		delete[] mCaption;
	}
}

// FUNCTION: 0x006C9940
void SubTitleClass::Set_RGB_Color(unsigned char red, unsigned char green, unsigned char blue)
{
	mRGBColor = (((unsigned long)red << 16) | ((unsigned long)green << 8) | (unsigned long)blue);
}

// FUNCTION: 0x006C9960
void SubTitleClass::Set_Caption(wchar_t * string)
{
	if (mCaption != NULL) {
		delete[] mCaption;
		mCaption = NULL;
	}

	if (string != NULL) {
		unsigned int length = wcslen(string);
		mCaption = new wchar_t[length + 1];

		if (mCaption != NULL) {
			wcscpy(mCaption, string);
		}
	}
}

int DAT_00B0BFD8, DAT_00B0BFDC, DAT_00B0BFE0, DAT_00B0BFE4;

// FUNCTION: 0x006C99B0
void FUN_006C99B0(void)
{
	DAT_00B0BFD8 = 0;
	DAT_00B0BFDC = 0;
	DAT_00B0BFE0 = 0;
	DAT_00B0BFE4 = 0;
}
