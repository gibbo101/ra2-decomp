// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <stddef.h>
#include "dialog.hh"

class Surface;

class EgoClass {

	public:

		EgoClass (int x, int y, wchar_t *text, TextPrintType flags);
		~EgoClass (void);

		bool Scroll (int distance);
		void Render (bool fresh);
		void Wipe (Surface *background);

		wchar_t			*Text;
		int				XPos;
		int				YPos;
		TextPrintType 	Flags;
};

void Show_Who_Was_Responsible(void);
