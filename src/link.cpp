// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "link.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007ED0B8;
extern const double DAT_007ED0C0;

// FUNCTION: 0x00556550 LinkClass::LinkClass
LinkClass::LinkClass(LinkClass const & link) :
	Next(0), Prev(0)
{
	Add((LinkClass &)link);
}

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

// FUNCTION: 0x00556620
LinkClass * LinkClass::Get_Next(void) const
{
	return(Next);
}

// FUNCTION: 0x00556630
LinkClass * LinkClass::Get_Prev(void) const
{
	return(Prev);
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

// FUNCTION: 0x005566A0
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

// FUNCTION: 0x005566D0
LinkClass & LinkClass::Add_Head(LinkClass & list)
{
	Remove();
	LinkClass * ptr = &list.Head_Of_List();
	ptr->Prev = this;
	Next = ptr;
	Prev = 0;
	return(*this);
}

// FUNCTION: 0x00556700
LinkClass & LinkClass::Add_Tail(LinkClass & list)
{
	Remove();
	LinkClass * ptr = &list.Tail_Of_List();
	ptr->Next = this;
	Prev = ptr;
	Next = 0;
	return(Head_Of_List());
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

// FUNCTION: 0x00556790 $E2
double DAT_00ABCB38 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005567C0 $E5
double DAT_00ABCBA0 = DAT_00ABCB38 / DAT_007ED0B8;

// FUNCTION: 0x005567E0 $E8
double DAT_00ABCB70 = DAT_007E1718 / DAT_00ABCBA0;

// FUNCTION: 0x00556800 $E11
double DAT_00ABCB68 = DAT_007E1720 * DAT_007ED0C0;

// FUNCTION: 0x00556820 $E14
double DAT_00ABCB40 = DAT_007E1728 * DAT_007ED0C0;

// FUNCTION: 0x00556840 $E17
double DAT_00ABCB18 = DAT_007E1730 * DAT_007ED0C0;
