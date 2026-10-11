// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "trigtype.h"
#include "_map.h"
#include "_rules.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "sun.h"
#include "swizzle.h"
#include "taction.h"
#include "teamtype.h"
#include "tevent.h"
#include "tracker.h"
#include "persist.hh"
#include <cstdio>

static char const * PersistentName[3] = {
	"Volatile",
	"Semi-persistant",
	"Persistent"
};

// FUNCTION: 0x00727190
PersistentType Persistence_From_Name(char const * name)
{
	for (int index = 0; index < ARRAY_SIZE(PersistentName); index++) {
		if (stricmp(name, PersistentName[index]) == 0) {
			return(PersistentType(index));
		}
	}
	return(VOLATILE);
}

// FUNCTION: 0x007271D0
char const * Name_From_Persistence(PersistentType pers)
{
	return(PersistentName[pers]);
}

// FUNCTION: 0x00727BB0
HRESULT STDMETHODCALLTYPE TriggerTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TriggerTypeClass;
	return(S_OK);
}

// FUNCTION: 0x00727C80
HRESULT STDMETHODCALLTYPE TriggerTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
