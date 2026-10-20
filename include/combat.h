// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"

#include "armor.hh"
#include "land.hh"
#include "mph.hh"

class WarheadTypeClass;
class AnimTypeClass;
class TechnoClass;
class Coord;
class DirType;
class HouseClass;
struct DamageGroup;

int Modify_Damage(int damage, WarheadTypeClass const * warhead, ArmorType armor, int distance);
void Chain_Reaction_Damage(Cell const & cell);
void Explosion_Damage(Coord const & coord, int strength, TechnoClass * source, WarheadTypeClass const * warhead, bool dochainreaction=true, HouseClass * house=0);
AnimTypeClass const * Combat_Anim(int damage, WarheadTypeClass const * warhead, LandType land, Coord const & coord);
void Combat_Lighting(int damage, WarheadTypeClass const * warhead, Coord coord, bool forced=false, unsigned int disable_flags=0);
void Wide_Area_Damage(Coord const & coord, LEPTON radius, int rawdamage, TechnoClass * source, WarheadTypeClass const * warhead);

bool Calculate_Projectile_Pitch(bool high_arc, int speed, int distance, int height, double gravity, DirType & pitch);
bool Calculate_Projectile_Angle(bool high_arc, int speed, int distance, int height, double gravity, double & angle);
bool Calculate_Projectile_Dir(int speed, int distance, int height, double gravity, DirType & dir);
bool Is_Projectile_Trajectory_Valid(int speed, int distance, int height, double gravity);

MPHType Calculate_Projectile_Speed(int range, double gravity);
int Get_Explosion_Z(Coord coord);
double Get_Floater_Gravity(void);
