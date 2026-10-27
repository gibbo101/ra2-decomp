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
#include "capmgr.h"
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
#include "psystype.h"
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

class Reestablish;
extern Point2D DAT_0089C848;
extern Cell DAT_0089C818;
extern TPoint3D<int> DAT_0089C870;
SideType FUN_006A46D0(char const * name);
void __fastcall FUN_00750D40(Coord const & coord, AudioController * controller);


// FUNCTION: GAMEMD 0x0043B4C0
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


// FUNCTION: GAMEMD 0x0043B5E0
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


// FUNCTION: GAMEMD 0x0043B680
BuildingClass::BuildingClass(NoInitClass const & x) :
	BASECLASS(x),
	CountDown(x),
	PlacementDelay(x),
	GateTimer(x),
	BuildingStage(),
	CashProductionTimer(x)
{
}


// FUNCTION: GAMEMD 0x0043B740
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

// FUNCTION: GAMEMD 0x0043C0D0
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

// FUNCTION: GAMEMD 0x0043C2A0
void BuildingClass::FUN_0043C2A0(void)
{
	for (int i = 0; i < 8; i++) {
		if (DamageFireAnims[i]) {
			DamageFireAnims[i]->Delete_Me();
			DamageFireAnims[i] = 0;
		}
	}
}

bool FUN_004722C0(CaptureManagerClass const * manager);

// FUNCTION: GAMEMD 0x0043C2D0
RadioMessageType BuildingClass::Receive_Message(RadioClass * from, RadioMessageType message, int & param)
{
	switch (message) {

		case RADIO_CAN_LOAD:
			BASECLASS::Receive_Message(from, message, param);
			if (!House->Is_Ally(from)) return(RADIO_STATIC);
			if (Mission == MISSION_CONSTRUCTION || Mission == MISSION_DECONSTRUCTION || BState == BSTATE_CONSTRUCTION || (!ScenarioInit && !HasFreeLink((TechnoClass *)from) && !Class->UnitAbsorb && !Class->InfantryAbsorb)) return(RADIO_NEGATIVE);
			if (((TechnoClass *)from)->Techno_Type_Class()->MZone != 5) {
				if ((Techno_Type_Class()->Naval && !((TechnoClass *)from)->Techno_Type_Class()->Naval) || (!Techno_Type_Class()->Naval && ((TechnoClass *)from)->Techno_Type_Class()->Naval)) {
					return(RADIO_NEGATIVE);
				}
			}
			if (((TechnoClass *)from)->Techno_Type_Class()->BalloonHover) return(RADIO_NEGATIVE);
			if (!IsOn) return(RADIO_NEGATIVE);
			if (Class->UnitAbsorb || Class->InfantryAbsorb) {
				if ((from->Fetch_RTTI() == RTTI_UNIT && !Class->UnitAbsorb) || (from->Fetch_RTTI() == RTTI_INFANTRY && !Class->InfantryAbsorb)) {
					return(RADIO_NEGATIVE);
				}
				if (((TechnoClass *)from)->CaptureManager != NULL && FUN_004722C0(((TechnoClass *)from)->CaptureManager)) return(RADIO_NEGATIVE);
				if (Cargo.How_Many() + 1 <= Class->MaxPassengers && ((TechnoClass *)from)->Techno_Type_Class()->Size <= Class->SizeLimit) {
					goto roger;
				}
			}
			if (Class->Grinding) goto roger;
			if (Class->Bunker) {
				if (((TechnoClass *)from)->FUN_0070FB50()) {
					if (Transmit_Message(RADIO_ON_DEPOT, from) != RADIO_ROGER) {
						return(RADIO_ROGER);
					}
				}
				return(RADIO_NEGATIVE);
			}
			if (Class->IsCanUnitRepair) {
				if (from->Fetch_RTTI() == RTTI_UNIT || from->Fetch_RTTI() == RTTI_AIRCRAFT) {
					if (Transmit_Message(RADIO_ON_DEPOT, from) != RADIO_ROGER) {
						return(RADIO_ROGER);
					}
				}
				return(RADIO_NEGATIVE);
			}
			if (Class->IsArmory || Class->IsHospital) {
				if (from->Fetch_RTTI() == RTTI_INFANTRY) {
					InfantryClass * infantry = !from ? NULL : (from->Fetch_RTTI() == RTTI_INFANTRY ? (InfantryClass *)from : NULL);
					if (infantry->CaptureManager != NULL && FUN_004722C0(infantry->CaptureManager)) return(RADIO_NEGATIVE);
					if (infantry->IsMindControlled()) return(RADIO_NEGATIVE);
					return(Ammo != 0 ? RADIO_ROGER : RADIO_NEGATIVE);
				}
			}
			if (Class->IsHelipad) {
				return(from->What_Am_I() == RTTI_AIRCRAFT ? RADIO_ROGER : RADIO_NEGATIVE);
			}
			if (Class->IsDockUnload && from->Fetch_RTTI() == RTTI_UNIT && ((UnitClass *)from)->Class->Harvester && (ScenarioInit || !Cargo.Is_Something_Attached())) {
				goto roger;
			}
			if (Class->IsWeeder && from->Fetch_RTTI() == RTTI_UNIT && ((UnitClass *)from)->Class->Weeder) {
				if (ScenarioInit) goto roger;
				if (!Cargo.Is_Something_Attached()) return(RADIO_ROGER);
			}
			return(RADIO_STATIC);

		case RADIO_IM_IN:
			if (Mission == MISSION_DECONSTRUCTION) {
				return(RADIO_NEGATIVE);
			}
			if (Class->UnitAbsorb || Class->InfantryAbsorb) {
				goto roger;
			}
			if (Class->IsCanUnitRepair || Class->IsCanUnitReload || Class->IsHospital || Class->IsArmory) {
				IsReadyToCommence = true;
				QueueMission(MISSION_REPAIR, false);
				from->QueueMission(MISSION_SLEEP, false);
				return(RADIO_ROGER);
			}
			if (Class->Bunker) {
				IsReadyToCommence = true;
				QueueMission(MISSION_REPAIR, false);
				return(RADIO_ROGER);
			}
			if (Class->IsDockUnload) {
				from->QueueMission(MISSION_UNLOAD, false);
				return(RADIO_ROGER);
			}
			break;

		case RADIO_DOCKING: {
			BASECLASS::Receive_Message(from, message, param);

			if (!IsOn) {
				return(RADIO_NEGATIVE);
			}

			if (Class->IsCanUnitRepair && ContainsLink((TechnoClass *)from)) {
				if (Transmit_Message(RADIO_NEED_REPAIR, from) == RADIO_NEGATIVE) {
					return(RADIO_NEGATIVE);
				}
			}

			if (Class->Bunker && ContainsLink((TechnoClass *)from)) {
				if (!((TechnoClass *)from)->FUN_0070FB50()) {
					return(RADIO_NEGATIVE);
				}
			}

			if (Class->IsHospital || Class->IsArmory) {
				if (!HasFreeLink((TechnoClass *)from)) {
					for (int i = 0; i < RadioLinks.Length(); i++) {
						TechnoClass * link = FUN_0065AD30(i);
						if (Transmit_Message(RADIO_NEED_REPAIR, link) == RADIO_NEGATIVE) {
							Transmit_Message(RADIO_RUN_AWAY, link);
						}
					}
					if (HasFreeLink((TechnoClass *)from)) goto roger;
					return(RADIO_NEGATIVE);
				}
				param = (int)&Map[Get_Coord()];
				Transmit_Message(RADIO_MOVE_HERE, param, from);
				return(RADIO_ROGER);
			}

			if (!ContainsLink((TechnoClass *)from) && HasFreeLink((TechnoClass *)from)) {
				Transmit_Message(RADIO_HELLO, from);
				ContainsLink((TechnoClass *)from);
			}

			bool needs_to_move = false;

			if (ContainsLink((TechnoClass *)from)) {
				if (Class->IsDockUnload || Class->IsWeeder) {
					FootClass * foot = !from ? NULL : ((from->AbstractFlags & 4) ? (FootClass *)from : NULL);
					CellClass * docking_cell = &Map[Docking_Coord(foot)];
					AbstractClass * navcom = foot->NavCom;
					if (navcom != NULL && docking_cell != navcom) {
						needs_to_move = true;
					}
				}
			}

			if (FUN_0065AD30(0) != NULL) {
				if (Class->IsCanUnitRepair || Class->Bunker) {
					if (Distance_To(FUN_0065AD30(0)) > CELL_LEPTON / 2) {
						needs_to_move = true;
					}
				}
			}

			if (Transmit_Message(RADIO_NEED_TO_MOVE, from) == RADIO_ROGER || needs_to_move) {
				param = (int)this;
				if (Class->IsDockUnload || Class->IsWeeder) {
					param = (int)&Map[Get_Cell() + Cell(3, 1)];

					if (Transmit_Message(RADIO_MOVE_HERE, param, from) == RADIO_YEA_NOW_WHAT) {
						Transmit_Message(RADIO_TETHER, from);
						if (Transmit_Message(RADIO_BACKUP_NOW, from) != RADIO_ROGER) {
							from->Scatter(COORD_NONE, true, true);
						}
					}
				} else if (Class->IsHelipad) {
					param = (int)this;
					if (Transmit_Message(RADIO_MOVE_HERE, param, from) == RADIO_YEA_NOW_WHAT) {
						Transmit_Message(RADIO_TETHER);
					}
				}
			}
			return(RADIO_ROGER);
		}

		case RADIO_ARE_REFINERY:
			if (Cargo.Is_Something_Attached() || !HasFreeLink((TechnoClass *)from) || IsInLimbo || House != from->Owner_HouseClass() || (!Class->IsRefinery && !Class->IsCanUnitRepair && !Class->IsWeeder)) {
				return(RADIO_NEGATIVE);
			}
			return(RADIO_ROGER);

		case RADIO_BUILDING:
			QueueMission(MISSION_REPAIR, false);
			BASECLASS::Receive_Message(from, message, param);
			return(RADIO_ROGER);

		case RADIO_COMPLETE:
			if (Mission != MISSION_DECONSTRUCTION) {
				QueueMission(MISSION_GUARD, false);
				if (Class->IsConstructionYard) {
					DestroyNthAnim(BANIM_PRE_PRODUCTION);
					DestroyNthAnim(BANIM_IDLE);
					PlayNthAnim(BANIM_PRODUCTION, HealthRatio <= Rule->ConditionYellow, false, 0);
				}
			}
		base:
			BASECLASS::Receive_Message(from, message, param);
		roger:
			return(RADIO_ROGER);

		case RADIO_OVER_OUT:
			Begin_Mode(BSTATE_IDLE);
			BASECLASS::Receive_Message(from, message, param);
			return(RADIO_ROGER);

		case RADIO_UNLOADED:
			if (Class->IsCanUnitRepair || Class->Bunker) {
				if (Distance_To((TechnoClass *)from) < 3 * CELL_LEPTON / 2) {
					return(RADIO_ROGER);
				}
			}
			BASECLASS::Receive_Message(from, message, param);
			if (Class->IsWeaponsFactory || Class->IsCanUnitRepair || Class->Bunker) return(RADIO_RUN_AWAY);
			return(RADIO_ROGER);

		case RADIO_REDRAW:
			if (Class->IsWeaponsFactory) {
				return(RADIO_ROGER);
			}
			break;

		default:
			break;
	}

	return(BASECLASS::Receive_Message(from, message, param));
}


// FUNCTION: GAMEMD 0x0043CEA0
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

// FUNCTION: GAMEMD 0x0043D030
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

// FUNCTION: GAMEMD 0x0043E900
int BuildingClass::Get_Z_Adjust(void) const
{
	int pixel = -Tactical::Z_Lepton_To_Pixel(Height);

	if (Class->IsTurretAnimAVoxel) {
		return(Class->AnimData[BANIM_TURRET].ZAdjust + pixel);
	}
	return(pixel);
}

// FUNCTION: GAMEMD 0x0043ED40
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

// FUNCTION: GAMEMD 0x0043EE50
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

// FUNCTION: GAMEMD 0x0043EEB0
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

// FUNCTION: GAMEMD 0x0043EF90
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

extern FacingType DAT_00818CB8[4];

// FUNCTION: GAMEMD 0x004415F0
void BuildingClass::Do_Destruction(TechnoClass * last_contact, TechnoClass * source, bool forced, Cell const * offset)
{
	FUN_0043C2A0();

	if (SpiedBy) {
		SpiedBy = 0;
		if (Class->IsRadar) {
			Update_Radar_Spied();
		}
	}

	if (Class->IsCloakGenerator) {
		Disable_Cloak_Generator();
		CurrentCloakRadius = 1;
		Cloaking_AI(true);
	}

	if (Class->IsLaserFencePost) {
		Update_Laser_Fence_Connections(true);
	}

	if (Class->RevealToAll && !House->Is_Player_Control()) {
		vt_entry_48C(0, 0, 1, PlayerPtr);
	}

	if (Techno_Type_Class()->DieSound.Count() == 0) {
		VocClass::PlayIndexAtPos(Rule->BuildingDieSound, PositionCoord, 0);
	}

	if (Class->Width() >= 2 && Class->Height(false) >= 2) {
		if (Class->Width() > 2) {
			Random_Pick(0, Class->Width() - 2);
		}
		if (Class->Height(false) > 2) {
			Random_Pick(0, Class->Height(false) - 2);
		}
		if (Percent_Chance(50)) {
			SmudgeTypeClass::Scorch_The_Ground(Coord(PositionCoord.As_Cell()), 100, 100, true);
		} else {
			SmudgeTypeClass::Crater_The_Ground(Coord(PositionCoord.As_Cell()), 100, 100, true);
		}
	}

	while (*offset != REFRESH_EOL) {
		Coord pos;
		Coord coord(0, 0, 0);
		Cell cell = Render_Coord().As_Cell() + *offset++;
		if (Class->Explosion.Count() > 0) {
			coord.Z = PositionCoord.Z;
			Coord ccoord = cell;
			pos = coord + Coord_Scatter(ccoord, CELL_LEPTON / 4);
			new AnimClass(Class->Explosion.Pick(Scen->RandomNumber), pos, Random_Pick(0, 3), 1, (ShapeFlags_Type)0x600, 0, false);
		}
	}

	if (Class->IsExploding) {
		for (int i = 0; i < 4; i++) {
			Coord coord = Adjacent_Coord_With_Height(PositionCoord, DAT_00818CB8[i]);
			if (Map[coord].Overlay != OVERLAY_NONE) {
				if (OverlayTypes[Map[coord].Overlay]->IsExplosive) {
					new AnimClass(AnimTypes[AnimTypeClass::From_Name("FIRE3")], coord, Random_Pick(1, 3) + 3, 1, (ShapeFlags_Type)0x600, 0, false);
				}
			}
		}
	}

	while (Storage.Get_Total_Amount() >= 1.0f) {
		int slot = Storage.First_Used_Slot();
		Storage.Decrease_Amount(1.0f, slot);
		House->Tiberium.Decrease_Amount(1.0f, slot);
		Coord coord = Coord_Scatter(PositionCoord, Random_Pick(CELL_LEPTON, 3 * CELL_LEPTON), true);
		Map[coord].Place_Tiberium((TiberiumType)slot, 1);
	}

	int shakes = Class->Cost_Of(House) / Rule->ShakeScreen;
	if (shakes > 0) {
		Shake_The_Screen(shakes);
	}

	if (Get_Mission() == MISSION_DECONSTRUCTION || Class->IsExploding) {
		CountDown = 0;
		Animation.Set_Rate(0);
	} else {
		CountDown = 8;
	}

	if (Class->DestroyAnim.Count() > 0) {
		AnimTypeClass const * type = Class->DestroyAnim.Pick(Scen->RandomNumber);
		if (type != NULL) {
			AnimClass * anim = new AnimClass(type, Render_Coord(), 0, 1, (ShapeFlags_Type)0x600, 0, false);
			if (Techno_Type_Class()->Palette != NULL) {
				anim->LightConvert = (LightConvertClass *)GetRemapColour();
				if (Class->PaletteFile[0]) {
					strncpy(anim->PaletteName, Class->PaletteFile, 32);
				}
			}
		}
	}

	DynamicVectorClass<ParticleSystemTypeClass const *> list;
	for (int i = Techno_Type_Class()->DestroyParticleSystems.Count() - 1; i >= 0; i--) {
		if (Techno_Type_Class()->DestroyParticleSystems[i]->BehavesLike == 0) {
			list.Add(Techno_Type_Class()->DestroyParticleSystems[i]);
		}
	}

	if (ParticleSystems[7] == NULL && list.Count() > 0 && Get_Height_AGL() > -10) {
		Coord off = Techno_Type_Class()->DestroySmokeOffset;
		ParticleSystems[7] = new ParticleSystemClass(list[Random_Pick(0, list.Count() - 1)], PositionCoord + off, NULL, this, (Coord const &)DAT_0089C848, NULL);
	}

	Strength = 0;
	if (forced) {
		IsSurvivorless = true;
	}
	Drop_Debris(WhomToRepay);
	Kill_Cargo(source);
}

