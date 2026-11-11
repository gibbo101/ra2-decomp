// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "wwmous.h"
#include "globals.h"
#include "bsurface.h"

// FUNCTION: GAMEMD 0x007BA320
bool WWMouseClass::Is_Hidden(void) const
{
	return RefCount < 0;
}

// FUNCTION: GAMEMD 0x007BA330
bool WWMouseClass::Is_Captured(void) const
{
	return field_10;
}

// FUNCTION: GAMEMD 0x007BA340
int WWMouseClass::Get_Mouse_X(void) const
{
	return field_1C;
}

// FUNCTION: GAMEMD 0x007BA350
int WWMouseClass::Get_Mouse_Y(void) const
{
	return field_20;
}

// FUNCTION: GAMEMD 0x007BA360
Point2D WWMouseClass::Get_Mouse_Point(void) const
{
	return Point2D(field_1C, field_20);
}

// FUNCTION: GAMEMD 0x007BA380
void WWMouseClass::Set_Mouse_Point(int x, int y)
{
	field_1C = x;
	field_20 = y;
}
