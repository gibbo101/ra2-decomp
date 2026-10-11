// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "voxdrsys.h"
#include "_voxel.h"
#include "bsurface.h"
#include "stbuffer.h"
#include "voxlib.h"
#include "wwfile.h"

unsigned char VoxelDrawBuffer[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT * VOXEL_BITMAP_BPP];

Vector3 MinVoxelBounds;

Vector3 MaxVoxelBounds;

VoxelRenderStruct VoxelRenderData[64];

int VoxelRenderDataCount;

unsigned short Voxel16BitColors[VOXEL_PALETTE_SIZE];

int VPLRemapStart;

int VPLRemapEnd;

int VPLLUTCount;

int VPLUnused;

unsigned char Voxel256Array[VOXEL_PALETTE_SIZE];

// FUNCTION: 0x00753B70
int VoxelDrawSystem::Load_VPL_File(FileClass & file)
{
	VoxelPaletteLibrary vpl(VoxelRGBColors, VoxelPaletteTranslateTable);
	int result = vpl.Read_File(file);
	VPLRemapStart = vpl.Header.RemapStart;
	VPLRemapEnd = vpl.Header.RemapEnd;
	VPLLUTCount = vpl.Header.LUTCount;
	VPLUnused = vpl.Header.Unused;
	return(result);
}

// FUNCTION: 0x00753BD0
void VoxelDrawSystem::Init_256_Array(void)
{
	for (int i = 0; i < (int)sizeof(Voxel256Array); i++) {
		Voxel256Array[i] = i;
	}
}

// FUNCTION: 0x00753BF0
void VoxelDrawSystem::Convert_Voxel_Colors(int red_left, int red_right, int green_left, int green_right, int blue_left, int blue_right)
{
	for (unsigned i = 0; i < VOXEL_PALETTE_SIZE; i++) {
		unsigned char red = VoxelRGBColors[i].Red;
		unsigned char green = VoxelRGBColors[i].Green;
		unsigned char blue = VoxelRGBColors[i].Blue;
		Voxel16BitColors[i] = (((red >> red_left) << red_right) | ((green >> green_left) << green_right) | ((blue >> blue_left) << blue_right));
	}
}

// FUNCTION: 0x00753C80
void VoxelDrawSystem::Precalculate_Light(VoxelLibrary * voxlib, int layer, int info, Matrix3D const & light_transform, Vector3 const & light)
{
	Matrix3D light_transform_inv = Matrix3D::Orthogonal_Inverse(light_transform);
	Vector3 light_tr = light_transform_inv.Rotate_Vector(light);
	int normal_type = voxlib->Get_Layer_Info(layer, info).NormalType;
	Precalculate_Normal_Lookup(light_tr, normal_type);
}

// FUNCTION: 0x00753D00
void VoxelDrawSystem::Precalculate_Light(VoxelLibrary * voxlib, int layer, int info, Matrix3D const & light_transform, Matrix3D const & view_transform, Vector3 const & light, float specular_strength)
{
	Matrix3D light_transform_inv = Matrix3D::Orthogonal_Inverse(light_transform);
	Matrix3D view_transform_inv = Matrix3D::Orthogonal_Inverse(view_transform);
	Vector3 light_tr = light_transform_inv.Rotate_Vector(light);
	Vector3 viewer_tr = view_transform_inv.Rotate_Vector(Vector3(0, 0, 1));
	int normal_type = voxlib->Get_Layer_Info(layer, info).NormalType;
	Precalculate_Normal_Lookup(light_tr, viewer_tr, specular_strength, normal_type);
}

