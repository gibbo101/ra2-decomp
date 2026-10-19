// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "list.h"
#include "_convert.h"
#include "_surface.h"
#include "bound.h"
#include "convert.h"
#include "data.h"
#include "dialog.h"
#include "dsurface.h"
#include "font.h"
#include "language\language.h"
#include "scheme.h"
#include "vector.h"
#include "wwmouse.h"

// FUNCTION: 0x00557AC0
wchar_t const * ListClass::Get_Item(int index) const
{
	if (List.Count() == 0) {
		return(NULL);
	}
	index = MIN(index, List.Count()-1);
	return(List[index]);
}

// FUNCTION: 0x00557D10
void ListClass::Set_Tabs(int const * tabs)
{
	Tabs = tabs;
}
