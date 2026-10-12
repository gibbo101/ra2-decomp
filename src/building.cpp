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
#include "bomb.h"
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
#include "fastmath.h"
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
	IsWrenchVisible(false),
	IsGoingToBlow(false),
	IsSurvivorless(false),
	IsCharging(false),
	IsCharged(false),
	IsCaptured(false),
	ActuallyPlacedOnMap(false),
	HasOpened(false),
	IsDamagedAnims(false),
	IsFogged(false),
	IsRepairing(false),
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

// FUNCTION: 0x0043ED40
DirType BuildingClass::Aim_Direction(AbstractClass * target) const
{
	Coord coord = COORD_NONE;

	if (Class->PrimaryFirePixelOffset != Point2D(0xFFFF, 0xFFFF)) {
		coord = Coord(TacticalMap->Pixel_To_Lepton(Class->PrimaryFirePixelOffset), 0);
	} else {
		Point2D pt(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y);
		if (pt != Point2D(0, 0)) {
			coord = Coord(TacticalMap->Pixel_To_Lepton(pt), 0);
		}
	}
	coord = coord + Center_Coord();
	return(::Direction(coord, target->Center_Coord()));
}

// FUNCTION: 0x0043EE50
int BuildingClass::Set_Turret_Frame(void)
{
	int stage = 0;

	if (Is_Turret_Equipped() && Anims[BANIM_TURRET] != NULL) {
		DirType dir = PrimaryFacing.Current();
		stage = BASECLASS::BodyShape[dir.As_Dir32()];
		AnimClass * anim = Anims[BANIM_TURRET];
		if (anim != NULL) {
			anim->Animation.Set_Stage(stage);
			anim->Animation.Set_Step(0);
		}
	}
	return(stage);
}

// FUNCTION: 0x0043EEB0
void BuildingClass::Set_Turret_Index(int index)
{
	char buffer[64];
	if (index != TurretIndex && !Class->IsTurretAnimAVoxel) {
		DestroyNthAnim(BANIM_TURRET);
		if (index != -1) {
			strcpy(buffer, Class->GraphicName);
			strcat(buffer, "_");
			int len = strlen(buffer);
			buffer[len] = index + 'B';
			buffer[len + 1] = '\0';
			PlayAnim(buffer, BANIM_TURRET, false, false, false);
			TurretIndex = index;
		}
	}
}

// FUNCTION: 0x0043EF90
int BuildingClass::Shape_Number(void) const
{
	int shapenum = Animation.Fetch_Stage();

	if (Class->IsLaserFence) {
		return(LaserFenceFrame);
	}

	if (Class->IsFirestormWall) {
		return(FirestormWallFrame);
	}

	if (BState == BSTATE_CONSTRUCTION) {
		if (Class->IsGate) {
			shapenum = (Class->Anims[BSTATE_CONSTRUCTION].Start + Class->Anims[BSTATE_CONSTRUCTION].Count - 1) - shapenum;
		}
		if (Get_Mission() == MISSION_DECONSTRUCTION) {
			shapenum = (Class->Anims[BState].Start + Class->Anims[BState].Count - 1) - shapenum;
		}
	} else if (Class->CanBeOccupied) {
		shapenum = 0;
		if (GetOccupantCount() > 0) {
			shapenum = 2;
		}
		if (HealthRatio <= Rule->ConditionRed || (Techno_Type_Class()->Level > 0 && HealthRatio <= Rule->ConditionYellow)) {
			shapenum++;
		}
		if (Class->Level == -1 && shapenum == 3) {
			shapenum = 1;
		}
	} else if (Class->IsGate) {
		if (HealthRatio <= Rule->ConditionYellow) {
			return(Class->GateStages + 1);
		}
		return(0);
	} else {
		if (HealthRatio <= Rule->ConditionYellow) {
			if (BState == BSTATE_IDLE) {
				shapenum++;
			} else {
				int last1 = Class->Anims[BSTATE_IDLE].Start + Class->Anims[BSTATE_IDLE].Count;
				int last2 = Class->Anims[BSTATE_ACTIVE].Start + Class->Anims[BSTATE_ACTIVE].Count;
				int largest = MAX(last1, last2);
				last2 = Class->Anims[BSTATE_AUX1].Start + Class->Anims[BSTATE_AUX1].Count;
				largest = MAX(largest, last2);
				last2 = Class->Anims[BSTATE_AUX2].Start + Class->Anims[BSTATE_AUX2].Count;
				largest = MAX(largest, last2);
				shapenum += largest;
			}
		}
	}
	return(shapenum);
}

