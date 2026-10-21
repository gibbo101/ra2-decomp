// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "matrix3d.h"
#include "rgb.h"
#include "voxdrsys.h"
#include "voxel.h"
#include "win.h"

#include "voxel.hh"

class FileClass;
class Vector3;

class VoxelLibrary
{
	public:
		struct LayerStruct
		{

			int InfoIndex;

			int Unused1;
			unsigned char Unused2;
		};

		struct LayerInfoStruct
		{

			unsigned char * StartOffset;

			unsigned char * EndOffset;

			unsigned char * DataOffset;

			float Scale;

			Matrix3DStruct Transform;

			Vector3 BoxCorner[VOXEL_BOUNDS_COUNT];

			unsigned char XSize;
			unsigned char YSize;
			unsigned char ZSize;

			unsigned char NormalType;
		};

	public:
		VoxelLibrary(void);
		VoxelLibrary(FileClass & file, int load_file_palette = false);
		~VoxelLibrary(void);

		void Clear(void);
		int Read_File(FileClass & file, int load_file_palette = false);

		LayerStruct const & Get_Layer(int layer);
		LayerInfoStruct const & Get_Layer_Info(int layer, int info);

		Vector3 Get_Bounding_Box_Center(int layer, int info);
		int Memory_Used(void);
		void Render_Object(VoxelRenderStruct & voxel, Vector3 & center);
		void Render_Shadow(VoxelShadowRenderStruct & voxel, Vector3 & center);
		void Compute_Bounding_Box(void);

		bool Load_Failed(void) const { return(LoadFailed); }
		unsigned int Get_Layer_Count(void) const { return(LayerCount); }
		unsigned int Get_Layer_Info_Count(void) const { return(LayerInfoCount); }
		int Get_Data_Size(void) const { return(DataSize); }

	private:

		bool LoadFailed;

		unsigned int LayerCount;
		unsigned int LayerInfoCount;
		int DataSize;
		LayerStruct * LayerHeaders;
		LayerInfoStruct * LayerInfos;
		unsigned char * Data;
};

void Precalculate_Normal_Lookup(Vector3 const & light, int normal_type);
void Precalculate_Normal_Lookup(Vector3 const & light, Vector3 const & viewer, float specular_strength, int normal_type);
int Find_Closest_Normal(Vector3 const & surface_normal, int normal_type);
void Init_Normal_Lookup(void);

class VoxelPaletteLibrary
{
	friend class VoxelLibrary;

	public:
		VoxelPaletteLibrary(RGBStruct * rgb, void * lut);
		~VoxelPaletteLibrary(void);

		int Read_Colors(FileClass & file);

		int Read_File(FileClass & file);
		int Write_File(FileClass & file);

	private:
		void Calculate_Lookup_Table(float *scale = NULL, int lut_count = 0);
		unsigned char Closest_Color(float red, float green, float blue) const;
		unsigned char Closest_Color(float red, float green, float blue, bool is_remap) const;

	public:

		VPLHeaderStruct Header;
	private:

		RGBStruct * Colors;

		unsigned char *LUT;

		BOOL ColorsAllocated;
		BOOL LUTAllocated;

	enum {
		VPL_NUM_COLORS = 256,
		VPL_NUM_ENTRIES = 32,
	};
};

extern "C" {
extern short VoxelPixelDeltaTable[VOXEL_BITMAP_WIDTH][2];
extern unsigned char VoxelNormalTranslateTable[VOXEL_PALETTE_SIZE];
}
