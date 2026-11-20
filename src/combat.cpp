// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "combat.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "bound.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "globals.h"
#include "goptions.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "isotype.h"
#include "mouse.h"
#include "overtype.h"
#include "ovrlight.h"
#include "partsys.h"
#include "psystype.h"
#include "rules.h"
#include "scenario.h"
#include "stimer.h"
#include "tactical.h"
#include "team.h"
#include "teamtype.h"
#include "tiberium.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vein.h"
#include "warhead.h"
#include "mph.hh"
#include <windef.h>

// SYNTHETIC: GAMEMD 0x00488F70 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x00488F90 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00488FC0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00488FE0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00489000 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00489020 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00489040 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00489060 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004890A0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004890D0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00489100 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00489130 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00489140 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00489160 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x00489180
int Modify_Damage(int damage, WarheadTypeClass const * warhead, ArmorType armor, int distance)
{
	if (!damage || Scen->Special.IsInert || warhead == NULL) return(0);

	if (damage < 0) {
		if (distance < 8) return(damage);
		return(0);
	}

	float fdamage = (float)damage;
	float fmin = fdamage * warhead->PercentAtMax;
	int spread = (int)(warhead->SpreadFactor * 256.0f);
	if (fmin != fdamage && spread) {
		damage = (int)((fdamage - fmin) * (spread - distance) / spread + fmin);
	}

	damage = MAX(damage, 0);
	damage = (int)(damage * warhead->Modifier[armor]);
	return(MIN(damage, Rule->MaxDamage));
}

// FUNCTION: GAMEMD 0x00489270
void Chain_Reaction_Damage(Cell const & cell)
{
}

extern int const DAT_007ED3D0[];
namespace Unsorted { extern Cell CellSpreadTable[]; }

struct DamageGroup {
	ObjectClass *Target;
	int Distance;
};

