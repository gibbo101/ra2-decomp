// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "link.h"

// FUNCTION: 0x005565A0
LinkClass::~LinkClass(void)
{
	Remove();
}

// FUNCTION: 0x005565F0
void LinkClass::Zap(void)
{
	Next = 0;
	Prev = 0;
}

// FUNCTION: 0x00556600
LinkClass & LinkClass::operator = (LinkClass const & link)
{
	if (&link == this) return(*this);

	Remove();
	Add((LinkClass &)link);

	return(*this);
}

// FUNCTION: 0x00556640
LinkClass & LinkClass::Head_Of_List(void)
{
	LinkClass * link = this;
	while (link->Prev) {
		link = link->Prev;
		if (link == this) break;
	}
	return(*link);
}

// FUNCTION: 0x00556670
LinkClass & LinkClass::Tail_Of_List(void)
{
	LinkClass * link = this;
	while (link->Next) {
		link = link->Next;
		if (link == this) break;
	}
	return(*link);
}

// FUNCTION: 0x00556730
LinkClass * LinkClass::Remove(void)
{
	LinkClass * head = &Head_Of_List();
	LinkClass * tail = &Tail_Of_List();

	if (Prev) {
		Prev->Next = Next;
	}
	if (Next) {
		Next->Prev = Prev;
	}
	Prev = 0;
	Next = 0;

	if (head==this) {
		if (tail==this) {
			return(0);
		}
		return(&tail->Head_Of_List());
	}
	return(head);
}
