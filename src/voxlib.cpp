// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "voxlib.h"
#include "voxdrsys.h"
#include "wwfile.h"
#include <climits>

typedef void (__cdecl *VoxelFuncPtr)(VoxelFuncArgumentStruct *);

void __cdecl Draw_Voxel_Regular_Normals(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Reverse_Normals(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Regular_Normals_ZBuffer(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Reverse_Normals_ZBuffer(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Regular_Normals_Lighting(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Reverse_Normals_Lighting(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Regular_Normals_ZBuffer_Lighting(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Reverse_Normals_ZBuffer_Lighting(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Regular(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Reverse(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Regular_ZBuffer(VoxelFuncArgumentStruct * state);

void __cdecl Draw_Voxel_Reverse_ZBuffer(VoxelFuncArgumentStruct * state);

extern "C" {
void __cdecl Draw_Voxel_Regular_Normals_ASM(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Reverse_Normals_ASM(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Regular_Lighting_Normals_ASM(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Reverse_Lighting_Normals_ASM(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Regular_ASM(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_Reverse_ASM(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_UNUSED1_ASM(VoxelFuncArgumentStruct * state);
void __cdecl Draw_Voxel_UNUSED2_ASM(VoxelFuncArgumentStruct * state);
}

VoxelFuncPtr VoxelDrawFunctions[32] = {
	&Draw_Voxel_Regular_Normals_ASM,
	&Draw_Voxel_Reverse_Normals_ASM,
	&Draw_Voxel_Regular_Normals_ZBuffer,
	&Draw_Voxel_Reverse_Normals_ZBuffer,
	&Draw_Voxel_Regular_Lighting_Normals_ASM,
	&Draw_Voxel_Reverse_Lighting_Normals_ASM,
	&Draw_Voxel_Regular_Normals_ZBuffer_Lighting,
	&Draw_Voxel_Reverse_Normals_ZBuffer_Lighting,
	&Draw_Voxel_Regular_ASM,
	&Draw_Voxel_Reverse_ASM,
	&Draw_Voxel_Regular_ZBuffer,
	&Draw_Voxel_Reverse_ZBuffer,
	&Draw_Voxel_Regular_ASM,
	&Draw_Voxel_Reverse_ASM,
	&Draw_Voxel_Regular_ZBuffer,
	&Draw_Voxel_Reverse_ZBuffer,

	&Draw_Voxel_Regular_Normals,
	&Draw_Voxel_Reverse_Normals,
	&Draw_Voxel_Regular_Normals_ZBuffer,
	&Draw_Voxel_Reverse_Normals_ZBuffer,
	&Draw_Voxel_Regular_Normals_Lighting,
	&Draw_Voxel_Reverse_Normals_Lighting,
	&Draw_Voxel_Regular_Normals_ZBuffer_Lighting,
	&Draw_Voxel_Reverse_Normals_ZBuffer_Lighting,
	&Draw_Voxel_Regular,
	&Draw_Voxel_Reverse,
	&Draw_Voxel_Regular_ZBuffer,
	&Draw_Voxel_Reverse_ZBuffer,
	&Draw_Voxel_Regular,
	&Draw_Voxel_Reverse,
	&Draw_Voxel_Regular_ZBuffer,
	&Draw_Voxel_Reverse_ZBuffer,
};

inline void Fill_Delta_Table1(VoxelFuncArgumentStruct * state)
{
	short (&ztable)[VOXEL_BITMAP_WIDTH][2] = VoxelPixelDeltaTable;

	ztable[0][0] = 0;
	ztable[0][1] = 0;

	Vector3i16 & step_z = state->TransformMatrix[3];

	for (unsigned int z = 1; z < state->ZSize; z++) {
		ztable[z][0] = ztable[z - 1][0] + step_z.I;
		ztable[z][1] = ztable[z - 1][1] + step_z.J;
	}
}

inline void Fill_Delta_Table3(VoxelFuncArgumentStruct * state)
{
	short (&ztable)[VOXEL_BITMAP_WIDTH][2] = VoxelPixelDeltaTable;

	ztable[0][0] = 0;
	ztable[0][1] = 0;

	Vector3i16 & step_z = state->TransformMatrix[3];

	for (unsigned int z = 1; z < state->ZSize; z++) {
		ztable[z][0] = ztable[z - 1][0] + step_z.I;
		VoxelPixelDeltaTable[z][1] = ztable[z - 1][1] + step_z.J;
	}
}

inline void Fill_Delta_Table2(VoxelFuncArgumentStruct * state)
{
	short (&ztable)[VOXEL_BITMAP_WIDTH][2] = VoxelPixelDeltaTable;

	ztable[0][0] = 0;
	ztable[0][1] = 0;

	Vector3i16 & step_z = state->TransformMatrix[3];

	for (unsigned int z = 1; z < state->ZSize; z++) {
		VoxelPixelDeltaTable[z][0] = ztable[z - 1][0] + step_z.I;
		VoxelPixelDeltaTable[z][1] = ztable[z - 1][1] + step_z.J;
	}
}

// FUNCTION: 0x00755CB0
VoxelLibrary::VoxelLibrary(void) :
	LoadFailed(false),
	LayerCount(0),
	LayerInfoCount(0),
	DataSize(0),
	LayerHeaders(NULL),
	LayerInfos(NULL),
	Data(NULL)
{
}

// FUNCTION: 0x00755CD0
VoxelLibrary::VoxelLibrary(FileClass & file, int load_file_palette) :
	LoadFailed(false),
	LayerCount(0),
	LayerInfoCount(0),
	DataSize(0),
	LayerHeaders(NULL),
	LayerInfos(NULL),
	Data(NULL)
{
	if (!Read_File(file, load_file_palette)) {
		LoadFailed = true;
	}
}

// FUNCTION: 0x007564A0
VoxelLibrary::LayerStruct const & VoxelLibrary::Get_Layer(int layer)
{
	return(LayerHeaders[layer]);
}

// FUNCTION: 0x007564B0
VoxelLibrary::LayerInfoStruct const & VoxelLibrary::Get_Layer_Info(int layer, int info)
{
	return(LayerInfos[LayerHeaders[layer].InfoIndex + info]);
}

// FUNCTION: 0x007564E0
Vector3 VoxelLibrary::Get_Bounding_Box_Center(int layer, int info)
{
	LayerInfoStruct const & layerinfo = Get_Layer_Info(layer, info);
	Vector3 v(layerinfo.BoxCorner[VOXEL_BOUNDS_TFR].X + layerinfo.BoxCorner[VOXEL_BOUNDS_BBL].X,
			layerinfo.BoxCorner[VOXEL_BOUNDS_TFR].Y + layerinfo.BoxCorner[VOXEL_BOUNDS_BBL].Y,
			layerinfo.BoxCorner[VOXEL_BOUNDS_TFR].Z + layerinfo.BoxCorner[VOXEL_BOUNDS_BBL].Z);
	return(Vector3(v.X * 0.5f, v.Y * 0.5f, v.Z * 0.5f));
}

// FUNCTION: 0x00756570
int VoxelLibrary::Memory_Used(void)
{
	return(sizeof(LayerStruct) * LayerCount) + (sizeof(LayerInfoStruct) * LayerInfoCount) + (DataSize + sizeof(VoxelLayerHeaderStruct));
}

// FUNCTION: 0x00756590
void VoxelLibrary::Render_Object(VoxelRenderStruct & voxel, Vector3 & center)
{
	LayerInfoStruct const & layerinfo = Get_Layer_Info(voxel.Layer, voxel.Info);
	int orientation = voxel.AnchorCornerIndex;

	VoxelFuncArgumentStruct arg;

	unsigned char x_size = layerinfo.XSize;
	unsigned char y_size = layerinfo.YSize;
	unsigned char z_size = layerinfo.ZSize;

	arg.XSize = x_size;
	arg.YSize = y_size;
	arg.ZSize = z_size;

	arg.StartOffset = layerinfo.StartOffset;
	arg.EndOffset = layerinfo.EndOffset;
	arg.DataOffset = layerinfo.DataOffset;

	Vector3 & corner_0 = voxel.BoxCorner[VoxelRenderOrientations[orientation].Corner0];
	Vector3 & corner_x = voxel.BoxCorner[VoxelRenderOrientations[orientation].CornerX];
	Vector3 & corner_y = voxel.BoxCorner[VoxelRenderOrientations[orientation].CornerY];
	Vector3 & corner_z = voxel.BoxCorner[VoxelRenderOrientations[orientation].CornerZ];

	arg.StrideX = VoxelRenderOrientations[orientation].XIndexStride;
	arg.StrideY = x_size * VoxelRenderOrientations[orientation].YIndexStride;
	arg.StartIndex = (x_size - 1) * VoxelRenderOrientations[orientation].ZIndexFactor + x_size * (y_size - 1) * VoxelRenderOrientations[orientation].YIndexFactor;

	arg.TransformMatrix[0].I = short(float(corner_0.X + 128 - center.X) * 256);
	arg.TransformMatrix[0].J = short(float(corner_0.Y + 128 - center.Y) * 256);
	arg.TransformMatrix[0].K = short(float(corner_0.Z + 128 - center.Z) * 256);

	arg.TransformMatrix[1].I = short(float(corner_x.X - corner_0.X) / x_size * 256);
	arg.TransformMatrix[2].I = short(float(corner_y.X - corner_0.X) / y_size * 256);
	arg.TransformMatrix[3].I = short(float(corner_z.X - corner_0.X) / z_size * 256);

	arg.TransformMatrix[1].J = short(float(corner_x.Y - corner_0.Y) / x_size * 256);
	arg.TransformMatrix[2].J = short(float(corner_y.Y - corner_0.Y) / y_size * 256);
	arg.TransformMatrix[3].J = short(float(corner_z.Y - corner_0.Y) / z_size * 256);

	if (VoxelDrawSystem::EnableZBuffer) {
		arg.TransformMatrix[1].K = short(float(corner_x.Z - corner_0.Z) / x_size * 256);
		arg.TransformMatrix[2].K = short(float(corner_y.Z - corner_0.Z) / y_size * 256);
		arg.TransformMatrix[3].K = short(float(corner_z.Z - corner_0.Z) / z_size * 256);
	}

	int funcnum = VoxelRenderOrientations[orientation].Reversed;

	if (VoxelDrawSystem::EnableZBuffer) {
		funcnum |= 2;
	}

	if (VoxelDrawSystem::EnableLighting) {
		funcnum |= 4;
	}

	if (layerinfo.NormalType == 0) {
		funcnum |= 8;
	}

	VoxelDrawFunctions[funcnum](&arg);
}

// FUNCTION: 0x00756BB0
void VoxelLibrary::Compute_Bounding_Box(void)
{
	for (unsigned i = 0; i < LayerInfoCount; i++) {
		LayerInfoStruct &layerinfo = LayerInfos[i];

		layerinfo.BoxCorner[VOXEL_BOUNDS_BFR] = Vector3(+layerinfo.XSize / 2.0f, +layerinfo.YSize / 2.0f, -layerinfo.ZSize / 2.0f);
		layerinfo.BoxCorner[VOXEL_BOUNDS_BBR] = Vector3(+layerinfo.XSize / 2.0f, -layerinfo.YSize / 2.0f, -layerinfo.ZSize / 2.0f);
		layerinfo.BoxCorner[VOXEL_BOUNDS_BBL] = Vector3(-layerinfo.XSize / 2.0f, -layerinfo.YSize / 2.0f, -layerinfo.ZSize / 2.0f);
		layerinfo.BoxCorner[VOXEL_BOUNDS_BFL] = Vector3(-layerinfo.XSize / 2.0f, +layerinfo.YSize / 2.0f, -layerinfo.ZSize / 2.0f);
		layerinfo.BoxCorner[VOXEL_BOUNDS_TFR] = Vector3(+layerinfo.XSize / 2.0f, +layerinfo.YSize / 2.0f, +layerinfo.ZSize / 2.0f);
		layerinfo.BoxCorner[VOXEL_BOUNDS_TBR] = Vector3(+layerinfo.XSize / 2.0f, -layerinfo.YSize / 2.0f, +layerinfo.ZSize / 2.0f);
		layerinfo.BoxCorner[VOXEL_BOUNDS_TBL] = Vector3(-layerinfo.XSize / 2.0f, -layerinfo.YSize / 2.0f, +layerinfo.ZSize / 2.0f);
		layerinfo.BoxCorner[VOXEL_BOUNDS_TFL] = Vector3(-layerinfo.XSize / 2.0f, +layerinfo.YSize / 2.0f, +layerinfo.ZSize / 2.0f);
	}
}

// FUNCTION: 0x00756DD0
void __cdecl Draw_Voxel_Regular_Normals(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table2(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned int data_offset = ((unsigned int *)state->StartOffset)[state->StartIndex];
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned char delta = *ptr;
					ptr++;

					pixel_x += VoxelPixelDeltaTable[delta][0];
					pixel_y += VoxelPixelDeltaTable[delta][1];

					remaining -= delta;

					unsigned int run_length = *ptr;
					ptr++;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							unsigned char color_index = *ptr;
							ptr++;

							ptr++;

							unsigned int buffer_index = (pixel_x >> 8) | (pixel_y & 0xFF00);
							VoxelDrawBuffer[buffer_index] = color_index;

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							run_length--;
						}
					}

					ptr++;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x00756F80
void __cdecl Draw_Voxel_Reverse_Normals(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table1(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned int data_offset = ((unsigned int *)state->EndOffset)[state->StartIndex];
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned int run_length = *ptr;
					ptr--;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							ptr--;

							unsigned char color_index = *ptr;
							ptr--;

							unsigned int buffer_index = (pixel_x >> 8) | (pixel_y & 0xFF00);
							VoxelDrawBuffer[buffer_index] = color_index;

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							run_length--;
						}
					}

					ptr--;

					unsigned char delta = *ptr;
					ptr--;

					pixel_x += VoxelPixelDeltaTable[delta][0];
					pixel_y += VoxelPixelDeltaTable[delta][1];

					remaining -= delta;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x00757120
void __cdecl Draw_Voxel_Regular_Normals_ZBuffer(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table3(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;
	unsigned short pixel_z = state->TransformMatrix[0].K;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;
		unsigned short row_start_z = pixel_z;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;
			unsigned short column_start_z = pixel_z;
			unsigned int data_offset = ((unsigned int *)state->StartOffset)[state->StartIndex];

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned char value = *ptr;
					ptr++;

					pixel_x += VoxelPixelDeltaTable[value][0];
					pixel_y += VoxelPixelDeltaTable[value][1];
					pixel_z += state->TransformMatrix[3].K * value;

					remaining -= value;

					unsigned int run_length = *ptr;
					ptr++;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							value = *ptr;
							ptr++;

							ptr++;

							unsigned int buffer_index = (pixel_x >> 8) | (pixel_y & 0xFF00);
							if ((pixel_z >> 8) > VoxelDrawZBuffer[buffer_index]) {
								VoxelDrawZBuffer[buffer_index] = (pixel_z >> 8);
								VoxelDrawBuffer[buffer_index] = value;
								VoxelDrawBuffer[buffer_index + 1] = value;
								VoxelDrawZBuffer[buffer_index + 1] = (pixel_z >> 8);
							}

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							pixel_z += state->TransformMatrix[3].K;
							run_length--;
						}
					}

					ptr++;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			pixel_z = column_start_z + state->TransformMatrix[1].K;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		pixel_z = row_start_z + state->TransformMatrix[2].K;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x00757360
void __cdecl Draw_Voxel_Reverse_Normals_ZBuffer(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table2(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;
	unsigned short pixel_z = state->TransformMatrix[0].K;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;
		unsigned short row_start_z = pixel_z;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;
			unsigned short column_start_z = pixel_z;
			unsigned int data_offset = ((unsigned int *)state->EndOffset)[state->StartIndex];

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned int run_length = *ptr;
					ptr--;

					unsigned char value;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							ptr--;

							value = *ptr;
							ptr--;

							unsigned int buffer_index = (pixel_x >> 8) | (pixel_y & 0xFF00);
							if ((pixel_z >> 8) > VoxelDrawZBuffer[buffer_index]) {
								VoxelDrawZBuffer[buffer_index] = (pixel_z >> 8);
								VoxelDrawBuffer[buffer_index] = value;
								VoxelDrawBuffer[buffer_index + 1] = value;
								VoxelDrawZBuffer[buffer_index + 1] = (pixel_z >> 8);
							}

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							pixel_z += state->TransformMatrix[3].K;
							run_length--;
						}
					}

					ptr--;

					value = *ptr;
					ptr--;

					pixel_x += VoxelPixelDeltaTable[value][0];
					pixel_y += VoxelPixelDeltaTable[value][1];
					pixel_z += state->TransformMatrix[3].K * value;

					remaining -= value;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			pixel_z = column_start_z + state->TransformMatrix[1].K;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		pixel_z = row_start_z + state->TransformMatrix[2].K;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x00757E70
void __cdecl Draw_Voxel_Regular(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table1(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned int data_offset = ((unsigned int *)state->StartOffset)[state->StartIndex];
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned char delta = *ptr;
					ptr++;

					pixel_x += VoxelPixelDeltaTable[delta][0];
					pixel_y += VoxelPixelDeltaTable[delta][1];

					remaining -= delta;

					unsigned int run_length = *ptr;
					ptr++;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							unsigned char color_index = *ptr;
							ptr++;

							VoxelDrawBuffer[(pixel_x >> 8) | (pixel_y & 0xFF00)] = color_index;
							VoxelDrawBuffer[(pixel_x >> 8) | (pixel_y & 0xFF00) + 1] = color_index;

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							run_length--;
						}
					}

					ptr++;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x00758030
void __cdecl Draw_Voxel_Reverse(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table2(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned int data_offset = ((unsigned int *)state->EndOffset)[state->StartIndex];
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned int run_length = *ptr;
					ptr--;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							unsigned char color_index = *ptr;
							ptr--;

							VoxelDrawBuffer[(pixel_x >> 8) | (pixel_y & 0xFF00)] = color_index;
							VoxelDrawBuffer[(pixel_x >> 8) | (pixel_y & 0xFF00) + 1] = color_index;

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							run_length--;
						}
					}

					ptr--;

					unsigned char delta = *ptr;
					ptr--;

					pixel_x += VoxelPixelDeltaTable[delta][0];
					pixel_y += VoxelPixelDeltaTable[delta][1];

					remaining -= delta;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x007581F0
void __cdecl Draw_Voxel_Regular_ZBuffer(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table2(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;
	unsigned short pixel_z = state->TransformMatrix[0].K;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;
		unsigned short row_start_z = pixel_z;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;
			unsigned short column_start_z = pixel_z;
			unsigned int data_offset = ((unsigned int *)state->StartOffset)[state->StartIndex];

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned char value = *ptr;
					ptr++;

					pixel_x += VoxelPixelDeltaTable[value][0];
					pixel_y += VoxelPixelDeltaTable[value][1];
					pixel_z += state->TransformMatrix[3].K * value;

					remaining -= value;

					unsigned int run_length = *ptr;
					ptr++;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							value = *ptr;
							ptr++;

							unsigned int buffer_index = (pixel_x >> 8) | (pixel_y & 0xFF00);
							if ((pixel_z >> 8) > VoxelDrawZBuffer[buffer_index]) {
								VoxelDrawZBuffer[buffer_index] = (pixel_z >> 8);
								VoxelDrawBuffer[buffer_index] = value;
								VoxelDrawBuffer[buffer_index + 1] = value;
								VoxelDrawZBuffer[buffer_index + 1] = (pixel_z >> 8);
							}

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							pixel_z += state->TransformMatrix[3].K;
							run_length--;
						}
					}

					ptr++;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			pixel_z = column_start_z + state->TransformMatrix[1].K;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		pixel_z = row_start_z + state->TransformMatrix[2].K;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x00758430
void __cdecl Draw_Voxel_Reverse_ZBuffer(VoxelFuncArgumentStruct * state)
{
	Fill_Delta_Table2(state);

	unsigned short pixel_x = state->TransformMatrix[0].I;
	unsigned short pixel_y = state->TransformMatrix[0].J;
	unsigned short pixel_z = state->TransformMatrix[0].K;

	for (unsigned int y = 0; y < state->YSize; y++) {
		unsigned int base_index = state->StartIndex;
		unsigned short row_start_x = pixel_x;
		unsigned short row_start_y = pixel_y;
		unsigned short row_start_z = pixel_z;

		for (unsigned int x = 0; x < state->XSize; x++) {
			unsigned short column_start_x = pixel_x;
			unsigned short column_start_y = pixel_y;
			unsigned short column_start_z = pixel_z;
			unsigned int data_offset = ((unsigned int *)state->EndOffset)[state->StartIndex];

			if (data_offset != UINT_MAX) {
				unsigned char * ptr = state->DataOffset + data_offset;
				unsigned int remaining = state->ZSize;

				while (remaining) {
					unsigned int run_length = *ptr;
					ptr--;

					unsigned char value;

					if (run_length) {
						remaining -= run_length;
						while (run_length) {
							value = *ptr;
							ptr--;

							unsigned int buffer_index = (pixel_x >> 8) | (pixel_y & 0xFF00);
							if ((pixel_z >> 8) > VoxelDrawZBuffer[buffer_index]) {
								VoxelDrawZBuffer[buffer_index] = (pixel_z >> 8);
								VoxelDrawBuffer[buffer_index] = value;
								VoxelDrawBuffer[buffer_index + 1] = value;
								VoxelDrawZBuffer[buffer_index + 1] = (pixel_z >> 8);
							}

							pixel_x += state->TransformMatrix[3].I;
							pixel_y += state->TransformMatrix[3].J;
							pixel_z += state->TransformMatrix[3].K;
							run_length--;
						}
					}

					ptr--;

					value = *ptr;
					ptr--;

					pixel_x += VoxelPixelDeltaTable[value][0];
					pixel_y += VoxelPixelDeltaTable[value][1];
					pixel_z += state->TransformMatrix[3].K * value;

					remaining -= value;
				}
			}

			pixel_x = column_start_x + state->TransformMatrix[1].I;
			pixel_y = column_start_y + state->TransformMatrix[1].J;
			pixel_z = column_start_z + state->TransformMatrix[1].K;
			state->StartIndex = state->StrideX + state->StartIndex;
		}

		pixel_x = row_start_x + state->TransformMatrix[2].I;
		pixel_y = row_start_y + state->TransformMatrix[2].J;
		pixel_z = row_start_z + state->TransformMatrix[2].K;
		state->StartIndex = state->StrideY + base_index;
	}
}

// FUNCTION: 0x00758930
void Init_Normal_Lookup(void)
{
	memset(VoxelNormalTranslateTable, VOXEL_PALETTE_LOOKUP_NEUTRAL, VOXEL_PALETTE_SIZE);
}
