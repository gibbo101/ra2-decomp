// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "building.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "bound.h"
#include "bsurface.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "dbgprint.h"
#include "draw.h"
#include "drive.h"
#include "dsurface.h"
#include "event.h"
#include "factory.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "iloco.h"
#include "ilocos.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipiggy.h"
#include "isotile.h"
#include "isotype.h"
#include "light.h"
#include "lightcon.h"
#include "mono.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "queue.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "smudtype.h"
#include "stimer.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "techtype.h"
#include "tiberium.h"
#include "tracker.h"
#include "unit.h"
#include "unittype.h"
#include "vox.h"
#include "warhead.h"
#include "weapon.h"
#include "color.hh"

extern Point2D DAT_0089C848;


// FUNCTION: 0x0043B4C0
bool __fastcall FUN_0043B4C0(HouseClass * house, CellClass * cell)
{
	if (PlayerPtr->Is_Ally(house)) return false;
	int count = PlayerPtr->PsychicDetectionBuildings.Count();
	for (int i = 0; i < count; i++) {
		BuildingClass * bld = PlayerPtr->PsychicDetectionBuildings[i];
		if (bld->IsPowerOnline()) {
			Coord a = bld->Center_Coord();
			Coord b = cell->Cell_Coord();
			Coord delta = Coord(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
			if (delta.Length() <= bld->Class->PsychicDetectionRadius << 8) return true;
		}
	}
	return false;
}


// FUNCTION: 0x0043B5E0
AnimClass * HouseClass::PsiWarn(HouseClass * house, ObjectClass * target, BulletClass * bullet, char const * name)
{
	AnimType type = AnimTypeClass::From_Name(name);
	AnimTypeClass * animtype = AnimTypes[type];
	Coord coord = target->Center_Coord();
	AnimClass * anim = new AnimClass(animtype, coord, 0, 1, (ShapeFlags_Type)0x600, 0, false);
	anim->FUN_00424C90(bullet);
	anim->FUN_00424CA0(house);
	anim->Invisible = true;
	return anim;
}


// FUNCTION: 0x0043B680
BuildingClass::BuildingClass(NoInitClass const & x) :
	BASECLASS(x),
	CountDown(x),
	PlacementDelay(x),
	GateTimer(x),
	BuildingStage(),
	CashProductionTimer(x)
{
}


// FUNCTION: 0x0043B740
BuildingClass::BuildingClass(BuildingTypeClass const * type, HouseClass * house) :
	BASECLASS(house),
	Class((BuildingTypeClass *)type),
	Factory(0),
	CountDown(0),
	BState(BSTATE_NONE),
	QueueBState(BSTATE_NONE),
	WhoLastHurtMe(HOUSE_FIRST),
	WhomToRepay(NULL),
	LastStrength(0),
	AnimToTrack(NULL),
	PsiWarnAnim(NULL),
	PlacementDelay(0),
	RequiresDamageFires(false),
	LastSuperWeaponIndex(-1),
	TurretIndex(-1),
	BuildingLight(NULL),
	GateTimer(0),
	LightSource(NULL),
	LaserFenceFrame(0),
	FirestormWallFrame(0),
	LastRenderRect(RECT_NONE),
	LastRenderCoord(COORD_NONE),
	LastRenderOffset(Point2D(0, 0)),
	IsOn(true),
	IsOverpowered(false),
	RegisteredAsPoweredUnitSource(false),
	SupportingPrisms(0),
	HasExtraPowerBonus(false),
	HasExtraPowerDrain(false),
	FiringOccupantIndex(0),
	WasOnline(false),
	IsNominal(false),
	IsToRebuild(false),
	IsToRepair(false),
	HasEngineer(false),
	CashProductionTimer(0),
	IsAllowedToSell(true),
	IsReadyToCommence(false),
	IsRepairing(false),
	IsGoingToBlow(false),
	IsSurvivorless(false),
	IsCharging(false),
	IsCharged(false),
	IsCaptured(false),
	ActuallyPlacedOnMap(false),
	HasOpened(false),
	IsDamagedAnims(false),
	IsFogged(false),
	IsWrenchVisible(false),
	HasBuildupData(false),
	IsPoweredOn(true),
	CloakGeneratorState(0),
	CurrentCloakRadius(0),
	TranslucencyLevel(0),
	StorageFilledSlots(0),
	SecretProduction(NULL),
	ColorAdd(0),
	ColorAddG(0),
	ColorAddB(0),
	unknown_int_6FC(0),
	Brightness(NORMAL_LIGHT),
	UpgradeLevel(0),
	GateFrame(-1),
	PrismStage(0),
	DelayBeforeFiring(0),
	TankBunkerState(0)
{
	Create_ID();

	for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		Upgrades[i] = NULL;
	}
	CoordStruct zero = {0, 0, 0};
	PrismTargetCoords = zero;

	if (Class != NULL) {
		BarrelPitch.Set_Desired(Class->StartPitch);
	}

	memset(Anims, 0, sizeof(Anims));
	for (int k = 0; k < 8; k++) DamageFireAnims[k] = NULL;
	for (int j = 0; j < BANIM_COUNT; j++) AnimStates[j] = false;
	Audio7.FUN_00405BE0();
	Audio8.FUN_00405BE0();
	BarrelPitch.Set_Desired(Dir256(DIR_E));
	BuildingStage.Set_Stage(0);

	Buildings.Add(this);

	Init();

	FactoryPtrTracker.Add(this);
	AnimPtrTracker.Add(this);
	TargetTracker.Add_Index(Fetch_ID(), this);

	if (Class != NULL) {
		SetLinkCount(Class->NumberOfDocks < 1 ? 1 : Class->NumberOfDocks);
	} else {
		SetLinkCount(1);
	}
}

