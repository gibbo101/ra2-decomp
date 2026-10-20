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

// FUNCTION: 0x00746EE0 $E2
// FUNCTION: 0x00746F10 $E5
// FUNCTION: 0x00746F30 $E8
// FUNCTION: 0x00746F50 $E11
// FUNCTION: 0x00746F70 $E14
// FUNCTION: 0x00746F90 $E17
// FUNCTION: 0x00746FB0 $E20
// FUNCTION: 0x00746FF0 $E23
// FUNCTION: 0x00747020 $E26
// FUNCTION: 0x00747050 $E29
// FUNCTION: 0x00747080 $E32
// FUNCTION: 0x00747090 $E35

// FUNCTION: 0x00747600
TPoint2D<int> UnitTypeClass::Turret_Adjust(Dir256 dir, TPoint2D<int> const & xy) const
{
	return(xy);
}

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

// FUNCTION: 0x00747620
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

// FUNCTION: 0x00747EB0
Coord const UnitTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord fix = coord;
	if (fix.Z < Map.Get_Height_GL(fix)) {
		fix.Z = Map.Get_Height_GL(fix);
	}
	return(fix);
}

// FUNCTION: 0x00747F30
HRESULT STDMETHODCALLTYPE UnitTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_UnitTypeClass;
	return(S_OK);
}

// FUNCTION: 0x007480B0
HRESULT STDMETHODCALLTYPE UnitTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
