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

// SYNTHETIC: GAMEMD 0x006BB070 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006BB0A0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006BB0C0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006BB0E0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006BB100 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006BB120 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006BB140 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006BB180 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006BB1B0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006BB1E0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006BB210 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006BB220 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006BB240 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006BB260 SYMBOL
// _$E329
// SYNTHETIC: GAMEMD 0x006BB2A0 SYMBOL
// _$E327

namespace DirectDrawWrap { extern bool DoSmth; void DisposeOfStuff(void); }
void FUN_004C19B0(void);
extern int ReadyToQuit;
extern SHPStruct * DAT_00B0BCF0;
extern bool DAT_00B0BCF4;

DWORD dwRegisterMap = -1;

DynamicVectorClass<DWORD> RegisteredClasses;

// FUNCTION: GAMEMD 0x006BB2E0
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

// FUNCTION: GAMEMD 0x006BB390
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

// FUNCTION: GAMEMD 0x006BB760
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

// FUNCTION: GAMEMD 0x006BB8E0
// DirectDrawWrap::DisposeOfStuff
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

// FUNCTION: GAMEMD 0x006BB990
LONG Top_Level_Exception_Filter(EXCEPTION_POINTERS *e_info)
{
	return(Exception_Handler(e_info->ExceptionRecord->ExceptionCode, e_info));
}

struct VirtualObject { virtual ~VirtualObject(); };
class BitText { public: virtual ~BitText(); static BitText * Instance; };
namespace FileSystem { extern SHPStruct * GRFXTXT_SHP; extern ConvertClass * GRFXTXT_Convert; extern unsigned char * GRFXTXT_PAL; }
extern CCINIClass INI_AI, INI_UIMD, DAT_00887058, INI_RA2MD;
struct OutOfLineDtor { ~OutOfLineDtor(); };
extern OutOfLineDtor * DAT_00A83DB8;
extern HINSTANCE DAT_0089F974;
extern HANDLE DAT_00B0BCE8;
extern DWORD DAT_00B0BCEC;
namespace Unsorted { extern HANDLE AppMutex; }
namespace StringTable { void Unload(void); }
namespace CopyProtection { void ShutDown(void); }
extern DynamicVectorClass<VirtualObject *> DAT_0087F698;
extern DynamicVectorClass<VirtualObject *> DAT_00884D90;
extern DynamicVectorClass<VirtualObject *> DAT_00884DC0;
extern VirtualObject * DAT_0087F6B0;
extern VirtualObject * DAT_0087F6B4;
extern VirtualObject * DAT_0087F6B8;
extern VirtualObject * DAT_0087F6C0;
extern VirtualObject * DAT_0087F6C4;
extern VirtualObject * DAT_0087F6C8;
extern VirtualObject * DAT_0087F6CC;
extern VirtualObject * DAT_0087F6D0;
extern VirtualObject * DAT_00884DD8;
extern VirtualObject * DAT_00884DDC;
extern VirtualObject * DAT_00884DF8;
extern VirtualObject * DAT_00884DFC;
extern VirtualObject * DAT_00884E00;
extern VirtualObject * DAT_00884E04;
extern VirtualObject * DAT_00884E08;
extern VirtualObject * DAT_00884E0C;
extern VirtualObject * DAT_00884E10;
extern VirtualObject * DAT_00884E14;
extern VirtualObject * DAT_00884E18;
extern VirtualObject * DAT_00884E1C;
extern VirtualObject * DAT_00884E20;
extern VirtualObject * DAT_00884E24;
extern VirtualObject * DAT_00884E28;
extern VirtualObject * DAT_00884E2C;
extern VirtualObject * DAT_00884E30;
extern VirtualObject * DAT_00884E34;
extern VirtualObject * DAT_00884E38;
extern VirtualObject * DAT_00884E3C;
extern VirtualObject * DAT_00884E40;
extern VirtualObject * DAT_00884E44;
extern VirtualObject * DAT_00884E48;
extern VirtualObject * DAT_00884E4C;
extern VirtualObject * DAT_00884E50;
extern VirtualObject * DAT_00884E54;
extern VirtualObject * DAT_00884E58;
extern VirtualObject * DAT_00884E5C;
extern VirtualObject * DAT_00884E60;
extern VirtualObject * DAT_00884E64;
extern VirtualObject * DAT_00884E68;
extern VirtualObject * DAT_00884E6C;
extern VirtualObject * DAT_00884E70;
extern VirtualObject * DAT_00884E74;
extern VirtualObject * DAT_00884E78;
extern VirtualObject * DAT_00887320;
extern VirtualObject * DAT_00887640;
extern VirtualObject * DAT_0089DDC0;
extern VirtualObject * DAT_00B1D13C;
extern VirtualObject * DAT_00B1D140;
extern unsigned char * DAT_0087F770;
extern unsigned char * DAT_0089DDBC;
extern unsigned char * DAT_00AA1060;
extern unsigned char * DAT_00AA1064;
extern unsigned char * DAT_00AA1068;
extern unsigned char * DAT_00AA106C;
extern unsigned char * DAT_00ABC554;
void FUN_007531A0(void);
void FUN_00534450(void);
void FUN_0069E180(void);
void FUN_0069E210(void);
void FUN_005FF800(void);
void FUN_0053D550(void);
void FUN_0065FD50(void);
void FUN_006D0270(MouseClass *);
void FUN_00626FF0(void);
void FUN_005BC300(void);
void FUN_005F77F0(void);
void FUN_0060D440(void);
void FUN_006370A0(void);
void FUN_0072AC40(void);
void FUN_00730970(void);
void FUN_00552AA0(void);
void FUN_005BE940(void);
void FUN_005DB6F0(void);
void FUN_007513F0(void);
void FUN_0047ADC0(void);
void FUN_005DA720(void);
void FUN_0053E1D0(void);
void FUN_00406D40(void);
void FUN_007B86B0(void);