extern DynamicVectorClass<BuildingClass *> DAT_008B41E0;

inline void Register_Dirty(Rect const & area)
{
	TacticalMap->Register_Dirty_Area(area, false);
}

// FUNCTION: 0x00441F60
void BuildingClass::AfterDestruction(void)
{
	ShapeSet const * shape;
	int frame;

	if (Class->FUN_0045F160(&shape, &frame)) {
		Cell const * offset = Occupy_List();
		Cell origin = Render_Coord().As_Cell();

		while (*offset != REFRESH_EOL) {
			Cell newcell = origin + *offset++;
			CellClass * cellptr = &Map[newcell];
			cellptr->Register_For_Redraw();
			cellptr->Overlay = (OverlayType)0xef;
			cellptr->Rubble = NULL;
			cellptr->Recalc_Attributes(-1);
			Map.ResetZones(cellptr->CellID);
			Map.RecalculateSubZones(cellptr->CellID);
			cellptr->BecomeUntargetable();
		}

		CellClass * cellptr = &Map[origin];
		cellptr->Rubble = Class;
		Rect area = Union(cellptr->Overlay_Render_Rect(), cellptr->Overlay_Shadow_Render_Rect());
		Register_Dirty(area - DSurface::ViewBounds.TopLeft);
		TacticalMap->Flag_Cell(Map[origin]);
	}

	if (Class->CrateBeneath) {
		Cell cell = Render_Coord().As_Cell();
		if (Class->CrateBeneathIsMoney) {
			Map.PlacePowerupCrate(cell, 0);
		} else {
			Map.PlacePowerupCrate(cell, 20);
		}
	}
}

// FUNCTION: 0x00442C40
void BuildingClass::Init(void)
{
	BASECLASS::Init();
	if (House != NULL) {
		WhoLastHurtMe = House->Class->House;
		House->Tracking_Add(this);
	}
	if (Class != NULL) {
		EstimatedHealth = Strength = Class->MaxStrength;
		if (Class->InitialAmmo == -1) {
			Ammo = Class->MaxAmmo;
		} else {
			Ammo = Class->InitialAmmo;
		}
		PrimaryFacing.Set_ROT(Class->ROT);
		if (Class->Get_Buildup_Data() == NULL) {
			IsAllowedToSell = false;
		} else {
			Class->Free_Buildup_Data();
			HasBuildupData = true;
		}
		if (House != NULL && House->WarFactoryInfiltrated && !Class->Naval && Class->IsTrainable && Class->UndeploysInto != NULL) {
			Veterancy.Set_Veteran(true);
		}
	}
	if (Class != NULL && Class->SecretLab) {
		DAT_008B41E0.Add(this);
	}
}

