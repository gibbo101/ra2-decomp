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
#include "airstrike.h"
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
#include "laser.h"
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
void __fastcall FUN_00750D40(Coord const & coord, AudioController * controller);


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

// FUNCTION: 0x00447F10
FireErrorType BuildingClass::Can_Fire(AbstractClass * target, int which, bool ignoreRange) const
{
	if (Class->CanBeOccupied && (!Class->CanOccupyFire || !GetOccupantCount())) {
		return(FIRE_ILLEGAL);
	}
	if (IsBeingDrained()) {
		return(FIRE_ILLEGAL);
	}
	if (Class->IsEMPulseCannon) {
		return(FIRE_CANT);
	}
	if (Get_Mission() == MISSION_DECONSTRUCTION || Get_Mission() == MISSION_CONSTRUCTION) {
		return(FIRE_ILLEGAL);
	}
	if (!IsPowerOnline()) {
		return(FIRE_CANT);
	}
	if (DelayBeforeFiring) {
		return(FIRE_REARM);
	}

	FireErrorType canfire = TechnoClass::Can_Fire(target, which, ignoreRange);
	if (canfire == FIRE_OK && Is_Turret_Equipped()) {
		DirType towards = Aim_Direction(target);
		DirType by = Class->IsTurretAnimAVoxel ? DIR_MIN : DIR_STEP_32;
		if (!PrimaryFacing.Current().Is_Complete_Turn(towards, by)) {
			return(FIRE_FACING);
		}
	}
	return(canfire);
}

// FUNCTION: 0x00448070
bool BuildingClass::FUN_00448070(bool demote)
{
	RTTIType type = Class->ToBuild;
	if (type == RTTI_NONE) {
		return(false);
	}
	bool was = IsLeader;
	int count = House->Buildings.Count();
	BuildingClass * building;
	for (int index = 0; index < count; index++) {
		building = House->Buildings[index];
		if (!building->IsInLimbo && building->IsLeader) {
			if (building->Class->ToBuild == type) {
				if ((Class->Naval && building->Class->Naval) || (!Class->Naval && !building->Class->Naval)) {
					if (demote) {
						building->IsLeader = false;
						IsLeader = true;
					}
					goto done;
				}
			}
		}
	}
	IsLeader = true;
done:
	if (Class->ToBuild == RTTI_BUILDINGTYPE) {
		House->FUN_0050DB00(this);
	}
	return(IsLeader != was);
}

// FUNCTION: 0x00448160
bool BuildingClass::Toggle_Primary(void)
{
	if (Class->ToBuild == RTTI_NONE) {
		return(IsLeader);
	}
	if (IsLeader) {
		IsLeader = false;
	} else {
		int count = House->Buildings.Count();
		for (int index = 0; index < count; index++) {
			BuildingClass * building = House->Buildings[index];
			if (!building->IsInLimbo && building->Class->ToBuild == Class->ToBuild &&
				((Class->Naval && building->Class->Naval) || (!Class->Naval && !building->Class->Naval))) {
				building->IsLeader = false;
			}
		}
		IsLeader = true;
		if (House->Is_Player_Control()) {
			VoxClass::Play("EVA_PrimaryBuildingSelected", -1, -1);
		}
		if (Class->ToBuild == RTTI_BUILDINGTYPE) {
			House->FUN_0050DB00(this);
		}
	}
	Mark(MARK_CHANGE);
	return(IsLeader);
}

// FUNCTION: 0x00449410
int BuildingClass::Sort_Y(void) const
{
	int y = BASECLASS::Sort_Y();
	return(y + (Class->IsTurretAnimAVoxel != 0 ? 32 : 0) - (Class->IsGate != 0 ? 16 : 0));
}

// FUNCTION: 0x00449440
MoveType BuildingClass::Can_Enter_Cell(CellClass const * cell, FacingType dir, int cell_height, CellClass const *, bool) const
{
	Cell c = cell->CellID;
	if (Class->UndeploysInto && IsDown) {
		return(Map[c].Is_Clear_To_Build(Class->Speed, (BuildingTypeClass *)Class, House) ? MOVE_OK : MOVE_NO);
	}
	return(Class->Legal_Placement(c, House) ? MOVE_OK : MOVE_NO);
}

