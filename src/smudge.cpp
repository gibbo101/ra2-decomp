// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "smudge.h"
#include "_map.h"
#include "ccini.h"
#include "cell.h"
#include "globals.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include <cstdio>

// FUNCTION: 0x006B4A50
SmudgeClass::SmudgeClass(SmudgeTypeClass const * type, Coord const & pos, HousesType house) :
	BASECLASS(),
	Class((SmudgeTypeClass *)type)
{
	Create_ID();
	Smudges.Add(this);
	if (pos != COORD_NONE) {
		ToOwn = house;
		if (!Unlimbo(pos)) {
			Delete_Me();
		} else {
			ToOwn = HOUSE_NONE;
		}
	}
}

// FUNCTION: 0x006B4B40
SmudgeClass::~SmudgeClass(void)
{
	Detach_This_From_All(this);
	Smudges.Delete(this);
	if (GameActive) {
		Limbo();
	}
	Class = NULL;
}

// FUNCTION: 0x006B4BE0
bool SmudgeClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		if (mark == MARK_DOWN || mark == MARK_DOWN_FORCED) {
			Cell origin = (Cell)PositionCell;

			if (ScenarioInit > 0 || Debug_Map || Class->Can_Place_Here(origin, true)) {
				Class->Place(origin);
			}
			IsDown = false;
			Delete_Me();
			return(true);
		}
	}

	return(false);
}

// FUNCTION: 0x006B4C70
void SmudgeClass::Disown(Cell const & cell)
{
}

// FUNCTION: 0x006B4C80
void SmudgeClass::Read_INI(CCINIClass & ini)
{
	char	buf[128];

	ini.Reset();

	int len = ini.Entry_Count(INI_NAME);
	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry(INI_NAME, index);
		SmudgeType	smudge;

		ini.Get_String(INI_NAME, entry, NULL, buf, sizeof(buf));
		smudge = SmudgeTypeClass::From_Name(strtok(buf, ","));
		if (smudge != SMUDGE_NONE) {
			char * ptr = strtok(NULL, ",");
			if (ptr != NULL) {

				int x;
				int y;
				x = atoi(ptr);
				ptr = strtok(NULL, ",");
				y = atoi(ptr);
				Cell cell(x, y);

				ptr = strtok(NULL, ",");

				if (ptr == NULL || atoi(ptr) == 0) {
					new SmudgeClass(SmudgeTypes[smudge], cell);
				}
			}
		}
	}
}

// FUNCTION: 0x006B4DD0
void SmudgeClass::Write_INI(CCINIClass & ini)
{
	ini.Clear(INI_NAME);

	int index = 0;
	for (int y = 0; y < MAP_CELL_H; y++) {
		for (int x = 0; x < MAP_CELL_W; x++) {
			CellClass * ptr;

			ptr = &Map[Cell(x, y)];
			if (ptr->Smudge != SMUDGE_NONE && ptr->SmudgeData == 0) {
				SmudgeTypeClass const * stype = SmudgeTypes[ptr->Smudge];
				char	uname[10];
				char	buf[127];

				sprintf(uname, "%d", index);
				sprintf(buf, "%s,%d,%d,%d", (char const *)stype->IniName, x, y, ptr->SmudgeData);
				ini.Put_String(INI_NAME, uname, buf);
				index++;
			}
		}
	}
}

// FUNCTION: 0x006B4EA0
HRESULT STDMETHODCALLTYPE SmudgeClass::Load(IStream *stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) SmudgeClass(NoInitClass());

		Swizzle_Pointer(&Class);
	}
	return(result);
}

// FUNCTION: 0x006B4F00
HRESULT STDMETHODCALLTYPE SmudgeClass::Save(IStream *stream, int cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x006B4F20
ObjectTypeClass const * SmudgeClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: 0x006B4F30
int SmudgeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x006B4F40
RTTIType SmudgeClass::Fetch_RTTI(void) const
{
	return(RTTI_SMUDGE);
}

// FUNCTION: 0x006B4F50
HRESULT STDMETHODCALLTYPE SmudgeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SmudgeClass;
	return(S_OK);
}

// FUNCTION: 0x006B4F90 SmudgeClass::Draw_It

// FUNCTION: 0x006B4FA0 SmudgeClass::~SmudgeClass
