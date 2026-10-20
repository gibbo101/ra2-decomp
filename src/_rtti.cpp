// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "_rtti.h"
#include "object.h"
#include "ccini.h"

struct {
	char const *Name;
	RTTIType RTTI;
} RTTIs[RTTI_COUNT] = {
	{ "<none>",				RTTI_NONE },
	{ "Unit",				RTTI_UNIT },
	{ "Aircraft",			RTTI_AIRCRAFT },
	{ "AircraftType",		RTTI_AIRCRAFTTYPE },
	{ "Anim",				RTTI_ANIM },
	{ "AnimType",			RTTI_ANIMTYPE },
	{ "Building",			RTTI_BUILDING },
	{ "BuildingType",		RTTI_BUILDINGTYPE },
	{ "Bullet",				RTTI_BULLET },
	{ "BulletType",			RTTI_BULLETTYPE },
	{ "Campaign",			RTTI_CAMPAIGN },
	{ "Cell",				RTTI_CELL },
	{ "Factory",			RTTI_FACTORY },
	{ "House",				RTTI_HOUSE },
	{ "HouseType",			RTTI_HOUSETYPE },
	{ "Infantry",			RTTI_INFANTRY },
	{ "InfantryType",		RTTI_INFANTRYTYPE },
	{ "Isotile",			RTTI_ISOTILE },
	{ "IsotileType",		RTTI_ISOTILETYPE },
	{ "Light",				RTTI_LIGHT },
	{ "Overlay",			RTTI_OVERLAY },
	{ "OverlayType",		RTTI_OVERLAYTYPE },
	{ "Particle",			RTTI_PARTICLE },
	{ "ParticleType",		RTTI_PARTICLETYPE },
	{ "ParticleSystem",		RTTI_PARTICLESYSTEM },
	{ "ParticleSystemType",	RTTI_PARTICLESYSTEMTYPE },
	{ "Script",				RTTI_SCRIPT },
	{ "ScriptType",			RTTI_SCRIPTTYPE },
	{ "Side",				RTTI_SIDE },
	{ "Smudge",				RTTI_SMUDGE },
	{ "SmudgeType",			RTTI_SMUDGETYPE },
	{ "Special",			RTTI_SPECIAL },
	{ "SuperWeaponType",	RTTI_SUPERWEAPONTYPE },
	{ "TaskForce",			RTTI_TASKFORCE },
	{ "Team",				RTTI_TEAM },
	{ "TeamType",			RTTI_TEAMTYPE },
	{ "Terrain",			RTTI_TERRAIN },
	{ "TerrainType",		RTTI_TERRAINTYPE },
	{ "Trigger",			RTTI_TRIGGER },
	{ "TriggerType",		RTTI_TRIGGERTYPE },
	{ "UnitType",			RTTI_UNITTYPE },
	{ "VoxelAnim",			RTTI_VOXELANIM },
	{ "VoxelAnimType",		RTTI_VOXELANIMTYPE },
	{ "Wave",				RTTI_WAVE },
	{ "Tag",				RTTI_TAG },
	{ "TagType",			RTTI_TAGTYPE },
	{ "Tiberium",			RTTI_TIBERIUM },
	{ "Action",				RTTI_ACTION },
	{ "Event",				RTTI_EVENT },
	{ "WeaponType",			RTTI_WEAPONTYPE },
	{ "WarheadType",		RTTI_WARHEADTYPE },
	{ "Waypoint",			RTTI_WAYPOINT },
	{ "Abstract",			RTTI_ABSTRACT },
	{ "Tube",				RTTI_TUBE },
	{ "LightSource",		RTTI_LIGHTSOURCE },
	{ "EMPulse",			RTTI_EMPULSE },
	{ "TacticalMap",		RTTI_TACTICALMAP },
	{ "SuperWeapon",		RTTI_SUPERWEAPON },
	{ "AITrigger",			RTTI_AITRIGGER },
	{ "AITriggerType",		RTTI_AITRIGGERTYPE },
	{ "Neuron",				RTTI_NEURON },
	{ "FoggedObject",		RTTI_FOGGEDOBJECT },
	{ "AlphaShape",			RTTI_ALPHASHAPE },
	{ "VeinholeMonster",	RTTI_VEINHOLEMONSTER },
	{ "NavyType",			RTTI_NAVYTYPE },
	{ "SpawnManager",		RTTI_SPAWNMANAGER },
	{ "CaptureManager",		RTTI_CAPTUREMANAGER },
	{ "Parasite",			RTTI_PARASITE },
	{ "Bomb",				RTTI_BOMB },
	{ "Temporal",			RTTI_TEMPORAL },
	{ "RadSite",			RTTI_RADSITE },
	{ "Airstrike",			RTTI_AIRSTRIKE },
	{ "SlaveManager",		RTTI_SLAVEMANAGER },
	{ "DiskLaser",			RTTI_DISKLASER },
};

