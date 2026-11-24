// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "_voxel.h"
#include "globals.h"
#include "inline.h"
#include "matrix3d.h"
#include "objtype.h"
#include "quat.h"
#include "ramp.hh"

// SYNTHETIC: GAMEMD 0x00754910 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00754940 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00754960 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00754980 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007549A0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007549C0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x007549E0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00754A20 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00754A50 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00754A80 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00754AB0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00754AC0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00754AE0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00754B00 SYMBOL
// _$E41

int SomeNumbers[17] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 5, 6, 7, 8, 9, 10, 11, 12 };

float ExtraLightTiltAngles[6] = { 0.0, 0.08726389, 0.17452778, 0.26179168, 0.17452778, 0.08726389 };

#pragma warning(disable : 4305)

Matrix3D ScreenSlopeMatrices[RAMP_COUNT];

Matrix3D SlopeMatrices[RAMP_COUNT];

Quaternion ScreenSlopeQuaternions[RAMP_COUNT];

Quaternion SlopeQuaternions[RAMP_COUNT];

Matrix3D BounceMatrices[4];

Matrix3D IsometricViewMatrix;

Vector3 LightDirectionVectors[256];

Vector3 LightTiltVectors[256];

// SYNTHETIC: GAMEMD 0x00754B20 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x00754B30 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x00754B40 SYMBOL
// _$E64
// SYNTHETIC: GAMEMD 0x00754B70 SYMBOL
// _$E67
// SYNTHETIC: GAMEMD 0x00754BA0 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x00754BB0 SYMBOL
// _$E61
// SYNTHETIC: GAMEMD 0x00754BC0 SYMBOL
// _$E70
// SYNTHETIC: GAMEMD 0x00754BD0 SYMBOL
// _$E73

// FUNCTION: GAMEMD 0x00754C00
void Set_Voxel_Light_Angle(float angle)
{
	Matrix3D mtx(true);
	VoxelLightAngle = angle;
	mtx.Rotate_Y(angle);
	VoxelLightSource = mtx * Vector3(-0.707106f, -0.707106f, 0);
	VoxelShadowLightVector = Vector3(-6 * VoxelLightSource.X, 0, 0);
}

// FUNCTION: GAMEMD 0x00754BE0
Matrix3D Get_Isometric_View_Matrix(void)
{
	return(IsometricViewMatrix);
}

