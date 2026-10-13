// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "_alpha.h"
#include "_command.h"
#include "_convert.h"
#include "_font.h"
#include "_keyboar.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_winfix.h"
#include "_zbuffer.h"
#include "aircraft.h"
#include "airctype.h"
#include "aitrig.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "app.h"
#include "assert.h"
#include "blight.h"
#include "brain.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "campaign.h"
#include "cd.h"
#include "cdcntrl.h"
#include "cell.h"
#include "classfactory.h"
#include "command.h"
#include "conquer.h"
#include "cstream.h"
#include "data.h"
#include "dbgprint.h"
#include "dllver.h"
#include "drive.h"
#include "droppod.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "empulse.h"
#include "except.h"
#include "factory.h"
#include "fly.h"
#include "fog.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "hover.h"
#include "iblowfish.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "ionblast.h"
#include "ipxmgr.h"
#include "isotype.h"
#include "jumpjet.h"
#include "language\language.h"
#include "levitate.h"
#include "light.h"
#include "lightcon.h"
#include "mech.h"
#include "mixfile.h"
#include "movie.h"
#include "msgloop.h"
#include "netdlg.h"
#include "overlay.h"
#include "overtype.h"
#include "ovrlight.h"
#include "particle.h"
#include "partsys.h"
#include "playcd.h"
#include "psystype.h"
#include "ptype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "script.h"
#include "session.h"
#include "shapeset.h"
#include "side.h"
#include "sidebar.h"
#include "smudge.h"
#include "smudtype.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "tactical.h"
#include "taction.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "team.h"
#include "teamtype.h"
#include "teleport.h"
#include "terrain.h"
#include "terrtype.h"
#include "tevent.h"
#include "theme.h"
#include "tiberium.h"
#include "trigger.h"
#include "trigtype.h"
#include "trim.h"
#include "tube.h"
#include "tunnel.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vanimtype.h"
#include "vector.h"
#include "walk.h"
#include "warhead.h"
#include "wave.h"
#include "waypoint.h"
#include "weapon.h"
#include "win.h"
#include "winstub.h"
#include "wwfont.h"
#include "wwmouse.h"
#include "zbuffer.h"
#include <conio.h>
#include <io.h>
#include <cfloat>

// FUNCTION: 0x006BB070 $E2
// FUNCTION: 0x006BB0A0 $E5
// FUNCTION: 0x006BB0C0 $E8
// FUNCTION: 0x006BB0E0 $E11
// FUNCTION: 0x006BB100 $E14
// FUNCTION: 0x006BB120 $E17
// FUNCTION: 0x006BB140 $E20
// FUNCTION: 0x006BB180 $E23
// FUNCTION: 0x006BB1B0 $E26
// FUNCTION: 0x006BB1E0 $E29
// FUNCTION: 0x006BB210 $E32
// FUNCTION: 0x006BB220 $E35

// FUNCTION: 0x006BB2E0
bool SetRegKeyValue(const LPCSTR pszKey, const LPCSTR pszSubkey, const LPCSTR pszValue)
{
	bool bOk = FALSE;
	LONG ec;
	HKEY hKey;
	TCHAR szKey[260];

	lstrcpy(szKey, pszKey);

	if (NULL != pszSubkey)
	{
		lstrcat(szKey, TEXT("\\"));
		lstrcat(szKey, pszSubkey);
	}

	ec = RegCreateKeyEx(HKEY_CLASSES_ROOT, szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	if (ERROR_SUCCESS == ec)
	{
		if (NULL != pszValue)
		{
			ec = RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)pszValue, (lstrlen(pszValue) + 1) * sizeof(TCHAR));
		}
		if (ERROR_SUCCESS == ec)
			bOk = TRUE;
		RegCloseKey(hKey);
	}

	return(bOk);
}

// FUNCTION: 0x006BB760
HRESULT UnregisterServer(void)
{
	HRESULT hr = NOERROR;
	TCHAR szCLSID[39 + 1 + 32];
	TCHAR szID[39 + 1];
	TCHAR szTemp[300];
	WCHAR temp[39 + 1];

	StringFromGUID2(CLSID_TiberianSun, temp, (sizeof(temp) / sizeof(WCHAR)) - 1);
	WideCharToMultiByte(0, WC_SEPCHARS, temp, -1, szID, (sizeof(temp) / sizeof(WCHAR)) - 1, 0, 0);
	lstrcpy(szCLSID, TEXT("CLSID\\"));
	lstrcat(szCLSID, szID);

	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT(GAME_CURVER));
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT(GAME_VCLSID));
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT(GAME_VPROGID));

	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT(GAME_CLSID));
	RegDeleteKey(HKEY_CLASSES_ROOT, TEXT(GAME_PROGID));

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("ProgID"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("VersionIndependentProgID"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("NotInsertable"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("InprocServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	wsprintf(szTemp, TEXT("%s\\%s"), szCLSID, TEXT("Typelib"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szTemp);

	RegDeleteKey(HKEY_CLASSES_ROOT, szCLSID);

	return(hr);
}

// FUNCTION: 0x006BB990
LONG Top_Level_Exception_Filter(EXCEPTION_POINTERS *e_info)
{
	return(Exception_Handler(e_info->ExceptionRecord->ExceptionCode, e_info));
}
