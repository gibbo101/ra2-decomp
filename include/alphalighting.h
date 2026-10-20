// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

template<class T> class DynamicVectorClass;

class AlphaLightingRemapClass;

class AlphaLightingRemapInitClass
{
public:
	AlphaLightingRemapInitClass(void);
	~AlphaLightingRemapInitClass(void);
	AlphaLightingRemapClass *Init(int count);
	void Deinit(AlphaLightingRemapClass *ptr);
};

class AlphaLightingRemapClass
{
public:
	AlphaLightingRemapClass(int count);
	~AlphaLightingRemapClass(void) {}

	unsigned short *Get_Table(int level)
	{
		int shade = level > 0 ? level : 0;
		shade = (261 * shade) >> 11;
		if (shade > 254) {
			shade = 254;
		}
		return(Buffer[shade]);
	}

	enum {
		BUFFER_SIZE = 256 * 256,
	};

	unsigned short Buffer[256][256];

	int Count;

	int RefCount;

	static DynamicVectorClass<AlphaLightingRemapClass *> AlphaLightingRemaps;
};
