// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

struct Lock
{
	volatile int count;
};

#define LockInit(l)    ((l)->count = 0)
#define LockAcquire(l) ((l)->count++)
#define LockRelease(l) { if (--(l)->count < 0) { (l)->count = 0; } }
#define Locked(l)      ((l)->count != 0)
#define NotLocked(l)   ((l)->count == 0)
