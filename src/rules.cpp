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

template<> void VectorClass<ColorScheme *>::Clear(void);
void FUN_006269A0(void);
void Difficulty_Get(CCINIClass const & ini, DifficultyClass & diff, char const * section);
class MPGameModeStub { public: CCINIClass * FUN_005D67B0(void); };
struct SessionStub { int GameMode; MPGameModeStub * Mode; bool Is_Multiplayer(void) const {return GameMode != 0 && GameMode != 5;} };
extern SessionStub DAT_00A8B238;

// FUNCTION: 0x006686C0
void RulesClass::Initialize(CCINIClass & ini)
{
	while (ColorSchemes.Count()) {
		delete ColorSchemes[0];
	}
	ColorSchemes.Clear();
	FUN_006269A0();
	while (TeamTypes.Count()) {
		delete TeamTypes[0];
	}
	while (TaskForces.Count()) {
		delete TaskForces[0];
	}
	while (ScriptTypes.Count()) {
		delete ScriptTypes[0];
	}
	while (OverlayTypes.Count()) {
		delete OverlayTypes[0];
	}
	while (SmudgeTypes.Count()) {
		delete SmudgeTypes[0];
	}
	while (TerrainTypes.Count()) {
		delete TerrainTypes[0];
	}
	while (InfantryTypes.Count()) {
		delete InfantryTypes[0];
	}
	while (UnitTypes.Count()) {
		delete UnitTypes[0];
	}
	while (AircraftTypes.Count()) {
		delete AircraftTypes[0];
	}
	while (BulletTypes.Count()) {
		delete BulletTypes[0];
	}
	while (BuildingTypes.Count()) {
		delete BuildingTypes[0];
	}
	while (SuperWeaponTypes.Count()) {
		delete SuperWeaponTypes[0];
	}
	while (AnimTypes.Count()) {
		delete AnimTypes[0];
	}
	while (Weapons.Count()) {
		delete Weapons[0];
	}
	while (::Warheads.Count()) {
		delete ::Warheads[0];
	}
	while (VoxelAnimTypes.Count()) {
		delete VoxelAnimTypes[0];
	}
	while (ParticleTypes.Count()) {
		delete ParticleTypes[0];
	}
	while (ParticleSystemTypes.Count()) {
		delete ParticleSystemTypes[0];
	}
	while (::Houses.Count()) {
		delete ::Houses[0];
	}
	while (HouseTypes.Count()) {
		delete HouseTypes[0];
	}

	Heap_Maximums(ini);
	Addition(ini);

	for (int index = 0; index < 16; index++) {
		ColorAdd[index] = RGBClass(0, 0, 0);
	}

	CCFileClass langfile("LANGRULE.INI");
	if (langfile.Is_Available() == true) {
		CCINIClass langini;
		if (langini.Load(langfile, true) > 1) {
			return;
		}
		Addition(langini);
	}

	Read_ColorAdd(ini);

	if (DAT_00A8B238.GameMode != 0 && DAT_00A8B238.Mode != NULL) {
		Addition(*DAT_00A8B238.Mode->FUN_005D67B0());
	}
}

