// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

typedef int Priority;

struct ListNode
{
	ListNode * next;
	ListNode * prev;
	Priority pri;
};

typedef ListNode ListHead;

#define ListNodeIsHead(n) ((Priority)(n) == (n)->pri)

void __fastcall ListInit(ListHead * head);
void __fastcall ListNodeInit(ListNode * node);
int __fastcall ListAddNodeSortAscending(ListHead * head, ListNode * new_node);
void __fastcall ListAddNode(ListHead * head, ListNode * new_node);
void __fastcall ListMerge(ListHead * from, ListHead * to);
int __fastcall ListCountItems(ListHead * head);
ListNode * __fastcall ListFirstItem(ListHead * head);
ListNode * __fastcall ListNextItem(ListNode * node);
ListNode * __fastcall ListGetItem(ListHead * head, int number);
void __fastcall ListNodeInsert(ListNode * node, ListNode * new_node);
void __fastcall ListNodeAppend(ListNode * node, ListNode * new_node);
void __fastcall ListNodeRemove(ListNode * node);
ListNode * __fastcall ListNodeNext(ListNode * node);
ListNode * __fastcall ListNodePrev(ListNode * node);
ListNode * __fastcall ListNodeLoopNext(ListNode * node);
ListNode * __fastcall ListNodeLoopPrev(ListNode * node);
