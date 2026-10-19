// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class CommandClass
{
	public:
		virtual ~CommandClass(void) {}

		virtual char const * Get_Unique_Name(void) const = 0;
		virtual char const * Get_Display_Name(void) const { return(Get_Description()); }
		virtual char const * Get_Category(void) const = 0;
		virtual char const * Get_Description(void) const = 0;
		virtual bool Unknown14(int key) const { return false; }
		virtual bool Unknown18(int key) const { return !(key & 0x800); }
		virtual bool Unknown1C(int key) const { return false; }
		virtual void Execute(int key) const = 0;
};