// FUNCTION: 0x00443410
bool BuildingClass::Active_Click_With(ActionType action, ObjectClass * object, bool)
{
	if (action == ACTION_ATTACK) {
		if (object != NULL) {
			Player_Assign_Mission(MISSION_ATTACK, object, NULL, NULL);
			return(true);
		}
		return(true);
	}

	if (action == ACTION_SELF) {
		if (Class->ToBuild != RTTI_NONE) {
			OutList.Add(EventClass(Owner(), EventClass::PRIMARY, TargetClass(this)));
		}
		if (Class->GapGenerator) {
			Player_Assign_Mission(MISSION_UNLOAD, object, NULL, NULL);
		}
		if (Class->UnitAbsorb || Class->InfantryAbsorb) {
			Player_Assign_Mission(MISSION_UNLOAD, object, NULL, NULL);
		}
		if (GetOccupantCount() > 0) {
			Player_Assign_Mission(MISSION_UNLOAD, object, NULL, NULL);
			return(true);
		}
		return(true);
	}

	if (action == (ActionType)0x37) {
		OutList.Add(EventClass(AttachedBomb->OwnerHouse->HeapID, EventClass::DETONATE, TargetClass(this)));
	} else if (action == ACTION_MOVE) {
		RTTIType build = Class->ToBuild;
		if (build != RTTI_INFANTRYTYPE && build != RTTI_UNITTYPE && build != RTTI_AIRCRAFTTYPE) {
			OutList.Add(EventClass(Owner(), EventClass::ARCHIVE, TargetClass(this), TargetClass(object)));
		} else {
			Cell cell = object->Center_Coord().As_Cell();
			Assign_Rally_Point(cell, true);
		}
		return(true);
	}

	return(true);
}

// FUNCTION: 0x004436F0
bool BuildingClass::Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool)
{
	if (action == ACTION_ATTACK) {
		Player_Assign_Mission(MISSION_ATTACK, &Map[cell], NULL, NULL);
		return(true);
	}

	if (action == (ActionType)6 || action == ACTION_MOVE) {
		bool blocked = false;
		if (Class->IsConstructionYard) {
			if (!House->Is_Human_Player() || !Session.Type || !Session.Options.MCVRedeploy || MindControlledBy != NULL) {
				blocked = true;
			}
		}
		if (!blocked) {
			if (Class->UndeploysInto != NULL || Is_Move_Override()) {
				Assign_Rally_Point(cell, true);
			}
		}
		if (Class->UndeploysInto != NULL && !blocked) {
			OutList.Add(EventClass(Owner(), EventClass::SELL, TargetClass(this)));
		}
	}
	return(true);
}

// FUNCTION: 0x00443860
void BuildingClass::Assign_Rally_Point(Cell const & cell, bool play_eva)
{
	Cell nearbyloc;
	SpeedType speed = SPEED_FOOT;
	MZoneType mzone = MZONE_NORMAL;

	bool underbridge = Map[cell].IsUnderBridge;

	if (Class->ToBuild == RTTI_AIRCRAFTTYPE) {
		speed = SPEED_WINGED;
		mzone = MZONE_FLYER;
	}
	if (Class->Naval) {
		speed = (SpeedType)6;
		mzone = (MZoneType)4;
	}

	Coord here = Position;
	int zone = Map.Get_Cell_Zone(here.As_Cell(), mzone, underbridge);

	nearbyloc = Map.Nearby_Location(cell, speed, zone, mzone, underbridge, Point2D(1, 1), false, false, false, true, Cell(0, 0), false, false);

	if (nearbyloc != CELL_NONE) {
		OutList.Add(EventClass(Owner(), EventClass::ARCHIVE, TargetClass(this), TargetClass(&Map[nearbyloc])));
	} else if (Class->IsConstructionYard && House->Is_Human_Player() && Session.Type && Session.Options.MCVRedeploy && MindControlledBy == NULL) {
		OutList.Add(EventClass(Owner(), EventClass::ARCHIVE, TargetClass(this), TargetClass(&Map[Center_Coord()])));
		return;
	}
	if (play_eva && House->Is_Player_Control() && !Class->IsConstructionYard && !Class->ResourceDestination) {
		VoxClass::Play("EVA_NewRallyPointEstablished", -1, -1);
	}
}

