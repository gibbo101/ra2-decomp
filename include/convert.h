// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "draw.hh"
#include "noinit.h"

class Blitter;
class RLEBlitter;
class PaletteClass;
class Surface;
template<class T> class DynamicVectorClass;

#define NUM_INTENSITY_LEVELS 63

class ConvertClass
{
		friend class IsometricTileTypeClass;
	public:
		ConvertClass(PaletteClass const & artpalette, PaletteClass const & screenpalette, Surface const & typicalsurface, int intensity_levels = 1, bool quick_init = false);
		ConvertClass(NoInitClass const &) {}
		virtual ~ConvertClass(void);

		int Convert_Pixel(int pixel) const {
			if (BBP == 1) return(((unsigned char const *)Translator)[pixel]);
			return(((unsigned short const *)Translator)[pixel]);
		}

		Blitter const * Blitter_From_Flags(ShapeFlags_Type flags) const;
		RLEBlitter const * RLEBlitter_From_Flags(ShapeFlags_Type flags) const;

		int Bytes_Per_Pixel(void) const {return(BBP);}

		void const * Get_Translate_Table(void) const {return(Translator);}

		void Set_Remap(unsigned char const * remap) {RemapTable = remap;}

		void Create_Blitters(void);
		void Destroy_Blitters(void);

		static void Reinitialize_Hicolor_Tables(int redleft, int redright, int greenleft, int greenright, int blueleft, int blueright);

	protected:

		int BBP;

		Blitter * PlainBlitter;
		Blitter * TransBlitter;
		Blitter * ShadowBlitter;
		Blitter * RemapBlitter;
		Blitter * Translucent1Blitter;
		Blitter * Translucent2Blitter;
		Blitter * Translucent3Blitter;

		Blitter * BlitPlainXlatZReadPtr;
		Blitter * BlitTransXlatZReadPtr;
		Blitter * BlitTransDarkenZReadPtr;
		Blitter * BlitTransZRemapXlatZReadPtr;
		Blitter * BlitTransLucent75ZReadPtr;
		Blitter * BlitTransLucent50ZReadPtr;
		Blitter * BlitTransLucent25ZReadPtr;
		Blitter * BlitTransLucent75ZReadWarpPtr;
		Blitter * BlitTransLucent50ZReadWarpPtr;
		Blitter * BlitTransLucent25ZReadWarpPtr;

		Blitter * BlitPlainXlatZReadWritePtr;
		Blitter * BlitTransXlatZReadWritePtr;
		Blitter * BlitTransDarkenZReadWritePtr;
		Blitter * BlitTransZRemapXlatZReadWritePtr;
		Blitter * BlitTransLucent75ZReadWritePtr;
		Blitter * BlitTransLucent50ZReadWritePtr;
		Blitter * BlitTransLucent25ZReadWritePtr;

		Blitter * BlitPlainXlatAlphaPtr;
		Blitter * BlitTransXlatAlphaPtr;
		Blitter * BlitTransZRemapXlatAlphaPtr;
		Blitter * BlitTransLucent75AlphaPtr;
		Blitter * BlitTransLucent50AlphaPtr;
		Blitter * BlitTransLucent25AlphaPtr;

		Blitter * BlitTransXlatWriteAlphaPtr;
		Blitter * BlitTransXlatMultWriteAlphaPtr;

		Blitter * BlitTranslucentWriteAlphaPtr;

		Blitter * BlitTranslucent50NonzeroAlphaPtr;
		Blitter * BlitTranslucent50ZeroAlphaPtr;

		Blitter * BlitPlainXlatAlphaZReadPtr;
		Blitter * BlitTransXlatAlphaZReadPtr;
		Blitter * BlitTransZRemapXlatAlphaZReadPtr;
		Blitter * BlitTransLucent75AlphaZReadPtr;
		Blitter * BlitTransLucent50AlphaZReadPtr;
		Blitter * BlitTransLucent25AlphaZReadPtr;
		Blitter * BlitTransLucent75AlphaZReadWarpPtr;
		Blitter * BlitTransLucent50AlphaZReadWarpPtr;
		Blitter * BlitTransLucent25AlphaZReadWarpPtr;

