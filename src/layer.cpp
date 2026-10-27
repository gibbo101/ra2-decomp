// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "layer.h"
#include "swizzle.h"

// SYNTHETIC: GAMEMD 0x005517A0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005517D0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005517F0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00551810 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00551830 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00551850 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00551870 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005518B0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005518E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00551910 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00551940 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00551950 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00551970 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00551990 SYMBOL
// _$E47

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
