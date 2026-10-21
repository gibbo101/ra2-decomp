// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E3D70;
extern const double DAT_007E3D78;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x004394A0 $E80
double DAT_0089C720 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004394D0 $E83
double DAT_0089C770 = DAT_0089C720 / DAT_007E3D70;

// FUNCTION: 0x004394F0 $E86
double DAT_0089C758 = DAT_007E1718 / DAT_0089C770;

// FUNCTION: 0x00439510 $E89
double DAT_0089C750 = DAT_007E1720 * DAT_007E3D78;

// FUNCTION: 0x00439530 $E92
double DAT_0089C728 = DAT_007E1728 * DAT_007E3D78;

// FUNCTION: 0x00439550 $E95
double DAT_0089C700 = DAT_007E1730 * DAT_007E3D78;

// FUNCTION: 0x00439570 $E98
int DAT_0089C778 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089C700 - DAT_0089C728) * DAT_0089C720));

// FUNCTION: 0x004395B0 $E101
double DAT_0089C730 = FUN_004CADE0(DAT_0089C778 * DAT_007E1740);

// FUNCTION: 0x004395E0 $E104
double DAT_0089C748 = FUN_004CADE0((DAT_0089C778 * 2) / DAT_0089C720);

// FUNCTION: 0x00439610 $E107
int DAT_0089C76C = (int)(DAT_0089C778 * 4 + DAT_007E1738);

// FUNCTION: 0x00439640 $E110
ShortPair DAT_0089C718;

// FUNCTION: 0x00439650 $E113
IntTriple DAT_0089C738;

struct IntTripleXYZ {
	int X, Y, Z;
	IntTripleXYZ(int x, int y, int z) { X = x; Y = y; Z = z; }
};

// FUNCTION: 0x00439670 $E116
IntTripleXYZ DAT_0089C760(0x80, 0x80, 0);

static inline Vector3 Normalize_Axis(const Vector3 & vec)
{
	float len = (float)fastmath::sqrt(vec.Z*vec.Z + vec.Y*vec.Y + vec.X*vec.X);
	if (len != 0.0) {
		return(vec / len);
	}
	return(vec);
}

// FUNCTION: 0x004397E0
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

// FUNCTION: 0x004399A0
Coord BounceClass::Get_Bounce_Coord(void) const
{
	return(Coord(MyCoord.X, MyCoord.Y, MyCoord.Z));
}

// FUNCTION: 0x004399E0
Matrix3D BounceClass::Get_Matrix(void) const
{
	return(Build_Matrix3D(Rotation));
}

// FUNCTION: 0x00439A10
double BounceClass::Get_Remaining_Motion(void) const
{
	Coord coord(MyCoord.X, MyCoord.Y, MyCoord.Z);

	int height = MyCoord.Z - Map.Get_Height_GL(coord);
	if (Map[coord].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT) {
		height -= BRIDGE_LEPTON_HEIGHT;
	}
	return(Point3D(Velocity.X, Velocity.Y, height * Gravity + Velocity.Z).Length());
}
