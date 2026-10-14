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
#include "geometry.h"
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

// FUNCTION: 0x00558550 $E32
// FUNCTION: 0x00558580 $E35
// FUNCTION: 0x005585A0 $E38
// FUNCTION: 0x005585C0 $E41
// FUNCTION: 0x005585E0 $E44
// FUNCTION: 0x00558600 $E47
// FUNCTION: 0x00558620 $E50
// FUNCTION: 0x00558660 $E53
// FUNCTION: 0x00558690 $E56
// FUNCTION: 0x005586C0 $E59
// FUNCTION: 0x005586F0 $E62
// FUNCTION: 0x00558700 $E65

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
