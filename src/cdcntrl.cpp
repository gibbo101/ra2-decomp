// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#pragma warning(disable : 4201)
#include "always.h"
#include "cdcntrl.h"
#include "assert.h"
#include "dbgprint.h"
#include "winfix.h"
#include <cstdio>
#include <tchar.h>
#include <winioctl.h>

// FUNCTION: GAMEMD 0x00479710
CDControlClass::CDControlClass(void)
{
	for (int i = 0; i < MAX_CD_DRIVES; i++) {
		LockCount[i] = 0;
	}
}

// FUNCTION: GAMEMD 0x00479730
CDControlClass::~CDControlClass(void)
{
}

// FUNCTION: GAMEMD 0x00479740
void CDControlClass::Force_CD_Eject(int drive)
{
	if (WinVersion.Is_Win9x()) {
		Eject_CD_Win95(drive);
	} else {
		Eject_CD(drive);
	}
}

// FUNCTION: GAMEMD 0x00479900
HANDLE CDControlClass::Open_Removable_Volume(char drive)
{
	HANDLE volume;
	unsigned drivetype;
	char volume_name[8];
	char rootname[5];
	DWORD access_flags;

	_stprintf(rootname, _TEXT("%c:\\"), drive + 'A');
	drivetype = GetDriveType(rootname);

	switch (drivetype) {
		case DRIVE_REMOVABLE:
			access_flags = GENERIC_READ | GENERIC_WRITE;
			break;

		case DRIVE_CDROM:
			access_flags = GENERIC_READ;
			break;

		default:
			DebugString("Attempt to open non-removable volume for locking or ejection\n");
			return (INVALID_HANDLE_VALUE);
	}

	_stprintf(volume_name, _TEXT("\\\\.\\%c:"), drive + 'A');
	volume = CreateFile(volume_name, access_flags, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (volume == INVALID_HANDLE_VALUE) {
		DebugString("Unable to open drive %c: for ejection\n", drive + 'A');
	}
	return (volume);
}

// FUNCTION: GAMEMD 0x004799B0
bool CDControlClass::Close_Removable_Volume(HANDLE volume)
{
	return ((CloseHandle(volume)) ? true : false);
}

// FUNCTION: GAMEMD 0x004799D0
bool CDControlClass::Lock_Volume(HANDLE volume)
{
	DWORD bytes_returned;

	for (int trycount = 0; trycount < LOCK_RETRIES; trycount++) {
		if (DeviceIoControl(volume, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &bytes_returned, NULL)) {
			return (true);
		}
		DebugString("DeviceIoControl failed to lock volume. Error %d - %s\n", GetLastError(), Last_Error_Text());
		Sleep(LOCK_TIMEOUT);
	}
	return (false);
}

// FUNCTION: GAMEMD 0x00479A40
bool CDControlClass::Unlock_Volume(HANDLE volume)
{
	DWORD bytes_returned;

	for (int trycount = 0; trycount < LOCK_RETRIES; trycount++) {
		if (DeviceIoControl(volume, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &bytes_returned, NULL)) return (true);
		DebugString("DeviceIoControl failed to unlock volume. Error %d - %s\n", GetLastError(), Last_Error_Text());
		Sleep(LOCK_TIMEOUT);
	}
	return (false);
}

// FUNCTION: GAMEMD 0x00479AB0
bool CDControlClass::Dismount_Volume(HANDLE volume)
{
	DWORD bytes_returned;
	bool result = ((DeviceIoControl(volume, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &bytes_returned, NULL)) ? true : false);

	if (result == false) {
		DebugString("DeviceIoControl failed to dismount volume. Error %d - %s\n", GetLastError(), Last_Error_Text());
	}
	return (result);
}

// FUNCTION: GAMEMD 0x00479B00
bool CDControlClass::Prevent_Removal_Of_Volume(HANDLE volume, bool prevent)
{
	DWORD bytes_returned;
	PREVENT_MEDIA_REMOVAL pmrbuffer;

	pmrbuffer.PreventMediaRemoval = prevent;

	bool result = ((DeviceIoControl(volume, IOCTL_STORAGE_MEDIA_REMOVAL, &pmrbuffer, sizeof(PREVENT_MEDIA_REMOVAL), NULL, 0, &bytes_returned, NULL)) ? true : false);

	if (result == false) {
		DebugString("DeviceIoControl failed to prevent media removal. Error %d - %s\n", GetLastError(), Last_Error_Text());
	}
	return (result);
}

// FUNCTION: GAMEMD 0x00479B50
bool CDControlClass::Auto_Eject_Volume(HANDLE volume)
{
	DWORD bytes_returned;
	bool result = ((DeviceIoControl(volume, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &bytes_returned, NULL)) ? true : false);

	if (result == false) {
		DebugString("DeviceIoControl failed to eject media. Error %d - %s\n", GetLastError(), Last_Error_Text());
	}
	return (result);
}

// FUNCTION: GAMEMD 0x00479BA0
bool CDControlClass::Eject_CD(char drive)
{
	HANDLE volume;
	bool ejected = false;

	volume = Open_Removable_Volume(drive);

	if (volume == INVALID_HANDLE_VALUE) return (false);

	if (Lock_Volume(volume) && Dismount_Volume(volume)) {
		if (Prevent_Removal_Of_Volume(volume, false) && Auto_Eject_Volume(volume)) {
			ejected = true;
		}
	}

	Close_Removable_Volume(volume);

	return (ejected);
}

// FUNCTION: GAMEMD 0x00479D60
bool CDControlClass::Lock_CD_Drive(char drive)
{
	HANDLE volume;
	bool retval = false;

	volume = Open_Removable_Volume(drive);

	if (volume == INVALID_HANDLE_VALUE) return (false);

	if (Lock_Volume(volume)) {
		if (Prevent_Removal_Of_Volume(volume, true)) {
			retval = true;
		}
	}

	Unlock_Volume(volume);
	Close_Removable_Volume(volume);

	return (true);
}

// FUNCTION: GAMEMD 0x00479EF0
bool CDControlClass::Unlock_CD_Drive(char drive)
{
	HANDLE volume;
	bool retval = false;

	volume = Open_Removable_Volume(drive);

	if (volume == INVALID_HANDLE_VALUE) return (false);

	if (Lock_Volume(volume)) {
		if (Prevent_Removal_Of_Volume(volume, false)) {
			retval = true;
		}
	}

	Unlock_Volume(volume);
	Close_Removable_Volume(volume);

	return (true);
}
