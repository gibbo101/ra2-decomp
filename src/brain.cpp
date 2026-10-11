// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "brain.h"
#include "crc.h"
#include "ftimer.h"
#include "globals.h"
#include "noinit.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x0043A500
HRESULT STDMETHODCALLTYPE NeuronClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_NeuronClass;
	return(S_OK);
}

// FUNCTION: 0x0043A5B0
HRESULT STDMETHODCALLTYPE NeuronClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0043A600
BrainClass::~BrainClass(void)
{
	Deinit();
}

// FUNCTION: 0x0043A670
void BrainClass::Deinit(void)
{
	int count = Neurons.Count();
	for (int i = 0; i < count; i++) {
		delete Neurons[i];
		Neurons[i] = NULL;
	}

	Neurons.Clear();
}

// FUNCTION: 0x0043A6B0
void BrainClass::Init(int min, int max)
{
	Deinit();
	MinCount = min;
	MaxCount = max;
}

// FUNCTION: 0x0043A770
HRESULT BrainClass::Save(IStream * stream, BOOL cleardirty)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	int count = Neurons.Count();

	HRESULT result = stream->Write(&count, sizeof(count), NULL);
	if (SUCCEEDED(result)) {
		for (int i = 0; i < count; i++) {
			result = Neurons[i]->Save(stream, cleardirty);
			if (FAILED(result)) {
				return(result);
			}
		}

		result = S_OK;
	}
	return(result);
}
