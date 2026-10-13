// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "motlib.h"
#include "wwfile.h"
#include "voxel.hh"

// FUNCTION: 0x005BBF90 $E2
// FUNCTION: 0x005BBFC0 $E5
// FUNCTION: 0x005BBFE0 $E8
// FUNCTION: 0x005BC000 $E11
// FUNCTION: 0x005BC020 $E14
// FUNCTION: 0x005BC040 $E17
// FUNCTION: 0x005BC060 $E20
// FUNCTION: 0x005BC0A0 $E23
// FUNCTION: 0x005BC0D0 $E26
// FUNCTION: 0x005BC100 $E29
// FUNCTION: 0x005BC130 $E32

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
