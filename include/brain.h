// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "noinit.h"
#include "vector.h"

class BrainClass;

class NeuronClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

		friend class BrainClass;

	public:
		NeuronClass(void);
		NeuronClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~NeuronClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override { return(RTTI_NEURON); }
		virtual int Fetch_Object_Size(void) const override { return(sizeof(NeuronClass)); }

		virtual void Compute_CRC(CRCEngine &) const override;

	private:

		void * Pointer1;
		void * Pointer2;

		BrainClass * MyBrain;

		int CreationFrame;

		int Unk1;
};

class BrainClass
{
	public:
		virtual ~BrainClass(void);

		void Deinit(void);
		void Init(int min, int max);
		bool Add_Neuron(NeuronClass *neuron);

		HRESULT Load(IStream * stream);
		HRESULT Save(IStream * stream, BOOL cleardirty);

	private:

		DynamicVectorClass<NeuronClass *> Neurons;

		int MinCount;
		int MaxCount;

};
