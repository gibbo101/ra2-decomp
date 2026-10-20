// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"

class LinkClass
{
	public:
		LinkClass(NoInitClass const &) {};
		LinkClass(void) : Next(0), Prev(0) {};
		virtual ~LinkClass(void);

		virtual LinkClass * Get_Next(void) const;
		virtual LinkClass * Get_Prev(void) const;
		virtual LinkClass & Add(LinkClass & object);
		virtual LinkClass & Add_Tail(LinkClass & object);
		virtual LinkClass & Add_Head(LinkClass & object);
		virtual LinkClass & Head_Of_List(void);
		virtual LinkClass & Tail_Of_List(void);
		virtual void Zap(void);
		virtual LinkClass * Remove(void);

		LinkClass & operator=(LinkClass const & link);
		LinkClass(LinkClass const & link);

	private:

		LinkClass * Next;
		LinkClass * Prev;
};
