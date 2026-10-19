// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "language\language.h"
#include "win.h"

class WWMessageBox
{
		wchar_t const * Caption;

	public:
		WWMessageBox(wchar_t const * caption = L" ") {Caption = caption;}

		int Process(wchar_t const * msg, wchar_t const * b1txt, wchar_t const * b2txt, wchar_t const * b3txt, bool preserve = false);
};