// FUNCTION: 0x0043C0D0
void BuildingClass::FUN_0043C0D0(void)
{
	int count = Rule->DamageFireTypes.Count();
	if (count) {
		int idx = Random_Pick(0, count - 1);
		for (int i = 0; i < 8; i++) {
			if (Class->DamageFireOffset[i] == DAT_0089C848) return;
			if (DamageFireAnims[i]) return;
			Coord c = Coord(TacticalMap->Pixel_To_Lepton(Class->DamageFireOffset[i]), 0);
			c += Render_Coord();
			AnimClass * anim = new AnimClass(Rule->DamageFireTypes[idx], c, 0, 1, (ShapeFlags_Type)0x600, 0, false);
			if (anim) {
				DamageFireAnims[i] = anim;
				anim->ZAdjust = (((Class->DamageFireOffset[i].Y - (Class->Height() + Class->Width()) * 15) * 3) >> 1) - 10;
				if (anim->ZAdjust > 0) anim->ZAdjust = 0;
				if (anim->Type->End > 0) anim->Animation.Set_Stage(Random_Pick(0, anim->Type->End - 1));
				if (++idx >= count) idx = 0;
			}
		}
	}
}

// FUNCTION: 0x0043C2A0
void BuildingClass::FUN_0043C2A0(void)
{
	for (int i = 0; i < 8; i++) {
		if (DamageFireAnims[i]) {
			DamageFireAnims[i]->Delete_Me();
			DamageFireAnims[i] = 0;
		}
	}
}

// FUNCTION: 0x0043CEA0
bool BuildingClass::Render(Rect & rect, bool forced, bool extras_only) const
{
	if (Debug_Map || !MainWindow || ((forced || IsToDisplay) && IsDown && !IsInLimbo)) {
		IsToDisplay = false;
		rect = Intersect(rect, TacticalRect);

		if (rect.Is_Overlapping(((BuildingClass *)this)->Get_Render_Rect() + TacticalRect.TopLeft)) {
			Point2D point;
			TacticalMap->Coord_To_Pixel(Render_Coord(), point);
			if (rect.X > TacticalRect.X) {
				point.X += TacticalRect.X - rect.X;
			}

			if (rect.Y > TacticalRect.Y) {
				point.Y += TacticalRect.Y - rect.Y;
			}

			if (extras_only) {
				if (!IsFogged) {
					((BuildingClass *)this)->Draw_Extras(point, rect);
				}
			} else {
				Draw_It(point, rect);
			}
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0043D030
void BuildingClass::Editor_Draw_It(Point2D const & xdrawpoint, Rect const & xcliprect) const
{
	Cell cell = PositionCell;

	ShapeSet const * shapefile = (ShapeSet const *)Get_Image_Data();
	if (shapefile == NULL) return;

	if (Class->IsInvisibleInGame) return;

	if (Mission != MISSION_OPEN || Door.Is_Door_Closed()) {

		if (Mission == MISSION_UNLOAD) {
			if (Class->DeployingAnim != NULL) {
				TechnoClass * object = Factory->Get_Object();
				if (object != NULL && object->Techno_Type_Class()->JumpJet) {
					shapefile = Class->RoofDeployingAnim;
				} else {
					shapefile = Class->DeployingAnim;
				}
			}
		}

		Point2D drawpoint = xdrawpoint;
		int shapeheight = shapefile->Get_Height();
		int height = drawpoint.Y - shapeheight / 2;

		Rect cliprect = xcliprect;
		if (cliprect.Height > height) {
			cliprect.Height = height;
		}

		if (cliprect.Height > 0) {
			Techno_Draw_Object(shapefile, Shape_Number(), drawpoint, cliprect, DIR_N, 256, -2 - TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_90DEG, false, Map[cell].Brightness, 0, 0, 0, Point2D(0, 0), SHAPE_NORMAL);
		}

		cliprect = xcliprect;
		Point2D xy = drawpoint;
		cliprect.Y += height;
		cliprect.Height = shapeheight;
		xy.Y -= height;
		if (cliprect.Y + cliprect.Height > xcliprect.Y + xcliprect.Height) {
			cliprect.Height = xcliprect.Y + xcliprect.Height - cliprect.Y;
		}
		if (cliprect.Y < xcliprect.Y) {
			cliprect.Height += cliprect.Y - xcliprect.Y;
			xy.Y += cliprect.Y - xcliprect.Y;
			cliprect.Y = xcliprect.Y;
		}

		if (cliprect.Height > 0) {
			Techno_Draw_Object(shapefile, Shape_Number(), xy, cliprect, DIR_N, 256, -TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_GROUND, false, Map[cell].Brightness, 0, 0, 0, Point2D(0, 0), SHAPE_NORMAL);
		}
	}
}

// FUNCTION: 0x0043E900
int BuildingClass::Get_Z_Adjust(void) const
{
	int pixel = -Tactical::Z_Lepton_To_Pixel(Height);

	if (Class->IsTurretAnimAVoxel) {
		return(Class->AnimData[BANIM_TURRET].ZAdjust + pixel);
	}
	return(pixel);
}

// FUNCTION: 0x00459E80
HRESULT STDMETHODCALLTYPE BuildingClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingClass;
	return(S_OK);
}
