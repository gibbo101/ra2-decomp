// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "_voxel.h"

#include "matrix3d.h"
#include "stbuffer.h"

bool UseVoxelCache;
Vector3 VoxelShadowLightVector;
float VoxelLightAngle;
Vector3 VoxelLightSource;
float VoxelCameraAngle;
Matrix3D VoxelCameraMatrix;
StaticBufferClass VoxelStaticBuffer(4000000);

// SYNTHETIC: GAMEMD 0x0040F220 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0040F240 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040F250 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0040F260 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0040F270 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x0040F290 SYMBOL
// _$E14
