// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "buffer.h"

// FUNCTION: 0x0043AEB0
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

// FUNCTION: 0x0043AEF0
BufferClass::BufferClass(int size)
{
	Size				= size;
	Buffer			= new unsigned char[Size];
	Allocated		= true;
}

// FUNCTION: 0x0043AF10
BufferClass::BufferClass(void)
{
	Buffer		= 0;
	Size			= 0;
	Allocated	= false;
}

// FUNCTION: 0x0043AF20
BufferClass::~BufferClass(void)
{
	if (Allocated) {
		delete[] Buffer;
	}
}