// FUNCTION: 0x00443B90
void BuildingClass::Assign_Target(AbstractClass * target)
{
	if (CurrentMission == MISSION_DECONSTRUCTION) goto none;
	if (!IsPowerOnline()) {
none:
		target = NULL;
	} else if (target != NULL && PrimaryWeapon != NULL && !PrimaryWeapon->Bullet->IsAntiAircraft && !IsCloseEnoughToAttack(target)) {
		BASECLASS::Assign_Target(NULL);
		if (Class->IsTickTank || Class->IsArtillary) {
			if (!House->Is_Human_Player() && !IsUnderEMP()) {
				QueueMission(MISSION_DECONSTRUCTION, false);
				Commence();
			}
		}
		return;
	}
	BASECLASS::Assign_Target(target);
}

// FUNCTION: 0x004456D0
void BuildingClass::Update_Buildables(void)
{
	if (House == PlayerPtr && !IsInLimbo && IsDiscoveredByPlayer && IsOn) {
		switch (Class->ToBuild) {
			StructType i;
			UnitType u;
			InfantryType f;
			AircraftType a;

			case RTTI_BUILDINGTYPE:
				for (i = STRUCT_FIRST; i < BuildingTypes.Count(); i++) {
					if (PlayerPtr->Can_Build((const ObjectTypeClass *)BuildingTypes[i], false, true)) {
						Map.Add(RTTI_BUILDINGTYPE, i);
					}
				}
				break;

			case RTTI_UNITTYPE:
				for (u = UNIT_FIRST; u < UnitTypes.Count(); u++) {
					if (PlayerPtr->Can_Build((const ObjectTypeClass *)UnitTypes[u], false, true)) {
						Map.Add(RTTI_UNITTYPE, u);
					}
				}
				break;

			case RTTI_INFANTRYTYPE:
				for (f = INFANTRY_FIRST; f < InfantryTypes.Count(); f++) {
					if (PlayerPtr->Can_Build((const ObjectTypeClass *)InfantryTypes[f], false, true)) {
						Map.Add(RTTI_INFANTRYTYPE, f);
					}
				}
				break;

			case RTTI_AIRCRAFTTYPE:
				for (a = AIRCRAFT_FIRST; a < AircraftTypes.Count(); a++) {
					if (PlayerPtr->Can_Build((const ObjectTypeClass *)AircraftTypes[a], false, true)) {
						Map.Add(RTTI_AIRCRAFTTYPE, a);
					}
				}
				break;

			default:
				break;
		}
	}
}

