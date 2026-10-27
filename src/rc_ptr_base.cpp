// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "rcptr.h"

// FUNCTION: GAMEMD 0x0065D430
rc_ptr_base::rc_ptr_base(void) : Ptr(0)
{
}

// FUNCTION: GAMEMD 0x0065D440
rc_ptr_base::rc_ptr_base(rc_object * ptr) : Ptr(ptr)
{
	if (ptr) {
		ptr->RefCount++;
	}
}

// FUNCTION: GAMEMD 0x0065D460
rc_ptr_base::~rc_ptr_base(void)
{
	rc_object * p = Ptr;
	if (p) {
		Ptr = 0;
		p->Release();
	}
}

// FUNCTION: GAMEMD 0x0065D480
void rc_ptr_base::FUN_0065D480(void * ptr)
{
	rc_object * p = (rc_object *)ptr;
	if (p != Ptr) {
		if (p) {
			p->RefCount++;
		}
		rc_object * old = Ptr;
		if (old) {
			Ptr = 0;
			old->Release();
		}
		Ptr = p;
	}
}

// FUNCTION: GAMEMD 0x0065D4B0
rc_ptr_base::rc_ptr_base(rc_ptr_base const & that) : Ptr(that.Ptr)
{
	if (Ptr) {
		Ptr->RefCount++;
	}
}

// FUNCTION: GAMEMD 0x0065D4D0
// ??_Grc_ptr_base@@UAEPAXI@Z
