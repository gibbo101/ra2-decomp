// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "globals.h"
#include "version.h"

namespace CopyProtection {

// FUNCTION: GAMEMD 0x0049F920
// CopyProtection::CheckVersion
bool __fastcall CheckVersion(void)
{
	HKEY key;
	DWORD size;
	DWORD type;
	DWORD value;

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, "WChat\\SysID", 0, KEY_READ, &key) != ERROR_SUCCESS) {
		return(false);
	}
	size = 4;
	LONG status = RegQueryValueEx(key, "ID", NULL, &type, (BYTE *)&value, &size);
	RegCloseKey(key);
	if (status != ERROR_SUCCESS || type != REG_DWORD) {
		return(false);
	}
	return(value == VerNum.Version_Number());
}

}
