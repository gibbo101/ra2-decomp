// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "audlist.h"
#include <stddef.h>

// FUNCTION: GAMEMD 0x004072C0
void __fastcall ListInit(ListHead * head)
{
	head->prev = head->next = head;
	head->pri = (Priority)head;
}

// FUNCTION: GAMEMD 0x004072D0
void __fastcall ListNodeInit(ListNode * node)
{
	node->prev = node->next = node;
	node->pri = 0;
}

// FUNCTION: GAMEMD 0x004072E0
int __fastcall ListAddNodeSortAscending(ListHead * head, ListNode * new_node)
{
	ListNode * node;
	Priority pri;
	int index;

	index = 0;
	pri = new_node->pri;
	node = (ListNode *)head;
	while ((node = ListNodeNext(node))) {
		if (pri <= node->pri) {
			ListNodeInsert(node, new_node);
			return index;
		}
		index++;
	}

	ListNodeInsert(head, new_node);
	return index;
}

// FUNCTION: GAMEMD 0x00407330
void __fastcall ListAddNode(ListHead * head, ListNode * new_node)
{
	ListNode * node;
	Priority pri;

	pri = new_node->pri;
	node = (ListNode *)head;
	while ((node = ListNodeNext(node))) {
		if (node->pri <= pri) {
			ListNodeInsert(node, new_node);
			return;
		}
	}

	ListNodeInsert(head, new_node);
}

// FUNCTION: GAMEMD 0x00407380
void __fastcall ListMerge(ListHead * from, ListHead * to)
{
	ListNode * first;
	ListNode * last;
	ListNode * node;

	first = from->next;
	last = from->prev;

	if (first == (ListNode *)from) {
		return;
	}

	node = to->prev;
	node->next = first;
	first->prev = node;
	last->next = (ListNode *)to;
	to->prev = last;

	ListInit(from);
}

// FUNCTION: GAMEMD 0x004073B0
int __fastcall ListCountItems(ListHead * head)
{
	ListNode * node;
	int count = 0;

	node = head->next;

	while (node != (ListNode *)head) {
		count++;
		node = node->next;
	}

	return count;
}

// FUNCTION: GAMEMD 0x004073C0
ListNode * __fastcall ListFirstItem(ListHead * head)
{
	return ListNextItem((ListNode *)head);
}

// FUNCTION: GAMEMD 0x004073E0
ListNode * __fastcall ListNextItem(ListNode * node)
{
	if (!node) {
		return NULL;
	}
	return (ListNodeNext(node));
}

// FUNCTION: GAMEMD 0x00407400
ListNode * __fastcall ListGetItem(ListHead * head, int number)
{
	ListNode * node;

	node = head->next;

	while (node != (ListNode *)head) {
		if (number-- == 0) {
			return node;
		}
		node = node->next;
	}

	return NULL;
}

// FUNCTION: GAMEMD 0x00407420
void __fastcall ListNodeInsert(ListNode * node, ListNode * new_node)
{
	new_node->prev = node->prev;
	new_node->next = node;
	node->prev = new_node;
	new_node->prev->next = new_node;
}

// FUNCTION: GAMEMD 0x00407440
void __fastcall ListNodeAppend(ListNode * node, ListNode * new_node)
{
	new_node->prev = node;
	new_node->next = node->next;
	node->next = new_node;
	new_node->next->prev = new_node;
}

// FUNCTION: GAMEMD 0x00407450
void __fastcall ListNodeRemove(ListNode * node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->prev = node->next = node;
}

// FUNCTION: GAMEMD 0x00407470
ListNode * __fastcall ListNodeNext(ListNode * node)
{
	ListNode * next;

	next = node->next;

	if (next && ListNodeIsHead(next)) {
		return NULL;
	}

	return next;
}

// FUNCTION: GAMEMD 0x00407480
ListNode * __fastcall ListNodePrev(ListNode * node)
{
	ListNode * next;

	next = node->prev;

	if (ListNodeIsHead(next)) {
		return NULL;
	}

	return next;
}

// FUNCTION: GAMEMD 0x00407490
ListNode * __fastcall ListNodeLoopNext(ListNode * node)
{
	ListNode * next;

	next = node->next;

	if (ListNodeIsHead(next)) {
		next = next->next;
		if (ListNodeIsHead(next)) {
			return NULL;
		}
	}

	return next;
}

// FUNCTION: GAMEMD 0x004074B0
ListNode * __fastcall ListNodeLoopPrev(ListNode * node)
{
	ListNode * next;

	next = node->prev;

	if (ListNodeIsHead(next)) {
		next = next->prev;
		if (ListNodeIsHead(next)) {
			return NULL;
		}
	}

	return next;
}
