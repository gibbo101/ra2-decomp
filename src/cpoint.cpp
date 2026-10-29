// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "cpoint.h"
#include "globals.h"
#include "scenario.h"
#include "geometry.h"
#include "vector.h"
#include "win.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x0049FD30 SYMBOL
// _$E2

extern ULONG COMRefCount;


// SYNTHETIC: GAMEMD 0x0049FD50 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0049FD80 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0049FDA0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0049FDC0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0049FDE0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0049FE00 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0049FE20 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0049FE60 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0049FE90 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0049FEC0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0049FEF0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0049FF00 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x0049FF20 SYMBOL
// _$E44

// FUNCTION: GAMEMD 0x0049FF40
bool operator!=(CONNECTDATA const &left, CONNECTDATA const &right)
{
	return(left.dwCookie != right.dwCookie || left.pUnk != right.pUnk);
}

// FUNCTION: GAMEMD 0x0049FF60
bool operator==(CONNECTDATA const &left, CONNECTDATA const &right)
{
	return(left.dwCookie == right.dwCookie && left.pUnk == right.pUnk);
}

// FUNCTION: GAMEMD 0x0049FF80
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

// FUNCTION: GAMEMD 0x0049FFF0
ULONG EnumConnectionsClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: GAMEMD 0x004A0010
ULONG EnumConnectionsClass::Release(void)
{
	COMRefCount--;

	LONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}

	return(count);
}

// FUNCTION: GAMEMD 0x004A00B0
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

// FUNCTION: GAMEMD 0x004A0160
HRESULT EnumConnectionsClass::Skip(ULONG cConnections)
{
	ListPosition += cConnections;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A0180
HRESULT EnumConnectionsClass::Reset(void)
{
	ListPosition = 0;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A0190
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

// FUNCTION: GAMEMD 0x004A02B0
EnumConnectionsClass::EnumConnectionsClass(DynamicVectorClass<CONNECTDATA> &list) :
	List(list),
	ListPosition(0),
	RefCount(0)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i].pUnk->AddRef();
	}
}

// FUNCTION: GAMEMD 0x004A0380
EnumConnectionsClass::EnumConnectionsClass(EnumConnectionsClass &that) :
	List(that.List),
	ListPosition(that.ListPosition),
	RefCount(0)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i].pUnk->AddRef();
	}
}

// FUNCTION: GAMEMD 0x004A0450
EnumConnectionsClass::~EnumConnectionsClass(void)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i].pUnk->Release();
	}

	List.Clear();
}

// FUNCTION: GAMEMD 0x004A04B0
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

// FUNCTION: GAMEMD 0x004A0520
ULONG ConnectionPointClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: GAMEMD 0x004A0540
ULONG ConnectionPointClass::Release(void)
{
	COMRefCount--;

	LONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}

	return(count);
}

// FUNCTION: GAMEMD 0x004A05D0
HRESULT ConnectionPointClass::GetConnectionInterface(IID *pIID)
{
	if (pIID == NULL) {
		return(E_POINTER);
	}

	*pIID = InterfaceID;

	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A0610
HRESULT ConnectionPointClass::GetConnectionPointContainer(IConnectionPointContainer** ppCPC)
{
	return(Object->QueryInterface(IID_IConnectionPointContainer, (void**)ppCPC));
}

// FUNCTION: GAMEMD 0x004A0630
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

// FUNCTION: GAMEMD 0x004A0700
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

// FUNCTION: GAMEMD 0x004A0760
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

// FUNCTION: GAMEMD 0x004A0870
ConnectionPointClass::ConnectionPointClass(REFIID riid, IUnknown *obj) :
	InterfaceID(riid),
	Object(obj),
	RefCount(0)
{
	obj->AddRef();
}

// FUNCTION: GAMEMD 0x004A08D0
ConnectionPointClass::~ConnectionPointClass(void)
{
	Object->Release();
	List.Clear();
}

// FUNCTION: GAMEMD 0x004A0920
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

// FUNCTION: GAMEMD 0x004A0990
ULONG EnumConnectionPointsClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: GAMEMD 0x004A09B0
ULONG EnumConnectionPointsClass::Release(void)
{
	COMRefCount--;

	LONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}

	return(count);
}

// FUNCTION: GAMEMD 0x004A0A50
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

// FUNCTION: GAMEMD 0x004A0AF0
HRESULT EnumConnectionPointsClass::Skip(ULONG cConnections)
{
	ListPosition += cConnections;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A0B10
HRESULT EnumConnectionPointsClass::Reset(void)
{
	ListPosition = 0;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A0B20
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

// FUNCTION: GAMEMD 0x004A0C20
EnumConnectionPointsClass::EnumConnectionPointsClass(DynamicVectorClass<IConnectionPoint *> &list) :
	ListPosition(0),
	RefCount(0),
	List(list)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i]->AddRef();
	}
}