// FUNCTION: 0x007540F0
void VoxelDrawSystem::Prep_For_Object(VoxelLibrary * voxlib, int layer, int info, Matrix3D const & transform)
{
	VoxelLibrary::LayerInfoStruct const & layer_info = voxlib->Get_Layer_Info(layer, info);

	VoxelRenderStruct & data = VoxelRenderData[VoxelRenderDataCount];

	data.MinBounds = Vector3(10000, 10000, 10000);
	data.MaxBounds = Vector3(-10000, -10000, -10000);
	data.Info = info;
	data.Layer = layer;
	data.VoxLib = voxlib;

	for (int i = 0; i < FACING_COUNT; i++) {
		data.BoxCorner[i] = transform * layer_info.BoxCorner[i];
		data.BoxCorner[i].Y = -data.BoxCorner[i].Y;
		if (data.BoxCorner[i].X < data.MinBounds.X) {
			data.MinBounds.X = data.BoxCorner[i].X;
		}
		if (data.BoxCorner[i].X > data.MaxBounds.X) {
			data.MaxBounds.X = data.BoxCorner[i].X;
		}
		if (data.BoxCorner[i].Y < data.MinBounds.Y) {
			data.MinBounds.Y = data.BoxCorner[i].Y;
		}
		if (data.BoxCorner[i].Y > data.MaxBounds.Y) {
			data.MaxBounds.Y = data.BoxCorner[i].Y;
		}
		if (data.BoxCorner[i].Z < data.MinBounds.Z) {
			data.MinBounds.Z = data.BoxCorner[i].Z;
			data.AnchorCornerIndex = i;
		}
		if (data.BoxCorner[i].Z > data.MaxBounds.Z) {
			data.MaxBounds.Z = data.BoxCorner[i].Z;
		}
	}

	if (data.MinBounds.X < MinVoxelBounds.X) {
		MinVoxelBounds.X = data.MinBounds.X;
	}
	if (data.MinBounds.Y < MinVoxelBounds.Y) {
		MinVoxelBounds.Y = data.MinBounds.Y;
	}
	if (data.MinBounds.Z < MinVoxelBounds.Z) {
		MinVoxelBounds.Z = data.MinBounds.Z;
	}
	if (data.MaxBounds.X > MaxVoxelBounds.X) {
		MaxVoxelBounds.X = data.MaxBounds.X;
	}
	if (data.MaxBounds.Y > MaxVoxelBounds.Y) {
		MaxVoxelBounds.Y = data.MaxBounds.Y;
	}
	if (data.MaxBounds.Z > MaxVoxelBounds.Z) {
		MaxVoxelBounds.Z = data.MaxBounds.Z;
	}

	VoxelRenderDataCount++;
}

// FUNCTION: 0x00754740
void VoxelDrawSystem::Clear_Buffer(int x, int y, int width, int height)
{
	if (x >= 0 && y >= 0 && x + width <= VOXEL_BITMAP_WIDTH-1 && y + height <= VOXEL_BITMAP_HEIGHT-1) {
		for (int i = y; i <= y + height; i++) {
			memset(&VoxelDrawBuffer[VOXEL_BITMAP_HEIGHT * i + x], 0, width);
		}
	} else {
		memset(VoxelDrawBuffer, 0, sizeof(VoxelDrawBuffer));
	}
}

// FUNCTION: 0x007547C0
void VoxelDrawSystem::Clear_Buffer(SurfaceRegion & region)
{
	if (region.Point.X >= 0 && region.Point.Y >= 0 && region.Point.X + region.Bounds.X <= VOXEL_BITMAP_WIDTH-1 && region.Point.Y + region.Bounds.Y <= VOXEL_BITMAP_HEIGHT-1) {
		for (int i = region.Point.Y; i <= region.Point.Y + region.Bounds.Y; i++) {
			memset(&VoxelDrawBuffer[VOXEL_BITMAP_HEIGHT * i + region.Point.X], 0, region.Bounds.X+1);
		}
	} else {
		memset(VoxelDrawBuffer, 0, sizeof(VoxelDrawBuffer));
	}
}

// FUNCTION: 0x00754860
void VoxelDrawSystem::Clear_Z_Buffer(int x, int y, int width, int height)
{
	for (int i = y; i <= y + height; i++) {
		memset(&VoxelDrawZBuffer[VOXEL_BITMAP_HEIGHT * i + x], 0, width);
	}
}

// FUNCTION: 0x007548B0
void VoxelDrawSystem::Clear_Z_Buffer(SurfaceRegion & region)
{
	for (int i = region.Point.Y; i <= region.Point.Y + region.Bounds.Y; i++) {
		memset(&VoxelDrawZBuffer[VOXEL_BITMAP_HEIGHT * i + region.Point.X], 0, region.Bounds.X+1);
	}
}
