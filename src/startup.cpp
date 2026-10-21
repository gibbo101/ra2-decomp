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
#include "airstrike.h"
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
#include "capmgr.h"
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
#include "parasite.h"
#include "bomb.h"
#include "radsite.h"
#include "disklaser.h"
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
#include "slavemgr.h"
#include "smudge.h"
#include "smudtype.h"
#include "spawnmgr.h"
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
#include "temporal.h"
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

struct SHPStruct
{
	char Data[0x24];
	~SHPStruct();
};

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
// FUNCTION: 0x006BB240 $E38
// FUNCTION: 0x006BB260 $E329
// FUNCTION: 0x006BB2A0 $E327

namespace DirectDrawWrap { extern bool DoSmth; void DisposeOfStuff(void); }
void FUN_004C19B0(void);
extern int ReadyToQuit;
extern SHPStruct * DAT_00B0BCF0;
extern bool DAT_00B0BCF4;

DWORD dwRegisterMap = -1;

DynamicVectorClass<DWORD> RegisteredClasses;

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

// FUNCTION: 0x006BB390
HRESULT RegisterServer(void)
{
	HRESULT hr = NOERROR;
	TCHAR szID[39 + 1];
	TCHAR szCLSID[48];
	TCHAR szLibID[39 + 1];
	WCHAR temp[39 + 1];
	TCHAR szModulePath[MAX_PATH];

	GetModuleFileName(ProgramInstance, szModulePath, sizeof(szModulePath) / sizeof(TCHAR));
	StringFromGUID2(CLSID_TiberianSun, temp, (sizeof(temp) / sizeof(WCHAR)) - 1);
	WideCharToMultiByte(0, 0, temp, -1, szID, (sizeof(temp) / sizeof(WCHAR)) - 1, 0, 0);
	StringFromGUID2(LIBID_TiberianSun, temp, (sizeof(temp) / sizeof(WCHAR)) - 1);
	WideCharToMultiByte(0, 0, temp, -1, szLibID, (sizeof(temp) / sizeof(WCHAR)) - 1, 0, 0);

	lstrcpy(szCLSID, TEXT("CLSID\\"));
	lstrcat(szCLSID, szID);

	SetRegKeyValue(TEXT(GAME_PROGID), NULL, GAME_VERNAME);
	SetRegKeyValue(TEXT(GAME_PROGID), TEXT("CLSID"), szID);

	SetRegKeyValue(TEXT(GAME_VPROGID), NULL, GAME_VERNAME);
	SetRegKeyValue(TEXT(GAME_VPROGID), TEXT("CurVer"), TEXT(GAME_PROGID));
	SetRegKeyValue(TEXT(GAME_VPROGID), TEXT("CLSID"), szID);

	SetRegKeyValue(szCLSID, NULL, GAME_VERNAME);
	SetRegKeyValue(szCLSID, TEXT("ProgID"), TEXT(GAME_PROGID));
	SetRegKeyValue(szCLSID, TEXT("VersionIndependentProgID"), TEXT(GAME_VPROGID));
	SetRegKeyValue(szCLSID, TEXT("NotInsertable"), NULL);
	SetRegKeyValue(szCLSID, TEXT("InprocServer32"), szModulePath);
	SetRegKeyValue(szCLSID, TEXT("Typelib"), szLibID);

	ITypeLib * pITypeLib = NULL;
	LoadTypeLib(L"RA2.TLB", &pITypeLib);
	if (pITypeLib != NULL) {
		pITypeLib->Release();
	}

	return(hr);
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

// FUNCTION: 0x006BB8E0 DirectDrawWrap::DisposeOfStuff
void DirectDrawWrap::DisposeOfStuff(void)
{
	static bool surfaces_reset = false;

	if (DirectDrawWrap::DoSmth) {
		FUN_004C19B0();
	}

	if (!surfaces_reset) {
		if (HiddenSurface) {
			delete HiddenSurface;
			HiddenSurface = NULL;
		}
		if (AlternateSurface) {
			delete AlternateSurface;
			AlternateSurface = NULL;
		}
		if (TileSurface) {
			delete TileSurface;
			TileSurface = NULL;
		}
		if (SidebarSurface) {
			delete SidebarSurface;
			SidebarSurface = NULL;
		}
		if (CompositeSurface) {
			delete CompositeSurface;
			CompositeSurface = NULL;
		}
		if (VisibleSurface) {
			delete VisibleSurface;
			VisibleSurface = NULL;
		}

		Reset_Video_Mode();

		surfaces_reset = true;
	}
}

// FUNCTION: 0x006BB990
LONG Top_Level_Exception_Filter(EXCEPTION_POINTERS *e_info)
{
	return(Exception_Handler(e_info->ExceptionRecord->ExceptionCode, e_info));
}

// FUNCTION: 0x006BEC50
void Exit_Process(int code)
{
	exit(code);
}

// FUNCTION: 0x006BEC60
void Emergency_Exit(void)
{
	ReadyToQuit = 1;

	PostMessage(MainWindow, WM_DESTROY, 0, 0);

	while (MainWindow) {
		Windows_Message_Handler();
		if (ReadyToQuit != 1) {
			break;
		}
	}

	if (dwRegisterMap != -1) {
		RevokeActiveObject(dwRegisterMap, NULL);
		dwRegisterMap = -1;
	}

	CoDisconnectObject((LPUNKNOWN)&ApplicationInstance, 0);

	OleUninitialize();

	if (MouseCursor) {
		MouseCursor->Release_Mouse();
		delete MouseCursor;
	}
	MouseCursor = NULL;

	if (DAT_00B0BCF4) {
		if (DAT_00B0BCF0) {
			delete [] DAT_00B0BCF0;
		}
		DAT_00B0BCF4 = false;
	}

	PostQuitMessage(EXIT_SUCCESS);

	Shutdown_Network();

	Prog_End();
}

// FUNCTION: 0x006BED50 ?$DynamicVectorClass@K::Clear
// FUNCTION: 0x006BED80 ?$VectorClass@K::ID
// FUNCTION: 0x006BEDA0 ?$VectorClass@K::GetItem
template class DynamicVectorClass<DWORD>;

// FUNCTION: 0x006BEE40 ?$TClassFactory@VCStreamClass@@::?$TClassFactory@VCStreamClass@@
// FUNCTION: 0x006BEE50 ?$TClassFactory@VCStreamClass@@::QueryInterface
// FUNCTION: 0x006BEEC0 ?$TClassFactory@VCStreamClass@@::AddRef
// FUNCTION: 0x006BEEE0 ?$TClassFactory@VCStreamClass@@::Release
// FUNCTION: 0x006BEF10 ?$TClassFactory@VCStreamClass@@::CreateInstance
// FUNCTION: 0x006BEF90 ?$TClassFactory@VCStreamClass@@::LockServer
template class TClassFactory<CStreamClass>;

// FUNCTION: 0x006BEFC0 ?$TClassFactory@VWaveClass@@::?$TClassFactory@VWaveClass@@
// FUNCTION: 0x006BEFD0 ?$TClassFactory@VWaveClass@@::QueryInterface
// FUNCTION: 0x006BF040 ?$TClassFactory@VWaveClass@@::AddRef
// FUNCTION: 0x006BF060 ?$TClassFactory@VWaveClass@@::Release
// FUNCTION: 0x006BF090 ?$TClassFactory@VWaveClass@@::CreateInstance
// FUNCTION: 0x006BF110 ?$TClassFactory@VWaveClass@@::LockServer
template class TClassFactory<WaveClass>;

// FUNCTION: 0x006BF140 ?$TClassFactory@VTerrainTypeClass@@::?$TClassFactory@VTerrainTypeClass@@
// FUNCTION: 0x006BF150 ?$TClassFactory@VTerrainTypeClass@@::QueryInterface
// FUNCTION: 0x006BF1C0 ?$TClassFactory@VTerrainTypeClass@@::AddRef
// FUNCTION: 0x006BF1E0 ?$TClassFactory@VTerrainTypeClass@@::Release
// FUNCTION: 0x006BF210 ?$TClassFactory@VTerrainTypeClass@@::CreateInstance
// FUNCTION: 0x006BF290 ?$TClassFactory@VTerrainTypeClass@@::LockServer
template class TClassFactory<TerrainTypeClass>;

// FUNCTION: 0x006BF2C0 ?$TClassFactory@VTerrainClass@@::?$TClassFactory@VTerrainClass@@
// FUNCTION: 0x006BF2D0 ?$TClassFactory@VTerrainClass@@::QueryInterface
// FUNCTION: 0x006BF340 ?$TClassFactory@VTerrainClass@@::AddRef
// FUNCTION: 0x006BF360 ?$TClassFactory@VTerrainClass@@::Release
// FUNCTION: 0x006BF390 ?$TClassFactory@VTerrainClass@@::CreateInstance
// FUNCTION: 0x006BF410 ?$TClassFactory@VTerrainClass@@::LockServer
template class TClassFactory<TerrainClass>;

// FUNCTION: 0x006BF440 ?$TClassFactory@VSuperWeaponTypeClass@@::?$TClassFactory@VSuperWeaponTypeClass@@
// FUNCTION: 0x006BF450 ?$TClassFactory@VSuperWeaponTypeClass@@::QueryInterface
// FUNCTION: 0x006BF4C0 ?$TClassFactory@VSuperWeaponTypeClass@@::AddRef
// FUNCTION: 0x006BF4E0 ?$TClassFactory@VSuperWeaponTypeClass@@::Release
// FUNCTION: 0x006BF510 ?$TClassFactory@VSuperWeaponTypeClass@@::CreateInstance
// FUNCTION: 0x006BF590 ?$TClassFactory@VSuperWeaponTypeClass@@::LockServer
template class TClassFactory<SuperWeaponTypeClass>;

// FUNCTION: 0x006BF5C0 ?$TClassFactory@VSuperClass@@::?$TClassFactory@VSuperClass@@
// FUNCTION: 0x006BF5D0 ?$TClassFactory@VSuperClass@@::QueryInterface
// FUNCTION: 0x006BF640 ?$TClassFactory@VSuperClass@@::AddRef
// FUNCTION: 0x006BF660 ?$TClassFactory@VSuperClass@@::Release
// FUNCTION: 0x006BF690 ?$TClassFactory@VSuperClass@@::CreateInstance
// FUNCTION: 0x006BF710 ?$TClassFactory@VSuperClass@@::LockServer
template class TClassFactory<SuperClass>;

// FUNCTION: 0x006BF740 ?$TClassFactory@VTactical@@::?$TClassFactory@VTactical@@
// FUNCTION: 0x006BF750 ?$TClassFactory@VTactical@@::QueryInterface
// FUNCTION: 0x006BF7C0 ?$TClassFactory@VTactical@@::AddRef
// FUNCTION: 0x006BF7E0 ?$TClassFactory@VTactical@@::Release
// FUNCTION: 0x006BF810 ?$TClassFactory@VTactical@@::CreateInstance
// FUNCTION: 0x006BF890 ?$TClassFactory@VTactical@@::LockServer
template class TClassFactory<Tactical>;

// FUNCTION: 0x006BF8C0 ?$TClassFactory@VCellClass@@::?$TClassFactory@VCellClass@@
// FUNCTION: 0x006BF8D0 ?$TClassFactory@VCellClass@@::QueryInterface
// FUNCTION: 0x006BF940 ?$TClassFactory@VCellClass@@::AddRef
// FUNCTION: 0x006BF960 ?$TClassFactory@VCellClass@@::Release
// FUNCTION: 0x006BF990 ?$TClassFactory@VCellClass@@::CreateInstance
// FUNCTION: 0x006BFA10 ?$TClassFactory@VCellClass@@::LockServer
template class TClassFactory<CellClass>;

// FUNCTION: 0x006BFA40 ?$TClassFactory@VEMPulseClass@@::?$TClassFactory@VEMPulseClass@@
// FUNCTION: 0x006BFA50 ?$TClassFactory@VEMPulseClass@@::QueryInterface
// FUNCTION: 0x006BFAC0 ?$TClassFactory@VEMPulseClass@@::AddRef
// FUNCTION: 0x006BFAE0 ?$TClassFactory@VEMPulseClass@@::Release
// FUNCTION: 0x006BFB10 ?$TClassFactory@VEMPulseClass@@::CreateInstance
// FUNCTION: 0x006BFB90 ?$TClassFactory@VEMPulseClass@@::LockServer
template class TClassFactory<EMPulseClass>;

// FUNCTION: 0x006BFBC0 ?$TClassFactory@VLightSourceClass@@::?$TClassFactory@VLightSourceClass@@
// FUNCTION: 0x006BFBD0 ?$TClassFactory@VLightSourceClass@@::QueryInterface
// FUNCTION: 0x006BFC40 ?$TClassFactory@VLightSourceClass@@::AddRef
// FUNCTION: 0x006BFC60 ?$TClassFactory@VLightSourceClass@@::Release
// FUNCTION: 0x006BFC90 ?$TClassFactory@VLightSourceClass@@::CreateInstance
// FUNCTION: 0x006BFD10 ?$TClassFactory@VLightSourceClass@@::LockServer
template class TClassFactory<LightSourceClass>;

// FUNCTION: 0x006BFD40 ?$TClassFactory@VSideClass@@::?$TClassFactory@VSideClass@@
// FUNCTION: 0x006BFD50 ?$TClassFactory@VSideClass@@::QueryInterface
// FUNCTION: 0x006BFDC0 ?$TClassFactory@VSideClass@@::AddRef
// FUNCTION: 0x006BFDE0 ?$TClassFactory@VSideClass@@::Release
// FUNCTION: 0x006BFE10 ?$TClassFactory@VSideClass@@::CreateInstance
// FUNCTION: 0x006BFE90 ?$TClassFactory@VSideClass@@::LockServer
template class TClassFactory<SideClass>;

// FUNCTION: 0x006BFEC0 ?$TClassFactory@VTiberiumClass@@::?$TClassFactory@VTiberiumClass@@
// FUNCTION: 0x006BFED0 ?$TClassFactory@VTiberiumClass@@::QueryInterface
// FUNCTION: 0x006BFF40 ?$TClassFactory@VTiberiumClass@@::AddRef
// FUNCTION: 0x006BFF60 ?$TClassFactory@VTiberiumClass@@::Release
// FUNCTION: 0x006BFF90 ?$TClassFactory@VTiberiumClass@@::CreateInstance
// FUNCTION: 0x006C0010 ?$TClassFactory@VTiberiumClass@@::LockServer
template class TClassFactory<TiberiumClass>;

// FUNCTION: 0x006C0040 ?$TClassFactory@VTubeClass@@::?$TClassFactory@VTubeClass@@
// FUNCTION: 0x006C0050 ?$TClassFactory@VTubeClass@@::QueryInterface
// FUNCTION: 0x006C00C0 ?$TClassFactory@VTubeClass@@::AddRef
// FUNCTION: 0x006C00E0 ?$TClassFactory@VTubeClass@@::Release
// FUNCTION: 0x006C0110 ?$TClassFactory@VTubeClass@@::CreateInstance
// FUNCTION: 0x006C0190 ?$TClassFactory@VTubeClass@@::LockServer
template class TClassFactory<TubeClass>;

// FUNCTION: 0x006C01C0 ?$TClassFactory@VCampaignClass@@::?$TClassFactory@VCampaignClass@@
// FUNCTION: 0x006C01D0 ?$TClassFactory@VCampaignClass@@::QueryInterface
// FUNCTION: 0x006C0240 ?$TClassFactory@VCampaignClass@@::AddRef
// FUNCTION: 0x006C0260 ?$TClassFactory@VCampaignClass@@::Release
// FUNCTION: 0x006C0290 ?$TClassFactory@VCampaignClass@@::CreateInstance
// FUNCTION: 0x006C0310 ?$TClassFactory@VCampaignClass@@::LockServer
template class TClassFactory<CampaignClass>;

// FUNCTION: 0x006C0340 ?$TClassFactory@VBuildingLightClass@@::?$TClassFactory@VBuildingLightClass@@
// FUNCTION: 0x006C0350 ?$TClassFactory@VBuildingLightClass@@::QueryInterface
// FUNCTION: 0x006C03C0 ?$TClassFactory@VBuildingLightClass@@::AddRef
// FUNCTION: 0x006C03E0 ?$TClassFactory@VBuildingLightClass@@::Release
// FUNCTION: 0x006C0410 ?$TClassFactory@VBuildingLightClass@@::CreateInstance
// FUNCTION: 0x006C0490 ?$TClassFactory@VBuildingLightClass@@::LockServer
template class TClassFactory<BuildingLightClass>;

// FUNCTION: 0x006BEDB0 ?$_com_ptr_t@V?$_com_IIID@UIUnknown@@$1?_GUID_00000000_0000_0000_c000_000000000046@@3U__s_GUID@@A@@::CreateInstance
template class _com_ptr_t<_com_IIID<IUnknown, &__uuidof(IUnknown)> >;

// FUNCTION: 0x006C04C0 ?$TClassFactory@VWaypointPathClass@@::?$TClassFactory@VWaypointPathClass@@
// FUNCTION: 0x006C04D0 ?$TClassFactory@VWaypointPathClass@@::QueryInterface
// FUNCTION: 0x006C0540 ?$TClassFactory@VWaypointPathClass@@::AddRef
// FUNCTION: 0x006C0560 ?$TClassFactory@VWaypointPathClass@@::Release
// FUNCTION: 0x006C0590 ?$TClassFactory@VWaypointPathClass@@::CreateInstance
// FUNCTION: 0x006C0610 ?$TClassFactory@VWaypointPathClass@@::LockServer
template class TClassFactory<WaypointPathClass>;

// FUNCTION: 0x006C0640 ?$TClassFactory@VTemporalClass@@::?$TClassFactory@VTemporalClass@@
// FUNCTION: 0x006C0650 ?$TClassFactory@VTemporalClass@@::QueryInterface
// FUNCTION: 0x006C06C0 ?$TClassFactory@VTemporalClass@@::AddRef
// FUNCTION: 0x006C06E0 ?$TClassFactory@VTemporalClass@@::Release
// FUNCTION: 0x006C0710 ?$TClassFactory@VTemporalClass@@::CreateInstance
// FUNCTION: 0x006C0790 ?$TClassFactory@VTemporalClass@@::LockServer
template class TClassFactory<TemporalClass>;

// FUNCTION: 0x006C07C0 ?$TClassFactory@VAirstrikeClass@@::?$TClassFactory@VAirstrikeClass@@
// FUNCTION: 0x006C07D0 ?$TClassFactory@VAirstrikeClass@@::QueryInterface
// FUNCTION: 0x006C0840 ?$TClassFactory@VAirstrikeClass@@::AddRef
// FUNCTION: 0x006C0860 ?$TClassFactory@VAirstrikeClass@@::Release
// FUNCTION: 0x006C0890 ?$TClassFactory@VAirstrikeClass@@::CreateInstance
// FUNCTION: 0x006C0910 ?$TClassFactory@VAirstrikeClass@@::LockServer
template class TClassFactory<AirstrikeClass>;

// FUNCTION: 0x006C0940 ?$TClassFactory@VSpawnManagerClass@@::?$TClassFactory@VSpawnManagerClass@@
// FUNCTION: 0x006C0950 ?$TClassFactory@VSpawnManagerClass@@::QueryInterface
// FUNCTION: 0x006C09C0 ?$TClassFactory@VSpawnManagerClass@@::AddRef
// FUNCTION: 0x006C09E0 ?$TClassFactory@VSpawnManagerClass@@::Release
// FUNCTION: 0x006C0A10 ?$TClassFactory@VSpawnManagerClass@@::CreateInstance
// FUNCTION: 0x006C0A90 ?$TClassFactory@VSpawnManagerClass@@::LockServer
template class TClassFactory<SpawnManagerClass>;

// FUNCTION: 0x006C0AC0 ?$TClassFactory@VSlaveManagerClass@@::?$TClassFactory@VSlaveManagerClass@@
// FUNCTION: 0x006C0AD0 ?$TClassFactory@VSlaveManagerClass@@::QueryInterface
// FUNCTION: 0x006C0B40 ?$TClassFactory@VSlaveManagerClass@@::AddRef
// FUNCTION: 0x006C0B60 ?$TClassFactory@VSlaveManagerClass@@::Release
// FUNCTION: 0x006C0B90 ?$TClassFactory@VSlaveManagerClass@@::CreateInstance
// FUNCTION: 0x006C0C10 ?$TClassFactory@VSlaveManagerClass@@::LockServer
template class TClassFactory<SlaveManagerClass>;

// FUNCTION: 0x006C0C40 ?$TClassFactory@VCaptureManagerClass@@::?$TClassFactory@VCaptureManagerClass@@
// FUNCTION: 0x006C0C50 ?$TClassFactory@VCaptureManagerClass@@::QueryInterface
// FUNCTION: 0x006C0CC0 ?$TClassFactory@VCaptureManagerClass@@::AddRef
// FUNCTION: 0x006C0CE0 ?$TClassFactory@VCaptureManagerClass@@::Release
// FUNCTION: 0x006C0D10 ?$TClassFactory@VCaptureManagerClass@@::CreateInstance
// FUNCTION: 0x006C0D90 ?$TClassFactory@VCaptureManagerClass@@::LockServer
template class TClassFactory<CaptureManagerClass>;

// FUNCTION: 0x006C0DC0 ?$TClassFactory@VDiskLaserClass@@::?$TClassFactory@VDiskLaserClass@@
// FUNCTION: 0x006C0DD0 ?$TClassFactory@VDiskLaserClass@@::QueryInterface
// FUNCTION: 0x006C0E40 ?$TClassFactory@VDiskLaserClass@@::AddRef
// FUNCTION: 0x006C0E60 ?$TClassFactory@VDiskLaserClass@@::Release
// FUNCTION: 0x006C0E90 ?$TClassFactory@VDiskLaserClass@@::CreateInstance
// FUNCTION: 0x006C0F10 ?$TClassFactory@VDiskLaserClass@@::LockServer
template class TClassFactory<DiskLaserClass>;

// FUNCTION: 0x006C0F40 ?$TClassFactory@VParasiteClass@@::?$TClassFactory@VParasiteClass@@
// FUNCTION: 0x006C0F50 ?$TClassFactory@VParasiteClass@@::QueryInterface
// FUNCTION: 0x006C0FC0 ?$TClassFactory@VParasiteClass@@::AddRef
// FUNCTION: 0x006C0FE0 ?$TClassFactory@VParasiteClass@@::Release
// FUNCTION: 0x006C1010 ?$TClassFactory@VParasiteClass@@::CreateInstance
// FUNCTION: 0x006C1090 ?$TClassFactory@VParasiteClass@@::LockServer
template class TClassFactory<ParasiteClass>;

// FUNCTION: 0x006C10C0 ?$TClassFactory@VBombClass@@::?$TClassFactory@VBombClass@@
// FUNCTION: 0x006C10D0 ?$TClassFactory@VBombClass@@::QueryInterface
// FUNCTION: 0x006C1140 ?$TClassFactory@VBombClass@@::AddRef
// FUNCTION: 0x006C1160 ?$TClassFactory@VBombClass@@::Release
// FUNCTION: 0x006C1190 ?$TClassFactory@VBombClass@@::CreateInstance
// FUNCTION: 0x006C1210 ?$TClassFactory@VBombClass@@::LockServer
template class TClassFactory<BombClass>;

// FUNCTION: 0x006C1240 ?$TClassFactory@VRadSiteClass@@::?$TClassFactory@VRadSiteClass@@
// FUNCTION: 0x006C1250 ?$TClassFactory@VRadSiteClass@@::QueryInterface
// FUNCTION: 0x006C12C0 ?$TClassFactory@VRadSiteClass@@::AddRef
// FUNCTION: 0x006C12E0 ?$TClassFactory@VRadSiteClass@@::Release
// FUNCTION: 0x006C1310 ?$TClassFactory@VRadSiteClass@@::CreateInstance
// FUNCTION: 0x006C1390 ?$TClassFactory@VRadSiteClass@@::LockServer
template class TClassFactory<RadSiteClass>;

// FUNCTION: 0x006C13C0 ?$TClassFactory@VTEventClass@@::?$TClassFactory@VTEventClass@@
// FUNCTION: 0x006C13D0 ?$TClassFactory@VTEventClass@@::QueryInterface
// FUNCTION: 0x006C1440 ?$TClassFactory@VTEventClass@@::AddRef
// FUNCTION: 0x006C1460 ?$TClassFactory@VTEventClass@@::Release
// FUNCTION: 0x006C1490 ?$TClassFactory@VTEventClass@@::CreateInstance
// FUNCTION: 0x006C1510 ?$TClassFactory@VTEventClass@@::LockServer
template class TClassFactory<TEventClass>;

// FUNCTION: 0x006C1540 ?$TClassFactory@VVoxelAnimTypeClass@@::?$TClassFactory@VVoxelAnimTypeClass@@
// FUNCTION: 0x006C1550 ?$TClassFactory@VVoxelAnimTypeClass@@::QueryInterface
// FUNCTION: 0x006C15C0 ?$TClassFactory@VVoxelAnimTypeClass@@::AddRef
// FUNCTION: 0x006C15E0 ?$TClassFactory@VVoxelAnimTypeClass@@::Release
// FUNCTION: 0x006C1610 ?$TClassFactory@VVoxelAnimTypeClass@@::CreateInstance
// FUNCTION: 0x006C1690 ?$TClassFactory@VVoxelAnimTypeClass@@::LockServer
template class TClassFactory<VoxelAnimTypeClass>;

// FUNCTION: 0x006C16C0 ?$TClassFactory@VVoxelAnimClass@@::?$TClassFactory@VVoxelAnimClass@@
// FUNCTION: 0x006C16D0 ?$TClassFactory@VVoxelAnimClass@@::QueryInterface
// FUNCTION: 0x006C1740 ?$TClassFactory@VVoxelAnimClass@@::AddRef
// FUNCTION: 0x006C1760 ?$TClassFactory@VVoxelAnimClass@@::Release
// FUNCTION: 0x006C1790 ?$TClassFactory@VVoxelAnimClass@@::CreateInstance
// FUNCTION: 0x006C1810 ?$TClassFactory@VVoxelAnimClass@@::LockServer
template class TClassFactory<VoxelAnimClass>;

// FUNCTION: 0x006C1840 ?$TClassFactory@VTActionClass@@::?$TClassFactory@VTActionClass@@
// FUNCTION: 0x006C1850 ?$TClassFactory@VTActionClass@@::QueryInterface
// FUNCTION: 0x006C18C0 ?$TClassFactory@VTActionClass@@::AddRef
// FUNCTION: 0x006C18E0 ?$TClassFactory@VTActionClass@@::Release
// FUNCTION: 0x006C1910 ?$TClassFactory@VTActionClass@@::CreateInstance
// FUNCTION: 0x006C1990 ?$TClassFactory@VTActionClass@@::LockServer
template class TClassFactory<TActionClass>;

// FUNCTION: 0x006C19C0 ?$TClassFactory@VTriggerClass@@::?$TClassFactory@VTriggerClass@@
// FUNCTION: 0x006C19D0 ?$TClassFactory@VTriggerClass@@::QueryInterface
// FUNCTION: 0x006C1A40 ?$TClassFactory@VTriggerClass@@::AddRef
// FUNCTION: 0x006C1A60 ?$TClassFactory@VTriggerClass@@::Release
// FUNCTION: 0x006C1A90 ?$TClassFactory@VTriggerClass@@::CreateInstance
// FUNCTION: 0x006C1B10 ?$TClassFactory@VTriggerClass@@::LockServer
template class TClassFactory<TriggerClass>;

// FUNCTION: 0x006C1B40 ?$TClassFactory@VTriggerTypeClass@@::?$TClassFactory@VTriggerTypeClass@@
// FUNCTION: 0x006C1B50 ?$TClassFactory@VTriggerTypeClass@@::QueryInterface
// FUNCTION: 0x006C1BC0 ?$TClassFactory@VTriggerTypeClass@@::AddRef
// FUNCTION: 0x006C1BE0 ?$TClassFactory@VTriggerTypeClass@@::Release
// FUNCTION: 0x006C1C10 ?$TClassFactory@VTriggerTypeClass@@::CreateInstance
// FUNCTION: 0x006C1C90 ?$TClassFactory@VTriggerTypeClass@@::LockServer
template class TClassFactory<TriggerTypeClass>;

// FUNCTION: 0x006C1CC0 ?$TClassFactory@VScriptClass@@::?$TClassFactory@VScriptClass@@
// FUNCTION: 0x006C1CD0 ?$TClassFactory@VScriptClass@@::QueryInterface
// FUNCTION: 0x006C1D40 ?$TClassFactory@VScriptClass@@::AddRef
// FUNCTION: 0x006C1D60 ?$TClassFactory@VScriptClass@@::Release
// FUNCTION: 0x006C1D90 ?$TClassFactory@VScriptClass@@::CreateInstance
// FUNCTION: 0x006C1E10 ?$TClassFactory@VScriptClass@@::LockServer
template class TClassFactory<ScriptClass>;

// FUNCTION: 0x006C1E40 ?$TClassFactory@VScriptTypeClass@@::?$TClassFactory@VScriptTypeClass@@
// FUNCTION: 0x006C1E50 ?$TClassFactory@VScriptTypeClass@@::QueryInterface
// FUNCTION: 0x006C1EC0 ?$TClassFactory@VScriptTypeClass@@::AddRef
// FUNCTION: 0x006C1EE0 ?$TClassFactory@VScriptTypeClass@@::Release
// FUNCTION: 0x006C1F10 ?$TClassFactory@VScriptTypeClass@@::CreateInstance
// FUNCTION: 0x006C1F90 ?$TClassFactory@VScriptTypeClass@@::LockServer
template class TClassFactory<ScriptTypeClass>;

// FUNCTION: 0x006C1FC0 ?$TClassFactory@VTagClass@@::?$TClassFactory@VTagClass@@
// FUNCTION: 0x006C1FD0 ?$TClassFactory@VTagClass@@::QueryInterface
// FUNCTION: 0x006C2040 ?$TClassFactory@VTagClass@@::AddRef
// FUNCTION: 0x006C2060 ?$TClassFactory@VTagClass@@::Release
// FUNCTION: 0x006C2090 ?$TClassFactory@VTagClass@@::CreateInstance
// FUNCTION: 0x006C2110 ?$TClassFactory@VTagClass@@::LockServer
template class TClassFactory<TagClass>;

// FUNCTION: 0x006C2140 ?$TClassFactory@VTagTypeClass@@::?$TClassFactory@VTagTypeClass@@
// FUNCTION: 0x006C2150 ?$TClassFactory@VTagTypeClass@@::QueryInterface
// FUNCTION: 0x006C21C0 ?$TClassFactory@VTagTypeClass@@::AddRef
// FUNCTION: 0x006C21E0 ?$TClassFactory@VTagTypeClass@@::Release
// FUNCTION: 0x006C2210 ?$TClassFactory@VTagTypeClass@@::CreateInstance
// FUNCTION: 0x006C2290 ?$TClassFactory@VTagTypeClass@@::LockServer
template class TClassFactory<TagTypeClass>;

// FUNCTION: 0x006C22C0 ?$TClassFactory@VTeamClass@@::?$TClassFactory@VTeamClass@@
// FUNCTION: 0x006C22D0 ?$TClassFactory@VTeamClass@@::QueryInterface
// FUNCTION: 0x006C2340 ?$TClassFactory@VTeamClass@@::AddRef
// FUNCTION: 0x006C2360 ?$TClassFactory@VTeamClass@@::Release
// FUNCTION: 0x006C2390 ?$TClassFactory@VTeamClass@@::CreateInstance
// FUNCTION: 0x006C2410 ?$TClassFactory@VTeamClass@@::LockServer
template class TClassFactory<TeamClass>;

// FUNCTION: 0x006C2440 ?$TClassFactory@VTeamTypeClass@@::?$TClassFactory@VTeamTypeClass@@
// FUNCTION: 0x006C2450 ?$TClassFactory@VTeamTypeClass@@::QueryInterface
// FUNCTION: 0x006C24C0 ?$TClassFactory@VTeamTypeClass@@::AddRef
// FUNCTION: 0x006C24E0 ?$TClassFactory@VTeamTypeClass@@::Release
// FUNCTION: 0x006C2510 ?$TClassFactory@VTeamTypeClass@@::CreateInstance
// FUNCTION: 0x006C2590 ?$TClassFactory@VTeamTypeClass@@::LockServer
template class TClassFactory<TeamTypeClass>;

// FUNCTION: 0x006C25C0 ?$TClassFactory@VTaskForceClass@@::?$TClassFactory@VTaskForceClass@@
// FUNCTION: 0x006C25D0 ?$TClassFactory@VTaskForceClass@@::QueryInterface
// FUNCTION: 0x006C2640 ?$TClassFactory@VTaskForceClass@@::AddRef
// FUNCTION: 0x006C2660 ?$TClassFactory@VTaskForceClass@@::Release
// FUNCTION: 0x006C2690 ?$TClassFactory@VTaskForceClass@@::CreateInstance
// FUNCTION: 0x006C2710 ?$TClassFactory@VTaskForceClass@@::LockServer
template class TClassFactory<TaskForceClass>;

// FUNCTION: 0x006C2740 ?$TClassFactory@VUnitTypeClass@@::?$TClassFactory@VUnitTypeClass@@
// FUNCTION: 0x006C2750 ?$TClassFactory@VUnitTypeClass@@::QueryInterface
// FUNCTION: 0x006C27C0 ?$TClassFactory@VUnitTypeClass@@::AddRef
// FUNCTION: 0x006C27E0 ?$TClassFactory@VUnitTypeClass@@::Release
// FUNCTION: 0x006C2810 ?$TClassFactory@VUnitTypeClass@@::CreateInstance
// FUNCTION: 0x006C2890 ?$TClassFactory@VUnitTypeClass@@::LockServer
template class TClassFactory<UnitTypeClass>;

// FUNCTION: 0x006C28C0 ?$TClassFactory@VBuildingTypeClass@@::?$TClassFactory@VBuildingTypeClass@@
// FUNCTION: 0x006C28D0 ?$TClassFactory@VBuildingTypeClass@@::QueryInterface
// FUNCTION: 0x006C2940 ?$TClassFactory@VBuildingTypeClass@@::AddRef
// FUNCTION: 0x006C2960 ?$TClassFactory@VBuildingTypeClass@@::Release
// FUNCTION: 0x006C2990 ?$TClassFactory@VBuildingTypeClass@@::CreateInstance
// FUNCTION: 0x006C2A10 ?$TClassFactory@VBuildingTypeClass@@::LockServer
template class TClassFactory<BuildingTypeClass>;

// FUNCTION: 0x006C2A40 ?$TClassFactory@VAircraftTypeClass@@::?$TClassFactory@VAircraftTypeClass@@
// FUNCTION: 0x006C2A50 ?$TClassFactory@VAircraftTypeClass@@::QueryInterface
// FUNCTION: 0x006C2AC0 ?$TClassFactory@VAircraftTypeClass@@::AddRef
// FUNCTION: 0x006C2AE0 ?$TClassFactory@VAircraftTypeClass@@::Release
// FUNCTION: 0x006C2B10 ?$TClassFactory@VAircraftTypeClass@@::CreateInstance
// FUNCTION: 0x006C2B90 ?$TClassFactory@VAircraftTypeClass@@::LockServer
template class TClassFactory<AircraftTypeClass>;

// FUNCTION: 0x006C2BC0 ?$TClassFactory@VInfantryTypeClass@@::?$TClassFactory@VInfantryTypeClass@@
// FUNCTION: 0x006C2BD0 ?$TClassFactory@VInfantryTypeClass@@::QueryInterface
// FUNCTION: 0x006C2C40 ?$TClassFactory@VInfantryTypeClass@@::AddRef
// FUNCTION: 0x006C2C60 ?$TClassFactory@VInfantryTypeClass@@::Release
// FUNCTION: 0x006C2C90 ?$TClassFactory@VInfantryTypeClass@@::CreateInstance
// FUNCTION: 0x006C2D10 ?$TClassFactory@VInfantryTypeClass@@::LockServer
template class TClassFactory<InfantryTypeClass>;

// FUNCTION: 0x006C2D40 ?$TClassFactory@VBulletTypeClass@@::?$TClassFactory@VBulletTypeClass@@
// FUNCTION: 0x006C2D50 ?$TClassFactory@VBulletTypeClass@@::QueryInterface
// FUNCTION: 0x006C2DC0 ?$TClassFactory@VBulletTypeClass@@::AddRef
// FUNCTION: 0x006C2DE0 ?$TClassFactory@VBulletTypeClass@@::Release
// FUNCTION: 0x006C2E10 ?$TClassFactory@VBulletTypeClass@@::CreateInstance
// FUNCTION: 0x006C2E90 ?$TClassFactory@VBulletTypeClass@@::LockServer
template class TClassFactory<BulletTypeClass>;

// FUNCTION: 0x006C2EC0 ?$TClassFactory@VIsometricTileTypeClass@@::?$TClassFactory@VIsometricTileTypeClass@@
// FUNCTION: 0x006C2ED0 ?$TClassFactory@VIsometricTileTypeClass@@::QueryInterface
// FUNCTION: 0x006C2F40 ?$TClassFactory@VIsometricTileTypeClass@@::AddRef
// FUNCTION: 0x006C2F60 ?$TClassFactory@VIsometricTileTypeClass@@::Release
// FUNCTION: 0x006C2F90 ?$TClassFactory@VIsometricTileTypeClass@@::CreateInstance
// FUNCTION: 0x006C3010 ?$TClassFactory@VIsometricTileTypeClass@@::LockServer
template class TClassFactory<IsometricTileTypeClass>;

// FUNCTION: 0x006C3040 ?$TClassFactory@VOverlayTypeClass@@::?$TClassFactory@VOverlayTypeClass@@
// FUNCTION: 0x006C3050 ?$TClassFactory@VOverlayTypeClass@@::QueryInterface
// FUNCTION: 0x006C30C0 ?$TClassFactory@VOverlayTypeClass@@::AddRef
// FUNCTION: 0x006C30E0 ?$TClassFactory@VOverlayTypeClass@@::Release
// FUNCTION: 0x006C3110 ?$TClassFactory@VOverlayTypeClass@@::CreateInstance
// FUNCTION: 0x006C3190 ?$TClassFactory@VOverlayTypeClass@@::LockServer
template class TClassFactory<OverlayTypeClass>;

// FUNCTION: 0x006C31C0 ?$TClassFactory@VSmudgeTypeClass@@::?$TClassFactory@VSmudgeTypeClass@@
// FUNCTION: 0x006C31D0 ?$TClassFactory@VSmudgeTypeClass@@::QueryInterface
// FUNCTION: 0x006C3240 ?$TClassFactory@VSmudgeTypeClass@@::AddRef
// FUNCTION: 0x006C3260 ?$TClassFactory@VSmudgeTypeClass@@::Release
// FUNCTION: 0x006C3290 ?$TClassFactory@VSmudgeTypeClass@@::CreateInstance
// FUNCTION: 0x006C3310 ?$TClassFactory@VSmudgeTypeClass@@::LockServer
template class TClassFactory<SmudgeTypeClass>;

// FUNCTION: 0x006C3340 ?$TClassFactory@VUnitClass@@::?$TClassFactory@VUnitClass@@
// FUNCTION: 0x006C3350 ?$TClassFactory@VUnitClass@@::QueryInterface
// FUNCTION: 0x006C33C0 ?$TClassFactory@VUnitClass@@::AddRef
// FUNCTION: 0x006C33E0 ?$TClassFactory@VUnitClass@@::Release
// FUNCTION: 0x006C3410 ?$TClassFactory@VUnitClass@@::CreateInstance
// FUNCTION: 0x006C3490 ?$TClassFactory@VUnitClass@@::LockServer
template class TClassFactory<UnitClass>;

// FUNCTION: 0x006C34C0 ?$TClassFactory@VBuildingClass@@::?$TClassFactory@VBuildingClass@@
// FUNCTION: 0x006C34D0 ?$TClassFactory@VBuildingClass@@::QueryInterface
// FUNCTION: 0x006C3540 ?$TClassFactory@VBuildingClass@@::AddRef
// FUNCTION: 0x006C3560 ?$TClassFactory@VBuildingClass@@::Release
// FUNCTION: 0x006C3590 ?$TClassFactory@VBuildingClass@@::CreateInstance
// FUNCTION: 0x006C3610 ?$TClassFactory@VBuildingClass@@::LockServer
template class TClassFactory<BuildingClass>;

// FUNCTION: 0x006C3640 ?$TClassFactory@VAircraftClass@@::?$TClassFactory@VAircraftClass@@
// FUNCTION: 0x006C3650 ?$TClassFactory@VAircraftClass@@::QueryInterface
// FUNCTION: 0x006C36C0 ?$TClassFactory@VAircraftClass@@::AddRef
// FUNCTION: 0x006C36E0 ?$TClassFactory@VAircraftClass@@::Release
// FUNCTION: 0x006C3710 ?$TClassFactory@VAircraftClass@@::CreateInstance
// FUNCTION: 0x006C3790 ?$TClassFactory@VAircraftClass@@::LockServer
template class TClassFactory<AircraftClass>;