// FUNCTION: GAMEMD 0x00441F60
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

// FUNCTION: GAMEMD 0x00442230
ResultType BuildingClass::Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced, bool no_crew, HouseClass * house)
{
	ResultType res = RESULT_NONE;

	if (this != source || source->Techno_Type_Class()->DamageSelf) {
		float healthratio = HealthRatio;
		int shapenum = Shape_Number();

		if (source != NULL && !IsStrange()) {
			House->LATime = Frame;
			House->LAEnemy = source->Owner_HouseClass()->HeapID;
			Base_Is_Attacked(source);
		}

		Cell const * offset = Occupy_List(false);
		DynamicVectorClass<TechnoClass *> contacts;
		int i;
		for (i = 0; i < RadioLinks.Length(); i++) {
			TechnoClass * link = FUN_0065AD30(i);
			if (link != NULL) {
				contacts.Add(link);
			}
		}

		if (Class->IsLaserFence && !forced) {
			return(RESULT_NONE);
		}

		if (Class->IsBridgeRepairHut && Class->IsImmune) {
			return(RESULT_NONE);
		}

		if (Strength != 0) {
			res = BASECLASS::Take_Damage(damage, distance, warhead, source, forced, no_crew, house);
			if (!IsActive) {
				return(res);
			}
			switch (res) {
				case RESULT_ALREADY_DESTROYED:
					return(res);

				case RESULT_DESTROYED:
					if (BunkerLinkedItem != NULL) {
						contacts.Delete(BunkerLinkedItem);
						UnloadBunker();
					}
					if (CaptureManager != NULL) {
						CaptureManager->FreeAll();
					}
					if (LocomotorTarget != NULL) {
						ReleaseLocomotor(true);
					}
					for (i = 0; i < contacts.Count(); i++) {
						TechnoClass * tech = contacts[i];
						if (Center_Coord().Distance_To(tech->Center_Coord()) >= CELL_LEPTON && !Class->IsHelipad) {
							Transmit_Message(RADIO_RUN_AWAY, tech);
							tech->QueueUpToEnter = NULL;
						} else {
							int strength = tech->Techno_Type_Class()->MaxStrength * 10;
							tech->Take_Damage(strength, 0, Rule->C4Warhead, NULL, true, true, NULL);
						}
					}
					contacts.Delete_All();
					if (Class->CanBeOccupied) {
						FUN_00457DE0(false, false);
					}
					if (LightSource != NULL) {
						LightSource->Disable(false);
					}
					Do_Destruction(NULL, source, forced, offset);
					if (CountDown > 0) {
						Delete_Me();
						AfterDestruction();
					}
					break;

				case RESULT_HALF:
					if (ParticleSystems[2] != NULL) {
						ParticleSystems[2]->SpawnFrames *= 1.5f;
					}

				case RESULT_MAJOR:
					if (Techno_Type_Class()->DamageSound == -1) {
						VocClass::PlayIndexAtPos(Rule->BuildingDamageSound, PositionCoord, 0);
					}
					while (*offset != REFRESH_EOL) {
						Cell cell = Get_Cell() + Cell(*offset++);
						Coord coord(cell);
						coord.Z = Map.Get_Height_GL(coord);
						if (warhead->IsSparky) {
							AnimClass * anim = NULL;
							switch (Random_Pick(0, 5 + Class->Height(false) + Class->Width())) {
								case 1:
								case 2:
								case 3:
								case 4:
								case 5:
									anim = new AnimClass(Rule->OnFire[0], Coord_Scatter(coord, 3 * CELL_LEPTON / 8, false), 0, Random_Pick(1, 3), (ShapeFlags_Type)0x600, 0, false);
									break;

								case 6:
								case 7:
								case 8:
									anim = new AnimClass(Rule->OnFire[1], Coord_Scatter(coord, 3 * CELL_LEPTON / 8, false), 0, Random_Pick(1, 3), (ShapeFlags_Type)0x600, 0, false);
									break;

								case 9:
									anim = new AnimClass(Rule->OnFire[2], Coord_Scatter(coord, 3 * CELL_LEPTON / 8, false), 0, 1, (ShapeFlags_Type)0x600, 0, false);
									break;
							}
							if (anim) {
								anim->SetOwnerObject(this);
							}
						}
					}
					break;

				default:
					break;
			}
		}

		if (!IsActive) {
			return(RESULT_DESTROYED);
		}

		if (source != NULL && res != RESULT_NONE) {
			if (!Class->IsInsignificant && !IsStrange()) {
				House->Attacked(this);
			}
			WhoLastHurtMe = source->Owner_HouseClass()->HeapID;
			if (CurrentMission != MISSION_DECONSTRUCTION &&
				!House->Is_Ally(source) &&
				Get_Class_Weapon_Data(0)->Weapon != NULL &&
				!Get_Class_Weapon_Data(0)->Weapon->Bullet->IsAntiAircraft &&
				(TarCom == NULL || !IsCloseEnoughToAttack(TarCom))) {
				if (source->Fetch_RTTI() != RTTI_AIRCRAFT && (!House->FUN_0050B730() || Rule->IsSmartDefense)) {
					Assign_Target(source);
				} else if (!PrimaryFacing.Is_Rotating() && IsPowerOnline()) {
					PrimaryFacing.Set_Desired(Random_Dir(DIR_N, DIR_MAX));
				}
			}
		}

		if (res != RESULT_NONE) {
			Set_Anim_Damage_State(HealthRatio <= Rule->ConditionYellow);
		}

		if (shapenum != Shape_Number()) {
			IsToDisplay = true;
			Set_Anim_Damage_State(HealthRatio <= Rule->ConditionYellow);
		}
		int remaining = contacts.Count();
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00442C40
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

// FUNCTION: GAMEMD 0x00443410
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

// FUNCTION: GAMEMD 0x004436F0
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

// FUNCTION: GAMEMD 0x00443860
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

// FUNCTION: GAMEMD 0x00443B90
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

// FUNCTION: GAMEMD 0x004456D0
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

// FUNCTION: GAMEMD 0x00445880
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

// FUNCTION: GAMEMD 0x00445E50
DirType BuildingClass::Turret_Facing(void) const
{
	if (!Is_Turret_Equipped() && TarCom != NULL) {
		return(::Direction(Center_Coord(), TarCom->Center_Coord()));
	}
	return(PrimaryFacing.Current());
}

// FUNCTION: GAMEMD 0x00445F00
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

// FUNCTION: GAMEMD 0x00446FF0
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

// FUNCTION: GAMEMD 0x00447110
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

// FUNCTION: GAMEMD 0x00447210
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

// FUNCTION: GAMEMD 0x00447540
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

// FUNCTION: GAMEMD 0x00447780
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

// FUNCTION: GAMEMD 0x00447AC0
Coord BuildingClass::Center_Coord(void) const
{
	int h = (Class->Height(false) * (CELL_LEPTON_H / 2)) - (CELL_LEPTON_H / 2);
	int w = (Class->Width() * (CELL_LEPTON_W / 2)) - (CELL_LEPTON_W / 2);

	return(PositionCoord + Coord(w, h));
}

// FUNCTION: GAMEMD 0x00447B20
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

// FUNCTION: GAMEMD 0x00447E00
int BuildingClass::DistanceToDockingCoord(TechnoClass * docker) const
{
	return(Docking_Coord(docker).Distance_To(docker->Center_Coord()));
}

// FUNCTION: GAMEMD 0x00447E90
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

// FUNCTION: GAMEMD 0x00447F10
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

// FUNCTION: GAMEMD 0x00448070
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

// FUNCTION: GAMEMD 0x00448160
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

// FUNCTION: GAMEMD 0x00449410
int BuildingClass::Sort_Y(void) const
{
	int y = BASECLASS::Sort_Y();
	return(y + (Class->IsTurretAnimAVoxel != 0 ? 32 : 0) - (Class->IsGate != 0 ? 16 : 0));
}

// FUNCTION: GAMEMD 0x00449440
MoveType BuildingClass::Can_Enter_Cell(CellClass const * cell, FacingType dir, int cell_height, CellClass const *, bool) const
{
	Cell c = cell->CellID;
	if (Class->UndeploysInto && IsDown) {
		return(Map[c].Is_Clear_To_Build(Class->Speed, (BuildingTypeClass *)Class, House) ? MOVE_OK : MOVE_NO);
	}
	return(Class->Legal_Placement(c, House) ? MOVE_OK : MOVE_NO);
}

// FUNCTION: GAMEMD 0x004494C0
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

// FUNCTION: GAMEMD 0x00449540
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

// FUNCTION: GAMEMD 0x004496B0
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

// FUNCTION: GAMEMD 0x00449A40
int BuildingClass::Do_MISSION_GUARD_AREA(void)
{
	return(Do_MISSION_GUARD());
}

// FUNCTION: GAMEMD 0x00449A50
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

// FUNCTION: GAMEMD 0x00449BC0
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

// FUNCTION: GAMEMD 0x0044ABD0
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

// FUNCTION: GAMEMD 0x0044B760
int BuildingClass::Do_MISSION_CAPTURE(void)
{
	return(MissionClass::Do_MISSION_CAPTURE());
}

// FUNCTION: GAMEMD 0x0044B770
int BuildingClass::Do_MISSION_HARVEST(void)
{
	return(MissionClass::Do_MISSION_HARVEST());
}

// FUNCTION: GAMEMD 0x0044D5D0
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

// FUNCTION: GAMEMD 0x0044D6A0
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

// FUNCTION: GAMEMD 0x0044D700
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

// FUNCTION: GAMEMD 0x0044D760
void BuildingClass::Death_Announcement(TechnoClass const * source) const
{
	if (source != NULL && House->Is_Player_Control()) {
		Cell cell = Destination_Coord().As_Cell();
		FUN_00660B80(cell);
	}
}

// FUNCTION: GAMEMD 0x0044E3A0
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

// FUNCTION: GAMEMD 0x0044E440
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

// FUNCTION: GAMEMD 0x0044E7B0
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

// FUNCTION: GAMEMD 0x0044E880
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

// FUNCTION: GAMEMD 0x0044E8F0
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

// FUNCTION: GAMEMD 0x0044EB10
InfantryTypeClass const * BuildingClass::Crew_Type(void) const
{
	if (!IsCaptured && Percent_Chance(25) && Class->ToBuild == RTTI_BUILDINGTYPE) {
		return(Rule->Engineer);
	}
	return(BASECLASS::Crew_Type());
}

// FUNCTION: GAMEMD 0x0044EB60
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

// FUNCTION: GAMEMD 0x0044EBF0
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

// FUNCTION: GAMEMD 0x0044EF80
int BuildingClass::Flush_For_Placement(TechnoClass * techno, Cell const & cell)
{
	if (techno) {
		return(((BuildingTypeClass const *)techno->Class_Of())->Flush_For_Placement(cell, House));
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x0044F5C0
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

// FUNCTION: GAMEMD 0x0044F640
Coord BuildingClass::Exit_Coord(int index) const
{
	if (Class->ExitCoordinate != (Coord const &)DAT_0089C848) {
		return(Class->ExitCoordinate + PositionCoord);
	}
	return(Center_Coord());
}

// FUNCTION: GAMEMD 0x0044F6E0
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

// FUNCTION: GAMEMD 0x0044F7A0
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

// FUNCTION: GAMEMD 0x0044FE60
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

// FUNCTION: GAMEMD 0x0044FEC0
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

// FUNCTION: GAMEMD 0x004500A0
Coord BuildingClass::Target_Coord(void) const
{
	Coord center = Center_Coord();
	return(Class->TargetCoordOffset + center);
}

// FUNCTION: GAMEMD 0x004500F0
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

// FUNCTION: GAMEMD 0x004503E0
void BuildingClass::Rotation_AI(void)
{
}

// FUNCTION: GAMEMD 0x004503F0
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

// FUNCTION: GAMEMD 0x004504F0
void BuildingClass::FUN_004504F0(int stage, CoordStruct const & coords)
{
	DelayBeforeFiring = Class->DelayedFireDelay;
	PrismStage = stage;
	PrismTargetCoords = coords;
	DestroyNthAnim(BANIM_ACTIVE_ONE);
	Replay_Anim(this, BANIM_SPECIAL_ONE, HealthRatio <= Rule->ConditionYellow);
}

// FUNCTION: GAMEMD 0x00450590
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

// FUNCTION: GAMEMD 0x00450630
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

// FUNCTION: GAMEMD 0x00451330
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

// FUNCTION: GAMEMD 0x004513D0
void const * BuildingClass::Get_Image_Data(void) const
{
	if (BState != BSTATE_CONSTRUCTION && ActuallyPlacedOnMap) {
		return(Class->Get_Image_Data());
	}
	return(Class->Get_Buildup_Data());
}

// FUNCTION: GAMEMD 0x00451400
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

// FUNCTION: GAMEMD 0x00451690
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

// FUNCTION: GAMEMD 0x00451750
void BuildingClass::PlayNthAnim(BAnimType anim, bool damaged, bool garrisoned, int delay)
{
	char const * name = damaged ? Class->AnimData[anim].AnimDamaged : (garrisoned ? Class->AnimData[anim].AnimGarrisoned : Class->AnimData[anim].Anim);
	if (name != NULL && *name) {
		PlayAnim(name, anim, damaged, garrisoned, delay);
	}
}

// FUNCTION: GAMEMD 0x004517D0
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

// FUNCTION: GAMEMD 0x00451890
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

// FUNCTION: GAMEMD 0x00451B40
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

// FUNCTION: GAMEMD 0x00451E40
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

// FUNCTION: GAMEMD 0x00451EA0
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

// FUNCTION: GAMEMD 0x00451EE0
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

// FUNCTION: GAMEMD 0x00451F60
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

// FUNCTION: GAMEMD 0x00452000
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

// FUNCTION: GAMEMD 0x00452170
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

// FUNCTION: GAMEMD 0x004521C0
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

// FUNCTION: GAMEMD 0x00452210
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

// FUNCTION: GAMEMD 0x00452250
unsigned BuildingClass::vt_entry_504(void)
{
	return(0);
}

// FUNCTION: GAMEMD 0x00452260
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

// FUNCTION: GAMEMD 0x00452360
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

// FUNCTION: GAMEMD 0x00452410
void BuildingClass::EnableStuff(void)
{
	IsPoweredOn = true;
	if (LightSource != NULL) {
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
}

// FUNCTION: GAMEMD 0x00452480
void BuildingClass::DisableStuff(void)
{
	IsPoweredOn = false;
	if (LightSource != NULL) {
		LightSource->Disable();
	}
	if (Class->IsCloakGenerator) {
		CloakGeneratorState = -1;
		if (CurrentCloakRadius == 0) {
			CurrentCloakRadius = Class->CloakRadiusInCells;
		}
		IsToDisplay = true;
	}
	if (Class->GapGenerator && GeneratingGap) {
		DestroyGap();
	}
	if (Class->IsLaserFencePost) {
		Toggle_Laser_Fence_Post(false);
	}
	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Class->AnimData[i].Powered && Anims[i] != NULL) {
			Anims[i]->Disable();
		}
	}
}

// FUNCTION: GAMEMD 0x00452540
bool BuildingClass::Open_Gate(void)
{
	if (!Class->IsGate) {
		return(true);
	}
	if (Get_Mission() == MISSION_OPEN && !Door.Is_Door_Closing() && !Door.Is_Door_Closed()) {
		return(Is_Gate_Open());
	}
	Set_Mission(MISSION_NONE);
	QueueMission(MISSION_OPEN, false);
	Commence();
	return(false);
}

// FUNCTION: GAMEMD 0x004525F0
bool BuildingClass::Is_Gate_Open(void) const
{
	if (!Class->IsGate) {
		return(true);
	}
	if (Get_Mission() == MISSION_OPEN && Door.Is_Door_Open()) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00452630
bool BuildingClass::Can_Repair(void) const
{
	if (Strength == 0) {
		return(false);
	}
	if (!Class->ClickRepairable) {
		return(false);
	}
	if (IsStrange()) {
		return(false);
	}
	return(BASECLASS::Can_Repair());
}

// FUNCTION: GAMEMD 0x00452670
bool BuildingClass::Can_Upgrade(BuildingTypeClass const * upgrade, HouseClass const * upgrader) const
{
	if (upgrader != House || stricmp(upgrade->PowersUpBuilding, Class->IniName)) {
		goto failed;
	}

	if (upgrade->PowersUpToLevel != -1) {
		if (upgrade->PowersUpToLevel <= 0 || upgrade->PowersUpToLevel > BUILDING_UPGRADE_MAX) {
			goto failed;
		}
	} else {
		if (UpgradeLevel < Class->Upgrades) {
			return(true);
		}
	}

	if (UpgradeLevel == 0) {
		return(true);
	}

failed:
	return(false);
}

// FUNCTION: GAMEMD 0x004526F0
WeaponDataStruct const * BuildingClass::Get_Class_Weapon_Data(int which) const
{
	if (UpgradeLevel != 0) {
		for (int i = 0; i < UpgradeLevel; i++) {
			if (Upgrades[i] != NULL) {
				WeaponDataStruct const * weapon = Upgrades[i]->Get_Weapon(which);
				if (weapon->Weapon != NULL) {
					return(weapon);
				}
			}
		}
	}
	if (CanOccupyFire() && FiringOccupantIndex < Occupants.Count()) {
		InfantryClass * occupant = Occupants[FiringOccupantIndex];
		if (occupant->Veterancy.Is_Elite()) {
			if (occupant->Class->EliteOccupyWeapon.Weapon != NULL) {
				return(&occupant->Class->EliteOccupyWeapon);
			}
			return(occupant->Get_Class_Weapon_Data(0));
		}
		if (occupant->Class->OccupyWeapon.Weapon != NULL) {
			return(&occupant->Class->OccupyWeapon);
		}
		return(occupant->Get_Class_Weapon_Data(0));
	}
	return(BASECLASS::Get_Class_Weapon_Data(which));
}

// FUNCTION: GAMEMD 0x004527D0
bool BuildingClass::Is_Turret_Equipped(void) const
{
	if (Class->IsTurretEquipped) {
		return(true);
	}
	if (UpgradeLevel != 0) {
		for (int i = 0; i < UpgradeLevel; i++) {
			if (Upgrades[i] != NULL && Upgrades[i]->IsTurretEquipped) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00452820
void BuildingClass::FUN_00452820(InfantryClass * infantry)
{
	Overpowerers.Delete(infantry);
	Overpowerers.Add(infantry);
}

// FUNCTION: GAMEMD 0x004528A0
bool BuildingClass::FUN_004528A0(AbstractClass const * target)
{
	Coord a = Center_Coord();
	Coord b = target->Center_Coord();
	return(a.Distance_To(b) <= Class->PsychicDetectionRadius << 8);
}

// FUNCTION: GAMEMD 0x00452940
bool BuildingClass::FUN_00452940(CellClass const * cell)
{
	Coord a = Center_Coord();
	Coord b = cell->Cell_Coord();
	return(a.Distance_To(b) <= Class->PsychicDetectionRadius << 8);
}

// FUNCTION: GAMEMD 0x004529E0
void BuildingClass::Init_Laser_Fence(void)
{
	if (!ScenarioInit && Class->IsLaserFencePost) {
		if (IsActive && !IsInLimbo) {
			Connect_Laser_Fence(FACING_N);
			Connect_Laser_Fence(FACING_E);
			Connect_Laser_Fence(FACING_S);
			Connect_Laser_Fence(FACING_W);
			Toggle_Laser_Fence_Post(true);
		}
	}
}

// FUNCTION: GAMEMD 0x00452A40
void BuildingClass::Update_Laser_Fence_Connections(int explode)
{
	if (!ScenarioInit && Class->IsLaserFencePost) {
		bool do_explode = false;
		if (explode == (int)true) {
			do_explode = true;
		}
		FacingType dir = FACING_N;
		Cell origin = Center_Coord().As_Cell();

		static int _facing_bits[] = { 1 << 0, 1 << 1, 1 << 2, 1 << 3 };

		for (int i = 0; i < ARRAY_SIZE(_facing_bits); i++) {
			Cell adjacent = Adjacent_Cell(origin, dir);
			BuildingClass * building = Map[adjacent].Cell_Building();
			if (building != NULL && building->Class->IsLaserFence) {
				if (Owner() == building->Owner()) {
					if ((building->PrimaryFacing.Current().As_Axis()) == (dir & 3)) {
						LaserFenceFrame |= _facing_bits[i];
						Disconnect_Laser_Fence(dir, do_explode);
					}
				}
			}
			dir = Facing_Add(dir, FACING_90);
		}
	}
}

// FUNCTION: GAMEMD 0x00452BB0
BuildingClass * BuildingClass::Find_Laser_Fence_Post(FacingType dir, bool connected, int xrange)
{
	int range = 1;

	if (xrange < 0) {
		if (Class->IsLaserFencePost) {
			range = Class->ThreatRange >> 8;
			range = MAX(range, 1);
		}
	} else {
		range = xrange;
	}

	unsigned face = (unsigned)dir % 4;
	Cell working_cell = Center_Coord().As_Cell();
	BuildingClass * result = NULL;

	for (int i = 0; i < range; i++) {
		working_cell = Adjacent_Cell(working_cell, dir);
		BuildingClass * building = Map[working_cell].Cell_Building();

		if (building) {
			if (building->Class->IsLaserFencePost) {
				if (Owner() == building->Owner()) {
					result = building;
				}
			}

			if (!connected) break;
			if (!building->Class->IsLaserFence)	break;
			if (Owner() != building->Owner()) break;
			if ((building->PrimaryFacing.Current().As_Axis()) != (int)face) break;
		}
	}

	return(result);
}

// FUNCTION: GAMEMD 0x00452D40
void BuildingClass::Init_All_Laser_Fences(void)
{
	for (int i = 0; i < Buildings.Count(); i++) {
		BuildingClass * building = Buildings[i];
		if (building != NULL && building->Class->IsLaserFencePost) {
			building->Init_Laser_Fence();
		}
	}
}

// FUNCTION: GAMEMD 0x00453240
void BuildingClass::Unlimbo_Laser_Fence_Helper(Cell const & cell)
{
	CellClass * cptr = &Map[cell];
	if (cptr != NULL) {
		BuildingClass * fence = cptr->Cell_Building();
		if (fence != NULL && fence->Class->IsLaserFence) {
			int facing = fence->PrimaryFacing.Current().As_Axis();
			BuildingClass * building = NULL;
			Cell old_cell = cell;

			Cell working_cell = Adjacent_Cell(old_cell, (FacingType)facing);
			while (working_cell.X != old_cell.X || working_cell.Y != old_cell.Y) {
				building = Map[working_cell].Cell_Building();
				if (building == NULL) {
					break;
				}
				if (building->Class->IsLaserFencePost) {
					building->Disconnect_Laser_Fence(FacingType(facing | FACING_180), false);
					break;
				}
				if (!building->Class->IsLaserFence) {
					building = NULL;
					break;
				}
				old_cell = working_cell;
				working_cell = Adjacent_Cell(old_cell, (FacingType)facing);
			}
			if (building == NULL) {
				fence->Delete_Me();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00453370
void BuildingClass::Init_Laser_Fence_Frame(void)
{
	if (!ScenarioInit && Class->IsLaserFencePost) {
		LaserFenceFrame = 0;
	}
}

inline Cell Next_Cell(Cell const & cell, Cell const & adj) { return cell + adj; }

// FUNCTION: GAMEMD 0x004533A0
void BuildingClass::Toggle_Laser_Fence_Post(bool force)
{
	if (Class->IsLaserFencePost && IsActive && !IsInLimbo) {

		int range = Class->ThreatRange >> 8;
		range = MAX(range, 1);

		bool on = false;
		if (IsPowerOnline() && IsPoweredOn && Mission != MISSION_DECONSTRUCTION && Mission != MISSION_CONSTRUCTION) {
			on = true;
		}

		FacingType dir = FACING_N;
		for (int i = 0; i < FACING_COUNT / 2; i++, dir = Facing_Add(dir, FACING_90)) {

			Cell working_cell = Adjacent_Cell(Center_Coord().As_Cell(), dir);
			BuildingClass * building = Map[working_cell].Cell_Building();
			int face = dir & 3;

			if (building == NULL) continue;
			if (!building->Class->IsLaserFence) continue;
			if (Owner() != building->Owner()) continue;
			if ((building->PrimaryFacing.Current().As_Axis()) != face) continue;

			BuildingClass * first = building;
			BuildingClass * last = building;

			if (!force) {
				if (on) {
					if (building->LaserFenceFrame < 8) continue;
				} else {
					if (building->LaserFenceFrame >= 8) continue;
				}
			}

			BuildingClass * post = NULL;
			if (on) {
				post = Find_Laser_Fence_Post(dir, true, -1);
			}

			int first_frame = 12;
			int mid_frame = 12;
			int last_frame = 12;
			if (face == 2) {
				first_frame = 8;
				mid_frame = 8;
				last_frame = 8;
			}

			bool post_on = false;
			if (on && post != NULL && post->IsPowerOnline() && post->IsPoweredOn
					&& post->Mission != MISSION_DECONSTRUCTION && post->Mission != MISSION_CONSTRUCTION) {

				post_on = true;
				switch (dir) {
					case FACING_N:
						first_frame = 5;
						mid_frame = 4;
						last_frame = 6;
						break;

					case FACING_S:
						first_frame = 6;
						mid_frame = 4;
						last_frame = 5;
						break;

					case FACING_E:
						first_frame = 1;
						mid_frame = 0;
						last_frame = 2;
						break;

					case FACING_W:
						first_frame = 2;
						mid_frame = 0;
						last_frame = 1;
						break;

					default:
						break;
				}
			}

			int index = 0;
			while (building != NULL) {
				if (index >= range) break;
				if (!building->Class->IsLaserFence) break;
				if (Owner() != building->Owner()) break;
				if ((building->PrimaryFacing.Current().As_Axis()) != face) break;

				last = building;
				if (building == first) {
					building->LaserFenceFrame = first_frame;
				} else {
					building->LaserFenceFrame = mid_frame;
				}
				building->IsToDisplay = true;

				if (post_on) {
					ObjectClass * occupier = Map[working_cell].Cell_Occupier();
					while (occupier != NULL) {
						RTTIType rtti = occupier->RTTI;
						if (rtti > RTTI_NONE && (rtti <= RTTI_AIRCRAFT || rtti == RTTI_INFANTRY)) {
							ObjectClass * next = occupier->Next;
							if (occupier->Strength > 0) {
								occupier->Take_Damage(occupier->Strength, 0, Rule->C4Warhead, building, true, true);
							}
							occupier = next;
						} else {
							occupier = occupier->Next;
						}
					}
				}

				index++;
				working_cell = Next_Cell(working_cell, AdjacentCell[(int)dir & 0x07]);
				building = Map[working_cell].Cell_Building();
			}

			if (post_on && last != NULL) {
				if (first == last) {
					if (face == 2) {
						last->LaserFenceFrame = 3;
					} else {
						last->LaserFenceFrame = 7;
					}
				} else {
					last->LaserFenceFrame = last_frame;
				}
				last->IsToDisplay = true;
			}
		}
	}
}


// FUNCTION: GAMEMD 0x00453A70
Coord BuildingClass::Turret_Coord(int which) const
{
	if (!Class->CanBeOccupied || GetOccupantCount() <= 0) {
		if (Class->PrimaryFirePixelOffset != Point2D(0xFFFF, 0xFFFF)) {
			Coord pt(TacticalMap->Pixel_To_Lepton(Class->PrimaryFirePixelOffset), 0);
			return(Render_Coord() + pt);
		}
	} else {
		Coord pt(TacticalMap->Pixel_To_Lepton(Class->MuzzleFlash[FiringOccupantIndex]), 0);
		return(pt + Render_Coord());
	}
	if (Class->IsBarrelAnimAVoxel) {
		return(Voxel_Fire_Coord(which, false));
	}

	Coord coord = BASECLASS::Turret_Coord(which);
	if (Class->IsTurretAnimAVoxel) {
		Point2D pt(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y);
		coord += TacticalMap->Pixel_To_Lepton(pt);
	}
	return(coord);
}


// FUNCTION: GAMEMD 0x00453BF0
Coord BuildingClass::Voxel_Fire_Coord(int which, bool just_fired) const
{
	WeaponTypeClass *weapon = Get_Class_Weapon_Data(which)->Weapon;
	int burst;
	if (just_fired) {
		if (BurstIndex == 0) {
			burst = weapon->Burst - 1;
		} else {
			burst = BurstIndex - 1;
		}
	} else {
		burst = BurstIndex;
	}

	int yoffset;
	if (burst == 0) {
		yoffset = +Class->VoxelBarrelOffsetToBarrelEnd.Y;
	}
	else if (burst == 1) {
		yoffset = -Class->VoxelBarrelOffsetToBarrelEnd.Y;
	} else {
		yoffset = 0;
	}
	Vector3 vec(Class->VoxelBarrelOffsetToBarrelEnd.X, yoffset, Class->VoxelBarrelOffsetToBarrelEnd.Z);

	Vector3 v = Get_Barrel_Matrix() * vec;

	Coord coord = Coord(v.X, -v.Y, v.Z);

	coord = Render_Coord() + coord;

	Point2D lpt(Class->AnimData[BANIM_TURRET].Location.X, Class->AnimData[BANIM_TURRET].Location.Y);
	coord += TacticalMap->Pixel_To_Lepton(lpt);

	return(coord);
}

// FUNCTION: GAMEMD 0x00453D60
void BuildingClass::Set_Occupy_Bit(Coord const & coord)
{
	Map[coord.As_Cell()].Flag.Occupy.Building = true;
}

// FUNCTION: GAMEMD 0x00453DC0
void BuildingClass::Clear_Occupy_Bit(Coord const & coord)
{
	Map[coord.As_Cell()].Flag.Occupy.Building = false;
}

// FUNCTION: GAMEMD 0x00453E20
HRESULT STDMETHODCALLTYPE BuildingClass::Load(IStream * stream)
{
	TargetTracker.Remove_Index(Fetch_ID());
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		int index;
		int count;
		HRESULT hr;

		new (this) BuildingClass(NoInitClass());

		TargetTracker.Add_Index(Fetch_ID(), this);

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&Factory);
		Swizzle_Pointer(&WhomToRepay);
		Swizzle_Pointer(&AnimToTrack);
		Swizzle_Pointer(&PsiWarnAnim);
		Swizzle_Pointer(&BuildingLight);
		Audio7.FUN_00405BE0();
		Audio8.FUN_00405BE0();
		Swizzle_Pointer(&SecretProduction);

		hr = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(hr);
		}

		for (index = 0; index < count; index++) {
			InfantryClass * obj;
			hr = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(hr)) {
				return(hr);
			}
			Overpowerers.Add(obj);
		}

		for (index = 0; index < count; index++) {
			Swizzle_Pointer(&Overpowerers[index]);
		}

		hr = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(hr);
		}

		for (index = 0; index < count; index++) {
			InfantryClass * obj;
			hr = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(hr)) {
				return(hr);
			}
			Occupants.Add(obj);
		}

		for (index = 0; index < count; index++) {
			Swizzle_Pointer(&Occupants[index]);
		}

		for (index = 0; index < BANIM_COUNT; index++) {
			Swizzle_Pointer(&Anims[index]);
		}

		for (index = 0; index < ARRAY_SIZE(Upgrades); index++) {
			Swizzle_Pointer(&Upgrades[index]);
		}

		for (index = 0; index < ARRAY_SIZE(DamageFireAnims); index++) {
			Swizzle_Pointer(&DamageFireAnims[index]);
		}

		LightSource = NULL;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00454190
HRESULT STDMETHODCALLTYPE BuildingClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;
		HRESULT hr;
		int count = Overpowerers.Count();

		hr = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(hr);
		}

		for (index = 0; index < count; index++) {
			hr = stream->Write(&Overpowerers[index], sizeof(Overpowerers[index]), NULL);
			if (FAILED(hr)) {
				return(hr);
			}
		}

		count = Occupants.Count();

		hr = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(hr);
		}

		for (index = 0; index < count; index++) {
			hr = stream->Write(&Occupants[index], sizeof(Occupants[index]), NULL);
			if (FAILED(hr)) {
				return(hr);
			}
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00454250
bool BuildingClass::Ready_To_Next_Mission(void) const
{
	if (IsReadyToCommence) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00454260
void BuildingClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc((int)CountDown);
	crc((int)CashProductionTimer);
	crc(BState);
	crc(QueueBState);
	crc(WhoLastHurtMe);
	if (WhomToRepay != NULL) {
		crc(WhomToRepay->Fetch_ID());
	}
	crc(LastStrength);
	if (AnimToTrack != NULL) {
		crc(AnimToTrack->Fetch_ID());
	}
	crc((int)PlacementDelay);
	crc(TurretIndex);
	crc(LaserFenceFrame);
	crc(FirestormWallFrame);
	crc(IsOn);
	crc(IsPoweredOn);
	crc(IsToRebuild);
	crc(IsToRepair);
	crc(IsAllowedToSell);
	crc(IsReadyToCommence);
	crc(IsRepairing);
	crc(IsWrenchVisible);
	crc(IsGoingToBlow);
	crc(IsSurvivorless);
	crc(IsCharging);
	crc(IsCharged);
	crc(IsCaptured);
	crc(ActuallyPlacedOnMap);
	crc(HasEngineer);
	crc(HasOpened);
	crc(IsDamagedAnims);
	crc(Brightness);
	crc(UpgradeLevel);
}

// FUNCTION: GAMEMD 0x004544A0
VisualType BuildingClass::Visual_Character(bool raw, HouseClass const * house) const
{
	if (TranslucencyLevel != 0) {
		if (TranslucencyLevel > 10) {
			if (raw) {
				if (house != NULL) {
					if (Map[PositionCoord.As_Cell()].Is_Sensed(house->HeapID)) {
						return(VISUAL_SHADOWY);
					}
				}
			} else {
				if (IsOwnedByPlayer || Is_Sensed_By_Player() || !MainWindow || (Session.Type != GAME_NORMAL && House != NULL && PlayerPtr != NULL && PlayerPtr->Is_Ally(House) && House->Is_Ally(PlayerPtr))) {
					return(VISUAL_SHADOWY);
				}
			}
			return(VISUAL_HIDDEN);
		}

		if (TranslucencyLevel > 5) {
			return(VISUAL_DARKEN);
		}

		return(VISUAL_INDISTINCT);
	}

	return(BASECLASS::Visual_Character(raw, house));
}

// FUNCTION: GAMEMD 0x004545D0
void BuildingClass::FUN_004545D0(void)
{
	DestroyNthAnim(BANIM_SUPER_LOW_POWER);
	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Class->AnimData[i].Powered) {
			if (Anims[i] != NULL) {
				Anims[i]->Enable();
			}
		} else if (Class->AnimData[i].PoweredLight) {
			if (Anims[i] == NULL && ActuallyPlacedOnMap) {
				if (!Class->IsAnimDelayedFire || i != BANIM_SPECIAL_ONE) {
					Replay_Anim(this, (BAnimType)i, HealthRatio <= Rule->ConditionYellow);
				}
			}
		} else if (Class->AnimData[i].PoweredEffect) {
			if (AnimStates[i]) {
				AnimStates[i] = false;
				Replay_Anim(this, (BAnimType)i, HealthRatio <= Rule->ConditionYellow);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00454730
void BuildingClass::FUN_00454730(void)
{
	DestroyNthAnim(BANIM_LOW_POWER);
	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Class->AnimData[i].PoweredSpecial) {
			Replay_Anim(this, (BAnimType)i, HealthRatio <= Rule->ConditionYellow);
		}
	}
}

// FUNCTION: GAMEMD 0x004547C0
void BuildingClass::FUN_004547C0(void)
{
	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Class->AnimData[i].Powered) {
			if (Anims[i] != NULL) {
				Anims[i]->Disable();
			}
		} else if (Class->AnimData[i].PoweredLight) {
			if (Anims[i] != NULL) {
				DestroyNthAnim((BAnimType)i);
				if (i == BANIM_SPECIAL_ONE) {
					if (Class->IsAnimDelayedFire && Class->AnimData[BANIM_ACTIVE_ONE].Powered) {
						Replay_Anim(this, BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
					}
				}
			}
		} else if (Class->AnimData[i].PoweredEffect) {
			if (Anims[i] != NULL) {
				AnimStates[i] = true;
				DestroyNthAnim((BAnimType)i);
				if (i == BANIM_SUPER_THREE) {
					Replay_Anim(this, BANIM_SUPER_LOW_POWER, HealthRatio <= Rule->ConditionYellow);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00454930
void BuildingClass::FUN_00454930(void)
{
	Replay_Anim(this, BANIM_LOW_POWER, HealthRatio <= Rule->ConditionYellow);
	for (int i = 0; i < BANIM_COUNT; i++) {
		if (Class->AnimData[i].PoweredSpecial) {
			DestroyNthAnim((BAnimType)i);
		}
	}
}

// FUNCTION: GAMEMD 0x004549B0
void BuildingClass::FUN_004549B0(void)
{
	if (IsPowerOnline()) {
		if (Class->PowersUnit != NULL && !RegisteredAsPoweredUnitSource) {
			RegisteredAsPoweredUnitSource = true;
			House->FUN_0050E010(Class->PowersUnit);
		}

		if (Class->IsCloakGenerator && CloakGeneratorState <= 0 && CurrentCloakRadius != Class->CloakRadiusInCells) {
			CloakGeneratorState = 1;
			if (CurrentCloakRadius == Class->CloakRadiusInCells) {
				CurrentCloakRadius = 0;
			}
			IsToDisplay = true;
		}

		if (Class->GapGenerator && !GeneratingGap) {
			CreateGap();
		}

		if (Class->IsPowered && Class->Drain > 0) {
			FUN_004545D0();
		}

		if (Class->PoweredSpecial) {
			DestroyNthAnim(BANIM_LOW_POWER);
			for (int i = 0; i < BANIM_COUNT; i++) {
				if (Class->AnimData[i].PoweredSpecial) {
					Replay_Anim(this, (BAnimType)i, HealthRatio <= Rule->ConditionYellow);
				}
			}
		}
	} else {
		if (Class->PowersUnit != NULL && RegisteredAsPoweredUnitSource) {
			RegisteredAsPoweredUnitSource = false;
			House->LostPoweredCenter(Class->PowersUnit);
		}

		if (CaptureManager != NULL) {
			CaptureManager->FreeAll();
		}

		if (LocomotorTarget != NULL) {
			ReleaseLocomotor(true);
		}

		if (Class->IsCloakGenerator && CloakGeneratorState >= 0 && CurrentCloakRadius != 0) {
			CloakGeneratorState = -1;
			IsToDisplay = true;
		}

		if (Class->GapGenerator && GeneratingGap) {
			DestroyGap();
			if (GapSuperCharged) {
				GapSuperCharged = false;
				HasExtraPowerDrain = false;
				House->RecalcPower = true;
				GapRadius = Class->GapRadiusInCells;
				CreateGap();
			}
		}

		if (Class->IsPowered && Class->Drain > 0) {
			FUN_004547C0();
		}

		if (Class->PoweredSpecial) {
			HouseClass * house = House;
			if ((int)house->PowerBlackoutTimer != 0 || house->IsBeingDrained) {
				Replay_Anim(this, BANIM_LOW_POWER, HealthRatio <= Rule->ConditionYellow);
				for (int i = 0; i < BANIM_COUNT; i++) {
					if (Class->AnimData[i].PoweredSpecial) {
						DestroyNthAnim((BAnimType)i);
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00454CE0
void __fastcall FUN_00454CE0(HouseClass * house)
{
}

// FUNCTION: GAMEMD 0x00454CF0
void BuildingClass::Enable_Cloak_Generator(void)
{
	if (Class->IsCloakGenerator) {
		CloakGeneratorState = 1;
		if (CurrentCloakRadius == Class->CloakRadiusInCells) {
			CurrentCloakRadius = 0;
		}
		IsToDisplay = true;
	}
}

// FUNCTION: GAMEMD 0x00454D30
void BuildingClass::Disable_Cloak_Generator(void)
{
	if (Class->IsCloakGenerator) {
		CloakGeneratorState = -1;
		if (CurrentCloakRadius == 0) {
			CurrentCloakRadius = Class->CloakRadiusInCells;
		}
		IsToDisplay = true;
	}
}

// FUNCTION: GAMEMD 0x00454D70
void __fastcall Cloak_Cell_Occupiers(CellClass * cellptr)
{
	ObjectClass * occupier = cellptr->Cell_Occupier();
	while (occupier != NULL) {
		RTTIType rtti = occupier->RTTI;
		if (rtti == RTTI_UNIT || rtti == RTTI_INFANTRY || rtti == RTTI_AIRCRAFT) {
			((FootClass *)occupier)->Sensed();
		}
		occupier = occupier->Next;
	}
}

// FUNCTION: GAMEMD 0x004555D0
bool BuildingClass::IsPowerOnline(void) const
{
	if (!IsOn && Overpowerers.Count() < 2) goto off;
	if ((int)EMPLockRemaining > 0) goto off;
	if (Strength == 0) goto off;
	if (Class->IsPowered && Class->Drain > 0 && House->Power_Fraction() < 1.0 && Overpowerers.Count() < 2) goto off;
	if (Class->PoweredSpecial) {
		HouseClass * house = House;
		if ((int)house->PowerBlackoutTimer != 0 || house->IsBeingDrained) goto off;
	}
	if (Class->NeedsEngineer && !HasEngineer) goto off;
	if (Get_Mission() == MISSION_CONSTRUCTION) goto off;
	if (Get_Mission() != MISSION_DECONSTRUCTION) return true;
off:
	return false;
}

// FUNCTION: GAMEMD 0x004556D0
void BuildingClass::SensorArrayDeactivate(Cell cell)
{
	int radius = Class->CloakRadiusInCells;
	int dist = radius * radius;
	HousesType houseid = House->HeapID;
	if (cell == CELL_NONE) {
		cell = Center_Coord().As_Cell();
	}
	for (int y = -radius; y < radius; y++) {
		for (int x = -radius; x < radius; x++) {
			Cell newcell = Cell(cell.X + x, cell.Y + y);
			if (x * x + y * y < dist) {
				CellClass * cellptr = &Map[newcell];
				cellptr->Unsensed_By(houseid);
				Cloak_Cell_Occupiers(cellptr);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00455820
void BuildingClass::FUN_00455820(Cell cell)
{
	if (IsPowerOnline()) {
		int radius = Class->SensorsSight;
		int dist = radius * radius;
		HousesType houseid = House->HeapID;
		if (cell == CELL_NONE) {
			cell = Center_Coord().As_Cell();
		}
		for (int y = -radius; y < radius; y++) {
			for (int x = -radius; x < radius; x++) {
				Cell newcell = Cell(cell.X + x, cell.Y + y);
				if (x * x + y * y < dist) {
					CellClass * cellptr = &Map[newcell];
					cellptr->Sensed_By(houseid);
					Cloak_Cell_Occupiers(cellptr);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00455980
void BuildingClass::DisguiseDetectorDeactivate(Cell cell)
{
	int radius = Class->DetectDisguiseRange;
	int dist = radius * radius;
	HousesType houseid = House->HeapID;
	if (cell == CELL_NONE) {
		cell = Center_Coord().As_Cell();
	}
	for (int y = -radius; y < radius; y++) {
		for (int x = -radius; x < radius; x++) {
			Cell newcell = Cell(cell.X + x, cell.Y + y);
			if (x * x + y * y < dist) {
				CellClass * cellptr = &Map[newcell];
				cellptr->Disguise_Unsensed_By(houseid);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00455A80
void BuildingClass::SensorArrayActivate(Cell cell)
{
	if (IsPowerOnline()) {
		int radius = Class->DetectDisguiseRange;
		int dist = radius * radius;
		HousesType houseid = House->HeapID;
		if (cell == CELL_NONE) {
			cell = Center_Coord().As_Cell();
		}
		for (int y = -radius; y < radius; y++) {
			for (int x = -radius; x < radius; x++) {
				Cell newcell = Cell(cell.X + x, cell.Y + y);
				if (x * x + y * y < dist) {
					CellClass * cellptr = &Map[newcell];
				cellptr->Disguise_Sensed_By(houseid);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00455B90
int BuildingClass::Get_Firestorm_Wall_Frame(void)
{
	unsigned frame = 0;
	CellClass * cellptr = &Map[Get_Coord()];
	for (int i = FACING_N; i < FACING_COUNT; i += FACING_90) {
		BuildingClass * building = cellptr->Adjacent_Cell((FacingType)i).Cell_Building();
		if (building != NULL && building->Class->IsFirestormWall) {
			if (!building->IsInLimbo && building->IsActive) {
				frame |= 1 << (i >> 1);
			}
		}
	}
	return(frame);
}

// FUNCTION: GAMEMD 0x00455C20
Rect BuildingClass::Get_Render_Rect(void)
{
	Coord render = Render_Coord();
	Point2D tac(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
	Rect rect;

	if (render == LastRenderCoord && tac == LastRenderOffset) {
		rect = LastRenderRect;
	} else {
		Point2D pixel;
		TacticalMap->Coord_To_Pixel(render, pixel);
		rect = Class->Get_Draw_Rect() + pixel;
		LastRenderCoord = render;
		LastRenderOffset = tac;
		LastRenderRect = rect;
	}

	return(rect);
}

// FUNCTION: GAMEMD 0x00455D50
void BuildingClass::Assign_Destination(AbstractClass * target, bool immediate)
{
	if (CurrentMission != MISSION_DECONSTRUCTION) {
		if (Is_Move_Override() || Class->IsConstructionYard) {
			ArchiveTarget = target;
		}
		BASECLASS::Assign_Destination(target, immediate);
	}
}

// FUNCTION: GAMEMD 0x00455DA0
bool BuildingClass::Is_Move_Override(void) const
{
	if (Class->ToBuild != RTTI_UNITTYPE && Class->ToBuild != RTTI_INFANTRYTYPE && !Class->Cloning && !Class->IsCanUnitRepair) {
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00455DD0
BuildingClass * Find_Unit_Repair_Facility(HouseClass * house, TechnoClass * obj)
{
	BuildingClass * best_bld = NULL;
	int best_dist = 0x7FFFFFFF;

	static int max_dist = int(CELL_LEPTON_DIAG * 20.0);

	int count = house->UnitRepairStations.Count();
	for (int i = 0; i < count; i++) {
		BuildingClass * bld = house->UnitRepairStations[i];
		if (obj->Transmit_Message(RADIO_CAN_LOAD, bld) != RADIO_STATIC) {
			int dist = (bld->Get_Coord() - obj->Get_Coord()).Length();
			if (dist < best_dist && dist < max_dist) {
				best_bld = bld;
				best_dist = dist;
			}
		}
	}
	return(best_bld);
}

// FUNCTION: GAMEMD 0x00455F10
void BuildingClass::Reserve_Base_Area(bool skip_inner_cells)
{
	int spacing = Rule->AIBaseSpacing;
	int width = 2 * spacing + Class->Width();
	int height = 2 * spacing + Class->Height();

	unsigned owner = 1 << House->HeapID;
	Cell top_left = PositionCoord.As_Cell() - Cell(spacing, spacing);

	for (int x = top_left.X; x < top_left.X + width; x++) {
		for (int y = top_left.Y; y < top_left.Y + height; y++) {
			Map[Cell(x, y)].OccupiedBy |= owner;
		}
	}

	Rect & base_rect = House->Base.BaseAreaRect;
	if (base_rect.X == 0) {
		base_rect.X = top_left.X;
	}
	if (base_rect.Y == 0) {
		base_rect.Y = top_left.Y;
	}
	if (top_left.X < base_rect.X) {
		int old_x = base_rect.X;
		base_rect.X = top_left.X;
		base_rect.Width += old_x - top_left.X;
	}
	if (top_left.X + width > base_rect.Width + base_rect.X) {
		base_rect.Width = top_left.X - base_rect.X + width;
	}
	if (top_left.Y < base_rect.Y) {
		int old_y = base_rect.Y;
		base_rect.Y = top_left.Y;
		base_rect.Height += old_y - top_left.Y;
	}
	if (top_left.Y + height > base_rect.Height + base_rect.Y) {
		base_rect.Height = top_left.Y - base_rect.Y + height;
	}

	if (!skip_inner_cells) {
		for (int x = top_left.X - 1; x < top_left.X + width + 2; x++) {
			for (int y = top_left.Y - 1; y < top_left.Y + height + 2; y++) {
				Cell cell = Cell(x, y);
				int mask = Map[cell].Occupation_Mask(House->HeapID);
				if (mask == (1 << FACING_COUNT) - 1) {
					House->Base.InnerCells.Delete(cell);
				} else if (mask > 0) {
					if (House->Base.InnerCells.ID(cell) == -1) {
						House->Base.InnerCells.Add(cell);
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004561F0
void BuildingClass::Release_Base_Area(void)
{
	int spacing = Rule->AIBaseSpacing;
	int width = 2 * spacing + Class->Width();
	int height = 2 * spacing + Class->Height();

	unsigned owner = 1 << House->HeapID;
	Cell top_left = PositionCoord.As_Cell() - Cell(spacing, spacing);

	int x, y;

	for (x = top_left.X; x < top_left.X + width; x++) {
		for (y = top_left.Y; y < top_left.Y + height; y++) {
			Map[Cell(x, y)].OccupiedBy &= ~owner;
		}
	}

	for (x = top_left.X - spacing; x < top_left.X + width + spacing * 2; x++) {
		for (y = top_left.Y - spacing; y < top_left.Y + height + spacing * 2; y++) {
			BuildingClass * building = Map[Cell(x, y)].Cell_Building();
			if (building != NULL && building != this) {
				if (building->Class->UndeploysInto == NULL || !building->Class->ResourceGatherer) {
					if (!building->IsStrange()) {
						building->Reserve_Base_Area(true);
					}
				}
			}
		}
	}

	for (x = top_left.X - 1; x < top_left.X + width + 2; x++) {
		for (y = top_left.Y - 1; y < top_left.Y + height + 2; y++) {
			Cell cell = Cell(x, y);
			int mask = Map[cell].Occupation_Mask(House->HeapID);
			if (mask == (1 << FACING_COUNT) - 1) {
				House->Base.InnerCells.Delete(cell);
			} else if (mask > 0) {
				if (House->Base.InnerCells.ID(cell) == -1) {
					House->Base.InnerCells.Add(cell);
				}
			} else {
				House->Base.InnerCells.Delete(cell);
				Map[cell].OccupiedBy &= ~owner;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00456580
void BuildingClass::Radar_Track(void)
{
	const FOUNDATION_LIST & list = Map.Get_Foundation(Class);
	for (int i = 0; i < list.Count(); i++) {
		Point2D pt = list[i] + RadarPos;
		Map.Radar_Track(this, pt);
	}
	IsRadarTracked = true;
}

// FUNCTION: GAMEMD 0x004565E0
void BuildingClass::Radar_Untrack(void)
{
	const FOUNDATION_LIST & list = Map.Get_Foundation(Class);
	for (int i = 0; i < list.Count(); i++) {
		Point2D pt = list[i] + RadarPos;
		Map.Radar_Untrack(this, pt);
	}
	IsRadarTracked = false;
}

// FUNCTION: GAMEMD 0x00456640
void BuildingClass::Plot_On_Radar(void) const
{
	const FOUNDATION_LIST & list = Map.Get_Foundation(Class);
	for (int i = 0; i < list.Count(); i++) {
		Point2D pt = list[i] + RadarPos;
		Map.Radar_Pixel(pt);
	}
}

// FUNCTION: GAMEMD 0x004566B0
int BuildingClass::GetRadialIndicatorRange(void) const
{
	int range = 0;
	if (Class->PsychicDetectionRadius > 0) {
		range = Class->PsychicDetectionRadius;
	} else if (Class->GapGenerator) {
		range = GapSuperCharged ? Class->SuperGapRadiusInCells : Class->GapRadiusInCells;
	} else if (Class->IsSensorArray || Class->IsCloakGenerator) {
		range = Class->CloakRadiusInCells;
	} else {
		WeaponDataStruct const * weapon = TechnoClass::GetTurretWeapon();
		if (FUN_0070E240(weapon) && weapon->Weapon->Range > 0) {
			range = weapon->Weapon->Range / 256;
		}
	}
	return(range);
}

// FUNCTION: GAMEMD 0x00456750
void BuildingClass::Draw_Radial_Indicator(DWORD which)
{
	int range = GetRadialIndicatorRange();
	if (range > 0 && IsOn && House->Is_Player_Control()) {
		if (Class->ConcentricRadialIndicator) {
			static RGBClass white(255, 255, 255);
			RGBClass color = House->RemapColorRGB;
			unsigned pulse = (timeGetTime() >> 1) & 0x3ff;
			if (!(pulse & 0x200)) {
				if (!(pulse & 0x100)) {
					color.Adjust(~pulse & 0xff, white);
				}
				DrawRadialIndicator(false, false, Center_Coord(), color, (float)range, false, true);
			}
			int radius = (int)(((double)range + 0.5) / fastmath::sqrt(2.0) * 60.0);
			unsigned size = (radius * pulse) >> 10;
			if (size > 32) {
				DrawRadialIndicator(false, false, Center_Coord(), House->RemapColorRGB, (float)size, true, true);
			}
		} else {
			DrawRadialIndicator(*(bool *)&which, true, Center_Coord(), House->RemapColorRGB, (float)range, false, true);
		}
	}
}

// SYNTHETIC: GAMEMD 0x00456970 SYMBOL
// _$E254

namespace Unsorted { extern int CurrentFrame; }

// FUNCTION: GAMEMD 0x00456980
void __fastcall DrawRadialIndicator(bool draw_line, bool adjust_color, Coord center, RGBClass color, float radius_in, bool in_pixels, bool whole)
{
	if (whole) {
		radius_in = (float)(int)radius_in;
	}
	int radius;
	if (in_pixels) {
		radius = (int)radius_in;
	} else {
		radius = (int)(((double)radius_in + 0.5) / fastmath::sqrt(2.0) * 60.0);
	}

	Point2D point;
	TacticalMap->Coord_To_Pixel(center, point);
	point += DSurface::ViewBounds.TopLeft;

	Rect bounds = Rect(point.X - radius, point.Y - radius / 2, radius * 2, radius);
	Rect clipped = Intersect(bounds, DSurface::ViewBounds);
	if (clipped.Width > 0 && clipped.Height > 0) {
		RGBClass black;
		RGBClass ring = color;
		if (adjust_color) {
			ring.Adjust(50, black);
		}
		DSurface::Temp->Draw_Ellipse(point, radius, radius / 2, DSurface::ViewBounds, DSurface::Build_Hicolor_Pixel(ring));
		DSurface::Temp->Draw_Ellipse(point, radius - 1, radius / 2 - 1, DSurface::ViewBounds, DSurface::Build_Hicolor_Pixel(ring));

		if (draw_line) {
			static double intensities[4] = {0.05, 0.2, 0.4, 1.0};

			double radius_x = radius;
			double radius_y = radius / 2;

			for (int i = 0; i < 4; i++) {
				double angle = (double)(Unsorted::CurrentFrame + i) * 0.005;
				angle = angle - (double)(int)(angle * 0.15915494309189535) * 6.283185307179586;
				Point2D end;

				if (fabs(angle - 1.5707963267948966) < 0.001) {
					end = point + Point2D(0, -radius_y);
				} else if (fabs(angle - 4.71238898038469) < 0.001) {
					end = point + Point2D(0, radius_y);
				} else {
					double slope = fastmath::tan(angle);
					double radius_y_sq = radius_y * radius_y;
					double radius_x_sq = radius_x * radius_x;
					double xdist = fastmath::sqrt(1.0 / (slope * slope / radius_y_sq + 1.0 / radius_x_sq));
					double ydist = fastmath::sqrt((1.0 - xdist * xdist / radius_x_sq) * radius_y_sq);
					double xoffset = angle > 1.5707963267948966 && angle < 4.71238898038469 ? -xdist : xdist;
					if (angle < 3.141592653589793) {
						ydist = -ydist;
					}
					end = point + Point2D(xoffset, ydist);
				}

				DSurface::Temp->Draw_Depth_Antialiased_Line(DSurface::ViewBounds, point - DSurface::ViewBounds.TopLeft, end - DSurface::ViewBounds.TopLeft, color, -500, -500, 0, 1, 1, 1, (float)intensities[i]);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00456E00
void BuildingClass::Clicked_As_Target(int count)
{
	if (count) {
		IsToDisplay = true;
	}

	BASECLASS::Clicked_As_Target(count);

	Update_Anim_Appearance();
}

// FUNCTION: GAMEMD 0x00456F80
int BuildingClass::Apparent_Brightness(int brightness) const
{
	if ((Flashing.FlashCount & 2) == 2) {
		return(brightness > 1500 ? brightness - 500 : brightness + 500);
	}
	return(brightness);
}

// FUNCTION: GAMEMD 0x00456FB0
int BuildingClass::Tinted_Brightness(int cellbrightness)
{
	int brightness = Apparent_Brightness(cellbrightness);
	if (IsIronCurtained() || (Airstrike != NULL && Airstrike->Target == Building_Of(this))) {
		brightness = GetEffectTintIntensity(brightness);
	}
	return(brightness);
}

// FUNCTION: GAMEMD 0x00457020
bool BuildingClass::Is_Radar_Visible(DetectedType & detected) const
{
	if (!Class->IsInvisible && !IsInLimbo) {
		if (House->Is_Player_Control()) {
			return(IsDiscoveredByPlayer ? true : false);
		}
		if (!Map.In_Local_Radar(Center_Coord().As_Cell(), true)) {
			return(false);
		}

		int height = Class->Height() * CELL_LEPTON_H - CELL_LEPTON;
		int width = Class->Width() * CELL_LEPTON_W - CELL_LEPTON;
		bool shrouded = Map.Is_Shrouded(PositionCoord) && Map.Is_Shrouded(PositionCoord + Coord(width, height)) && MainWindow;

		if (Cloak != CLOAKED && TranslucencyLevel != 15 && !IsFogged && !shrouded) {
			return(true);
		}
		if (!Is_Sensed_By_Player()) {
			return(false);
		}
		if (!PlayerPtr->Is_Ally(House) && !IsFogged && !shrouded) {
			detected = DETECTED_CLOAKED;
		}
		return(true);
	}
	return(false);
}

extern bool DAT_00884B8E;

static inline bool Is_Tech_Building(BuildingTypeClass const * type)
{
	int count = Rule->BuildTech.Count();
	for (int i = 0; i < count; i++) {
		if (Rule->BuildTech[i] == type) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004571E0
void BuildingClass::Infiltrate(HouseClass * house)
{
	bool announce = false;
	if (House != house) {
		if (House->Is_Player_Control() || house->Is_Player_Control()) {
			if (RadarEventClass::Create(RADAREVENT_BUILDING_INFILTRATED, PositionCell)) {
				announce = true;
			}
		}
		if (Class->IsRadar) {
			House->ReshroudMap();
			if (House->Is_Player_Control() && !House->SpySatActive && announce) {
				VoxClass::Play("EVA_RadarSabotaged", -1, -1);
			}
			if (house->Is_Player_Control() && !House->SpySatActive && announce) {
				VoxClass::Play("EVA_BuildingInfRadarSabotaged", -1, -1);
			}
		} else if (Class->Power > 0) {
			House->CreatePowerOutage(Rule->SpyPowerBlackout);
			if (House->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_PowerSabotaged", -1, -1);
			} else if (house->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_BuildingInfiltrated", -1, -1);
				VoxClass::Play("EVA_EnemyBasePoweredDown", -1, -1);
			}
		} else if (Is_Tech_Building(Class)) {
			if (Class->AIBasePlanningSide == 0) {
				house->Side0TechInfiltrated = true;
			} else if (Class->AIBasePlanningSide == 1) {
				house->Side1TechInfiltrated = true;
			} else {
				house->Side2TechInfiltrated = true;
			}
			house->IsRecalcNeeded = true;
			if (House->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_TechnologyStolen", -1, -1);
			}
			if (house->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_BuildingInfiltrated", -1, -1);
				VoxClass::Play("EVA_NewTechnologyAcquired", -1, -1);
			}
		} else if (Class->SuperWeapon != SUPER_NONE) {
			House->SuperWeapon[Class->SuperWeapon]->Reset();
			if (House->Is_Player_Control() || (house->Is_Player_Control() && announce)) {
				VoxClass::Play("EVA_BuildingInfiltrated", -1, -1);
			}
		} else if (Class->Capacity > 0) {
			int stolen = (int)(House->Available_Money() * Rule->SpyMoneyStealPercent);
			House->Spend_Money(stolen);
			house->Refund_Money(stolen);
			if (House->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_CashStolen", -1, -1);
			}
			if (house->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_BuildingInfCashStolen", -1, -1);
			}
		} else if (Class->ToBuild == RTTI_UNITTYPE) {
			house->WarFactoryInfiltrated = true;
			house->IsRecalcNeeded = true;
			if (house->Is_Player_Control()) {
				DAT_00884B8E = true;
			}
			if (House->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_TechnologyStolen", -1, -1);
			}
			if (house->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_BuildingInfiltrated", -1, -1);
				VoxClass::Play("EVA_NewTechnologyAcquired", -1, -1);
			}
		} else if (Class->ToBuild == RTTI_INFANTRYTYPE) {
			house->BarracksInfiltrated = true;
			house->IsRecalcNeeded = true;
			if (house->Is_Player_Control()) {
				DAT_00884B8E = true;
			}
			if (House->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_TechnologyStolen", -1, -1);
			}
			if (house->Is_Player_Control() && announce) {
				VoxClass::Play("EVA_BuildingInfiltrated", -1, -1);
				VoxClass::Play("EVA_NewTechnologyAcquired", -1, -1);
			}
		}
		Mark(MARK_CHANGE);
	}
}

// FUNCTION: GAMEMD 0x004575B0
void BuildingClass::Sell_All_Upgrades(void)
{
	char level = UpgradeLevel;
	while (level > 0) {
		int refund = Upgrades[level - 1]->GetRefund(House, true);
		House->Refund_Money(refund);
		Remove_Upgrade();
		House->RecalcPower = true;
		House->RecalcRadar = true;
		level = UpgradeLevel;
	}
	Sell_Back(true);
}

// FUNCTION: GAMEMD 0x00457620
bool BuildingClass::IsStrange(void) const
{
	return(Class->IsVehicle());
}

// FUNCTION: GAMEMD 0x00457630
int BuildingClass::FirstActiveSWIdx(void) const
{
	if (Class->SuperWeapon != SUPER_NONE) {
		BuildingTypeClass const * aux = SuperWeapons[Class->SuperWeapon]->Class->AuxBuilding;
		if (aux != NULL && House->ABQuantity.Value(aux->HeapID) == 0) {
			return(SUPER_NONE);
		}
	}
	return(Class->SuperWeapon);
}

// FUNCTION: GAMEMD 0x00457690
int BuildingClass::SecondActiveSWIdx(void) const
{
	if (Class->SuperWeapon2 != SUPER_NONE) {
		BuildingTypeClass const * aux = SuperWeapons[Class->SuperWeapon2]->Class->AuxBuilding;
		if (aux != NULL && House->ABQuantity.Value(aux->HeapID) == 0) {
			return(SUPER_NONE);
		}
	}
	return(Class->SuperWeapon2);
}

// FUNCTION: GAMEMD 0x004576F0
void BuildingClass::GotHijacked(void)
{
	for (int i = 0; i < Infantry.Count(); i++) {
		InfantryClass * infantry = Infantry[i];
		BuildingClass * building = Map[infantry->Destination_Coord()].Cell_Building();
		if (infantry->IsActive && building != NULL && building == this) {
			if (infantry->NavCom == NULL || infantry->NavCom == building) {
				infantry->Scatter((Coord const &)DAT_0089C848, true, true);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00457770
bool BuildingClass::Is_Ready_To_Cloak(void) const
{
	if (BASECLASS::Is_Ready_To_Cloak()) {
		Cell cell = PositionCell;
		int width = Class->Width();
		int height = Class->Height();
		for (int x = -1; x <= width; x++) {
			for (int y = -1; y <= height; y++) {
				TechnoClass * tech = Map[cell + Cell(x, y)].Cell_Techno();
				if (tech == (TechnoClass *)this) {
					tech = Map[cell + Cell(x, y)].Cell_Infantry();
				}
				if (tech != NULL && tech->House != House) {
					if (tech->TClass->IsScanner) {
						return(false);
					}
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004578C0
bool BuildingClass::Should_Uncloak(void) const
{
	if (!BASECLASS::Should_Uncloak()) {
		Cell cell = PositionCell;
		int width = Class->Width();
		int height = Class->Height();
		for (int x = -1; x <= width; x++) {
			for (int y = -1; y <= height; y++) {
				TechnoClass * tech = Map[cell + Cell(x, y)].Cell_Techno();
				if (tech == (TechnoClass *)this) {
					tech = Map[cell + Cell(x, y)].Cell_Infantry();
				}
				if (tech != NULL && tech->House != House) {
					if (tech->TClass->IsScanner) {
						return(true);
					}
				}
			}
		}
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00457A10
bool BuildingClass::Should_Fog(void) const
{
	Cell cell = PositionCell;
	Cell const * occupy = Class->Occupy_List();
	while (*occupy != REFRESH_EOL) {
		if (!Map[*occupy + cell].IsFogged) {
			return(false);
		}
		occupy++;
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00457AA0
void BuildingClass::Make_Fogged(DynamicVectorClass<FoggedObjectClass *> * fogged_objects, CellClass * cellptr, bool fade)
{
	Cell const * occupy = Class->Occupy_List();

	Unselect();
	FoggedObjectClass * fogged_object = new FoggedObjectClass(this, fade);

	Cell cell = PositionCell;
	while (*occupy != REFRESH_EOL) {
		CellClass * cptr = &Map[cell + *occupy];
		if (cellptr != NULL && cptr == cellptr) {
			fogged_objects->Add(fogged_object);
		} else {
			DynamicVectorClass<FoggedObjectClass *> * new_fogged_objects = cptr->FoggedObjects;
			if (new_fogged_objects != NULL) {
				new_fogged_objects->Add(fogged_object);
			} else {
				cptr->FoggedObjects = new DynamicVectorClass<FoggedObjectClass *>;
				cptr->FoggedObjects->Resize(1);
				cptr->FoggedObjects->Set_Growth_Step(1);
				cptr->FoggedObjects->Add(fogged_object);
			}
		}
		occupy++;
	}
}

// FUNCTION: GAMEMD 0x00457C90
int BuildingClass::IronCurtain(int duration, HouseClass * source, bool force_shield)
{
	if (IsGoingToBlow) {
		IsGoingToBlow = false;
		CountDown = 0;
		WhomToRepay = NULL;
	}
	return(TechnoClass::IronCurtain(duration, source, force_shield));
}

// FUNCTION: GAMEMD 0x00457CE0
bool BuildingClass::CanBeOccupiedBy(InfantryClass * infantry) const
{
	if (infantry == NULL || !Class->CanBeOccupied) return(false);
	if (CurrentMission == MISSION_CONSTRUCTION || CurrentMission == MISSION_DECONSTRUCTION) return(false);
	if (!Map.In_Local_Radar(Center_Coord())) return(false);
	if (IsBeingWarpedOut()) return(false);

	if (infantry->Class->Occupier) {
		if (House != infantry->House && !House->Class->IsMultiplayPassive) return(false);
		if (GetOccupantCount() == Class->MaxNumberOccupants) return(false);
		if (Is_Red_HP()) return(false);
		if (infantry->IsMindControlled()) return(false);
	} else {
		if (!infantry->Class->Assaulter) return(false);
		if (House->Is_Ally(infantry)) return(false);
		if (GetOccupantCount() == 0) return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004581F0
int BuildingClass::GetOccupantCount(void) const
{
	return(Occupants.Count());
}

// FUNCTION: GAMEMD 0x00458200
void BuildingClass::FUN_00458200(void)
{
	if (Class->Level == -1) {
		if (Is_Red_HP()) {
			FUN_00457DE0(false, false);
		}
		HouseClass * civilian = NULL;
		int count = Houses.Count();
		SideType side = FUN_006A46D0("Civilian");
		for (int i = 0; i < count; i++) {
			if (Houses[i]->Class->Side == side) {
				civilian = Houses[i];
				break;
			}
		}
		if (GetOccupantCount() == 0 && House != civilian) {
			if (House->Is_Player_Control()) {
				VocClass::PlayGlobal(Rule->BuildingAbandonedSound, 1.0f, 0x2000);
				if (RadarEventClass::Create(RADAREVENT_GARRISON_ABANDONED, Get_Cell())) {
					VoxClass::Play("EVA_StructureAbandoned", -1, -1);
				}
			}
			FUN_00458330();
			Captured(civilian, false);
		}
		if (GetOccupantCount() > 0 && House == civilian) {
			FUN_00458330();
			Captured(Occupants[0]->House, false);
		}
	}
}

// FUNCTION: GAMEMD 0x00458330
void BuildingClass::FUN_00458330(void)
{
	if (Anims[BANIM_IDLE] != NULL) {
		Begin_Anim(BANIM_IDLE, HealthRatio <= Rule->ConditionYellow);
	}
	if (Anims[BANIM_ACTIVE_ONE] != NULL) {
		Begin_Anim(BANIM_ACTIVE_ONE, HealthRatio <= Rule->ConditionYellow);
	}
	if (Anims[BANIM_ACTIVE_TWO] != NULL) {
		Begin_Anim(BANIM_ACTIVE_TWO, HealthRatio <= Rule->ConditionYellow);
	}
	if (Anims[BANIM_ACTIVE_THREE] != NULL) {
		Begin_Anim(BANIM_ACTIVE_THREE, HealthRatio <= Rule->ConditionYellow);
	}
	if (Anims[BANIM_ACTIVE_FOUR] != NULL) {
		Begin_Anim(BANIM_ACTIVE_FOUR, HealthRatio <= Rule->ConditionYellow);
	}
}

// FUNCTION: GAMEMD 0x00458750
void BuildingClass::FUN_00458750(void)
{
	CashProductionTimer = Class->ProduceCashDelay;
	if (!House->Class->IsMultiplayPassive && IsPowerOnline()) {
		int amount = Class->ProduceCashAmount;
		if (amount > 0) {
			House->Refund_Money(amount);
		} else {
			House->Spend_Money(-amount);
		}
	}
}

// FUNCTION: GAMEMD 0x004587D0
bool BuildingClass::FUN_004587D0(ObjectTypeClass const * type)
{
	TechnoClass * object = (TechnoClass *)type->Create_One_Of(House);
	return(Exit_Object(object, DAT_0089C818) ? true : false);
}

// FUNCTION: GAMEMD 0x00458810
Matrix3D BuildingClass::Get_Barrel_Matrix(void) const
{
	Matrix3D matrix;
	matrix.Make_Identity();

	matrix.Translate_X(Class->VoxelBarrelOffsetToBuildingPivotPoint.X);
	matrix.Translate_Y(Class->VoxelBarrelOffsetToBuildingPivotPoint.Y);
	matrix.Translate_Z(Class->VoxelBarrelOffsetToBuildingPivotPoint.Z);

	matrix.Rotate_Z(PrimaryFacing.Current().As_Radian32());

	matrix.Translate_X(Class->VoxelBarrelOffsetToRotatePivotPoint.X);
	matrix.Translate_Y(Class->VoxelBarrelOffsetToRotatePivotPoint.Y);
	matrix.Translate_Z(Class->VoxelBarrelOffsetToRotatePivotPoint.Z);

	matrix.Rotate_Y(-(BarrelPitch.Current().As_Radian32()));

	matrix.Translate_X(Class->VoxelBarrelOffsetToPitchPivotPoint.X);
	matrix.Translate_Y(Class->VoxelBarrelOffsetToPitchPivotPoint.Y);
	matrix.Translate_Z(Class->VoxelBarrelOffsetToPitchPivotPoint.Z);

	float scale = Class->VoxelBarrelScale;
	matrix.Scale_X(scale);
	matrix.Scale_Y(scale);
	matrix.Scale_Z(scale);

	return(matrix);
}

// FUNCTION: GAMEMD 0x004589C0
int BuildingClass::FUN_004589C0(void) const
{
	if ((int)Storage.Get_Total_Amount() == 0) {
		return(0);
	}
	return((int)Storage.Get_Total_Amount() * 4 / Class->Capacity);
}

// FUNCTION: GAMEMD 0x00458A00
bool BuildingClass::FUN_00458A00(CellClass const * cell)
{
	if (cell->Cell_Building() != this) {
		return(false);
	}
	if (Class->NumberImpassableRows == -1) {
		return(true);
	}
	if (Class->Bunker && BunkerLinkedItem != NULL) {
		return(true);
	}
	int x = cell->CellID.X;
	int cx = Get_Cell().X;
	int rows = Class->NumberImpassableRows;
	return(rows + cx > x ? true : false);
}

// FUNCTION: GAMEMD 0x00458A80
bool BuildingClass::FUN_00458A80(Coord const & coord, Dir256 dir)
{
	Cell const * offset = Occupy_List(false);
	Cell origin = coord.As_Cell();
	DynamicVectorClass<TechnoClass *> displaced;
	int a, b, c;

	while (*offset != REFRESH_EOL) {
		Cell cell = *offset++ + origin;
		CellClass * cellptr = &Map[cell];
		if (cellptr->Overlay != OVERLAY_NONE) {
			cellptr->Overlay = OVERLAY_NONE;
			cellptr->OverlayData = 0;
			cellptr->Rubble = NULL;
		}
		ObjectClass * object = cellptr->Cell_Occupier();
		while (object != NULL) {
			if (!(object->AbstractFlags & 1) || object->Fetch_RTTI() == RTTI_BUILDING) {
				for (a = displaced.Count() - 1; a >= 0; a--) {
					TechnoClass * techno = displaced[a];
					Coord loc = techno->PositionCoord;
					if (!techno->Unlimbo(loc, DIR_N)) {
						techno->Delete_Me();
					}
				}
				goto failed;
			}
			displaced.Add((TechnoClass *)object);
			object->Limbo();
			object = cellptr->Cell_Occupier();
		}
	}

	if (!Unlimbo(coord, dir)) {
		Delete_Me();
		for (b = displaced.Count() - 1; b >= 0; b--) {
			TechnoClass * techno = displaced[b];
			Coord loc = techno->PositionCoord;
			if (!techno->Unlimbo(loc, DIR_N)) {
				techno->Delete_Me();
			}
		}
failed:
		displaced.Clear();
		return(false);
	}

	for (c = displaced.Count() - 1; c >= 0; c--) {
		TechnoClass * techno = displaced[c];
		Cell where = techno->Get_Cell();
		Cell spot = Map.Nearby_Location(where, techno->Techno_Type_Class()->Speed);
		if (!techno->Unlimbo(Map[spot].Center_Coord(), DIR_N)) {
			techno->Delete_Me();
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00458DB0
bool BuildingClass::Is_Weapon_Equipped(void) const
{
	if (CanOccupyFire()) {
		return(true);
	}
	return(TechnoClass::Is_Weapon_Equipped());
}

// FUNCTION: GAMEMD 0x00458DD0
bool BuildingClass::CanOccupyFire(void) const
{
	return(Class->CanBeOccupied && Class->CanOccupyFire && GetOccupantCount() > 0);
}

// FUNCTION: GAMEMD 0x00458E00
int BuildingClass::GetOccupyRangeBonus(void) const
{
	if (Class->Width() < Class->Height()) {
		return(Class->Width() / 2);
	}
	return(Class->Height() / 2);
}

static inline DirType Direction_From(AbstractClass const * object, Coord const & coord)
{
	return(::Direction(object->Center_Coord(), coord));
}

// FUNCTION: GAMEMD 0x00458E50
void BuildingClass::UpdateBunker(void)
{
	TechnoClass * unit = BunkerLinkedItem ? BunkerLinkedItem : FUN_0065AD30(0);
	if (unit == NULL) {
		TankBunkerState = 0;
		QueueMission(MISSION_GUARD, false);
		return;
	}
	if (unit->Fetch_RTTI() != RTTI_UNIT) {
		TankBunkerState = 0;
		QueueMission(MISSION_GUARD, false);
		return;
	}

	switch (TankBunkerState) {
		case 0: {
			if (unit->Get_Cell_Ptr()->Cell_Building() != this) {
				break;
			}
			if (((FootClass *)unit)->Locomotion->Is_Moving()) {
				break;
			}
			Cell const * list = Occupy_List(false);
			Cell cell = Get_Cell();
			while (*list != REFRESH_EOL) {
				CellClass * cellptr = &Map[*list + cell];
				TechnoClass * other = cellptr->Cell_Techno(Point2D(128, 128), false, this);
				if (other != NULL && other != unit) {
					other->Scatter(Center_Coord(), true, false);
				}
				list++;
			}
			TankBunkerState = 1;
			break;
		}

		case 1: {
			bool blocked = false;
			Cell const * list = Occupy_List(false);
			Cell cell = Get_Cell();
			while (*list != REFRESH_EOL) {
				CellClass * cellptr = &Map[*list + cell];
				TechnoClass * other = cellptr->Cell_Techno(Point2D(128, 128), false, this);
				if (other != NULL && other != unit) {
					blocked = true;
					if ((other->AbstractFlags & 4) && ((FootClass *)other)->NavCom == NULL) {
						TankBunkerState = 1;
						return;
					}
				}
				list++;
			}
			if (blocked) {
				break;
			}
			DirType dir = Direction_From(this, unit->Center_Coord());
			unit->PrimaryFacing.Set_Desired(dir.Right_180());
			TankBunkerState = 2;
			break;
		}

		case 2: {
			if (unit->PrimaryFacing.Is_Rotating()) {
				break;
			}
			unsigned v = ((unsigned)unit->PrimaryFacing.Current().Raw >> 7) + 1 & 0x1fe;
			int track;
			if (v == 0x40) {
				track = 0x43;
			} else if (v == 0x1c0) {
				track = 0x46;
			} else {
				track = 0x44;
				if (v != 0xc0) {
					track = 0x45;
				}
			}
			Coord center = Center_Coord();
			((FootClass *)unit)->Locomotion->Force_Track(track, center);
			((FootClass *)unit)->Set_Speed(1.0);
			TankBunkerState = 3;
			break;
		}

		case 3:
			if (unit->Get_Cell_Ptr()->Cell_Building() != this) {
				break;
			}
			if (((FootClass *)unit)->Locomotion->Is_Moving()) {
				break;
			}
			unit->PrimaryFacing.Set_Desired(DirType(DIR_S));
			TankBunkerState = 4;
			break;

		case 4:
			if (unit->PrimaryFacing.Is_Rotating()) {
				break;
			}
			Replay_Anim(this, BANIM_SPECIAL_ONE, HealthRatio <= Rule->ConditionYellow);
			Replay_Anim(this, BANIM_SPECIAL_TWO, HealthRatio <= Rule->ConditionYellow);
			TankBunkerState = 5;
			break;

		case 5:
			BunkerLinkedItem = unit;
			unit->BunkerLinkedItem = this;
			unit->Group = -1;
			unit->Unselect();
			TankBunkerState = 6;
			BunkerLinkedItem->QueueMission(MISSION_GUARD, true);
			if (Rule->BunkerWallsUpSound != -1) {
				VocClass::PlayAt(Rule->BunkerWallsUpSound, PositionCoord);
			}
			break;

		default:
			break;
	}
}

// FUNCTION: GAMEMD 0x004593A0
void BuildingClass::UnloadBunker(void)
{
	TechnoClass * unit = BunkerLinkedItem;
	if (unit != NULL && unit->Fetch_RTTI() == RTTI_UNIT) {
		((FootClass *)unit)->Locomotion->Power_On();
		Coord center = Center_Coord() + Coord(-128, 128, 0);
		((FootClass *)unit)->Locomotion->Force_Track(0x47, center);
		((FootClass *)unit)->Set_Speed(1.0);
		unit->BunkerLinkedItem = NULL;
		BunkerLinkedItem = NULL;
		Transmit_Message(RADIO_OVER_OUT);
	}
}

// FUNCTION: GAMEMD 0x00459470
void BuildingClass::ClearBunker(void)
{
	DestroyNthAnim(BANIM_SPECIAL_ONE);
	DestroyNthAnim(BANIM_SPECIAL_TWO);
	if (BunkerLinkedItem != NULL) {
		if (Rule->BunkerWallsDownSound != -1) {
			VocClass::PlayAt(Rule->BunkerWallsDownSound, PositionCoord);
		}
		Replay_Anim(this, BANIM_SPECIAL_THREE, HealthRatio <= Rule->ConditionYellow);
		Replay_Anim(this, BANIM_SPECIAL_FOUR, HealthRatio <= Rule->ConditionYellow);
		Transmit_Message(RADIO_OVER_OUT);
		BunkerLinkedItem->BunkerLinkedItem = NULL;
		BunkerLinkedItem = NULL;
		TankBunkerState = 0;
		QueueMission(MISSION_GUARD, false);
	}
}

// FUNCTION: GAMEMD 0x004595C0
void BuildingClass::EmptyBunker(void)
{
	DestroyNthAnim(BANIM_SPECIAL_ONE);
	DestroyNthAnim(BANIM_SPECIAL_TWO);
	if (Rule->BunkerWallsDownSound != -1) {
		VocClass::PlayAt(Rule->BunkerWallsDownSound, PositionCoord);
	}
	Replay_Anim(this, BANIM_SPECIAL_THREE, HealthRatio <= Rule->ConditionYellow);
	Replay_Anim(this, BANIM_SPECIAL_FOUR, HealthRatio <= Rule->ConditionYellow);
	TechnoClass * unit = BunkerLinkedItem;
	if (unit == NULL) {
		unit = BunkerLinkedItem;
	}
	if (unit == NULL) {
		TankBunkerState = 0;
		QueueMission(MISSION_GUARD, false);
		return;
	}
	if (unit->Fetch_RTTI() == RTTI_UNIT) {
		unit->BunkerLinkedItem = NULL;
		((FootClass *)unit)->Locomotion->Power_On();
		Coord center = Center_Coord() + Coord(-128, 128, 0);
		((FootClass *)unit)->Locomotion->Force_Track(0x47, center);
		((FootClass *)unit)->Set_Speed(1.0);
		Cell exit = Get_Cell() + Cell(-1, 1);
		CellClass * dest = &Map[Map.Nearby_Location(exit, unit->Techno_Type_Class()->Speed)];
		unit->Assign_Destination(dest, true);
		unit->QueueMission(MISSION_MOVE, false);
		BunkerLinkedItem = NULL;
		TankBunkerState = 0;
		QueueMission(MISSION_GUARD, false);
		Transmit_Message(RADIO_OVER_OUT);
	}
}

// FUNCTION: GAMEMD 0x00459840
TechnoTypeClass const * BuildingClass::FUN_00459840(void) const
{
	if (Class->SecretInfantry != NULL) {
		return(Class->SecretInfantry);
	}
	if (Class->SecretUnit != NULL) {
		return(Class->SecretUnit);
	}
	if (Class->SecretBuilding != NULL) {
		return(Class->SecretBuilding);
	}
	return(SecretProduction);
}

// FUNCTION: GAMEMD 0x00459870
int BuildingClass::Anti_Air(void) const
{
	return(Class->AntiAirValue);
}

// FUNCTION: GAMEMD 0x00459880
int BuildingClass::Anti_Armor(void) const
{
	return(Class->AntiArmorValue);
}

// FUNCTION: GAMEMD 0x00459890
int BuildingClass::Anti_Infantry(void) const
{
	return(Class->AntiInfantryValue);
}

// FUNCTION: GAMEMD 0x004598A0
int BuildingClass::IsAbsorbAllowed(void) const
{
	if (Class->UnitAbsorb || Class->InfantryAbsorb) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004598D0
bool BuildingClass::FUN_004598D0(void) const
{
	if (Class->UndeploysInto != NULL && Class->ResourceGatherer) {
		return(false);
	}
	return(IsStrange() ? false : true);
}

static inline bool _Same(Coord const & a, TPoint3D<int> const & b) {return(a == b);}
static inline bool _Usable(Coord const & a) {return(a != (Coord const &)DAT_0089C848 && !(a == DAT_0089C870));}

// FUNCTION: GAMEMD 0x00459900
void BuildingClass::UpdateRefinerySmokeSystems(void)
{
	ParticleSystemTypeClass const * type = Class->RefinerySmokeParticleSystem;
	if (type != NULL) {
		int frames = Class->RefinerySmokeFrames;
		{
		Coord offset = Class->RefinerySmokeOffsetOne;
		if (offset != (Coord const &)DAT_0089C848 && !_Same(offset, DAT_0089C870)) {
			ParticleSystemClass * psys = new ParticleSystemClass(type, PositionCoord + offset, NULL, this, (Coord const &)DAT_0089C848, NULL);
			psys->FUN_006301F0(frames);
		}
		}
		{
		Coord offset = Class->RefinerySmokeOffsetTwo;
		if (_Usable(offset)) {
			ParticleSystemClass * psys = new ParticleSystemClass(type, PositionCoord + offset, NULL, this, (Coord const &)DAT_0089C848, NULL);
			psys->FUN_006301F0(frames);
		}
		}
		{
		Coord offset = Class->RefinerySmokeOffsetThree;
		if (_Usable(offset)) {
			ParticleSystemClass * psys = new ParticleSystemClass(type, PositionCoord + offset, NULL, this, (Coord const &)DAT_0089C848, NULL);
			psys->FUN_006301F0(frames);
		}
		}
		{
		Coord offset = Class->RefinerySmokeOffsetFour;
		if (_Usable(offset)) {
			ParticleSystemClass * psys = new ParticleSystemClass(type, PositionCoord + offset, NULL, this, (Coord const &)DAT_0089C848, NULL);
			psys->FUN_006301F0(frames);
		}
		}
	}
}

// FUNCTION: GAMEMD 0x00459C00
bool BuildingClass::CanBeSelectedNow(void) const
{
	if (!Class->IsVehicle()) {
		return(false);
	}
	return(TechnoClass::CanBeSelectedNow());
}

static inline CellClass * _As_Cell(AbstractClass * target)
{
	return((target == NULL) ? NULL : ((target->RTTI == RTTI_CELL) ? (CellClass *)target : NULL));
}

#pragma warning(push)
#pragma warning(disable : 4716)
// FUNCTION: GAMEMD 0x00459C20
int BuildingClass::VoiceDeploy(void)
{
	if (Is_Target_Cell(ArchiveTarget)) {
		if (_As_Cell(ArchiveTarget)->Land_Type() == LAND_TIBERIUM && Class->ResourceDestination && Class->ResourceGatherer && Class->VoiceHarvest != -1) {
			QueueVoice(Class->VoiceHarvest);
		} else {
			return(TechnoClass::VoiceDeploy());
		}
	} else {
		return(TechnoClass::VoiceDeploy());
	}
}
#pragma warning(pop)

static inline bool _Exit_Free(Cell const & cell)
{
	return(Map.In_Radar(cell) && Map[cell].Cell_Occupier() == NULL && Map[cell].Is_Clear_To_Move(SPEED_FOOT, false, false));
}

// FUNCTION: GAMEMD 0x00459CA0
bool BuildingClass::FUN_00459CA0(void) const
{
	Cell const * list = Class->ExitList;
	Cell origin = Get_Cell();
	if (list != NULL) {
		while (*list != REFRESH_EOL) {
			if (_Exit_Free(origin + *list++)) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00459D80
// TechnoClass::Is_In_Team

// FUNCTION: GAMEMD 0x00459D90
// TechnoClass::Is_In_Same_Zone_As

// FUNCTION: GAMEMD 0x00459DA0
// TechnoClass::Get_Z_Gradient

// FUNCTION: GAMEMD 0x00459DB0
Cell TechnoClass::GetLastFlightMapCoords(void) const
{
	return(DAT_0089C818);
}

// FUNCTION: GAMEMD 0x00459DC0
void TechnoClass::SetLastFlightMapCoords(Cell coord)
{
}

// FUNCTION: GAMEMD 0x00459DD0
// TechnoClass::Is_Allowed_To_Leave_Map

// FUNCTION: GAMEMD 0x00459DE0
// TechnoClass::Advance_Waypoint_Path

// FUNCTION: GAMEMD 0x00459DF0
bool TechnoClass::EnterGrinder(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00459E00
bool TechnoClass::EnterBioReactor(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00459E10
bool TechnoClass::EnterTankBunker(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00459E20
bool TechnoClass::EnterBattleBunker(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00459E30
bool TechnoClass::GarrisonStructure(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00459E40
bool TechnoClass::IsParalyzed(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x00459E50
bool TechnoClass::Crash(ObjectClass * killer)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00459E60
// TechnoClass::Draw_Action_Line

// FUNCTION: GAMEMD 0x00459E70
int BuildingClass::Fetch_Object_Size(void) const
{
	return(sizeof(BuildingClass));
}

// FUNCTION: GAMEMD 0x00459E80
HRESULT STDMETHODCALLTYPE BuildingClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00459EC0
RTTIType BuildingClass::Fetch_RTTI(void) const
{
	return(RTTI_BUILDING);
}

// FUNCTION: GAMEMD 0x00459ED0
wchar_t const * BuildingClass::Full_Name(void) const
{
	return(Class->UIName);
}

// FUNCTION: GAMEMD 0x00459EE0
ObjectTypeClass const * BuildingClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: GAMEMD 0x00459EF0
Coord BuildingClass::Render_Coord(void) const
{
	return(PositionCoord - Coord(CELL_LEPTON/2, CELL_LEPTON/2, 0));
}

// SYNTHETIC: GAMEMD 0x00459F20
// ??_GBuildingClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00459F40
BuildingClass * __fastcall FUN_00459F40(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	return(object->Fetch_RTTI() == RTTI_BUILDING ? (BuildingClass *)object : NULL);
}

// TEMPLATE: GAMEMD 0x00459F60
// ??8?$TPoint3D@H@@QBE_NABV0@@Z
template class TPoint3D<int>;

// TEMPLATE: GAMEMD 0x00459F90
// ?Clear@?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00459FC0
// ?ID@?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@UAEHPBQBVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00459FE0
// ?GetItem@?$VectorClass@PBVParticleSystemTypeClass@@@@UBEPBVParticleSystemTypeClass@@H@Z
template class DynamicVectorClass<InfantryClass *>;

// TEMPLATE: GAMEMD 0x00459FF0
// ?Clear@?$DynamicVectorClass@PAVInfantryClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0045A020
// ?ID@?$DynamicVectorClass@PAVInfantryClass@@@@UAEHPBQAVInfantryClass@@@Z
// TEMPLATE: GAMEMD 0x0045A040
// ?GetItem@?$VectorClass@PAVInfantryClass@@@@UBEPAVInfantryClass@@H@Z
template class DynamicVectorClass<BuildingClass *>;

// TEMPLATE: GAMEMD 0x0045A050
// ??C?$_com_ptr_t@V?$_com_IIID@UILocomotion@@$1?_GUID_070f3290_9841_11d1_b709_00a024ddafd1@@3U__s_GUID@@A@@@@QBEPAUILocomotion@@XZ

// TEMPLATE: GAMEMD 0x0045A070
// ?Clear@?$DynamicVectorClass@PAVFoggedObjectClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0045A0A0
// ?ID@?$DynamicVectorClass@PAVFoggedObjectClass@@@@UAEHPBQAVFoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x0045A0C0
// ?GetItem@?$VectorClass@PAVFoggedObjectClass@@@@UBEPAVFoggedObjectClass@@H@Z
template class DynamicVectorClass<FoggedObjectClass *>;

// TEMPLATE: GAMEMD 0x0045A0D0
// ?Clear@?$DynamicVectorClass@PAVReestablish@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0045A100
// ?ID@?$DynamicVectorClass@PAVReestablish@@@@UAEHPBQAVReestablish@@@Z
// TEMPLATE: GAMEMD 0x0045A120
// ?GetItem@?$VectorClass@PAVReestablish@@@@UBEPAVReestablish@@H@Z
template class DynamicVectorClass<Reestablish *>;

// TEMPLATE: GAMEMD 0x0045A130
// ??0?$TRect@H@@QAE@ABV?$TPoint2D@H@@HH@Z
template class TRect<int>;

// TEMPLATE: GAMEMD 0x0045A160
// ?_Release@?$_com_ptr_t@V?$_com_IIID@UILocomotion@@$1?_GUID_070f3290_9841_11d1_b709_00a024ddafd1@@3U__s_GUID@@A@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x0045A170
// ?_AddRef@?$_com_ptr_t@V?$_com_IIID@UILocomotion@@$1?_GUID_070f3290_9841_11d1_b709_00a024ddafd1@@3U__s_GUID@@A@@@@AAEXXZ
template class _com_ptr_t<_com_IIID<ILocomotion, &__uuidof(ILocomotion)> >;

// TEMPLATE: GAMEMD 0x0045A180
// ??8?$VectorClass@PBVParticleSystemTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0045A1C0
// ?Resize@?$VectorClass@PBVParticleSystemTypeClass@@@@UAE_NHPBQBVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0045A270
// ?Clear@?$VectorClass@PBVParticleSystemTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0045A2A0
// ?ID@?$VectorClass@PBVParticleSystemTypeClass@@@@UAEHABQBVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0045A2E0
// ?ID@?$VectorClass@PBVParticleSystemTypeClass@@@@UAEHPBQBVParticleSystemTypeClass@@@Z
template class VectorClass<ParticleSystemTypeClass const *>;

// TEMPLATE: GAMEMD 0x0045A300
// ?Resize@?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@UAE_NHPBQBVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0045A3B0
// ?ID@?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@UAEHABQBVParticleSystemTypeClass@@@Z
template class DynamicVectorClass<ParticleSystemTypeClass const *>;

// TEMPLATE: GAMEMD 0x0045A3E0
// ??8?$VectorClass@PAVInfantryClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0045A420
// ?Resize@?$VectorClass@PAVInfantryClass@@@@UAE_NHPBQAVInfantryClass@@@Z
// TEMPLATE: GAMEMD 0x0045A4D0
// ?Clear@?$VectorClass@PAVInfantryClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0045A500
// ?ID@?$VectorClass@PAVInfantryClass@@@@UAEHABQAVInfantryClass@@@Z
// TEMPLATE: GAMEMD 0x0045A540
// ?ID@?$VectorClass@PAVInfantryClass@@@@UAEHPBQAVInfantryClass@@@Z
template class VectorClass<InfantryClass *>;

// TEMPLATE: GAMEMD 0x0045A560
// ?Resize@?$DynamicVectorClass@PAVInfantryClass@@@@UAE_NHPBQAVInfantryClass@@@Z
// TEMPLATE: GAMEMD 0x0045A610
// ?ID@?$DynamicVectorClass@PAVInfantryClass@@@@UAEHABQAVInfantryClass@@@Z
template class DynamicVectorClass<InfantryClass *>;

// TEMPLATE: GAMEMD 0x0045A640
// ??8?$VectorClass@PAVFoggedObjectClass@@@@UBE_NABV0@@Z
template class VectorClass<FoggedObjectClass *>;


// TEMPLATE: GAMEMD 0x0045A680
// ?Resize@?$DynamicVectorClass@PAVFoggedObjectClass@@@@UAE_NHPBQAVFoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x0045A730
// ?ID@?$DynamicVectorClass@PAVFoggedObjectClass@@@@UAEHABQAVFoggedObjectClass@@@Z

// TEMPLATE: GAMEMD 0x0045A760
// ??8?$VectorClass@PAVReestablish@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0045A7A0
// ?Resize@?$VectorClass@PAVReestablish@@@@UAE_NHPBQAVReestablish@@@Z
// TEMPLATE: GAMEMD 0x0045A850
// ?Clear@?$VectorClass@PAVReestablish@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0045A880
// ?ID@?$VectorClass@PAVReestablish@@@@UAEHABQAVReestablish@@@Z
// TEMPLATE: GAMEMD 0x0045A8C0
// ?ID@?$VectorClass@PAVReestablish@@@@UAEHPBQAVReestablish@@@Z
template class VectorClass<BuildingClass *>;

// TEMPLATE: GAMEMD 0x0045A8E0
// ?Resize@?$DynamicVectorClass@PAVReestablish@@@@UAE_NHPBQAVReestablish@@@Z
// TEMPLATE: GAMEMD 0x0045A990
// ?ID@?$DynamicVectorClass@PAVReestablish@@@@UAEHABQAVReestablish@@@Z

// SYNTHETIC: GAMEMD 0x0045A9C0
// ??_G?$VectorClass@PBVParticleSystemTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0045AA10
// ??_G?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0045AA60
// ??_G?$VectorClass@PAVInfantryClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0045AAB0
// ??_G?$DynamicVectorClass@PAVInfantryClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0045AB00
// ??_G?$DynamicVectorClass@PAVFoggedObjectClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0045AB50
// ?Resize@?$VectorClass@PAVFoggedObjectClass@@@@UAE_NHPBQAVFoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x0045AC00
// ?Clear@?$VectorClass@PAVFoggedObjectClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0045AC30
// ?ID@?$VectorClass@PAVFoggedObjectClass@@@@UAEHABQAVFoggedObjectClass@@@Z
// TEMPLATE: GAMEMD 0x0045AC70
// ?ID@?$VectorClass@PAVFoggedObjectClass@@@@UAEHPBQAVFoggedObjectClass@@@Z
// SYNTHETIC: GAMEMD 0x0045AC90
// ??_G?$VectorClass@PAVFoggedObjectClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0045ACE0
// ??_G?$VectorClass@PAVReestablish@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0045AD30
// ??_G?$DynamicVectorClass@PAVReestablish@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0045AD80
// ??0?$VectorClass@PBVParticleSystemTypeClass@@@@QAE@HPBQBVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x0045ADD0
// ?Delete_Index@?$DynamicVectorClass@PAVAbstractClass@@@@QAE_NH@Z
// TEMPLATE: GAMEMD 0x0045AE10
// ?Delete_Index@?$DynamicVectorClass@PAVBuildingClass@@@@QAE_NH@Z
// TEMPLATE: GAMEMD 0x0045AE50
// ??0?$VectorClass@PAVReestablish@@@@QAE@HPBQAVReestablish@@@Z
template class VectorClass<Reestablish *>;
template class DynamicVectorClass<AbstractClass *>;


#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0045AFA0 SYMBOL
// _$E256
// SYNTHETIC: GAMEMD 0x0045AFD0 SYMBOL
// _$E259
// SYNTHETIC: GAMEMD 0x0045AFF0 SYMBOL
// _$E262
// SYNTHETIC: GAMEMD 0x0045B010 SYMBOL
// _$E265
// SYNTHETIC: GAMEMD 0x0045B030 SYMBOL
// _$E268
// SYNTHETIC: GAMEMD 0x0045B050 SYMBOL
// _$E271
// SYNTHETIC: GAMEMD 0x0045B070 SYMBOL
// _$E274
// SYNTHETIC: GAMEMD 0x0045B0B0 SYMBOL
// _$E277
// SYNTHETIC: GAMEMD 0x0045B0E0 SYMBOL
// _$E280
// SYNTHETIC: GAMEMD 0x0045B110 SYMBOL
// _$E283
// SYNTHETIC: GAMEMD 0x0045B140 SYMBOL
// _$E286
// SYNTHETIC: GAMEMD 0x0045B150 SYMBOL
// _$E289
// SYNTHETIC: GAMEMD 0x0045B170 SYMBOL
// _$E292

// SYNTHETIC: GAMEMD 0x0045B190 SYMBOL
// _$E295
Rect DAT_0089C8A0(0, 0, 0, 0);
