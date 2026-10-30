// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "bullet.h"
#include "_convert.h"
#include "_map.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "building.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "convert.h"
#include "draw.h"
#include "empulse.h"
#include "fly.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "inline.h"
#include "lightcon.h"
#include "map.h"
#include "missile.h"
#include "notify.h"
#include "overtype.h"
#include "partsys.h"
#include "radsite.h"
#include "rules.h"
#include "scheme.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tracker.h"
#include "trajhelp.h"
#include "unit.h"
#include "voxdrsys.h"
#include "warhead.h"
#include "weapon.h"

// SYNTHETIC: GAMEMD 0x004660D0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00466100 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00466120 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00466140 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00466160 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00466180 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004661A0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004661E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00466210 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00466240 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00466270 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00466280 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004662A0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004662C0 SYMBOL
// _$E47

DynamicVectorClass<BulletClass *> BulletClass::ScalableBullets;

extern Cell DAT_0089DDF0;
extern int DAT_0089DE70;
extern int DAT_0081AF00;
extern ULONG COMRefCount;
#include "airstrike.h"
#include "bomb.h"
#include "bomblist.h"
#include "capmgr.h"
#include "parasite.h"
#include "scenario.h"
#include "tag.h"
#include "temporal.h"
#include "unittype.h"
#include "vanim.h"
#include "vector3.h"
#include "voc.h"

extern double DAT_0081AEF8;



// SYNTHETIC: GAMEMD 0x004662E0 SYMBOL
// _$E166
// SYNTHETIC: GAMEMD 0x00466320 SYMBOL
// _$E163

// FUNCTION: GAMEMD 0x00466380
BulletClass::BulletClass(void) :
	BASECLASS(),
	Class(NULL),
	Payback(NULL),
	IsInaccurate(false),
	Fuse(),
	field_A4(true),
	IsLaunching(true),
	IsBright(false),
	BounceCount(0),
	CourseLockCounter(0),
	TarCom(NULL),
	MaxSpeed(0),
	InheritedColor(-1),
	ClosureSamples(0),
	SmoothedClosure(0),
	Warhead(NULL),
	AnimFrame(0),
	AnimRate(0),
	WeaponType(NULL),
	LastMapCoords(-1, -1),
	NextAnim(NULL),
	SpawnNextAnim(false)
{
	Create_ID();
	SourceCoords.X = 0;
	SourceCoords.Y = 0;
	SourceCoords.Z = 0;
	TargetCoords.X = 0;
	TargetCoords.Y = 0;
	TargetCoords.Z = 0;
	Array.Add(this);
}

// FUNCTION: GAMEMD 0x004664C0
void BulletClass::Construct(BulletTypeClass const *type, AbstractClass *target, TechnoClass *payback, int strength, WarheadTypeClass const *warhead, int max_speed, bool bright)
{
	Class = (BulletTypeClass *)type;
	Payback = payback;
	TarCom = target;
	MaxSpeed = max_speed;
	Warhead = (WarheadTypeClass *)warhead;
	IsBright = bright;
	Strength = strength;
	AnimFrame = 0;
	AnimRate = Class->AnimRate;
	if (Class->FirersPalette && Payback != NULL) {
		InheritedColor = Payback->House->Scheme;
	} else {
		InheritedColor = -1;
	}
	DamageMultiplier = 256;
	NextAnim = NULL;
	SpawnNextAnim = false;
}

// FUNCTION: GAMEMD 0x00466560
// BulletClass::~BulletClass
BulletClass::~BulletClass(void)
{
	Detach_This_From_All(this, true);

	if (SpawnNextAnim) {
		PointerExpiredNotification::NotifyInvalidAnim.Delete(this);
	}

	if (GameActive) {
		BulletClass::Limbo();
	}

	Class = NULL;
	Payback = NULL;
	NextAnim = NULL;
	Array.Delete(this);
}

// FUNCTION: GAMEMD 0x00466660
Cell const * BulletClass::Occupy_List(bool) const
{
	static Cell const _list[] = { REFRESH_EOL };

	return(_list);
}

