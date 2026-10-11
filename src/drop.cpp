// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "drop.h"
#include "font.h"
#include "shapeset.h"

// FUNCTION: 0x004B4F20
LinkClass & DropListClass::Add(LinkClass & object)
{
	DropButton.Add(object);
	return((DropListClass &)BASECLASS::Add(object));
}

// FUNCTION: 0x004B4F40
LinkClass & DropListClass::Add_Tail(LinkClass & object)
{
	DropButton.Add_Tail(object);
	return((DropListClass &)BASECLASS::Add_Tail(object));
}

// FUNCTION: 0x004B4F60
LinkClass & DropListClass::Add_Head(LinkClass & object)
{
	DropButton.Add_Head(object);
	return((DropListClass &)BASECLASS::Add_Head(object));
}