// FUNCTION: GAMEMD 0x006BE1C0
void __cdecl Prog_End(void)
{
	int i;

	GameActive = false;

	if (FileSystem::GRFXTXT_SHP != NULL) {
		delete FileSystem::GRFXTXT_SHP;
		FileSystem::GRFXTXT_SHP = NULL;
	}
	if (FileSystem::GRFXTXT_Convert != NULL) {
		delete FileSystem::GRFXTXT_Convert;
		FileSystem::GRFXTXT_Convert = NULL;
	}
	if (FileSystem::GRFXTXT_PAL != NULL) {
		delete FileSystem::GRFXTXT_PAL;
		FileSystem::GRFXTXT_PAL = NULL;
	}

	Session.Free_Scenario_Descriptions();

	while (ColorSchemes.Count()) {
		delete ColorSchemes[0];
	}
	ColorSchemes.Clear();

	FUN_00534450();
	FUN_0069E180();
	FUN_0069E210();
	MixFileClass::DestroyCache();

	while (Movies.Count()) {
		free((void *)Movies[0]);
		Movies.Delete_Index(0);
	}
	Movies.Clear();

	for (i = 0; i < AllCommands.Count(); i++) {
		delete AllCommands[i];
	}
	AllCommands.Clear();

	if (RuleINI != NULL) {
		delete RuleINI;
		RuleINI = NULL;
	}
	INI_AI.Clear();
	ArtINI.Clear();
	INI_UIMD.Clear();
	DAT_00887058.Clear();
	INI_RA2MD.Clear();
	INI_RA2MD.Clear();

	FUN_005FF800();
	FUN_0053D550();
	if (DAT_00887640 != NULL) {
		delete DAT_00887640;
		DAT_00887640 = NULL;
	}

	Map.Free_Cells();
	Map.Clear_Radar();
	DirectDrawWrap::DisposeOfStuff();

	if (DepthBuffer != NULL) {
		delete DepthBuffer;
		DepthBuffer = NULL;
	}
	if (AlphaBuffer != NULL) {
		delete AlphaBuffer;
		AlphaBuffer = NULL;
	}
	if (BitText::Instance != NULL) {
		delete BitText::Instance;
		BitText::Instance = NULL;
	}

	if (DAT_0087F6B8 != NULL) {
		delete DAT_0087F6B8;
		DAT_0087F6B8 = NULL;
	}
	if (DAT_0087F6C0 != NULL) {
		delete DAT_0087F6C0;
		DAT_0087F6C0 = NULL;
	}
	if (DAT_0087F6C4 != NULL) {
		delete DAT_0087F6C4;
		DAT_0087F6C4 = NULL;
	}
	if (DAT_0087F6B4 != NULL) {
		delete DAT_0087F6B4;
		DAT_0087F6B4 = NULL;
	}
	if (DAT_0087F6C8 != NULL) {
		delete DAT_0087F6C8;
		DAT_0087F6C8 = NULL;
	}
	if (DAT_0087F6CC != NULL) {
		delete DAT_0087F6CC;
		DAT_0087F6CC = NULL;
	}
	if (DAT_0087F6D0 != NULL) {
		delete DAT_0087F6D0;
		DAT_0087F6D0 = NULL;
	}
	if (DAT_0087F6B0 != NULL) {
		delete DAT_0087F6B0;
		DAT_0087F6B0 = NULL;
	}
	if (DAT_00B1D140 != NULL) {
		delete DAT_00B1D140;
		DAT_00B1D140 = NULL;
	}
	if (DAT_00B1D13C != NULL) {
		delete DAT_00B1D13C;
		DAT_00B1D13C = NULL;
	}
	if (DAT_0089DDC0 != NULL) {
		delete DAT_0089DDC0;
		DAT_0089DDC0 = NULL;
	}
	if (DAT_0089DDBC != NULL) {
		delete DAT_0089DDBC;
		DAT_0089DDBC = NULL;
	}

	Map.Shutdown();

	for (i = 0; i < DAT_0087F698.Count(); i++) {
		delete DAT_0087F698[i];
	}
	DAT_0087F698.Clear();

	if (DAT_00884E08 != NULL) {
		delete DAT_00884E08;
		DAT_00884E08 = NULL;
	}
	if (DAT_00884E0C != NULL) {
		delete DAT_00884E0C;
		DAT_00884E0C = NULL;
	}
	if (DAT_00884E10 != NULL) {
		delete DAT_00884E10;
		DAT_00884E10 = NULL;
	}
	if (DAT_00884E14 != NULL) {
		delete DAT_00884E14;
		DAT_00884E14 = NULL;
	}
	if (DAT_00884E18 != NULL) {
		delete DAT_00884E18;
		DAT_00884E18 = NULL;
	}
	if (DAT_00884E1C != NULL) {
		delete DAT_00884E1C;
		DAT_00884E1C = NULL;
	}
	if (DAT_00884E20 != NULL) {
		delete DAT_00884E20;
		DAT_00884E20 = NULL;
	}
	if (DAT_00884E24 != NULL) {
		delete DAT_00884E24;
		DAT_00884E24 = NULL;
	}
	if (DAT_00884E28 != NULL) {
		delete DAT_00884E28;
		DAT_00884E28 = NULL;
	}
	if (DAT_00ABC554 != NULL) {
		delete DAT_00ABC554;
		DAT_00ABC554 = NULL;
	}
	if (DAT_00AA1060 != NULL) {
		delete DAT_00AA1060;
		DAT_00AA1060 = NULL;
	}
	if (DAT_00AA1064 != NULL) {
		delete DAT_00AA1064;
		DAT_00AA1064 = NULL;
	}
	if (DAT_00AA1068 != NULL) {
		delete DAT_00AA1068;
		DAT_00AA1068 = NULL;
	}
	if (DAT_00AA106C != NULL) {
		delete DAT_00AA106C;
		DAT_00AA106C = NULL;
	}
	if (DAT_00887320 != NULL) {
		delete DAT_00887320;
		DAT_00887320 = NULL;
	}
	if (DAT_00884E2C != NULL) {
		delete DAT_00884E2C;
		DAT_00884E2C = NULL;
	}
	if (DAT_00884E30 != NULL) {
		delete DAT_00884E30;
		DAT_00884E30 = NULL;
	}
	if (DAT_00884E34 != NULL) {
		delete DAT_00884E34;
		DAT_00884E34 = NULL;
	}
	if (DAT_00884E3C != NULL) {
		delete DAT_00884E3C;
		DAT_00884E3C = NULL;
	}
	if (DAT_00884E38 != NULL) {
		delete DAT_00884E38;
		DAT_00884E38 = NULL;
	}
	if (DAT_00884E44 != NULL) {
		delete DAT_00884E44;
		DAT_00884E44 = NULL;
	}
	if (DAT_00884E40 != NULL) {
		delete DAT_00884E40;
		DAT_00884E40 = NULL;
	}
	while (DAT_00884D90.Count() > 0) {
		delete DAT_00884D90[0];
		DAT_00884D90.Delete_Index(0);
	}
	if (DAT_00884E4C != NULL) {
		delete DAT_00884E4C;
		DAT_00884E4C = NULL;
	}
	if (DAT_00884E48 != NULL) {
		delete DAT_00884E48;
		DAT_00884E48 = NULL;
	}
	if (DAT_00884E54 != NULL) {
		delete DAT_00884E54;
		DAT_00884E54 = NULL;
	}
	if (DAT_00884E50 != NULL) {
		delete DAT_00884E50;
		DAT_00884E50 = NULL;
	}
	if (DAT_00884E5C != NULL) {
		delete DAT_00884E5C;
		DAT_00884E5C = NULL;
	}
	if (DAT_00884E58 != NULL) {
		delete DAT_00884E58;
		DAT_00884E58 = NULL;
	}
	if (DAT_00884E68 != NULL) {
		delete DAT_00884E68;
		DAT_00884E68 = NULL;
	}
	if (DAT_00884E6C != NULL) {
		delete DAT_00884E6C;
		DAT_00884E6C = NULL;
	}
	while (DAT_00884DC0.Count() > 0) {
		delete DAT_00884DC0[0];
		DAT_00884DC0.Delete_Index(0);
	}
	if (DAT_00884E64 != NULL) {
		delete DAT_00884E64;
		DAT_00884E64 = NULL;
	}
	if (DAT_00884E60 != NULL) {
		delete DAT_00884E60;
		DAT_00884E60 = NULL;
	}
	if (DAT_00884DDC != NULL) {
		delete DAT_00884DDC;
		DAT_00884DDC = NULL;
	}
	if (DAT_00884DD8 != NULL) {
		delete DAT_00884DD8;
		DAT_00884DD8 = NULL;
	}
	if (DAT_00884E74 != NULL) {
		delete DAT_00884E74;
		DAT_00884E74 = NULL;
	}
	if (DAT_00884E70 != NULL) {
		delete DAT_00884E70;
		DAT_00884E70 = NULL;
	}
	if (DAT_00884E78 != NULL) {
		delete DAT_00884E78;
		DAT_00884E78 = NULL;
	}
	if (DAT_00884DFC != NULL) {
		delete DAT_00884DFC;
		DAT_00884DFC = NULL;
	}
	if (DAT_00884DF8 != NULL) {
		delete DAT_00884DF8;
		DAT_00884DF8 = NULL;
	}
	if (DAT_00884E00 != NULL) {
		delete DAT_00884E00;
		DAT_00884E00 = NULL;
	}
	if (DAT_00884E04 != NULL) {
		delete DAT_00884E04;
		DAT_00884E04 = NULL;
	}

	if (TacticalMap != NULL) {
		delete TacticalMap;
		TacticalMap = NULL;
	}

	FUN_0065FD50();
	FUN_006D0270(&Map);
	FUN_00626FF0();
	FUN_005BC300();
	FUN_005F77F0();
	FUN_0060D440();
	FUN_006370A0();
	FUN_0072AC40();
	FUN_00730970();
	FUN_00552AA0();
	FUN_005BE940();
	FUN_005DB6F0();
	FUN_007513F0();
	FUN_007531A0();
	FUN_0047ADC0();

	if (DAT_0087F770 != NULL) {
		delete DAT_0087F770;
		DAT_0087F770 = NULL;
	}

	FUN_005DA720();

	if (DAT_00A83DB8 != NULL) {
		delete DAT_00A83DB8;
		DAT_00A83DB8 = NULL;
	}
	if (Rule != NULL) {
		delete Rule;
		Rule = NULL;
	}
	if (Scen != NULL) {
		delete Scen;
		Scen = NULL;
	}

	for (i = 0; i < RegisteredClasses.Count(); i++) {
		CoRevokeClassObject(RegisteredClasses[i]);
	}
	RegisteredClasses.Clear();

	RevokeActiveObject(DAT_00B0BCEC, NULL);

	if (DAT_0089F974 != NULL) {
		FreeLibrary(DAT_0089F974);
	}
	if (DAT_00B0BCE8 != NULL) {
		CloseHandle(DAT_00B0BCE8);
		DAT_00B0BCE8 = NULL;
	}
	if (Unsorted::AppMutex != NULL) {
		CloseHandle(Unsorted::AppMutex);
		Unsorted::AppMutex = NULL;
	}

	FUN_0053E1D0();
	Theme.Free_Themes();
	FUN_00406D40();
	StringTable::Unload();
	CopyProtection::ShutDown();
	FUN_007B86B0();
}

