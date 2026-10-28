// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "warhead.h"
#include "_warhead.h"
#include "animtype.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "partsys.h"
#include "psystype.h"
#include "session.h"
#include "geometry.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"

// SYNTHETIC: GAMEMD 0x0075CC90 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0075CCC0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0075CCE0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x0075CD00 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x0075CD20 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x0075CD40 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x0075CD60 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x0075CDA0 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x0075CDD0 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x0075CE00 SYMBOL
// _$E62
// SYNTHETIC: GAMEMD 0x0075CE30 SYMBOL
// _$E65
// SYNTHETIC: GAMEMD 0x0075CE40 SYMBOL
// _$E68
// SYNTHETIC: GAMEMD 0x0075CE60 SYMBOL
// _$E71

// FUNCTION: GAMEMD 0x0075CE80
float _Parse_Percentage(const char * string)
{
	if (strchr(string, '%')) {
		return((double)atoi(string) / 100);
	}

	return(atof(string));
}

namespace LocomotionClass { namespace CLSIDs { extern const CLSID Teleport; } }

inline void Make_ID(WarheadTypeClass * w) { w->AbstractClass::Create_ID(); }

// FUNCTION: GAMEMD 0x0075CEC0
WarheadTypeClass::WarheadTypeClass(char const * ininame) :
	BASECLASS(ininame),
	Deform(0),
	ProneDamage(1.),
	DeformThreshhold(0),
	ExplosionSet(),
	InfantryDeath(0),
	SpreadFactor(0),
	CellInset(0),
	PercentAtMax(1),
	CausesDelayKill(false),
	DelayKillFrames(5),
	DelayKillAtMax(1),
	CombatLightSize(0),
	Particle(NULL),
	IsWallDestroyer(false),
	WallAbsoluteDestroyer(false),
	PenetratesBunker(false),
	IsWoodDestroyer(false),
	IsTiberiumDestroyer(false),
	IsOrganic(false),
	IsSparky(false),
	Sonic(false),
	IsFire(false),
	IsConventional(false),
	IsRocker(false),
	DirectRocker(false),
	IsBright(false),
	CLDisableRed(false),
	CLDisableGreen(false),
	CLDisableBlue(false),
	IsEMEffect(false),
	MindControl(false),
	Poison(false),
	IvanBomb(false),
	ElectricAssault(false),
	Parasite(false),
	Temporal(false),
	IsLocomotor(false),
	Locomotor(LocomotionClass::CLSIDs::Teleport),
	Airstrike(false),
	Psychedelic(false),
	BombDisarm(false),
	Paralyzes(0),
	Culling(false),
	MakesDisguise(false),
	NukeMaker(false),
	Radiation(false),
	PsychicDamage(false),
	AffectsAllies(true),
	Bullets(false),
	IsVeinhole(false),
	ShakeXlo(0),
	ShakeXhi(0),
	ShakeYlo(0),
	ShakeYhi(0),
	DebrisTypes(),
	DebrisMaximums(),
	MaxDebris(0),
	MinDebris(0)
{
	Make_ID(this);

	TypeList<VoxelAnimTypeClass const *> * debris = &DebrisTypes;
	TypeList<int> * maximums = &DebrisMaximums;
	debris->Clear();
	maximums->Clear();

	Warheads.Add(this);
	AbstractTypePtrTracker.Add(this);

	for (int armor = ARMOR_FIRST; armor < ARMOR_COUNT; armor++) {
		Modifier[armor] = 1;
	}
}

// FUNCTION: GAMEMD 0x0075D1B0
WarheadTypeClass::WarheadTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	DebrisTypes(x),
	DebrisMaximums(x)
{
}

// FUNCTION: GAMEMD 0x0075D230
WarheadTypeClass::~WarheadTypeClass(void)
{
	Detach_This_From_All(this, true);
	Warheads.Delete(this);
	AbstractTypePtrTracker.Delete(this);
}

// FUNCTION: GAMEMD 0x0075DEC0
void WarheadTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(IsWallDestroyer);
	crc(IsWoodDestroyer);
	crc(IsTiberiumDestroyer);
	crc(IsOrganic);
	crc(IsSparky);
	crc(Sonic);
	crc(IsFire);
	crc(IsConventional);
	crc(IsRocker);
	crc(DirectRocker);
	crc(IsBright);
	crc(CLDisableRed);
	crc(CLDisableGreen);
	crc(CLDisableBlue);
	crc(Deform);
	crc(DeformThreshhold);
	crc(ProneDamage);
	crc(IsVeinhole);
	crc(ShakeXlo);
	crc(ShakeXhi);
	crc(ShakeYlo);
	crc(ShakeYhi);
	crc(&Locomotor, sizeof(Locomotor));
	for (int armor = ARMOR_FIRST; armor < ARMOR_COUNT; armor++) {
		crc(Modifier[armor]);
	}

	crc(ExplosionSet.Count());
	crc(InfantryDeath);
}

// FUNCTION: GAMEMD 0x0075E080
HRESULT STDMETHODCALLTYPE WarheadTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WarheadTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0075E0C0
HRESULT STDMETHODCALLTYPE WarheadTypeClass::Load(IStream * stream)
{
	DebrisTypes.Clear();
	DebrisMaximums.Clear();
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) WarheadTypeClass(NoInitClass());

		ExplosionSet.Load(stream);
		DebrisTypes.Load(stream);
		DebrisMaximums.Load(stream);

		for (int i = 0; i < ExplosionSet.Count(); i++) {
			Swizzle_Pointer(&ExplosionSet[i]);
		}
		for (int j = 0; j < DebrisTypes.Count(); j++) {
			Swizzle_Pointer(&DebrisTypes[j]);
		}
		Swizzle_Pointer(&Particle);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0075E2C0
HRESULT STDMETHODCALLTYPE WarheadTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		ExplosionSet.Save(stream);
		DebrisTypes.Save(stream);
		DebrisMaximums.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0075E3B0
WarheadTypeClass * WarheadTypeClass::Find_Or_Make(const char * name)
{
	return(TFind_Or_Make<WarheadTypeClass>(name, Warheads));
}

// FUNCTION: GAMEMD 0x0075E440
void WarheadTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (target == (AbstractClass *)Particle) {
		Particle = NULL;
	}
	ExplosionSet.Delete((AnimTypeClass *)target);
}

// FUNCTION: GAMEMD 0x0075E4A0
WarheadTypeClass * WarheadTypeClass::From_Name(char const * name)
{
	for (int index = 0; index < Warheads.Count(); index++) {
		if (stricmp(Warheads[index]->Name(), name) == 0) {
			return(Warheads[index]);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0075E4F0
int WarheadTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0075E500
RTTIType WarheadTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_WARHEADTYPE);
}

// SYNTHETIC: GAMEMD 0x0075E510
// ??_GWarheadTypeClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0075E690
// ??0?$TypeList@PBVVoxelAnimTypeClass@@@@QAE@ABVNoInitClass@@@Z

// TEMPLATE: GAMEMD 0x0075E6A0
// ??0?$TypeList@H@@QAE@ABVNoInitClass@@@Z

#include <math.h>