// FUNCTION: GAMEMD 0x004A0CE0
EnumConnectionPointsClass::EnumConnectionPointsClass(EnumConnectionPointsClass &that) :
	ListPosition(that.ListPosition),
	RefCount(0),
	List(that.List)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i]->AddRef();
	}
}

// FUNCTION: GAMEMD 0x004A0DB0
EnumConnectionPointsClass::~EnumConnectionPointsClass(void)
{
	for (int i = 0; i < List.Count(); i++) {
		List[i]->Release();
	}

	List.Clear();
}

// SYNTHETIC: GAMEMD 0x004A0E10
// ??_G?$DynamicVectorClass@UtagCONNECTDATA@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x004A0E60
// ??_G?$DynamicVectorClass@PAUIConnectionPoint@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x004A0EB0
// ?Clear@?$DynamicVectorClass@UtagCONNECTDATA@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004A0EE0
// ?ID@?$DynamicVectorClass@UtagCONNECTDATA@@@@UAEHPBUtagCONNECTDATA@@@Z
// TEMPLATE: GAMEMD 0x004A0F00
// ?GetItem@?$VectorClass@UtagCONNECTDATA@@@@UBE?AUtagCONNECTDATA@@H@Z
// TEMPLATE: GAMEMD 0x004A0F20
// ?Clear@?$DynamicVectorClass@PAUIConnectionPoint@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004A0F50
// ?ID@?$DynamicVectorClass@PAUIConnectionPoint@@@@UAEHPBQAUIConnectionPoint@@@Z
// TEMPLATE: GAMEMD 0x004A0F70
// ?GetItem@?$VectorClass@PAUIConnectionPoint@@@@UBEPAUIConnectionPoint@@H@Z
// TEMPLATE: GAMEMD 0x004A0F80
// ??8?$VectorClass@UtagCONNECTDATA@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004A0FE0
// ?Resize@?$VectorClass@UtagCONNECTDATA@@@@UAE_NHPBUtagCONNECTDATA@@@Z
// TEMPLATE: GAMEMD 0x004A10A0
// ?Clear@?$VectorClass@UtagCONNECTDATA@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004A10D0
// ?ID@?$VectorClass@UtagCONNECTDATA@@@@UAEHABUtagCONNECTDATA@@@Z
// TEMPLATE: GAMEMD 0x004A1120
// ?ID@?$VectorClass@UtagCONNECTDATA@@@@UAEHPBUtagCONNECTDATA@@@Z
// TEMPLATE: GAMEMD 0x004A1140
// ?Resize@?$DynamicVectorClass@UtagCONNECTDATA@@@@UAE_NHPBUtagCONNECTDATA@@@Z
// TEMPLATE: GAMEMD 0x004A1200
// ?ID@?$DynamicVectorClass@UtagCONNECTDATA@@@@UAEHABUtagCONNECTDATA@@@Z
// TEMPLATE: GAMEMD 0x004A1240
// ??8?$VectorClass@PAUIConnectionPoint@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004A1280
// ?Resize@?$VectorClass@PAUIConnectionPoint@@@@UAE_NHPBQAUIConnectionPoint@@@Z
// TEMPLATE: GAMEMD 0x004A1330
// ?Clear@?$VectorClass@PAUIConnectionPoint@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004A1360
// ?ID@?$VectorClass@PAUIConnectionPoint@@@@UAEHABQAUIConnectionPoint@@@Z
// TEMPLATE: GAMEMD 0x004A13A0
// ?ID@?$VectorClass@PAUIConnectionPoint@@@@UAEHPBQAUIConnectionPoint@@@Z
// TEMPLATE: GAMEMD 0x004A13C0
// ?Resize@?$DynamicVectorClass@PAUIConnectionPoint@@@@UAE_NHPBQAUIConnectionPoint@@@Z
// TEMPLATE: GAMEMD 0x004A1470
// ?ID@?$DynamicVectorClass@PAUIConnectionPoint@@@@UAEHABQAUIConnectionPoint@@@Z
// SYNTHETIC: GAMEMD 0x004A14A0
// ??_G?$VectorClass@UtagCONNECTDATA@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x004A14F0
// ??_G?$VectorClass@PAUIConnectionPoint@@@@UAEPAXI@Z
template class VectorClass<CONNECTDATA>;
template class DynamicVectorClass<CONNECTDATA>;
template class VectorClass<IConnectionPoint *>;
template class DynamicVectorClass<IConnectionPoint *>;
