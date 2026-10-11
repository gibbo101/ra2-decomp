// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tooltip.h"
#include "data.h"
#include "dbgprint.h"

// FUNCTION: 0x00724BB0
void ToolTipManager::Draw(const ToolTipText *text)
{
	DebugString("ToolTip: %s\n", text->Text);
}
