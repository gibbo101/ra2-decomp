// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dbgprint.h"
#include "playcd.h"
#include "win.h"

// FUNCTION: GAMEMD 0x004E6070
GetCDClass::GetCDClass(void)
{
	char path[] = "a:\\";

	CDCount = 0;
	CDIndex = 0;

	DebugString("GetCDClass constructor\n");

	memset(CDDrives, NO_CD_DRIVE, MAX_CD_DRIVES);

	for (char i = 'c'; i <= 'z'; i++) {
		path[0] = i;
		if (GetDriveType(path) == DRIVE_CDROM) {
			CDDrives[CDCount++] = int(i - 'a');
			DebugString("CD drive found - %c:\n", (i +'A'-'a'));
		}
	}

	if (CDCount == 0) {
		for (char i = 'a'; i <= 'b'; i++) {
			path[0] = i;
			if (GetDriveType(path) == DRIVE_CDROM) {
				CDDrives[CDCount++] = int(i - 'a');
			}
		}
	}

	if (CDCount == 0) {
		DebugString("No CD drives found\n");
	}
}
