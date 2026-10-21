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

BOOL VoxelDrawSystem::EnableLighting;

BOOL VoxelDrawSystem::EnableZBuffer;

unsigned char VoxelDrawBuffer[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT * VOXEL_BITMAP_BPP];

// FUNCTION: 0x007539D0 $E7
// FUNCTION: 0x00753A30 $E5
BSurface VoxelSurface(VOXEL_BITMAP_WIDTH, VOXEL_BITMAP_HEIGHT, VOXEL_BITMAP_BPP, VoxelDrawBuffer);

unsigned char VoxelDrawZBuffer[VOXEL_BITMAP_WIDTH * VOXEL_BITMAP_HEIGHT * VOXEL_BITMAP_BPP];

// FUNCTION: 0x00753A50 $E12
// FUNCTION: 0x00753AB0 $E10
BSurface VoxelZSurface(VOXEL_BITMAP_WIDTH, VOXEL_BITMAP_HEIGHT, VOXEL_BITMAP_BPP, VoxelDrawZBuffer);

// FUNCTION: 0x00753AD0 $E15
Vector3 MinVoxelBounds;

// FUNCTION: 0x00753AE0 $E18
Vector3 MaxVoxelBounds;

// FUNCTION: 0x00753AF0 $E21
Rect LastRenderRect(0, 0, VOXEL_BITMAP_WIDTH - 1, VOXEL_BITMAP_HEIGHT - 1);

// FUNCTION: 0x00753B10 $E24
VoxelRenderStruct VoxelRenderData[64];

// FUNCTION: 0x00753B40 $E27
VoxelShadowRenderStruct VoxelShadowRenderData[64];

int VoxelRenderDataCount;

int VoxelShadowRenderDataCount;

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

// FUNCTION: 0x00753C60
unsigned char * VoxelDrawSystem::Get_Surface_Buffer(void)
{
	return(VoxelDrawBuffer);
}