// FUNCTION: GAMEMD 0x006BEC50
void Exit_Process(int code)
{
	exit(code);
}

// FUNCTION: GAMEMD 0x006BEC60
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

// TEMPLATE: GAMEMD 0x006BED50
// ?Clear@?$DynamicVectorClass@K@@UAEXXZ
// TEMPLATE: GAMEMD 0x006BED80
// ?ID@?$VectorClass@K@@UAEHPBK@Z
// TEMPLATE: GAMEMD 0x006BEDA0
// ?GetItem@?$VectorClass@K@@UBEKH@Z
template class DynamicVectorClass<DWORD>;

// TEMPLATE: GAMEMD 0x006BEE40
// ??0?$TClassFactory@VCStreamClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BEE50
// ?QueryInterface@?$TClassFactory@VCStreamClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BEEC0
// ?AddRef@?$TClassFactory@VCStreamClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BEEE0
// ?Release@?$TClassFactory@VCStreamClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BEF10
// ?CreateInstance@?$TClassFactory@VCStreamClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BEF90
// ?LockServer@?$TClassFactory@VCStreamClass@@@@UAGJH@Z
template class TClassFactory<CStreamClass>;

// TEMPLATE: GAMEMD 0x006BEFC0
// ??0?$TClassFactory@VWaveClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BEFD0
// ?QueryInterface@?$TClassFactory@VWaveClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF040
// ?AddRef@?$TClassFactory@VWaveClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF060
// ?Release@?$TClassFactory@VWaveClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF090
// ?CreateInstance@?$TClassFactory@VWaveClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF110
// ?LockServer@?$TClassFactory@VWaveClass@@@@UAGJH@Z
template class TClassFactory<WaveClass>;