// FUNCTION: 0x00668FB0
bool RulesClass::Special_Weapons(CCINIClass & ini)
{
	static char const * SPECIALWEAPONS = "SpecialWeapons";
	if (ini.Is_Present(SPECIALWEAPONS)) {
		NukeWarhead = TGet_Class(ini, SPECIALWEAPONS, "NukeWarhead", NukeWarhead);
		NukeProjectile = TGet_Class(ini, SPECIALWEAPONS, "NukeProjectile", NukeProjectile);
		NukeDown = TGet_Class(ini, SPECIALWEAPONS, "NukeDown", NukeDown);
		MutateWarhead = TGet_Class(ini, SPECIALWEAPONS, "MutateWarhead", MutateWarhead);
		MutateExplosionWarhead = TGet_Class(ini, SPECIALWEAPONS, "MutateExplosionWarhead", MutateExplosionWarhead);
		EMPulseWarhead = TGet_Class(ini, SPECIALWEAPONS, "EMPulseWarhead", EMPulseWarhead);
		EMPulseProjectile = TGet_Class(ini, SPECIALWEAPONS, "EMPulseProjectile", EMPulseProjectile);
		for (int i = 0; i < ::Warheads.Count(); i++) {
			::Warheads[i]->Read_INI(ini);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0066B900
bool RulesClass::Crate_Rules(CCINIClass const & ini)
{
	static char const * CRATERULES = "CrateRules";
	if (ini.Is_Present(CRATERULES)) {
		IsFreeMCV = ini.Get_Bool(CRATERULES, "FreeMCV", IsFreeMCV);
		WoodCrateImg = TGet_Class(ini, CRATERULES, "WoodCrateImg", WoodCrateImg);
		CrateImg = TGet_Class(ini, CRATERULES, "CrateImg", CrateImg);
		WaterCrateImg = TGet_Class(ini, CRATERULES, "WaterCrateImg", WaterCrateImg);
		HealCrateSound = ini.Get_VocType(CRATERULES, "HealCrateSound", HealCrateSound);
		CrateMinimum = ini.Get_Int(CRATERULES, "CrateMinimum", CrateMinimum);
		CrateMaximum = ini.Get_Int(CRATERULES, "CrateMaximum", CrateMaximum);
		CrateRadius = ini.Get_Lepton(CRATERULES, "CrateRadius", CrateRadius);
		CrateTime = ini.Get_Float(CRATERULES, "CrateRegen", CrateTime);
		UnitCrateType = TGet_Class(ini, CRATERULES, "UnitCrateType", UnitCrateType);
		SoloCrateMoney = ini.Get_Int(CRATERULES, "SoloCrateMoney", SoloCrateMoney);
		SilverCrate = ini.Get_CrateType(CRATERULES, "SilverCrate", SilverCrate);
		WoodCrate = ini.Get_CrateType(CRATERULES, "WoodCrate", WoodCrate);
		WaterCrate = ini.Get_CrateType(CRATERULES, "WaterCrate", WaterCrate);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0066CF70
bool RulesClass::Radiation(CCINIClass const & ini)
{
	static char const * RADIATION = "Radiation";
	if (ini.Is_Present(RADIATION)) {
		RadDurationMultiple = ini.Get_Int(RADIATION, "RadDurationMultiple", RadDurationMultiple);
		RadApplicationDelay = ini.Get_Int(RADIATION, "RadApplicationDelay", RadApplicationDelay);
		RadLevelMax = ini.Get_Int(RADIATION, "RadLevelMax", RadLevelMax);
		RadLevelDelay = ini.Get_Int(RADIATION, "RadLevelDelay", RadLevelDelay);
		RadLightDelay = ini.Get_Int(RADIATION, "RadLightDelay", RadLightDelay);
		RadLevelFactor = ini.Get_Float(RADIATION, "RadLevelFactor", RadLevelFactor);
		RadLightFactor = ini.Get_Float(RADIATION, "RadLightFactor", RadLightFactor);
		RadTintFactor = ini.Get_Float(RADIATION, "RadTintFactor", RadTintFactor);
		RadColor = ini.Get_RGBClass(RADIATION, "RadColor", RadColor);
		RadSiteWarhead = TGet_Class(ini, RADIATION, "RadSiteWarhead", RadSiteWarhead);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0066D150
bool RulesClass::Elevation_Model(CCINIClass const & ini)
{
	static char const * ELEVATIONMODEL = "ElevationModel";
	if (ini.Is_Present(ELEVATIONMODEL)) {
		ElevationIncrement = ini.Get_Int(ELEVATIONMODEL, "ElevationIncrement", ElevationIncrement);
		ElevationIncrementBonus = ini.Get_Float(ELEVATIONMODEL, "ElevationIncrementBonus", ElevationIncrementBonus);
		ElevationBonusCap = ini.Get_Float(ELEVATIONMODEL, "ElevationBonusCap", ElevationBonusCap);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0066D1F0
bool RulesClass::Wall_Model(CCINIClass const & ini)
{
	static char const * WALLMODEL = "WallModel";
	if (ini.Is_Present(WALLMODEL)) {
		AlliedWallTransparency = ini.Get_Bool(WALLMODEL, "AlliedWallTransparency", AlliedWallTransparency);
		WallPenetratorThreshold = ini.Get_Float(WALLMODEL, "WallPenetratorThreshold", WallPenetratorThreshold);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0066D270
void Difficulty_Get(CCINIClass const & ini, DifficultyClass & diff, char const * section)
{
	if (ini.Is_Present(section)) {
		diff.FirepowerBias = ini.Get_Float(section, "FirePower", 1);
		diff.GroundspeedBias = ini.Get_Float(section, "Groundspeed", 1);
		diff.AirspeedBias = ini.Get_Float(section, "Airspeed", 1);
		diff.ArmorBias = ini.Get_Float(section, "Armor", 1);
		diff.ROFBias = ini.Get_Float(section, "ROF", 1);
		diff.CostBias = ini.Get_Float(section, "Cost", 1);
		diff.RepairDelay = ini.Get_Float(section, "RepairDelay", .02);
		diff.BuildDelay = ini.Get_Float(section, "BuildDelay", .03);
		diff.IsBuildSlowdown = ini.Get_Bool(section, "BuildSlowdown", false);
		diff.BuildSpeedBias = ini.Get_Float(section, "BuildTime", 1);
		diff.IsWallDestroyer = ini.Get_Bool(section, "DestroyWalls", true);
		diff.IsContentScan = ini.Get_Bool(section, "ContentScan", false);
	}
}

// FUNCTION: 0x0066D480
bool RulesClass::Read_ColorAdd(CCINIClass const & ini)
{
	static char const * COLORADD = "ColorAdd";
	if (ini.Is_Present(COLORADD)) {
		int count = ini.Entry_Count(COLORADD);
		for (int index = 0; index < count; index++) {
			char const * key = ini.Get_Entry(COLORADD, index);
			RGBClass color = ini.Get_RGBClass(COLORADD, key, RGBClass(0, 0, 0));
			ColorAdd[index] = color;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00672230
bool RulesClass::Heap_Maximums(CCINIClass const & ini)
{
	static char const * MAXIMUMS = "Maximums";
	if (ini.Is_Present(MAXIMUMS)) {
		MaxPlayers = ini.Get_Int(MAXIMUMS, "Players", MaxPlayers);
	}
	return(true);
}



// FUNCTION: 0x006722F0
bool RulesClass::Do_HouseTypes(CCINIClass const & ini)
{
	static char const * HOUSES = "Countries";
	char buffer[32];
	int count = ini.Entry_Count(HOUSES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(HOUSES, ini.Get_Entry(HOUSES, i), "", buffer, sizeof(buffer))) {
			HouseTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x006725F0
bool RulesClass::Do_SuperWeaponTypes(CCINIClass const & ini)
{
	static char const * SUPERWEAPONTYPES = "SuperWeaponTypes";
	char buffer[32];
	int count = ini.Entry_Count(SUPERWEAPONTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(SUPERWEAPONTYPES, ini.Get_Entry(SUPERWEAPONTYPES, i), "", buffer, sizeof(buffer))) {
			SuperWeaponTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x006726D0
bool RulesClass::Do_TerrainTypes(CCINIClass const & ini)
{
	static char const * TERRAINTYPES = "TerrainTypes";
	char buffer[32];
	int count = ini.Entry_Count(TERRAINTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(TERRAINTYPES, ini.Get_Entry(TERRAINTYPES, i), "", buffer, sizeof(buffer))) {
			TerrainTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x006727D0
bool RulesClass::Do_SmudgeTypes(CCINIClass const & ini)
{
	static char const * SMUDGETYPES = "SmudgeTypes";
	char buffer[32];
	int count = ini.Entry_Count(SMUDGETYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(SMUDGETYPES, ini.Get_Entry(SMUDGETYPES, i), "", buffer, sizeof(buffer))) {
			SmudgeTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x00672840
bool RulesClass::Do_OverlayTypes(CCINIClass const & ini)
{
	static char const * OVERLAYTYPES = "OverlayTypes";
	char buffer[32];
	int count = ini.Entry_Count(OVERLAYTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(OVERLAYTYPES, ini.Get_Entry(OVERLAYTYPES, i), "", buffer, sizeof(buffer))) {
			OverlayTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

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
