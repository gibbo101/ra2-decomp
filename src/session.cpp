// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "session.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rules.h"
#include "addon.h"
#include "ccini.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "msgloop.h"
#include "nullmgr.h"
#include "phone.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "special.h"
#include "wonline.h"
#include "stringtable.h"
#include "xstraw.h"
#include <ctime> // for station ID computation
#include <dos.h> // for station ID computation
#include <math.h>

// FUNCTION: 0x00696D00 $E2
// FUNCTION: 0x00696D20 $E5
// FUNCTION: 0x00696D50 $E8
// FUNCTION: 0x00696D70 $E11
// FUNCTION: 0x00696D90 $E14
// FUNCTION: 0x00696DB0 $E17
// FUNCTION: 0x00696DD0 $E20
// FUNCTION: 0x00696DF0 $E23
// FUNCTION: 0x00696E30 $E26
// FUNCTION: 0x00696E60 $E29
// FUNCTION: 0x00696E90 $E32
// FUNCTION: 0x00696EC0 $E35
// FUNCTION: 0x00696ED0 $E38
// FUNCTION: 0x00696EF0 $E41

// FUNCTION: 0x00696F10
int NodeNameType::Get_Country(void)
{
	return (Slot.InitialCountry == -2) ? -2 : Slot.Country;
}

// FUNCTION: 0x00696F20
int NodeNameType::Get_Color(void)
{
	return (Slot.InitialColor == -2) ? -2 : Slot.ColorIdx;
}

// FUNCTION: 0x00696F30
int NodeNameType::Get_Color_Or_Random(void)
{
	if (Slot.InitialColor != -2 || Slot.ColorIdx != -1) {
		return Slot.ColorIdx;
	}
	return -2;
}

// FUNCTION: 0x00696F50
int NodeNameType::Get_Start(void)
{
	if (Slot.InitialStart != -2 || Slot.Start != -1) {
		return Slot.Start;
	}
	return -2;
}

// FUNCTION: 0x00696F70
int NodeNameType::Get_Team(void)
{
	if (Slot.InitialTeam != -2 || Slot.Team != -1) {
		return Slot.Team;
	}
	return -2;
}

// FUNCTION: 0x00696F90
void NodeNameType::Set_Country(int value)
{
	if (value == -2) {
		Slot.Country = 0;
		Slot.InitialCountry = value;
	} else {
		Slot.Country = value;
		Slot.InitialCountry = -1;
	}
}

// FUNCTION: 0x00696FC0
void NodeNameType::Set_Color(int value)
{
	if (value == -2) {
		Slot.ColorIdx = -1;
		Slot.InitialColor = value;
	} else {
		Slot.ColorIdx = value;
		Slot.InitialColor = -1;
	}
}

// FUNCTION: 0x00696FF0
void NodeNameType::Set_Start(int value)
{
	if (value == -2) {
		Slot.Start = -1;
		Slot.InitialStart = value;
	} else {
		Slot.Start = value;
		Slot.InitialStart = -1;
	}
}

// FUNCTION: 0x00697020
void NodeNameType::Set_Team(int value)
{
	if (value == -2) {
		Slot.Team = -1;
		Slot.InitialTeam = value;
	} else {
		Slot.Team = value;
		Slot.InitialTeam = -1;
	}
}

// FUNCTION: 0x00697050
SessionOptionsClass::SessionOptionsClass(void)
{
	MPGameMode = 0;
	ScenIndex = 0;
	GameSpeed = 1;
	Credits = 10000;
	UnitCount = 10;
	ShortGame = true;
	SuperWeaponsAllowed = true;
	BuildOffAlly = true;
	MCVRepacks = true;
	CratesAppear = true;
	for (int i = 0; i < 8; i++) {
		SlotData[i][0] = 0;
		SlotData[i][1] = 0;
		SlotData[i][2] = 0;
	}
}

// FUNCTION: 0x006977C0
void SessionClass::Load_Call_Wait_Strings(void)
{
	for (int i = 0; i < CALL_WAIT_STRINGS_NUM; i++) {
		CallWaitStrings.Add((char const *)_wcsdup(StringTable::LoadString(CallWaitStringIDs[i], NULL, "D:\\ra2mdpost\\Session.CPP", 593)));
	}
}

// FUNCTION: 0x00697840
SessionClass::~SessionClass(void)
{
	while (InitStrings.Count()) {
		delete[] InitStrings[0];
		InitStrings.Delete_Index(0);
	}

	while (CallWaitStrings.Count()) {
		free((void *)CallWaitStrings[0]);
		CallWaitStrings.Delete_Index(0);
	}
}

// FUNCTION: 0x0069A340
unsigned int SessionClass::Compute_Unique_ID(void)
{
	unsigned int id;
	char *path;
	unsigned i;

	id = timeGetTime();

	unsigned int space_available = Disk_Space_Available();
	Add_CRC(&id, space_available);

	path = getenv("PATH");
	if (path) {
		for (i = 0; i < strlen(path); i++) {
			Add_CRC(&id, (unsigned int)path[i]);
		}
	}

	return(id);
}

// FUNCTION: 0x0069B560
bool GameOptionsType::Save(IStream * stream) const
{
	if (stream != NULL) {
		unsigned int size = sizeof(*this);
		HRESULT res = stream->Write(&size, sizeof(size), NULL);
		if (SUCCEEDED(res)) {
			res = stream->Write(this, size, NULL);
			if (SUCCEEDED(res)) {
				return(true);
			}
		}
	}

	return(false);
}
