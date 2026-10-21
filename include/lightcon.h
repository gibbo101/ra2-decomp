// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "convert.h"
#include "noinit.h"

#include <cstddef>

class Surface;
class PaletteClass;

class LightConvertClass : public ConvertClass
{
		typedef ConvertClass BASECLASS;

	public:
		LightConvertClass(PaletteClass const & artpalette, PaletteClass const & screenpalette, Surface const & typicalsurface, int red_tint = 1000, int green_tint = 1000, int blue_tint = 1000, bool blitters_created = false, bool * tint_mask = NULL, int intensity_levels = NUM_INTENSITY_LEVELS);
		LightConvertClass(NoInitClass const & x) : BASECLASS(x) {}
		virtual ~LightConvertClass(void) override;

		virtual void Apply_Tint(int red_tint, int green_tint, int blue_tint, bool ion_light);

		void Add_Reference(void) {ReferenceCount++;}
		void Remove_Reference(void) {ReferenceCount--;}

	private:

		PaletteClass const * ArtPalette;

		PaletteClass const * ScreenPalette;

		bool * TintMask;
	public:

		int ReferenceCount;

		int NormalRedTint;
		int NormalGreenTint;
		int NormalBlueTint;

		int IonRedTint;
		int IonGreenTint;
		int IonBlueTint;

		bool UseIonLighting;
};
