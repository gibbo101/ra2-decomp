// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "diskswap.h"
#include "assert.h"
#include "cdfile.h"
#include "globals.h"
#include "playcd.h"
#include "stimer.h"
#include "timer.h"
#include "win.h"
#include <stdio.h>

// FUNCTION: GAMEMD 0x004A8270
bool DiskSwap::Force_Available(DiskID disk)
{
	static int _last = -1;
	int new_cd_drive = 0;

	if (disk == DISK_LOCAL) return(true);

	if (disk >= 0) disk = (DiskID)2;

	int current_drive = CDFileClass::Get_CD_Drive();
	int cd_current = DiskSwap::Get_Disk_ID(current_drive, 1*TIMER_SECOND);

	if (cd_current > DISK_ANY) {
		if (disk == cd_current || disk == DISK_ANY) {
			new_cd_drive = current_drive;
		}
	}

	if (!new_cd_drive) {
		int last_drive = CDFileClass::Get_Last_CD_Drive();

		if (last_drive && last_drive != CDFileClass::Get_CD_Drive()) {
			cd_current = DiskSwap::Get_Disk_ID(last_drive, 10*TIMER_SECOND);
			if (cd_current > DISK_ANY) {
				if (disk == cd_current || disk == DISK_ANY) {
					new_cd_drive = last_drive;
				}
			}
		}
	}

	if (!new_cd_drive) {
		int drive_search_timeout = 2*TIMER_SECOND;

		for (;;) {
			for (int i = 0 ; i < CDList.Get_Number_Of_Drives(); i++) {
				int cd_drive = CDList.Get_Next_CD_Drive();
				int cd_current = DiskSwap::Get_Disk_ID(cd_drive, drive_search_timeout);

				if (cd_current > DISK_ANY) {
					if (disk == cd_current || disk == DISK_ANY || disk == DISK_LOCAL) {
						new_cd_drive = cd_drive;
						break;
					}
				}
			}

			if (new_cd_drive > DISK_FIRST) {
				CDFileClass::Set_CD_Drive(new_cd_drive);
				CDFileClass::Refresh_Search_Drives();
				break;
			}

			if (!Request_Disk(disk)) {
				new_cd_drive = DISK_FIRST;
				break;
			}

			drive_search_timeout = 5*TIMER_SECOND;
		}
	}

	if (!new_cd_drive) {
		return(false);
	}

	if (disk > DISK_ANY && _last != disk) {
		_last = disk;
		return(Swap(disk));
	}

	return(true);
}

// FUNCTION: GAMEMD 0x004A83D0
bool DiskSwap::Swap(DiskID disk)
{
	char str[256];

	sprintf(str, "Please insert disk %d", disk);
	if (MessageBox(NULL, str, "Hey!", MB_OKCANCEL) == IDOK) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004A8410
bool DiskSwap::Request_Disk(DiskID disk)
{
	return(true);
}
