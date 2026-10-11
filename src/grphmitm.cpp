// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmitm.h"
#include "ini.h"
#include "mschoice.h"

// FUNCTION: 0x004F3A50
GraphicMenuItem::GraphicMenuItem(int id) :
	ID(id),
	Selected(false),
	Enabled(true),
	SelectSound(NULL)
{
}

// FUNCTION: 0x004F3A70
GraphicMenuItem::~GraphicMenuItem(void)
{
	if (SelectSound != NULL) {
		delete SelectSound;
	}
}

// FUNCTION: 0x004F3A90
void GraphicMenuItem::Set_Selected(bool selected)
{
	if (selected != Selected) {
		Selected = selected;
		On_Selected_Change(selected);
	}
}

// FUNCTION: 0x004F3AB0
void GraphicMenuItem::Set_Enabled(bool enabled)
{
	if (enabled != Enabled) {
		Enabled = enabled;
		On_Enabled_Change(enabled);
	}
}

// FUNCTION: 0x004F3B10
void GraphicMenuItem::Action(MSEngine * engine)
{
	if (SelectSound != NULL) {
		SelectSound->Play();
	}
}
