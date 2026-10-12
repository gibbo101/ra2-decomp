// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "voc.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_tactica.h"
#include "ccini.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"
#include "tactical.h"
#include "vector.h"

DynamicVectorClass<VocClass *> Vocs;

// FUNCTION: 0x00750110 $E2
// FUNCTION: 0x00750140 $E5
// FUNCTION: 0x00750160 $E8
// FUNCTION: 0x00750180 $E11
// FUNCTION: 0x007501A0 $E14
// FUNCTION: 0x007501C0 $E17
// FUNCTION: 0x007501E0 $E20
// FUNCTION: 0x00750220 $E23
// FUNCTION: 0x00750250 $E26
// FUNCTION: 0x00750280 $E29
// FUNCTION: 0x007502B0 $E32
// FUNCTION: 0x007502C0 $E35

// FUNCTION: 0x007532D0
VocClass * VocClass_From_Name(char const * name)
{
	if (name == NULL) return(NULL);

	if (!strcmpi(name, "<none>")) return(NULL);

	for (VocType voc = VOC_FIRST; voc < Vocs.Count(); voc = VocType(voc + 1)) {
		if (stricmp(name, Vocs[voc]->Name) == 0) {
			return(Vocs[voc]);
		}
	}

	return(NULL);
}

// FUNCTION: 0x00753330
char const * Voc_Name(VocType voc)
{
	if (voc != VOC_NONE && voc < Vocs.Count()) {
		return(Vocs[voc]->Name);
	}
	return("<none>");
}