static char UNKNOWN_RTTI_NAME[] = { "Unknown" };

// FUNCTION: 0x0040DCB0
char const *Name_From_RTTI(RTTIType rtti)
{
	for (int i = 0; i < RTTI_COUNT; i++) {
		if (RTTIs[i].RTTI == rtti) {
			return(RTTIs[i].Name);
		}
	}
	return(UNKNOWN_RTTI_NAME);
}

// FUNCTION: 0x0040DCE0
RTTIType RTTI_From_Name(char const *name)
{
	for (int i = 0; i < RTTI_COUNT; i++) {
		if (!strcmpi(RTTIs[i].Name, name)) {
			return(RTTIs[i].RTTI);
		}
	}
	return(RTTI_NONE);
}

// FUNCTION: 0x0040DD20 Dynamic_Cast@@YIPBVTechnoClass
template <>
TechnoClass const * Dynamic_Cast<TechnoClass const *>(AbstractClass const * target)
{
	if (target != NULL) {
		RTTIType rtti = target->Fetch_RTTI();
		switch (rtti) {
			case RTTI_INFANTRY:
				return((TechnoClass *)target);
			case RTTI_UNIT:
				return((TechnoClass *)target);
			case RTTI_BUILDING:
				return((TechnoClass *)target);
			case RTTI_AIRCRAFT:
				return((TechnoClass *)target);
		}
	}
	return(NULL);
}

// FUNCTION: 0x0040DD70 Dynamic_Cast@@YIPAVTechnoClass
template <>
TechnoClass * Dynamic_Cast<TechnoClass *>(AbstractClass * target)
{
	if (target != NULL) {
		RTTIType rtti = target->Fetch_RTTI();
		switch (rtti) {
			case RTTI_INFANTRY:
				return((TechnoClass *)target);
			case RTTI_UNIT:
				return((TechnoClass *)target);
			case RTTI_BUILDING:
				return((TechnoClass *)target);
			case RTTI_AIRCRAFT:
				return((TechnoClass *)target);
		}
	}
	return(NULL);
}

// FUNCTION: 0x0040DDC0 $E2
// FUNCTION: 0x0040DDF0 $E5
// FUNCTION: 0x0040DE10 $E8
// FUNCTION: 0x0040DE30 $E11
// FUNCTION: 0x0040DE50 $E14
// FUNCTION: 0x0040DE70 $E17
// FUNCTION: 0x0040DE90 $E20
// FUNCTION: 0x0040DED0 $E23
// FUNCTION: 0x0040DF00 $E26
// FUNCTION: 0x0040DF30 $E29
// FUNCTION: 0x0040DF60 $E32
// FUNCTION: 0x0040DF70 $E35
// FUNCTION: 0x0040DF90 $E38

CCINIClass ArtINI;
CCINIClass UIMDINI;
CCINIClass AIINI;
CCINIClass DAT_00887058;
CCINIClass RA2MDINI;

// FUNCTION: 0x0040DFB0 $E60
// FUNCTION: 0x0040E050 $E58
// FUNCTION: 0x0040E060 $E65
// FUNCTION: 0x0040E100 $E63
// FUNCTION: 0x0040E110 $E70
// FUNCTION: 0x0040E1B0 $E68
// FUNCTION: 0x0040E1C0 $E75
// FUNCTION: 0x0040E260 $E73
// FUNCTION: 0x0040E270 $E80
