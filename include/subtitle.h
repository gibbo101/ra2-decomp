// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"
#include <wchar.h>

class SubTitleClass
{
public:
	SubTitleClass();
	~SubTitleClass();

	// Display time and duration are in ticks (1/60 seconds).
	void Set_Display_Time(unsigned long time) { mTimeStamp = time; }
	unsigned long Get_Display_Time(void) const { return mTimeStamp; }
	void Set_Display_Duration(unsigned long duration) { mDuration = duration; }
	unsigned long Get_Display_Duration(void) const { return mDuration; }

	void Set_RGB_Color(unsigned char red, unsigned char green, unsigned char blue);
	unsigned long Get_RGB_Color(void) const { return mRGBColor; }

	void Set_Line_Position(int linePos) { mLinePosition = linePos; }
	int Get_Line_Position(void) const { return mLinePosition; }

	typedef enum
	{
		Left,
		Right,
		Center,
	} Alignment;

	void Set_Alignment(Alignment align) { mAlignment = align; }
	Alignment Get_Alignment(void) const { return mAlignment; }

	void Set_Caption(wchar_t * string);
	const wchar_t * Get_Caption(void) const { return mCaption; }

private:
	unsigned long mTimeStamp;
	unsigned long mDuration;
	unsigned long mRGBColor;
	int mLinePosition;
	Alignment mAlignment;
	wchar_t * mCaption;
};
