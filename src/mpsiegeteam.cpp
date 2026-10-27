// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x005CAC00 SYMBOL
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
extern const double DAT_007EE7D0;
extern const double DAT_007EE7D8;

extern int DAT_00ABFAE0;
extern int DAT_00ABFAE4;
extern int DAT_00ABFAE8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x005CAC20 SYMBOL
// _$E2
double DAT_00ABFAA0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005CAC50 SYMBOL
// _$E5
double DAT_00ABFAF0 = DAT_00ABFAA0 / DAT_007EE7D0;

// SYNTHETIC: GAMEMD 0x005CAC70 SYMBOL
// _$E8
double DAT_00ABFAD8 = DAT_007E1718 / DAT_00ABFAF0;

// SYNTHETIC: GAMEMD 0x005CAC90 SYMBOL
// _$E11
double DAT_00ABFAD0 = DAT_007E1720 * DAT_007EE7D8;

// SYNTHETIC: GAMEMD 0x005CACB0 SYMBOL
// _$E14
double DAT_00ABFAA8 = DAT_007E1728 * DAT_007EE7D8;

// SYNTHETIC: GAMEMD 0x005CACD0 SYMBOL
// _$E17
double DAT_00ABFA80 = DAT_007E1730 * DAT_007EE7D8;

// SYNTHETIC: GAMEMD 0x005CACF0 SYMBOL
// _$E20
int DAT_00ABFAF8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABFA80 - DAT_00ABFAA8) * DAT_00ABFAA0));

// SYNTHETIC: GAMEMD 0x005CAD30 SYMBOL
// _$E23
double DAT_00ABFAB0 = FUN_004CADE0(DAT_00ABFAF8 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005CAD60 SYMBOL
// _$E26
double DAT_00ABFAC8 = FUN_004CADE0((DAT_00ABFAF8 * 2) / DAT_00ABFAA0);

// SYNTHETIC: GAMEMD 0x005CAD90 SYMBOL
// _$E29
int DAT_00ABFAEC = (int)(DAT_00ABFAF8 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005CADC0 SYMBOL
// _$E32
ShortPair DAT_00ABFA98;

// SYNTHETIC: GAMEMD 0x005CADD0 SYMBOL
// _$E35
IntTriple DAT_00ABFAB8;

#include "mpsiege.h"
#include "session.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

// FUNCTION: GAMEMD 0x005CADF0
void Reset_Siege_Team_Stats(void)
{
	DAT_00ABFAE8 = 0;
	DAT_00ABFAE0 = 128;
	DAT_00ABFAE4 = 128;
}

// FUNCTION: GAMEMD 0x005CAE10
MPSiegeDefenderTeam::MPSiegeDefenderTeam(void) :
	MultiplayerTeam(StringTable::LoadString("MP:DefenderTeam", NULL, "D:\\ra2mdpost\\MPSiegeTeam.cpp", 6), 1)
{
}

// FUNCTION: GAMEMD 0x005CAE60
MPSiegeDefenderTeam::~MPSiegeDefenderTeam(void)
{
}

// FUNCTION: GAMEMD 0x005CAE70
bool __stdcall Siege_Only_Spectators_Remain(int self)
{
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (self != i && Session.Players[i]->Slot.SpectatorFlag == 1) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005CAEB0
MPSiegeAttackerTeam::MPSiegeAttackerTeam(void) :
	MultiplayerTeam(StringTable::LoadString("MP:AttackerTeam", NULL, "D:\\ra2mdpost\\MPSiegeTeam.cpp", 22), 1)
{
}
