// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "buffer.h"
#include <math.h>


// FUNCTION: GAMEMD 0x0043AEB0
BufferClass::BufferClass(void *buffer, int size)
{
	Size			= size;

	if (buffer) {
		Buffer			= (unsigned char *)buffer;
		Allocated		= false;
	} else {
		Buffer			= new unsigned char[Size];
		Allocated		= true;
	}
}

// FUNCTION: GAMEMD 0x0043AEF0
BufferClass::BufferClass(int size)
{
	Size				= size;
	Buffer			= new unsigned char[Size];
	Allocated		= true;
}

// FUNCTION: GAMEMD 0x0043AF10
BufferClass::BufferClass(void)
{
	Buffer		= 0;
	Size			= 0;
	Allocated	= false;
}

// FUNCTION: GAMEMD 0x0043AF20
BufferClass::~BufferClass(void)
{
	if (Allocated) {
		delete[] Buffer;
	}
}

#include "rect.h"

// SYNTHETIC: GAMEMD 0x0043B130 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x0043AF40 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0043AF70 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0043AF90 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0043AFB0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0043AFD0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0043AFF0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0043B010 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0043B050 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0043B080 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0043B0B0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0043B0E0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0043B0F0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0043B110 SYMBOL
// _$E38

