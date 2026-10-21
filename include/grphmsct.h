// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "grphmitm.h"

class GraphicMenuShortcutItem : public GraphicMenuItem
{
	typedef DynamicVectorClass<unsigned int> SHORTCUT_LIST;
	typedef VectorCursor<unsigned int, SHORTCUT_LIST> SHORTCUT_ITER;

	public:
		GraphicMenuShortcutItem(int id, Wstring const & keys);
		virtual ~GraphicMenuShortcutItem(void) override;
		virtual bool Is_Input_Key(KeyNumType key) override;

	private:

		SHORTCUT_LIST Shortcuts;
};