// FUNCTION: GAMEMD 0x00754CB0
void Init_Voxel_Matrices(void)
{
	float slope_angle = CELL_SLOPE_ANGLE;
	float slope_diag_angle = CELL_DIAG_SLOPE_ANGLE;

	const double pi = 3.1415;

	Vector3 north_east = Matrix3D(Vector3(0, 0, 1), pi / 4) * Vector3(1, 0, 0);
	Vector3 north_west = Matrix3D(Vector3(0, 0, 1), 3 * pi / 4) * Vector3(1, 0, 0);
	Vector3 south_west = Matrix3D(Vector3(0, 0, 1), 5 * pi / 4) * Vector3(1, 0, 0);
	Vector3 south_east = Matrix3D(Vector3(0, 0, 1), 7 * pi / 4) * Vector3(1, 0, 0);

	Vector3 east(1, 0, 0);
	Vector3 north(0, 1, 0);
	Vector3 west(-1, 0, 0);
	Vector3 south(0, -1, 0);

	ScreenSlopeMatrices[0].Make_Identity();

	ScreenSlopeMatrices[1] = Matrix3D(5 * pi / 4, slope_diag_angle);
	ScreenSlopeMatrices[2] = Matrix3D(3 * pi / 4, slope_diag_angle);
	ScreenSlopeMatrices[3] = Matrix3D(pi / 4, slope_diag_angle);
	ScreenSlopeMatrices[4] = Matrix3D(7 * pi / 4, slope_diag_angle);

	ScreenSlopeQuaternions[1] = Axis_To_Quat(south_west, slope_diag_angle);
	ScreenSlopeQuaternions[2] = Axis_To_Quat(north_west, slope_diag_angle);
	ScreenSlopeQuaternions[3] = Axis_To_Quat(north_east, slope_diag_angle);
	ScreenSlopeQuaternions[4] = Axis_To_Quat(south_east, slope_diag_angle);

	ScreenSlopeMatrices[5] = Matrix3D(pi, slope_angle);
	ScreenSlopeMatrices[6] = Matrix3D(pi / 2, slope_angle);
	ScreenSlopeMatrices[7] = Matrix3D(0.0, slope_angle);
	ScreenSlopeMatrices[8] = Matrix3D(3 * pi / 2, slope_angle);

	ScreenSlopeQuaternions[5] = Axis_To_Quat(west, slope_angle);
	ScreenSlopeQuaternions[6] = Axis_To_Quat(north, slope_angle);
	ScreenSlopeQuaternions[7] = Axis_To_Quat(east, slope_angle);
	ScreenSlopeQuaternions[8] = Axis_To_Quat(south, slope_angle);

	ScreenSlopeMatrices[9] = Matrix3D(pi, slope_angle);
	ScreenSlopeMatrices[10] = Matrix3D(pi / 2, slope_angle);
	ScreenSlopeMatrices[11] = Matrix3D(0.0, slope_angle);
	ScreenSlopeMatrices[12] = Matrix3D(3 * pi / 2, slope_angle);

	ScreenSlopeQuaternions[9] = Axis_To_Quat(west, slope_angle);
	ScreenSlopeQuaternions[10] = Axis_To_Quat(north, slope_angle);
	ScreenSlopeQuaternions[11] = Axis_To_Quat(east, slope_angle);
	ScreenSlopeQuaternions[12] = Axis_To_Quat(south, slope_angle);

	ScreenSlopeMatrices[13] = Matrix3D(pi, slope_diag_angle);
	ScreenSlopeMatrices[14] = Matrix3D(pi / 2, slope_diag_angle);
	ScreenSlopeMatrices[15] = Matrix3D(0.0, slope_diag_angle);
	ScreenSlopeMatrices[16] = Matrix3D(3 * pi / 2, slope_diag_angle);

	ScreenSlopeQuaternions[13] = Axis_To_Quat(west, slope_diag_angle);
	ScreenSlopeQuaternions[14] = Axis_To_Quat(north, slope_diag_angle);
	ScreenSlopeQuaternions[15] = Axis_To_Quat(east, slope_diag_angle);
	ScreenSlopeQuaternions[16] = Axis_To_Quat(south, slope_diag_angle);

	ScreenSlopeMatrices[17].Make_Identity();
	ScreenSlopeMatrices[18].Make_Identity();
	ScreenSlopeMatrices[19].Make_Identity();
	ScreenSlopeMatrices[20].Make_Identity();

	ScreenSlopeQuaternions[17].Make_Identity();
	ScreenSlopeQuaternions[18].Make_Identity();
	ScreenSlopeQuaternions[19].Make_Identity();
	ScreenSlopeQuaternions[20].Make_Identity();

	for (int i = 0; i < ARRAY_SIZE(LightDirectionVectors); i++) {
		Matrix3D direction_matrix;
		direction_matrix.Make_Identity();
		float theta = i * (float)(2 * pi / 256);
		direction_matrix.Rotate_Z(theta);

		Vector3 & direction_vec = LightDirectionVectors[i];
		Vector3 & tilt_vec = LightTiltVectors[i];

		direction_vec = direction_matrix * Vector3(1, 0, 0);

		tilt_vec = Vector3::Cross_Product(LightDirectionVectors[i], Vector3(0, 0, 1));

		Matrix3D tilt_matrix(tilt_vec, VoxelLightAngle);
		direction_vec = tilt_matrix * direction_vec;
	}

	SlopeMatrices[0].Make_Identity();

	SlopeMatrices[1] = Matrix3D(3 * pi / 2, slope_diag_angle);
	SlopeMatrices[2] = Matrix3D(pi, slope_diag_angle);
	SlopeMatrices[3] = Matrix3D(pi / 2, slope_diag_angle);
	SlopeMatrices[4] = Matrix3D(0.0, slope_diag_angle);

	SlopeMatrices[5] = Matrix3D(5 * pi / 4, slope_angle);
	SlopeMatrices[6] = Matrix3D(3 * pi / 4, slope_angle);
	SlopeMatrices[7] = Matrix3D(pi / 4, slope_angle);
	SlopeMatrices[8] = Matrix3D(7 * pi / 4, slope_angle);

	SlopeMatrices[9] = Matrix3D(5 * pi / 4, slope_angle);
	SlopeMatrices[10] = Matrix3D(3 * pi / 4, slope_angle);
	SlopeMatrices[11] = Matrix3D(pi / 4, slope_angle);
	SlopeMatrices[12] = Matrix3D(7 * pi / 4, slope_angle);

	SlopeMatrices[13] = Matrix3D(5 * pi / 4, slope_diag_angle);
	SlopeMatrices[14] = Matrix3D(3 * pi / 4, slope_diag_angle);
	SlopeMatrices[15] = Matrix3D(pi / 4, slope_diag_angle);
	SlopeMatrices[16] = Matrix3D(7 * pi / 4, slope_diag_angle);

	SlopeQuaternions[0].Make_Identity();

	SlopeQuaternions[1] = Axis_To_Quat(south, slope_diag_angle);
	SlopeQuaternions[2] = Axis_To_Quat(west, slope_diag_angle);
	SlopeQuaternions[3] = Axis_To_Quat(north, slope_diag_angle);
	SlopeQuaternions[4] = Axis_To_Quat(east, slope_diag_angle);

	SlopeQuaternions[5] = Axis_To_Quat(south_west, slope_angle);
	SlopeQuaternions[6] = Axis_To_Quat(north_west, slope_angle);
	SlopeQuaternions[7] = Axis_To_Quat(north_east, slope_angle);
	SlopeQuaternions[8] = Axis_To_Quat(south_east, slope_angle);

	SlopeQuaternions[9] = Axis_To_Quat(south_west, slope_angle);
	SlopeQuaternions[10] = Axis_To_Quat(north_west, slope_angle);
	SlopeQuaternions[11] = Axis_To_Quat(north_east, slope_angle);
	SlopeQuaternions[12] = Axis_To_Quat(south_east, slope_angle);

	SlopeQuaternions[13] = Axis_To_Quat(south_west, slope_diag_angle);
	SlopeQuaternions[14] = Axis_To_Quat(north_west, slope_diag_angle);
	SlopeQuaternions[15] = Axis_To_Quat(north_east, slope_diag_angle);
	SlopeQuaternions[16] = Axis_To_Quat(south_east, slope_diag_angle);

	SlopeMatrices[17].Make_Identity();
	SlopeMatrices[18].Make_Identity();
	SlopeMatrices[19].Make_Identity();
	SlopeMatrices[20].Make_Identity();

	SlopeQuaternions[17].Make_Identity();
	SlopeQuaternions[18].Make_Identity();
	SlopeQuaternions[19].Make_Identity();
	SlopeQuaternions[20].Make_Identity();

	IsometricViewMatrix.Make_Identity();
	IsometricViewMatrix.Rotate_X(-RAD_60);
	IsometricViewMatrix.Rotate_Z(-RAD_45);

	for (int j = 0; j < ARRAY_SIZE(BounceMatrices); j++) {
		BounceMatrices[j].Make_Identity();
	}

	BounceMatrices[0][1].Y = -1.0;

	BounceMatrices[1][0].X = 0.0;
	BounceMatrices[1][0].Y = 1.0;
	BounceMatrices[1][1].X = 1.0;
	BounceMatrices[1][1].Y = 0.0;

	BounceMatrices[2][0].X = -1.0;

	BounceMatrices[3][0].X = 0.0;
	BounceMatrices[3][0].Y = -1.0;
	BounceMatrices[3][1].X = -1.0;
	BounceMatrices[3][1].Y = 0.0;
}

