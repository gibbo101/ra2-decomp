// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "gauge.h"

class ShapeButtonClass;

class SliderClass : public GaugeClass
{
		typedef GaugeClass BASECLASS;

	public:
		SliderClass(unsigned id, int x, int y, int w, int h, int belong_to_list=false);
		virtual ~SliderClass(void) override;

		virtual void Set_Thumb_Size(int value);
		virtual int  Set_Maximum(int value) override;
		virtual int  Set_Value(int) override;
		virtual int  Bump(int up);
		virtual int  Step(int up);
		virtual int  Draw_Me(int forced) override;
		virtual void Peer_To_Peer(unsigned flags, KeyNumType & key, ControlClass & whom) override;

		virtual int Thumb_Pixels(void) override { return(ThumbSize);}

	protected:

		ShapeButtonClass * PlusGadget;
		ShapeButtonClass * MinusGadget;

		bool BelongToList;

		int Thumb;

		int ThumbSize;
		int ThumbStart;

		virtual int  Action(unsigned flags, KeyNumType &key, int modifier) override;
		virtual void Draw_Thumb(void) override;

	private:
		void Recalc_Thumb(void);
};
