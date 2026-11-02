// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audcache.h"
#include <string.h>

// FUNCTION: GAMEMD 0x004019E0
AudioCache * __fastcall AudioCacheCreate(AudioIDXData * bag, int cacheSize, int maxItems, int frameSize)
{
	AudioCache * cache;
	int frameBytes;
	int pages;

	cache = new AudioCache;
	memset(cache, 0, sizeof(AudioCache));

	cache->bag = bag;
	cache->frameSize = frameSize;
	frameBytes = sizeof(AudioFrame) + frameSize;
	pages = cacheSize / frameBytes;
	cache->framePool = MemoryPoolCreate(pages, frameBytes);
	cache->itemPool = MemoryPoolCreate(maxItems, sizeof(AudioCacheItem));
	ListInit(&cache->items);

	if (!cache->framePool || !cache->itemPool) {
		AudioCacheDestroy(cache);
		cache = 0;
	}
	return cache;
}

// FUNCTION: GAMEMD 0x00401B00
void __fastcall AudioCacheDestroy(AudioCache * cache)
{
	AudioCacheItem * item;

	while ((item = (AudioCacheItem *)ListNodeNext(&cache->items))) {
		AudioCacheItemFree(item);
	}
	if (cache->framePool) {
		MemoryPoolDestroy(cache->framePool);
	}
	if (cache->itemPool) {
		MemoryPoolDestroy(cache->itemPool);
	}
	delete cache;
}

// FUNCTION: GAMEMD 0x00401BB0
AudioCacheItem * __fastcall AudioCacheGetItem(AudioCache * cache, int id)
{
	AudioCacheItem * item;
	AudioCacheItem * head;

	item = head = (AudioCacheItem *)&cache->items;
	while ((item = (AudioCacheItem *)item->nd.next) != head) {
		if (item->id == id && item->valid) {
			return item;
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00401BE0
void __fastcall AudioCacheInvalidate(AudioCache * cache)
{
	AudioCacheItem * item;
	AudioCacheItem * head;

	item = head = (AudioCacheItem *)&cache->items;
	while ((item = (AudioCacheItem *)item->nd.next) != head) {
		item->valid = 0;
	}
}

// FUNCTION: GAMEMD 0x00401C00
AudioCacheItem * __fastcall AudioCacheLoadItem(AudioCache * cache, int id)
{
	AudioCacheItem * item;
	int error;

	if ((item = AudioCacheGetItem(cache, id))) {
		ListNodeRemove(&item->nd);
		ListNodeAppend(&cache->items, &item->nd);
		return item;
	}

	if (cache->bag->OpenSample(id)) {
		item = (AudioCacheItem *)MemoryPoolGetItem(cache->itemPool);
		if (!item) {
			AudioCacheFreeOldestItem(cache);
			item = (AudioCacheItem *)MemoryPoolGetItem(cache->itemPool);
			if (!item) {
				cache->bag->ClearCurrentSample();
				return 0;
			}
		}

		item->id = id;
		item->cache = cache;
		ListNodeInit(&item->nd);
		LockInit(&item->lock);
		AudioSampleInit(&item->sample);
		AudioFormatInit(&item->format);
		item->sample.Format = &item->format;

		error = 0;
		{
			int bytesToTransfer;
			int bytes;
			int bytesTransfered;

			bytesToTransfer = cache->bag->CurrentSampleSize;
			while (bytesToTransfer) {
				AudioFrame * frame;
				void * data;

				if ((bytes = cache->frameSize) > bytesToTransfer) {
					bytes = bytesToTransfer;
				}

				while (!(frame = (AudioFrame *)MemoryPoolGetItem(cache->framePool))) {
					if (!AudioCacheFreeOldestItem(cache)) {
						break;
					}
				}
				if (!frame) {
					error = 1;
					break;
				}

				data = (void *)((unsigned)frame + sizeof(AudioFrame));
				AudioFrameInit(frame, data, bytes);
				AudioSampleAddFrame(&item->sample, frame);
				bytesTransfered = cache->bag->ReadSample(data, bytes);
				if (bytesTransfered != bytes) {
					error = 1;
					break;
				}
				bytesToTransfer -= bytesTransfered;
			}
		}

		if (!error) {
			cache->bag->GetSampleInformation(item->id, &item->format);
			ListNodeAppend(&cache->items, &item->nd);
			item->valid = 1;
			cache->bag->ClearCurrentSample();
			return item;
		}
		AudioCacheItemFree(item);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00401EF0
int __fastcall AudioCacheFreeOldestItem(AudioCache * cache)
{
	AudioCacheItem * item;

	item = (AudioCacheItem *)ListNodePrev(&cache->items);
	while (item) {
		if (!AudioCacheItemInUse(item)) {
			AudioCacheItemFree(item);
			return 1;
		}
		item = (AudioCacheItem *)ListNodePrev((ListNode *)item);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00401F80
void __fastcall AudioCacheItemLock(AudioCacheItem * item)
{
	LockAcquire(&item->lock);
}

// FUNCTION: GAMEMD 0x00401F90
void __fastcall AudioCacheItemUnlock(AudioCacheItem * item)
{
	LockRelease(&item->lock);
}

// FUNCTION: GAMEMD 0x00401FB0
int __fastcall AudioCacheItemInUse(AudioCacheItem * item)
{
	return item->lock.count;
}

// FUNCTION: GAMEMD 0x00401FC0
void __fastcall AudioCacheItemFree(AudioCacheItem * item)
{
	if (ListNodeInList(&item->nd)) {
		ListNodeRemove(&item->nd);
	}

	{
		AudioFrame * frame;

		while ((frame = (AudioFrame *)ListNodeNext(&item->sample.Frames))) {
			ListNodeRemove(&frame->nd);
			AudioFrameDeinit(frame);
			MemoryPoolReturnItem(item->cache->framePool, frame);
		}
	}

	AudioSampleDeinit(&item->sample);
	MemoryPoolReturnItem(item->cache->itemPool, item);
}

// FUNCTION: GAMEMD 0x00402020
int __fastcall AudioCacheItemId(AudioCacheItem * item)
{
	return item->id;
}

// FUNCTION: GAMEMD 0x00402030
AudioSample * __fastcall AudioCacheItemSample(AudioCacheItem * item)
{
	return &item->sample;
}