// FUNCTION: 0x00753C70
Surface * VoxelDrawSystem::Get_Surface(void)
{
	return(&VoxelSurface);
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

// FUNCTION: 0x00753E00
void VoxelDrawSystem::Reset(void)
{
	static int _need_buffer_init = true;

	VoxelRenderDataCount = 0;
	VoxelShadowRenderDataCount = 0;

	if (_need_buffer_init) {
		memset(VoxelDrawBuffer, 0, sizeof(VoxelDrawBuffer));
		memset(VoxelDrawZBuffer, 0, sizeof(VoxelDrawZBuffer));
		_need_buffer_init = false;
	} else {
		if (LastRenderRect.X >= 0 && LastRenderRect.Y >= 0 && LastRenderRect.X + LastRenderRect.Width <= VOXEL_BITMAP_WIDTH-1 && LastRenderRect.Y + LastRenderRect.Height <= VOXEL_BITMAP_HEIGHT-1) {
			for (int i = LastRenderRect.Y; i <= LastRenderRect.Y + LastRenderRect.Height; i++) {
				memset(&VoxelDrawBuffer[VOXEL_BITMAP_HEIGHT * i + LastRenderRect.X], 0, LastRenderRect.Width + 1);
			}
		} else {
			memset(VoxelDrawBuffer, 0, sizeof(VoxelDrawBuffer));
		}
		if (VoxelDrawSystem::EnableZBuffer) {
			if (LastRenderRect.Y <= LastRenderRect.Height + LastRenderRect.Y) {
				for (int i = LastRenderRect.Y; i <= LastRenderRect.Y + LastRenderRect.Height; i++) {
					memset(&VoxelDrawZBuffer[VOXEL_BITMAP_HEIGHT * i + LastRenderRect.X], 0, LastRenderRect.Width + 1);
				}
			}
		}
	}

	MinVoxelBounds = Vector3(10000, 10000, 10000);
	MaxVoxelBounds = Vector3(-10000, -10000, -10000);
}

// FUNCTION: 0x00753F90
void VoxelDrawSystem::Prep_For_Shadow(VoxelLibrary * voxlib, int layer, int info, Matrix3D const & camera, Matrix3D const & motion, Vector3 const & light, Surface * surface, int shadow_x, int shadow_y)
{
	VoxelLibrary::LayerInfoStruct const & layer_info = voxlib->Get_Layer_Info(layer, info);

	VoxelShadowRenderStruct & data = VoxelShadowRenderData[VoxelShadowRenderDataCount];

	data.VoxLib = voxlib;
	data.Layer = layer;
	data.Info = info;
	data.ShadowSurface = surface;
	data.ShadowX = shadow_x;
	data.ShadowY = shadow_y;

	for (int i = 0; i < 4; i++) {
		data.ShadowCorner[i] = motion * layer_info.BoxCorner[i];
		data.ShadowCorner[i].Z = 0;
		data.ShadowCorner[i] = camera * data.ShadowCorner[i];
		data.ShadowCorner[i] = data.ShadowCorner[i] + light;
		data.ShadowCorner[i].Y = -data.ShadowCorner[i].Y;

		if (data.ShadowCorner[i].X < MinVoxelBounds.X) {
			MinVoxelBounds.X = data.ShadowCorner[i].X;
		}
		if (data.ShadowCorner[i].Y < MinVoxelBounds.Y) {
			MinVoxelBounds.Y = data.ShadowCorner[i].Y;
		}
		if (data.ShadowCorner[i].X > MaxVoxelBounds.X) {
			MaxVoxelBounds.X = data.ShadowCorner[i].X;
		}
		if (data.ShadowCorner[i].Y > MaxVoxelBounds.Y) {
			MaxVoxelBounds.Y = data.ShadowCorner[i].Y;
		}
	}

	VoxelShadowRenderDataCount++;
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

static inline void _Render(VoxelRenderStruct ** render_data, Rect & rect, int & x, int & y)
{
	Vector3 center;
	center.X = (MaxVoxelBounds.X + MinVoxelBounds.X) * 0.5f;
	center.Y = (MaxVoxelBounds.Y + MinVoxelBounds.Y) * 0.5f;
	center.Z = (MaxVoxelBounds.Z + MinVoxelBounds.Z) * 0.5f;

	int index;
	for (index = 0; index < VoxelShadowRenderDataCount; index++) {
		VoxelShadowRenderData[index].VoxLib->Render_Shadow(VoxelShadowRenderData[index], center);
	}

	for (index = 0; index < VoxelRenderDataCount; index++) {
		render_data[index] = &VoxelRenderData[index];
	}

	for (int i = 0; i < VoxelRenderDataCount - 1; i++) {
		for (int index2 = i + 1; index2 < VoxelRenderDataCount; index2++) {
			VoxelRenderStruct * first = render_data[i];
			VoxelRenderStruct * second = render_data[index2];

			if (first->MaxBounds.Z > second->MaxBounds.Z) {
				render_data[i] = second;
				render_data[index2] = first;
			}
		}
	}

	if (VoxelDrawSystem::EnableZBuffer) {
		for (index = VoxelRenderDataCount - 1; index >= 0; index--) {
			render_data[index]->VoxLib->Render_Object(*render_data[index], center);
		}
	} else {
		for (index = 0; index < VoxelRenderDataCount; index++) {
			render_data[index]->VoxLib->Render_Object(*render_data[index], center);
		}
	}

	rect.Width = MaxVoxelBounds.X - MinVoxelBounds.X;

	rect.Height = MaxVoxelBounds.Y - MinVoxelBounds.Y;

	rect.Set(VOXEL_BITMAP_WIDTH / 2 - rect.Width / 2, VOXEL_BITMAP_HEIGHT / 2 - rect.Height / 2, rect.Width + 8, rect.Height + 8);

	rect.X -= 4;
	rect.Y -= 4;
	LastRenderRect = rect;

	x = (int)center.X - rect.Width / 2;
	y = (int)center.Y - rect.Height / 2;
}

// FUNCTION: 0x007542F0
void VoxelDrawSystem::Render(Rect & rect, int & x, int & y)
{
	static VoxelRenderStruct * render_data[64];
	_Render(render_data, rect, x, y);
}

// FUNCTION: 0x00754510
SurfaceRegion VoxelDrawSystem::Render(void)
{
	static VoxelRenderStruct * render_data[64];
	Rect rect;
	int x;
	int y;
	_Render(render_data, rect, x, y);
	SurfaceRegion region;
	region.Point = Point2D(x, y);
	region.Bounds = rect;
	return(region);
}

// FUNCTION: 0x00754720
void VoxelDrawSystem::Clear_Buffer(void)
{
	memset(VoxelDrawBuffer, 0, sizeof(VoxelDrawBuffer));
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

// FUNCTION: 0x00754840
void VoxelDrawSystem::Clear_Z_Buffer(void)
{
	memset(VoxelDrawZBuffer, 0, sizeof(VoxelDrawZBuffer));
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

#include "geometry.h"

// FUNCTION: 0x00754900 Vector3::Vector3

// FUNCTION: 0x00754910 $E30
// FUNCTION: 0x00754940 $E33
// FUNCTION: 0x00754960 $E36
// FUNCTION: 0x00754980 $E39
// FUNCTION: 0x007549A0 $E42
// FUNCTION: 0x007549C0 $E45
// FUNCTION: 0x007549E0 $E48
// FUNCTION: 0x00754A20 $E51
// FUNCTION: 0x00754A50 $E54
// FUNCTION: 0x00754A80 $E57
// FUNCTION: 0x00754AB0 $E60
// FUNCTION: 0x00754AC0 $E63
// FUNCTION: 0x00754AE0 $E66
// FUNCTION: 0x00754B00 $E2
