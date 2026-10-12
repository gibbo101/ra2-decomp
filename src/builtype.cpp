// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "builtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "airctype.h"
#include "bound.h"
#include "bsurface.h"
#include "building.h"
#include "bullettype.h"
#include "ccfile.h"
#include "cell.h"
#include "data.h"
#include "findmake.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "isotype.h"
#include "mixfile.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "unittype.h"
#include "warhead.h"
#include "weapon.h"

// FUNCTION: 0x00464A70
Coord const BuildingTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord crd = coord;
	crd.Z = Map.Get_Height_GL(crd);
	return(crd);
}

// FUNCTION: 0x00464B30
void BuildingTypeClass::Compute_CRC(CRCEngine & crc) const
{
	TechnoTypeClass::Compute_CRC(crc);
	crc(GateCloseDelay);
	crc(LightVisibility);
	crc(LightIntensity);
	crc(LightRedTint);
	crc(LightGreenTint);
	crc(LightBlueTint);
	crc((char const *)BuildupFilename);
	crc((char const *)PowersUpBuilding);
	crc(Adjacent);
	crc(ToBuild);
	crc(Power);
	crc(Drain);
	crc(ExtraPower);
	crc(ExtraDrain);
	crc(Size);
	crc(ZHeight);
	crc(OccupyHeight);
	crc(MidPoint);
	crc(DoorStages);
	crc(&Anims, 72);
	crc(&AnimData, 1428);
	crc(Upgrades);
	crc(SpecialZOverlayZAdjust);
	crc(IsHoverPad);
	crc(IsPlug);
	crc(IsTemple);
	crc(HasSpotlight);
	crc(IsBase);
	crc(EligibleForAllyBuilding);
	crc(NeedsEngineer);
	crc(IsBibbed);
	crc(IsWall);
	crc(IsCaptureable);
	crc(IsPowered);
	crc(IsUnsellable);
	crc(IsRadar);
	crc(SpySat);
	crc(IsHasChargeAnim);
	crc(IsAnimDelayedFire);
	crc(DelayedFireDelay);
	crc(IsSiloDamage);
	crc(IsCanUnitRepair);
	crc(IsCanUnitReload);
	crc(Cloning);
	crc(Grinding);
	crc(UnitAbsorb);
	crc(InfantryAbsorb);
	crc(SecretLab);
	crc(IsFlat);
	crc(IsDockUnload);
	crc(IsRecoilless);
	crc(IsBridgeRepairHut);
	crc(IsGate);
	crc(IsSAM);
	crc(IsConstructionYard);
	crc(IsNukeSilo);
	crc(IsRefinery);
	crc(IsWeaponsFactory);
	crc(IsLaserFencePost);
	crc(IsLaserFence);
	crc(IsFirestormWall);
	crc(IsHelipad);
	crc(OrePurifier);
	crc(IsExtraDamageStage);
	crc(IsHospital);
	crc(IsArmory);
	crc(GateStages);
	crc(PowersUpToLevel);
	crc(IsDamagedDoor);
	crc(IsInvisibleInGame);
	crc(IsTerrainPalette);
	crc(IsTurretAnimAVoxel);
	crc(IsBarrelAnimAVoxel);
	crc((char const *)VoxelBarrelFile);
	crc(VoxelBarrelScale);
	crc(&VoxelBarrelOffsetToPitchPivotPoint, 12);
	crc(&VoxelBarrelOffsetToRotatePivotPoint, 12);
	crc(&VoxelBarrelOffsetToBuildingPivotPoint, 12);
	crc(&VoxelBarrelOffsetToBarrelEnd, 12);
	crc(CrateBeneath);
	crc(LeaveRubble);
	crc(CrateBeneathIsMoney);
	crc(NumberOfDocks);
	crc(StartPitch);
	crc(ConcentricRadialIndicator);
}

// FUNCTION: 0x00465380
HRESULT STDMETHODCALLTYPE BuildingTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingTypeClass;
	return(S_OK);
}
