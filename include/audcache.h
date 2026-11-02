// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audbag.h"
#include "audlock.h"
#include "audmem.h"

#define ListNodeInList(n) ((n)->prev != (n))

struct AudioCache;

struct AudioCacheItem
{
	ListNode nd;
	Lock lock;
	unsigned : 32;
	int id;
	int valid;
	AudioSample sample;
	AudioCache * cache;
	AudioFormat format;
	unsigned : 32;
};

struct AudioCache
{
	AudioIDXData * bag;
	AudioMemoryPool * framePool;
	AudioMemoryPool * itemPool;
	ListHead items;
	int frameSize;
	char Reserved[0xCC];
};

AudioCache * __fastcall AudioCacheCreate(AudioIDXData * bag, int cacheSize, int maxItems, int frameSize);
void __fastcall AudioCacheDestroy(AudioCache * cache);
AudioCacheItem * __fastcall AudioCacheGetItem(AudioCache * cache, int id);
void __fastcall AudioCacheInvalidate(AudioCache * cache);
AudioCacheItem * __fastcall AudioCacheLoadItem(AudioCache * cache, int id);
int __fastcall AudioCacheFreeOldestItem(AudioCache * cache);
void __fastcall AudioCacheItemLock(AudioCacheItem * item);
void __fastcall AudioCacheItemUnlock(AudioCacheItem * item);
int __fastcall AudioCacheItemInUse(AudioCacheItem * item);
void __fastcall AudioCacheItemFree(AudioCacheItem * item);
int __fastcall AudioCacheItemId(AudioCacheItem * item);
AudioSample * __fastcall AudioCacheItemSample(AudioCacheItem * item);
