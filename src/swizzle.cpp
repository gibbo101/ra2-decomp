// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "swizzle.h"
#include "vector.h"

// FUNCTION: GAMEMD 0x006CF180
SwizzleManagerClass::SwizzleManagerClass(void)
{
	RequestTable.Set_Growth_Step(1000);
	PointerTable.Set_Growth_Step(1000);
}

// FUNCTION: GAMEMD 0x006CF1D0
SwizzleManagerClass::~SwizzleManagerClass(void)
{
	Process_Tables();
}

// FUNCTION: GAMEMD 0x006CF230
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Reset(void)
{
	Process_Tables();

	return(S_OK);
}

// FUNCTION: GAMEMD 0x006CF240
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

// FUNCTION: GAMEMD 0x006CF2C0
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Here_I_Am(LONG id, void *pointer)
{
	SwizzlePointerClass pair;
	pair.Pointer = pointer;
	pair.ID = id;
	PointerTable.Add(pair);

	return(S_OK);
}

// FUNCTION: GAMEMD 0x006CF320
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

// FUNCTION: GAMEMD 0x006CF350
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

// FUNCTION: GAMEMD 0x006CF410
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Get_Save_Size(int *size)
{
	if (size == NULL) {
		return(E_POINTER);
	}

	*size = sizeof(int);
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006CF430
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

// FUNCTION: GAMEMD 0x006CF490
HRESULT STDMETHODCALLTYPE SwizzleManagerClass::Fetch_Swizzle_ID(void *pointer, LONG *id)
{
	if (pointer != NULL && id != 0) {
		*id = (LONG)pointer;
		return(S_OK);
	}

	return(E_POINTER);
}

// FUNCTION: GAMEMD 0x006CF4B0
// SwizzleManagerClass::AddRef
// FUNCTION: GAMEMD 0x006CF4C0
// SwizzleManagerClass::Release
// FUNCTION: GAMEMD 0x006CF4D0
// SwizzleManagerClass::Save_Interface
// FUNCTION: GAMEMD 0x006CF4E0
// SwizzleManagerClass::Load_Interface

// TEMPLATE: GAMEMD 0x006CF4F0
// ?Clear@?$DynamicVectorClass@VSwizzlePointerClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006CF520
// ?ID@?$DynamicVectorClass@VSwizzlePointerClass@@@@UAEHPBVSwizzlePointerClass@@@Z
// TEMPLATE: GAMEMD 0x006CF540
// ?GetItem@?$VectorClass@VSwizzlePointerClass@@@@UBE?AVSwizzlePointerClass@@H@Z
// TEMPLATE: GAMEMD 0x006CF720
// ?Resize@?$DynamicVectorClass@VSwizzlePointerClass@@@@UAE_NHPBVSwizzlePointerClass@@@Z
// TEMPLATE: GAMEMD 0x006CF820
// ?ID@?$DynamicVectorClass@VSwizzlePointerClass@@@@UAEHABVSwizzlePointerClass@@@Z
// SYNTHETIC: GAMEMD 0x006CF8A0
// ??_G?$DynamicVectorClass@VSwizzlePointerClass@@@@UAEPAXI@Z
template class DynamicVectorClass<SwizzlePointerClass>;

// TEMPLATE: GAMEMD 0x006CF560
// ??8?$VectorClass@VSwizzlePointerClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x006CF5A0
// ?Resize@?$VectorClass@VSwizzlePointerClass@@@@UAE_NHPBVSwizzlePointerClass@@@Z
// TEMPLATE: GAMEMD 0x006CF690
// ?Clear@?$VectorClass@VSwizzlePointerClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006CF6C0
// ?ID@?$VectorClass@VSwizzlePointerClass@@@@UAEHABVSwizzlePointerClass@@@Z
// TEMPLATE: GAMEMD 0x006CF700
// ?ID@?$VectorClass@VSwizzlePointerClass@@@@UAEHPBVSwizzlePointerClass@@@Z
// SYNTHETIC: GAMEMD 0x006CF850
// ??_G?$VectorClass@VSwizzlePointerClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x006CF8F0
// ??0?$VectorClass@VSwizzlePointerClass@@@@QAE@HPBVSwizzlePointerClass@@@Z
template class VectorClass<SwizzlePointerClass>;
