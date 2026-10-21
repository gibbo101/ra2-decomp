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
#include "mstimer.h"
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

// FUNCTION: GAMEMD 0x0040DCB0
char const *Name_From_RTTI(RTTIType rtti)
{
	for (int i = 0; i < RTTI_COUNT; i++) {
		if (RTTIs[i].RTTI == rtti) {
			return(RTTIs[i].Name);
		}
	}
	return(UNKNOWN_RTTI_NAME);
}

// FUNCTION: GAMEMD 0x0040DCE0
RTTIType RTTI_From_Name(char const *name)
{
	for (int i = 0; i < RTTI_COUNT; i++) {
		if (!strcmpi(RTTIs[i].Name, name)) {
			return(RTTIs[i].RTTI);
		}
	}
	return(RTTI_NONE);
}

// FUNCTION: GAMEMD 0x0040DD20
// Dynamic_Cast@@YIPBVTechnoClass
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

// FUNCTION: GAMEMD 0x0040DD70
// Dynamic_Cast@@YIPAVTechnoClass
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

// SYNTHETIC: GAMEMD 0x0040DDC0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0040DDF0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040DE10 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0040DE30 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0040DE50 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0040DE70 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0040DE90 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0040DED0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0040DF00 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0040DF30 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0040DF60 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0040DF70 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0040DF90 SYMBOL
// _$E38

CCINIClass ArtINI;
CCINIClass UIMDINI;
CCINIClass AIINI;
CCINIClass DAT_00887058;
CCINIClass RA2MDINI;

// SYNTHETIC: GAMEMD 0x0040DFB0 SYMBOL
// _$E60
// SYNTHETIC: GAMEMD 0x0040E050 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x0040E060 SYMBOL
// _$E65
// SYNTHETIC: GAMEMD 0x0040E100 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x0040E110 SYMBOL
// _$E70
// SYNTHETIC: GAMEMD 0x0040E1B0 SYMBOL
// _$E68
// SYNTHETIC: GAMEMD 0x0040E1C0 SYMBOL
// _$E75
// SYNTHETIC: GAMEMD 0x0040E260 SYMBOL
// _$E73
// SYNTHETIC: GAMEMD 0x0040E270 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x0040E310 SYMBOL
// _$E78
// FUNCTION: GAMEMD 0x0040E320
// GenericNode::GenericNode
// FUNCTION: GAMEMD 0x0040E340
// GenericNode::Link
// SYNTHETIC: GAMEMD 0x0040E390
// ??_GGenericNode@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0040E3E0
// ??_GGenericList@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0040E4A0
// ??_GCCINIClass@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0040E4C0
// ??_G?$List@PAUINISection@INIClass@@@@UAEPAXI@Z

