// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "session.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rules.h"
#include "addon.h"
#include "ccini.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "msgloop.h"
#include "nullmgr.h"
#include "phone.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "special.h"
#include "wonline.h"
#include "xstraw.h"
#include <ctime> // for station ID computation
#include <dos.h> // for station ID computation
#include <math.h>

// FUNCTION: 0x00696D00 $E2
// FUNCTION: 0x00696D20 $E5
// FUNCTION: 0x00696D50 $E8
// FUNCTION: 0x00696D70 $E11
// FUNCTION: 0x00696D90 $E14
// FUNCTION: 0x00696DB0 $E17
// FUNCTION: 0x00696DD0 $E20
// FUNCTION: 0x00696DF0 $E23
// FUNCTION: 0x00696E30 $E26
// FUNCTION: 0x00696E60 $E29
// FUNCTION: 0x00696E90 $E32
// FUNCTION: 0x00696EC0 $E35
// FUNCTION: 0x00696ED0 $E38
// FUNCTION: 0x00696EF0 $E41

// FUNCTION: 0x0069A340
unsigned int SessionClass::Compute_Unique_ID(void)
{
	unsigned int id;
	char *path;
	unsigned i;

	id = timeGetTime();

	unsigned int space_available = Disk_Space_Available();
	Add_CRC(&id, space_available);

	path = getenv("PATH");
	if (path) {
		for (i = 0; i < strlen(path); i++) {
			Add_CRC(&id, (unsigned int)path[i]);
		}
	}

	return(id);
}

// FUNCTION: 0x0069B560
bool GameOptionsType::Save(IStream * stream) const
{
	if (stream != NULL) {
		unsigned int size = sizeof(*this);
		HRESULT res = stream->Write(&size, sizeof(size), NULL);
		if (SUCCEEDED(res)) {
			res = stream->Write(this, size, NULL);
			if (SUCCEEDED(res)) {
				return(true);
			}
		}
	}

	return(false);
}
