// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cheklist.h"
#include "_convert.h"
#include "_surface.h"
#include "convert.h"
#include "dialog.h"
#include "dsurface.h"
#include "scheme.h"
#include "vector.h"
#include <cstdio>

// FUNCTION: 0x004882D0
CheckListClass::~CheckListClass(void)
{
	while (CheckListClass::Count()) {
		CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(0);

		BASECLASS::Remove_Item(0);
		delete obj;
	}
}

// FUNCTION: 0x00488310
int CheckListClass::Add_Item(wchar_t const * text)
{
	CheckObject * obj = new CheckObject(text, false);
	return(BASECLASS::Add_Item((wchar_t const *)obj));
}

// FUNCTION: 0x00488350
wchar_t const * CheckListClass::Current_Item(void) const
{
	CheckObject * obj = (CheckObject *)BASECLASS::Current_Item();
	if (obj) {
		return(obj->Text);
	}
	return(0);
}

// FUNCTION: 0x00488360
wchar_t const * CheckListClass::Get_Item(int index) const
{
	CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
	if (obj) {
		return(obj->Text);
	}
	return(0);
}

// FUNCTION: 0x00488450
void CheckListClass::Check_Item(int index, bool checked)
{
	CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
	if (obj && obj->IsChecked != checked) {
		obj->IsChecked = checked;
		Flag_To_Redraw();
	}
}

// FUNCTION: 0x00488480
bool CheckListClass::Is_Checked(int index) const
{
	CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
	if (obj) {
		return(obj->IsChecked);
	}
	return(false);
}
