// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "unittype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_rules.h"
#include "bsurface.h"
#include "convert.h"
#include "face.h"
#include "findmake.h"
#include "globals.h"
#include "inline.h"
#include "mixfile.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "tracker.h"
#include "unit.h"
#include "ccini.h"
#include "color.hh"

#include <cstdio>
#include <cstring>
#include <stdlib.h>

// SYNTHETIC: GAMEMD 0x00746EE0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00746F10 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00746F30 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00746F50 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00746F70 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00746F90 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00746FB0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00746FF0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00747020 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00747050 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00747080 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00747090 SYMBOL
// _$E35

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
	static char TEMPERAT_PAL[];
};

// SYNTHETIC: GAMEMD 0x007470B0 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x007470D0
UnitTypeClass::UnitTypeClass(char const * ininame) :
	BASECLASS(ininame, SPEED_NONE),
	MovementRestrictedTo((LandType)-1),
	HalfDamageSmokeLocation(0, 0, 0),
	Passive(false),
	CrateGoodie(false),
	Harvester(false),
	Weeder(false),
	unknown_E10(false),
	HasTurret(false),
	DeployToFire(false),
	IsSimpleDeployer(false),
	IsTilter(true),
	UseTurretShadow(false),
	TooBigToFitUnderBridge(false),
	CanBeach(false),
	SmallVisceroid(false),
	LargeVisceroid(false),
	CarriesCrate(false),
	NonVehicle(false),
	StandingFrames(0),
	DeathFrames(0),
	DeathFrameRate(1),
	StartStandFrame(-1),
	StartWalkFrame(-1),
	StartFiringFrame(-1),
	StartDeathFrame(-1),
	MaxDeathCounter(-1),
	Facings(8),
	WalkFrames(12),
	FiringFrames(0),
	HeapID((UnitType)-1),
	AltImage(NULL)
{
	Create_ID();
	Rotation = 32;
	UnitTypes.Add(this);
	HeapID = (UnitType)UnitTypes.ID(this);
	for (int i = 0; i < 2; i++) FiringSyncFrame[i] = -1;
	for (int j = 0; j < 4; j++) BurstDelay[j] = -1;
	ImmuneToPsionics = false;
	ImmuneToPsionicWeapons = false;
	ImmuneToPoison = false;
	Parasiteable = true;
	Organic = false;
	ConsideredAircraft = false;
	Bunkerable = true;
}

// FUNCTION: GAMEMD 0x007472C0
UnitTypeClass::UnitTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	AltImageFile(x)
{
}

// FUNCTION: GAMEMD 0x007472F0
UnitTypeClass::~UnitTypeClass(void)
{
	Detach_This_From_All(this, true);
	UnitTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x00747370
UnitType UnitTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int classid = 0; classid < UnitTypes.Count(); classid++) {
			if (stricmp(UnitTypes[classid]->Name(), name) == 0) {
				return(UnitType(classid));
			}
		}
	}
	return(UNIT_NONE);
}

// FUNCTION: GAMEMD 0x007473C0
bool UnitTypeClass::CanUseWaypoint(void) const
{
	return(BASECLASS::CanUseWaypoint());
}

// FUNCTION: GAMEMD 0x007473D0
bool UnitTypeClass::CanAttackMove(void) const
{
	return(BASECLASS::CanAttackMove());
}

// FUNCTION: GAMEMD 0x007473E0
void UnitTypeClass::One_Time(void)
{
	if (EightBitSurface == NULL) {
		EightBitSurface = new BSurface(256, 256, 1);
		EightBitSurface->Fill(0);
	}
	if (EightBitDrawer == NULL) {
		EightBitDrawer = new ConvertClass(*(PaletteClass *)FileSystem::TEMPERAT_PAL, *(PaletteClass *)FileSystem::TEMPERAT_PAL, *EightBitSurface, 1, false);
	}
	SmallVisceroidShapes = FileSystem::LoadFile("VISC_SML.SHP", false);
	LargeVisceroidShapes = FileSystem::LoadFile("VISC_LRG.SHP", false);
}

