// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "wdtnet.h"

int WDT_Setup_Game(bool first_time);
rc_ptr<WDTState> * __cdecl WDT_Get_New_State(void);
void WDT_Start_New_Campaign(rc_ptr<WDTState> const & state);
rc_ptr<WDTTerritory> WDT_Get_Territory(int index);
bool __cdecl WDT_Is_Allowed(void);

/// Owning smart pointer; deletes the held object on destruction and reassignment.
template <class T>
class WDTPointer
{
public:
	WDTPointer(T *ptr = NULL) : Ptr(ptr) {}
	~WDTPointer(void)
	{
		if (Ptr != NULL) {
			delete Ptr;
			Ptr = NULL;
		}
	}

	const WDTPointer<T> & operator=(T * ptr)
	{
		if (Ptr != NULL) {
			delete Ptr;
		}
		Ptr = ptr;
		return(*this);
	}

	T *operator *(void) const
	{
		return(Ptr);
	}

	/*
	 * This is the object the pointer owns. Assigning over it or destroying the pointer
	 * deletes it, which is the whole point of the wrapper.
	 */
	T *Ptr;
};
