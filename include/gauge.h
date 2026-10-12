// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "control.h"

class GaugeClass : public ControlClass
{
		typedef ControlClass BASECLASS;

	public:

		GaugeClass(unsigned id, int x, int y, int w, int h);

		virtual int Draw_Me(int forced=false) override;
		virtual int Set_Maximum(int value);
		virtual int Set_Value(int value);
		virtual int Get_Value(void) const {return(CurValue);};
		virtual void Use_Thumb(int value);

		virtual int Thumb_Pixels(void) { return(4);}

		bool IsColorized;

	protected:

		bool HasThumb;

		bool IsHorizontal;

		int MaxValue;
		int CurValue;

		int ClickDiff;

	protected:
		virtual void Draw_Thumb(void);
		virtual int  Action(unsigned flags, KeyNumType &key, int modifier) override;
		virtual int  Pixel_To_Value(int pixel);
		virtual int  Value_To_Pixel(int value);
};

class TriColorGaugeClass : public GaugeClass
{
		typedef GaugeClass BASECLASS;

	public:
		TriColorGaugeClass(unsigned id, int x, int y, int w, int h);
		virtual int Draw_Me(int forced) override;
		virtual int Set_Red_Limit(int value);
		virtual int Set_Yellow_Limit(int value);

	protected:
		int RedLimit;
		int YellowLimit;
};
