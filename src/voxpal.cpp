// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "voxlib.h"
#include "wwfile.h"

// FUNCTION: 0x00758950
VoxelPaletteLibrary::VoxelPaletteLibrary(RGBStruct * rgb, void * lut)
{
	if (rgb == NULL) {
		Colors = new RGBStruct[VPL_NUM_COLORS];
		ColorsAllocated = TRUE;
	} else {
		Colors = rgb;
		ColorsAllocated = FALSE;
	}

	if (lut == NULL) {
		LUT = (unsigned char *)new unsigned int[VPL_NUM_COLORS*VPL_NUM_ENTRIES];
		LUTAllocated = TRUE;
	} else {
		LUT = (unsigned char *)lut;
		LUTAllocated = FALSE;
	}
}

// FUNCTION: 0x007589C0
VoxelPaletteLibrary::~VoxelPaletteLibrary(void)
{
	if (ColorsAllocated) {
		delete [] Colors;
	}
	if (LUTAllocated) {
		delete [] LUT;
	}
}

// FUNCTION: 0x007589F0
int VoxelPaletteLibrary::Read_Colors(FileClass & file)
{
	int result = 0;

	file.Open(FileClass::READ);
	if (file.Read(Colors, VPL_NUM_COLORS*sizeof(RGBStruct)) != (VPL_NUM_COLORS*sizeof(RGBStruct))) {
		result = 1;
	}

	file.Close();

	return(result);
}

// FUNCTION: 0x00758A30
int VoxelPaletteLibrary::Read_File(FileClass & file)
{
	VPLHeaderStruct hdr;
	int result = 0;

	if (result == 0) {
		if (!file.Open(FileClass::READ)) {
			result = 1;
		}
	}

	if (result == 0) {
		if (file.Read(&hdr, sizeof(hdr)) != sizeof(hdr)) {
			result = 1;
		}
	}

	if (result == 0) {
		Header.RemapStart = hdr.RemapStart;
		Header.RemapEnd = hdr.RemapEnd;
		Header.LUTCount = hdr.LUTCount;
		Header.Unused = hdr.Unused;

		if (file.Read(Colors, VPL_NUM_COLORS*sizeof(*Colors)) != (VPL_NUM_COLORS*sizeof(*Colors))) {
			result = 1;
		}
	}

	if (result == 0) {
		if (file.Read(LUT, Header.LUTCount*VPL_NUM_COLORS) != (Header.LUTCount*VPL_NUM_COLORS)) {
			result = 1;
		}
	}

	file.Close();

	return(result);
}

// FUNCTION: 0x00758AD0
int VoxelPaletteLibrary::Write_File(FileClass & file)
{
	VPLHeaderStruct hdr;
	int result = 0;

	if (result == 0) {
		if (!file.Open(FileClass::WRITE)) {
			result = 1;
		}
	}

	if (result == 0) {
		hdr.RemapStart = Header.RemapStart;
		hdr.RemapEnd = Header.RemapEnd;
		hdr.LUTCount = Header.LUTCount;
		hdr.Unused = Header.Unused;

		if (file.Write(&hdr, sizeof(hdr)) != sizeof(hdr)) {
			result = 1;
		}
	}

	if (result == 0) {
		if (file.Write(Colors, VPL_NUM_COLORS*sizeof(*Colors)) != (VPL_NUM_COLORS*sizeof(*Colors))) {
			result = 1;
		}
	}

	if (result == 0) {
		if (file.Write(LUT, Header.LUTCount*VPL_NUM_COLORS) != (Header.LUTCount*VPL_NUM_COLORS)) {
			result = 1;
		}
	}

	file.Close();

	return(result);
}

// FUNCTION: 0x00758B70
void VoxelPaletteLibrary::Calculate_Lookup_Table(float *scale, int lut_count)
{
	int index;
	int entry;
	int pos;

	#define STEPS (1.0f / (VPL_NUM_ENTRIES/2))

	static float _f[VPL_NUM_ENTRIES] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	static const float _addend = 0.6f;

	float *shades;

	if (scale && lut_count) {
		Header.LUTCount = lut_count;
		shades = scale;

	} else {
		shades = _f;

		Header.LUTCount = VPL_NUM_ENTRIES;

		for (index = 0; index < VPL_NUM_ENTRIES/2; index++) {
			_f[index] = (index * STEPS) * 0.8f + _addend;
		}

		for (index = VPL_NUM_ENTRIES/2; index < VPL_NUM_ENTRIES; index++) {
			_f[index] = (index - 16) * STEPS + index * STEPS + 0.4f;
		}
	}

	bool remap;

	remap = false;
	for (entry = 0; entry < VPL_NUM_ENTRIES/2; entry++) {
		for (index = 1; index < VPL_NUM_COLORS; index++) {
			remap = index >= Header.RemapStart && index <= Header.RemapEnd;

			float red = Colors[index].Red;
			red *= shades[entry];
			float green = Colors[index].Green;
			green *= shades[entry];
			float blue = Colors[index].Blue;
			blue *= shades[entry];

			pos = (entry << 8) | index;
			LUT[pos] = Closest_Color(red, green, blue, remap);
		}
	}

	remap = false;
	for (entry = VPL_NUM_ENTRIES/2; entry < VPL_NUM_ENTRIES; entry++) {
		for (index = 1; index < VPL_NUM_COLORS; index++) {
			remap = index >= Header.RemapStart && index <= Header.RemapEnd;

			float red = Colors[index].Red;
			red *= shades[entry];
			float green = Colors[index].Green;
			green *= shades[entry];
			float blue = Colors[index].Blue;
			blue *= shades[entry];

			red = red <= 255 ? red : 255;
			green = green <= 255 ? green : 255;
			blue = blue <= 255 ? blue : 255;

			pos = (entry << 8) | index;
			LUT[pos] = Closest_Color(red, green, blue, remap);
		}
	}

	#undef STEPS
}

// FUNCTION: 0x00758E10
unsigned char VoxelPaletteLibrary::Closest_Color(float red, float green, float blue) const
{
	int closest = 0;
	float value = 100000.0f;

	int r, g, b = 0;
	for (int index = 0; index < VPL_NUM_COLORS; index++) {
		r = Colors[index].Red;
		float fr = red	 - r;

		g = Colors[index].Green;
		float fg = green - g;

		b = Colors[index].Blue;
		float fb = blue  - b;

		float difference = fr * fr + fg * fg + fb * fb;
		if (difference < value) {
			value = difference;
			closest = index;
		}
	}

	return(closest);
}

// FUNCTION: 0x00758EA0
unsigned char VoxelPaletteLibrary::Closest_Color(float red, float green, float blue, bool is_remap) const
{
	int closest = is_remap ? Header.RemapStart : 1;
	float value = 100000.0f;

	for (int index = 1; index < VPL_NUM_COLORS; index++) {
		if (is_remap) {
			if (index < Header.RemapStart || index > Header.RemapEnd) {
				continue;
			}

		} else if (index >= Header.RemapStart && index <= Header.RemapEnd) {
			continue;
		}

		float dred = red	- Colors[index].Red;
		float dgreen = green - Colors[index].Green;
		float dblue = blue	- Colors[index].Blue;

		float difference = dred * dred + dgreen * dgreen + dblue * dblue;
		if (difference < value) {
			value = difference;
			closest = index;
		}
	}

	return(closest);
}