// SYNTHETIC: GAMEMD 0x004666B0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x004666C0
bool BulletClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00468000
int BulletClass::Shape_Number(void) const
{
	int shapenum = 0;

	if (!Class->NoRotate) {
		shapenum = TechnoClass::BodyShape[DirType(fastmath::atan2(-Velocity.Y, Velocity.X)).As_Dir32()];
	}

	if (Class->AnimLow || Class->AnimHigh) {
		shapenum = AnimFrame;
	}

	return(shapenum);
}

// FUNCTION: GAMEMD 0x00468090
void BulletClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	if (Scen->Special.IsFogOfWar && Map.Is_Fogged(Get_Coord())) return;

	if (Class->IsInvisible) return;

	if (SpawnNextAnim) return;

	if (Class->IsVoxel) {
		Matrix3D matrix(true);
		matrix.Rotate_Z(Velocity.Get_Yaw().As_Radian32());
		matrix.Rotate_Y(-Velocity.Get_Pitch().As_Radian32());
		Draw_Voxel(Class->Voxel, Get_Isometric_View_Matrix() * matrix, point, cliprect, 0, SHAPE_ALPHA, NORMAL_LIGHT);

	} else {
		ShapeSet const * shapeptr = (ShapeSet const *)Get_Image_Data();
		if (shapeptr == NULL) return;

		int shapenum = Shape_Number();

		int height_agl = HeightAGL;
		int height_gl = Map.Get_Height_GL(Get_Coord());

		if (!IsOnBridge && Map[Get_Coord()].IsUnderBridge && height_agl >= BRIDGE_LEPTON_HEIGHT) {
			height_agl -= BRIDGE_LEPTON_HEIGHT;
			height_gl += BRIDGE_LEPTON_HEIGHT;
		}

		if (height_agl > 0 && Class->IsShadow) {
			Point2D shadowpoint = point + Point2D(0, TacticalMap->Z_Lepton_To_Pixel(height_agl));
			Draw_Shape(*LogicalSurface, *NormalDrawer, shapeptr, shapenum, shadowpoint, cliprect, ShapeFlags_Type(SHAPE_DARKEN|SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZGRAD), 0, -10 - TacticalMap->Z_Lepton_To_Pixel(height_gl));
		}

		ConvertClass * drawer = NormalDrawer;
		if (Class->IsAnimPalette) {
			drawer = AnimDrawer;
		} else if (Class->FirersPalette) {
			if (InheritedColor != -1) {
				drawer = ColorSchemes[InheritedColor]->Converter;
			} else {
				drawer = ColorSchemes[PlayerPtr->Scheme]->Converter;
			}
		}
		Draw_Shape(*LogicalSurface, *drawer, shapeptr, shapenum, point, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZGRAD), 0, -30 - TacticalMap->Z_Lepton_To_Pixel(Height));
	}
}

// FUNCTION: GAMEMD 0x00468430
void BulletClass::LoseTarget(void)
{
	Cell cell = TarCom->Center_Coord().As_Cell();
	if (!ScenarioInit && !TarCom->In_Air() && cell != DAT_0089DDF0) {
		TarCom = &Map[cell];
	} else {
		TarCom = NULL;
	}
}