// FUNCTION: 0x00445880
bool BuildingClass::Limbo(void)
{
	Coord coord = PositionCoord;
	bool threatnode = false;
	bool res = false;
	Cell cell = coord;
	int i;

	if (!IsInLimbo && Class != NULL) {

		for (i = 0; i < 8; i++) {
			if (DamageFireAnims[i] != NULL) {
				DamageFireAnims[i]->Delete_Me();
				DamageFireAnims[i] = NULL;
			}
		}

		if (Class->OrePurifier && ActuallyPlacedOnMap) {
			House->NumOrePurifiers--;
			if (House->NumOrePurifiers < 0) {
				House->NumOrePurifiers = 0;
			}
		}

		if (Class->IsHelipad && ActuallyPlacedOnMap) {
			House->AirportDocks -= Class->NumberOfDocks;
			if (House->AirportDocks < 0) {
				House->AirportDocks = 0;
			}
		}

		if (Class->InfantryGainSelfHeal && ActuallyPlacedOnMap) {
			House->InfantrySelfHeal -= Class->InfantryGainSelfHeal;
			if (House->InfantrySelfHeal < 0) {
				House->InfantrySelfHeal = 0;
			}
		}

		if (Class->UnitsGainSelfHeal && ActuallyPlacedOnMap) {
			House->UnitsSelfHeal -= Class->UnitsGainSelfHeal;
			if (House->UnitsSelfHeal < 0) {
				House->UnitsSelfHeal = 0;
			}
		}

		if (Class->IsLaserFencePost) {
			Update_Laser_Fence_Connections(false);
		}

		if (!ScenarioInit) {
			if (Class->IsSensorArray) {
				SensorArrayDeactivate(CELL_NONE);
			}
			if (Class->DetectDisguise) {
				DisguiseDetectorDeactivate(CELL_NONE);
			}
			if ((Class->UndeploysInto == NULL || !Class->ResourceGatherer) && !IsStrange()) {
				Release_Base_Area();
			}
		}

		for (i = 0; i < BUILDING_UPGRADE_MAX; i++) {
			if (Upgrades[i] != NULL && Upgrades[i]->IsThreatRatingNode) {
				threatnode = true;
				break;
			}
		}

		if (!GameActive) {
			res = true;
		}

		if (!res) {
			if (Class == Rule->WallTower) {
				for (i = 0; i < FACING_COUNT; i += FACING_COUNT / 4) {
					Cell c = AdjacentCell[i & 7] + cell;
					CellClass * cptr = &Map[c];
					cptr->Wall_Update(false);
					if (cptr->Overlay != OVERLAY_NONE && OverlayTypes[cptr->Overlay]->IsWall && cptr->OverlayData < OVERLAYDATA_WALL_DAMAGE_STAGE) {
						cptr->Reduce_Wall(200);
					}
				}
			}

			if (Class == Rule->GDIGateOne || Class == Rule->NodGateOne) {
				Map[Cell(cell - Cell(1, 0))].Wall_Update(false);
				Map[Cell(cell + Cell(3, 0))].Wall_Update(false);
			}

			if (Class == Rule->GDIGateTwo || Class == Rule->NodGateTwo) {
				Map[Cell(cell - Cell(0, 1))].Wall_Update(false);
				Map[Cell(cell + Cell(0, 3))].Wall_Update(false);
			}

			if (Class->ToTile == NULL) {
				int w = Class->Width();
				int h = Class->Height();
				Cell position = PositionCell;

				for (int y = 0; y < h + 2; y++) {
					for (int x = 0; x < w + 2; x++) {
						CellClass *cptr = &Map[(position - Cell(1, 1)) + Cell(x, y)];
						cptr->AdjacentObjectCount--;
					}
				}
			}

			if (TacticalMap != NULL) {
				TacticalMap->Register_Dirty_Area(Get_Render_Rect());
			}

			if (BuildingLight != NULL) {
				BuildingLight->Delete_Me();
			}

			House->Active_Remove(this);
			House->IsRecalcNeeded = true;
			House->Recalc_Center();

			if (House == PlayerPtr) {
				Map.PowerClass::IsToRedraw = true;
				Map.Flag_To_Redraw();
			}

			House->Invalidate_Base_Node_Position(this);
		}
	}

	if (!res) {
		res = BASECLASS::Limbo();

		if (threatnode) {
			House->Activate_Threat_Node();
		}

		if (Class->ToBuild != RTTI_NONE) {
			House->Update_Factories(Class->ToBuild, Class->Naval, 0);
		}
	}

	if (Class->IsLaserFencePost && !ScenarioInit) {
		LaserFenceFrame = 0;
	}

	return(res);
}

// FUNCTION: 0x00445E50
DirType BuildingClass::Turret_Facing(void) const
{
	if (!Is_Turret_Equipped() && TarCom != NULL) {
		return(::Direction(Center_Coord(), TarCom->Center_Coord()));
	}
	return(PrimaryFacing.Current());
}

// FUNCTION: 0x00445F00
AbstractClass * BuildingClass::Greatest_Threat(ThreatType threat, Coord const & coord, bool onlyenemy) const
{
	if (PrimaryWeapon != NULL) {
		threat = ThreatType(threat | PrimaryWeapon->Allowed_Threats());
	}
	if (SecondaryWeapon != NULL) {
		threat = ThreatType(threat | SecondaryWeapon->Allowed_Threats());
	}
	threat = ThreatType(threat | THREAT_RANGE);
	return(BASECLASS::Greatest_Threat(threat, coord, onlyenemy));
}