// TEMPLATE: GAMEMD 0x006BF140
// ??0?$TClassFactory@VTerrainTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BF150
// ?QueryInterface@?$TClassFactory@VTerrainTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF1C0
// ?AddRef@?$TClassFactory@VTerrainTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF1E0
// ?Release@?$TClassFactory@VTerrainTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF210
// ?CreateInstance@?$TClassFactory@VTerrainTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF290
// ?LockServer@?$TClassFactory@VTerrainTypeClass@@@@UAGJH@Z
template class TClassFactory<TerrainTypeClass>;

// TEMPLATE: GAMEMD 0x006BF2C0
// ??0?$TClassFactory@VTerrainClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BF2D0
// ?QueryInterface@?$TClassFactory@VTerrainClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF340
// ?AddRef@?$TClassFactory@VTerrainClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF360
// ?Release@?$TClassFactory@VTerrainClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF390
// ?CreateInstance@?$TClassFactory@VTerrainClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF410
// ?LockServer@?$TClassFactory@VTerrainClass@@@@UAGJH@Z
template class TClassFactory<TerrainClass>;

// TEMPLATE: GAMEMD 0x006BF440
// ??0?$TClassFactory@VSuperWeaponTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BF450
// ?QueryInterface@?$TClassFactory@VSuperWeaponTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF4C0
// ?AddRef@?$TClassFactory@VSuperWeaponTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF4E0
// ?Release@?$TClassFactory@VSuperWeaponTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF510
// ?CreateInstance@?$TClassFactory@VSuperWeaponTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF590
// ?LockServer@?$TClassFactory@VSuperWeaponTypeClass@@@@UAGJH@Z
template class TClassFactory<SuperWeaponTypeClass>;

// TEMPLATE: GAMEMD 0x006BF5C0
// ??0?$TClassFactory@VSuperClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BF5D0
// ?QueryInterface@?$TClassFactory@VSuperClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF640
// ?AddRef@?$TClassFactory@VSuperClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF660
// ?Release@?$TClassFactory@VSuperClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF690
// ?CreateInstance@?$TClassFactory@VSuperClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF710
// ?LockServer@?$TClassFactory@VSuperClass@@@@UAGJH@Z
template class TClassFactory<SuperClass>;

// TEMPLATE: GAMEMD 0x006BF740
// ??0?$TClassFactory@VTactical@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BF750
// ?QueryInterface@?$TClassFactory@VTactical@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF7C0
// ?AddRef@?$TClassFactory@VTactical@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF7E0
// ?Release@?$TClassFactory@VTactical@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF810
// ?CreateInstance@?$TClassFactory@VTactical@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF890
// ?LockServer@?$TClassFactory@VTactical@@@@UAGJH@Z
template class TClassFactory<Tactical>;

// TEMPLATE: GAMEMD 0x006BF8C0
// ??0?$TClassFactory@VCellClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BF8D0
// ?QueryInterface@?$TClassFactory@VCellClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BF940
// ?AddRef@?$TClassFactory@VCellClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF960
// ?Release@?$TClassFactory@VCellClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BF990
// ?CreateInstance@?$TClassFactory@VCellClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFA10
// ?LockServer@?$TClassFactory@VCellClass@@@@UAGJH@Z
template class TClassFactory<CellClass>;

// TEMPLATE: GAMEMD 0x006BFA40
// ??0?$TClassFactory@VEMPulseClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BFA50
// ?QueryInterface@?$TClassFactory@VEMPulseClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFAC0
// ?AddRef@?$TClassFactory@VEMPulseClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFAE0
// ?Release@?$TClassFactory@VEMPulseClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFB10
// ?CreateInstance@?$TClassFactory@VEMPulseClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFB90
// ?LockServer@?$TClassFactory@VEMPulseClass@@@@UAGJH@Z
template class TClassFactory<EMPulseClass>;

// TEMPLATE: GAMEMD 0x006BFBC0
// ??0?$TClassFactory@VLightSourceClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BFBD0
// ?QueryInterface@?$TClassFactory@VLightSourceClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFC40
// ?AddRef@?$TClassFactory@VLightSourceClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFC60
// ?Release@?$TClassFactory@VLightSourceClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFC90
// ?CreateInstance@?$TClassFactory@VLightSourceClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFD10
// ?LockServer@?$TClassFactory@VLightSourceClass@@@@UAGJH@Z
template class TClassFactory<LightSourceClass>;

