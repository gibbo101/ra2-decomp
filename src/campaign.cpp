// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "campaign.h"
#include "assert.h"
#include "ccini.h"
#include "crc.h"
#include "globals.h"
#include "noinit.h"
#include "sun.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x0046CF80
HRESULT STDMETHODCALLTYPE CampaignClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_CampaignClass;
	return(S_OK);
}

// FUNCTION: 0x0046D050
HRESULT STDMETHODCALLTYPE CampaignClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