// FUNCTION: 0x00446FF0
void BuildingClass::Repair(int control)
{
	switch (control) {
		case -1:
			IsRepairing = (IsRepairing == false);
			break;

		case 1:
			if (IsRepairing) return;
			IsRepairing = true;
			break;

		case 0:
			if (!IsRepairing) return;
			IsRepairing = false;
			break;

		default:
			break;
	}

	VocType soundid;
	if (IsRepairing) {
		if (Strength == Class->MaxStrength) {
			soundid = Rule->ScoldSound;
		} else {
			soundid = Rule->GenericClick;
			if (House->Is_Player_Control()) {
				Clicked_As_Target();
			}
			IsWrenchVisible = true;
			if (House->Is_Player_Control()) {
				VoxClass::Play("EVA_Repairing", -1, -1);
			}
		}
	} else {
		soundid = Rule->GenericClick;
	}
	if (House->Is_Player_Control()) {
		VocClass::PlayAt(soundid, PositionCoord, NULL);
	}
}

void BuildingClass::Begin_Anim(BAnimType anim, bool damaged, int delay)
{
	bool garrisoned = GetOccupantCount() > 0;
	char const * name = damaged ? Class->AnimData[anim].AnimDamaged : (garrisoned ? Class->AnimData[anim].AnimGarrisoned : Class->AnimData[anim].Anim);
	if (name != NULL && *name) {
		PlayAnim(name, anim, damaged, garrisoned, false);
	}
}

// FUNCTION: 0x00447110
void BuildingClass::Sell_Back(int control)
{
	if (HasBuildupData) {
		bool decon = false;
		switch (control) {
			case -1:
				decon = (Get_Mission() != MISSION_DECONSTRUCTION);
				break;

			case 1:
				if (Get_Mission() == MISSION_DECONSTRUCTION) return;
				if (IsGoingToBlow) return;
				decon = true;
				break;

			case 0:
				if (Get_Mission() != MISSION_DECONSTRUCTION) return;
				decon = false;
				break;

			default:
				break;
		}

		if (decon) {
			QueueMission(MISSION_DECONSTRUCTION, false);
			Commence();
		}
		if (House->Is_Player_Control()) {
			VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000);
		}
	} else {
		if (Class->IsFirestormWall) {
			Class->Cost_Of(House);
			House->FUN_0050B730();
			Limbo();
			Delete_Me();
		}
	}
}