namespace fastmath { double __cdecl sqrt(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1B10;
extern const double DAT_007E1B18;

// SYNTHETIC: GAMEMD 0x0040E580 SYMBOL
// _$E83
double DAT_00887288 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0040E5B0 SYMBOL
// _$E86
double DAT_008872F0 = DAT_00887288 / DAT_007E1B10;

// SYNTHETIC: GAMEMD 0x0040E5D0 SYMBOL
// _$E89
double DAT_008872D8 = DAT_007E1718 / DAT_008872F0;

// SYNTHETIC: GAMEMD 0x0040E5F0 SYMBOL
// _$E92
double DAT_008872D0 = DAT_007E1720 * DAT_007E1B18;

// SYNTHETIC: GAMEMD 0x0040E610 SYMBOL
// _$E95
double DAT_00887290 = DAT_007E1728 * DAT_007E1B18;

extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
namespace fastmath { double __cdecl tan(double x); double __cdecl atan(double x); }

// SYNTHETIC: GAMEMD 0x0040E630 SYMBOL
// _$E98
double DAT_00887278 = DAT_007E1730 * DAT_007E1B18;

// SYNTHETIC: GAMEMD 0x0040E650 SYMBOL
// _$E101
int DAT_008872F8 = (int)(DAT_007E1738 * fastmath::tan(DAT_00887278 - DAT_00887290) * DAT_00887288);

// SYNTHETIC: GAMEMD 0x0040E690 SYMBOL
// _$E104
double DAT_00887298 = fastmath::atan(DAT_008872F8 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0040E6C0 SYMBOL
// _$E107
double DAT_008872C8 = fastmath::atan((DAT_008872F8 * 2) / DAT_00887288);

// SYNTHETIC: GAMEMD 0x0040E6F0 SYMBOL
// _$E110
int DAT_008872EC = (int)((DAT_008872F8 * 4) + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0040E720 SYMBOL
// _$E113
Cell DAT_00887280(0, 0);

// SYNTHETIC: GAMEMD 0x0040E730 SYMBOL
// _$E116
Coord DAT_008872A0(0, 0, 0);

// SYNTHETIC: GAMEMD 0x0040E750 SYMBOL
// _$E119
Coord DAT_008872E0(128, 128, 0);

// SYNTHETIC: GAMEMD 0x0040E770 SYMBOL
// _$E124
DynamicVectorClass<ScriptClass *> Scripts;

// SYNTHETIC: GAMEMD 0x0040E7B0 SYMBOL
// _$E122

// TEMPLATE: GAMEMD 0x0040E7F0
// ?Clear@?$DynamicVectorClass@PAVScriptClass@@@@UAEXXZ
template class DynamicVectorClass<ScriptClass *>;

// TEMPLATE: GAMEMD 0x0040E820
// ?ID@?$DynamicVectorClass@PAVScriptClass@@@@UAEHPBQAVScriptClass@@@Z

// TEMPLATE: GAMEMD 0x0040E840
// ?GetItem@?$VectorClass@PAVScriptClass@@@@UBEPAVScriptClass@@H@Z
// TEMPLATE: GAMEMD 0x0040E850
// ??8?$VectorClass@PAVScriptClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0040E890
// ?Resize@?$VectorClass@PAVScriptClass@@@@UAE_NHPBQAVScriptClass@@@Z
// TEMPLATE: GAMEMD 0x0040E940
// ?Clear@?$VectorClass@PAVScriptClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040E970
// ?ID@?$VectorClass@PAVScriptClass@@@@UAEHABQAVScriptClass@@@Z
// TEMPLATE: GAMEMD 0x0040E9B0
// ?ID@?$VectorClass@PAVScriptClass@@@@UAEHPBQAVScriptClass@@@Z
// TEMPLATE: GAMEMD 0x0040E9D0
// ?Resize@?$DynamicVectorClass@PAVScriptClass@@@@UAE_NHPBQAVScriptClass@@@Z
// TEMPLATE: GAMEMD 0x0040EA80
// ?ID@?$DynamicVectorClass@PAVScriptClass@@@@UAEHABQAVScriptClass@@@Z
// SYNTHETIC: GAMEMD 0x0040EAB0
// ??_G?$VectorClass@PAVScriptClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0040EB00
// ??_G?$DynamicVectorClass@PAVScriptClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0040EB50 SYMBOL
// _$E129
// SYNTHETIC: GAMEMD 0x0040EB80 SYMBOL
// _$E127
CDTimerClass<SystemTimerClass> DAT_00887348;

// SYNTHETIC: GAMEMD 0x0040EB90 SYMBOL
// _$E134
// SYNTHETIC: GAMEMD 0x0040EBC0 SYMBOL
// _$E132
CDTimerClass<MillisecondSystemTimerClass> DAT_00887328;

// SYNTHETIC: GAMEMD 0x0040EBD0 SYMBOL
// _$E139
// SYNTHETIC: GAMEMD 0x0040EC00 SYMBOL
// _$E137
CDTimerClass<SystemTimerClass> DAT_00887338;

struct Dat4 {
	int a, b, c, d;
	Dat4(void) : a(0), b(0), c(0), d(0) {}
};

// SYNTHETIC: GAMEMD 0x0040EC10 SYMBOL
// _$E142
Dat4 DAT_00887358;
