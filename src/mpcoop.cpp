// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mpcoop.h"

// FUNCTION: 0x005C18A0
bool MPCooperative::vt_entry_10(void)
{
	for (int j = Campaigns.Count() - 1; j >= 0; --j) {
		CoopCamp_0049B610 * camp = Campaigns[j];
		if (camp) {
			camp->FUN_0049B720();
			operator delete(camp);
		}
	}
	DynamicVectorClass<CoopCamp_0049B610 *> * vec = &Campaigns;
	int max = vec->Length();
	vec->Clear();
	vec->Resize(max, 0);
	int count;
	DAT_0089F518.FUN_0049E4F0(&count);
	for (int i = 0; i < count; ++i) {
		CoopCamp_0049B610 * raw = (CoopCamp_0049B610 *)operator new(sizeof(CoopCamp_0049B610));
		CoopCamp_0049B610 * camp = raw ? raw->FUN_0049B610() : 0;
		camp->FUN_0049CAF0(i);
		vec->Add(camp);
	}
	return(true);
}
