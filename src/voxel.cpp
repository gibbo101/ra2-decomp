// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "_voxel.h"
#include "globals.h"
#include "inline.h"
#include "matrix3d.h"
#include "objtype.h"
#include "quat.h"
#include "ramp.hh"

Vector3 LightTiltVectors[256];

float ExtraLightTiltAngles[6] = { 0.0, 0.08726389, 0.17452778, 0.26179168, 0.17452778, 0.08726389 };

#pragma warning(disable : 4305)

Matrix3D ScreenSlopeMatrices[RAMP_COUNT];

Matrix3D SlopeMatrices[RAMP_COUNT];

Quaternion ScreenSlopeQuaternions[RAMP_COUNT];

Quaternion SlopeQuaternions[RAMP_COUNT];

Matrix3D BounceMatrices[4];

Matrix3D IsometricViewMatrix;

Vector3 LightDirectionVectors[256];

// FUNCTION: 0x00754BE0
Matrix3D Get_Isometric_View_Matrix(void)
{
	return(IsometricViewMatrix);
}

// FUNCTION: 0x00755990
void Get_Screen_Slope_Matrix(int ramp, Matrix3D & matrix)
{
	matrix = ScreenSlopeMatrices[ramp];
}

// FUNCTION: 0x007559B0
Matrix3D Get_Slope_Matrix(int ramp)
{
	return(SlopeMatrices[ramp]);
}

// FUNCTION: 0x007559D0
void Get_Screen_Slope_Transition_Matrix(int oldramp, int newramp, Matrix3D & matrix, double time)
{
	if (oldramp == newramp) {
		Get_Screen_Slope_Matrix(newramp, matrix);
	} else {
		matrix = Build_Matrix3D(Slerp(ScreenSlopeQuaternions[oldramp], ScreenSlopeQuaternions[newramp], time));
	}
}

// FUNCTION: 0x00755A40
Matrix3D Get_Slope_Transition_Matrix(int oldramp, int newramp, double time)
{
	if (oldramp != newramp) {
		return(Build_Matrix3D(Slerp(SlopeQuaternions[oldramp], SlopeQuaternions[newramp], time)));
	} else {
		return(SlopeMatrices[newramp]);
	}
}

// FUNCTION: 0x00755AD0
unsigned char _Direction256(Coord const & coord1, Coord const & coord2)
{
	DirType dir = Direction(coord1, coord2);
	unsigned char facing = DIR_NE - dir.As_Dir256();
	return(facing);
}

// FUNCTION: 0x00755B30
void Apply_Light_Transform(Matrix3D & matrix, int axis, int angle)
{
	Matrix3D mtx(LightTiltVectors[axis], ExtraLightTiltAngles[angle]);
	matrix = matrix * mtx;
	matrix.Translate(-3 * LightDirectionVectors[axis]);
	matrix.Translate(0, 0, 3);
}

// FUNCTION: 0x00755BF0
Vector3 Get_Light_Vector(Dir256 dir, int)
{
	Vector3& vec = LightDirectionVectors[dir];
	return(Vector3(vec.X * 1.5f, vec.Y * 1.5f, vec.Z * 1.5f));
}

// FUNCTION: 0x00755C60
Matrix3D Get_Bounce_Matrix(FacingType facing)
{
	if (facing >= FACING_S) {
		facing = FacingType(facing - FACING_S);
	}
	return(BounceMatrices[facing]);
}
