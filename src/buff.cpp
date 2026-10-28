// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "buff.h"

// FUNCTION: GAMEMD 0x0043AD00
Buffer::Buffer(void * buffer, int size) :
	BufferPtr(buffer),
	Size(size),
	IsAllocated(false)
{
	if (buffer == NULL && size > 0) {
		BufferPtr = new char[size];
		IsAllocated = true;
	}
}

// FUNCTION: GAMEMD 0x0043AD40
Buffer::Buffer(char * buffer, int size) :
	BufferPtr(buffer),
	Size(size),
	IsAllocated(false)
{
	if (buffer == NULL && size > 0) {
		BufferPtr = new char[size];
		IsAllocated = true;
	}
}

// FUNCTION: GAMEMD 0x0043AD80
Buffer::Buffer(void const * buffer, int size) :
	BufferPtr((void *)buffer),
	Size(size),
	IsAllocated(false)
{
	if (buffer == NULL && size > 0) {
		BufferPtr = new char[size];
		IsAllocated = true;
	}
}

// FUNCTION: GAMEMD 0x0043ADC0
Buffer::Buffer(int size) :
	BufferPtr(NULL),
	Size(size),
	IsAllocated(false)
{
	if (size > 0) {
		BufferPtr = new char[size];
		IsAllocated = true;
	}
}

// FUNCTION: GAMEMD 0x0043ADF0
Buffer::Buffer(Buffer const & buffer) :
	IsAllocated(false)
{
	BufferPtr = buffer.BufferPtr;
	Size = buffer.Size;
}

// FUNCTION: GAMEMD 0x0043AE10
Buffer & Buffer::operator = (Buffer const & buffer)
{
	if (&buffer != this) {
		if (IsAllocated) {
			delete [] BufferPtr;
		}
		IsAllocated = false;
		BufferPtr = buffer.BufferPtr;
		Size = buffer.Size;
	}
	return(*this);
}

// FUNCTION: GAMEMD 0x0043AE50
Buffer::~Buffer(void)
{
	if (IsAllocated) {
		delete [] BufferPtr;
	}
	BufferPtr = NULL;
	Size = 0;
	IsAllocated = false;
}
