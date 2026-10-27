// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x005C91D0 SYMBOL
// _$E38

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
extern const double DAT_007EE5E0;
extern const double DAT_007EE5E8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x005C91F0 SYMBOL
// _$E2
double DAT_00ABF8F8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005C9220 SYMBOL
// _$E5
double DAT_00ABF948 = DAT_00ABF8F8 / DAT_007EE5E0;

// SYNTHETIC: GAMEMD 0x005C9240 SYMBOL
// _$E8
double DAT_00ABF930 = DAT_007E1718 / DAT_00ABF948;

// SYNTHETIC: GAMEMD 0x005C9260 SYMBOL
// _$E11
double DAT_00ABF928 = DAT_007E1720 * DAT_007EE5E8;

// SYNTHETIC: GAMEMD 0x005C9280 SYMBOL
// _$E14
double DAT_00ABF900 = DAT_007E1728 * DAT_007EE5E8;

// SYNTHETIC: GAMEMD 0x005C92A0 SYMBOL
// _$E17
double DAT_00ABF8D8 = DAT_007E1730 * DAT_007EE5E8;

// SYNTHETIC: GAMEMD 0x005C92C0 SYMBOL
// _$E20
int DAT_00ABF950 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF8D8 - DAT_00ABF900) * DAT_00ABF8F8));

// SYNTHETIC: GAMEMD 0x005C9300 SYMBOL
// _$E23
double DAT_00ABF908 = FUN_004CADE0(DAT_00ABF950 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005C9330 SYMBOL
// _$E26
double DAT_00ABF920 = FUN_004CADE0((DAT_00ABF950 * 2) / DAT_00ABF8F8);

// SYNTHETIC: GAMEMD 0x005C9360 SYMBOL
// _$E29
int DAT_00ABF944 = (int)(DAT_00ABF950 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005C9390 SYMBOL
// _$E32
ShortPair DAT_00ABF8F0;

// SYNTHETIC: GAMEMD 0x005C93A0 SYMBOL
// _$E35
IntTriple DAT_00ABF910;

#include "mpobserver.h"
#include "mpsiege.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

extern int DAT_00ABF938;
extern int DAT_00ABF93C;
extern int DAT_00ABF940;
class HouseClass;

class MPRevealMap
{
	public:
		void FUN_00577F30(HouseClass * house);
};

extern MPRevealMap DAT_0087F7E8;
bool __stdcall FUN_005D8C90(int index);

// FUNCTION: GAMEMD 0x005C93C0
void Reset_Observer_View(void)
{
	DAT_00ABF940 = 0;
	DAT_00ABF938 = 0x80;
	DAT_00ABF93C = 0x80;
}

// FUNCTION: GAMEMD 0x005C93E0
MPMegawealthClass::MPMegawealthClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MultiplayerGameMode(title, tooltip, inifile, mapfilter, aiallowed, modeindex)
{
}

// FUNCTION: GAMEMD 0x005C9420
MPMegawealthClass::~MPMegawealthClass(void)
{
}

// FUNCTION: GAMEMD 0x005C9430
bool MPMegawealthClass::vt_entry_8C(void)
{
	DAT_0087F7E8.FUN_00577F30(NULL);
	return(true);
}

// SYNTHETIC: GAMEMD 0x005C9440
// ??_GMPMegawealthClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x005C9470
MPObserverTeam::MPObserverTeam(void) :
	MultiplayerTeam(StringTable::LoadString("MP:ObserverTeam", NULL, "D:\\ra2mdpost\\MPObserver.cpp", 5), -1)
{
}

// FUNCTION: GAMEMD 0x005C94C0
bool MPObserverTeam::IsTeamIncluded(int index)
{
	return(FUN_005D8C90(index));
}

// SYNTHETIC: GAMEMD 0x005C94D0
// ??_GMPObserverTeam@@UAEPAXI@Z