		Blitter * BlitPlainXlatAlphaZReadWritePtr;
		Blitter * BlitTransXlatAlphaZReadWritePtr;
		Blitter * BlitTransZRemapXlatAlphaZReadWritePtr;
		Blitter * BlitTransLucent75AlphaZReadWritePtr;
		Blitter * BlitTransLucent50AlphaZReadWritePtr;
		Blitter * BlitTransLucent25AlphaZReadWritePtr;

		RLEBlitter * RLETransBlitter;
		RLEBlitter * RLEShadowBlitter;
		RLEBlitter * RLERemapBlitter;
		RLEBlitter * RLETranslucent1Blitter;
		RLEBlitter * RLETranslucent2Blitter;
		RLEBlitter * RLETranslucent3Blitter;

		RLEBlitter * RLEBlitTransXlatZReadPtr;
		RLEBlitter * RLEBlitTransZRemapXlatZReadPtr;
		RLEBlitter * RLEBlitTransDarkenZReadPtr;
		RLEBlitter * RLEBlitTransLucent75ZReadPtr;
		RLEBlitter * RLEBlitTransLucent50ZReadPtr;
		RLEBlitter * RLEBlitTransLucent25ZReadPtr;
		RLEBlitter * RLEBlitTransLucent75ZReadWarpPtr;
		RLEBlitter * RLEBlitTransLucent50ZReadWarpPtr;
		RLEBlitter * RLEBlitTransLucent25ZReadWarpPtr;

		RLEBlitter * RLEBlitTransXlatZReadWritePtr;
		RLEBlitter * RLEBlitTransZRemapXlatZReadWritePtr;
		RLEBlitter * RLEBlitTransDarkenZReadWritePtr;
		RLEBlitter * RLEBlitTransLucent75ZReadWritePtr;
		RLEBlitter * RLEBlitTransLucent50ZReadWritePtr;
		RLEBlitter * RLEBlitTransLucent25ZReadWritePtr;

		RLEBlitter * RLEBlitTransXlatAlphaPtr;
		RLEBlitter * RLEBlitTransZRemapXlatAlphaPtr;
		RLEBlitter * RLEBlitTransLucent75AlphaPtr;
		RLEBlitter * RLEBlitTransLucent50AlphaPtr;
		RLEBlitter * RLEBlitTransLucent25AlphaPtr;

		RLEBlitter * RLEBlitTransXlatAlphaZReadPtr;
		RLEBlitter * RLEBlitTransZRemapXlatAlphaZReadPtr;
		RLEBlitter * RLEBlitTransLucent75AlphaZReadPtr;
		RLEBlitter * RLEBlitTransLucent50AlphaZReadPtr;
		RLEBlitter * RLEBlitTransLucent25AlphaZReadPtr;
		RLEBlitter * RLEBlitTransLucent75AlphaZReadWarpPtr;
		RLEBlitter * RLEBlitTransLucent50AlphaZReadWarpPtr;
		RLEBlitter * RLEBlitTransLucent25AlphaZReadWarpPtr;

		RLEBlitter * RLEBlitTransXlatAlphaZReadWritePtr;
		RLEBlitter * RLEBlitTransZRemapXlatAlphaZReadWritePtr;
		RLEBlitter * RLEBlitTransLucent75AlphaZReadWritePtr;
		RLEBlitter * RLEBlitTransLucent50AlphaZReadWritePtr;
		RLEBlitter * RLEBlitTransLucent25AlphaZReadWritePtr;

		int IntensityLevels;

		void * IntensityTranslator;

		void * Translator;

		unsigned char * ShadowTable;

		mutable unsigned char const * RemapTable;

		int HalfbrightMask;
		int QuarterbrightMask;

	public:

		static DynamicVectorClass<ConvertClass *> Drawers;
};
