// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "layer.h"
#include "swizzle.h"

// FUNCTION: GAMEMD 0x005519B0
bool LayerClass::Submit(ObjectClass const * object, bool sort)
{
	if (sort) {
		return((Sorted_Add(object)) != false);
	}
	return(Add((ObjectClass *)object) != false);
}

// FUNCTION: GAMEMD 0x00551A30
void LayerClass::Sort(void)
{
	for (int index = 0; index < Count()-1; index++) {
		ObjectClass *o1 = (*this)[index+1];
		ObjectClass *o2 = (*this)[index];
		int t1 = o1->Sort_Y();
		int t2 = o2->Sort_Y();
		if (t1 < t2) {
			(*this)[index+1] = o2;
			(*this)[index] = o1;
		}
	}
}

// FUNCTION: GAMEMD 0x00551A90
int LayerClass::Sorted_Add(ObjectClass const * const object)
{
	if (ActiveCount >= Length()) {
		if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {
				return(false);
			}
		} else {
			return(false);
		}
	}

	int index;
	for (index = 0; index < ActiveCount; index++) {
		if ((*(*this)[index]) > (*object)) {
			break;
		}
	}

	for (int i = ActiveCount-1; i >= index; i--) {
		(*this)[i+1] = (*this)[i];
	}
	(*this)[index] = (ObjectClass *)object;
	ActiveCount++;
	return(true);
}

// FUNCTION: GAMEMD 0x00551B20
HRESULT LayerClass::Save(IStream * stream)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	int count = Count();

	HRESULT result = stream->Write(&count, sizeof(count), NULL);
	if (SUCCEEDED(result)) {
		for (int index = 0; index < count; index++) {
			result = stream->Write(&Vector[index], sizeof(int), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}
		return(S_OK);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00551B90
HRESULT LayerClass::Load(IStream * stream)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	int count = 0;
	ObjectClass *obj;

	HRESULT result = stream->Read(&count, sizeof(count), NULL);
	if (SUCCEEDED(result)) {
		int index;
		for (index = 0; index < count; index++) {
			result = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(result)) {
				return(result);
			}
			Add(obj);
		}

		for (index = 0; index < count; index++) {
			Swizzle_Pointer(&Vector[index]);
		}

		return(S_OK);
	}
	return(result);
}
