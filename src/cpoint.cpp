// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cpoint.h"
#include "globals.h"
#include "scenario.h"
#include "vector.h"
#include "win.h"

extern ULONG COMRefCount;

// FUNCTION: 0x0049FF40
bool operator!=(CONNECTDATA const &left, CONNECTDATA const &right)
{
	return(left.dwCookie != right.dwCookie || left.pUnk != right.pUnk);
}

// FUNCTION: 0x0049FF60
bool operator==(CONNECTDATA const &left, CONNECTDATA const &right)
{
	return(left.dwCookie == right.dwCookie && left.pUnk == right.pUnk);
}

// FUNCTION: 0x0049FF80
HRESULT EnumConnectionsClass::QueryInterface(REFIID riid, LPVOID *ppv)
{
	if (ppv == NULL) {
		return(E_POINTER);
	}

	*ppv = NULL;

	if (riid == IID_IUnknown) {
		*ppv = (IUnknown *)(this);
	}
	if (riid == IID_IEnumConnections) {
		*ppv = (IEnumConnections *)this;
	}

	if (NULL != *ppv) {
		this->AddRef();
		return(S_OK);
	}

	return(E_NOINTERFACE);
}

// FUNCTION: 0x0049FFF0
ULONG EnumConnectionsClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: 0x004A0010
ULONG EnumConnectionsClass::Release(void)
{
	COMRefCount--;

	LONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}

	return(count);
}

// FUNCTION: 0x004A00B0
HRESULT EnumConnectionsClass::Next(ULONG cConnections, CONNECTDATA * rgpcd, ULONG * pcFetched)
{
	if (rgpcd == NULL) {
		return(E_POINTER);
	}

	if (cConnections > 1 && pcFetched == NULL) {
		return(E_INVALIDARG);
	}

	if (pcFetched != NULL) {
		*pcFetched = 0;
	}

	int count = 0;
	for (unsigned int i = 0; i < cConnections; i++) {
		if (ListPosition >= List.Count()) {
			break;
		}

		List[ListPosition].pUnk->AddRef();
		*rgpcd++ = List[ListPosition++];
		count++;
	}

	if (pcFetched != NULL) {
		*pcFetched = count;
	}

	return(count ? S_OK : E_FAIL);
}

// FUNCTION: 0x004A0160
HRESULT EnumConnectionsClass::Skip(ULONG cConnections)
{
	ListPosition += cConnections;
	return(S_OK);
}

// FUNCTION: 0x004A0180
HRESULT EnumConnectionsClass::Reset(void)
{
	ListPosition = 0;
	return(S_OK);
}

// FUNCTION: 0x004A0190
HRESULT EnumConnectionsClass::Clone(IEnumConnections** ppEnum)
{
	if (ppEnum == NULL) {
		return(E_POINTER);
	}

	EnumConnectionsClass *enumerator = new EnumConnectionsClass(*this);

	if (enumerator == NULL) {
		return(E_OUTOFMEMORY);
	}

	enumerator->RefCount = 0;
	enumerator->AddRef();

	*ppEnum = enumerator;

	return(S_OK);
}

// FUNCTION: 0x004A02B0
EnumConnectionsClass::EnumConnectionsClass(DynamicVectorClass<CONNECTDATA> &list) :
	List(list),
	ListPosition(0),
	RefCount(0)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i].pUnk->AddRef();
	}
}

// FUNCTION: 0x004A0380
EnumConnectionsClass::EnumConnectionsClass(EnumConnectionsClass &that) :
	List(that.List),
	ListPosition(that.ListPosition),
	RefCount(0)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i].pUnk->AddRef();
	}
}

// FUNCTION: 0x004A0450
EnumConnectionsClass::~EnumConnectionsClass(void)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i].pUnk->Release();
	}

	List.Clear();
}

// FUNCTION: 0x004A04B0
HRESULT ConnectionPointClass::QueryInterface(REFIID riid, void** ppv)
{
	if (ppv == NULL) {
		return(E_POINTER);
	}

	IUnknown * iface = NULL;

	if (riid == IID_IConnectionPoint || riid == IID_IUnknown) {
		iface = (IEnumConnections *)this;
	}

	*ppv = iface;

	if (iface != NULL) {
		iface->AddRef();
		return(S_OK);
	}

	return(E_NOINTERFACE);
}

// FUNCTION: 0x004A0520
ULONG ConnectionPointClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: 0x004A0540
ULONG ConnectionPointClass::Release(void)
{
	COMRefCount--;

	LONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}

	return(count);
}

// FUNCTION: 0x004A05D0
HRESULT ConnectionPointClass::GetConnectionInterface(IID *pIID)
{
	if (pIID == NULL) {
		return(E_POINTER);
	}

	*pIID = InterfaceID;

	return(S_OK);
}

// FUNCTION: 0x004A0610
HRESULT ConnectionPointClass::GetConnectionPointContainer(IConnectionPointContainer** ppCPC)
{
	return(Object->QueryInterface(IID_IConnectionPointContainer, (void**)ppCPC));
}

