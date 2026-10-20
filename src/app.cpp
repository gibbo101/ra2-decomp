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
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E36B0;
extern const double DAT_007E36B8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct Int3Pad {
	int A, B, C;
	Int3Pad() { C = 0; A = 128; B = 128; }
};

struct Int4Zero {
	int A, B, C, D;
	Int4Zero() { A = 0; B = 0; C = 0; D = 0; }
};

// FUNCTION: 0x00429010 $E46
double DAT_0089A278 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00429040 $E49
double DAT_0089A2C8 = DAT_0089A278 / DAT_007E36B0;

// FUNCTION: 0x00429060 $E52
double DAT_0089A2B0 = DAT_007E1718 / DAT_0089A2C8;

// FUNCTION: 0x00429080 $E55
double DAT_0089A2A8 = DAT_007E1720 * DAT_007E36B8;

// FUNCTION: 0x004290A0 $E58
double DAT_0089A280 = DAT_007E1728 * DAT_007E36B8;

// FUNCTION: 0x004290C0 $E61
double DAT_0089A258 = DAT_007E1730 * DAT_007E36B8;

// FUNCTION: 0x004290E0 $E64
int DAT_0089A2D0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089A258 - DAT_0089A280) * DAT_0089A278));

// FUNCTION: 0x00429120 $E67
double DAT_0089A288 = FUN_004CADE0(DAT_0089A2D0 * DAT_007E1740);

// FUNCTION: 0x00429150 $E70
double DAT_0089A2A0 = FUN_004CADE0((DAT_0089A2D0 * 2) / DAT_0089A278);

// FUNCTION: 0x00429180 $E73
int DAT_0089A2C4 = (int)(DAT_0089A2D0 * 4 + DAT_007E1738);

// FUNCTION: 0x004291B0 $E76
ShortPair DAT_0089A270;

// FUNCTION: 0x004291C0 $E79
IntTriple DAT_0089A290;

// FUNCTION: 0x004291E0 $E82
Int3Pad DAT_0089A2B8;

// FUNCTION: 0x00429200 $E85
ApplicationClass ApplicationInstance;

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

// FUNCTION: 0x004293E0
ULONG STDMETHODCALLTYPE ApplicationClass::AddRef(void)
{
	return(1);
}

// FUNCTION: 0x004293F0
ULONG STDMETHODCALLTYPE ApplicationClass::Release(void)
{
	return(1);
}

// FUNCTION: 0x00429400
HRESULT STDMETHODCALLTYPE ApplicationClass::App_Interface(IApplication **pVal)
{
	return(QueryInterface(IID_IApplication, (void **)pVal));
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

// FUNCTION: 0x00429480
HRESULT STDMETHODCALLTYPE ApplicationClass::Application_Interface(IApplication **pVal)
{
	return(QueryInterface(IID_IApplication, (void **)pVal));
}

// FUNCTION: 0x004294A0
HRESULT STDMETHODCALLTYPE ApplicationClass::Quit(void)
{
	return(S_OK);
}

// FUNCTION: 0x004294B0
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

// FUNCTION: 0x00429500
HRESULT STDMETHODCALLTYPE ApplicationClass::get_FrameCount(LONG *retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}

	*retval = Scen->ElapsedTimer();

	return(S_OK);
}

// FUNCTION: 0x00429550
HRESULT STDMETHODCALLTYPE ApplicationClass::get_Swizzle_Interface(ISwizzle **pVal)
{
	return(Swizzler.QueryInterface(IID_ISwizzle, (void**)pVal));
}

// FUNCTION: 0x00429570 $E88
Int4Zero DAT_0089C268;
