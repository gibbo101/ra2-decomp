// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "app.h"
#include "_map.h"
#include "globals.h"
#include "scenario.h"
#include "swizzle.h"

// FUNCTION: 0x00429210
LONG STDMETHODCALLTYPE TiberianSunClassFactory::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	if (riid != IID_IClassFactory && riid != IID_IUnknown) {
		return(E_NOINTERFACE);
	}

	*ppvObject = static_cast<IClassFactory *>(this);

	reinterpret_cast<IUnknown *>(*ppvObject)->AddRef();
	return(S_OK);
}

// FUNCTION: 0x00429270
HRESULT STDMETHODCALLTYPE TiberianSunClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_INVALIDARG);
	}

	*ppvObject = NULL;

	if (pUnkOuter != NULL) {
		return(CLASS_E_NOAGGREGATION);
	}

	HRESULT res = E_NOINTERFACE;

	if (riid == IID_IApplication || riid == IID_IUnknown) {
		IUnknown * iterf = &ApplicationInstance;
		if (iterf == NULL) {
			return(E_OUTOFMEMORY);
		}
		res = iterf->QueryInterface(riid, ppvObject);
		if (FAILED(res)) {
			delete iterf;
		}
	}

	if (riid == IID_IGameMap) {
		IUnknown * iterf = &Map;
		if (iterf == NULL) {
			return(E_OUTOFMEMORY);
		}
		res = iterf->QueryInterface(riid, ppvObject);
		if (FAILED(res)) {
			delete iterf;
		}
	}
	return(res);
}

// FUNCTION: 0x00429360
LONG STDMETHODCALLTYPE ApplicationClass::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	if (riid == IID_IApplication || riid == IID_IUnknown) {
		*ppvObject = (IApplication*)this;
		((IUnknown*)(*ppvObject))->AddRef();
		return(S_OK);
	}

	if (riid == IID_IGameMap) {
		*ppvObject = (IGameMap*)&Map;
		return(S_OK);
	}

	return(E_NOINTERFACE);
}

// FUNCTION: 0x00429420
HRESULT STDMETHODCALLTYPE ApplicationClass::get_FullName(BSTR *retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}

	*retval = SysAllocString(L"Tiberian Sun");

	return(S_OK);
}

// FUNCTION: 0x00429450
HRESULT STDMETHODCALLTYPE ApplicationClass::get_Name(BSTR *retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}

	*retval = SysAllocString(L"Sun");

	return(S_OK);
}

// FUNCTION: 0x00429550
HRESULT STDMETHODCALLTYPE ApplicationClass::get_Swizzle_Interface(ISwizzle **pVal)
{
	return(Swizzler.QueryInterface(IID_ISwizzle, (void**)pVal));
}
