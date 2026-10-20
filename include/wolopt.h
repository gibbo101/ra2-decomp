// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

class WOLOptionsClass
{
	public:
		WOLOptionsClass(void) : Options(0), IsValid(false) { Load(); }
		bool Load(void);
		bool Is_Bit0_Clear(void);
		bool Is_Bit1_Clear(void);
		bool Is_Bit2_Clear(void);
		bool Save(unsigned long value);

		unsigned long Options;
		bool IsValid;
};
