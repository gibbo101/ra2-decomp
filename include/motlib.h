// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "matrix3d.h"

#include <windows.h>

class FileClass;

class MotionLibrary
{
	public:
		MotionLibrary();
		MotionLibrary(FileClass & file);
		~MotionLibrary();

		void Clear();
		BOOL Read_File(FileClass & file);
		void Scale(float scale);

		bool Load_Failed() const { return(LoadFailed); }
		int Get_Layer_Count() const { return(LayerCount); }
		int Get_Frame_Count() const { return(FrameCount); }
		Matrix3D const & Get_Layer_Matrix(int layer, unsigned frame) const {return(LayerMatrices[layer + Get_Layer_Count() * (frame % Get_Frame_Count())]);}

	private:

		bool LoadFailed;

		unsigned int LayerCount;

		unsigned int FrameCount;

		Matrix3D * LayerMatrices;
};
