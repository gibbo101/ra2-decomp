// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "abstract.h"
#include "_rtti.h"
#include "assert.h"
#include "coord.h"
#include "crc.h"
#include "globals.h"
#include "noinit.h"
#include "scenario.h"
#include "swizzle.h"
#include "vector.h"

// FUNCTION: 0x00410210
int STDMETHODCALLTYPE AbstractClass::What_Am_I(void) const
{
	return(RTTI);
}

// FUNCTION: 0x00410260
HRESULT STDMETHODCALLTYPE AbstractClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;

	if (riid == IID_IUnknown) {
		*ppvObject = (IUnknown *)(IPersistStream *)this;
	}
	if (riid == IID_IPersistStream) {
		*ppvObject = (IPersistStream *)this;
	}
	if (riid == IID_IPersist) {
		*ppvObject = (IPersist *)this;
	}
	if (riid == IID_IRTTITypeInfo) {
		*ppvObject = (IRTTITypeInfo *)this;
	}
	if (*ppvObject == NULL) {
		return(E_NOINTERFACE);
	}

	AddRef();
	return(S_OK);
}
