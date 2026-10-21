// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "link.h"

#include <math.h>

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { C = 0; A = 128; B = 128; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
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

// FUNCTION: 0x00556860 $E20
int DAT_00ABCBA8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABCB18 - DAT_00ABCB40) * DAT_00ABCB38));

// FUNCTION: 0x005568A0 $E23
double DAT_00ABCB48 = FUN_004CADE0(DAT_00ABCBA8 * DAT_007E1740);

// FUNCTION: 0x005568D0 $E26
double DAT_00ABCB60 = FUN_004CADE0((DAT_00ABCBA8 * 2) / DAT_00ABCB38);

// FUNCTION: 0x00556900 $E29
int DAT_00ABCB9C = (int)(DAT_00ABCBA8 * 4 + DAT_007E1738);

// FUNCTION: 0x00556930 $E32
ShortPair DAT_00ABCB30;

// FUNCTION: 0x00556940 $E35
IntTriple DAT_00ABCB50;

// FUNCTION: 0x00556960 $E38
IntTriple2 DAT_00ABCB90;

// FUNCTION: 0x00556980 $E41
IntQuad DAT_00ABCB20;
