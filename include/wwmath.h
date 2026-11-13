// SPDX-License-Identifier: GPL-3.0-or-later
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once
#include "always.h"

class WWMath
{
public:
	static WWINLINE long Float_To_Long(float f);
};

#if defined(_MSC_VER) && defined(_M_IX86)
WWINLINE long WWMath::Float_To_Long(float f)
{
	long i;

	__asm {
		fld [f]
		fistp [i]
	}

	return i;
}
#else 
WWINLINE long WWMath::Float_To_Long(float f)
{
	return (long) f;
}
#endif