// FUNCTION: GAMEMD 0x007474B0
bool UnitTypeClass::Create_And_Place(Cell const & cell, HouseClass * house) const
{
	UnitClass * unit = new UnitClass(this, house);
	if (unit != NULL) {
		DirType dir = Random_Dir(DIR_N, DIR_MAX);
		return(unit->Unlimbo(cell, dir.As_Dir256()));
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00747560
ObjectClass * UnitTypeClass::Create_One_Of(HouseClass * house) const
{
	return(new UnitClass(this, house));
}

// FUNCTION: GAMEMD 0x00747590
Point3D UnitTypeClass::Pixel_Dimensions(void) const
{
	int width = MaxSize - (MaxSize / 4);
	width = min(width, 60);
	int height = MaxSize - (MaxSize / 4);
	height = min(height, 60);
	return(Point3D(width, width, height));
}

// FUNCTION: GAMEMD 0x007475D0
Point3D UnitTypeClass::Lepton_Dimensions(void) const
{
	return(Point3D(CELL_LEPTON_W, CELL_LEPTON_H, LEVEL_LEPTON_H * 2));
}

// FUNCTION: GAMEMD 0x00747600
TPoint2D<int> UnitTypeClass::Turret_Adjust(Dir256 dir, TPoint2D<int> const & xy) const
{
	return(xy);
}

// FUNCTION: GAMEMD 0x00747620
bool UnitTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		CrateGoodie = ini.Get_Bool(Name(), "CrateGoodie", CrateGoodie);
		DeployToFire = ini.Get_Bool(Name(), "DeployToFire", DeployToFire);
		IsSimpleDeployer = ini.Get_Bool(Name(), "IsSimpleDeployer", IsSimpleDeployer);
		Harvester = ini.Get_Bool(Name(), "Harvester", Harvester);
		Weeder = ini.Get_Bool(Name(), "Weeder", Weeder);

		if (Speed == (SpeedType)-1) {
			Speed = IsCrusher ? (SpeedType)1 : (SpeedType)2;
		}

		Speed = ini.Get_SpeedType(Name(), "SpeedType", Speed);

		IsTilter = ini.Get_Bool(Name(), "IsTilter", IsTilter);
		CarriesCrate = ini.Get_Bool(Name(), "CarriesCrate", CarriesCrate);
		HasTurret = !IsTurretEquipped;
		TooBigToFitUnderBridge = ini.Get_Bool(Name(), "TooBigToFitUnderBridge", TooBigToFitUnderBridge);

		HalfDamageSmokeLocation = ini.Get_Point(Name(), "HalfDamageSmokeLocation", HalfDamageSmokeLocation);

		InitialMission = (MissionType)15;
		if (Harvester || Weeder) {
			InitialMission = (MissionType)10;
		}

		UseTurretShadow = ArtINI.Get_Bool(Graphic_Name(), "UseTurretShadow", UseTurretShadow);
		WalkFrames = ArtINI.Get_Int(Graphic_Name(), "WalkFrames", WalkFrames);
		FiringFrames = ArtINI.Get_Int(Graphic_Name(), "FiringFrames", FiringFrames);
		Passive = ini.Get_Bool(Name(), "Passive", Passive);
		MovementRestrictedTo = ini.Get_LandType(Name(), "MovementRestrictedTo", MovementRestrictedTo);
		CanBeach = ini.Get_Bool(Name(), "CanBeach", CanBeach);
		SmallVisceroid = ini.Get_Bool(Name(), "SmallVisceroid", SmallVisceroid);
		LargeVisceroid = ini.Get_Bool(Name(), "LargeVisceroid", LargeVisceroid);
		NonVehicle = ini.Get_Bool(Name(), "NonVehicle", NonVehicle);

		if (FiringFrames > 0) {
			StandingFrames = 1;
		}

		StandingFrames = ArtINI.Get_Int(Graphic_Name(), "StandingFrames", StandingFrames);
		DeathFrames = ArtINI.Get_Int(Graphic_Name(), "DeathFrames", DeathFrames);
		DeathFrameRate = ArtINI.Get_Int(Graphic_Name(), "DeathFrameRate", DeathFrameRate);

		if (DeathFrameRate < 1) {
			DeathFrameRate = 1;
		}

		if (!FiringFrames && !IsTurretEquipped) {
			Facings = 1;
		}

		Facings = ArtINI.Get_Int(Graphic_Name(), "Facings", Facings);

		if (StartWalkFrame == -1) {
			StartWalkFrame = 0;
		}

		if (StartStandFrame == -1) {
			StartStandFrame = StandingFrames == 0 ? StartWalkFrame : Facings * WalkFrames;
		}

		if (StartFiringFrame == -1) {
			StartFiringFrame = FiringFrames == 0 ? StartStandFrame : Facings * (StandingFrames + WalkFrames);
		}

		if (StartDeathFrame == -1) {
			StartDeathFrame = DeathFrames == 0 ? -1 : Facings * (FiringFrames + WalkFrames + 1);
			MaxDeathCounter = StartDeathFrame + DeathFrames;
		}

		StartStandFrame = ArtINI.Get_Int(Graphic_Name(), "StartStandFrame", StartStandFrame);
		StartWalkFrame = ArtINI.Get_Int(Graphic_Name(), "StartWalkFrame", StartWalkFrame);
		StartFiringFrame = ArtINI.Get_Int(Graphic_Name(), "StartFiringFrame", StartFiringFrame);
		StartDeathFrame = ArtINI.Get_Int(Graphic_Name(), "StartDeathFrame", StartDeathFrame);
		MaxDeathCounter = ArtINI.Get_Int(Graphic_Name(), "MaxDeathCounter", MaxDeathCounter);

		for (int i = 0; i < 2; i++) {
			char buffer[20];
			sprintf(buffer, "FiringSyncFrame%d", i);
			FiringSyncFrame[i] = ArtINI.Get_Int(Graphic_Name(), buffer, FiringSyncFrame[i]);
		}

		for (int j = 0; j < 4; j++) {
			char buffer[20];
			sprintf(buffer, "BurstDelay%d", j);
			BurstDelay[j] = ini.Get_Int(Name(), buffer, BurstDelay[j]);
		}

		char defvalue[25];
		strcpy(defvalue, AltImageFile);
		ini.Get_String(Name(), "AltImage", defvalue, AltImageFile, sizeof(AltImageFile));

		char filename[260];
		_makepath(filename, NULL, NULL, AltImageFile, ".SHP");
		AltImage = FileSystem::LoadFile(filename, false);

		if (*this == "FV") {
			int index = ini.Get_Int(Name(), "NormalTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "NormalTurretWeapon", -1));
			index = ini.Get_Int(Name(), "RepairTurretIndex", 1);
			FUN_00717890(index, ini.Get_Int(Name(), "RepairTurretWeapon", -1));
			index = ini.Get_Int(Name(), "MachineGunTurretIndex", 2);
			FUN_00717890(index, ini.Get_Int(Name(), "MachineGunTurretWeapon", -1));
			index = ini.Get_Int(Name(), "FlakTurretIndex", 3);
			FUN_00717890(index, ini.Get_Int(Name(), "FlakTurretWeapon", -1));
			index = ini.Get_Int(Name(), "PistolTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "PistolTurretWeapon", -1));
			index = ini.Get_Int(Name(), "SniperTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "SniperTurretWeapon", -1));
			index = ini.Get_Int(Name(), "ShockTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "ShockTurretWeapon", -1));
			index = ini.Get_Int(Name(), "ExplodeTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "ExplodeTurretWeapon", -1));
			index = ini.Get_Int(Name(), "BrainBlastTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "BrainBlastTurretWeapon", -1));
			index = ini.Get_Int(Name(), "RadCannonTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "RadCannonTurretWeapon", -1));
			index = ini.Get_Int(Name(), "ChronoTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "ChronoTurretWeapon", -1));
			index = ini.Get_Int(Name(), "TerroristExplodeTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "TerroristExplodeTurretWeapon", -1));
			index = ini.Get_Int(Name(), "CowTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "CowTurretWeapon", -1));
			index = ini.Get_Int(Name(), "InitiateTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "InitiateTurretWeapon", -1));
			index = ini.Get_Int(Name(), "VirusTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "VirusTurretWeapon", -1));
			index = ini.Get_Int(Name(), "YuriPrimeTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "YuriPrimeTurretWeapon", -1));
			index = ini.Get_Int(Name(), "GuardianTurretIndex", 0);
			FUN_00717890(index, ini.Get_Int(Name(), "GuardianTurretWeapon", -1));
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00747EB0
Coord const UnitTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord fix = coord;
	if (fix.Z < Map.Get_Height_GL(fix)) {
		fix.Z = Map.Get_Height_GL(fix);
	}
	return(fix);
}

// FUNCTION: GAMEMD 0x00747F20
int UnitTypeClass::Repair_Step(void) const
{
	return(Rule->RepairStep);
}

// FUNCTION: GAMEMD 0x00747F30
HRESULT STDMETHODCALLTYPE UnitTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_UnitTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00747F70
void UnitTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(MovementRestrictedTo);
	crc(Passive);
	crc(CrateGoodie);
	crc(Harvester);
	crc(Weeder);
	crc(unknown_E10);
	crc(HasTurret);
	crc(IsTilter);
	crc(IsSimpleDeployer);
	crc(UseTurretShadow);
}

// FUNCTION: GAMEMD 0x00748010
HRESULT STDMETHODCALLTYPE UnitTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) UnitTypeClass(NoInitClass());

		Fetch_Voxel_Image();
		Fetch_Normal_Image();

		if (AltImage != NULL) {
			char filename[260];
			_makepath(filename, NULL, NULL, AltImageFile, ".SHP");
			AltImage = FileSystem::LoadFile(filename, false);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x007480B0
HRESULT STDMETHODCALLTYPE UnitTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x007480D0
UnitTypeClass * UnitTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<UnitTypeClass>(name, UnitTypes));
}

// FUNCTION: GAMEMD 0x00748160
int UnitTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(UnitTypeClass));
}

// FUNCTION: GAMEMD 0x00748170
// UnitTypeClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x00748180
// UnitTypeClass::Fetch_Heap_ID

// SYNTHETIC: GAMEMD 0x00748190
// ??_GUnitTypeClass@@UAEPAXI@Z
