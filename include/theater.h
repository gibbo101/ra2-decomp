// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "theater.hh"

#define THEATER_BUFFER_SIZE	4000000L

struct TheaterDataType {
	char		Name[16];
	char		UIName[32];
	char		Root[10];
	char		IsoRoot[10];
	char		PaletteFileName[10];
	char		Suffix[4];
	char		MMExtension[4];
	char		Letter[2];
	float		RadarTerrainBrightness;
	float		Unknown5C;
	float		Unknown60;
	float		Unknown64;
	int			Unknown68;
	int			Unknown6C;
};
