// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class TechnoTypeClass;
class XSurface;
class ConvertClass;
class Blitter;

class DropshipLoadoutClass
{
	enum {
		MAX_ENTRIES = 5
	};

	public:
		DropshipLoadoutClass(void);
		~DropshipLoadoutClass(void);

		bool Add(TechnoTypeClass * ttype);
		bool Remove(int index);
		void Clear(void);
		void Swizzle(void);
		TechnoTypeClass * Fetch(int index);

	private:
		friend void Dropship_Screen(void);
		static int Get_Allowable_Index(TechnoTypeClass * ttype);

	public:

		int CreationFrame;
		int Unused1;
		int Unused2;
		bool UnusedBool1;

		int EntryCount;

		TechnoTypeClass * Entries[MAX_ENTRIES];

		int TotalCost;
};

void Dropship_Screen(void);
