// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "wwmous.h"
#include "globals.h"
#include "bsurface.h"

void __cdecl FUN_004068E0(char const * format, ...);

extern WWMouseClass * DAT_00B78164;
extern HANDLE DAT_00B78168;

#line 1274 "D:\\ra2mdpost\\wwmous.cpp"
static inline void Block_Mouse(void)
{
	if (WaitForSingleObject(DAT_00B78168, 10000) == WAIT_TIMEOUT) {
		FUN_004068E0("Warning: Probable deadlock occurred on MouseMutex. %s, line %d\n", __FILE__, __LINE__);
	}
}


static inline void Unblock_Mouse(void)
{
	ReleaseMutex(DAT_00B78168);
}

WWMouseClass::~WWMouseClass(void)
{
	Block_Mouse();
	if (DAT_00B78164 == this) {
		DAT_00B78164 = 0;
	}
	Unblock_Mouse();
	if (field_44 != 0) {
		delete field_44;
		field_44 = 0;
	}
	if (field_58 != 0) {
		delete field_58;
		field_58 = 0;
	}
	if (field_6C != 0) {
		delete field_6C;
		field_6C = 0;
	}
}

// FUNCTION: 0x007BA320
bool WWMouseClass::Is_Hidden(void) const
{
	return RefCount < 0;
}

// FUNCTION: 0x007BA330
bool WWMouseClass::Is_Captured(void) const
{
	return field_10;
}

// FUNCTION: 0x007BA340
int WWMouseClass::Get_Mouse_X(void) const
{
	return field_1C;
}

// FUNCTION: 0x007BA350
int WWMouseClass::Get_Mouse_Y(void) const
{
	return field_20;
}

// FUNCTION: 0x007BA360
Point2D WWMouseClass::Get_Mouse_Point(void) const
{
	return Point2D(field_1C, field_20);
}

// FUNCTION: 0x007BA380
void WWMouseClass::Set_Mouse_Point(int x, int y)
{
	field_1C = x;
	field_20 = y;
}

// FUNCTION: 0x007BA3A0 WWMouseClass::~WWMouseClass
