// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "loaddlg.h"
#include "campaign.h"
#include "cd.h"
#include "conquer.h"
#include "data.h"
#include "globals.h"
#include "houstype.h"
#include "init.h"
#include "language\language.h"
#include "msgbox.h"
#include "ownrdraw.h"
#include "saveload.h"
#include "savever.h"
#include "scenario.h"
#include "session.h"
#include "win.h"

// FUNCTION: 0x005587F0
bool LoadOptionsClass::Load(void)
{
	Style = LOAD;
	Description = NULL;
	return(Dialog());
}

// FUNCTION: 0x00558840
bool LoadOptionsClass::Delete(void)
{
	Style = WWDELETE;
	Description = NULL;
	return(Dialog());
}

// FUNCTION: 0x00559EB0
bool LoadOptionsClass::Delete_File(const char * file_name)
{
	if (DeleteFile(file_name) == TRUE) {
		return(true);
	}
	return(false);
}
