// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "matrix3d.h"
#include "rgb.h"
#include "sun.h"
#include "vector3.h"
#include "vector3i.h"
#include "win.h"

#include "voxel.hh"

class Surface;
class BSurface;
class FileClass;
class VoxelLibrary;
struct SurfaceRegion;
class Vector3i16;
template<class T> class TRect;
typedef TRect<int> Rect;

#define VOXEL_BITMAP_WIDTH 256
#define VOXEL_BITMAP_HEIGHT 256
#define VOXEL_BITMAP_BPP 1

namespace VoxelDrawSystem
{
	extern BOOL EnableLighting;
	extern BOOL EnableZBuffer;

	int Load_VPL_File(FileClass & file);

	inline void Enable_Lighting(void) { EnableLighting = true; }
	inline void Disable_Lighting(void) { EnableLighting = false; }
	inline void Enable_ZBuffer(void) { EnableZBuffer = true; }
	inline void Disable_ZBuffer(void) { EnableZBuffer = false; }

	void Init_256_Array(void);
	void Convert_Voxel_Colors(int red_left, int red_right, int green_left, int green_right, int blue_left, int blue_right);

	unsigned char * Get_Surface_Buffer(void);
	Surface * Get_Surface(void);

	void Precalculate_Light(VoxelLibrary * voxlib, int layer, int info, const Matrix3D & light_transform, Vector3 const & light);
	void Precalculate_Light(VoxelLibrary * voxlib, int layer, int info, const Matrix3D & light_transform, Matrix3D const & view_transform, Vector3 const & light, float specular_strength);

	void Reset(void);

	void Prep_For_Shadow(VoxelLibrary * voxlib, int layer, int info, Matrix3D const & camera, Matrix3D const & motion, Vector3 const & light, Surface * surface = NULL, int shadow_x = 0, int shadow_y = 0);
	void Prep_For_Object(VoxelLibrary * voxlib, int layer, int info, Matrix3D const & transform);

	void Render(Rect & rect, int & x, int & y);
	SurfaceRegion Render(void);

	void Clear_Buffer(void);
	void Clear_Buffer(int x, int y, int width, int height);
	void Clear_Buffer(SurfaceRegion & region);

	void Clear_Z_Buffer(void);
	void Clear_Z_Buffer(int x, int y, int width, int height);
	void Clear_Z_Buffer(SurfaceRegion & region);
};

struct VoxelRenderStruct {
	VoxelLibrary * VoxLib;
	int Layer;
	int Info;
	int AnchorCornerIndex;
	Vector3 MinBounds;
	Vector3 MaxBounds;
	Vector3 BoxCorner[VOXEL_BOUNDS_COUNT];
};

struct VoxelShadowRenderStruct {

	VoxelLibrary * VoxLib;
	int Layer;
	int Info;

	Vector3 ShadowCorner[4];
};

struct VoxelRenderOrientation {
	int Reversed;
	int Corner0;
	int CornerX;
	int CornerY;
	int CornerZ;
	int ZIndexFactor;
	int YIndexFactor;
	int XIndexStride;
	int YIndexStride;
};

extern VoxelRenderOrientation VoxelRenderOrientations[VOXEL_BOUNDS_COUNT];

struct VoxelFuncArgumentStruct {

	unsigned char * StartOffset;
	unsigned char * EndOffset;
	unsigned char * DataOffset;

	int StartIndex;

	int StrideX;
	int StrideY;

	Vector3i16 TransformMatrix[4];

	unsigned char XSize;
	unsigned char YSize;
	unsigned char ZSize;
};
extern "C" {
extern unsigned char VoxelPaletteTranslateTable[MAX_PALETTE_LOOKUP_ENTRIES][VOXEL_PALETTE_SIZE];
extern unsigned char VoxelDrawBuffer[];
extern unsigned char VoxelDrawZBuffer[];
}

extern RGBStruct VoxelRGBColors[VOXEL_PALETTE_SIZE];

extern BSurface VoxelSurface;

extern BSurface VoxelZSurface;