// FUNCTION: 0x004494C0
bool BuildingClass::Can_Demolish(void) const
{
	if (IsBeingDrained() || Class->IsUnsellable) {
		return(false);
	}
	if (HasBuildupData && BState != BSTATE_CONSTRUCTION && Get_Mission() != MISSION_DECONSTRUCTION && Get_Mission() != MISSION_CONSTRUCTION) {
		return(true);
	}
	if (Class->IsFirestormWall && !House->FirestormDefenseActivated) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00449540
bool BuildingClass::Clear_Weapons_Factory_Bib(void)
{
	if (Class->IsWeaponsFactory) {
		Cell exit = Class->ExitList[10];
		Cell cell = exit + PositionCell;
		Cell cell2 = Cell(cell.X - 1, cell.Y);
		Coord coord = cell2.As_Coord();
		CellClass * cellptr = &Map[cell2];

		TechnoClass * tech = cellptr->Cell_Techno(Point2D(0, 0), false, this);
		if (tech != NULL) {
			DebugString("Weapons factory clearing %s from bib\n", (const char *)tech->Techno_Type_Class()->IniName);
			cellptr->Incoming((Coord const &)DAT_0089C848, true, true);
			for (FacingType f = FACING_FIRST; f < FACING_COUNT; f++) {
				CellClass * cptr = &cellptr->Adjacent_Cell(f);
				TechnoClass * tech = cptr->Cell_Techno(Point2D(0, 0), false, this);
				if (tech != NULL) {
					DebugString("Weapons factory clearing %s from bib area\n", (const char *)tech->Techno_Type_Class()->IniName);
					cptr->Incoming(coord, true, true);
				}
			}
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x004496B0
int BuildingClass::Do_MISSION_GUARD(void)
{
	if (Techno_Type_Class()->IsGattling) {
		GattlingRateDown(MissionAccumulateTime);
		MissionAccumulateTime = 0;
	}

	if (Is_Weapon_Equipped()) {
		IsReadyToCommence = true;
		if (!Class->IsEMPulseCannon) {
			bool idle = false;
			if (Class->SuperWeapon != -1 && SuperClass::Array[Class->SuperWeapon]->Class->AuxBuilding && House->ABQuantity.Value(SuperClass::Array[Class->SuperWeapon]->Class->AuxBuilding->HeapID) == 0) {
				idle = true;
			} else if (Class->SuperWeapon == -1) {
				idle = true;
			}
			if (idle) {
				if (!Class->CanBeOccupied || GetOccupantCount() > 0) {
					if (TarCom != NULL) {
						QueueMission(MISSION_ATTACK, false);
						Commence();
						return(1);
					}
				}
			}
		}
	} else {
		if (Class->IsHasStupidGuardMode) {
			return(100);
		}

		switch (Status) {
			case 0:
				Begin_Mode(BSTATE_IDLE);
				Status = 1;
				break;

			case 1:
				if (Class->IsCanUnitRepair || Class->IsCanUnitReload || Class->Bunker) {
					for (int index = 0; index < RadioLinks.Length(); index++) {
						TechnoClass * contact = FUN_0065AD30(index);
						if (contact != NULL && contact->Is_Techno() && contact->Mission == MISSION_ENTER && Distance_To(contact) < CELL_LEPTON / 4) {
							if (Transmit_Message(RADIO_NEED_TO_MOVE, contact) == RADIO_ROGER) {
								QueueMission(MISSION_REPAIR, false);
								return(1);
							}
						}
					}
				}
				if (Class->IsWeaponsFactory) {
					Clear_Weapons_Factory_Bib();
				}
				break;

			default:
				break;
		}

		if (Class->IsCanUnitReload && HasAnyLink()) {
			if (Transmit_Message(RADIO_PREPARED) != RADIO_ROGER && Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER) {
				QueueMission(MISSION_REPAIR, false);
			}
		}
		if (Class->IsCanUnitRepair) {
			return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
		}
		return(Current_Mission_Control().Normal_Delay() * 3 + Random_Pick(0, 2));
	}
	return(Current_Mission_Control().AA_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x00449A40
int BuildingClass::Do_MISSION_GUARD_AREA(void)
{
	return(Do_MISSION_GUARD());
}

// FUNCTION: 0x00449A50
int BuildingClass::Do_MISSION_CONSTRUCTION(void)
{
	switch (Status) {
		case 0:
			Begin_Mode(BSTATE_CONSTRUCTION);
			Transmit_Message(RADIO_BUILDING);
			{
				int sound = Class->BuildupSound;
				if (sound == -1) {
					sound = Rule->Construction;
				}
				if (sound != -1) {
					VocClass::PlayAt(sound, PositionCoord, &Audio7);
				}
			}
			IsToDisplay = true;
			Status = 1;
			break;

		case 1:
			IsToDisplay = true;
			FUN_00750D40(PositionCoord, &Audio7);
			if (IsReadyToCommence) {
				Transmit_Message(RADIO_COMPLETE);
				Transmit_Message(RADIO_OVER_OUT);
				Begin_Mode(BSTATE_IDLE);
				Grand_Opening();
				QueueMission(MISSION_GUARD, false);
				if (!Class->IsLaserFence) {
					PrimaryFacing.Set(Class->StartFace);
				}
				Class->Free_Buildup_Data();
				Audio7.EndLooping();
			}
			break;

		default:
			break;
	}
	return(1);
}

// FUNCTION: 0x00449BC0
bool BuildingClass::Can_Be_Undeployed(void)
{
	if (Class->UndeploysInto) {
		if (!Class->IsConstructionYard) {
			return(true);
		}
		if (Session.Type && ArchiveTarget && House->Is_Human_Player()) {
			if (Session.Type && Session.Options.MCVRedeploy && MindControlledBy == NULL) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: 0x0044ABD0
void BuildingClass::FireLaser(Coord coords)
{
	RGBClass black(0, 0, 0);
	LaserDrawClass * laser = new LaserDrawClass(Fire_Coord(0, Coord(0, 0, 0)), coords, 0, true, House->LaserColor, black, black, Rule->PrismSupportDuration, false, true, 1.0f, 0.0f);
	if (laser) {
		laser->IsHouseColor = true;
		laser->Thickness = 3;
	}
	Arm = Rule->PrismSupportDelay;
	SupportingPrisms = 0;
}

namespace Unsorted { extern bool ArmageddonMode; }
void __fastcall FUN_00660B80(Cell const & cell);

// FUNCTION: 0x0044B760
int BuildingClass::Do_MISSION_CAPTURE(void)
{
	return(MissionClass::Do_MISSION_CAPTURE());
}

// FUNCTION: 0x0044B770
int BuildingClass::Do_MISSION_HARVEST(void)
{
	return(MissionClass::Do_MISSION_HARVEST());
}

// FUNCTION: 0x0044D5D0
bool BuildingClass::Revealed(HouseClass * house)
{
	if (BASECLASS::Revealed(house)) {
		if (!ScenarioInit) {
			House->JustBuiltStructure = Class->HeapID;
			House->IsBuiltSomething = true;
		}
		House->IsRecalcNeeded = true;
		if (!HasAnyLink() && House->Is_Human_Player() && Get_Mission() != MISSION_CONSTRUCTION && MissionQueue != MISSION_CONSTRUCTION) {
			Grand_Opening();
		} else {
			if (!HasAnyLink() && !House->Is_Human_Player() && house == House && Get_Mission() != MISSION_CONSTRUCTION) {
				Grand_Opening();
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0044D6A0
bool BuildingClass::Enter_Idle_Mode(bool initial, bool)
{
	MissionType mission = MISSION_GUARD;
	if (!initial || ScenarioInit || Unsorted::ArmageddonMode) {
		Begin_Mode(BSTATE_IDLE);
		mission = MISSION_GUARD;
	} else {
		Begin_Mode(BSTATE_CONSTRUCTION);
		mission = MISSION_CONSTRUCTION;
	}
	QueueMission(mission, false);
	return(false);
}

// FUNCTION: 0x0044D700
int BuildingClass::Pip_Count(void) const
{
	switch (Class->PipScale) {
		case PIPSCALE_TIBERIUM:
			if (Class->IsWeeder) {
				return(int(Class->Max_Pips() * House->Weed_Fraction()));
			}
			return(0);
	}
	return(BASECLASS::Pip_Count());
}

// FUNCTION: 0x0044D760
void BuildingClass::Death_Announcement(TechnoClass const * source) const
{
	if (source != NULL && House->Is_Player_Control()) {
		Cell cell = Destination_Coord().As_Cell();
		FUN_00660B80(cell);
	}
}

// FUNCTION: 0x0044E3A0
bool BuildingClass::Is_Blocked_By_Occupier(void)
{
	Cell cell = Get_Cell();
	Cell const * list = Occupy_List();
	bool blocked = false;
	while (*list != REFRESH_EOL) {
		Cell newcell = cell + *list;
		list++;
		ObjectClass * optr = Map[newcell].Cell_Occupier();
		while (optr != NULL) {
			if (this != optr) {
				blocked = true;
				break;
			}
			optr = optr->Next;
		}
	}
	return(blocked);
}

// FUNCTION: 0x0044E440
int BuildingClass::Do_MISSION_OPEN(void)
{
	if (Class->IsGate) {
		enum {
			START_OPENING,
			OPENING,
			OPEN,
			START_CLOSING,
			CLOSING,
			CLOSED
		};
		switch (Status) {
			case START_OPENING:
				if (Door.Is_Door_Open()) {
					GateTimer = int(Class->GateCloseDelay * TICKS_PER_MINUTE);
					Status = OPEN;
					break;
				}
				if (!Door.Is_Door_Opening()) {
					if (Door.Is_Door_Closing()) {
						Door.Reverse();
					} else {
						Door.Open_Door(Class->DeployTime);
						VocClass::PlayAt(Rule->GateDownSound, PositionCoord, NULL);
						Rect redrawrect = Get_Render_Rect();
						TacticalMap->Register_Dirty_Area(redrawrect, false);
					}
				}
				Status = OPENING;
				GateTimer = int(Class->GateCloseDelay * TICKS_PER_MINUTE);
				return(0);

			case START_CLOSING:
				Door.Close_Door(Class->DeployTime);
				Status = CLOSING;
				VocClass::PlayAt(Rule->GateUpSound, PositionCoord, NULL);
				return(0);

			case OPEN:
				if (!Is_Blocked_By_Occupier()) {
					if (GateTimer.Progress() == 1.0) {
						Status = START_CLOSING;
					}
				} else {
					GateTimer = int(Class->GateCloseDelay * TICKS_PER_MINUTE);
				}
				break;

			case OPENING:
				if (Door.Is_Door_Open()) {
					Status = OPEN;
				}

			case CLOSING:
				if (Door.Is_Door_Closed()) {
					Enter_Idle_Mode();
					Status = CLOSED;
					IsToDisplay = true;
				}
				if (!Door.Is_Ready_To_Open()) {
					int stage = int(Door.Percent_Complete() * Class->GateStages);
					if (stage != GateFrame) {
						GateFrame = stage;
						IsToDisplay = true;
					}
				}
				return(0);
		}

		return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
	}

	QueueMission(MISSION_GUARD, false);
	return(1);
}

// FUNCTION: 0x0044E7B0
int BuildingClass::Power_Output(void) const
{
	int power = Class->Power;
	if (!IsBeingWarpedOut()) {
		if (HasExtraPowerBonus) {
			power += Class->ExtraPower;
		}
		if ((Class->UnitAbsorb || Class->InfantryAbsorb) && Class->ExtraPower > 0) {
			int n = Cargo.How_Many();
			if (n > 0) {
				power += Class->ExtraPower * n;
			}
		}
		if (UpgradeLevel != 0) {
			for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
				if (Upgrades[i] != NULL) {
					power += Upgrades[i]->Power;
				}
			}
		}
		if (power > 0 && IsOn) {
			return(int(power * HealthRatio));
		}
	}
	return(0);
}

// FUNCTION: 0x0044E880
int BuildingClass::Power_Drain(void) const
{
	if (!IsBeingWarpedOut() && IsOn) {
		int drain = Class->Drain;
		if (HasExtraPowerDrain) {
			drain += Class->ExtraDrain;
		}
		if (UpgradeLevel != 0) {
			for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
				if (Upgrades[i] != NULL) {
					drain += Upgrades[i]->Drain;
				}
			}
		}
		return(drain);
	}
	return(0);
}

static inline void Replay_Anim(BuildingClass * building, BAnimType anim, bool damaged)
{
	char const * name = damaged ? building->Class->AnimData[anim].AnimDamaged : building->Class->AnimData[anim].Anim;
	if (name != NULL && *name) {
		building->PlayAnim(name, anim, damaged, false, false);
	}
}

// FUNCTION: 0x0044E8F0
void BuildingClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);

	if (WhomToRepay == target) {
		WhomToRepay = NULL;
	}
	if (AnimToTrack == target) {
		AnimToTrack = NULL;
	}
	if (Factory == target) {
		Factory = NULL;
	}
	if (BuildingLight == target) {
		BuildingLight = NULL;
	}
	if (LightSource == target) {
		LightSource = NULL;
	}
	if (Anims[BANIM_PRODUCTION] == target) {
		Replay_Anim(this, BANIM_IDLE, HealthRatio <= Rule->ConditionYellow);
	}
	if (Class->Grinding && Anims[BANIM_SPECIAL_ONE] == target) {
		Replay_Anim(this, BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
	}
	if (target != NULL && ((AbstractClass *)target)->What_Am_I() == RTTI_ANIM) {
		for (int i = 0; i < 8; i++) {
			if (DamageFireAnims[i] == target) {
				DamageFireAnims[i] = NULL;
				return;
			}
		}
		if (((AnimClass *)target)->IsBuildingAnim) {
			Detach_Anim((AnimClass *)target);
			return;
		}
	}
	if (Class == target) {
		Class = NULL;
	}
	for (int j = 0; j < BUILDING_UPGRADE_MAX; j++) {
		if (Upgrades[j] == target) {
			Upgrades[j] = NULL;
		}
	}
	Overpowerers.Delete((InfantryClass *)target);
	if (ScenarioInit) {
		Occupants.Delete((InfantryClass *)target);
	}
}

// FUNCTION: 0x0044EB10
InfantryTypeClass const * BuildingClass::Crew_Type(void) const
{
	if (!IsCaptured && Percent_Chance(25) && Class->ToBuild == RTTI_BUILDINGTYPE) {
		return(Rule->Engineer);
	}
	return(BASECLASS::Crew_Type());
}

// FUNCTION: 0x0044EB60
void BuildingClass::FUN_0044EB60(FactoryClass * factory)
{
	if (factory != NULL) {
		TechnoClass * object = factory->Get_Object();
		bool limbo = IsInLimbo;
		IsInLimbo = true;
		if (object != NULL && !object->Techno_Type_Class()->Who_Can_Build_Me(true, false, false, House)) {
			House->Abandon_Production(Class->ToBuild, object->Techno_Type_Class()->Fetch_Heap_ID(), false, false);
		}
		IsInLimbo = limbo;
	}
}

// FUNCTION: 0x0044EBF0
void BuildingClass::Detach_All(bool all)
{
	if (all) {
		if (Factory) {
			Factory->Abandon();
			delete (FactoryClass *)Factory;
			Factory = 0;
		}

		if (House) {
			if (Class->ToBuild == RTTI_BUILDINGTYPE) {
				FUN_0044EB60(House->BuildingFactory);
				FUN_0044EB60(House->Primary_Unused1);
				FUN_0044EB60(House->Primary_Unused2);
				FUN_0044EB60(House->Primary_Unused3);
				FUN_0044EB60(House->Primary_ForDefenses);
			} else {
				FactoryClass * factory = House->Fetch_Factory(Class->ToBuild, Class->Naval, 0);
				if (factory) {
					FUN_0044EB60(factory);
				}
			}
		}
	}

	if (!all) {
		for (int index = 0; index < RadioLinks.Length(); index++) {
			TechnoClass * link = FUN_0065AD30(index);
			if (link && !House->Is_Ally(link)) {
				Transmit_Message(RADIO_OVER_OUT, link);
			}
		}
	} else {
		SendToEachLink(RADIO_OVER_OUT);
	}

	BASECLASS::Detach_All(all);
}

// FUNCTION: 0x0044EF80
int BuildingClass::Flush_For_Placement(TechnoClass * techno, Cell const & cell)
{
	if (techno) {
		return(((BuildingTypeClass const *)techno->Class_Of())->Flush_For_Placement(cell, House));
	}
	return(-1);
}

// FUNCTION: 0x0044F5C0
bool BuildingClass::Can_Player_Move(void) const
{
	if ((int)EMPLockRemaining > 0) {
		return(false);
	}
	if (Is_Move_Override()) {
		return(true);
	}
	if (Class->IsConstructionYard) {
		if (!House->FUN_0050B730() || !Session.Type || !Session.Options.MCVRedeploy || MindControlledBy != NULL) {
			return(false);
		}
	}
	return(Class->UndeploysInto != NULL);
}

// FUNCTION: 0x0044F640
Coord BuildingClass::Exit_Coord(int index) const
{
	if (Class->ExitCoordinate != (Coord const &)DAT_0089C848) {
		return(Class->ExitCoordinate + PositionCoord);
	}
	return(Center_Coord());
}

// FUNCTION: 0x0044F6E0
Cell BuildingClass::Check_Point(CheckPointType cp) const
{
	int xoffset = 6;
	int yoffset = 5;
	Cell cell = Center_Coord().As_Cell();

	switch (cp) {
		case CHECK_STACK:
			xoffset = 0;
			break;

		case CHECK_CROSSWIND:
			yoffset = 0;
			break;

		case CHECK_DOWNWIND:
		default:
			break;
	}

	if ((cell.X - Map.MapRect.X) > Map.MapRect.Width/2)  {
		xoffset = -xoffset;
	}

	if ((cell.Y - Map.MapRect.Y) > Map.MapRect.Height/2)  {
		yoffset = -yoffset;
	}

	return(cell + Cell(xoffset, yoffset));
}

// FUNCTION: 0x0044F7A0
void BuildingClass::Update_Radar_Spied(void)
{
	House->RadarSpied = 0;
	int count = House->Buildings.Count();
	for (int index = 0; index < count; index++) {
		BuildingClass * obj = House->Buildings[index];
		if (obj && !obj->IsInLimbo && obj->Class->IsRadar) {
			House->RadarSpied |= obj->SpiedBy;
		}
	}
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}

char const * const BuildingClass::INI_NAME = "Structures";

// FUNCTION: 0x0044FE60
void BuildingClass::Write_All(CCINIClass & ini)
{
	ini.Clear(INI_NAME);

	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (!building->IsInLimbo && !building->Class->IsLaserFence) {
			building->Write_INI(ini);
		}
	}
}

// FUNCTION: 0x0044FEC0
void BuildingClass::Write_INI(CCINIClass & ini)
{
	char	uname[10];
	char	buf[127];

	sprintf(uname, "%d", Fetch_ID());
	int behavior;
	if (BuildingLight != NULL) {
		behavior = BuildingLight->Behavior;
	} else {
		behavior = 0;
	}

	char const * tag_name;
	if (Tag != NULL && Tag->Class != NULL) {
		tag_name = (char const *)Tag->Class->IniName;
	} else {
		tag_name = "None";
	}
	sprintf(buf, "%s,%s,%d,%d,%d,%d,%s,%d,%d,%d,%d,%d",
		(char const *)House->IniName,
		(char const *)Class->IniName,
		(int)(HealthRatio*256 + .5),
		Get_Cell().X,
		Get_Cell().Y,
		PrimaryFacing.Current().As_Dir256(),
		tag_name,
		IsAllowedToSell,
		IsToRebuild,
		IsOn,
		UpgradeLevel,
		behavior
	);

	for (int i = 0; i < BUILDING_UPGRADE_MAX; i++) {
		sprintf(&buf[strlen(buf)], ",%s", Upgrades[i] != NULL ? (char const *)Upgrades[i]->IniName : "None");
	}

	sprintf(&buf[strlen(buf)], ",%d", IsToRepair != 0);
	sprintf(&buf[strlen(buf)], ",%d", IsNominal != 0);
	ini.Put_String(INI_NAME, uname, buf);
}

// FUNCTION: 0x004500A0
Coord BuildingClass::Target_Coord(void) const
{
	Coord center = Center_Coord();
	return(Class->TargetCoordOffset + center);
}

// FUNCTION: 0x004500F0
void BuildingClass::Factory_AI(void)
{
	if (Factory != NULL && Factory->Has_Completed() && PlacementDelay == 0) {
		TechnoClass * product = Factory->Get_Object();

		switch (Exit_Object(product, CELL_NONE)) {
			case 0:
				if (product->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)product)->Class->Naval) {
					product->Owner_HouseClass()->NodeLogic_1F0 = false;
				}
				Factory->Abandon();
				delete (FactoryClass *)Factory;
				Factory = 0;
				break;

			case 1:
				PlacementDelay = int(Rule->PlacementDelay * TICKS_PER_MINUTE);
				break;

			case 2:
				VocClass::PlayIndexAtPos(Class->CreateUnitSound, PositionCoord, 0);
				House->Just_Built(product);
				Factory->Completed();
				delete (FactoryClass *)Factory;
				Factory = 0;
				break;

			default:
				break;
		}
	}

	if (House->IsStarted && Get_Mission() != MISSION_CONSTRUCTION && Get_Mission() != MISSION_DECONSTRUCTION && Class->ToBuild != RTTI_NONE) {
		if (Factory != NULL) {
			if (PlacementDelay == 0 && !Factory->Is_Building()) {
				Factory->Abandon();
				delete (FactoryClass *)Factory;
				Factory = 0;
			}
		} else {
			if (House->IsStarted && House->Available_Money() > 10) {
				TechnoTypeClass const * techno = House->Suggest_New_Object(Class->ToBuild, false);
				if (techno != NULL) {
					if (techno->Fetch_RTTI() == RTTI_UNITTYPE) {
						if (Class->Naval) {
							if (!techno->Naval) return;
						}
						if (techno->Naval) {
							if (!Class->Naval) return;
						}
					}
					Factory = new FactoryClass;
					if (Factory != NULL) {
						if (!Factory->Set(*techno, *House, false)) {
							delete (FactoryClass *)Factory;
							Factory = 0;
						} else {
							House->Production_Begun(Factory->Get_Object());
							Factory->Start(false);
						}
					}
				}
			}
		}
	}
}

// FUNCTION: 0x004503E0
void BuildingClass::Rotation_AI(void)
{
}

// FUNCTION: 0x004503F0
void BuildingClass::FUN_004503F0(void)
{
	if (PrismStage != 0) {
		if (--DelayBeforeFiring <= 0) {
			DelayBeforeFiring = 0;
			switch (PrismStage) {
				case 1:
					if (TarCom != NULL) {
						if (Can_Fire(TarCom, PrismTargetCoords.X, true) == FIRE_OK) {
							BulletClass * bullet = Fire_At(TarCom, PrismTargetCoords.X);
							if (bullet != NULL && SupportingPrisms != 0) {
								bullet->DamageMultiplier = (unsigned)((Rule->PrismSupportModifier * SupportingPrisms + 100) << 8) / 100;
								SupportingPrisms = 0;
							}
						}
					}
					break;

				case 2:
					FireLaser(PrismTargetCoords);
					break;

				default:
					break;
			}
			PrismStage = 0;
		}
	}
}

// FUNCTION: 0x004504F0
void BuildingClass::FUN_004504F0(int stage, CoordStruct const & coords)
{
	DelayBeforeFiring = Class->DelayedFireDelay;
	PrismStage = stage;
	PrismTargetCoords = coords;
	DestroyNthAnim(BANIM_ACTIVE_ONE);
	Replay_Anim(this, BANIM_SPECIAL_ONE, HealthRatio <= Rule->ConditionYellow);
}

// FUNCTION: 0x00450590
void BuildingClass::FUN_00450590(void)
{
	for (int i = Overpowerers.Count() - 1; i >= 0; i--) {
		if (Overpowerers[i]->TarCom != this) {
			Overpowerers.Delete_Index(i);
		}
	}
	int count = Overpowerers.Count();
	if (count >= 3 || (House->Power_Fraction() == 1.0 && IsOn && count >= 1)) {
		IsOverpowered = true;
	} else {
		IsOverpowered = false;
	}
}

// FUNCTION: 0x00450630
void BuildingClass::Repair_AI(void)
{
	if (House->IQ >= Rule->IQRepairSell && Get_Mission() != MISSION_CONSTRUCTION && Get_Mission() != MISSION_DECONSTRUCTION) {
		if (Can_Repair()) {
			if (House->Available_Money() >= Rule->RepairThreshhold) {
				if (!House->DidRepair) {
					if (!IsRepairing && (IsCaptured || IsToRepair || House->Is_Human_Player())) {
						House->DidRepair = true;
						Repair(1);
						if (!House->Is_Human_Player()) {
							House->RepairTimer = Random_Pick((int)(House->RepairDelay * (TICKS_PER_MINUTE / 4)), (int)(House->RepairDelay * TICKS_PER_MINUTE * 2));
						}
					}
				}
			} else {
				if ((Session.Type != GAME_NORMAL || IsAllowedToSell) &&
					IsTickedOff &&
					(unsigned)House->Control.IQ >= (unsigned)Rule->IQSellBack &&
					(unsigned)Random_Pick(0, 50) < (unsigned)House->Control.TechLevel &&
					Tag == NULL &&
					Class->ToBuild != RTTI_BUILDINGTYPE &&
					HealthRatio < Rule->ConditionRed)
				{
					Sell_Back(1);
				}
			}
		}
	}

	if (IsRepairing && (Frame % (int)(Rule->RepairRate * TICKS_PER_MINUTE)) == 0) {
		IsWrenchVisible = (IsWrenchVisible == false);
		int cost = Class->Repair_Cost();
		int step = Class->Repair_Step();
		int shapenum = Shape_Number();

		if (House->Available_Money() >= cost) {
			House->Spend_Money(cost);
			Strength += step;
			EstimatedHealth += step;
			if (Strength >= Class->MaxStrength) {
				EstimatedHealth = Strength = Class->MaxStrength;
				IsRepairing = false;
			}
			Set_Anim_Damage_State(HealthRatio <= Rule->ConditionYellow);
			if (HealthRatio > Rule->ConditionYellow && ParticleSystems[ATTACHED_PARTICLE_DAMAGE] != NULL) {
				ParticleSystems[ATTACHED_PARTICLE_DAMAGE]->Delete_Me();
			}
			if (Shape_Number() != shapenum) {
				IsToDisplay = true;
			}
		} else {
			IsRepairing = false;
		}
	}
}

// FUNCTION: 0x00451330
int BuildingClass::How_Many_Survivors(void) const
{
	if (IsSurvivorless || !Class->IsCrew) return(0);

	int divisor;
	if (House->ActLike == 0) {
		divisor = Rule->AlliedSurvivorDivisor;
	} else if (House->ActLike == 1) {
		divisor = Rule->SovietSurvivorDivisor;
	} else if (House->ActLike == 2) {
		divisor = Rule->ThirdSurvivorDivisor;
	} else {
		return(0);
	}
	if (divisor == 0) return(0);
	if (IsCaptured) divisor *= 2;
	int count = Class->GetRefund(House, false) / divisor;
	if (count < 1) return(1);
	if (count > 5) return(5);
	return(count);
}

// FUNCTION: 0x004513D0
void const * BuildingClass::Get_Image_Data(void) const
{
	if (BState != BSTATE_CONSTRUCTION && ActuallyPlacedOnMap) {
		return(Class->Get_Image_Data());
	}
	return(Class->Get_Buildup_Data());
}

// FUNCTION: 0x00451400
bool BuildingClass::Add_Upgrade(void)
{
	if (Strength != Class->MaxStrength) {
		Strength = Class->MaxStrength;
		Set_Anim_Damage_State(false);
		if (HealthRatio > Rule->ConditionYellow) {
			if (ParticleSystems[ATTACHED_PARTICLE_DAMAGE]) {
				ParticleSystems[ATTACHED_PARTICLE_DAMAGE]->Delete_Me();
			}
		}
		for (int i = BANIM_ACTIVE_ONE; i <= BANIM_ACTIVE_FOUR; i++) {
			if (Anims[i] == NULL) {
				PlayNthAnim((BAnimType)i, false, false);
			}
		}
		IsToDisplay = true;
	}

	if (Class == Rule->WallTower) {
		int index = TurretIndex + 1;
		if (index > 2) index = 0;
		Set_Turret_Index(index);
		UpgradeLevel++;
		return(true);
	}

	if (UpgradeLevel == Class->Upgrades) {
		return(false);
	}

	UpgradeLevel++;
	PlayNthAnim((BAnimType)(UpgradeLevel - 1), HealthRatio <= Rule->ConditionYellow, false);
	return(true);
}

// FUNCTION: 0x00451690
bool BuildingClass::Remove_Upgrade(void)
{
	if (UpgradeLevel == 0) {
		return(false);
	}

	bool super = false;
	if (Upgrades[UpgradeLevel-1] != NULL && Upgrades[UpgradeLevel-1]->SuperWeapon != SUPER_NONE) {
		super = true;
	}

	if (Upgrades[UpgradeLevel-1] != NULL && Upgrades[UpgradeLevel-1]->IsTurretEquipped) {
		DestroyNthAnim(BANIM_TURRET);
		Upgrades[UpgradeLevel-1] = NULL;
		UpgradeLevel = 0;
		TurretIndex = -1;
		if (super) House->Update_Present_Super_Weapons();
		return(true);
	}

	DestroyNthAnim((BAnimType)(UpgradeLevel-1));
	Upgrades[--UpgradeLevel] = NULL;
	if (super) House->Update_Present_Super_Weapons();
	return(true);
}

// FUNCTION: 0x00451750
void BuildingClass::PlayNthAnim(BAnimType anim, bool damaged, bool garrisoned, int delay)
{
	char const * name = damaged ? Class->AnimData[anim].AnimDamaged : (garrisoned ? Class->AnimData[anim].AnimGarrisoned : Class->AnimData[anim].Anim);
	if (name != NULL && *name) {
		PlayAnim(name, anim, damaged, garrisoned, delay);
	}
}

// FUNCTION: 0x004517D0
void BuildingClass::Set_Anim_Coords(void)
{
	Coord render = Render_Coord();
	for (int i = 0; i < BANIM_COUNT; i++) {
		AnimClass * anim = Anims[i];
		if (anim != NULL) {
			anim->Set_Coord(render + TacticalMap->Pixel_To_Coord_Absolute(Class->AnimData[i].Location));
		}
	}
}

static inline BuildingClass * Building_Of(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	return((object->Fetch_RTTI() == RTTI_BUILDING) ? (BuildingClass *)object : NULL);
}

static inline void Warp_Pause(BuildingClass * building, AnimClass * anim)
{
	if (building->IsBeingWarpedOut()) {
		anim->Paused = true;
	}
}

static inline void Power_Disable(BuildingClass * building, AnimClass * anim, int index)
{
	if (!(building->IsPoweredOn && building->IsPowerOnline()) && building->Class->IsPowered && building->Class->AnimData[index].Powered) {
		anim->Disable();
	}
}

// FUNCTION: 0x00451890
void BuildingClass::PlayAnim(char const * name, BAnimType anim, bool damaged, bool garrisoned, int delay)
{
	Set_Anim_Damage_State(damaged);

	AnimType animtype = AnimTypeClass::From_Name(name);
	if (animtype != ANIM_NONE) {
		Coord coord (Render_Coord() + TacticalMap->Pixel_To_Coord_Absolute(Class->AnimData[anim].Location));
		AnimClass * animptr = new AnimClass(AnimTypes[animtype], coord, delay, 1, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZREAD));

		animptr->ZAdjust = Class->AnimData[anim].ZAdjust;
		animptr->YSortAdjust = Class->AnimData[anim].YSort;
		animptr->IsBuildingAnim = true;

		if (IsFogged) {
			animptr->IsFogged = true;
		}

		Set_Anim_Translucency(TranslucencyLevel);

		if (Anims[anim] != NULL) {
			animptr->Animation.Set_Stage(Anims[anim]->Animation.Fetch_Stage());
			AnimClass * oldanim = Anims[anim];
			Anims[anim] = NULL;
			delete oldanim;
		}

		Anims[anim] = animptr;

		Power_Disable(this, animptr, anim);

		if (animptr->Type->ShouldUseCellDrawer) {
			animptr->LightConvert = (LightConvertClass *)GetRemapColour();
			int cellbrightness = Get_Cell_Ptr()->Brightness;
			int brightness = Apparent_Brightness(cellbrightness);
			if (IsIronCurtained() || (Airstrike != NULL && Airstrike->Target == Building_Of(this))) {
				brightness = GetEffectTintIntensity(brightness);
			}
			animptr->TintColor = brightness;
		}

		if (anim == BANIM_TURRET && Class->IsBarrelAnimAVoxel) {
			animptr->Make_Invisible();
		}

		Warp_Pause(this, animptr);
	}
}

// FUNCTION: 0x00451B40
void BuildingClass::Detach_Anim(AnimClass * anim)
{
	if (IsActive) {
		for (int i = 0; i < BANIM_COUNT; i++) {
			if (Anims[i] == anim) {
				Anims[i] = NULL;
				switch (i) {
					case BANIM_SPECIAL_ONE:
						if (Class->IsCanUnitRepair) {
							if (HasAnyLink() && Mission == MISSION_REPAIR) {
								Replay_Anim(this, BANIM_SPECIAL_TWO, HealthRatio <= Rule->ConditionYellow);
							} else {
								Replay_Anim(this, BANIM_IDLE, HealthRatio <= Rule->ConditionYellow);
							}
						} else if (Class->IsAnimDelayedFire && anim->TimeToDie) {
							Begin_Anim(BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
						}
						break;

					case BANIM_SPECIAL_THREE:
						if (Class->IsCanUnitRepair && anim->TimeToDie) {
							Replay_Anim(this, BANIM_IDLE, HealthRatio <= Rule->ConditionYellow);
						}
						break;

					case BANIM_SUPER_TWO:
						if (anim->TimeToDie) {
							Replay_Anim(this, BANIM_SUPER_THREE, HealthRatio <= Rule->ConditionYellow);
						}
						break;

					case BANIM_SUPER_FOUR:
						if (anim->TimeToDie) {
							Replay_Anim(this, BANIM_SUPER_ONE, HealthRatio <= Rule->ConditionYellow);
						}
						break;
				}
				break;
			}
		}
	}
}

// FUNCTION: 0x00451E40
void BuildingClass::DestroyNthAnim(BAnimType anim)
{
	if (anim == BANIM_ALL) {
		for (int i = 0; i < BANIM_COUNT; i++) {
			AnimClass * animation = Anims[i];
			if (animation != NULL) {
				Anims[i] = NULL;
				delete animation;
			}
		}
	} else {
		AnimClass * animation = Anims[anim];
		if (animation != NULL) {
			Anims[anim] = NULL;
			delete animation;
		}
	}
}

// FUNCTION: 0x00451EA0
void BuildingClass::HideNthAnim(BAnimType anim)
{
	if (anim == BANIM_ALL) {
		for (int i = 0; i < BANIM_COUNT; i++) {
			if (Anims[i] != NULL) {
				Anims[i]->Make_Invisible();
			}
		}
	} else {
		AnimClass * animation = Anims[anim];
		if (animation != NULL) {
			animation->Make_Invisible();
		}
	}
}

// FUNCTION: 0x00451EE0
void BuildingClass::Set_Anim_Damage_State(bool damaged)
{
	if (IsDamagedAnims != damaged) {
		IsDamagedAnims = damaged;
		for (int i = 0; i < BANIM_COUNT; i++) {
			if (Anims[i] != NULL) {
				PlayNthAnim((BAnimType)i, damaged, false);
			}
		}
	}
}

// FUNCTION: 0x00451F60
void BuildingClass::Set_Anim_Drawer(ConvertClass * drawer, int brightness)
{
	for (int i = 0; i < BANIM_COUNT; i++) {
		AnimClass * anim = Anims[i];
		if (anim != NULL) {
			if (anim->Type != NULL && anim->Type->ShouldUseCellDrawer) {
				anim->LightConvert = (LightConvertClass *)drawer;
				anim->TintColor = brightness;
			}
			Set_Anim_Translucency(TranslucencyLevel);
		}
	}
}

// FUNCTION: 0x00452000
void BuildingClass::Update_Anim_Appearance(void)
{
	ConvertClass * drawer = (ConvertClass *)GetRemapColour();
	int cellbrightness = Get_Cell_Ptr()->Brightness;
	int brightness = Apparent_Brightness(cellbrightness);
	if (IsIronCurtained() || (Airstrike != NULL && Airstrike->Target == Building_Of(this))) {
		brightness = GetEffectTintIntensity(brightness);
	}
	Brightness = brightness;
	Set_Anim_Drawer(drawer, Brightness);
	Set_Anim_Translucency(TranslucencyLevel);
}

// FUNCTION: 0x00452170
void BuildingClass::Set_Anim_Translucency(int translucency)
{
	int trans = translucency;
	if (trans == 15 && Visual_Character() == VISUAL_HIDDEN) {
		trans = 16;
	}

	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Anims[i] != NULL) {
			Anims[i]->TranslucencyLevel = trans;
		}
	}
}

// FUNCTION: 0x004521C0
void BuildingClass::DisableTemporal(void)
{
	IsOn = false;
	for (int i = 0; i < BANIM_COUNT; i++) {
		AnimClass * anim = Anims[i];
		if (anim != NULL) {
			anim->Paused = true;
			anim->Unpaused = false;
			anim->PausedAnimFrame = anim->Animation.Fetch_Stage();
			anim->UnderTemporal = true;
		}
	}
}

// FUNCTION: 0x00452210
void BuildingClass::EnableTemporal(void)
{
	IsOn = true;
	for (int i = 0; i < BANIM_COUNT; i++) {
		AnimClass * anim = Anims[i];
		if (anim != NULL) {
			anim->Paused = false;
			anim->Unpaused = true;
			anim->UnderTemporal = false;
		}
	}
}

// FUNCTION: 0x00452250
unsigned BuildingClass::vt_entry_504(void)
{
	return(0);
}

// FUNCTION: 0x00452260
void BuildingClass::Turn_On(void)
{
	if (!IsOn && EMPLockRemaining == 0) {
		IsOn = true;
		IsToDisplay = true;
		House->RecalcPower = true;
		House->RecalcRadar = true;
		House->IsRecalcNeeded = true;
		IsPoweredOn = true;

		if (LightSource) {
			LightSource->Enable();
		}

		if (Class->IsLaserFencePost) {
			Toggle_Laser_Fence_Post(false);
		}

		for (int i = 0; i < BANIM_COUNT; i++) {
			if (Class->AnimData[i].Powered && Anims[i] != NULL) {
				Anims[i]->Enable();
			}
		}

		if (Class->ToBuild != RTTI_NONE) {
			House->Update_Factories(Class->ToBuild, Class->Naval, 0);
		}

		if (House->Is_Player_Control()) {
			VoxClass::Play("EVA_BuildingOnLine", -1, -1);
		}
	}
}

// FUNCTION: 0x00452360
void BuildingClass::Turn_Off(void)
{
	if (IsOn && (Class->Drain > 0 || Class->IsPowered)) {
		IsOn = false;
		IsToDisplay = true;
		House->RecalcPower = true;
		House->RecalcRadar = true;
		House->IsRecalcNeeded = true;
		Power_Off();

		if (House->Is_Player_Control()) {
			VoxClass::Play("EVA_BuildingOffLine", -1, -1);
		}

		if (Class->ToBuild != RTTI_NONE) {
			House->Update_Factories(Class->ToBuild, Class->Naval, 0);
		}
	}
}

// FUNCTION: 0x00459E80
HRESULT STDMETHODCALLTYPE BuildingClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingClass;
	return(S_OK);
}

// FUNCTION: 0x0045A130 ?$TRect@H::?$TRect@H
template class TRect<int>;
