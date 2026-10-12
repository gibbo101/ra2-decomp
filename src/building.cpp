// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "building.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "bound.h"
#include "bsurface.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "dbgprint.h"
#include "draw.h"
#include "drive.h"
#include "dsurface.h"
#include "event.h"
#include "factory.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "iloco.h"
#include "ilocos.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipiggy.h"
#include "isotile.h"
#include "isotype.h"
#include "light.h"
#include "lightcon.h"
#include "mono.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "queue.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "smudtype.h"
#include "stimer.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "techtype.h"
#include "tiberium.h"
#include "tracker.h"
#include "unit.h"
#include "unittype.h"
#include "vox.h"
#include "warhead.h"
#include "weapon.h"
#include "color.hh"

extern Point2D DAT_0089C848;

// FUNCTION: 0x0043C0D0
void BuildingClass::FUN_0043C0D0(void)
{
	int count = Rule->DamageFireTypes.Count();
	if (count) {
		int idx = Random_Pick(0, count - 1);
		for (int i = 0; i < 8; i++) {
			if (Class->DamageFireOffset[i] == DAT_0089C848) return;
			if (DamageFireAnims[i]) return;
			Coord c = Coord(TacticalMap->Pixel_To_Lepton(Class->DamageFireOffset[i]), 0);
			c += Render_Coord();
			AnimClass * anim = new AnimClass(Rule->DamageFireTypes[idx], c, 0, 1, (ShapeFlags_Type)0x600, 0, false);
			if (anim) {
				DamageFireAnims[i] = anim;
				anim->ZAdjust = (((Class->DamageFireOffset[i].Y - (Class->Height() + Class->Width()) * 15) * 3) >> 1) - 10;
				if (anim->ZAdjust > 0) anim->ZAdjust = 0;
				if (anim->Type->End > 0) anim->Animation.Set_Stage(Random_Pick(0, anim->Type->End - 1));
				if (++idx >= count) idx = 0;
			}
		}
	}
}

// FUNCTION: 0x00459E80
HRESULT STDMETHODCALLTYPE BuildingClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingClass;
	return(S_OK);
}