// FUNCTION: GAMEMD 0x00489280
int __fastcall Explosion_Damage(
	Coord const &coord,
	int strength,
	TechnoClass *source,
	WarheadTypeClass const *warhead,
	bool chain,
	HouseClass *house)
{
	int distance;
	ObjectClass *object;
	DynamicVectorClass<DamageGroup *> groups;
	int i;

	if (!strength || Scen->Special.IsInert || warhead == 0) {
		return 1;
	}

	int range = (int)(warhead->SpreadFactor * 256.0f);
	bool crush = false;

	if (warhead == Rule->CrushWarhead) {
		crush = true;
	}

	Cell cell = Cell(coord);
	CellClass *cptr = &Map[cell];
	bool isbridge = false;
	bool hit = false;
	bool blocked = false;
	bool close = true;

	if (warhead->SpreadFactor > 0.5f) {
		close = false;
	}

	int cell_height = Map.Get_Height_GL(
		Coord(cell.X * 256 + 128, cell.Y * 256 + 128, 0));

	if (cell_height < coord.Z) {
		AircraftTracker.FillCurrentVector(
			cptr, (int)warhead->SpreadFactor);

		TechnoClass *techno;
		while ((techno = AircraftTracker.Get()) != 0) {
			if (techno->IsActive &&
				techno->IsDown &&
				techno->Strength > 0) {
				distance = coord.Distance_To(techno->PositionCoord);

				if (distance <= range) {
					if (close &&
						distance < 0x55 &&
						techno->IsIronCurtained() &&
						!techno->ForceShielded) {
						blocked = true;
					}

					DamageGroup *group = new DamageGroup;
					group->Target = techno;
					group->Distance =
						coord.Distance_To(techno->PositionCoord);
					groups.Add(group);
				}
			}
		}
	}

	if (cptr->IsUnderBridge &&
		coord.Z > Map.Get_Height_GL(coord) +
			BRIDGE_LEPTON_HEIGHT / 2) {
		isbridge = true;
	}

	int rings = (int)(warhead->SpreadFactor + 0.99);

	for (int m = 0; m < DAT_007ED3D0[rings]; m++) {
		Cell tcell = cell + Unsorted::CellSpreadTable[m];
		cptr = &Map[tcell];

		if (cptr->Overlay != -1) {
			OverlayTypeClass *overlay = OverlayTypes[cptr->Overlay];

			if (overlay->IsChainReaction &&
				(!overlay->IsTiberium ||
				 warhead->IsTiberiumDestroyer) &&
				chain) {
				cptr->Reduce_Tiberium(strength / 10);
			}

			if (overlay->IsWall) {
				if (warhead->WallAbsoluteDestroyer) {
					cptr->Reduce_Wall(-1);
				} else if (
					warhead->IsWallDestroyer ||
					(warhead->IsWoodDestroyer &&
					 overlay->Armor == ARMOR_WOOD)) {
					cptr->Reduce_Wall(strength);
				}
			}

			if (cptr->Overlay == -1) {
				cptr->BecomeUntargetable();
			}
		}

		object = cptr->Cell_Occupier(isbridge);

		for (; object; object = object->Next) {
			if ((object != source ||
				 source->Techno_Type_Class()->DamageSelf ||
				 crush) &&
				object->IsActive) {
				if (object->Fetch_RTTI() == RTTI_UNIT &&
					Scen->Special.IsHarvesterImmune) {
					if (Rule->HarvesterUnit.Is_In_List(
							(UnitTypeClass *)object->Class_Of())) {
						continue;
					}
				}

				DamageGroup *group = new DamageGroup;
				group->Target = object;

				if (object->Fetch_RTTI() == RTTI_BUILDING) {
					if (m == 0) {
						if (coord.Z - cptr->Center_Coord().Z >
							LEVEL_LEPTON_H * 2) {
							group->Distance =
								Distance(coord, cptr->Center_Coord()) -
								LEVEL_LEPTON_H * 2;
						} else {
							group->Distance = 0;
						}
					} else {
						group->Distance =
							Distance(coord, cptr->Center_Coord());
					}
				} else {
					group->Distance =
						Distance(coord, object->Target_Coord());
				}

				if (close && m == 0 && object) {
					if (object->Is_Techno() &&
						((TechnoClass *)object)->IsIronCurtained() &&
						!((TechnoClass *)object)->ForceShielded &&
						group->Distance < 0x55) {
						blocked = true;
					}
				}

				groups.Add(group);
			}
		}
	}

	for (int j = 0; j < groups.Count(); j++) {
		object = groups[j]->Target;
		distance = groups[j]->Distance;

		if (object->IsActive &&
			(object->Fetch_RTTI() != RTTI_BUILDING ||
			 !((BuildingClass *)object)->Class->IsInvisibleInGame)) {
			if (!blocked ||
				(object &&
				 object->Is_Techno() &&
				 ((TechnoClass *)object)->IsIronCurtained())) {
				if (object->Fetch_RTTI() == RTTI_AIRCRAFT && object->In_Air()) {
					distance /= 2;
				}

				if (object->Strength > 0 &&
					object->IsDown &&
					!object->IsInLimbo &&
					distance <= range) {
					int damage = strength;
					object->Take_Damage(
						damage, distance, warhead,
						source, false, false, house);
					hit = true;
				}
			}
		}
	}

	for (int k = 0; k < groups.Count(); k++) {
		delete groups[k];
	}

	groups.Delete_All();

	if (blocked) {
		return 2;
	}

	double force =
		strength * 0.01 < 4.0 ? strength * 0.01 : 4.0;

	if (warhead->IsRocker && force > 0.3) {
		for (int x = cell.X - 3; x <= cell.X + 3; x++) {
			for (int y = cell.Y - 3; y <= cell.Y + 3; y++) {
				object = isbridge
					? Map[Cell(x, y)].Cell_Bridge_Occupier()
					: Map[Cell(x, y)].Cell_Occupier(false);

				for (; object; object = object->Next) {
					TechnoClass *techno =
						Dynamic_Cast<TechnoClass *>(object);

					if (techno) {
						if ((short)x == cell.X &&
							(short)y == cell.Y &&
							source) {
							Coord tcoord = techno->PositionCoord;
							Coord scoord = source->PositionCoord;

							TPoint3D<float> dir(
								(float)(scoord.X - tcoord.X),
								(float)(scoord.Y - tcoord.Y),
								(float)(scoord.Z - tcoord.Z));

							dir = dir.Normalize() * 10.0f;
							tcoord += Coord(
								(int)dir.X, (int)dir.Y, (int)dir.Z);

							techno->vt_entry_3D8(
								tcoord, (float)force, false);
						} else if (warhead->SpreadFactor > 0.0f) {
							techno->vt_entry_3D8(
								coord, (float)force, false);
						}
					}
				}
			}
		}
	}

	bool ion = warhead == Rule->IonCannonWarhead;
	cptr = &Map[cell];

	if (Scen->Special.IsDestroyBridges &&
		warhead->IsWallDestroyer) {
		int ittype =
			cptr->ITType - IsometricTileTypeClass::BridgeSet + 1;

		CellClass *bcptr = 0;
		if (cptr->IsUnderBridge) {
			if (cptr->IsBridgeDeck) {
				bcptr = &Map[cptr->Fetch_CellID()];
			} else {
				bcptr = &Map[cptr->BridgeDeckCell->Fetch_CellID()];
			}
		}

		if (bcptr && bcptr->Is_Overlay_Bridge() ||
			ittype == IsometricTileTypeClass::BridgeMiddle1 + 0 ||
			ittype == IsometricTileTypeClass::BridgeMiddle1 + 1 ||
			ittype == IsometricTileTypeClass::BridgeMiddle1 + 2 ||
			ittype == IsometricTileTypeClass::BridgeMiddle1 + 3 ||
			ittype == IsometricTileTypeClass::BridgeMiddle2 + 0 ||
			ittype == IsometricTileTypeClass::BridgeMiddle2 + 1 ||
			ittype == IsometricTileTypeClass::BridgeMiddle2 + 2 ||
			ittype == IsometricTileTypeClass::BridgeMiddle2 + 3) {
			if (!cptr->IsUnderBridge ||
				(coord.Z <= BRIDGE_LEPTON_HEIGHT +
					LEVEL_LEPTON_H * (cptr->Height + 1) &&
				 coord.Z > BRIDGE_LEPTON_HEIGHT +
					LEVEL_LEPTON_H * (cptr->Height - 2))) {
				if (warhead->IsWallDestroyer &&
					(warhead == Rule->IonCannonWarhead ||
					 Random_Pick(1, Rule->BridgeStrength) < strength)) {
					bool damaged = Map.Damage_Bridge(cell);
					int tries = 3;

					while (!damaged && ion && tries > 0) {
						damaged = Map.Damage_Bridge(cell);
						tries--;
					}

					if (damaged) {
						cptr->BecomeUntargetable();
					}

					Point2D point;
					TacticalMap->Coord_To_Pixel(coord, point);
					TacticalMap->Register_Dirty_Area(
						Rect(point.X - 128, point.Y - 128, 256, 256),
						false);
				}
			}
		}

		int ittype2 =
			cptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1;

		if (bcptr && bcptr->Is_Overlay_Rail_Bridge() ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 0 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 1 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 2 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 3 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 0 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 1 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 2 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 3) {
			if (!cptr->IsUnderBridge ||
				(coord.Z <= BRIDGE_LEPTON_HEIGHT +
					LEVEL_LEPTON_H * (cptr->Height + 1) &&
				 coord.Z > BRIDGE_LEPTON_HEIGHT +
					LEVEL_LEPTON_H * (cptr->Height - 2))) {
				if (warhead->IsWallDestroyer &&
					(warhead == Rule->IonCannonWarhead ||
					 Random_Pick(1, Rule->BridgeStrength) < strength)) {
					bool damaged = Map.Damage_Bridge(cell);
					int tries = 3;

					while (!damaged && ion && tries > 0) {
						damaged = Map.Damage_Bridge(cell);
						tries--;
					}

					if (damaged) {
						cptr->BecomeUntargetable();
					}

					Point2D point;
					TacticalMap->Coord_To_Pixel(coord, point);
					TacticalMap->Register_Dirty_Area(
						Rect(point.X - 96, point.Y - 96, 192, 192),
						false);
				}
			}
		}

		if (cptr->Overlay >= OVERLAY_LOWBRIDGE_01 && cptr->Overlay <= OVERLAY_LOWBRIDGE_26) {
			if (warhead == Rule->IonCannonWarhead ||
				Random_Pick(1, Rule->BridgeStrength) < strength) {
				if (Map.Damage_Low_Bridge(cell)) {
					cptr->BecomeUntargetable();
				}
			}
		}

		if (cptr->Overlay >= 0xCD && cptr->Overlay <= 0xE6) {
			if (warhead == Rule->IonCannonWarhead ||
				Random_Pick(1, Rule->BridgeStrength) < strength) {
				if (Map.FUN_0057CCF0(cell)) {
					cptr->BecomeUntargetable();
				}
			}
		}
	}

	if (cptr->Overlay != -1 &&
		OverlayTypes[cptr->Overlay]->IsExplosive) {
		cptr->Register_For_Redraw();
		cptr->Overlay = OVERLAY_NONE;
		cptr->Recalc_Attributes(-1);
		Map.ResetZones(cptr->CellID);
		Map.Update_Cell_Subzones(cptr->CellID);
		cptr->BecomeUntargetable();

		new AnimClass(
			Rule->BarrelExplode, coord, 0, 1,
			ShapeFlags_Type(SHAPE_WIN_REL | SHAPE_CENTER),
			0, false);

		Explosion_Damage(
			coord, Rule->AmmoCrateDamage, (TechnoClass *)0,
			Rule->C4Warhead, true, house);

		for (i = 0; i < Rule->BarrelDebris.Count(); i++) {
			if (Percent_Chance(15)) {
				new VoxelAnimClass(Rule->BarrelDebris[i], coord, 0);
				break;
			}
		}

		if (Percent_Chance(25)) {
			(new ParticleSystemClass(
				Rule->BarrelParticle, coord, 0, 0, COORD_NONE))
				->Spawn_Held_Particle(coord, coord);
		}
	}

	if (warhead->Particle != 0) {
		ParticleSystemClass *psys = new ParticleSystemClass(
			warhead->Particle, coord, 0, 0, COORD_NONE, house);
		psys->Spawn_Held_Particle(coord, coord);
	}

	return !hit;
}


