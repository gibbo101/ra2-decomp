// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rules.h"
#include "_bench.h"
#include "_palette.h"
#include "_rules.h"
#include "_warhead.h"
#include "_weapon.h"
#include "addon.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "bench.h"
#include "builtype.h"
#include "bullettype.h"
#include "ccfile.h"
#include "conquer.h"
#include "convert.h"
#include "dbgprint.h"
#include "findmake.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infatype.h"
#include "levitate.h"
#include "mission.h"
#include "movie.h"
#include "overtype.h"
#include "psystype.h"
#include "ptype.h"
#include "scheme.h"
#include "script.h"
#include "side.h"
#include "smudtype.h"
#include "stimer.h"
#include "sun.h"
#include "suprtype.h"
#include "swizzle.h"
#include "taskforc.h"
#include "teamtype.h"
#include "terrtype.h"
#include "tiberium.h"
#include "trim.h"
#include "unittype.h"
#include "vanimtype.h"
#include "vector.h"
#include "warhead.h"
#include "weapon.h"
#include "bench.hh"
#include <cstring>

// FUNCTION: 0x006728B0
bool RulesClass::Do_AnimTypes(CCINIClass const & ini)
{
	char const * const ANIMATIONS = "Animations";
	char buffer[32];
	int count = ini.Entry_Count(ANIMATIONS);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(ANIMATIONS, ini.Get_Entry(ANIMATIONS, i), "", buffer, sizeof(buffer))) {
			AnimTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x00672920
bool RulesClass::Do_VoxelAnimTypes(CCINIClass const & ini)
{
	char const * const VOXELANIMS = "VoxelAnims";
	char buffer[32];
	int count = ini.Entry_Count(VOXELANIMS);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(VOXELANIMS, ini.Get_Entry(VOXELANIMS, i), "", buffer, sizeof(buffer))) {
			VoxelAnimTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x00672990
bool RulesClass::Do_WarheadTypes(CCINIClass const & ini)
{
	char buffer[32];
	int count = ini.Entry_Count("Warheads");
	for (int i = 0; i < count; i++) {
		if (ini.Get_String("Warheads", ini.Get_Entry("Warheads", i), "", buffer, sizeof(buffer))) {
			WarheadTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x00672A00
bool RulesClass::Do_ParticleTypes(CCINIClass const & ini)
{
	char const * const PARTICLES = "Particles";
	char buffer[32];
	int count = ini.Entry_Count(PARTICLES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(PARTICLES, ini.Get_Entry(PARTICLES, i), "", buffer, sizeof(buffer))) {
			ParticleTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x00672A70
bool RulesClass::Do_ParticleSystemTypes(CCINIClass const & ini)
{
	char const * const PARTICLESYSTEMS = "ParticleSystems";
	char buffer[32];
	int count = ini.Entry_Count(PARTICLESYSTEMS);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(PARTICLESYSTEMS, ini.Get_Entry(PARTICLESYSTEMS, i), "", buffer, sizeof(buffer))) {
			ParticleSystemTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x00674550
bool RulesClass::Do_Movies(CCINIClass const & ini)
{
	static char const * const MOVIES = "Movies";
	char buffer[32];
	if (ini.Is_Present(MOVIES)) {
		int count = ini.Entry_Count(MOVIES);
		for (int i = 0; i < count; i++) {
			if (ini.Get_String(MOVIES, ini.Get_Entry(MOVIES, i), "<none>", buffer, sizeof(buffer))) {
				if (VQ_From_Name(buffer) == -1) {
					::Movies.Add(strdup(buffer));
				}
			}
		}
		return(true);
	}
	return(false);
}
