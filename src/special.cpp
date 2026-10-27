// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>
#include "special.h"
#include "ccini.h"
#include "session.h"
#include "globals.h"

namespace Unsorted { extern bool ArmageddonMode; }


// SYNTHETIC: GAMEMD 0x006B88D0 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x006B88F0 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x006B8920 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x006B8940 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x006B8960 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x006B8980 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x006B89A0 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x006B89C0 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x006B8A00 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x006B8A30 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x006B8A60 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x006B8A90 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x006B8AA0 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x006B8AC0 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x006B8AE0
void SpecialClass::Init(void)
{
	IsShadowGrow = false;
	IsSpeedBuild = false;
	IsFromInstall = false;
	Bit3 = true;
	IsCaptureTheFlag = false;
	IsInert = false;
	IsTGrowth = false;
	IsTSpread = true;

	IsMCVDeploy = false;
	IsInitialVeteran = false;
	IsAllianceFixed = false;
	IsHarvesterImmune = false;
	IsFogOfWar = false;
	Bit2_16 = false;
	IsTExplode = false;
	IsDestroyBridges = true;
	IsTiberiumMeteorites = false;
	IsIonStorms = false;
	IsVisceroid = false;
}

// FUNCTION: GAMEMD 0x006B8AF0
bool SpecialClass::operator==(const SpecialClass &that) const
{
	return(
		(
		IsShadowGrow == that.IsShadowGrow &&
		IsSpeedBuild == that.IsSpeedBuild &&
		IsFromInstall == that.IsFromInstall &&
		Bit3 == that.Bit3 &&
		IsCaptureTheFlag == that.IsCaptureTheFlag &&
		IsInert == that.IsInert &&
		IsTGrowth == that.IsTGrowth &&
		IsTSpread == that.IsTSpread &&
		IsMCVDeploy == that.IsMCVDeploy &&
		IsInitialVeteran == that.IsInitialVeteran &&
		IsAllianceFixed == that.IsAllianceFixed &&
		IsHarvesterImmune == that.IsHarvesterImmune &&
		IsFogOfWar == that.IsFogOfWar &&
		Bit2_16 == that.Bit2_16 &&
		IsTExplode == that.IsTExplode &&
		IsDestroyBridges == that.IsDestroyBridges &&
		IsTiberiumMeteorites == that.IsTiberiumMeteorites &&
		IsIonStorms == that.IsIonStorms &&
		IsVisceroid == that.IsVisceroid
		) ? true : false);
}

// FUNCTION: GAMEMD 0x006B8B10
void SpecialClass::Apply_To_Game(void)
{
}

// FUNCTION: GAMEMD 0x006B8B20
void SpecialClass::Initialize(void)
{
}

// FUNCTION: GAMEMD 0x006B8B30
void SpecialClass::Write_INI(CCINIClass &ini) const
{
	static char const * SPECIAL = "SpecialFlags";

	ini.Put_Bool(SPECIAL, "TiberiumGrows", IsTGrowth);
	ini.Put_Bool(SPECIAL, "TiberiumSpreads", IsTSpread);
	ini.Put_Bool(SPECIAL, "TiberiumExplosive", IsTExplode);
	ini.Put_Bool(SPECIAL, "DestroyableBridges", IsDestroyBridges);
	ini.Put_Bool(SPECIAL, "MCVDeploy", IsMCVDeploy);
	ini.Put_Bool(SPECIAL, "InitialVeteran", IsInitialVeteran);
	ini.Put_Bool(SPECIAL, "FixedAlliance", IsAllianceFixed);
	ini.Put_Bool(SPECIAL, "HarvesterImmune", IsHarvesterImmune);
	ini.Put_Bool(SPECIAL, "FogOfWar", IsFogOfWar);
	ini.Put_Bool(SPECIAL, "Inert", IsInert);
	ini.Put_Bool(SPECIAL, "IonStorms", IsIonStorms);
	ini.Put_Bool(SPECIAL, "Meteorites", IsTiberiumMeteorites);
	ini.Put_Bool(SPECIAL, "Visceroids", IsVisceroid);
}

// FUNCTION: GAMEMD 0x006B8CA0
void SpecialClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	static char const * SPECIAL = "SpecialFlags";

	IsTExplode = ini.Get_Bool(SPECIAL, "TiberiumExplosive", IsTExplode);
	IsMCVDeploy = ini.Get_Bool(SPECIAL, "MCVDeploy", IsMCVDeploy);
	IsInitialVeteran = ini.Get_Bool(SPECIAL, "InitialVeteran", IsInitialVeteran);
	IsIonStorms = ini.Get_Bool(SPECIAL, "IonStorms", IsIonStorms);
	IsTiberiumMeteorites = ini.Get_Bool(SPECIAL, "Meteorites", IsTiberiumMeteorites);
	IsVisceroid = ini.Get_Bool(SPECIAL, "Visceroids", IsVisceroid);

	if (Session.Type == GAME_NORMAL || Unsorted::ArmageddonMode) {
		IsTGrowth = ini.Get_Bool(SPECIAL, "TiberiumGrows", IsTGrowth);
		IsTSpread = ini.Get_Bool(SPECIAL, "TiberiumSpreads", IsTSpread);
		IsDestroyBridges = ini.Get_Bool(SPECIAL, "DestroyableBridges", IsDestroyBridges);
		IsAllianceFixed = ini.Get_Bool(SPECIAL, "FixedAlliance", IsAllianceFixed);
		IsFogOfWar = ini.Get_Bool(SPECIAL, "FogOfWar", IsFogOfWar);
		IsInert = ini.Get_Bool(SPECIAL, "Inert", IsInert);
		IsHarvesterImmune = ini.Get_Bool(SPECIAL, "HarvesterImmune", IsHarvesterImmune);
	}
}
