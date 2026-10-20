// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "conquer.h"
#include "_keyboar.h"
#include "_map.h"
#include "_palette.h"
#include "_rules.h"
#include "_source.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "_tooltip.h"
#include "_wsproto.h"
#include "cctooltip.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "except.h"
#include "gamedlg.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "init.h"
#include "internet.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language\language.h"
#include "logic.h"
#include "mainloop.h"
#include "movie.h"
#include "mplayer.h"
#include "msgloop.h"
#include "netdlg.h"
#include "netdlg2.h"
#include "netshare.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "setpal.h"
#include "sidebar.h"
#include "sounddlg.h"
#include "stats.h"
#include "surface.h"
#include "tactical.h"
#include "theme.h"
#include "voc.h"
#include "vox.h"
#include "wolapi\wolapi.h"
#include "wonline.h"
#include "wsproto.h"
#include "special.hh"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <direct.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E51E0;
extern const double DAT_007E51E8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0048C6B0 $E87
double DAT_0089E948 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// FUNCTION: 0x0048C6E0 $E90
double DAT_0089E9A0 = DAT_0089E948 / DAT_007E51E0;

// FUNCTION: 0x0048C700 $E93
double DAT_0089E988 = DAT_007E1718 / DAT_0089E9A0;

// FUNCTION: 0x0048C720 $E96
double DAT_0089E980 = DAT_007E1720 * DAT_007E51E8;

// FUNCTION: 0x0048C740 $E99
double DAT_0089E958 = DAT_007E1728 * DAT_007E51E8;

// FUNCTION: 0x0048C760 $E102
double DAT_0089E918 = DAT_007E1730 * DAT_007E51E8;

// FUNCTION: 0x0048C780 $E105
int DAT_0089E9A8 = (int)(FUN_004CAD50(DAT_0089E918 - DAT_0089E958) * DAT_007E1738 * DAT_0089E948);

// FUNCTION: 0x0048C7C0 $E108
double DAT_0089E960 = FUN_004CADE0(DAT_0089E9A8 * DAT_007E1740);

// FUNCTION: 0x0048C7F0 $E111
double DAT_0089E978 = FUN_004CADE0((DAT_0089E9A8 * 2) / DAT_0089E948);

// FUNCTION: 0x0048C820 $E114
int DAT_0089E99C = (int)(DAT_0089E9A8 * 4 + DAT_007E1738);

// FUNCTION: 0x0048C850 $E117
ShortPair DAT_0089E940;

// FUNCTION: 0x0048C860 $E120
IntTriple DAT_0089E968;

// FUNCTION: 0x0048D1D0
bool MapGen_Call_Back(void)
{
	Call_Back();
	return(false);
}

// FUNCTION: 0x0048DBC0
char const * Name_From_Source(SourceType source)
{
	if ((unsigned)source < SOURCE_COUNT) {
		return(SourceName[source]);
	}
	return("None");
}

// FUNCTION: 0x0048DC20
FacingType KN_To_Facing(int input)
{
	input &= ~(KN_ALT_BIT|KN_SHIFT_BIT|KN_CTRL_BIT);
	switch (input) {
		case KN_LEFT:
			return(FACING_W);

		case KN_RIGHT:
			return(FACING_E);

		case KN_UP:
			return(FACING_N);

		case KN_DOWN:
			return(FACING_S);

		case KN_UPLEFT:
			return(FACING_NW);

		case KN_UPRIGHT:
			return(FACING_NE);

		case KN_DOWNLEFT:
			return(FACING_SW);

		case KN_DOWNRIGHT:
			return(FACING_SE);

		default:
			break;
	}
	return(FACING_NONE);
}

// FUNCTION: 0x0048DCD0
TechnoTypeClass const * Fetch_Techno_Type(RTTIType type, int id)
{
	switch (type) {
		case RTTI_UNITTYPE:
		case RTTI_UNIT:
			return(TechnoTypeClass *)(UnitTypes[id]);

		case RTTI_BUILDINGTYPE:
		case RTTI_BUILDING:
			return(TechnoTypeClass *)(BuildingTypes[id]);

		case RTTI_INFANTRYTYPE:
		case RTTI_INFANTRY:
			return(TechnoTypeClass *)(InfantryTypes[id]);

		case RTTI_AIRCRAFTTYPE:
		case RTTI_AIRCRAFT:
			return(TechnoTypeClass *)(AircraftTypes[id]);

		default:
			break;
	}
	return(NULL);
}