// FUNCTION: GAMEMD 0x0048A4F0
AnimTypeClass const * Combat_Anim(int damage, WarheadTypeClass const * warhead, LandType land, Coord const & coord)
{
	if (damage == 0 || warhead == NULL) {
		return(NULL);
	}

	if (land == LAND_WATER && warhead->IsConventional && !Map[coord].IsUnderBridge && coord.Z < Map.Get_Height_GL(coord) + LEVEL_LEPTON_H * 2) {
		if (Rule->SplashList.Count()) {
			int val = MIN(damage, 35 * Rule->SplashList.Count() - 1);
			return(Rule->SplashList[val / 35]);
		}
		return(NULL);
	}

	if (warhead == Rule->LightningWarhead) {
		return(Rule->WeatherConBoltExplosion);
	}

	if (warhead->ExplosionSet.Count()) {
		if (warhead->IsEMEffect) {
			return(warhead->ExplosionSet[Random_Pick(0, warhead->ExplosionSet.Count() - 1)]);
		}
		int val = MIN(damage, 25 * warhead->ExplosionSet.Count() - 1);
		return(warhead->ExplosionSet[val / 25]);
	}

	return(NULL);
}

namespace FPSCounter { extern unsigned CurrentFrameRate; }
namespace Detail { unsigned GetMinFrameRate(void); }

