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
#include "cast.h"

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
// _$E171

// FUNCTION: GAMEMD 0x004666C0
bool BulletClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

#include "revent.h"
#include "voxel.h"

void FUN_0053AB70(void);

static inline int Coord_Dist(Coord const & a, Coord const & b);

static inline Coord Ballistic_Move(TVelocity3D<double> & position, TVelocity3D<double> & velocity, double gravity, double elasticity, double max_velocity, TechnoClass * payback, ImpactType & impact, bool & forced)
{
	velocity.Z -= gravity;
	if (max_velocity > 0.0) {
		double speed = velocity.Speed();
		if (speed > max_velocity) {
			velocity *= (float)(speed / speed);
		}
	}
	Coord old_coord(position.X, position.Y, position.Z);
	position += velocity;
	Coord new_coord(position.X, position.Y, position.Z);
	int height = Map.Get_Height_GL(new_coord);
	int bridge_height = height + BRIDGE_LEPTON_HEIGHT;
	CellClass * cellptr = &Map[new_coord];
	bool rose_through_bridge;
	bool fell_through_bridge;
	fell_through_bridge = false;
	rose_through_bridge = false;
	if (cellptr->IsUnderBridge || Map[old_coord].IsUnderBridge) {
		if (new_coord.Z >= bridge_height) {
			if (old_coord.Z < bridge_height) {
				rose_through_bridge = true;
			}
		} else if (old_coord.Z >= bridge_height) {
			fell_through_bridge = true;
		}
	}
	bool hit_obstacle = false;
	if (!fell_through_bridge && !rose_through_bridge) {
		if (position.Z >= height && position.Z - 150 < height) {
			BuildingClass * building = cellptr->Cell_Building();
			hit_obstacle = building != NULL || cellptr->Has_Wall_Or_Gate();
			if (hit_obstacle) {
				if (building != NULL) {
					if (building == payback || building->Class->IsLaserFence && building->LaserFenceFrame >= 8) {
						hit_obstacle = false;
					}
					if (building->IsStrange()) {
						hit_obstacle = false;
					}
					if (payback != NULL && payback->House->Is_Ally(building)) {
						hit_obstacle = false;
					}
				}
			}
		}
	}
	bool do_bounce = true;
	if (position.Z >= height) {
		if (!fell_through_bridge && !rose_through_bridge && !hit_obstacle) {
			do_bounce = false;
		}
	}
	if (do_bounce) {
		bool contact = true;
		if (payback == NULL) {
			if (fell_through_bridge) {
				position.Z = bridge_height;
			} else if (rose_through_bridge) {
				bridge_height -= 20;
				position.Z = bridge_height;
			} else {
				bridge_height = height - 100;
				if (bridge_height < position.Z) {
					position.Z = height;
				}
			}
			Matrix3D slope_matrix = Get_Slope_Matrix(TacticalMap->Get_Cell_Ramp(new_coord));
			Matrix3D slope_matrix_inv = Matrix3D::Orthogonal_Inverse(slope_matrix);
			Vector3 vec(velocity.X, -velocity.Y, velocity.Z);
			vec = slope_matrix_inv.Rotate_Vector(vec);
			vec *= elasticity;
			vec.Z = -vec.Z;
			vec = slope_matrix.Rotate_Vector(vec);
			velocity.X = vec.X;
			velocity.Y = -vec.Y;
			velocity.Z = vec.Z;
		} else {
			if (fell_through_bridge) {
				position.Z = bridge_height;
			} else if (rose_through_bridge) {
				bridge_height -= 20;
				position.Z = bridge_height;
			} else {
				contact = false;
			}
		}
		if (contact) {
			impact = IMPACT_NORMAL;
			forced = true;
		}
	}
	Coord result;
	result.X = (int)position.X;
	result.Y = (int)position.Y;
	result.Z = (int)position.Z;
	return(result);
}

static inline void Set_Horizontal_Velocity(TVelocity3D<double> & velocity,
		double const & x, double const & y)
{
	velocity.X = x;
	velocity.Y = y;
}