// FUNCTION: GAMEMD 0x004684E0
void BulletClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);

	if (Payback == target) {
		Payback = NULL;
	}
	if (TarCom == target) {
		LoseTarget();
	}
	if (Class == target) {
		Class = NULL;
	}
	if (WeaponType == target) {
		WeaponType = NULL;
	}
	if (NextAnim == target) {
		NextAnim = NULL;
	}

	if (all && target->Fetch_RTTI() == RTTI_BULLET) {
		BulletClass * bullet = (BulletClass *)target;
		if (bullet->Class->Scalable) {
			int tracked = ScalableBullets.Count() + ScalableBullets.Count() + ScalableBullets.Count();
			for (int i = 0; i < ScalableBullets.Count(); i++) {
				if (bullet == ScalableBullets[i]) {
					ScalableBullets.Delete(bullet);
					return;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00468B90
LayerType BulletClass::In_Which_Layer(void) const
{
	return(Class->Flat ? LAYER_SURFACE : LAYER_AIR);
}

// FUNCTION: GAMEMD 0x00468BB0
bool BulletClass::Is_Forced_To_Explode(Coord & coord) const
{
	coord = PositionCoord;
	CellClass * cell = &Map[coord];

	if (Class->SubjectToCliffs || Class->SubjectToWalls) {
		CellClass * source = &Map[SourceCoords];
		CellClass * target = &Map[TargetCoords];
		CellClass * current = &Map[LastMapCoords];
		HouseClass * owner = (HouseClass *)Payback;
		if (Payback != NULL) {
			owner = Payback->House;
		}
		if (TrajectoryHelper::GetObstacle(source, target, current, coord, Class, owner)) {
			return(true);
		}
	}

	if (HeightAGL <= -DAT_0089DE70 * 4) {
		return(true);
	}

	if (TarCom != NULL && Class->FlakScatter) {
		Coord pos = PositionCoord;
		if (pos.Z < TarCom->As_Coord().Z && HeightAGL < 0) {
			return(true);
		}
	}

	if (Class->Level && cell->On_Ground()) {
		return(true);
	}

	if (Class->IsAntiAircraft && TarCom != NULL && TarCom->In_Air() && Distance(TarCom) < 128) {
		return(true);
	}

	return(false);
}

static inline int Coord_Dist(Coord const & a, Coord const & b)
{
	Coord c = a - b;
	return(c.Length());
}

// FUNCTION: GAMEMD 0x00468D80
void BulletClass::Bullet_Explodes(bool forced)
{
	Coord coord = PositionCoord;
	TechnoClass * target = NULL;
	if (TarCom != NULL && TarCom->In_Air()) {
		target = (TechnoClass *)TarCom;
	}

	if (!Class->IsInaccurate) {
		if (TarCom != NULL) {
			if (Coord_Dist(coord, TarCom->Center_Coord()) < 32 && !Class->IsAirburst && !Class->IsInaccurate) {
				coord = TarCom->Center_Coord();
			}
		}

		if (!Warhead->IsEMEffect && !Class->IsAirburst) {
			if (!forced && !Class->IsArcing && !Is_Homing() && Fuse.Fuse_Target() != COORD_NONE) {
				coord = Fuse.Fuse_Target();
			}

			if (target != NULL && target->In_Which_Layer() != LAYER_GROUND) {
				if (Distance(TarCom) < 128) {
					coord = target->Target_Coord();
				}
			} else if (TarCom != NULL && Distance(TarCom) < 42) {
				coord = TarCom->As_Coord();
				if (TarCom->Fetch_RTTI() == RTTI_BUILDING) {
					Point3D const & offset = ((BuildingClass *)TarCom)->Class->TargetCoordOffset;
					if (offset.X != 0 || offset.Y != 0 || offset.Z != 0) {
						coord = ((ObjectClass *)TarCom)->Target_Coord();
					}
				}
			}
		}
	}

	if (!Class->IsAirburst) {
		Coord newcoord = coord;
		for (int i = 0; i < Class->Cluster; i++) {
			Detonate(coord);
			if (!IsActive) {
				break;
			}
			coord = Coord_Scatter(newcoord, Random_Pick(CELL_LEPTON, CELL_LEPTON * 2));
		}
	} else {
		Detonate(coord);
	}
}

static inline TechnoClass * Techno_Of(AbstractClass * target)
{
	return(target == NULL ? NULL : ((target->AbstractFlags & 1) ? (TechnoClass *)target : NULL));
}

static inline ObjectClass * Object_Of(AbstractClass * target)
{
	return(target == NULL ? NULL : ((target->AbstractFlags & 2) ? (ObjectClass *)target : NULL));
}

static inline FootClass * Foot_Of(AbstractClass * target)
{
	return(target == NULL ? NULL : ((target->AbstractFlags & 4) ? (FootClass *)target : NULL));
}

static inline bool Is_Foot_Object(AbstractClass const * target)
{
	return(target != NULL && (target->AbstractFlags & 4));
}

static inline UnitClass * Unit_Of(AbstractClass * target)
{
	return(target == NULL ? NULL : ((target->Fetch_RTTI() == RTTI_UNIT) ? (UnitClass *)target : NULL));
}

static inline Vector3 Normalize_Dir(const Vector3 & vec)
{
	float len = (float)fastmath::sqrt(vec.Z*vec.Z + vec.Y*vec.Y + vec.X*vec.X);
	if (len != 0.0) {
		return(vec / len);
	}
	return(vec);
}

// FUNCTION: GAMEMD 0x004690B0
void BulletClass::Detonate(Coord const & coord)
{
	int result = 1;
	bool nullified = false;

	if (Warhead->ShakeXlo != 0 || Warhead->ShakeXhi != 0) {
		Map.ScreenX = Sim_Random_Pick(Warhead->ShakeXlo, Warhead->ShakeXhi);
	}
	if (Warhead->ShakeYlo != 0 || Warhead->ShakeYhi != 0) {
		Map.ScreenY = Sim_Random_Pick(Warhead->ShakeYlo, Warhead->ShakeYhi);
	}

	if (WeaponType != NULL && WeaponType->RadLevel > 0) {
		ApplyRadiationToCell(coord.As_Cell(), (int)WeaponType->WarheadPtr->SpreadFactor, WeaponType->RadLevel);
	}

	if (Warhead->MindControl) {
		if (Payback != NULL && Payback->CaptureManager != NULL) {
			ObjectClass * object = Object_Of(TarCom);
			bool player = (TarCom != NULL && (TarCom->AbstractFlags & 1) && ((TechnoClass *)TarCom)->House->Is_Player_Control());
			if (object != NULL && object->IsActive && object->Tag != NULL) {
				object->Tag->Spring(TEVENT_ATTACKED, object, DAT_0089DDF0, false, Payback);
				object->Tag->Spring(TEVENT_ATTACKED_BY, object, DAT_0089DDF0, false, Payback);
			}
			if (Payback->CaptureManager->CaptureUnit(TarCom) && Rule->YuriMindControlSound != -1) {
				if (Payback->House->Is_Player_Control() || player) {
					VocClass::PlayAt(Rule->YuriMindControlSound, object->PositionCoord);
				}
			}
		}
	} else if (Warhead->IvanBomb) {
		BombListInstance.Plant(Payback, Techno_Of(TarCom));
	} else if (Warhead->ElectricAssault) {
		if (TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_BUILDING && Payback != NULL && Payback->Fetch_RTTI() == RTTI_INFANTRY) {
			((BuildingClass *)TarCom)->FUN_00452820((InfantryClass *)Payback);
		}
	} else if (Warhead->Parasite) {
		if (Payback != NULL) {
			((FootClass *)Payback)->ParasiteImUsing->TryInfect(Foot_Of(TarCom));
		}
	} else if (Warhead->Temporal) {
		if (Payback != NULL && TarCom != NULL) {
			if (!(TarCom->AbstractFlags & 4) || Foot_Of(TarCom)->In_Which_Layer() == LAYER_GROUND) {
				TechnoClass * techno = (TechnoClass *)TarCom;
				if (techno != NULL) {
					if (!(techno->AbstractFlags & 1)) {
						techno = NULL;
					} else if (techno->BunkerLinkedItem != NULL && techno->Fetch_RTTI() == RTTI_UNIT) {
						techno = techno->BunkerLinkedItem;
						Payback->Assign_Target(techno);
					}
				}
				Payback->TemporalImUsing->Fire(techno);
			}
		}
	} else if (Warhead->IsLocomotor) {
		if (Payback != NULL && Payback->LocomotorTarget != TarCom && Payback->LocomotorSource == NULL) {
			if (!Is_Foot_Object(Payback) || !Foot_Of(Payback)->IsAttackedByLocomotor) {
				bool infactory = false;
				if (TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_UNIT) {
					TechnoClass * contact = Unit_Of(TarCom)->FUN_0065AD30(0);
					if (contact != NULL && contact->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)contact)->Class->IsWeaponsFactory) {
						if (Map[Unit_Of(TarCom)->Get_Coord()].Cell_Building() == contact) {
							infactory = true;
						}
					}
				}

				if (Payback->LocomotorTarget != NULL) {
					Payback->ReleaseLocomotor(false);
				}

				if (TarCom != NULL && !infactory) {
					if (!Is_Foot_Object(TarCom) || Foot_Of(TarCom)->In_Which_Layer() == LAYER_GROUND) {
						FootClass * foot = (FootClass *)TarCom;
						if (Is_Foot_Object(foot) && !foot->IsIronCurtained()) {
							if ((TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_UNIT) || (TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_AIRCRAFT)) {
								if (!foot->IsAttackedByLocomotor && foot->Techno_Type_Class()->Size < Strength) {
									Payback->ImbueLocomotor(foot, Warhead->Locomotor);
								}
							}
						}
					}
				}
			}
		}
	} else if (Warhead->Airstrike) {
		if (Payback != NULL && TarCom != NULL && (TarCom->AbstractFlags & 1) && Payback->Airstrike != NULL) {
			if (Payback->Airstrike->FUN_0041D7E0(Techno_Of(TarCom))) {
				Payback->Airstrike->StartMission(Techno_Of(TarCom));
			}
		}
	} else if (Warhead->DirectRocker && TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_UNIT) {
		TechnoClass * techno = Techno_Of(TarCom);
		if (Payback != NULL && techno != NULL && !techno->IsIronCurtained()) {
			double force = MIN(((DamageMultiplier * Strength) >> 8) * Rule->DirectRockingCoefficient / DAT_0081AEF8, 4.0);
			Coord tcoord = techno->PositionCoord;
			Coord pcoord = Payback->PositionCoord;
			Vector3 dir((float)(pcoord.X - tcoord.X), (float)(pcoord.Y - tcoord.Y), (float)(pcoord.Z - tcoord.Z));
			Vector3 unit = Normalize_Dir(dir);
			Vector3 push(unit.X * 10.0f, unit.Y * 10.0f, unit.Z * 10.0f);
			tcoord += Coord((int)push.X, (int)push.Y, (int)push.Z);
			techno->vt_entry_3D8(tcoord, (float)force, true);
			techno->DirectRockerLinkedUnit = (FootClass *)Payback;
			Payback->DirectRockerLinkedUnit = (FootClass *)techno;
		}
	} else if (Warhead->BombDisarm) {
		if (TarCom != NULL && (TarCom->AbstractFlags & 2) && ((ObjectClass *)TarCom)->AttachedBomb != NULL) {
			((ObjectClass *)TarCom)->AttachedBomb->Disarm();
		}
	} else if (Warhead->MakesDisguise) {
		if (Payback != NULL) {
			Payback->DisguiseAs(TarCom);
		}
	} else if (Warhead->NukeMaker) {
		NukeMaker();
	} else {
		if (Class->ShrapnelWeapon != NULL) {
			Shrapnel();
		}
		int damage = (DamageMultiplier * Strength) >> 8;
		HouseClass * house = NULL;
		if (Payback != NULL) {
			house = Payback->House;
		}
		result = MapClass::DamageArea(coord, damage, Payback, Warhead, true, house);
		if (!IsActive) return;
		if (result == 2) {
			nullified = true;
		}
	}

	Coord blast = coord;
	if (Class->IsInvisible) {
		blast = Coord_Scatter(blast, CELL_LEPTON / 8, false);
	}

	LandType land = LAND_NONE;
	if (HeightAGL < DAT_0089DE70 * 2) {
		land = Map[Get_Coord()].Land_Type();
		if (land == LAND_WATER && result == 0) {
			ObjectClass * occupier = Map[Get_Coord()].Cell_Occupier();
			if (occupier != NULL && occupier->Fetch_RTTI() == RTTI_UNIT) {
				UnitTypeClass * utype = ((UnitClass *)occupier)->Class;
				if (utype->Naval && !utype->Underwater) {
					land = LAND_NONE;
				}
			}
		}
	}

	AnimTypeClass const * anim = Combat_Anim(Strength, Warhead, land, PositionCoord);

	if (IsBright && !nullified) {
		unsigned int flags = 0;
		if (Warhead->CLDisableRed) flags = 2;
		if (Warhead->CLDisableGreen) flags |= 4;
		if (Warhead->CLDisableBlue) flags |= 8;
		Combat_Lighting(blast, Strength, Warhead, true, flags);
	}

	if (nullified) {
		AnimTypeClass const * nullanim = Rule->WeaponNullifyAnim;
		if (nullanim != NULL) {
			new AnimClass(nullanim, blast, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(blast));
		}
		return;
	}

	if (anim != NULL) {
		AnimClass * aptr = new AnimClass(anim, blast, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(blast));
		if (aptr == NULL && Warhead == Rule->NukeWarhead) {
			HousesType house = HOUSE_NONE;
			if (Payback != NULL) {
				house = Payback->House->Class->House;
			}
			AnimClass::Do_Atom_Damage(house, blast.As_Cell());
		}
	}

	if (Warhead->MaxDebris > 0) {
		int count = Random_Pick(Warhead->MinDebris, Warhead->MaxDebris - 1);
		if (Warhead->DebrisTypes.Count() > 0) {
			int index = 0;
			while (count > 0) {
				int number = abs((int)Scen->RandomNumber) % (Warhead->DebrisMaximums[index] + 1);
				if (number >= count) number = count;
				for (int i = 0; i < number; i++) {
					new VoxelAnimClass(Warhead->DebrisTypes[index], Center_Coord(), NULL);
				}
				count -= number;
				index++;
				if (index >= Warhead->DebrisTypes.Count()) {
					index = 0;
				}
			}
		}
		if (Warhead->DebrisTypes.Count() == 0) {
			for (; count > 0; count--) {
				new AnimClass(Rule->MetallicDebris[Random_Pick(0, Rule->MetallicDebris.Count() - 1)], Center_Coord() + Coord(0, 0, 20), 0, 1, ShapeFlags_Type(0x600), 0, false);
			}
		}
	}

	if (Class->IsAirburst) {
		WeaponTypeClass * weapon = Class->AirburstWeapon;
		BulletTypeClass const * btype = weapon->Bullet;
		FacingType face = FACING_N;
		CellClass * cell = Get_Cell_Ptr();
		for (int i = 0; i < 8; i++) {
			CellClass * adjacent = &cell->Adjacent_Cell(face);
			face = FacingType((face + 1) & 7);
			BulletClass * bullet = Create_Bullet(btype, adjacent, Payback, weapon->Attack, weapon->WarheadPtr, 50, false);
			if (bullet != NULL) {
				TVelocity3D<double> velocity(DirType((Dir256)Random_Pick(DIR_MIN, DIR_NE)), DirType(DIR_S), weapon->MaxSpeed / 10);
				bullet->Unlimbo(PositionCoord, velocity);
			}
		}

		BulletClass * bullet = Create_Bullet(btype, cell, Payback, weapon->Attack, weapon->WarheadPtr, 50, false);
		if (bullet != NULL) {
			TVelocity3D<double> velocity(DirType((Dir256)Random_Pick(DIR_MIN, DIR_NE)), DirType(DIR_S), weapon->MaxSpeed / 10);
			bullet->Unlimbo(PositionCoord, velocity);
		}
	}
}

// FUNCTION: GAMEMD 0x0046ADE0
void BulletClass::ApplyRadiationToCell(Cell cell, int radius, int amount)
{
	CellClass * cptr = &Map[cell];
	RadSiteClass * site = cptr->FUN_00487C80();
	if (site == NULL) {
		site = new RadSiteClass();
		site->SetBaseCell(&cell);
		site->SetSpread(radius);
		site->SetRadLevel(amount);
		site->Activate();
		cptr->FUN_00487C70(site);
	} else {
		site->Add(amount);
	}
}

// FUNCTION: GAMEMD 0x0046AE70
HRESULT STDMETHODCALLTYPE BulletClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) BulletClass(NoInitClass());
		if (NextAnim != NULL) {
			PointerExpiredNotification::NotifyInvalidAnim.Add(this);
		}

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&Payback);
		Swizzle_Pointer(&Warhead);
		Swizzle_Pointer(&TarCom);
		Swizzle_Pointer(&WeaponType);
		Swizzle_Pointer(&NextAnim);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0046AFB0