// FUNCTION: GAMEMD 0x0048A620
void Combat_Lighting(Coord coord, int damage, WarheadTypeClass const * warhead, bool forced, unsigned int disable_flags)
{
	if ((Options.DetailLevel && FPSCounter::CurrentFrameRate >= Detail::GetMinFrameRate()) || (disable_flags & 0xF)) {
		if (forced || warhead != NULL && warhead->IsBright) {
			int size;
			float scale = warhead->CombatLightSize;
			if (scale > 0.0f) {
				if (scale > 1.0f) scale = 1.0f;
				size = (int)(scale * 63.0f);
			} else {
				size = (damage << 6) >> 8;
				if (size >= 63) {
					size = 63;
				} else if (size <= 21) {
					size = 21;
				}
			}
			SpotLightClass * light = new SpotLightClass(coord, size);
			light->Disable(disable_flags);
		}
	}
}

// FUNCTION: GAMEMD 0x0048A700
void Wide_Area_Damage(Coord const & coord, LEPTON radius, int rawdamage, TechnoClass * source, WarheadTypeClass const * warhead)
{
	if (warhead == NULL) return;

	int cell_radius = (radius + CELL_LEPTON_W-1) / CELL_LEPTON_W;
	Cell cell = coord.As_Cell();

	for (int x = -cell_radius; x <= cell_radius; x++) {
		for (int y = -cell_radius; y <= cell_radius; y++) {
			int xpos = cell.X + x;
			int ypos = cell.Y + y;
			if ((unsigned)xpos > MAP_CELL_W) {
				continue;
			}
			if ((unsigned)ypos > MAP_CELL_H) {
				continue;
			}
			Cell tcell = Cell(xpos, ypos);
			if (!Map.In_Radar(tcell)) continue;

			int dist_from_center = Distance(Coord(x+cell_radius, y+cell_radius), Coord(cell_radius, cell_radius));
			double damagemod = 1;
			if (dist_from_center > 0) {
				damagemod = 1.0 / ((double)cell_radius / dist_from_center);
			}
			Coord tcoord(tcell, coord.Z);
			Explosion_Damage(tcoord, rawdamage * damagemod, source, warhead);
		}
	}
}

