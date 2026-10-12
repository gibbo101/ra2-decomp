// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "refcount.h"
#include "widewstring.h"
#include "wstring.h"


class PlayerProfile : public ReferenceCounted
{
	public:
		PlayerProfile(Wstring const & name, int a, char b, int c, int d, int e, int f, int g, int h, int i, int j, char k, WideWstring const & l, WideWstring const & m);
		virtual ~PlayerProfile(void);

		Wstring Name;
		int Field10;
		char Field14;
		int Field18, Field1C, Field20, Field24, Field28, Field2C, Field30, Field34;
		char Field38;
		WideWstring Wide3C;
		WideWstring Wide40;
};