HRESULT STDMETHODCALLTYPE BulletClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x0046AFD0
ULONG STDMETHODCALLTYPE BulletClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: GAMEMD 0x0046AFF0
ULONG STDMETHODCALLTYPE BulletClass::Release(void)
{
	COMRefCount--;
	ULONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}
	return(count);
}

// FUNCTION: GAMEMD 0x0046B030
bool BulletClass::Is_Homing(void) const
{
	if (Class->ROT > 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0046B050
BulletClass * __fastcall Create_Bullet(BulletTypeClass const * type, AbstractClass * target, TechnoClass * payback, int strength, WarheadTypeClass const * warhead, int max_speed, bool bright)
{
	LPVOID unk = NULL;
	if (FAILED(CoCreateInstance(CLSID_BulletClass, NULL, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER, IID_IUnknown, &unk))) {
		return(NULL);
	}

	BulletClass * bullet = (BulletClass *)unk;
	bullet->Construct(type, target, payback, strength, warhead, max_speed, bright);
	return(bullet);
}

// FUNCTION: GAMEMD 0x0046B0C0
void BulletClass::Draw_Voxel(VoxelDataStruct const & voxeldata, Matrix3D const & transform, Point2D const & drawpoint, Rect const & cliprect, int frame, ShapeFlags_Type flags, int brightness) const
{
	VoxelLibrary * voxlib = voxeldata.VoxLib;
	MotionLibrary * motlib = voxeldata.MotLib;

	VoxelDrawSystem::Precalculate_Light(voxlib, 0, 0, transform, VoxelLightSource);
	VoxelDrawSystem::Reset();

	for (unsigned int i = 0; i < voxlib->Get_Layer_Count(); i++) {
		Matrix3D matrix;
		if (motlib != NULL) {
			Matrix3D hva_transform = motlib->Get_Layer_Matrix(i, frame);
			matrix = transform * hva_transform;
		}
		VoxelDrawSystem::Prep_For_Object(voxlib, i, 0, VoxelCameraMatrix * matrix);
	}

	SurfaceRegion region = VoxelDrawSystem::Render();
	int extra = 0;
	ShapeFlags_Type f = ShapeFlags_Type(flags & ~SHAPE_REMAP);
	if (Class->Flat) {
		extra = DAT_0081AF00;
		f = ShapeFlags_Type(f | 0x2000);
	}

	Blit_Block(*LogicalSurface, *ColorSchemes[Class->Color]->Converter, *VoxelDrawSystem::Get_Surface(), region.Bounds, region.Point + drawpoint, cliprect, NULL, ColorSchemes[Class->Color]->Converter->Blitter_From_Flags(f), extra, Class->Flat ? ZGRAD_GROUND : ZGRAD_90DEG, brightness, 0, 0);
}

// FUNCTION: GAMEMD 0x0046B260
void BulletClass::Set_Weapon_Type(WeaponTypeClass * weapon)
{
	WeaponType = weapon;
}

// FUNCTION: GAMEMD 0x0046B270
WeaponTypeClass * BulletClass::Get_Weapon_Type(void) const
{
	return(WeaponType);
}

// FUNCTION: GAMEMD 0x0046B280
void BulletClass::InitScalable(void)
{
	ScalableBullets.Add(this);
	int extra = ScalableBullets.Count() - 5;
	if (extra < 0) {
		extra = 0;
	}
	Class->SetScaledSpawnDelay(Class->SpawnDelay + extra / 3);
}

// FUNCTION: GAMEMD 0x0046B310
void BulletClass::NukeMaker(void)
{
	AbstractClass * target = TarCom;
	CellClass * cptr = &Map[target->Center_Coord().As_Cell()];
	WeaponTypeClass * weapon = Weapons[WeaponTypeClass::From_Name("NukePayload")];

	BulletTypeClass const * type = weapon->Bullet;
	WarheadTypeClass const * warhead = weapon->WarheadPtr;
	int speed = weapon->MaxSpeed;
	BulletClass * bullet = Create_Bullet(type, target, Payback, weapon->Attack, warhead, speed, true);
	if (bullet != NULL) {
		bullet->Limbo();
		bullet->WeaponType = weapon;
		Coord coord = cptr->Center_Coord();
		coord = coord + Coord(0, 0, WeaponType->Bullet->DetonationAltitude);
		TVelocity3D<double> velocity(DIR_N, DIR_N, -1.0);
		bullet->Unlimbo(coord, velocity);
	}
}

// FUNCTION: GAMEMD 0x0046B560
HRESULT STDMETHODCALLTYPE BulletClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BulletClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0046B540
int BulletClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0046B550
RTTIType BulletClass::Fetch_RTTI(void) const
{
	return(RTTI_BULLET);
}

// FUNCTION: GAMEMD 0x0046B5A0
void BulletClass::Assign_Target(AbstractClass * target)
{
	TarCom = target;
}

// FUNCTION: GAMEMD 0x0046B5B0
ObjectTypeClass const * BulletClass::Class_Of(void) const
{
	return(Class);
}

// SYNTHETIC: GAMEMD 0x0046B5C0
// ??_GBulletClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0046B5E0
// ?Clear@?$DynamicVectorClass@PAVBulletClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0046B610
// ?ID@?$DynamicVectorClass@PAVBulletClass@@@@UAEHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B630
// ?GetItem@?$VectorClass@PAVBulletClass@@@@UBEPAVBulletClass@@H@Z
template class DynamicVectorClass<BulletClass *>;

// TEMPLATE: GAMEMD 0x0046B640
// ??0?$CDTimerClass@VFrameTimerClass@@@@QAE@H@Z
template class CDTimerClass<FrameTimerClass>;

// TEMPLATE: GAMEMD 0x0046B660
// ??8?$VectorClass@PAVBulletClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0046B6A0
// ?Resize@?$VectorClass@PAVBulletClass@@@@UAE_NHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B750
// ?Clear@?$VectorClass@PAVBulletClass@@@@UAEXXZ
template class VectorClass<BulletClass *>;

// TEMPLATE: GAMEMD 0x0046B780
// ?ID@?$VectorClass@PAVBulletClass@@@@UAEHABQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B7C0
// ?ID@?$VectorClass@PAVBulletClass@@@@UAEHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B7E0
// ?Resize@?$DynamicVectorClass@PAVBulletClass@@@@UAE_NHPBQAVBulletClass@@@Z
// TEMPLATE: GAMEMD 0x0046B890
// ?ID@?$DynamicVectorClass@PAVBulletClass@@@@UAEHABQAVBulletClass@@@Z
// SYNTHETIC: GAMEMD 0x0046B8C0
// ??_G?$VectorClass@PAVBulletClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0046B910
// ??_G?$DynamicVectorClass@PAVBulletClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0046B960
// ??Y?$TVelocity3D@N@@QAEAAV0@ABV0@@Z
template class TVelocity3D<double>;

// FUNCTION: GAMEMD 0x0046B990
// BulletClass::AddRef

// FUNCTION: GAMEMD 0x0046B9A0
// BulletClass::Release
