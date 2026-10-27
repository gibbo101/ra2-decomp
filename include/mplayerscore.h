// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "house.h"

#ifdef HOUSE_NAME_MAX
#define	MPLAYER_NAME_MAX			HOUSE_NAME_MAX
#else
#define	MPLAYER_NAME_MAX			12
#endif

#define	MAX_MULTI_GAMES	4

struct MPlayerScoreType {
	wchar_t Name[MPLAYER_NAME_MAX];

	int Scheme;

	int Wins;

	int Lost[MAX_MULTI_GAMES];
	int Kills[MAX_MULTI_GAMES];

	int Built[MAX_MULTI_GAMES];

	int Score[MAX_MULTI_GAMES];
};
