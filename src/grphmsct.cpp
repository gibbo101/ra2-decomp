// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmsct.h"
#include "grphmenu.h"
#include "grphmitm.h"
#include "grphmsct.h"
#include "ini.h"
#include "keyboard.h"

// FUNCTION: 0x004F3E90
GraphicMenuShortcutItem::~GraphicMenuShortcutItem(void)
{
}

// FUNCTION: 0x004F3EE0
bool GraphicMenuShortcutItem::Is_Input_Key(KeyNumType key)
{
	if (!Enabled) {
		return(false);
	}

	SHORTCUT_ITER it = Shortcuts;
	for (; it; it++) {
		if (*it == (unsigned int)key) {
			break;
		}
	}

	return(it);
}
