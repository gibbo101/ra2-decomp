// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "vector.h"
#include "win.h"

struct IStream;

class CounterClass : protected VectorClass<int>
{
public:
	CounterClass(void);
	virtual ~CounterClass(void) override {}

	int Increment(int index);
	int Decrement(int index);

	int Value(int index) const;

	void Clear(void);

	int Total(void) const;

	HRESULT Save(IStream * stream);
	HRESULT Load(IStream * stream);

	bool Reserve(int index);

	int operator[](int index) {return(Vector[index]);};

	enum {
		GROWTH_STEP = 10,
	};

private:
	int TotalCount;
};
