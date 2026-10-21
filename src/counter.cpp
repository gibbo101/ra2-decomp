// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "counter.h"
#include <comdef.h>

// FUNCTION: GAMEMD 0x0049F9B0
CounterClass::CounterClass(void)
{
	TotalCount = 0;
}

// FUNCTION: GAMEMD 0x0049FAE0
int CounterClass::Value(int index) const
{
	if (const_cast<CounterClass *>(this)->Reserve(index)) {
		return(Vector[index]);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x0049FB40
void CounterClass::Clear(void)
{
	for (int i = 0; i < VectorMax; ++i) {
		Vector[i] = int(0);
	}
	TotalCount = 0;
}

// FUNCTION: GAMEMD 0x0049FB60
int CounterClass::Total(void) const
{
	return(TotalCount);
}

// FUNCTION: GAMEMD 0x0049FB70
HRESULT CounterClass::Save(IStream * stream)
{
	HRESULT hr = stream->Write(&TotalCount, sizeof(TotalCount), NULL);
	if (SUCCEEDED(hr)) {
		int count = VectorMax;
		hr = stream->Write(&count, sizeof(count), NULL);
		if (SUCCEEDED(hr)) {
			for (int index = 0; index < count; index++) {
				hr = stream->Write(&Vector[index], sizeof(int), NULL);
				if (FAILED(hr)) {
					return(hr);
				}
			}
			return(0);
		}
	}
	return(hr);
}

// FUNCTION: GAMEMD 0x0049FBE0
HRESULT CounterClass::Load(IStream * stream)
{
	new (this) CounterClass;

	HRESULT result = stream->Read(&TotalCount, sizeof(TotalCount), NULL);
	if (SUCCEEDED(result)) {
		int count;
		result = stream->Read(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			if (!Resize(count)) {
				return(8);
			}
			for (int index = 0; index < count; index++) {
				result = stream->Read(&Vector[index], sizeof(int), NULL);
				if (FAILED(result)) {
					return(result);
				}
			}
			return(S_OK);
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0049FC80
bool CounterClass::Reserve(int index)
{
	if (index < VectorMax) {
		return(true);
	}

	int oldmax = VectorMax;
	if (Resize(index + GROWTH_STEP)) {
		for (int i = oldmax; i < VectorMax; i++) {
			Vector[i] = int(0);
		}
		return(true);
	}
	return(false);
}

// SYNTHETIC: GAMEMD 0x0049FCE0
// ??_GCounterClass@@UAEPAXI@Z

int DAT_0089F768;
int DAT_0089F76C;
int DAT_0089F770;
int DAT_0089F774;

// FUNCTION: GAMEMD 0x0049FD30
void FUN_0049FD30(void)
{
	DAT_0089F768 = 0;
	DAT_0089F76C = 0;
	DAT_0089F770 = 0;
	DAT_0089F774 = 0;
}
