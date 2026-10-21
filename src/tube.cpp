// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tube.h"
#include "_map.h"
#include "ccini.h"
#include "cell.h"
#include "coord.h"
#include "crc.h"
#include "globals.h"
#include "inline.h"
#include "tracker.h"
#include <cstdio>

char const * TubeClass::INI_NAME = "Tubes";

// FUNCTION: GAMEMD 0x00727FD0
TubeClass::TubeClass(Cell const &cell, FacingType dir) :
	Enter(cell),
	Exit(cell),
	EnterDir(dir),
	Count(0)
{
	Create_ID();

	for (int i = 0; i < 100; i++) {
		Dirs[i] = FACING_NONE;
	}

	Tubes.Add(this);

	if (cell != Cell(0, 0)) {
		CellClass *cptr = &Map[cell];
		cptr->Tube = Tubes.ID(this);
	}
}

// FUNCTION: GAMEMD 0x007280D0
TubeClass::~TubeClass(void)
{
	Detach_This_From_All(this, true);

	CellClass *cptr = &Map[Enter];
	if (cptr) {
		if (cptr->Tube == Tubes.ID(this)) {
			cptr->Tube = -1;
		}
	}

	Tubes.Delete(this);
}

// FUNCTION: GAMEMD 0x007281A0
HRESULT STDMETHODCALLTYPE TubeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TubeClass(NoInitClass());

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x007281E0
HRESULT STDMETHODCALLTYPE TubeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00728200
void TubeClass::Assign_Tubes(void)
{
	int tube_count = Tubes.Count();
	int current_tube = -1;
	for (int index = 0; index < tube_count; index++) {
		TubeClass * tube = Tubes[index];
		CellClass * cptr = &Map[tube->Enter];

		if (!cptr->Has_Tunnel()) {
			delete tube;
			tube_count--;
			cptr->Tube = -1;
			index--;
		} else {
			cptr->Tube = ++current_tube;
		}
	}
}

// FUNCTION: GAMEMD 0x00728280
void TubeClass::Write_INI(CCINIClass & ini)
{
	char key[12];
	char buffer[550];

	ini.Clear(INI_NAME);
	Assign_Tubes();

	for (int index = 0; index < Tubes.Count(); index++) {

		TubeClass * tube = Tubes[index];
		if (tube != NULL) {

			sprintf(key, "%d", index);
			sprintf(buffer, "%d,%d,%d,%d,%d", tube->Enter.X, tube->Enter.Y, tube->EnterDir, tube->Exit.X, tube->Exit.Y);
			for (int dir = 0; dir < 100; dir++) {
				sprintf(buffer + strlen(buffer), ",%d", tube->Dirs[dir]);
			}
			ini.Put_String(INI_NAME, key, buffer);
		}
	}
}

// FUNCTION: GAMEMD 0x007283C0
void TubeClass::Read_INI(CCINIClass const & ini)
{
	char buffer[INIClass::MAX_LINE_LENGTH];

	const_cast<CCINIClass &>(ini).Reset();
	int count = ini.Entry_Count(INI_NAME);

	for (int index = 0; index < count; index++) {

		char const * entry = ini.Get_Entry(INI_NAME, index);
		ini.Get_String(INI_NAME, entry, NULL, buffer, sizeof(buffer));

		TubeClass * tube = new TubeClass(Cell(0,0));

		tube->Enter.X = atoi(strtok(buffer, ","));
		tube->Enter.Y = atoi(strtok(NULL, ","));
		tube->EnterDir = (FacingType)atoi(strtok(NULL, ","));
		tube->Exit.X = atoi(strtok(NULL, ","));
		tube->Exit.Y = atoi(strtok(NULL, ","));

		tube->Count = -1;
		for (int dir = 0; dir < 100; dir++) {
			tube->Dirs[dir] = (FacingType)atoi(strtok(NULL, ","));
			tube->Count++;
			if (tube->Dirs[dir] == FACING_NONE) break;
		}

		Map[tube->Enter].Tube = index;
	}
}

// FUNCTION: GAMEMD 0x00728540
bool TubeClass::Not_In_Tube(Cell const & cell) const
{
	Cell tubecell = Enter;

	if (tubecell == cell) {
		return(true);
	}

	int dir = 0;
	while (Dirs[dir] != FACING_NONE) {
		tubecell = Adjacent_Cell(tubecell, Dirs[dir++]);
		if (tubecell == cell) {
			return(false);
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x007285D0
void TubeClass::Add_Direction(FacingType dir)
{
	if (Count < 99) {
		Dirs[Count] = dir;
		Dirs[++Count] = FACING_NONE;
	}

	Exit = Adjacent_Cell(Exit, dir);
}

// FUNCTION: GAMEMD 0x00728630
void TubeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(Enter.X);
	crc(Enter.Y);
	crc(Exit.X);
	crc(Exit.Y);
	crc(EnterDir);
	for (int i = 0; i < 100; i++) {
		crc(Dirs[i]);
	}
	crc(Count);
}

// FUNCTION: GAMEMD 0x007286B0
int TubeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x007286C0
RTTIType TubeClass::Fetch_RTTI(void) const
{
	return(RTTI_TUBE);
}

// FUNCTION: GAMEMD 0x007286D0
HRESULT STDMETHODCALLTYPE TubeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TubeClass;
	return(S_OK);
}

// SYNTHETIC: GAMEMD 0x00728710
// ??_GTubeClass@@UAEPAXI@Z

#include <math.h>

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
extern const double DAT_007F5A10;
extern const double DAT_007F5A18;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x007287F0 SYMBOL
// _$E53
double DAT_00B0F8F8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x00728820 SYMBOL
// _$E56
double DAT_00B0F948 = DAT_00B0F8F8 / DAT_007F5A10;

// SYNTHETIC: GAMEMD 0x00728840 SYMBOL
// _$E59
double DAT_00B0F930 = DAT_007E1718 / DAT_00B0F948;

// SYNTHETIC: GAMEMD 0x00728860 SYMBOL
// _$E62
double DAT_00B0F928 = DAT_007E1720 * DAT_007F5A18;

// SYNTHETIC: GAMEMD 0x00728880 SYMBOL
// _$E65
double DAT_00B0F900 = DAT_007E1728 * DAT_007F5A18;

// SYNTHETIC: GAMEMD 0x007288A0 SYMBOL
// _$E68
double DAT_00B0F8D8 = DAT_007E1730 * DAT_007F5A18;

// SYNTHETIC: GAMEMD 0x007288C0 SYMBOL
// _$E71
int DAT_00B0F950 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0F8D8 - DAT_00B0F900) * DAT_00B0F8F8));

// SYNTHETIC: GAMEMD 0x00728900 SYMBOL
// _$E74
double DAT_00B0F908 = FUN_004CADE0(DAT_00B0F950 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00728930 SYMBOL
// _$E77
double DAT_00B0F920 = FUN_004CADE0((DAT_00B0F950 * 2) / DAT_00B0F8F8);

// SYNTHETIC: GAMEMD 0x00728960 SYMBOL
// _$E80
int DAT_00B0F944 = (int)(DAT_00B0F950 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x00728990 SYMBOL
// _$E83
ShortPair DAT_00B0F8F0;

// SYNTHETIC: GAMEMD 0x007289A0 SYMBOL
// _$E86
IntTriple DAT_00B0F910;
