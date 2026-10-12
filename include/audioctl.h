// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

struct AudioController
{
	void * Event;
	unsigned Stamp;
	void * EventType;
	void ** AudioIndex;
	unsigned Unused;

	void FUN_00405BE0(void);
	void FUN_00405C00(void);
	void Stop(void);
	void EndLooping(void);
};