// FUNCTION: 0x00447210
ActionType BuildingClass::What_Action(ObjectClass const * object, bool disallow_force) const
{
	if (Class->IsInvisibleInGame) {
		return(ACTION_NONE);
	}

	if (object->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)object)->Class->IsInvisibleInGame) {
		return(ACTION_NONE);
	}

	ActionType action = BASECLASS::What_Action(object, disallow_force);

	if (action == ACTION_TOGGLE_SELECT) {
		return(action);
	}

	if (action == ACTION_SELF) {
		if (Class->GapGenerator && PlayerPtr == House) {
			if (!GapSuperCharged && House->Power_Output() - House->Power_Drain() <= Class->ExtraDrain) {
				goto none;
			}
			action = ACTION_SELF;
		} else if (((Class->UnitAbsorb || Class->InfantryAbsorb) && Cargo.How_Many() > 0) || GetOccupantCount() > 0) {
			action = ACTION_SELF;
		} else if (Class->ToBuild == RTTI_NONE) {
			action = ACTION_NONE;
		}
		if (action == ACTION_SELF && Class->ToBuild != RTTI_NONE && IsLeader) {
			action = ACTION_NONE;
		}
	} else if (action == ACTION_ATTACK) {
		if (PrimaryWeapon != NULL) {
			if (!PrimaryWeapon->Bullet->IsAntiGround || Class->IsEMPulseCannon) {
				action = ACTION_NONE;
			}
			if (CurrentMission == MISSION_DECONSTRUCTION) {
none:
				action = ACTION_NONE;
			}
		}
	}

	if (action == ACTION_MOVE || action == ACTION_NOMOVE) {
		if (!Can_Player_Move()) {
			action = ACTION_SELECT;
		} else if (Class->ToBuild == RTTI_INFANTRYTYPE || Class->ToBuild == RTTI_UNITTYPE || Class->ToBuild == RTTI_AIRCRAFTTYPE) {
			bool altdown = (Keyboard->Down(Options.KeyForceMove1) || Keyboard->Down(Options.KeyForceMove2));
			if (!altdown) {
				action = ACTION_SELECT;
			} else {
				Cell cell = object->Center_Coord().As_Cell();
				if (Class->ToBuild != RTTI_AIRCRAFTTYPE) {
					if (!Is_In_Same_Zone(cell)) {
						action = ACTION_NOMOVE;
					}
					if (!Map[cell].IsUnderBridge && Map[cell].Passability != PASSABLE_LAND) {
						action = ACTION_NOMOVE;
					}
				}
			}
		}
	}

	if (action == ACTION_ATTACK && object->Class_Of()->IsImmune) {
		action = ACTION_NOMOVE;
	}
	return(action);
}

// FUNCTION: 0x00447540
ActionType BuildingClass::What_Action(Cell const & cell, bool check_fog, bool disallow_force) const
{
	if (Class->IsInvisibleInGame) {
		return(ACTION_NONE);
	}

	ActionType action;

	if (Class->UndeploysInto != NULL && check_fog) {
		action = BASECLASS::What_Action(cell, false, disallow_force);
		if (!Map.Is_Shrouded(Map[cell].Center_Coord())) {
			if (action == ACTION_MOVE && Map[cell].Land_Type() == LAND_TIBERIUM && Class->ResourceDestination && Class->ResourceGatherer) {
				return(ACTION_HARVEST);
			}
		}
	} else {
		action = BASECLASS::What_Action(cell, check_fog, disallow_force);
	}

	if (action == ACTION_MOVE || action == ACTION_NOMOVE) {
		if (!Can_Player_Move() && action == ACTION_MOVE) {
			action = ACTION_NONE;
		} else if (Is_Move_Override()) {
			if (!(Keyboard->Down(Options.KeyForceMove1) || Keyboard->Down(Options.KeyForceMove2))) {
				action = ACTION_NONE;
			} else if (Class->ToBuild != RTTI_AIRCRAFTTYPE) {
				if (!Is_In_Same_Zone(cell)) {
					action = ACTION_NOMOVE;
				}
				if (!Map[cell].IsUnderBridge && Map[cell].Passability != PASSABLE_LAND) {
					action = ACTION_NOMOVE;
				}
			}
		}
	}

	if (action == ACTION_ATTACK && PrimaryWeapon != NULL) {
		if (!PrimaryWeapon->Bullet->IsAntiGround || Class->IsEMPulseCannon) {
			action = ACTION_NONE;
		}
		if (CurrentMission == MISSION_DECONSTRUCTION) {
			action = ACTION_NONE;
		}
	}

	if (action != ACTION_ATTACK && Is_Move_Override() && House->Is_Player_Control()) {
		action = ACTION_MOVE;
	}
	return(action);
}

