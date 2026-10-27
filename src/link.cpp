// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "link.h"

#include <math.h>


// FUNCTION: GAMEMD 0x00556550
// LinkClass::LinkClass
LinkClass::LinkClass(LinkClass const & link) :
	Next(0), Prev(0)
{
	Add((LinkClass &)link);
}

// FUNCTION: GAMEMD 0x005565A0
LinkClass::~LinkClass(void)
{
	Remove();
}

// FUNCTION: GAMEMD 0x005565F0
void LinkClass::Zap(void)
{
	Next = 0;
	Prev = 0;
}

// FUNCTION: GAMEMD 0x00556600
LinkClass & LinkClass::operator = (LinkClass const & link)
{
	if (&link == this) return(*this);

	Remove();
	Add((LinkClass &)link);

	return(*this);
}

// FUNCTION: GAMEMD 0x00556620
LinkClass * LinkClass::Get_Next(void) const
{
	return(Next);
}

// FUNCTION: GAMEMD 0x00556630
LinkClass * LinkClass::Get_Prev(void) const
{
	return(Prev);
}

// FUNCTION: GAMEMD 0x00556640
LinkClass & LinkClass::Head_Of_List(void)
{
	LinkClass * link = this;
	while (link->Prev) {
		link = link->Prev;
		if (link == this) break;
	}
	return(*link);
}

// FUNCTION: GAMEMD 0x00556670
LinkClass & LinkClass::Tail_Of_List(void)
{
	LinkClass * link = this;
	while (link->Next) {
		link = link->Next;
		if (link == this) break;
	}
	return(*link);
}

// FUNCTION: GAMEMD 0x005566A0
LinkClass & LinkClass::Add(LinkClass & list)
{
	Remove();
	LinkClass * ptr = list.Next;
	list.Next = this;
	Prev = &list;
	Next = ptr;
	if (ptr) {
		ptr->Prev = this;
	}
	return(Head_Of_List());
}

// FUNCTION: GAMEMD 0x005566D0
LinkClass & LinkClass::Add_Head(LinkClass & list)
{
	Remove();
	LinkClass * ptr = &list.Head_Of_List();
	ptr->Prev = this;
	Next = ptr;
	Prev = 0;
	return(*this);
}

// FUNCTION: GAMEMD 0x00556700
LinkClass & LinkClass::Add_Tail(LinkClass & list)
{
	Remove();
	LinkClass * ptr = &list.Tail_Of_List();
	ptr->Next = this;
	Prev = ptr;
	Next = 0;
	return(Head_Of_List());
}

// FUNCTION: GAMEMD 0x00556730
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

#include "rect.h"

// SYNTHETIC: GAMEMD 0x00556980 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x00556790 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005567C0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005567E0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00556800 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00556820 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00556840 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00556860 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005568A0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005568D0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00556900 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00556930 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00556940 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00556960 SYMBOL
// _$E38

