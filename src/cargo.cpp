// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cargo.h"
#include "foot.h"
#include "techtype.h"
#include "geometry.h"
#include <math.h>


// FUNCTION: GAMEMD 0x004733A0
void CargoClass::Attach(FootClass * object)
{
	if (object == NULL) return;

	object->Limbo();

	ObjectClass * o = NULL;
	if (object->Next && object->Next->Is_Foot()) {
		o = object->Next;
		while (o != NULL) {
			if (o->Next == NULL) break;
			if (!o->Next->Is_Foot()) {
				o = NULL;
				break;
			}
			o = o->Next;
			if (o == NULL) break;
		}
	}
	if (o != NULL) {
		o->Next = CargoHold;
	} else {
		object->Next = CargoHold;
	}

	CargoHold = object;
	Quantity = 0;
	object = CargoHold;
	while (object != NULL) {
		Quantity++;
		if (object->Next == (void*)NULL) break;
		if (!object->Next->Is_Foot()) break;
		object = (FootClass *)(ObjectClass *)object->Next;
	}
}

// FUNCTION: GAMEMD 0x00473430
FootClass * CargoClass::Detach_Object(void)
{
	FootClass * unit = Attached_Object();

	if (unit != NULL) {
		CargoHold = (FootClass *)(ObjectClass *)unit->Next;
		unit->Next = 0;
		Quantity--;
	}
	return unit;
}

// FUNCTION: GAMEMD 0x00473450
FootClass * CargoClass::Attached_Object(void) const
{
	if (Is_Something_Attached()) {
		return(CargoHold);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x00473460
int CargoClass::GetTotalSize(void) const
{
	int count = Quantity;
	FootClass * object = CargoHold;
	int total = 0;
	if (object != NULL) {
		FootClass * o = Is_Something_Attached() ? CargoHold : NULL;
		if (o != NULL) {
			int i = 0;
			while (i < count && o != NULL) {
				total = (int)((double)total + o->Techno_Type_Class()->Size);
				o = (FootClass *)(ObjectClass *)o->Next;
				i++;
			}
		}
	}
	return total;
}

// FUNCTION: GAMEMD 0x004734B0
void CargoClass::Detach(FootClass * object)
{
	if (object != NULL && CargoHold != NULL) {
		if (CargoHold == object) {
			CargoHold = (FootClass *)CargoHold->Next;
			Quantity--;
		} else {
			FootClass * o = CargoHold;
			if (o->Next != NULL) {
				while (o->Next != NULL && object != o->Next) {
					o = (FootClass *)o->Next;
					if (o->Next == NULL) return;
				}
				o->Next = o->Next->Next;
				Quantity--;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00473500
int CargoClass::IndexOf(ObjectClass const * object) const
{
	ObjectClass * o = CargoHold;
	int index = 0;
	while (o != NULL) {
		index++;
		if (o == object) break;
		ObjectClass * next = o->Next;
		if (next != NULL) {
			if (!(next->AbstractFlags & 4)) { o = NULL; break; }
			o = next;
		}
		if (o == NULL) break;
	}
	if (o == NULL) index = 0;
	return index;
}

// SYNTHETIC: GAMEMD 0x00473540 SYMBOL
// _$E30

// SYNTHETIC: GAMEMD 0x00473570 SYMBOL
// _$E33

// SYNTHETIC: GAMEMD 0x00473590 SYMBOL
// _$E36

// SYNTHETIC: GAMEMD 0x004735B0 SYMBOL
// _$E39

// SYNTHETIC: GAMEMD 0x004735D0 SYMBOL
// _$E42

// SYNTHETIC: GAMEMD 0x004735F0 SYMBOL
// _$E45
