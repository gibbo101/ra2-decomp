// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "_warhead.h"

#include "vector.h"
#include "warhead.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0040F2A0 SYMBOL
// _$E4
// SYNTHETIC: GAMEMD 0x0040F2D0 SYMBOL
// _$E7
// SYNTHETIC: GAMEMD 0x0040F2F0 SYMBOL
// _$E10
// SYNTHETIC: GAMEMD 0x0040F310 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x0040F330 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x0040F350 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x0040F370 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x0040F3B0 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x0040F3E0 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x0040F410 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x0040F440 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x0040F450 SYMBOL
// _$E37

// SYNTHETIC: GAMEMD 0x0040F470 SYMBOL
// _$E40

// SYNTHETIC: GAMEMD 0x0040F490 SYMBOL
// _$E45

DynamicVectorClass<WarheadTypeClass *> Warheads;

// SYNTHETIC: GAMEMD 0x0040F4D0 SYMBOL
// _$E43

// FUNCTION: GAMEMD 0x0040F510
WarheadType Warhead_From_Name(char const * name)
{
	if (stricmp(name, "<none>") != 0 && stricmp(name, "none") != 0) {
		for (int index = WARHEAD_FIRST; index < Warheads.Count(); index++) {
			if (stricmp(name, Warheads[index]->Name()) == 0) {
				return(WarheadType(index));
			}
		}
		WarheadTypeClass *ptr = new WarheadTypeClass(name);
		return(WarheadType(Warheads.ID(ptr)));

	}
	return(WARHEAD_NONE);
}

// FUNCTION: GAMEMD 0x0040F5C0
char const * Warhead_Name(WarheadType warhead)
{
	if (warhead == WARHEAD_NONE || (unsigned)warhead >= (unsigned)Warheads.Count()) return(NULL);

	return(Warheads[warhead]->Name());
}

// TEMPLATE: GAMEMD 0x0040F5E0
// ?Clear@?$DynamicVectorClass@PAVWarheadTypeClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0040F610
// ?ID@?$DynamicVectorClass@PAVWarheadTypeClass@@@@UAEHPBQAVWarheadTypeClass@@@Z

// TEMPLATE: GAMEMD 0x0040F630
// ?GetItem@?$VectorClass@PAVWarheadTypeClass@@@@UBEPAVWarheadTypeClass@@H@Z

// TEMPLATE: GAMEMD 0x0040F640
// ??8?$VectorClass@PAVWarheadTypeClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0040F680
// ?Resize@?$VectorClass@PAVWarheadTypeClass@@@@UAE_NHPBQAVWarheadTypeClass@@@Z

// TEMPLATE: GAMEMD 0x0040F730
// ?Clear@?$VectorClass@PAVWarheadTypeClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0040F760
// ?ID@?$VectorClass@PAVWarheadTypeClass@@@@UAEHABQAVWarheadTypeClass@@@Z

template class DynamicVectorClass<WarheadTypeClass *>;
