// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "swizzle.h"
#include "vector.h"

int __cdecl ptr_compare_func(const void *ptr1, const void *ptr2)
{
	const SwizzlePointerClass *p1 = static_cast<const SwizzlePointerClass *>(ptr1);
	const SwizzlePointerClass *p2 = static_cast<const SwizzlePointerClass *>(ptr2);

	if (p1->ID == p2->ID) {
		return(0);
	}
	if (p1->ID < p2->ID) {
		return(-1);
	}
	return(1);
}

// FUNCTION: 0x006CF180
SwizzleManagerClass::SwizzleManagerClass(void)
{
	RequestTable.Set_Growth_Step(1000);
	PointerTable.Set_Growth_Step(1000);
}

// FUNCTION: 0x006CF1D0
SwizzleManagerClass::~SwizzleManagerClass(void)
{
	Process_Tables();
}

// FUNCTION: 0x006CF230
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Reset(void)
{
	Process_Tables();

	return(S_OK);
}

// FUNCTION: 0x006CF240
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Swizzle(void **pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}

	LONG id = LONG(*pointer);
	if (!id) {
		return(S_OK);
	}

	SwizzlePointerClass pair(id, pointer);
	RequestTable.Add(pair);

	*pointer = NULL;

	return(S_OK);
}

// FUNCTION: 0x006CF2C0
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Here_I_Am(LONG id, void *pointer)
{
	SwizzlePointerClass pair;
	pair.Pointer = pointer;
	pair.ID = id;
	PointerTable.Add(pair);

	return(S_OK);
}

// FUNCTION: 0x006CF350
void SwizzleManagerClass::Process_Tables(void)
{
	if (RequestTable.Count() != 0) {
		if (PointerTable.Count() > 0) {
			qsort(&PointerTable[0], PointerTable.Count(), sizeof(SwizzlePointerClass), ptr_compare_func);
		}
		if (RequestTable.Count() > 0) {
			qsort(&RequestTable[0], RequestTable.Count(), sizeof(SwizzlePointerClass), ptr_compare_func);
		}

		int pointer_count = PointerTable.Count();
		SwizzlePointerClass *pointer_entry = &PointerTable[0];

		int request_count = RequestTable.Count();
		SwizzlePointerClass *request_entry = &RequestTable[0];

		while (request_count > 0) {
			LONG req_id = request_entry->ID;
			LONG ptr_id = pointer_entry->ID;

			if (req_id == ptr_id) {
				LONG *ptr = (LONG *)request_entry->Pointer;
				*ptr = (LONG)pointer_entry->Pointer;

				request_entry++;
				request_count--;
				continue;
			}

			while (req_id > ptr_id) {
				pointer_entry++;
				pointer_count--;
				ptr_id = pointer_entry->ID;
			}

			ptr_id = pointer_entry->ID;

			if (req_id != ptr_id) {
				volatile int div_zero = 0;
				div_zero = 1 / div_zero;
			}
		}

		RequestTable.Clear();
		PointerTable.Clear();
	}
}

// FUNCTION: 0x006CF410
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Get_Save_Size(int *size)
{
	if (size == NULL) {
		return(E_POINTER);
	}

	*size = sizeof(int);
	return(S_OK);
}

// FUNCTION: 0x006CF430
LONG STDMETHODCALLTYPE SwizzleManagerClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	if ( riid != IID_ISwizzle && riid != IID_IUnknown) {
		return(E_NOINTERFACE);
	}

	*ppvObject = reinterpret_cast<ISwizzle *>(this);

	reinterpret_cast<IUnknown *>(*ppvObject)->AddRef();
	return(S_OK);
}

// FUNCTION: 0x006CF490
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Fetch_Swizzle_ID(void *pointer, LONG *id)
{
	if (pointer != NULL && id != 0) {
		*id = (LONG)pointer;
		return(S_OK);
	}

	return(E_POINTER);
}
