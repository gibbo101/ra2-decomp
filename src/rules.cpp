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
#include "multiplayer.h"
#include "overtype.h"
#include "psystype.h"
#include "ptype.h"
#include "scheme.h"
#include "session.h"
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

namespace Powerups {
extern char const * Effects[19];
extern int Weights[19];
extern AnimType Anims[19];
extern double Arguments[19];
extern bool Naval[19];
}
void __fastcall FUN_006CFD20(int button, int slot);
int __fastcall FUN_006CFCC0(char const * name);
void __fastcall FUN_006CFDB0(int count);
extern int DAT_008427CC;
template<> void VectorClass<ColorScheme *>::Clear(void);
void FUN_006269A0(void);
void __fastcall FUN_00626AB0(char const * name, HSVClass & hsv);
void Difficulty_Get(CCINIClass const & ini, DifficultyClass & diff, char const * section);

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

	if (Session.Type != GAME_NORMAL && Session.MPGameMode != NULL) {
		Addition(*Session.MPGameMode->FUN_005D67B0());
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

// FUNCTION: 0x0066D3A0
bool RulesClass::Color_Schemes(CCINIClass const & ini)
{
	static char const * COLORS = "Colors";
	if (ini.Is_Present(COLORS)) {
		int count = ini.Entry_Count(COLORS);
		for (int i = 0; i < count; i++) {
			char const * name = ini.Get_Entry(COLORS, i);
			HSVClass hsv = ini.Get_HSVClass(COLORS, name, HSVClass(0, 0, 0));
			FUN_00626AB0(name, hsv);
			ColorScheme::Find_Or_Make(name, hsv, SchemePalette, GamePalette, 1);
			ColorScheme::Find_Or_Make(name, hsv, SchemePalette, GamePalette, 0x35);
		}
		return(true);
	}
	return(false);
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

// FUNCTION: 0x00671EA0
bool RulesClass::MPlayer(CCINIClass const & ini)
{
	static char const * MPLAYER = "MultiplayerDialogSettings";
	if (ini.Is_Present(MPLAYER)) {
		MinMoney = ini.Get_Int(MPLAYER, "MinMoney", MinMoney);
		MPMoney = ini.Get_Int(MPLAYER, "Money", MPMoney);
		MPMaxMoney = ini.Get_Int(MPLAYER, "MaxMoney", MPMaxMoney);
		MoneyIncrement = ini.Get_Int(MPLAYER, "MoneyIncrement", MoneyIncrement);
		MinUnitCount = ini.Get_Int(MPLAYER, "MinUnitCount", MinUnitCount);
		MPUnitCount = ini.Get_Int(MPLAYER, "UnitCount", MPUnitCount);
		MaxUnitCount = ini.Get_Int(MPLAYER, "MaxUnitCount", MaxUnitCount);
		MPBuildLevel = ini.Get_Int(MPLAYER, "TechLevel", MPBuildLevel);
		GameSpeed = ini.Get_Int(MPLAYER, "GameSpeed", GameSpeed);
		AIDifficultyStruct = ini.Get_Int(MPLAYER, "AIDifficulty", AIDifficultyStruct);
		AIPlayers = ini.Get_Int(MPLAYER, "AIPlayers", AIPlayers);
		IsMPBridgeDestruction = ini.Get_Bool(MPLAYER, "BridgeDestruction", IsMPBridgeDestruction);
		IsMPShadowGrow = ini.Get_Bool(MPLAYER, "ShadowGrow", IsMPShadowGrow);
		Shroud = ini.Get_Bool(MPLAYER, "Shroud", Shroud);
		IsMPBasesOn = ini.Get_Bool(MPLAYER, "Bases", IsMPBasesOn);
		IsMPTiberiumGrow = ini.Get_Bool(MPLAYER, "TiberiumGrows", IsMPTiberiumGrow);
		IsMPCrates = ini.Get_Bool(MPLAYER, "Crates", IsMPCrates);
		IsMPCaptureTheFlag = ini.Get_Bool(MPLAYER, "CaptureTheFlag", IsMPCaptureTheFlag);
		HarvesterTruce = ini.Get_Bool(MPLAYER, "HarvesterTruce", HarvesterTruce);
		MultiEngineer = ini.Get_Bool(MPLAYER, "MultiEngineer", MultiEngineer);
		AlliesAllowed = ini.Get_Bool(MPLAYER, "AlliesAllowed", AlliesAllowed);
		AllyChangeAllowed = ini.Get_Bool(MPLAYER, "AllyChangeAllowed", AllyChangeAllowed);
		ShortGame = ini.Get_Bool(MPLAYER, "ShortGame", ShortGame);
		SuperWeaponsAllowed = ini.Get_Bool(MPLAYER, "SuperWeaponsAllowed", SuperWeaponsAllowed);
		BuildOffAlly = ini.Get_Bool(MPLAYER, "BuildOffAlly", BuildOffAlly);
		FogOfWar = ini.Get_Bool(MPLAYER, "FogOfWar", FogOfWar);
		MCVRedeploys = ini.Get_Bool(MPLAYER, "MCVRedeploys", MCVRedeploys);
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

// FUNCTION: 0x00672280
bool RulesClass::Do_InfantryTypes(CCINIClass const & ini)
{
	static char const * INFANTRYTYPES = "InfantryTypes";
	char buffer[32];
	int count = ini.Entry_Count(INFANTRYTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(INFANTRYTYPES, ini.Get_Entry(INFANTRYTYPES, i), "", buffer, sizeof(buffer))) {
			InfantryTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
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

// FUNCTION: 0x00672360
bool RulesClass::Do_VehicleTypes(CCINIClass const & ini)
{
	static char const * VEHICLETYPES = "VehicleTypes";
	char buffer[32];
	int count = ini.Entry_Count(VEHICLETYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(VEHICLETYPES, ini.Get_Entry(VEHICLETYPES, i), "", buffer, sizeof(buffer))) {
			UnitTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x006723D0
bool RulesClass::Do_AircraftTypes(CCINIClass const & ini)
{
	static char const * AIRCRAFTTYPES = "AircraftTypes";
	char buffer[32];
	int count = ini.Entry_Count(AIRCRAFTTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(AIRCRAFTTYPES, ini.Get_Entry(AIRCRAFTTYPES, i), "", buffer, sizeof(buffer))) {
			AircraftTypeClass::Find_Or_Make(buffer);
		}
	}
	return(count > 0);
}

// FUNCTION: 0x00672440
bool RulesClass::Do_Sides(CCINIClass const & ini)
{
	static char const * SIDES = "Sides";
	int count = ini.Entry_Count(SIDES);
	DebugString("Processing sides.\n");
	for (int i = 0; i < count; i++) {
		char const * name = ini.Get_Entry(SIDES, i);
		int side = SideClass::From_Name(name);
		SideClass * sidep;
		if (side == SIDE_NONE) {
			sidep = new SideClass(name);
		} else {
			sidep = ::Sides[side];
		}
		DebugString("Side %d: %s \n", i, (char const *)sidep->IniName);
		sidep->Houses = ini.Get_House_List(SIDES, name, sidep->Houses);
		for (int house = 0; house < sidep->Houses.Count(); house++) {
			side = sidep->Houses[house];
			HouseTypes[side]->Side = (SideType)::Sides.ID(sidep);
			DebugString("  %s\n", (char const *)HouseTypes[side]->IniName);
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

// FUNCTION: 0x00672660
bool RulesClass::Do_BuildingTypes(CCINIClass const & ini)
{
	static char const * BUILDINGTYPES = "BuildingTypes";
	char buffer[32];
	int count = ini.Entry_Count(BUILDINGTYPES);
	for (int i = 0; i < count; i++) {
		if (ini.Get_String(BUILDINGTYPES, ini.Get_Entry(BUILDINGTYPES, i), "", buffer, sizeof(buffer))) {
			BuildingTypeClass::Find_Or_Make(buffer);
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

// FUNCTION: 0x00672740
bool RulesClass::Do_TeamTypes(CCINIClass & ini)
{
	static char const * TEAMS = "Teams";
	int count = ini.Entry_Count(TEAMS);
	for (int i = 0; i < count; i++) {
		TeamTypeClass * team = TGet_Class<TeamTypeClass>(ini, TEAMS, ini.Get_Entry(TEAMS, i), NULL);
		team->Read_INI(ini);
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

// FUNCTION: 0x00673E80
bool RulesClass::Powerups(CCINIClass const & ini)
{
	static char const * POWERUPS = "Powerups";
	if (ini.Is_Present(POWERUPS)) {
		for (int crate = 0; crate < 19; crate++) {
			char buffer[128];
			if (ini.Get_String(POWERUPS, ::Powerups::Effects[crate], "0,NONE", buffer, sizeof(buffer))) {
				char * token = strtok(buffer, ",");
				if (token) {
					strtrim(token);
					::Powerups::Weights[crate] = atoi(token);
				}
				token = strtok(NULL, ",");
				if (token) {
					strtrim(token);
					::Powerups::Anims[crate] = Anim_From_Name(token);
				}
				token = strtok(NULL, ",");
				if (token != NULL) {
					strtrim(token);
					if (stricmp(token, "yes") == 0) {
						::Powerups::Naval[crate] = true;
					} else if (stricmp(token, "no") == 0) {
						::Powerups::Naval[crate] = false;
					}
				}
				token = strtok(NULL, ",");
				if (token != NULL) {
					if (strchr(token, '%') != NULL) {
						::Powerups::Arguments[crate] = atof(token) * 0.01;
					} else {
						strtrim(token);
						::Powerups::Arguments[crate] = atof(token);
					}
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00674000
bool RulesClass::Land_Types(CCINIClass const & ini)
{
	static char const * _lands[LAND_COUNT] = {
		"Clear", "Road", "Water", "Rock", "Wall", "Tiberium",
		"Beach", "Rough", "Ice", "Railroad", "Tunnel", "Weeds",
	};
	for (int land = LAND_FIRST; land < LAND_COUNT; land++) {
		GroundType * gptr = &Ground[land];
		if (ini.Is_Present(_lands[land])) {
			gptr->Cost[SPEED_HOVER] = MIN(ini.Get_Float(_lands[land], "Hover", 1.0), 1.0);
			gptr->Cost[SPEED_FOOT] = MIN(ini.Get_Float(_lands[land], "Foot", 1.0), 1.0);
			gptr->Cost[SPEED_TRACK] = MIN(ini.Get_Float(_lands[land], "Track", 1.0), 1.0);
			gptr->Cost[SPEED_WHEEL] = MIN(ini.Get_Float(_lands[land], "Wheel", 1.0), 1.0);
			gptr->Cost[SPEED_WINGED] = 1.0f;
			gptr->Cost[SPEED_FLOAT] = MIN(ini.Get_Float(_lands[land], "Float", 1.0), 1.0);
			gptr->Cost[SPEED_AMPHIBIOUS] = MIN(ini.Get_Float(_lands[land], "Amphibious", 1.0), 1.0);
			gptr->Cost[SPEED_CREEP] = MIN(ini.Get_Float(_lands[land], "FloatBeach", 1.0), 1.0);
			gptr->Build = ini.Get_Bool(_lands[land], "Buildable", false);
		}
	}
	return(true);
}

// FUNCTION: 0x00674240
bool RulesClass::IQ(CCINIClass const & ini)
{
	static char const * IQCONTROL = "IQ";
	if (ini.Is_Present(IQCONTROL)) {
		MaxIQ = ini.Get_Int(IQCONTROL, "MaxIQLevels", MaxIQ);
		IQSuperWeapons = ini.Get_Int(IQCONTROL, "SuperWeapons", IQSuperWeapons);
		IQProduction = ini.Get_Int(IQCONTROL, "Production", IQProduction);
		IQGuardArea = ini.Get_Int(IQCONTROL, "GuardArea", IQGuardArea);
		IQRepairSell = ini.Get_Int(IQCONTROL, "RepairSell", IQRepairSell);
		IQCrush = ini.Get_Int(IQCONTROL, "AutoCrush", IQCrush);
		IQScatter = ini.Get_Int(IQCONTROL, "Scatter", IQScatter);
		IQContentScan = ini.Get_Int(IQCONTROL, "ContentScan", IQContentScan);
		IQAircraft = ini.Get_Int(IQCONTROL, "Aircraft", IQAircraft);
		IQHarvester = ini.Get_Int(IQCONTROL, "Harvester", IQHarvester);
		IQSellBack = ini.Get_Int(IQCONTROL, "SellBack", IQSellBack);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006743D0
bool RulesClass::Jumpjet_Controls(CCINIClass const & ini)
{
	static char const * SECTION = "JumpjetControls";
	if (ini.Is_Present(SECTION)) {
		JumpjetTurnRate = ini.Get_Int(SECTION, "TurnRate", JumpjetTurnRate);
		JumpjetSpeed = ini.Get_Int(SECTION, "Speed", JumpjetSpeed);
		JumpjetClimb = ini.Get_Float(SECTION, "Climb", JumpjetClimb);
		JumpjetCruiseHeight = ini.Get_Int(SECTION, "CruiseHeight", JumpjetCruiseHeight);
		JumpjetAcceleration = ini.Get_Float(SECTION, "Acceleration", JumpjetAcceleration);
		JumpjetWobblesPerSecond = ini.Get_Float(SECTION, "WobblesPerSecond", JumpjetWobblesPerSecond);
		JumpjetWobbleDeviation = ini.Get_Int(SECTION, "WobbleDeviation", JumpjetWobbleDeviation);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00674500
bool RulesClass::Difficulty(CCINIClass const & ini)
{
	Difficulty_Get(ini, Diff[DIFF_EASY], "Easy");
	Difficulty_Get(ini, Diff[DIFF_NORMAL], "Normal");
	Difficulty_Get(ini, Diff[DIFF_HARD], "Difficult");
	return(true);
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

// FUNCTION: 0x00674650
bool RulesClass::Read_AdvancedCommandBar(CCINIClass const & ini, bool multiplayer)
{
	static char const * BAR = "AdvancedCommandBar";
	static char const * MPBAR = "MultiplayerAdvancedCommandBar";
	static char const * BUTTONLIST = "ButtonList";
	static char const * NONE = "";
	char const * section = multiplayer ? MPBAR : BAR;
	char buffer[512];
	if (ini.Is_Present(section)) {
		for (int i = 0; i < 25; i++) {
			FUN_006CFD20(i, DAT_008427CC);
		}
		if (ini.Get_String(section, BUTTONLIST, NONE, buffer, sizeof(buffer))) {
			int count = 0;
			int slot = 0;
			for (char * token = strtok(buffer, ","); token; token = strtok(NULL, ",")) {
				int button = FUN_006CFCC0(token);
				if (button != DAT_008427CC) {
					FUN_006CFD20(button, slot);
				}
				count++;
				slot++;
			}
			FUN_006CFDB0(count);
		}
		return(true);
	}
	return(false);
}

extern unsigned int DAT_00B053B4;
extern bool DAT_00B053B8;
int FUN_005D5ED0(void);

// FUNCTION: 0x00679A10
bool RulesClass::Objects(CCINIClass & ini)
{
	for (int house = HOUSE_FIRST; house < HouseTypes.Count(); house++) {
		HouseTypes[house]->Read_INI(ini);
	}
	for (int index = 0; index < SuperWeaponTypes.Count(); index++) {
		SuperWeaponTypes[index]->Read_INI(ini);
	}
	for (int anim = 0; anim < AnimTypes.Count(); anim++) {
		AnimTypes[anim]->Read_INI(ArtINI);
	}
	int bindex;
	for (bindex = 0; bindex < BuildingTypes.Count(); bindex++) {
		BuildingTypes[bindex]->Read_INI(ini);
	}
	for (int aindex = 0; aindex < AircraftTypes.Count(); aindex++) {
		AircraftTypes[aindex]->Read_INI(ini);
	}
	for (int uindex = 0; uindex < UnitTypes.Count(); uindex++) {
		UnitTypes[uindex]->Read_INI(ini);
	}
	for (int iindex = 0; iindex < InfantryTypes.Count(); iindex++) {
		InfantryTypes[iindex]->Read_INI(ini);
	}
	int windex;
	for (windex = 0; windex < Weapons.Count(); windex++) {
		Weapons[windex]->Read_INI(ini);
	}
	for (int proj = 0; proj < BulletTypes.Count(); proj++) {
		BulletTypes[proj]->Read_INI(ini);
	}
	for (int whead = 0; whead < ::Warheads.Count(); whead++) {
		::Warheads[whead]->Read_INI(ini);
	}
	for (windex = 0; windex < Weapons.Count(); windex++) {
		Weapons[windex]->Init_Max_Speed();
	}
	for (bindex = 0; bindex < BuildingTypes.Count(); bindex++) {
		BuildingTypes[bindex]->Calculate_Base_Defense_Values();
	}
	for (int tindex = 0; tindex < TerrainTypes.Count(); tindex++) {
		TerrainTypes[tindex]->Read_INI(ini);
	}
	for (int sindex = 0; sindex < SmudgeTypes.Count(); sindex++) {
		SmudgeTypes[sindex]->Read_INI(ini);
	}
	for (int oindex = 0; oindex < OverlayTypes.Count(); oindex++) {
		OverlayTypes[oindex]->Read_INI(ini);
	}
	for (int pindex = 0; pindex < ParticleTypes.Count(); pindex++) {
		ParticleTypes[pindex]->Read_INI(ini);
	}
	for (int psindex = 0; psindex < ParticleSystemTypes.Count(); psindex++) {
		ParticleSystemTypes[psindex]->Read_INI(ini);
	}
	for (int vindex = 0; vindex < VoxelAnimTypes.Count(); vindex++) {
		VoxelAnimTypes[vindex]->Read_INI(ini);
	}
	for (int mission = MISSION_FIRST; mission < MISSION_COUNT; mission++) {
		MissionControlClass * miss = &MissionControl[mission];
		miss->Mission = (MissionType)mission;
		miss->Read_INI(ini);
	}
	return(true);
}

// FUNCTION: 0x00679CC0
int FUN_00679CC0(void)
{
	if (!DAT_00B053B8) {
		CCINIClass ini;
		CCFileClass file("CoopCampMD.ini");
		if (ini.ReadCCFile(file, false, false)) {
			DAT_00B053B4 = ini.Get_Unique_ID();
		}
		DAT_00B053B8 = true;
	}
	return DAT_00B053B4;
}

// FUNCTION: 0x00679D90
int RulesClass::Get_Rule_Unique_ID(void)
{
	int id = RuleINI->Get_Unique_ID();
	int count = FUN_005D5ED0();
	for (int index = 1; index <= count; index++) {
		MPGameModeClass * mode = MPGameModeClass::Get(index);
		if (mode != NULL) {
			id += mode->FUN_005D67B0()->Get_Unique_ID();
		}
	}
	return id + FUN_00679CC0();
}

// FUNCTION: 0x00679EC0
int RulesClass::Get_Art_Unique_ID(void)
{
	return ArtINI.Get_Unique_ID();
}

// FUNCTION: 0x00679ED0
int RulesClass::Get_AI_Unique_ID(void)
{
	return AIINI.Get_Unique_ID();
}

// FUNCTION: 0x00679EE0
void RulesClass::Load_Art_INI(void)
{
	ArtINI.Clear();
	CCFileClass art("ARTMD.INI");
	ArtINI.ReadCCFile(art, false, false);
}

// FUNCTION: 0x00679F70 ?$VectorClass@H::~?$VectorClass@H
template class VectorClass<int>;

class BuildingTypeClass;
class UnitTypeClass;
class AircraftTypeClass;
class InfantryTypeClass;

// FUNCTION: 0x00679FA0 ?$VectorClass@PBVBuildingTypeClass@@::~?$VectorClass@PBVBuildingTypeClass@@
template class VectorClass<BuildingTypeClass const *>;

// FUNCTION: 0x00679FD0 ?$VectorClass@PBVUnitTypeClass@@::~?$VectorClass@PBVUnitTypeClass@@
template class VectorClass<UnitTypeClass const *>;

// FUNCTION: 0x0067A000 ?$VectorClass@PBVAircraftTypeClass@@::~?$VectorClass@PBVAircraftTypeClass@@
template class VectorClass<AircraftTypeClass const *>;

// FUNCTION: 0x0067A030 ?$VectorClass@PBVInfantryTypeClass@@::~?$VectorClass@PBVInfantryTypeClass@@
template class VectorClass<InfantryTypeClass const *>;
