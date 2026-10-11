// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mplayer.h"
#include "_surface.h"
#include "addon.h"
#include "init.h"
#include "msgbox.h"
#include "nulldlg.h"
#include "ownrdraw.h"
#include "session.h"
#include "worlddom.h"

// FUNCTION: 0x005C6120
void Clear_Vector(DynamicVectorClass <NodeNameType *> * vector)
{
	int i;

	for (i = 0; i < vector->Count(); i++) {
		delete (*vector)[i];
	}
	vector->Clear();
}
