// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

typedef unsigned __int64 TimeStamp;

#define SECONDS(n) ((n) * 1000)

void InitAudioTimer(void);
TimeStamp AudioGetTime(void);
