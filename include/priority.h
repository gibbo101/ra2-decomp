// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "assert.h"
#include "dbgprint.h"

#include <climits>
#include <objidl.h>

#define PARENT(index) (index >> 1)
#define LEFT_CHILD(index) ((index << 1))
#define RIGHT_CHILD(index) ((index << 1) + 1)
#define SWAP(left, right) \
{ \
	T * temp = Heap[left]; \
	Heap[left] = Heap[right]; \
	Heap[right] = temp; \
}

template<typename T>
class PriorityQueueClass
{
		friend class AStarClass;
	public:
		PriorityQueueClass(int size);
		~PriorityQueueClass(void);

		void Clear(void);

		bool Insert(T & node);
		T * Extract_Min(void);
		T * Replace_Root(T & node);
		bool Remove_Matching(T const & item);
		void Heapify(int index);

		int Count(void) const { return(ActiveCount); }

		bool Load(IStream * stream, T * nodes);
		bool Save(IStream * stream, T * nodes);

	private:

		int ActiveCount;

		int Size;

		T ** Heap;

		unsigned MaxNodePointer;
		unsigned MinNodePointer;
};

template<typename T>
PriorityQueueClass<T>::PriorityQueueClass(int size)
{
	MaxNodePointer = 0;
	MinNodePointer = UINT_MAX;
	ActiveCount = 0;
	Size = size;
	Heap = new T * [size + 1]();
	for (int index = 0; index <= Size; index++) {
		Heap[index] = NULL;
	}
}

template<typename T>
PriorityQueueClass<T>::~PriorityQueueClass(void)
{
	delete[] Heap;
}

template<typename T>
void PriorityQueueClass<T>::Clear(void)
{
	for (int index = 0; index <= ActiveCount; index++) {
		Heap[index] = NULL;
	}
	ActiveCount = 0;
}

template<typename T>
inline bool PriorityQueueClass<T>::Insert(T & node)
{
	unsigned index = ActiveCount + 1;
	unsigned parent_index = PARENT(index);
	float score = node.Score;

	if (index >= (unsigned)Size) {
		return(false);
	}

	while (index > 1) {
		if (Heap[parent_index]->Score <= score) {
			break;
		}
		Heap[index] = Heap[parent_index];
		index = parent_index;
		parent_index = PARENT(index);
	}

	Heap[index] = &node;
	ActiveCount++;

	if ((unsigned)&node > MaxNodePointer) {
		MaxNodePointer = (unsigned)&node;
	}

	if ((unsigned)&node < MinNodePointer) {
		MinNodePointer = (unsigned)&node;
	}

	return(true);
}

template <typename T>
inline T * PriorityQueueClass <T>::Extract_Min(void)
{
	if (ActiveCount == 0) {
		return(NULL);
	}

	T * min = Heap[1];
	Heap[1] = Heap[ActiveCount];
	Heap[ActiveCount] = NULL;
	ActiveCount--;

	Heapify(1);

	return(min);
}

template <typename T>
inline T * PriorityQueueClass<T>::Replace_Root(T & node)
{
	if (ActiveCount == 0) {
		return(&node);
	}

	T * old_root = Heap[1];

	if (node < *old_root) {
		return(&node);
	}

	Heap[1] = &node;

	Heapify(1);

	return(old_root);
}

template <typename T>
inline bool PriorityQueueClass <T>::Remove_Matching(T const & item)
{
	for (int index = 1; index <= ActiveCount; index++) {
		if (Heap[index]->Element == item.Element) {
			if (index == ActiveCount) {
				ActiveCount--;
			} else {
				T * last = Heap[ActiveCount];
				float last_score = last->Score;
				int parent = PARENT(index);
				ActiveCount--;
				if (index != 1 && Heap[parent]->Score >= last_score) {
					while ((unsigned)index > 1) {
						if (Heap[parent]->Score <= last_score) break;
						Heap[index] = Heap[parent];
						index = parent;
						parent = PARENT(index);
					}
					Heap[index] = last;
					for (int i = 1; i <= ActiveCount; i++) {
						if ((unsigned)Heap[i] < MinNodePointer || (unsigned)Heap[i] > MaxNodePointer) {
							volatile int div_zero = 0;
							div_zero = 1 / div_zero;
						}
					}
				} else {
					Heap[index] = last;
					Heapify(index);
					for (int i = 1; i <= ActiveCount; i++) {
						if ((unsigned)Heap[i] < MinNodePointer || (unsigned)Heap[i] > MaxNodePointer) {
							volatile int div_zero = 0;
							div_zero = 1 / div_zero;
						}
					}
				}
			}

			DebugString("Exiting Remove_Matching\n");
			return(true);
		}
	}

	DebugString("Exiting Remove_Matching\n");
	return(false);
}

template <typename T>
inline void PriorityQueueClass<T>::Heapify(int index)
{
	int smallest = index;

	int left = LEFT_CHILD(index);
	int right = RIGHT_CHILD(index);

	smallest = left <= Count() && *Heap[left] < *Heap[index] ? left : index;
	smallest = right <= Count() && *Heap[right] < *Heap[smallest] ? right : smallest;

	while (smallest != index) {

		SWAP(index, smallest)

		index = smallest;

		left = LEFT_CHILD(index);
		right = RIGHT_CHILD(index);

		smallest = left <= Count() && *Heap[left] < *Heap[index] ? left : index;
		smallest = right <= Count() && *Heap[right] < *Heap[smallest] ? right : smallest;
	}
}

template<typename T>
bool PriorityQueueClass<T>::Load(IStream * stream, T * nodes)
{
	if (FAILED(stream->Read(&ActiveCount, sizeof(ActiveCount), NULL))) {
		return(false);
	}

	if (FAILED(stream->Read(&Size, sizeof(Size), NULL))) {
		return(false);
	}

	for (int slot = 0; slot < Size; slot++) {
		int index;
		if (FAILED(stream->Read(&index, sizeof(index), NULL))) {
			return(false);
		}

		Heap[slot] = &nodes[index];
	}

	return(true);
}

template<typename T>
bool PriorityQueueClass<T>::Save(IStream * stream, T * nodes)
{
	if (FAILED(stream->Write(&ActiveCount, sizeof(ActiveCount), NULL))) {
		return(false);
	}

	if (FAILED(stream->Write(&Size, sizeof(Size), NULL))) {
		return(false);
	}

	for (int slot = 0; slot < Size; slot++) {
		int index = (int)(Heap[slot] - nodes);
		if (FAILED(stream->Write(&index, sizeof(index), NULL))) {
			return(false);
		}
	}

	return(true);
}

#undef PARENT
#undef LEFT_CHILD
#undef RIGHT_CHILD
#undef SWAP