// FUNCTION: GAMEMD 0x00755990
void Get_Screen_Slope_Matrix(int ramp, Matrix3D & matrix)
{
	matrix = ScreenSlopeMatrices[ramp];
}

// FUNCTION: GAMEMD 0x007559B0
Matrix3D Get_Slope_Matrix(int ramp)
{
	return(SlopeMatrices[ramp]);
}

// FUNCTION: GAMEMD 0x007559D0
void Get_Screen_Slope_Transition_Matrix(int oldramp, int newramp, Matrix3D & matrix, double time)
{
	if (oldramp == newramp) {
		Get_Screen_Slope_Matrix(newramp, matrix);
	} else {
		matrix = Build_Matrix3D(Slerp(ScreenSlopeQuaternions[oldramp], ScreenSlopeQuaternions[newramp], time));
	}
}

// FUNCTION: GAMEMD 0x00755A40
Matrix3D Get_Slope_Transition_Matrix(int oldramp, int newramp, double time)
{
	if (oldramp != newramp) {
		return(Build_Matrix3D(Slerp(SlopeQuaternions[oldramp], SlopeQuaternions[newramp], time)));
	} else {
		return(SlopeMatrices[newramp]);
	}
}

// FUNCTION: GAMEMD 0x00755AC0
int Get_Some_Voxel_Number(int index)
{
	return(SomeNumbers[index]);
}

// FUNCTION: GAMEMD 0x00755AD0
unsigned char _Direction256(Coord const & coord1, Coord const & coord2)
{
	DirType dir = Direction(coord1, coord2);
	unsigned char facing = DIR_NE - dir.As_Dir256();
	return(facing);
}

// FUNCTION: GAMEMD 0x00755B30
void Apply_Light_Transform(Matrix3D & matrix, int axis, int angle)
{
	Matrix3D mtx(LightTiltVectors[axis], ExtraLightTiltAngles[angle]);
	matrix = matrix * mtx;
	matrix.Translate(-3 * LightDirectionVectors[axis]);
	matrix.Translate(0, 0, 3);
}

// FUNCTION: GAMEMD 0x00755BF0
Vector3 Get_Light_Vector(Dir256 dir, int)
{
	Vector3& vec = LightDirectionVectors[dir];
	return(Vector3(vec.X * 1.5f, vec.Y * 1.5f, vec.Z * 1.5f));
}

// FUNCTION: GAMEMD 0x00755C50
void __fastcall Game_DestroyVoxelCaches(void)
{
	ObjectTypeClass::Clear_Voxel_Indexes();
}

// FUNCTION: GAMEMD 0x00755C60
Matrix3D Get_Bounce_Matrix(FacingType facing)
{
	if (facing >= FACING_S) {
		facing = FacingType(facing - FACING_S);
	}
	return(BounceMatrices[facing]);
}
