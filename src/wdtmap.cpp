// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "ini.h"
#include "wdtnet.h"

#include <cstdio>
#include <math.h>
#include "geometry.h"
#include "rect.h"
#include "msanim.h"
#include "surface.h"

extern bool DAT_008487BC;
extern bool DAT_008487BD;
void __fastcall FUN_0072E450(Surface * surface, Rect * rect, int flag);

class MSFrameAnim : public MSAnim
{
	public:
		MSFrameAnim(void);
		virtual ~MSFrameAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;
};

// SYNTHETIC: GAMEMD 0x00767C80 SYMBOL
// _$E3
// SYNTHETIC: GAMEMD 0x00767CA0 SYMBOL
// _$E6
// SYNTHETIC: GAMEMD 0x00767CD0 SYMBOL
// _$E9
// SYNTHETIC: GAMEMD 0x00767CF0 SYMBOL
// _$E12
// SYNTHETIC: GAMEMD 0x00767D10 SYMBOL
// _$E15
// SYNTHETIC: GAMEMD 0x00767D30 SYMBOL
// _$E18
// SYNTHETIC: GAMEMD 0x00767D50 SYMBOL
// _$E21
// SYNTHETIC: GAMEMD 0x00767D70 SYMBOL
// _$E24
// SYNTHETIC: GAMEMD 0x00767DB0 SYMBOL
// _$E27
// SYNTHETIC: GAMEMD 0x00767DE0 SYMBOL
// _$E30
// SYNTHETIC: GAMEMD 0x00767E10 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x00767E40 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x00767E50 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x00767E70 SYMBOL
// _$E42

// FUNCTION: GAMEMD 0x00767E90
MSFrameAnim::MSFrameAnim(void) :
	MSAnim(0, 0, false)
{
}

// FUNCTION: GAMEMD 0x00767EC0
MSFrameAnim::~MSFrameAnim(void)
{
}

// FUNCTION: GAMEMD 0x00767ED0
bool MSFrameAnim::Advance(Surface * surface, Rect & rect)
{
	if (DAT_008487BC) {
		FUN_0072E450(surface, &rect, 1);
	}
	return false;
}

// FUNCTION: GAMEMD 0x00767EF0
void MSFrameAnim::Redraw(Surface * surface, Rect const * rect)
{
	if (DAT_008487BD) {
		int w = surface->Get_Width();
		int h = surface->Get_Height();
		if (rect) {
			Rect r = *rect;
			FUN_0072E450(surface, &r, 1);
		} else {
			Rect r(0, 0, w, h);
			FUN_0072E450(surface, &r, 1);
		}
	}
}

// FUNCTION: GAMEMD 0x00767F80
Rect MSFrameAnim::Get_Rect(void) const
{
	return Rect(0, 0, 640, 480);
}

// SYNTHETIC: GAMEMD 0x00769BD0
// ??_GMSFrameAnim@@UAEPAXI@Z
