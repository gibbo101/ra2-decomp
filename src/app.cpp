// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "app.h"
#include "_map.h"
#include "globals.h"
#include "scenario.h"
#include "geometry.h"
#include "swizzle.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x00428FF0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00429010 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x00429040 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x00429060 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x00429080 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x004290A0 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x004290C0 SYMBOL
// _$E62
// SYNTHETIC: GAMEMD 0x004290E0 SYMBOL
// _$E65
// SYNTHETIC: GAMEMD 0x00429120 SYMBOL
// _$E68
// SYNTHETIC: GAMEMD 0x00429150 SYMBOL
// _$E71
// SYNTHETIC: GAMEMD 0x00429180 SYMBOL
// _$E74
// SYNTHETIC: GAMEMD 0x004291B0 SYMBOL
// _$E77
// SYNTHETIC: GAMEMD 0x004291C0 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x004291E0 SYMBOL
// _$E83
// SYNTHETIC: GAMEMD 0x00429200 SYMBOL
// _$E86
ApplicationClass ApplicationInstance;

// FUNCTION: GAMEMD 0x00429210
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

// FUNCTION: GAMEMD 0x00429270
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

// FUNCTION: GAMEMD 0x00429360
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

// FUNCTION: GAMEMD 0x004293E0
ULONG STDMETHODCALLTYPE ApplicationClass::AddRef(void)
{
	return(1);
}

// FUNCTION: GAMEMD 0x004293F0
ULONG STDMETHODCALLTYPE ApplicationClass::Release(void)
{
	return(1);
}

// FUNCTION: GAMEMD 0x00429400
HRESULT STDMETHODCALLTYPE ApplicationClass::App_Interface(IApplication **pVal)
{
	return(QueryInterface(IID_IApplication, (void **)pVal));
}

// FUNCTION: GAMEMD 0x00429420
HRESULT STDMETHODCALLTYPE ApplicationClass::get_FullName(BSTR *retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}

	*retval = SysAllocString(L"Tiberian Sun");

	return(S_OK);
}

// FUNCTION: GAMEMD 0x00429450
HRESULT STDMETHODCALLTYPE ApplicationClass::get_Name(BSTR *retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}

	*retval = SysAllocString(L"Sun");

	return(S_OK);
}

// FUNCTION: GAMEMD 0x00429480
HRESULT STDMETHODCALLTYPE ApplicationClass::Application_Interface(IApplication **pVal)
{
	return(QueryInterface(IID_IApplication, (void **)pVal));
}

// FUNCTION: GAMEMD 0x004294A0
HRESULT STDMETHODCALLTYPE ApplicationClass::Quit(void)
{
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004294B0
HRESULT STDMETHODCALLTYPE ApplicationClass::get_ScenarioName(BSTR *retval)
{
	wchar_t psz[32];

	if (retval == NULL) {
		return(E_POINTER);
	}

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Scen->ScenarioName, -1, psz, sizeof(psz)/2);
	*retval = SysAllocString(psz);

	return(S_OK);
}

// FUNCTION: GAMEMD 0x00429500
HRESULT STDMETHODCALLTYPE ApplicationClass::get_FrameCount(LONG *retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}

	*retval = Scen->ElapsedTimer();

	return(S_OK);
}

// FUNCTION: GAMEMD 0x00429550
HRESULT STDMETHODCALLTYPE ApplicationClass::get_Swizzle_Interface(ISwizzle **pVal)
{
	return(Swizzler.QueryInterface(IID_ISwizzle, (void**)pVal));
}
