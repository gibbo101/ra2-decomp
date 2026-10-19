// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "partsys.h"
#include "_map.h"
#include "_rtti.h"
#include "ccrand.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "goptions.h"
#include "inline.h"
#include "laser.h"
#include "matrix3d.h"
#include "ovrlight.h"
#include "particle.h"
#include "psystype.h"
#include "ptype.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "techno.h"
#include "tracker.h"
#include "vector.h"
#include "wave.h"

// FUNCTION: 0x006301A0
HRESULT STDMETHODCALLTYPE ParticleSystemClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleSystemClass;
	return(S_OK);
}

// FUNCTION: 0x006301F0
void ParticleSystemClass::FUN_006301F0(int lifetime)
{
	Lifetime = lifetime;
}
