// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "bounce.h"
#include "_map.h"
#include "_tactica.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "map.h"
#include "tactical.h"
#include "vector3.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x00439480 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x004394A0 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x004394D0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x004394F0 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x00439510 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x00439530 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x00439550 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x00439570 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x004395B0 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x004395E0 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x00439610 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x00439640 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x00439650 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x00439670 SYMBOL
// _$E41

static inline Vector3 Normalize_Axis(const Vector3 & vec)
{
	float len = (float)fastmath::sqrt(vec.Z*vec.Z + vec.Y*vec.Y + vec.X*vec.X);
	if (len != 0.0) {
		return(vec / len);
	}
	return(vec);
}

// FUNCTION: GAMEMD 0x004397E0
void BounceClass::Init(Coord const & coord, double elasticity, double gravity, double max_velocity, Vector3 const & velocity, double rotation)
{
	Elasticity = elasticity;
	Gravity = gravity;
	MaxVelocity = max_velocity;
	MyCoord.X = coord.X;
	MyCoord.Y = coord.Y;
	MyCoord.Z = coord.Z;
	Velocity = velocity;

	Vector3 rotation_axis(
		Random_Pick(-65535, 65535) * (1.0f / 65535.0f),
		Random_Pick(-65535, 65535) * (1.0f / 65535.0f),
		Random_Pick(-65535, 65535) * (1.0f / 65535.0f)
	);

	rotation_axis = Normalize_Axis(rotation_axis);
	AngularVelocity = Axis_To_Quat(rotation_axis, rotation);
	Matrix3D identity_matrix;
	identity_matrix.Make_Identity();
	Rotation = Build_Quaternion(identity_matrix);
}

// FUNCTION: GAMEMD 0x004399A0
Coord BounceClass::Get_Bounce_Coord(void) const
{
	return(Coord(MyCoord.X, MyCoord.Y, MyCoord.Z));
}

// FUNCTION: GAMEMD 0x004399E0
Matrix3D BounceClass::Get_Matrix(void) const
{
	return(Build_Matrix3D(Rotation));
}

// FUNCTION: GAMEMD 0x00439A10
double BounceClass::Get_Remaining_Motion(void) const
{
	Coord coord(MyCoord.X, MyCoord.Y, MyCoord.Z);

	int height = MyCoord.Z - Map.Get_Height_GL(coord);
	if (Map[coord].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT) {
		height -= BRIDGE_LEPTON_HEIGHT;
	}
	return(Point3D(Velocity.X, Velocity.Y, height * Gravity + Velocity.Z).Length());
}
