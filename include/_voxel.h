// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "motlib.h"
#include "noinit.h"
#include "stbuffer.h"
#include "voxlib.h"

template<class INDEX, class T> class IndexClass;
class Matrix3D;
class Vector3;

struct VoxelDataStruct
{
	VoxelDataStruct(void)
	{
		VoxLib = NULL;
		MotLib = NULL;
	}

	VoxelDataStruct(NoInitClass const & x) {}
	~VoxelDataStruct(void);

	VoxelLibrary * VoxLib;

	MotionLibrary * MotLib;
};

struct VoxelCacheStruct
{

	short X;
	short Y;
	unsigned short Width;
	unsigned short Height;
	void * Buffer;
};

typedef IndexClass<int, StaticBufferClass::Entry *> VoxelIndexClass;

extern bool UseVoxelCache;
extern Vector3 VoxelShadowLightVector;
extern float VoxelLightAngle;
extern Vector3 VoxelLightSource;
extern float VoxelCameraAngle;
extern Matrix3D VoxelCameraMatrix;
extern StaticBufferClass VoxelStaticBuffer;

extern const float DefaultLightAngle;
extern const float DefaultCameraAngle;

inline void Set_Voxel_Camera_Angle(float angle)
{
	VoxelCameraAngle = angle;
	VoxelCameraMatrix.Make_Identity();
}

void Set_Voxel_Light_Angle(float angle);
