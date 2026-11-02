// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "audmem.h"
#include <stddef.h>

// FUNCTION: GAMEMD 0x004074D0
AudioMemoryPool * __fastcall MemoryPoolCreate(unsigned items, unsigned size)
{
	unsigned poolsize;
	AudioMemoryPool * pool;
	MemoryItem * item;
	unsigned i;

	poolsize = items * (size + sizeof(MemoryItem)) + sizeof(AudioMemoryPool);

	if ((pool = (AudioMemoryPool *)operator new(poolsize))) {
		item = (MemoryItem *)((unsigned)pool + (unsigned)sizeof(AudioMemoryPool));

		pool->next = item;

		for (i = 0; i < items - 1; i++) {
			item->next = (MemoryItem *)((unsigned)item + (unsigned)(sizeof(MemoryItem) + size));
			item = item->next;
		}

		item->next = NULL;
	}

	return pool;
}

// FUNCTION: GAMEMD 0x00407510
void __fastcall MemoryPoolDestroy(AudioMemoryPool * pool)
{
	operator delete(pool);
}

// FUNCTION: GAMEMD 0x00407520
void * __fastcall MemoryPoolGetItem(AudioMemoryPool * pool)
{
	MemoryItem * item = NULL;

	if (!(item = pool->next)) {
		return NULL;
	}

	pool->next = item->next;

	return (void *)((unsigned)item + (unsigned)sizeof(MemoryItem));
}

// FUNCTION: GAMEMD 0x00407530
void __fastcall MemoryPoolReturnItem(AudioMemoryPool * pool, void * item)
{
	MemoryItem * mi = (MemoryItem *)((unsigned)item - (unsigned)sizeof(MemoryItem));

	mi->next = pool->next;
	pool->next = mi;
}

// FUNCTION: GAMEMD 0x00407540
int __fastcall MemoryPoolCount(AudioMemoryPool * pool)
{
	MemoryItem * item;
	int count = 0;

	item = pool->next;
	while (item) {
		count++;
		item = item->next;
	}
	return count;
}