// TEMPLATE: GAMEMD 0x006BFD40
// ??0?$TClassFactory@VSideClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BFD50
// ?QueryInterface@?$TClassFactory@VSideClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFDC0
// ?AddRef@?$TClassFactory@VSideClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFDE0
// ?Release@?$TClassFactory@VSideClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFE10
// ?CreateInstance@?$TClassFactory@VSideClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFE90
// ?LockServer@?$TClassFactory@VSideClass@@@@UAGJH@Z
template class TClassFactory<SideClass>;

// TEMPLATE: GAMEMD 0x006BFEC0
// ??0?$TClassFactory@VTiberiumClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006BFED0
// ?QueryInterface@?$TClassFactory@VTiberiumClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006BFF40
// ?AddRef@?$TClassFactory@VTiberiumClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFF60
// ?Release@?$TClassFactory@VTiberiumClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006BFF90
// ?CreateInstance@?$TClassFactory@VTiberiumClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0010
// ?LockServer@?$TClassFactory@VTiberiumClass@@@@UAGJH@Z
template class TClassFactory<TiberiumClass>;

// TEMPLATE: GAMEMD 0x006C0040
// ??0?$TClassFactory@VTubeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0050
// ?QueryInterface@?$TClassFactory@VTubeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C00C0
// ?AddRef@?$TClassFactory@VTubeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C00E0
// ?Release@?$TClassFactory@VTubeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0110
// ?CreateInstance@?$TClassFactory@VTubeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0190
// ?LockServer@?$TClassFactory@VTubeClass@@@@UAGJH@Z
template class TClassFactory<TubeClass>;

// TEMPLATE: GAMEMD 0x006C01C0
// ??0?$TClassFactory@VCampaignClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C01D0
// ?QueryInterface@?$TClassFactory@VCampaignClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0240
// ?AddRef@?$TClassFactory@VCampaignClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0260
// ?Release@?$TClassFactory@VCampaignClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0290
// ?CreateInstance@?$TClassFactory@VCampaignClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0310
// ?LockServer@?$TClassFactory@VCampaignClass@@@@UAGJH@Z
template class TClassFactory<CampaignClass>;

// TEMPLATE: GAMEMD 0x006C0340
// ??0?$TClassFactory@VBuildingLightClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0350
// ?QueryInterface@?$TClassFactory@VBuildingLightClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C03C0
// ?AddRef@?$TClassFactory@VBuildingLightClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C03E0
// ?Release@?$TClassFactory@VBuildingLightClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0410
// ?CreateInstance@?$TClassFactory@VBuildingLightClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0490
// ?LockServer@?$TClassFactory@VBuildingLightClass@@@@UAGJH@Z
template class TClassFactory<BuildingLightClass>;

// TEMPLATE: GAMEMD 0x006BEDB0
// ?CreateInstance@?$_com_ptr_t@V?$_com_IIID@UIUnknown@@$1?_GUID_00000000_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@QAEJABU_GUID@@PAUIUnknown@@K@Z
template class _com_ptr_t<_com_IIID<IUnknown, &__uuidof(IUnknown)> >;

// TEMPLATE: GAMEMD 0x006C04C0
// ??0?$TClassFactory@VWaypointPathClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C04D0
// ?QueryInterface@?$TClassFactory@VWaypointPathClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0540
// ?AddRef@?$TClassFactory@VWaypointPathClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0560
// ?Release@?$TClassFactory@VWaypointPathClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0590
// ?CreateInstance@?$TClassFactory@VWaypointPathClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0610
// ?LockServer@?$TClassFactory@VWaypointPathClass@@@@UAGJH@Z
template class TClassFactory<WaypointPathClass>;

// TEMPLATE: GAMEMD 0x006C0640
// ??0?$TClassFactory@VTemporalClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0650
// ?QueryInterface@?$TClassFactory@VTemporalClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C06C0
// ?AddRef@?$TClassFactory@VTemporalClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C06E0
// ?Release@?$TClassFactory@VTemporalClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0710
// ?CreateInstance@?$TClassFactory@VTemporalClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0790
// ?LockServer@?$TClassFactory@VTemporalClass@@@@UAGJH@Z
template class TClassFactory<TemporalClass>;

// TEMPLATE: GAMEMD 0x006C07C0
// ??0?$TClassFactory@VAirstrikeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C07D0
// ?QueryInterface@?$TClassFactory@VAirstrikeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0840
// ?AddRef@?$TClassFactory@VAirstrikeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0860
// ?Release@?$TClassFactory@VAirstrikeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0890
// ?CreateInstance@?$TClassFactory@VAirstrikeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0910
// ?LockServer@?$TClassFactory@VAirstrikeClass@@@@UAGJH@Z
template class TClassFactory<AirstrikeClass>;

// TEMPLATE: GAMEMD 0x006C0940
// ??0?$TClassFactory@VSpawnManagerClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0950
// ?QueryInterface@?$TClassFactory@VSpawnManagerClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C09C0
// ?AddRef@?$TClassFactory@VSpawnManagerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C09E0
// ?Release@?$TClassFactory@VSpawnManagerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0A10
// ?CreateInstance@?$TClassFactory@VSpawnManagerClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0A90
// ?LockServer@?$TClassFactory@VSpawnManagerClass@@@@UAGJH@Z
template class TClassFactory<SpawnManagerClass>;

// TEMPLATE: GAMEMD 0x006C0AC0
// ??0?$TClassFactory@VSlaveManagerClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0AD0
// ?QueryInterface@?$TClassFactory@VSlaveManagerClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0B40
// ?AddRef@?$TClassFactory@VSlaveManagerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0B60
// ?Release@?$TClassFactory@VSlaveManagerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0B90
// ?CreateInstance@?$TClassFactory@VSlaveManagerClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0C10
// ?LockServer@?$TClassFactory@VSlaveManagerClass@@@@UAGJH@Z
template class TClassFactory<SlaveManagerClass>;

