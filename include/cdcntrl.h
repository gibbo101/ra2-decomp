// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#undef STRICT
#define  STRICT
#include <cctype>
#include <windows.h>
#include <windowsx.h>

#define MAX_CD_DRIVES 26

class CDControlClass {

	public:
		CDControlClass(void);
		~CDControlClass(void);

		void Force_CD_Eject(int drive);

		bool Lock_CD_Tray(int drive);

		bool Unlock_CD_Tray(int drive);

		void Unlock_All_CD_Trays(void);

	private:

		bool Eject_CD(char drive);
		HANDLE Open_Removable_Volume(char drive);
		bool Lock_Volume(HANDLE volume);
		bool Unlock_Volume(HANDLE volume);
		bool Dismount_Volume(HANDLE volume);
		bool Prevent_Removal_Of_Volume(HANDLE volume, bool prevent);
		bool Auto_Eject_Volume(HANDLE volume);
		bool Close_Removable_Volume(HANDLE volume);
		bool Lock_CD_Drive(char drive);
		bool Unlock_CD_Drive(char drive);

		HANDLE WINAPI Open_VWin32 (void);
		bool WINAPI Close_VWin32 (HANDLE vwin32);
		bool WINAPI Unlock_Logical_Volume (HANDLE vwin32, char drive);
		bool WINAPI Lock_Logical_Volume (HANDLE vwin32, char drive, char lock_level, short permissions);
		bool Lock_Volume_95 (HANDLE vwin32, char drive);
		bool Unlock_Volume_95 (HANDLE vwin32, char drive);
		bool Auto_Eject_Volume_95 (HANDLE vwin32, char drive);
		void Eject_CD_Win95 (char drive);
		bool Lock_CD_Drive_95(char drive);
		bool Unlock_CD_Drive_95(char drive);

		int LockCount[MAX_CD_DRIVES];
};

extern CDControlClass CDControl;

#define LOCK_TIMEOUT        500
#define LOCK_RETRIES        20

#if !defined (VWIN32_DIOC_DOS_IOCTL)
#define VWIN32_DIOC_DOS_IOCTL      1

typedef struct _DIOC_REGISTERS {
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
} DIOC_REGISTERS, *PDIOC_REGISTERS;

#endif

#define CARRY_FLAG             0x0001

#pragma pack(1)

typedef struct _PARAMBLOCK {
   BYTE bOperation;
   BYTE bNumLocks;
} PARAMBLOCK, *PPARAMBLOCK;

#pragma pack()
