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

// FUNCTION: 0x00488F90 $E2
// FUNCTION: 0x00488FC0 $E5
// FUNCTION: 0x00488FE0 $E8
// FUNCTION: 0x00489000 $E11
// FUNCTION: 0x00489020 $E14
// FUNCTION: 0x00489040 $E17
// FUNCTION: 0x00489060 $E20
// FUNCTION: 0x004890A0 $E23
// FUNCTION: 0x004890D0 $E26
// FUNCTION: 0x00489100 $E29
// FUNCTION: 0x00489130 $E32
// FUNCTION: 0x00489140 $E35
// FUNCTION: 0x00489160 $E38

// FUNCTION: 0x00489180
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

// FUNCTION: 0x00489270
void Chain_Reaction_Damage(Cell const & cell)
{
}

// FUNCTION: 0x0048A4F0
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

// FUNCTION: 0x0048A620
void Combat_Lighting(int damage, WarheadTypeClass const * warhead, Coord coord, bool forced, unsigned int disable_flags)
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

// FUNCTION: 0x0048A700
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

// FUNCTION: 0x0048A8D0
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

// FUNCTION: 0x0048A9D0
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

// FUNCTION: 0x0048AB20
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

// FUNCTION: 0x0048AB90
MPHType Calculate_Projectile_Speed(int range, double gravity)
{
	return((MPHType)(int)fastmath::sqrt((double)range * gravity * 1.2));
}

// FUNCTION: 0x0048ABC0
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

// FUNCTION: 0x0048ACE0
int Get_Explosion_Z(Coord coord)
{
	return(-15);
}

// FUNCTION: 0x0048ACF0
double Get_Floater_Gravity(void)
{
	return(Rule->Gravity * 0.5);
}

// FUNCTION: 0x0048AD10 ?$DynamicVectorClass@PAUDamageGroup@@::Clear
// FUNCTION: 0x0048AD40 ?$DynamicVectorClass@PAUDamageGroup@@::ID
template class DynamicVectorClass<DamageGroup *>;

// FUNCTION: 0x0048AD60 ?$VectorClass@PAUDamageGroup@@::GetItem

// FUNCTION: 0x0048AD70 ?$VectorClass@PAUDamageGroup@@::operator==
// FUNCTION: 0x0048ADB0 ?$VectorClass@PAUDamageGroup@@::Resize
// FUNCTION: 0x0048AE60 ?$VectorClass@PAUDamageGroup@@::Clear
// FUNCTION: 0x0048AE90 ?$VectorClass@PAUDamageGroup@@::ID
// FUNCTION: 0x0048AED0 ?$VectorClass@PAUDamageGroup@@::ID
// FUNCTION: 0x0048AEF0 ?$DynamicVectorClass@PAUDamageGroup@@::Resize
// FUNCTION: 0x0048AFA0 ?$DynamicVectorClass@PAUDamageGroup@@::ID
// FUNCTION: 0x0048AFD0 ?$DynamicVectorClass@PAUDamageGroup@@::~?$DynamicVectorClass@PAUDamageGroup@@
// FUNCTION: 0x0048B020 ?$VectorClass@PAUDamageGroup@@::~?$VectorClass@PAUDamageGroup@@
// FUNCTION: 0x0048B070 ?$VectorClass@PAUDamageGroup@@::?$VectorClass@PAUDamageGroup@@
template class VectorClass<DamageGroup *>;

// FUNCTION: 0x0048B0C0 $E41