// TEMPLATE: GAMEMD 0x006C0C40
// ??0?$TClassFactory@VCaptureManagerClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0C50
// ?QueryInterface@?$TClassFactory@VCaptureManagerClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0CC0
// ?AddRef@?$TClassFactory@VCaptureManagerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0CE0
// ?Release@?$TClassFactory@VCaptureManagerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0D10
// ?CreateInstance@?$TClassFactory@VCaptureManagerClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0D90
// ?LockServer@?$TClassFactory@VCaptureManagerClass@@@@UAGJH@Z
template class TClassFactory<CaptureManagerClass>;

// TEMPLATE: GAMEMD 0x006C0DC0
// ??0?$TClassFactory@VDiskLaserClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0DD0
// ?QueryInterface@?$TClassFactory@VDiskLaserClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0E40
// ?AddRef@?$TClassFactory@VDiskLaserClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0E60
// ?Release@?$TClassFactory@VDiskLaserClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0E90
// ?CreateInstance@?$TClassFactory@VDiskLaserClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0F10
// ?LockServer@?$TClassFactory@VDiskLaserClass@@@@UAGJH@Z
template class TClassFactory<DiskLaserClass>;

// TEMPLATE: GAMEMD 0x006C0F40
// ??0?$TClassFactory@VParasiteClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C0F50
// ?QueryInterface@?$TClassFactory@VParasiteClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C0FC0
// ?AddRef@?$TClassFactory@VParasiteClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C0FE0
// ?Release@?$TClassFactory@VParasiteClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1010
// ?CreateInstance@?$TClassFactory@VParasiteClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1090
// ?LockServer@?$TClassFactory@VParasiteClass@@@@UAGJH@Z
template class TClassFactory<ParasiteClass>;

// TEMPLATE: GAMEMD 0x006C10C0
// ??0?$TClassFactory@VBombClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C10D0
// ?QueryInterface@?$TClassFactory@VBombClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1140
// ?AddRef@?$TClassFactory@VBombClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1160
// ?Release@?$TClassFactory@VBombClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1190
// ?CreateInstance@?$TClassFactory@VBombClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1210
// ?LockServer@?$TClassFactory@VBombClass@@@@UAGJH@Z
template class TClassFactory<BombClass>;

// TEMPLATE: GAMEMD 0x006C1240
// ??0?$TClassFactory@VRadSiteClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C1250
// ?QueryInterface@?$TClassFactory@VRadSiteClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C12C0
// ?AddRef@?$TClassFactory@VRadSiteClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C12E0
// ?Release@?$TClassFactory@VRadSiteClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1310
// ?CreateInstance@?$TClassFactory@VRadSiteClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1390
// ?LockServer@?$TClassFactory@VRadSiteClass@@@@UAGJH@Z
template class TClassFactory<RadSiteClass>;

// TEMPLATE: GAMEMD 0x006C13C0
// ??0?$TClassFactory@VTEventClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C13D0
// ?QueryInterface@?$TClassFactory@VTEventClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1440
// ?AddRef@?$TClassFactory@VTEventClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1460
// ?Release@?$TClassFactory@VTEventClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1490
// ?CreateInstance@?$TClassFactory@VTEventClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1510
// ?LockServer@?$TClassFactory@VTEventClass@@@@UAGJH@Z
template class TClassFactory<TEventClass>;

// TEMPLATE: GAMEMD 0x006C1540
// ??0?$TClassFactory@VVoxelAnimTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C1550
// ?QueryInterface@?$TClassFactory@VVoxelAnimTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C15C0
// ?AddRef@?$TClassFactory@VVoxelAnimTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C15E0
// ?Release@?$TClassFactory@VVoxelAnimTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1610
// ?CreateInstance@?$TClassFactory@VVoxelAnimTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1690
// ?LockServer@?$TClassFactory@VVoxelAnimTypeClass@@@@UAGJH@Z
template class TClassFactory<VoxelAnimTypeClass>;

// TEMPLATE: GAMEMD 0x006C16C0
// ??0?$TClassFactory@VVoxelAnimClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C16D0
// ?QueryInterface@?$TClassFactory@VVoxelAnimClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1740
// ?AddRef@?$TClassFactory@VVoxelAnimClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1760
// ?Release@?$TClassFactory@VVoxelAnimClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1790
// ?CreateInstance@?$TClassFactory@VVoxelAnimClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1810
// ?LockServer@?$TClassFactory@VVoxelAnimClass@@@@UAGJH@Z
template class TClassFactory<VoxelAnimClass>;

// TEMPLATE: GAMEMD 0x006C1840
// ??0?$TClassFactory@VTActionClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C1850
// ?QueryInterface@?$TClassFactory@VTActionClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C18C0
// ?AddRef@?$TClassFactory@VTActionClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C18E0
// ?Release@?$TClassFactory@VTActionClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1910
// ?CreateInstance@?$TClassFactory@VTActionClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1990
// ?LockServer@?$TClassFactory@VTActionClass@@@@UAGJH@Z
template class TClassFactory<TActionClass>;

// TEMPLATE: GAMEMD 0x006C19C0
// ??0?$TClassFactory@VTriggerClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C19D0
// ?QueryInterface@?$TClassFactory@VTriggerClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1A40
// ?AddRef@?$TClassFactory@VTriggerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1A60
// ?Release@?$TClassFactory@VTriggerClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1A90
// ?CreateInstance@?$TClassFactory@VTriggerClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1B10
// ?LockServer@?$TClassFactory@VTriggerClass@@@@UAGJH@Z
template class TClassFactory<TriggerClass>;