// FUNCTION: 0x0048DD50
unsigned int Disk_Space_Available(void)
{
	ULARGE_INTEGER freebytecount;
	ULARGE_INTEGER totalbytecount;
	ULARGE_INTEGER totalfreebytecount;
	unsigned int diskspace;

	bool (__stdcall *getfreediskspaceex) (LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

	DebugString("Checking available disk space\n");

	HINSTANCE kernel = GetModuleHandle("KERNEL32.DLL");
	if (kernel != NULL) {
		getfreediskspaceex = (bool (_stdcall*) (LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER)) GetProcAddress (kernel, "GetDiskFreeSpaceExA");
		if (getfreediskspaceex != NULL) {
			DebugString("Using GetDiskFreeSpaceEx\n");

			if (!getfreediskspaceex(NULL, &freebytecount, &totalbytecount, &totalfreebytecount)) {
				DebugString("GetDiskFreeSpaceEx failed with error code %d - %s\n", GetLastError(), Last_Error_Text());
			} else {
				diskspace = int(((int)freebytecount.LowPart + ((int)freebytecount.HighPart * (double)((__int64)UINT_MAX + 1))) / (double)1024);
				DebugString("Free disk space is %d Mb\n", diskspace / 1024);
				return(diskspace);
			}
		} else {
			DebugString("GetProcAddress failed with error code %d - %s\n", GetLastError(), Last_Error_Text());
		}
	} else {
		DebugString("Failed to get module handle for KERNEL32.DLL\n");
	}

	DWORD sectorspercluster, bytespersector, freeclustercount, totalclustercount;

	if (GetDiskFreeSpace(NULL, &sectorspercluster, &bytespersector, &freeclustercount, &totalclustercount)) {
		diskspace = ((sectorspercluster * bytespersector) / 1024) * freeclustercount;
		DebugString("Free disk space is %d Mb\n", diskspace / 1024);
		return(diskspace);
	}

	return(0);
}

// FUNCTION: 0x0048DE70
CrateType Crate_From_Name(char const * name)
{
	if (name != NULL) {
		for (CrateType crate = CRATE_FIRST; crate < CRATE_COUNT; crate++) {
			if (stricmp(name, CrateNames[crate]) == 0) return(crate);
		}
	}
	return(CRATE_MONEY);
}

// FUNCTION: 0x0048DEB0
int Owner_From_Name(char const * text)
{
	int ownable = 0;
	HousesType h = HouseTypeClass::From_Name(text);
	if (h != HOUSE_NONE) {
		ownable |= (1 << h);
	}
	return(ownable);
}

// FUNCTION: 0x0048DEE0
void List_Copy(Cell const * source, int len, Cell * dest)
{
	if (source == NULL || dest == NULL) {
		return;
	}

	while (len > 0) {
		*dest = *source;
		if (*dest == REFRESH_EOL) return;
		dest++;
		source++;
		len--;
	}

	*(dest-1) = REFRESH_EOL;
}

// FUNCTION: 0x0048DF30
VQType VQ_From_Name(char const * name)
{
	if (name != NULL && strcmpi("<none>", name)) {
		for (int movie = 0; movie < Movies.Count(); movie++) {
			if (stricmp(name, Movies[movie]) == 0) return(VQType(movie));
		}
	}
	return(VQ_NONE);
}

// FUNCTION: 0x0048DF80
LandType Land_From_Name(char const * name)
{
	if (strcmpi("<none>", name) && name != NULL) {
		for (LandType land = LAND_FIRST; land < LAND_COUNT; land++) {
			if (stricmp(LandName[land], name) == 0) return(land);
		}
	}
	return(LAND_NONE);
}

// FUNCTION: 0x0048DFD0
char const * Name_From_Land(LandType land)
{
	if ((unsigned)land < LAND_COUNT) {
		return(LandName[land]);
	}
	return("<none>");
}

// FUNCTION: 0x0048E030
char const * Name_From_Speed(SpeedType speed)
{
	if ((unsigned)speed < SPEED_COUNT) {
		return(SpeedName[speed]);
	}
	return("<none>");
}