// FUNCTION: 0x00447780
void BuildingClass::Begin_Mode(BStateType bstate)
{
	QueueBState = bstate;
	if (BState == BSTATE_NONE || bstate == BSTATE_CONSTRUCTION || ScenarioInit) {
		QueueBState = BSTATE_NONE;
		BState = bstate;
		BuildingTypeClass::AnimControlType const * ctrl = Fetch_Anim_Control();

		if (ScenarioInit) {
			Begin_Anim(BANIM_IDLE, HealthRatio <= Rule->ConditionYellow);
		}
		if (ScenarioInit) {
			Begin_Anim(BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
		}
		if (ScenarioInit) {
			Begin_Anim(BANIM_ACTIVE_TWO, HealthRatio <= Rule->ConditionYellow);
		}
		if (ScenarioInit) {
			Begin_Anim(BANIM_ACTIVE_THREE, HealthRatio <= Rule->ConditionYellow);
		}
		if (ScenarioInit) {
			Begin_Anim(BANIM_ACTIVE_FOUR, HealthRatio <= Rule->ConditionYellow);
		}

		int rate = ctrl->Rate;
		if (Class->Normalized && bstate != BSTATE_CONSTRUCTION) {
			rate = Options.Normalize_Delay(rate);
		}
		Animation.Set_Rate(rate);
		Animation.Set_Stage(ctrl->Start);
	}
}

// FUNCTION: 0x00447AC0
Coord BuildingClass::Center_Coord(void) const
{
	int h = (Class->Height(false) * (CELL_LEPTON_H / 2)) - (CELL_LEPTON_H / 2);
	int w = (Class->Width() * (CELL_LEPTON_W / 2)) - (CELL_LEPTON_W / 2);

	return(PositionCoord + Coord(w, h));
}

// FUNCTION: 0x00447B20
Coord BuildingClass::Docking_Coord(TechnoClass * docker) const
{
	if (Class->IsWeeder) {
		Cell cell = PositionCell + Cell(2, 1);
		Coord coord = Coord(cell, 0);
		coord.Z = PositionCoord.Z;
		return(coord);
	}

	Coord coord;

	if (Class->IsRefinery) {
		return(BASECLASS::Docking_Coord(docker) + Coord(CELL_LEPTON_W / 2, 0, 0));
	} else if (Class->Bunker && docker != NULL) {
		Dir256 dir = ::Direction(Center_Coord(), docker->Center_Coord()).As_Dir256();
		if (dir >= 0 && dir < 0x40) {
			return(Center_Coord() + Coord(CELL_LEPTON_W / 2, -CELL_LEPTON_H / 2, 0));
		} else if (dir >= 0x40 && dir < 0x80) {
			return(Center_Coord() + Coord(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0));
		} else if (dir >= 0x80 && dir < 0xC0) {
			return(Center_Coord() + Coord(-CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0));
		} else {
			return(Center_Coord() + Coord(-CELL_LEPTON_W / 2, -CELL_LEPTON_H / 2, 0));
		}
	} else if (!Class->IsHelipad && !Class->IsCanUnitRepair) {
		return(BASECLASS::Docking_Coord(docker));
	} else {
		switch (Class->NumberOfDocks) {
			case 0:
				return(Center_Coord());

			case 1:
				coord = Center_Coord() + Class->DockingOffsets[0];
				return(coord);

			default:
				int index = FindLinkIndex(docker);
				if (index >= 0 && index < Class->NumberOfDocks) {
					coord = Center_Coord() + Class->DockingOffsets[index];
					return(coord);
				}
				return(Center_Coord());
		}
	}
	return(coord);
}

// FUNCTION: 0x00447E00
int BuildingClass::DistanceToDockingCoord(TechnoClass * docker) const
{
	return(Docking_Coord(docker).Distance_To(docker->Center_Coord()));
}

// FUNCTION: 0x00447E90
Coord BuildingClass::Destination_Coord(TechnoClass * docker) const
{
	if (Class->IsHelipad) {
		return(Docking_Coord(docker));
	}
	if (Class->IsCanUnitRepair || Class->Bunker) {
		return(Docking_Coord(docker));
	}
	return(Center_Coord());
}

// FUNCTION: 0x00459E80
HRESULT STDMETHODCALLTYPE BuildingClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingClass;
	return(S_OK);
}
