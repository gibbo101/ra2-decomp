// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
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
#include "vanimtype.h"
#include "incdec.h"

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

// FUNCTION: GAMEMD 0x0075D3A0
bool WarheadTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (ini.Is_Present(Name())) {
		SpreadFactor = ini.Get_Float(Name(), "CellSpread", SpreadFactor);
		CellInset = ini.Get_Float(Name(), "CellInset", CellInset);
		PercentAtMax = ini.Get_Float(Name(), "PercentAtMax", PercentAtMax);
		CausesDelayKill = ini.Get_Bool(Name(), "CausesDelayKill", CausesDelayKill);
		DelayKillFrames = ini.Get_Int(Name(), "DelayKillFrames", DelayKillFrames);
		DelayKillAtMax = ini.Get_Float(Name(), "DelayKillAtMax", DelayKillAtMax);
		CombatLightSize = ini.Get_Float(Name(), "CombatLightSize", CombatLightSize);
		Particle = TGet_Class(ini, Name(), "Particle", Particle);

		IsConventional = ini.Get_Bool(Name(), "Conventional", IsConventional);
		IsWallDestroyer = ini.Get_Bool(Name(), "Wall", IsWallDestroyer);
		WallAbsoluteDestroyer = ini.Get_Bool(Name(), "WallAbsoluteDestroyer", WallAbsoluteDestroyer);
		PenetratesBunker = ini.Get_Bool(Name(), "PenetratesBunker", PenetratesBunker);
		IsWoodDestroyer = ini.Get_Bool(Name(), "Wood", IsWoodDestroyer);
		IsTiberiumDestroyer = ini.Get_Bool(Name(), "Tiberium", IsTiberiumDestroyer);
		IsSparky = ini.Get_Bool(Name(), "Sparky", IsSparky);
		Sonic = ini.Get_Bool(Name(), "Sonic", Sonic);
		IsRocker = ini.Get_Bool(Name(), "Rocker", IsRocker);
		DirectRocker = ini.Get_Bool(Name(), "DirectRocker", DirectRocker);
		IsFire = ini.Get_Bool(Name(), "Fire", IsFire);
		IsBright = ini.Get_Bool(Name(), "Bright", IsBright);
		CLDisableRed = ini.Get_Bool(Name(), "CLDisableRed", CLDisableRed);
		CLDisableGreen = ini.Get_Bool(Name(), "CLDisableGreen", CLDisableGreen);
		CLDisableBlue = ini.Get_Bool(Name(), "CLDisableBlue", CLDisableBlue);

		ExplosionSet = TGet_TypeList<AnimTypeClass>(ini, Name(), "AnimList", ExplosionSet);

		InfantryDeath = ini.Get_Int(Name(), "InfDeath", InfantryDeath);

		Deform = ini.Get_Float(Name(), "Deform", Deform);
		DeformThreshhold = ini.Get_Int(Name(), "DeformThreshhold", DeformThreshhold);

		IsEMEffect = ini.Get_Bool(Name(), "EMEffect", IsEMEffect);
		MindControl = ini.Get_Bool(Name(), "MindControl", MindControl);
		Poison = ini.Get_Bool(Name(), "Poison", Poison);
		IvanBomb = ini.Get_Bool(Name(), "IvanBomb", IvanBomb);
		ElectricAssault = ini.Get_Bool(Name(), "ElectricAssault", ElectricAssault);
		Parasite = ini.Get_Bool(Name(), "Parasite", Parasite);
		Temporal = ini.Get_Bool(Name(), "Temporal", Temporal);
		IsLocomotor = ini.Get_Bool(Name(), "IsLocomotor", IsLocomotor);
		Locomotor = ini.Get_CLSID(Name(), "Locomotor", Locomotor);
		Airstrike = ini.Get_Bool(Name(), "Airstrike", Airstrike);
		Psychedelic = ini.Get_Bool(Name(), "Psychedelic", Psychedelic);
		BombDisarm = ini.Get_Bool(Name(), "BombDisarm", BombDisarm);
		Paralyzes = ini.Get_Int(Name(), "Paralyzes", Paralyzes);
		Culling = ini.Get_Bool(Name(), "Culling", Culling);
		MakesDisguise = ini.Get_Bool(Name(), "MakesDisguise", MakesDisguise);
		NukeMaker = ini.Get_Bool(Name(), "NukeMaker", NukeMaker);

		ProneDamage = ini.Get_Float(Name(), "ProneDamage", ProneDamage);
		Radiation = ini.Get_Bool(Name(), "Radiation", Radiation);
		PsychicDamage = ini.Get_Bool(Name(), "PsychicDamage", PsychicDamage);
		AffectsAllies = ini.Get_Bool(Name(), "AffectsAllies", AffectsAllies);
		Bullets = ini.Get_Bool(Name(), "Bullets", Bullets);
		IsVeinhole = ini.Get_Bool(Name(), "Veinhole", IsVeinhole);

		ShakeXlo = ini.Get_Int(Name(), "ShakeXlo", ShakeXlo);
		ShakeXhi = ini.Get_Int(Name(), "ShakeXhi", ShakeXhi);
		ShakeYlo = ini.Get_Int(Name(), "ShakeYlo", ShakeYlo);
		ShakeYhi = ini.Get_Int(Name(), "ShakeYhi", ShakeYhi);

		MaxDebris = ini.Get_Int(Name(), "MaxDebris", MaxDebris);
		MinDebris = ini.Get_Int(Name(), "MinDebris", MinDebris);
		if (MinDebris < 0) {
			MinDebris = 0;
		}
		if (MaxDebris < MinDebris) {
			MaxDebris = MinDebris;
		}

		DebrisTypes = TGet_TypeList<VoxelAnimTypeClass>(ini, Name(), "DebrisTypes", DebrisTypes);
		DebrisMaximums = ini.Get_IntList(IniName, "DebrisMaximums", DebrisMaximums);

		char buffer[128];
		if (ini.Get_String(Name(), "Verses", "100%%,100%%,100%%,100%%,100%%,100%%,100%%,100%%,100%%,100%%,100%%", buffer, sizeof(buffer))) {
			char * aval = strtok(buffer, ",");
			for (ArmorType armor = ARMOR_FIRST; armor < ARMOR_COUNT; armor++) {
				Modifier[armor] = _Parse_Percentage(aval);
				aval = strtok(NULL, ",");
			}
		}

		IsOrganic = (Modifier[ARMOR_MEDIUM] == 0 && Modifier[ARMOR_WOOD] == 0);

		return(true);
	}
	return(false);
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

// TEMPLATE: GAMEMD 0x0075E6B0
// ??0?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@QAE@ABV0@@Z

#include <math.h>