// FUNCTION: 0x004A0630
HRESULT ConnectionPointClass::Advise(IUnknown * pUnkSink, DWORD * pdwCookie)
{
	if (pUnkSink != NULL && pdwCookie != NULL) {
		CONNECTDATA data;

		data.dwCookie = Scen->Get_Unique_ID();
		if (FAILED(pUnkSink->QueryInterface(InterfaceID, (void **)&data.pUnk))) {
			return(CONNECT_E_CANNOTCONNECT);
		}

		if (!List.Add(data)) {
			return(CONNECT_E_ADVISELIMIT);
		}

		*pdwCookie = data.dwCookie;
		return(S_OK);
	}

	return(E_POINTER);
}

// FUNCTION: 0x004A0700
HRESULT ConnectionPointClass::Unadvise(DWORD dwCookie)
{
	for (int i = 0; i < List.Count(); i++) {
		if (List[i].dwCookie == dwCookie) {
			List.Delete_Index(i);
			return(S_OK);
		}
	}

	return(CONNECT_E_NOCONNECTION);
}

// FUNCTION: 0x004A0760
HRESULT ConnectionPointClass::EnumConnections(IEnumConnections** ppEnum)
{
	if (ppEnum == NULL) {
		return(E_POINTER);
	}

	EnumConnectionsClass * enumerator = new EnumConnectionsClass(List);

	if (enumerator == NULL) {
		return(E_OUTOFMEMORY);
	}

	return(enumerator->QueryInterface(IID_IEnumConnections, (void**)ppEnum));
}

// FUNCTION: 0x004A0870
ConnectionPointClass::ConnectionPointClass(REFIID riid, IUnknown *obj) :
	InterfaceID(riid),
	Object(obj),
	RefCount(0)
{
	obj->AddRef();
}

// FUNCTION: 0x004A08D0
ConnectionPointClass::~ConnectionPointClass(void)
{
	Object->Release();
	List.Clear();
}

// FUNCTION: 0x004A0920
HRESULT EnumConnectionPointsClass::QueryInterface(REFIID riid, LPVOID *ppv)
{
	if (ppv == NULL) {
		return(E_POINTER);
	}

	IUnknown * iface = NULL;

	if (riid == IID_IEnumConnectionPoints || riid == IID_IUnknown) {
		iface = (IEnumConnectionPoints *)this;
	}

	*ppv = iface;

	if (iface != NULL) {
		this->AddRef();
		return(S_OK);
	}

	return(E_NOINTERFACE);
}

// FUNCTION: 0x004A0990
ULONG EnumConnectionPointsClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: 0x004A09B0
ULONG EnumConnectionPointsClass::Release(void)
{
	COMRefCount--;

	LONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}

	return(count);
}

// FUNCTION: 0x004A0A50
HRESULT EnumConnectionPointsClass::Next(ULONG cConnections, IConnectionPoint ** rgpcn, ULONG * pcFetched)
{
	if (rgpcn == NULL) {
		return(E_POINTER);
	}

	if (cConnections > 1 && pcFetched == NULL) {
		return(E_POINTER);
	}

	if (pcFetched != NULL) {
		*pcFetched = 0;
	}

	int count = 0;
	for (unsigned int i = 0; i < cConnections; i++) {
		if (ListPosition >= List.Count()) {
			break;
		}

		List[ListPosition]->AddRef();
		*rgpcn++ = List[ListPosition++];
		count++;
	}

	if (pcFetched != NULL) {
		*pcFetched = count;
	}

	return(count ? S_OK : E_FAIL);
}

// FUNCTION: 0x004A0AF0
HRESULT EnumConnectionPointsClass::Skip(ULONG cConnections)
{
	ListPosition += cConnections;
	return(S_OK);
}

// FUNCTION: 0x004A0B10
HRESULT EnumConnectionPointsClass::Reset(void)
{
	ListPosition = 0;
	return(S_OK);
}

// FUNCTION: 0x004A0B20
HRESULT EnumConnectionPointsClass::Clone(IEnumConnectionPoints** ppEnum)
{
	if (ppEnum == NULL) {
		return(E_POINTER);
	}

	EnumConnectionPointsClass *enumerator = new EnumConnectionPointsClass(*this);

	if (enumerator == NULL) {
		return(E_OUTOFMEMORY);
	}

	enumerator->AddRef();

	*ppEnum = enumerator;

	return(S_OK);
}

// FUNCTION: 0x004A0C20
EnumConnectionPointsClass::EnumConnectionPointsClass(DynamicVectorClass<IConnectionPoint *> &list) :
	ListPosition(0),
	RefCount(0),
	List(list)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i]->AddRef();
	}
}

// FUNCTION: 0x004A0CE0
EnumConnectionPointsClass::EnumConnectionPointsClass(EnumConnectionPointsClass &that) :
	ListPosition(that.ListPosition),
	RefCount(0),
	List(that.List)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i]->AddRef();
	}
}

// FUNCTION: 0x004A0DB0
EnumConnectionPointsClass::~EnumConnectionPointsClass(void)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i]->Release();
	}

	List.Clear();
}