// TEMPLATE: GAMEMD 0x006C1B40
// ??0?$TClassFactory@VTriggerTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C1B50
// ?QueryInterface@?$TClassFactory@VTriggerTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1BC0
// ?AddRef@?$TClassFactory@VTriggerTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1BE0
// ?Release@?$TClassFactory@VTriggerTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1C10
// ?CreateInstance@?$TClassFactory@VTriggerTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1C90
// ?LockServer@?$TClassFactory@VTriggerTypeClass@@@@UAGJH@Z
template class TClassFactory<TriggerTypeClass>;

// TEMPLATE: GAMEMD 0x006C1CC0
// ??0?$TClassFactory@VScriptClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C1CD0
// ?QueryInterface@?$TClassFactory@VScriptClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1D40
// ?AddRef@?$TClassFactory@VScriptClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1D60
// ?Release@?$TClassFactory@VScriptClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1D90
// ?CreateInstance@?$TClassFactory@VScriptClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1E10
// ?LockServer@?$TClassFactory@VScriptClass@@@@UAGJH@Z
template class TClassFactory<ScriptClass>;

// TEMPLATE: GAMEMD 0x006C1E40
// ??0?$TClassFactory@VScriptTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C1E50
// ?QueryInterface@?$TClassFactory@VScriptTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1EC0
// ?AddRef@?$TClassFactory@VScriptTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1EE0
// ?Release@?$TClassFactory@VScriptTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C1F10
// ?CreateInstance@?$TClassFactory@VScriptTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C1F90
// ?LockServer@?$TClassFactory@VScriptTypeClass@@@@UAGJH@Z
template class TClassFactory<ScriptTypeClass>;

// TEMPLATE: GAMEMD 0x006C1FC0
// ??0?$TClassFactory@VTagClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C1FD0
// ?QueryInterface@?$TClassFactory@VTagClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2040
// ?AddRef@?$TClassFactory@VTagClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2060
// ?Release@?$TClassFactory@VTagClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2090
// ?CreateInstance@?$TClassFactory@VTagClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2110
// ?LockServer@?$TClassFactory@VTagClass@@@@UAGJH@Z
template class TClassFactory<TagClass>;

// TEMPLATE: GAMEMD 0x006C2140
// ??0?$TClassFactory@VTagTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C2150
// ?QueryInterface@?$TClassFactory@VTagTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C21C0
// ?AddRef@?$TClassFactory@VTagTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C21E0
// ?Release@?$TClassFactory@VTagTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2210
// ?CreateInstance@?$TClassFactory@VTagTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2290
// ?LockServer@?$TClassFactory@VTagTypeClass@@@@UAGJH@Z
template class TClassFactory<TagTypeClass>;

// TEMPLATE: GAMEMD 0x006C22C0
// ??0?$TClassFactory@VTeamClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C22D0
// ?QueryInterface@?$TClassFactory@VTeamClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2340
// ?AddRef@?$TClassFactory@VTeamClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2360
// ?Release@?$TClassFactory@VTeamClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2390
// ?CreateInstance@?$TClassFactory@VTeamClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2410
// ?LockServer@?$TClassFactory@VTeamClass@@@@UAGJH@Z
template class TClassFactory<TeamClass>;

// TEMPLATE: GAMEMD 0x006C2440
// ??0?$TClassFactory@VTeamTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C2450
// ?QueryInterface@?$TClassFactory@VTeamTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C24C0
// ?AddRef@?$TClassFactory@VTeamTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C24E0
// ?Release@?$TClassFactory@VTeamTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2510
// ?CreateInstance@?$TClassFactory@VTeamTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2590
// ?LockServer@?$TClassFactory@VTeamTypeClass@@@@UAGJH@Z
template class TClassFactory<TeamTypeClass>;

// TEMPLATE: GAMEMD 0x006C25C0
// ??0?$TClassFactory@VTaskForceClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C25D0
// ?QueryInterface@?$TClassFactory@VTaskForceClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2640
// ?AddRef@?$TClassFactory@VTaskForceClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2660
// ?Release@?$TClassFactory@VTaskForceClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2690
// ?CreateInstance@?$TClassFactory@VTaskForceClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2710
// ?LockServer@?$TClassFactory@VTaskForceClass@@@@UAGJH@Z
template class TClassFactory<TaskForceClass>;

// TEMPLATE: GAMEMD 0x006C2740
// ??0?$TClassFactory@VUnitTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C2750
// ?QueryInterface@?$TClassFactory@VUnitTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C27C0
// ?AddRef@?$TClassFactory@VUnitTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C27E0
// ?Release@?$TClassFactory@VUnitTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2810
// ?CreateInstance@?$TClassFactory@VUnitTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2890
// ?LockServer@?$TClassFactory@VUnitTypeClass@@@@UAGJH@Z
template class TClassFactory<UnitTypeClass>;

// TEMPLATE: GAMEMD 0x006C28C0
// ??0?$TClassFactory@VBuildingTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C28D0
// ?QueryInterface@?$TClassFactory@VBuildingTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2940
// ?AddRef@?$TClassFactory@VBuildingTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2960
// ?Release@?$TClassFactory@VBuildingTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2990
// ?CreateInstance@?$TClassFactory@VBuildingTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2A10
// ?LockServer@?$TClassFactory@VBuildingTypeClass@@@@UAGJH@Z
template class TClassFactory<BuildingTypeClass>;

// TEMPLATE: GAMEMD 0x006C2A40
// ??0?$TClassFactory@VAircraftTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C2A50
// ?QueryInterface@?$TClassFactory@VAircraftTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2AC0
// ?AddRef@?$TClassFactory@VAircraftTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2AE0
// ?Release@?$TClassFactory@VAircraftTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2B10
// ?CreateInstance@?$TClassFactory@VAircraftTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2B90
// ?LockServer@?$TClassFactory@VAircraftTypeClass@@@@UAGJH@Z
template class TClassFactory<AircraftTypeClass>;

