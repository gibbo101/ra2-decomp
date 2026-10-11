// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "motlib.h"
#include "wwfile.h"
#include "voxel.hh"

// FUNCTION: 0x005BD570
MotionLibrary::MotionLibrary(FileClass & file) :
	LoadFailed(false),
	LayerCount(0),
	FrameCount(0),
	LayerMatrices(NULL)
{
	if (!Read_File(file)) {
		LoadFailed = true;
	}
}

// FUNCTION: 0x005BD730
void MotionLibrary::Scale(float scale)
{
	for (unsigned frame = 0; frame < FrameCount; frame++) {
		for (unsigned layer = 0; layer < LayerCount; layer++) {
			LayerMatrices[layer + frame * LayerCount][0].W = scale * LayerMatrices[layer + frame * LayerCount][0].W;
			LayerMatrices[layer + frame * LayerCount][1].W = scale * LayerMatrices[layer + frame * LayerCount][1].W;
			LayerMatrices[layer + frame * LayerCount][2].W = scale * LayerMatrices[layer + frame * LayerCount][2].W;
		}
	}
}
