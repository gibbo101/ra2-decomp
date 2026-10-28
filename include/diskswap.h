// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "diskid.hh"

class DiskSwap
{
	public:
		virtual bool Force_Available(DiskID disk = DISK_ANY);
		virtual bool Request_Disk(DiskID disk);
		virtual bool Swap(DiskID disk);

		static DiskID Get_Disk_ID(int cd_drive, int timeout);
};
