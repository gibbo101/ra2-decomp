// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <comdef.h>

EXTERN_C const IID IID_IFlyControl;

MIDL_INTERFACE("820F501C-4F39-11D2-9B70-00104B972FE8")
IFlyControl : public IUnknown
{
public:

	virtual LONG STDMETHODCALLTYPE Landing_Altitude(void) = 0;

	virtual LONG STDMETHODCALLTYPE Landing_Direction(void) = 0;

	virtual BOOL STDMETHODCALLTYPE Is_Loaded(void) = 0;

	virtual LONG STDMETHODCALLTYPE Is_Strafe(void) = 0;

	virtual LONG STDMETHODCALLTYPE Is_Fighter(void) = 0;

	virtual LONG STDMETHODCALLTYPE Is_Locked(void) = 0;
};

_COM_SMARTPTR_TYPEDEF(IFlyControl, __uuidof(IFlyControl));
