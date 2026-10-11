// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "ptype.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "warhead.h"

// FUNCTION: 0x00645620
HRESULT STDMETHODCALLTYPE ParticleTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleTypeClass;
	return(S_OK);
}

// FUNCTION: 0x006458D0
ParticleBehaviorType Particle_Behavior_From_Name(char const * name)
{
	static char const * _BehaviorNames[BEHAVIOR_COUNT] = {
		"Gas",
		"Smoke",
		"Fire",
		"Spark",
		"Railgun",
		"Web",
		"WeakGas"
	};

	for (ParticleBehaviorType behavior = BEHAVIOR_FIRST; behavior < BEHAVIOR_COUNT; behavior++) {
		if (!strcmpi(name, _BehaviorNames[behavior])) {
			return(behavior);
		}
	}
	return(BEHAVIOR_NONE);
}
