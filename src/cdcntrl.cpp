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

// FUNCTION: GAMEMD 0x0047A080
void CDControlClass::Eject_CD_Win95(char drive)
{
	HANDLE vwin32 = INVALID_HANDLE_VALUE;

	drive++;
	vwin32 = Open_VWin32();

	bool drive_locked = Lock_Logical_Volume(vwin32, drive, 0, 0);
	if (!drive_locked) {
		DebugString("Unable to lock volume %c:\n", 'A' + drive - 1);
		return;
	}

	if (!Unlock_Volume_95(vwin32, drive)) {
		DebugString("Could not unlock media from drive %c:\n", 'A' + drive - 1);
		Unlock_Logical_Volume(vwin32, drive);
		return;
	}

	if (!Auto_Eject_Volume_95(vwin32, drive)) {
		DebugString("Could not eject media from drive %c:\n", 'A' + drive - 1);
	}

	Unlock_Logical_Volume(vwin32, drive);
	Close_VWin32(vwin32);
}

// FUNCTION: GAMEMD 0x0047A1B0
bool CDControlClass::Lock_CD_Drive_95(char drive)
{
	DebugString("CDControlClass::Lock_CD_Drive_95\n");

	bool retval = true;
	HANDLE vwin32 = INVALID_HANDLE_VALUE;

	DebugString("Preventing ejection on CD drive %c\n", drive + 'A');

	drive++;
	vwin32 = Open_VWin32();

	DebugString("About to lock logical volume to enable exclusive access\n");

	bool drive_locked = Lock_Logical_Volume(vwin32, drive, 0, 0);
	if (!drive_locked) {
		DebugString("Unable to lock volume %c:\n", 'A' + drive - 1);
		return (false);
	}
	DebugString("Volume locked OK\n");

	DebugString("About to prevent CD tray ejection\n");

	if (!Lock_Volume_95(vwin32, drive)) {
		DebugString("Could not lock CD tray in drive %c:\n", 'A' + drive - 1);
		retval = false;
	} else {
		DebugString("CD tray ejection disabled OK\n");
	}

	Unlock_Logical_Volume(vwin32, drive);
	Close_VWin32(vwin32);

	DebugString("CDControlClass::Lock_CD_Drive_95 returning %s\n", retval ? "true" : "false");

	return (retval);
}

// FUNCTION: GAMEMD 0x0047A2D0
bool CDControlClass::Unlock_CD_Drive_95(char drive)
{
	DebugString("CDControlClass::Unlock_CD_Drive_95\n");

	bool retval = true;
	HANDLE vwin32 = INVALID_HANDLE_VALUE;

	DebugString("Allowing ejection on CD drive %c\n", drive + 'A');

	drive++;
	vwin32 = Open_VWin32();

	DebugString("About to lock logical volume to enable exclusive access\n");

	bool drive_locked = Lock_Logical_Volume(vwin32, drive, 0, 0);
	if (!drive_locked) {
		DebugString("Unable to lock volume %c:\n", 'A' + drive - 1);
		return (false);
	}
	DebugString("Volume locked OK\n");

	DebugString("About to allow CD tray ejection\n");

	if (!Unlock_Volume_95(vwin32, drive)) {
		DebugString("Could not unlock CD tray in drive %c:\n", 'A' + drive - 1);
		retval = false;
	} else {
		DebugString("CD tray ejection enabled OK\n");
	}

	Unlock_Logical_Volume(vwin32, drive);
	Close_VWin32(vwin32);

	DebugString("CDControlClass::Unlock_CD_Drive_95 returning %s\n", retval ? "true" : "false");
	return (retval);
}

// FUNCTION: GAMEMD 0x0047A3F0
bool CDControlClass::Unlock_Volume_95(HANDLE vwin32, char drive)
{
	DIOC_REGISTERS regs = {0};
	PARAMBLOCK unlock_params = {0};
	bool result;
	DWORD cb;

	unlock_params.bOperation = 2;

	regs.reg_EAX = 0x440D;
	regs.reg_EBX = drive;
	regs.reg_ECX = MAKEWORD(0x48, 0x08);
	regs.reg_EDX = (DWORD)&unlock_params;

	result = (DeviceIoControl(vwin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0)) ? true : false;

	if (result) {
		if (regs.reg_Flags & CARRY_FLAG) {
			result = (regs.reg_EAX == 0xB0) || (regs.reg_EAX == 0x01);
		}

	} else {
		DebugString("DeviceIoControl failed to perform DOS IO control function. Error %d - %s\n", GetLastError(), Last_Error_Text());
	}

	if (!result) return (false);

	DebugString("Apparent number of locks to undo = %d\n", unlock_params.bNumLocks);

	if (unlock_params.bNumLocks == 0) {
		unlock_params.bNumLocks = 1;
	}

	for (int i = 0; i < unlock_params.bNumLocks; ++i) {
		unlock_params.bOperation = 1;
		regs.reg_EAX = 0x440D;
		regs.reg_EBX = drive;
		regs.reg_ECX = MAKEWORD(0x48, 0x08);
		regs.reg_EDX = (DWORD)&unlock_params;

		result = (DeviceIoControl(vwin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0)) ? true : false;
		if (result == false) {
			DebugString("DeviceIoControl failed to perform DOS IO control function. Error %d - %s\n", GetLastError(), Last_Error_Text());
		}

		result = result && !(regs.reg_Flags & CARRY_FLAG);
		if (!result) break;
	}
	return (result);
}

// FUNCTION: GAMEMD 0x0047A560
bool CDControlClass::Lock_Volume_95(HANDLE vwin32, char drive)
{
	DIOC_REGISTERS regs = {0};
	PARAMBLOCK unlock_params = {0};
	bool result;
	DWORD cb;

	Unlock_Volume_95(vwin32, drive);

	unlock_params.bOperation = 0;
	regs.reg_EAX = 0x440D;
	regs.reg_EBX = drive;
	regs.reg_ECX = MAKEWORD(0x48, 0x08);
	regs.reg_EDX = (DWORD)&unlock_params;

	result = (DeviceIoControl(vwin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0)) ? true : false;
	if (result == false) {
		DebugString("DeviceIoControl failed to perform DOS IO control function. Error %d - %s\n", GetLastError(), Last_Error_Text());
	}

	result = result && !(regs.reg_Flags & CARRY_FLAG);
	return (result);
}

// FUNCTION: GAMEMD 0x0047A620
bool CDControlClass::Auto_Eject_Volume_95(HANDLE vwin32, char drive)
{
	DIOC_REGISTERS regs = {0};
	bool result;
	DWORD cb;

	regs.reg_EAX = 0x440D;
	regs.reg_EBX = drive;
	regs.reg_ECX = MAKEWORD(0x49, 0x08);

	result = (DeviceIoControl(vwin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0)) ? true : false;
	if (result == false) {
		DebugString("DeviceIoControl failed to perform DOS IO control function. Error %d - %s\n", GetLastError(), Last_Error_Text());
	}

	result = result && !(regs.reg_Flags & CARRY_FLAG);

	return (result);
}

// FUNCTION: GAMEMD 0x0047A6B0
HANDLE WINAPI CDControlClass::Open_VWin32(void)
{
	HANDLE result = CreateFile(TEXT("\\\\.\\vwin32"), 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL);
	return (result);
}

// FUNCTION: GAMEMD 0x0047A6D0
bool WINAPI CDControlClass::Close_VWin32(HANDLE vwin32)
{
	return ((CloseHandle(vwin32)) ? true : false);
}
