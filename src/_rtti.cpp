// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "geometry.h"
#include "_rtti.h"
#include "object.h"
#include "_script.h"
#include "ccini.h"
#include "stimer.h"
#include "timer.h"
#include <cmath>

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
// FUNCTION: 0x0040E310 $E78
// FUNCTION: 0x0040E320 GenericNode::GenericNode
// FUNCTION: 0x0040E340 GenericNode::Link
// FUNCTION: 0x0040E390 ~GenericNode
// FUNCTION: 0x0040E3E0 ~GenericList
// FUNCTION: 0x0040E4A0 ~CCINIClass
// FUNCTION: 0x0040E4C0 ~?$List@PAUINISection@INIClass@@

namespace fastmath { double __cdecl sqrt(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1B10;
extern const double DAT_007E1B18;

// FUNCTION: 0x0040E580 $E83
double DAT_00887288 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0040E5B0 $E86
double DAT_008872F0 = DAT_00887288 / DAT_007E1B10;

// FUNCTION: 0x0040E5D0 $E89
double DAT_008872D8 = DAT_007E1718 / DAT_008872F0;

// FUNCTION: 0x0040E5F0 $E92
double DAT_008872D0 = DAT_007E1720 * DAT_007E1B18;

// FUNCTION: 0x0040E610 $E95
double DAT_00887290 = DAT_007E1728 * DAT_007E1B18;

extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
namespace fastmath { double __cdecl tan(double x); double __cdecl atan(double x); }

// FUNCTION: 0x0040E630 $E98
double DAT_00887278 = DAT_007E1730 * DAT_007E1B18;

// FUNCTION: 0x0040E650 $E101
int DAT_008872F8 = (int)(DAT_007E1738 * fastmath::tan(DAT_00887278 - DAT_00887290) * DAT_00887288);

// FUNCTION: 0x0040E690 $E104
double DAT_00887298 = fastmath::atan(DAT_008872F8 * DAT_007E1740);

// FUNCTION: 0x0040E6C0 $E107
double DAT_008872C8 = fastmath::atan((DAT_008872F8 * 2) / DAT_00887288);

// FUNCTION: 0x0040E6F0 $E110
int DAT_008872EC = (int)((DAT_008872F8 * 4) + DAT_007E1738);

// FUNCTION: 0x0040E720 $E113
Cell DAT_00887280(0, 0);

// FUNCTION: 0x0040E730 $E116
Coord DAT_008872A0(0, 0, 0);

// FUNCTION: 0x0040E750 $E119
Coord DAT_008872E0(128, 128, 0);

// FUNCTION: 0x0040E770 $E124
DynamicVectorClass<ScriptClass *> Scripts;

// FUNCTION: 0x0040E7B0 $E122

// FUNCTION: 0x0040E7F0 ?$DynamicVectorClass@PAVScriptClass@@::Clear
template class DynamicVectorClass<ScriptClass *>;

// FUNCTION: 0x0040E820 ?$DynamicVectorClass@PAVScriptClass@@::ID

// FUNCTION: 0x0040E840 ?$VectorClass@PAVScriptClass@@::GetItem
// FUNCTION: 0x0040E850 ?$VectorClass@PAVScriptClass@@::operator==
// FUNCTION: 0x0040E890 ?$VectorClass@PAVScriptClass@@::Resize
// FUNCTION: 0x0040E940 ?$VectorClass@PAVScriptClass@@::Clear
// FUNCTION: 0x0040E970 ?$VectorClass@PAVScriptClass@@::ID
// FUNCTION: 0x0040E9B0 ?$VectorClass@PAVScriptClass@@::ID
// FUNCTION: 0x0040E9D0 ?$DynamicVectorClass@PAVScriptClass@@::Resize
// FUNCTION: 0x0040EA80 ?$DynamicVectorClass@PAVScriptClass@@::ID
// FUNCTION: 0x0040EAB0 ~?$VectorClass@PAVScriptClass@@
// FUNCTION: 0x0040EB00 ~?$DynamicVectorClass@PAVScriptClass@@

// FUNCTION: 0x0040EB50 $E129
// FUNCTION: 0x0040EB80 $E127
CDTimerClass<SystemTimerClass> DAT_00887348;
