// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "psystype.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "ptype.h"
#include "sun.h"
#include "tracker.h"
#include "warhead.h"

// FUNCTION: 0x006447A0
HRESULT STDMETHODCALLTYPE ParticleSystemTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleSystemTypeClass;
	return(S_OK);
}

// FUNCTION: 0x00644830
HRESULT STDMETHODCALLTYPE ParticleSystemTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	return(S_OK);
}

// FUNCTION: 0x00644850
ParticleSystemBehaviorType Particle_System_Behavior_From_Name(char const * name)
{
	static char const * _BehaviorNames[PSYS_BEHAVIOR_COUNT] = {
		"Smoke",
		"Gas",
		"Fire",
		"Spark",
		"Railgun",
		"Web",
		"WeakGas"
	};

	for (ParticleSystemBehaviorType behavior = PSYS_BEHAVIOR_FIRST; behavior < PSYS_BEHAVIOR_COUNT; behavior++) {
		if (!strcmpi(name, _BehaviorNames[behavior])) {
			return(behavior);
		}
	}
	return(PSYS_BEHAVIOR_NONE);
}
