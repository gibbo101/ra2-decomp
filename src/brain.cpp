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
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0043A160 SYMBOL
// _$E3
// SYNTHETIC: GAMEMD 0x0043A190 SYMBOL
// _$E6
// SYNTHETIC: GAMEMD 0x0043A1B0 SYMBOL
// _$E9
// SYNTHETIC: GAMEMD 0x0043A1D0 SYMBOL
// _$E12
// SYNTHETIC: GAMEMD 0x0043A1F0 SYMBOL
// _$E15
// SYNTHETIC: GAMEMD 0x0043A210 SYMBOL
// _$E18
// SYNTHETIC: GAMEMD 0x0043A230 SYMBOL
// _$E21
// SYNTHETIC: GAMEMD 0x0043A270 SYMBOL
// _$E24
// SYNTHETIC: GAMEMD 0x0043A2A0 SYMBOL
// _$E27
// SYNTHETIC: GAMEMD 0x0043A2D0 SYMBOL
// _$E30
// SYNTHETIC: GAMEMD 0x0043A300 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x0043A310 SYMBOL
// _$E36

// SYNTHETIC: GAMEMD 0x0043A330 SYMBOL
// _$E39

// FUNCTION: GAMEMD 0x0043A350
NeuronClass::NeuronClass(void):
	Pointer1(NULL),
	Pointer2(NULL),
	MyBrain(NULL),
	CreationFrame(Frame)
{
	TeamPtrTracker.Add(this);
	AbstractTypePtrTracker.Add(this);
}

// FUNCTION: GAMEMD 0x0043A440
NeuronClass::~NeuronClass(void)
{
	AbstractTypePtrTracker.Delete(this);
	TeamPtrTracker.Delete(this);
}

// FUNCTION: GAMEMD 0x0043A500
HRESULT STDMETHODCALLTYPE NeuronClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_NeuronClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0043A540
HRESULT STDMETHODCALLTYPE NeuronClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) NeuronClass(NoInitClass());

		Swizzle_Pointer(&Pointer2);
		Swizzle_Pointer(&Pointer1);
		Swizzle_Pointer(&MyBrain);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0043A5B0
HRESULT STDMETHODCALLTYPE NeuronClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0043A5D0
void NeuronClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc((int)(Frame - CreationFrame));
}

// FUNCTION: GAMEMD 0x0043A600
BrainClass::~BrainClass(void)
{
	Deinit();
}

// FUNCTION: GAMEMD 0x0043A670
void BrainClass::Deinit(void)
{
	int count = Neurons.Count();
	for (int i = 0; i < count; i++) {
		delete Neurons[i];
		Neurons[i] = NULL;
	}

	Neurons.Clear();
}

// FUNCTION: GAMEMD 0x0043A6B0
void BrainClass::Init(int min, int max)
{
	Deinit();
	MinCount = min;
	MaxCount = max;
}

// FUNCTION: GAMEMD 0x0043A700
bool BrainClass::Add_Neuron(NeuronClass *neuron)
{
	if (Neurons.Count() < MaxCount) {
		Neurons.Add(neuron);
		neuron->MyBrain = this;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0043A770
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

// FUNCTION: GAMEMD 0x0043A7E0
HRESULT BrainClass::Load(IStream * stream)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	int count = 0;

	HRESULT result = stream->Read(&count, sizeof(count), NULL);
	if (SUCCEEDED(result)) {
		for (int i = 0; i < count; i++) {
			NeuronClass * neuron = new NeuronClass;
			if (neuron == NULL) {
				return(E_OUTOFMEMORY);
			}

			result = neuron->Load(stream);
			if (FAILED(result)) {
				return(result);
			}

			Add_Neuron(neuron);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0043A9A0
// NeuronClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x0043A9B0
// NeuronClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x0043A9C0
// ??_GNeuronClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0043AA90
// ??_GBrainClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0043AB20
// ?GetItem@?$VectorClass@PAVNeuronClass@@@@UBEPAVNeuronClass@@H@Z

// TEMPLATE: GAMEMD 0x0043AB30
// ??8?$VectorClass@PAVNeuronClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0043AB70
// ?Resize@?$VectorClass@PAVNeuronClass@@@@UAE_NHPBQAVNeuronClass@@@Z

// TEMPLATE: GAMEMD 0x0043AC20
// ?Clear@?$VectorClass@PAVNeuronClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0043AC50
// ?ID@?$VectorClass@PAVNeuronClass@@@@UAEHABQAVNeuronClass@@@Z

// TEMPLATE: GAMEMD 0x0043AC90
// ?ID@?$VectorClass@PAVNeuronClass@@@@UAEHPBQAVNeuronClass@@@Z

// TEMPLATE: GAMEMD 0x0043ACB0
// ??_G?$VectorClass@PAVNeuronClass@@@@UAEPAXI@Z

template class VectorClass<NeuronClass *>;
