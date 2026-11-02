// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

struct MemoryItem
{
	MemoryItem * next;
	unsigned unused;
};

struct AudioMemoryPool
{
	MemoryItem * next;
	unsigned unused;
};

AudioMemoryPool * __fastcall MemoryPoolCreate(unsigned items, unsigned size);
void __fastcall MemoryPoolDestroy(AudioMemoryPool * pool);
void * __fastcall MemoryPoolGetItem(AudioMemoryPool * pool);
void __fastcall MemoryPoolReturnItem(AudioMemoryPool * pool, void * item);
int __fastcall MemoryPoolCount(AudioMemoryPool * pool);