// TEMPLATE: GAMEMD 0x006C2BC0
// ??0?$TClassFactory@VInfantryTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C2BD0
// ?QueryInterface@?$TClassFactory@VInfantryTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2C40
// ?AddRef@?$TClassFactory@VInfantryTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2C60
// ?Release@?$TClassFactory@VInfantryTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2C90
// ?CreateInstance@?$TClassFactory@VInfantryTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2D10
// ?LockServer@?$TClassFactory@VInfantryTypeClass@@@@UAGJH@Z
template class TClassFactory<InfantryTypeClass>;

// TEMPLATE: GAMEMD 0x006C2D40
// ??0?$TClassFactory@VBulletTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C2D50
// ?QueryInterface@?$TClassFactory@VBulletTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2DC0
// ?AddRef@?$TClassFactory@VBulletTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2DE0
// ?Release@?$TClassFactory@VBulletTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2E10
// ?CreateInstance@?$TClassFactory@VBulletTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2E90
// ?LockServer@?$TClassFactory@VBulletTypeClass@@@@UAGJH@Z
template class TClassFactory<BulletTypeClass>;

// TEMPLATE: GAMEMD 0x006C2EC0
// ??0?$TClassFactory@VIsometricTileTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C2ED0
// ?QueryInterface@?$TClassFactory@VIsometricTileTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C2F40
// ?AddRef@?$TClassFactory@VIsometricTileTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2F60
// ?Release@?$TClassFactory@VIsometricTileTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C2F90
// ?CreateInstance@?$TClassFactory@VIsometricTileTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3010
// ?LockServer@?$TClassFactory@VIsometricTileTypeClass@@@@UAGJH@Z
template class TClassFactory<IsometricTileTypeClass>;

// TEMPLATE: GAMEMD 0x006C3040
// ??0?$TClassFactory@VOverlayTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C3050
// ?QueryInterface@?$TClassFactory@VOverlayTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C30C0
// ?AddRef@?$TClassFactory@VOverlayTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C30E0
// ?Release@?$TClassFactory@VOverlayTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3110
// ?CreateInstance@?$TClassFactory@VOverlayTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3190
// ?LockServer@?$TClassFactory@VOverlayTypeClass@@@@UAGJH@Z
template class TClassFactory<OverlayTypeClass>;

// TEMPLATE: GAMEMD 0x006C31C0
// ??0?$TClassFactory@VSmudgeTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C31D0
// ?QueryInterface@?$TClassFactory@VSmudgeTypeClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3240
// ?AddRef@?$TClassFactory@VSmudgeTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3260
// ?Release@?$TClassFactory@VSmudgeTypeClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3290
// ?CreateInstance@?$TClassFactory@VSmudgeTypeClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3310
// ?LockServer@?$TClassFactory@VSmudgeTypeClass@@@@UAGJH@Z
template class TClassFactory<SmudgeTypeClass>;

// TEMPLATE: GAMEMD 0x006C3340
// ??0?$TClassFactory@VUnitClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C3350
// ?QueryInterface@?$TClassFactory@VUnitClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C33C0
// ?AddRef@?$TClassFactory@VUnitClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C33E0
// ?Release@?$TClassFactory@VUnitClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3410
// ?CreateInstance@?$TClassFactory@VUnitClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3490
// ?LockServer@?$TClassFactory@VUnitClass@@@@UAGJH@Z
template class TClassFactory<UnitClass>;

// TEMPLATE: GAMEMD 0x006C34C0
// ??0?$TClassFactory@VBuildingClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C34D0
// ?QueryInterface@?$TClassFactory@VBuildingClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3540
// ?AddRef@?$TClassFactory@VBuildingClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3560
// ?Release@?$TClassFactory@VBuildingClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3590
// ?CreateInstance@?$TClassFactory@VBuildingClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3610
// ?LockServer@?$TClassFactory@VBuildingClass@@@@UAGJH@Z
template class TClassFactory<BuildingClass>;

// TEMPLATE: GAMEMD 0x006C3640
// ??0?$TClassFactory@VAircraftClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C3650
// ?QueryInterface@?$TClassFactory@VAircraftClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C36C0
// ?AddRef@?$TClassFactory@VAircraftClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C36E0
// ?Release@?$TClassFactory@VAircraftClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3710
// ?CreateInstance@?$TClassFactory@VAircraftClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3790
// ?LockServer@?$TClassFactory@VAircraftClass@@@@UAGJH@Z
template class TClassFactory<AircraftClass>;

// TEMPLATE: GAMEMD 0x006C37C0
// ??0?$TClassFactory@VInfantryClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C37D0
// ?QueryInterface@?$TClassFactory@VInfantryClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3840
// ?AddRef@?$TClassFactory@VInfantryClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3860
// ?Release@?$TClassFactory@VInfantryClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3890
// ?CreateInstance@?$TClassFactory@VInfantryClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3910
// ?LockServer@?$TClassFactory@VInfantryClass@@@@UAGJH@Z
template class TClassFactory<InfantryClass>;

// TEMPLATE: GAMEMD 0x006C3940
// ??0?$TClassFactory@VAnimClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x006C3950
// ?QueryInterface@?$TClassFactory@VAnimClass@@@@UAGJABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C39C0
// ?AddRef@?$TClassFactory@VAnimClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C39E0
// ?Release@?$TClassFactory@VAnimClass@@@@UAGKXZ
// TEMPLATE: GAMEMD 0x006C3A10
// ?CreateInstance@?$TClassFactory@VAnimClass@@@@UAGJPAUIUnknown@@ABU_GUID@@PAPAX@Z
// TEMPLATE: GAMEMD 0x006C3A90
// ?LockServer@?$TClassFactory@VAnimClass@@@@UAGJH@Z
template class TClassFactory<AnimClass>;