// FUNCTION: GAMEMD 0x004666E0
void BulletClass::AI(void)
{
	static const int _limit = 4 * TICKS_PER_SECOND;
	Coord coord;
	int distance;

	BASECLASS::AI();
	if (!IsActive) return;

	if (SpawnNextAnim) {
		if (NextAnim != NULL) return;
		AnimPtrTracker.Delete(this);
		SpawnNextAnim = false;
		Bullet_Explodes(false);
		Delete_Me();
		return;
	}

	bool collided = false;
	bool forced = false;
	if (Class->IsDropping && !IsFalling) {
		forced = true;
	}
	if (Class->AnimLow || Class->AnimHigh) {
		AnimRate--;
		if (AnimRate == 0) {
			AnimRate = Class->AnimRate;
			AnimFrame++;
			if (AnimFrame > Class->AnimHigh) {
				AnimFrame = Class->AnimLow;
			}
		}
	}
	coord = PositionCoord;
	if (Class->Trailer != NULL) {
		if (Class->ScaledSpawnDelay) {
			int trailer_phase = Frame % Class->ScaledSpawnDelay;
			if (trailer_phase == 0) {
				new AnimClass(Class->Trailer, coord, 1, 1);
			}
		} else {
			int trailer_phase = Frame % Class->SpawnDelay;
			if (trailer_phase == 0) {
				new AnimClass(Class->Trailer, coord, 1, 1);
			}
		}
	}

	ImpactType impact = IMPACT_NONE;
	if (Class->ROT > 0) {
		int max_speed = MaxSpeed;
		double speed = Velocity.Magnitude();
		if (Class->CourseLockDuration == 0) {
			if (MaxSpeed >= 40 || speed + 0.5 >= max_speed) {
				IsLaunching = false;
			}
		} else {
			if (CourseLockCounter < Class->CourseLockDuration) {
				CourseLockCounter++;
				if (CourseLockCounter >= Class->CourseLockDuration) {
					IsLaunching = false;
				}
			}
		}
		int acceleration = Class->Acceleration;
		if (IsLaunching && Class->CourseLockDuration == 0) {
			acceleration = (Frame % 2 == 0) ? 1 : 0;
		}
		if (speed < max_speed) {
			speed += acceleration;
			if (speed >= max_speed) {
				speed = max_speed;
			}
			Velocity.Set_Speed(speed);
		} else if (speed > max_speed) {
			speed -= acceleration / 2;
			if (speed <= 0) {
				speed = 0;
			}
			Velocity.Set_Speed(speed);
		}
		TVelocity3D<double> velocity = Velocity;
		Coord target_coord = TarCom != NULL ? TarCom->As_Coord() : COORD_NONE;
		if (TarCom != NULL && (TarCom->AbstractFlags & 2)) {
			target_coord = ((ObjectClass *)TarCom)->Target_Coord();
		}
		double & rotvar = Rule->MissileROTVar;
		double phase = ((Frame + Fetch_ID()) % TICKS_PER_SECOND) * (1.0 / TICKS_PER_SECOND);
		int rot = (fastmath::sin(phase * DEG_TO_RAD(360)) * rotvar + (rotvar + 1)) * Class->ROT;
		if (Center_Coord().Distance_To(target_coord) < CELL_LEPTON) {
			rot *= 1.5;
		}
		Coord old_coord = coord;
		int is_aircraft;
		if (TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_AIRCRAFT) {
			is_aircraft = true;
		} else {
			is_aircraft = false;
		}
		distance = Projectile_Motion(coord, velocity, target_coord, DirType((Dir256)(IsLaunching == false ? rot : 0)), (bool &)is_aircraft, Class->IsAirburst, Class->IsVeryHigh, Class->Level);
		CellClass * cellptr = &Map[coord];
		TVelocity3D<double> new_velocity = velocity;
		Velocity = new_velocity;
		double half_speed = Velocity.Magnitude() * 0.5;
		if (distance <= half_speed || HeightAGL <= 0) {
			forced = true;
			impact = IMPACT_NORMAL;
			if (HeightAGL > 0 && !Class->IsAirburst && target_coord != COORD_NONE) {
				coord = target_coord;
			}
		}
		if (target_coord == COORD_NONE && HeightAGL >= Rule->MissileSafetyAltitude) {
			forced = true;
			impact = IMPACT_NORMAL;
		}
		int delta = Coord_Dist(old_coord, target_coord) - Coord_Dist(coord, target_coord);
		if (!IsLaunching) {
			if (ClosureSamples < _limit) {
				ClosureSamples++;
				SmoothedClosure += delta;
			} else {
				SmoothedClosure = SmoothedClosure * ((4 * TICKS_PER_SECOND - 1.0) / (4 * TICKS_PER_SECOND)) + delta;
				if (SmoothedClosure >= 0 && SmoothedClosure < _limit) {
					if (!Class->IsAirburst && !Class->IsVeryHigh) {
						forced = true;
						impact = IMPACT_NORMAL;
					}
				}
			}
		}
		if (impact == IMPACT_NONE && (cellptr->IsUnderBridge || Map[old_coord].IsUnderBridge)) {
			int ground = Map.Get_Height_GL(coord);
			int bridge_height = ground + BRIDGE_LEPTON_HEIGHT;
			if (coord.Z > bridge_height && old_coord.Z < bridge_height) {
				impact = IMPACT_NORMAL;
				coord.Z = bridge_height;
				forced = true;
			} else if (coord.Z < bridge_height && old_coord.Z > bridge_height) {
				impact = IMPACT_NORMAL;
				coord.Z = bridge_height;
				forced = true;
			}
		}
	} else {
		TVelocity3D<double> new_velocity;
		TVelocity3D<double> const & previous = Velocity;
		TVelocity3D<double> captured(previous, previous.Z);
		new_velocity = captured;
		TVelocity3D<double> velocity;
		Set_Horizontal_Velocity(velocity, captured.X, captured.Y);
		velocity.Z = captured.Z;
		if (new_velocity.Speed() < 8) {
			impact = IMPACT_NORMAL;
		}
		TVelocity3D<double> position(coord.X, coord.Y, coord.Z);
		double gravity = Rule->Gravity;
		if (Class->IsFloater) {
			gravity = Get_Floater_Gravity();
		}
		if (Class->Vertical) {
			int speed = Velocity.Speed();
			if (speed < MaxSpeed) {
				Velocity.Set_Speed(speed + Class->Acceleration);
			}
			TVelocity3D<double> move_velocity = Velocity;
			Point3D displacement(move_velocity.X, move_velocity.Y, move_velocity.Z);
			Coord old_coord = coord;
			coord = old_coord + displacement;
			if (coord.Z > Class->DetonationAltitude) {
				impact = IMPACT_NORMAL;
				forced = true;
			} else if (HeightAGL < 0) {
				impact = IMPACT_NORMAL;
				forced = true;
			} else {
				int ground = Map.Get_Height_GL(coord);
				int bridge_height = ground + BRIDGE_LEPTON_HEIGHT;
				if (Map[coord].IsUnderBridge || Map[old_coord].IsUnderBridge) {
					if (coord.Z >= bridge_height) {
						if (old_coord.Z < bridge_height) {
							impact = IMPACT_NORMAL;
							forced = true;
						}
					} else if (old_coord.Z >= bridge_height) {
						impact = IMPACT_NORMAL;
						forced = true;
					}
				}
			}
		} else {
			coord = Ballistic_Move(position, velocity, gravity, Class->Elasticity, 0.0, Payback, impact, forced);
		}

		Cell cell = coord.As_Cell();
		Cell target_cell = TargetCoords.As_Cell();
		if (cell == target_cell && !Class->Vertical && HeightAGL < LEVEL_LEPTON_H * 2) {
			impact = IMPACT_NORMAL;
			forced = true;
			collided = true;
		} else {
			CellClass * cellptr = &Map[cell];
			CellClass * target_cellptr = &Map[target_cell];
			BuildingClass * building = cellptr->Cell_Building();
			BuildingClass * target_building = target_cellptr->Cell_Building();
			if (building != NULL && !Class->Vertical && building == target_building && HeightAGL < LEVEL_LEPTON_H * 2) {
				impact = IMPACT_NORMAL;
				forced = true;
				collided = true;
			} else {
				TechnoClass * techno = Map[coord].Cell_Techno();
				bool has_techno = techno != NULL;
				bool has_payback = Payback != NULL;
				bool is_payback = has_payback && techno == Payback;
				bool is_ally = has_techno && has_payback && Payback->House->Is_Ally(techno);
				bool nearby = has_techno && Coord_Dist(coord, techno->PositionCoord) < CELL_LEPTON / 2;
				if (!is_payback && !is_ally && nearby) {
					impact = IMPACT_NORMAL;
					forced = true;
					if (!Class->IsInaccurate) {
						coord = techno->PositionCoord;
					}
				} else if (!Map.In_Radar(coord)) {
					impact = IMPACT_EDGE;
					forced = true;
					coord = PositionCoord;
				} else {
					Set_Horizontal_Velocity(new_velocity, velocity.X, velocity.Y);
					if (!Class->Vertical) {
						TVelocity3D<double> updated_velocity = new_velocity;
						updated_velocity.Z = velocity.Z;
						Velocity = updated_velocity;
					}
					if (Velocity.Speed() < 10 && HeightAGL < 10) {
						impact = IMPACT_NORMAL;
						forced = true;
					}
				}
			}
		}
	}

	switch (impact) {
		case IMPACT_EDGE:
			Mark();
			Delete_Me();
			break;
		default:
		case IMPACT_NONE:
		case IMPACT_NORMAL: {
			Mark();
			PositionCoord = coord;
			if (!forced) {
				Coord cur_coord = PositionCoord;
				forced = Is_Forced_To_Explode(cur_coord);
				PositionCoord = cur_coord;
			}
			if (forced && HeightAGL < 0) {
				HeightAGL = 0;
			}
			FuseResultType fuse = (Is_Homing() || Class->IsFueled) ? Fuse.Fuse_Checkup(coord) : FUSE_WAIT;
			if (Payback != NULL && Payback->Techno_Type_Class()->JumpJet && fuse == FUSE_EXPLODE_FAR) {
				fuse = FUSE_EXPLODE_CLOSE;
			}
			if (!forced && (Class->IsDropping || fuse == FUSE_WAIT)) {
				if (Class->IsDegenerate && Strength > 5) {
					Strength--;
				}
			} else {
				if (TarCom != NULL) {
					if ((fuse == FUSE_EXPLODE_CLOSE || collided) && !Class->IsAirburst && !Class->IsInaccurate) {
						Coord midpoint = coord;
						Coord target_coord = TarCom->As_Coord();
						midpoint.Z = (midpoint.Z + target_coord.Z) / 2;
						int target_distance = midpoint.Distance_To(target_coord);
						if (collided) {
							target_distance /= 3;
						}
						if (fuse == FUSE_EXPLODE_CLOSE || target_distance <= MAX(CELL_LEPTON / 2, Velocity.Speed() * 2)) {
							PositionCoord = TarCom->Center_Coord();
						}
					}
				}
				if (*Warhead == "NUKE") {
					if (HeightAGL < 0) {
						HeightAGL = 0;
					}
					FUN_0053AB70();
					RadarEventClass::Create(RADAREVENT_SUPERWEAPON_ACTIVATED, PositionCell);
					AnimType anim = AnimTypeClass::From_Name("NUKEBALL");
					if (anim != ANIM_NONE) {
						AnimTypeClass * anim_type = AnimTypes[anim];
						if (anim_type != NULL) {
							NextAnim = new AnimClass(anim_type, PositionCoord, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(PositionCoord));
							SpawnNextAnim = true;
							PointerExpiredNotification::NotifyInvalidAnim.Add(this);
							break;
						}
					}
				}
				Bullet_Explodes(forced);
				Delete_Me();
			}
			break;
		}
	}
	LastMapCoords = coord.As_Cell();
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
	Cell cell = TarCom->Center_Coord();
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
			for (int i = 0; i < ScalableBullets.Count(); i++) {
				if (bullet == ScalableBullets[i]) {
					ScalableBullets.Delete(bullet);
					return;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00468670
bool BulletClass::Unlimbo(Coord const & coord, TVelocity3D<double> const & velocity)
{
	if (BASECLASS::Unlimbo(coord)) {
		Velocity = velocity;
		SourceCoords = coord;
		LastMapCoords = SourceCoords.As_Cell();
		Map.Remove(this);

		Coord tcoord = TarCom->As_Coord();
		TargetCoords = tcoord;

		if (Class->FlakScatter && Class->IsInvisible) {
			TPoint3D<float> delta;
			delta.X = coord.X - tcoord.X;
			delta.Y = tcoord.Y - coord.Y;
			float vertical = coord.Z - tcoord.Z;
			delta.Z = vertical;
			float distance = delta.Length();
			int scatter = Random_Pick(0, Rule->BallisticScatter * 2);
			scatter *= (int)distance;
			scatter /= WeaponType->Range;
			double scatter_angle = Random_Double(0.0, M_PI * 2.0);
			tcoord = Move_Coord(tcoord, DirType(scatter_angle), scatter);
		}

		if (Class->IsInvisible) {
			HouseClass * house = Payback != NULL ? Payback->House : NULL;
			Coord firestorm_coord = Map.Firestorm_On_Path(coord, tcoord, house);
			if (firestorm_coord != COORD_NONE) {
				firestorm_coord.Z = Map.Get_Height_GL(firestorm_coord);
				PositionCoord = firestorm_coord;
			} else {
				CellClass * obstacle = TrajectoryHelper::FindFirstObstacle(coord, tcoord, Class, house);
				if (obstacle != NULL) {
					PositionCoord = obstacle->Center_Coord();
				} else {
					PositionCoord = tcoord;
				}
				// The target position replaces the obstacle position.
				PositionCoord = tcoord;
				MaxSpeed = 0;
				Velocity.Set_Speed(0);
			}
		}

		Fuse.Arm_Fuse(PositionCoord, tcoord, As_Aircraft(TarCom) != NULL ? 0 : Class->Arming);
		if (Is_Homing()) {
			Velocity.Set_Speed(1);
		}
		if (IsActive) {
			Map.Submit(this);
		}
		return(true);
	}
	return(false);
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
	return(Coord(a - b).Length());
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

static inline ObjectClass * Object_Of(AbstractClass * target)
{
	return(target == NULL ? NULL : ((target->AbstractFlags & 2) ? (ObjectClass *)target : NULL));
}

static inline bool Is_Foot_Object(AbstractClass const * target)
{
	return(target != NULL && (target->AbstractFlags & 4));
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
		BombListInstance.Plant(Payback, As_Techno(TarCom));
	} else if (Warhead->ElectricAssault) {
		if (TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_BUILDING && Payback != NULL && Payback->Fetch_RTTI() == RTTI_INFANTRY) {
			((BuildingClass *)TarCom)->FUN_00452820((InfantryClass *)Payback);
		}
	} else if (Warhead->Parasite) {
		if (Payback != NULL) {
			((FootClass *)Payback)->ParasiteImUsing->TryInfect(As_Foot(TarCom));
		}
	} else if (Warhead->Temporal) {
		if (Payback != NULL && TarCom != NULL) {
			if (!(TarCom->AbstractFlags & 4) || As_Foot(TarCom)->In_Which_Layer() == LAYER_GROUND) {
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
			if (!Is_Foot_Object(Payback) || !As_Foot(Payback)->IsAttackedByLocomotor) {
				bool infactory = false;
				if (TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_UNIT) {
					TechnoClass * contact = As_Unit(TarCom)->FUN_0065AD30(0);
					if (contact != NULL && contact->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)contact)->Class->IsWeaponsFactory) {
						if (Map[As_Unit(TarCom)->Get_Coord()].Cell_Building() == contact) {
							infactory = true;
						}
					}
				}

				if (Payback->LocomotorTarget != NULL) {
					Payback->ReleaseLocomotor(false);
				}

				if (TarCom != NULL && !infactory) {
					if (!Is_Foot_Object(TarCom) || As_Foot(TarCom)->In_Which_Layer() == LAYER_GROUND) {
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
			if (Payback->Airstrike->FUN_0041D7E0(As_Techno(TarCom))) {
				Payback->Airstrike->StartMission(As_Techno(TarCom));
			}
		}
	} else if (Warhead->DirectRocker && TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_UNIT) {
		TechnoClass * techno = As_Techno(TarCom);
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

extern int DAT_007ED3D0[];
namespace Unsorted { extern Cell CellSpreadTable[]; }

// FUNCTION: GAMEMD 0x0046A310
void BulletClass::Shrapnel(void)
{
	int configured = Class->ShrapnelCount;
	int count;
	if (configured < 0) {
		if (Payback == NULL) {
			count = 3;
		} else {
			count = -configured - ::Distance(Payback->Center_Coord(), PositionCoord) / CELL_LEPTON;
			if (count <= 0) {
				return;
			}
		}
	} else {
		count = configured;
	}

	if (Get_Cell_Ptr()->Cell_Occupier() == NULL ||
		Get_Cell_Ptr()->Cell_Occupier()->Fetch_RTTI() == RTTI_BUILDING) {
		return;
	}

	WeaponTypeClass * weapon = Class->ShrapnelWeapon;
	BulletTypeClass const * type = weapon->Bullet;
	Cell center = Get_Cell();
	int range = weapon->Range / CELL_LEPTON;
	int fired = 0;
	TVelocity3D<double> velocity;
	for (int radius = 1; radius <= range; radius++) {
		int end = DAT_007ED3D0[radius];
		for (int index = DAT_007ED3D0[radius - 1]; index < end; index++) {
			CellClass * cellptr = &Map[center + Unsorted::CellSpreadTable[index]];
			ObjectClass * object = cellptr->Cell_Occupier();
			if (object != NULL && object != Payback && Payback != NULL &&
				!Payback->House->Is_Ally(object)) {
				BulletClass * bullet = Create_Bullet(type, object, Payback,
					weapon->Attack, weapon->WarheadPtr, weapon->MaxSpeed, weapon->IsBright);
				if (bullet != NULL) {
					velocity.Set(0, 0, 0);
					velocity.Set_Yaw(::Direction(PositionCoord, object->Center_Coord()));
					velocity.Set_Pitch(DirType(DIR_NE));
					velocity.Set_Speed(weapon->MaxSpeed);
					bullet->Unlimbo(PositionCoord, velocity);
					Coord source;
					source = PositionCoord;
					if (weapon->IsLaser && Payback != NULL) {
						Payback->CreateLaser(object, 0, weapon, source);
					}
					if (weapon->IsElectricBolt && Payback != NULL) {
						Payback->CreateEBolt(object, weapon, source);
					}
				}
				fired++;
				if (fired == count) {
					return;
				}
			}
		}
	}

	for (; fired < count; fired++) {
		Cell cell = Get_Cell();
		cell.X += Random_Pick(-2, 2);
		cell.Y += Random_Pick(-2, 2);
		BulletClass * bullet = Create_Bullet(type, &Map[cell], Payback,
			weapon->Attack, weapon->WarheadPtr, weapon->MaxSpeed, weapon->IsBright);
		if (bullet != NULL) {
			velocity.Set(0, 0, 0);
			CellClass & destination = Map[cell];
			velocity.Set_Yaw(::Direction(PositionCoord, destination.Center_Coord()));
			velocity.Set_Speed(weapon->MaxSpeed);
			velocity.Set_Pitch(DirType(DIR_NE));
			bullet->Unlimbo(PositionCoord, velocity);
			Coord source;
			source = PositionCoord;
			if (weapon->IsLaser && Payback != NULL) {
				Payback->CreateLaser(&Map[cell], 0, weapon, source);
			}
			if (weapon->IsElectricBolt && Payback != NULL) {
				Payback->CreateEBolt(&Map[cell], weapon, source);
			}
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