// FUNCTION: GAMEMD 0x0048A8D0
bool Calculate_Projectile_Pitch(bool high_arc, int speed, int distance, int height, double gravity, DirType & pitch)
{
	double angle;
	if (Calculate_Projectile_Angle(high_arc, speed, distance, height, gravity, angle)) {
		if (high_arc) {
			pitch = DirType(angle);
			return(true);
		}

		double test;
		Calculate_Projectile_Angle(false, speed, distance + 1, height, gravity, test);
		if (test < angle) {
			pitch = DirType(-angle);
		} else {
			pitch = DirType(angle);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0048A9D0
bool Calculate_Projectile_Angle(bool high_arc, int speed, int distance, int height, double gravity, double & angle)
{
	if (height > 0) {
		angle = 0.7853822499999998;
		return(true);
	}

	if ((double)distance == 0.0) {
		distance = 0;
	}

	double dx = (double)distance;
	double velocity = (double)speed;
	double dy = (double)height;
	double v = velocity;

	double vsq = v * v;
	double base = vsq - ((double)height * gravity);
	double g2 = gravity * gravity;
	double dx2 = (dx * dx);

	double value = (v * v * vsq)
				 - (2 * vsq * dy * gravity)
				 - (g2 * dx2);

	double denominator = (((dy * dy) / dx2) + 1.0) * 2;

	if (value < 0.0 || denominator == 0.0) {
		return(false);
	}

	double numerator = !high_arc ? base + fastmath::sqrt(value)
								  : base - fastmath::sqrt(value);

	value = numerator / denominator;

	if (value >= 0.0) {
		angle = fastmath::acos(fastmath::sqrt(value) / v);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0048AB20
bool Calculate_Projectile_Dir(int speed, int distance, int height, double gravity, DirType & dir)
{
	double angle;
	if (Calculate_Projectile_Angle(false, speed, distance, height, gravity, angle)) {
		if (angle < 0.0) {
			angle += M_PI * 2;
		}
		dir = DirType(angle);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0048AB90
MPHType Calculate_Projectile_Speed(int range, double gravity)
{
	return((MPHType)(int)fastmath::sqrt((double)range * gravity * 1.2));
}

// FUNCTION: GAMEMD 0x0048ABC0
bool Is_Projectile_Trajectory_Valid(int speed, int distance, int height, double gravity)
{
	double v = (double)speed;
	double dx = (double)distance;
	if (dx == 0.0) dx = 0.001;
	double dy = (double)height;

	double y2 = dy * dy;
	double vsq = v * v;
	double g2 = gravity * gravity;
	double x2 = dx * dx;

	double discriminant = (v * v * vsq)
						- (2 * vsq * dy * gravity)
						- (g2 * x2);

	if (discriminant < 0.0) {
		return(false);
	}

	double denominator = 2 * ((y2 / x2) + 1.0);
	if (denominator == 0.0) {
		return(false);
	}

	double base = vsq - (dy * gravity);

	if (((base + fastmath::sqrt(discriminant)) / denominator) >= 0.0 ||
		((base - fastmath::sqrt(discriminant)) / denominator) >= 0.0) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0048ACE0
int Get_Explosion_Z(Coord coord)
{
	return(-15);
}

// FUNCTION: GAMEMD 0x0048ACF0
double Get_Floater_Gravity(void)
{
	return(Rule->Gravity * 0.5);
}

// TEMPLATE: GAMEMD 0x0048AD10
// ?Clear@?$DynamicVectorClass@PAUDamageGroup@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0048AD40
// ?ID@?$DynamicVectorClass@PAUDamageGroup@@@@UAEHPBQAUDamageGroup@@@Z
template class DynamicVectorClass<DamageGroup *>;

// TEMPLATE: GAMEMD 0x0048AD60
// ?GetItem@?$VectorClass@PAUDamageGroup@@@@UBEPAUDamageGroup@@H@Z

// TEMPLATE: GAMEMD 0x0048AD70
// ??8?$VectorClass@PAUDamageGroup@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0048ADB0
// ?Resize@?$VectorClass@PAUDamageGroup@@@@UAE_NHPBQAUDamageGroup@@@Z
// TEMPLATE: GAMEMD 0x0048AE60
// ?Clear@?$VectorClass@PAUDamageGroup@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0048AE90
// ?ID@?$VectorClass@PAUDamageGroup@@@@UAEHABQAUDamageGroup@@@Z
// TEMPLATE: GAMEMD 0x0048AED0
// ?ID@?$VectorClass@PAUDamageGroup@@@@UAEHPBQAUDamageGroup@@@Z
// TEMPLATE: GAMEMD 0x0048AEF0
// ?Resize@?$DynamicVectorClass@PAUDamageGroup@@@@UAE_NHPBQAUDamageGroup@@@Z
// TEMPLATE: GAMEMD 0x0048AFA0
// ?ID@?$DynamicVectorClass@PAUDamageGroup@@@@UAEHABQAUDamageGroup@@@Z
// SYNTHETIC: GAMEMD 0x0048AFD0
// ??_G?$VectorClass@PAUDamageGroup@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0048B020
// ??_G?$DynamicVectorClass@PAUDamageGroup@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0048B070
// ??0?$VectorClass@PAUDamageGroup@@@@QAE@HPBQAUDamageGroup@@@Z
template class VectorClass<DamageGroup *>;
