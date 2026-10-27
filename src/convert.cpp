// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "convert.h"

#include "blitblit.h"
#include "dsurface.h"
#include "hsv.h"
#include "rlerle.h"
#include "vector.h"

DynamicVectorClass<ConvertClass *> ConvertClass::Drawers;

// SYNTHETIC: GAMEMD 0x0048E6A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0048E6E0 SYMBOL
// _$E26

// FUNCTION: GAMEMD 0x0048E740
ConvertClass::ConvertClass(PaletteClass const & artpalette, PaletteClass const & screenpalette, Surface const & surface, int intensity_levels, bool quick_init) :
	BBP(surface.Bytes_Per_Pixel()),
	PlainBlitter(NULL),
	TransBlitter(NULL),
	ShadowBlitter(NULL),
	RemapBlitter(NULL),
	Translucent1Blitter(NULL),
	Translucent2Blitter(NULL),
	Translucent3Blitter(NULL),
	BlitPlainXlatZReadPtr(NULL),
	BlitTransXlatZReadPtr(NULL),
	BlitTransDarkenZReadPtr(NULL),
	BlitTransZRemapXlatZReadPtr(NULL),
	BlitTransLucent75ZReadPtr(NULL),
	BlitTransLucent50ZReadPtr(NULL),
	BlitTransLucent25ZReadPtr(NULL),
	BlitTransLucent75ZReadWarpPtr(NULL),
	BlitTransLucent50ZReadWarpPtr(NULL),
	BlitTransLucent25ZReadWarpPtr(NULL),
	BlitPlainXlatZReadWritePtr(NULL),
	BlitTransXlatZReadWritePtr(NULL),
	BlitTransDarkenZReadWritePtr(NULL),
	BlitTransZRemapXlatZReadWritePtr(NULL),
	BlitTransLucent75ZReadWritePtr(NULL),
	BlitTransLucent50ZReadWritePtr(NULL),
	BlitTransLucent25ZReadWritePtr(NULL),
	BlitPlainXlatAlphaPtr(NULL),
	BlitTransXlatAlphaPtr(NULL),
	BlitTransZRemapXlatAlphaPtr(NULL),
	BlitTransLucent75AlphaPtr(NULL),
	BlitTransLucent50AlphaPtr(NULL),
	BlitTransLucent25AlphaPtr(NULL),
	BlitTransXlatWriteAlphaPtr(NULL),
	BlitTransXlatMultWriteAlphaPtr(NULL),
	BlitTranslucentWriteAlphaPtr(NULL),
	BlitTranslucent50NonzeroAlphaPtr(NULL),
	BlitTranslucent50ZeroAlphaPtr(NULL),
	BlitPlainXlatAlphaZReadPtr(NULL),
	BlitTransXlatAlphaZReadPtr(NULL),
	BlitTransZRemapXlatAlphaZReadPtr(NULL),
	BlitTransLucent75AlphaZReadPtr(NULL),
	BlitTransLucent50AlphaZReadPtr(NULL),
	BlitTransLucent25AlphaZReadPtr(NULL),
	BlitTransLucent75AlphaZReadWarpPtr(NULL),
	BlitTransLucent50AlphaZReadWarpPtr(NULL),
	BlitTransLucent25AlphaZReadWarpPtr(NULL),
	BlitPlainXlatAlphaZReadWritePtr(NULL),
	BlitTransXlatAlphaZReadWritePtr(NULL),
	BlitTransZRemapXlatAlphaZReadWritePtr(NULL),
	BlitTransLucent75AlphaZReadWritePtr(NULL),
	BlitTransLucent50AlphaZReadWritePtr(NULL),
	BlitTransLucent25AlphaZReadWritePtr(NULL),
	RLETransBlitter(NULL),
	RLEShadowBlitter(NULL),
	RLERemapBlitter(NULL),
	RLETranslucent1Blitter(NULL),
	RLETranslucent2Blitter(NULL),
	RLETranslucent3Blitter(NULL),
	RLEBlitTransXlatZReadPtr(NULL),
	RLEBlitTransZRemapXlatZReadPtr(NULL),
	RLEBlitTransDarkenZReadPtr(NULL),
	RLEBlitTransLucent75ZReadPtr(NULL),
	RLEBlitTransLucent50ZReadPtr(NULL),
	RLEBlitTransLucent25ZReadPtr(NULL),
	RLEBlitTransLucent75ZReadWarpPtr(NULL),
	RLEBlitTransLucent50ZReadWarpPtr(NULL),
	RLEBlitTransLucent25ZReadWarpPtr(NULL),
	RLEBlitTransXlatZReadWritePtr(NULL),
	RLEBlitTransZRemapXlatZReadWritePtr(NULL),
	RLEBlitTransDarkenZReadWritePtr(NULL),
	RLEBlitTransLucent75ZReadWritePtr(NULL),
	RLEBlitTransLucent50ZReadWritePtr(NULL),
	RLEBlitTransLucent25ZReadWritePtr(NULL),
	RLEBlitTransXlatAlphaPtr(NULL),
	RLEBlitTransZRemapXlatAlphaPtr(NULL),
	RLEBlitTransLucent75AlphaPtr(NULL),
	RLEBlitTransLucent50AlphaPtr(NULL),
	RLEBlitTransLucent25AlphaPtr(NULL),
	RLEBlitTransXlatAlphaZReadPtr(NULL),
	RLEBlitTransZRemapXlatAlphaZReadPtr(NULL),
	RLEBlitTransLucent75AlphaZReadPtr(NULL),
	RLEBlitTransLucent50AlphaZReadPtr(NULL),
	RLEBlitTransLucent25AlphaZReadPtr(NULL),
	RLEBlitTransLucent75AlphaZReadWarpPtr(NULL),
	RLEBlitTransLucent50AlphaZReadWarpPtr(NULL),
	RLEBlitTransLucent25AlphaZReadWarpPtr(NULL),
	RLEBlitTransXlatAlphaZReadWritePtr(NULL),
	RLEBlitTransZRemapXlatAlphaZReadWritePtr(NULL),
	RLEBlitTransLucent75AlphaZReadWritePtr(NULL),
	RLEBlitTransLucent50AlphaZReadWritePtr(NULL),
	RLEBlitTransLucent25AlphaZReadWritePtr(NULL),
	IntensityLevels(intensity_levels),
	IntensityTranslator(NULL),
	Translator(NULL),
	ShadowTable(NULL),
	RemapTable(NULL)
{
	if (IntensityLevels < 1) {
		IntensityLevels = 1;
	}

	if (BBP == 1) {
		ShadowTable = new unsigned char [256];
		ShadowTable[0] = 0;
		for (int shadow = 1; shadow < 256; shadow++) {
			HSVClass hsv = artpalette[shadow];
			hsv.Set_Value((unsigned char)(hsv.Get_Value() / 2));
			ShadowTable[shadow] = (unsigned char)artpalette.Closest_Color(hsv);
		}

		IntensityTranslator = new unsigned char [256 * IntensityLevels];
		unsigned char * trans = (unsigned char *)IntensityTranslator;
		Translator = &trans[256 * ((IntensityLevels - 1) >> 1)];

		if (IntensityLevels == 1) {
			trans[0] = 0;
			trans++;
			for (int index = 1; index < 256; index++) {
				trans[0] = (unsigned char)screenpalette.Closest_Color(artpalette[index]);
				trans++;
			}
		} else {
			for (int t = 0; t <= IntensityLevels - 1; t++) {
				trans[0] = 0;
				trans++;
				for (int index = 1; index < 256; index++) {
					HSVClass hsv = artpalette[index];
					hsv.Set_Value((unsigned char)((t * hsv.Get_Value() * 2) / (IntensityLevels - 1)));
					trans[0] = (unsigned char)screenpalette.Closest_Color(hsv);
					trans++;
				}
			}
		}
	} else {
		IntensityTranslator = new unsigned short [256 * IntensityLevels];
		unsigned short * trans = (unsigned short *)IntensityTranslator;
		Translator = &trans[256 * ((IntensityLevels - 1) >> 1)];

		if (!quick_init) {
			((DSurface &)surface).Build_Remap_Table((unsigned short *)trans, IntensityLevels, artpalette);
		}

		HalfbrightMask = ((DSurface &)surface).Get_Halfbright_Mask();
		QuarterbrightMask = ((DSurface &)surface).Get_Quarterbright_Mask();
	}

	if (!quick_init) {
		Create_Blitters();
	}

	Drawers.Add(this);
}

// FUNCTION: GAMEMD 0x00490400
// ConvertClass::~ConvertClass
// SYNTHETIC: GAMEMD 0x00491210
// ??_GConvertClass@@UAEPAXI@Z
ConvertClass::~ConvertClass(void)
{
	Destroy_Blitters();

	delete [] IntensityTranslator;
	IntensityTranslator = NULL;

	delete [] ShadowTable;
	ShadowTable = NULL;

	Drawers.Delete(this);
}

// FUNCTION: GAMEMD 0x00490490
void ConvertClass::Destroy_Blitters(void)
{
	delete PlainBlitter;
	PlainBlitter = NULL;

	delete TransBlitter;
	TransBlitter = NULL;

	delete ShadowBlitter;
	ShadowBlitter = NULL;

	delete RemapBlitter;
	RemapBlitter = NULL;

	delete Translucent1Blitter;
	Translucent1Blitter = NULL;

	delete Translucent2Blitter;
	Translucent2Blitter = NULL;

	delete Translucent3Blitter;
	Translucent3Blitter = NULL;

	delete BlitPlainXlatZReadPtr;
	BlitPlainXlatZReadPtr = NULL;

	delete BlitTransXlatZReadPtr;
	BlitTransXlatZReadPtr = NULL;

	delete BlitTransDarkenZReadPtr;
	BlitTransDarkenZReadPtr = NULL;

	delete BlitTransZRemapXlatZReadPtr;
	BlitTransZRemapXlatZReadPtr = NULL;

	delete BlitTransLucent75ZReadPtr;
	BlitTransLucent75ZReadPtr = NULL;

	delete BlitTransLucent50ZReadPtr;
	BlitTransLucent50ZReadPtr = NULL;

	delete BlitTransLucent25ZReadPtr;
	BlitTransLucent25ZReadPtr = NULL;

	delete BlitTransLucent75ZReadWarpPtr;
	BlitTransLucent75ZReadWarpPtr = NULL;

	delete BlitTransLucent50ZReadWarpPtr;
	BlitTransLucent50ZReadWarpPtr = NULL;

	delete BlitTransLucent25ZReadWarpPtr;
	BlitTransLucent25ZReadWarpPtr = NULL;

	delete BlitPlainXlatZReadWritePtr;
	BlitPlainXlatZReadWritePtr = NULL;

	delete BlitTransXlatZReadWritePtr;
	BlitTransXlatZReadWritePtr = NULL;

	delete BlitTransDarkenZReadWritePtr;
	BlitTransDarkenZReadWritePtr = NULL;

	delete BlitTransZRemapXlatZReadWritePtr;
	BlitTransZRemapXlatZReadWritePtr = NULL;

	delete BlitTransLucent75ZReadWritePtr;
	BlitTransLucent75ZReadWritePtr = NULL;

	delete BlitTransLucent50ZReadWritePtr;
	BlitTransLucent50ZReadWritePtr = NULL;

	delete BlitTransLucent25ZReadWritePtr;
	BlitTransLucent25ZReadWritePtr = NULL;

	delete BlitPlainXlatAlphaPtr;
	BlitPlainXlatAlphaPtr = NULL;

	delete BlitTransXlatAlphaPtr;
	BlitTransXlatAlphaPtr = NULL;

	delete BlitTransZRemapXlatAlphaPtr;
	BlitTransZRemapXlatAlphaPtr = NULL;

	delete BlitTransLucent75AlphaPtr;
	BlitTransLucent75AlphaPtr = NULL;

	delete BlitTransLucent50AlphaPtr;
	BlitTransLucent50AlphaPtr = NULL;

	delete BlitTransLucent25AlphaPtr;
	BlitTransLucent25AlphaPtr = NULL;

	delete BlitTranslucent50NonzeroAlphaPtr;
	BlitTranslucent50NonzeroAlphaPtr = NULL;

	delete BlitTranslucent50ZeroAlphaPtr;
	BlitTranslucent50ZeroAlphaPtr = NULL;

	delete BlitTransXlatWriteAlphaPtr;
	BlitTransXlatWriteAlphaPtr = NULL;

	delete BlitTransXlatMultWriteAlphaPtr;
	BlitTransXlatMultWriteAlphaPtr = NULL;

	delete BlitTranslucentWriteAlphaPtr;
	BlitTranslucentWriteAlphaPtr = NULL;

	delete BlitPlainXlatAlphaZReadPtr;
	BlitPlainXlatAlphaZReadPtr = NULL;

	delete BlitTransXlatAlphaZReadPtr;
	BlitTransXlatAlphaZReadPtr = NULL;

	delete BlitTransZRemapXlatAlphaZReadPtr;
	BlitTransZRemapXlatAlphaZReadPtr = NULL;

	delete BlitTransLucent75AlphaZReadPtr;
	BlitTransLucent75AlphaZReadPtr = NULL;

	delete BlitTransLucent50AlphaZReadPtr;
	BlitTransLucent50AlphaZReadPtr = NULL;

	delete BlitTransLucent25AlphaZReadPtr;
	BlitTransLucent25AlphaZReadPtr = NULL;

	delete BlitTransLucent75AlphaZReadWarpPtr;
	BlitTransLucent75AlphaZReadWarpPtr = NULL;

	delete BlitTransLucent50AlphaZReadWarpPtr;
	BlitTransLucent50AlphaZReadWarpPtr = NULL;

	delete BlitTransLucent25AlphaZReadWarpPtr;
	BlitTransLucent25AlphaZReadWarpPtr = NULL;

	delete BlitPlainXlatAlphaZReadWritePtr;
	BlitPlainXlatAlphaZReadWritePtr = NULL;

	delete BlitTransXlatAlphaZReadWritePtr;
	BlitTransXlatAlphaZReadWritePtr = NULL;

	delete BlitTransZRemapXlatAlphaZReadWritePtr;
	BlitTransZRemapXlatAlphaZReadWritePtr = NULL;

	delete BlitTransLucent75AlphaZReadWritePtr;
	BlitTransLucent75AlphaZReadWritePtr = NULL;

	delete BlitTransLucent50AlphaZReadWritePtr;
	BlitTransLucent50AlphaZReadWritePtr = NULL;

	delete BlitTransLucent25AlphaZReadWritePtr;
	BlitTransLucent25AlphaZReadWritePtr = NULL;

	delete RLETransBlitter;
	RLETransBlitter = NULL;

	delete RLEShadowBlitter;
	RLEShadowBlitter = NULL;

	delete RLERemapBlitter;
	RLERemapBlitter = NULL;

	delete RLETranslucent1Blitter;
	RLETranslucent1Blitter = NULL;

	delete RLETranslucent2Blitter;
	RLETranslucent2Blitter = NULL;

	delete RLETranslucent3Blitter;
	RLETranslucent3Blitter = NULL;

	delete RLEBlitTransXlatZReadPtr;
	RLEBlitTransXlatZReadPtr = NULL;

	delete RLEBlitTransZRemapXlatZReadPtr;
	RLEBlitTransZRemapXlatZReadPtr = NULL;

	delete RLEBlitTransDarkenZReadPtr;
	RLEBlitTransDarkenZReadPtr = NULL;

	delete RLEBlitTransLucent75ZReadPtr;
	RLEBlitTransLucent75ZReadPtr = NULL;

	delete RLEBlitTransLucent50ZReadPtr;
	RLEBlitTransLucent50ZReadPtr = NULL;

	delete RLEBlitTransLucent25ZReadPtr;
	RLEBlitTransLucent25ZReadPtr = NULL;

	delete RLEBlitTransLucent75ZReadWarpPtr;
	RLEBlitTransLucent75ZReadWarpPtr = NULL;

	delete RLEBlitTransLucent50ZReadWarpPtr;
	RLEBlitTransLucent50ZReadWarpPtr = NULL;

	delete RLEBlitTransLucent25ZReadWarpPtr;
	RLEBlitTransLucent25ZReadWarpPtr = NULL;

	delete RLEBlitTransXlatZReadWritePtr;
	RLEBlitTransXlatZReadWritePtr = NULL;

	delete RLEBlitTransZRemapXlatZReadWritePtr;
	RLEBlitTransZRemapXlatZReadWritePtr = NULL;

	delete RLEBlitTransDarkenZReadWritePtr;
	RLEBlitTransDarkenZReadWritePtr = NULL;

	delete RLEBlitTransLucent75ZReadWritePtr;
	RLEBlitTransLucent75ZReadWritePtr = NULL;

	delete RLEBlitTransLucent50ZReadWritePtr;
	RLEBlitTransLucent50ZReadWritePtr = NULL;

	delete RLEBlitTransLucent25ZReadWritePtr;
	RLEBlitTransLucent25ZReadWritePtr = NULL;

	delete RLEBlitTransXlatAlphaPtr;
	RLEBlitTransXlatAlphaPtr = NULL;

	delete RLEBlitTransZRemapXlatAlphaPtr;
	RLEBlitTransZRemapXlatAlphaPtr = NULL;

	delete RLEBlitTransLucent75AlphaPtr;
	RLEBlitTransLucent75AlphaPtr = NULL;

	delete RLEBlitTransLucent50AlphaPtr;
	RLEBlitTransLucent50AlphaPtr = NULL;

	delete RLEBlitTransLucent25AlphaPtr;
	RLEBlitTransLucent25AlphaPtr = NULL;

	delete RLEBlitTransXlatAlphaZReadPtr;
	RLEBlitTransXlatAlphaZReadPtr = NULL;

	delete RLEBlitTransZRemapXlatAlphaZReadPtr;
	RLEBlitTransZRemapXlatAlphaZReadPtr = NULL;

	delete RLEBlitTransLucent75AlphaZReadPtr;
	RLEBlitTransLucent75AlphaZReadPtr = NULL;

	delete RLEBlitTransLucent50AlphaZReadPtr;
	RLEBlitTransLucent50AlphaZReadPtr = NULL;

	delete RLEBlitTransLucent25AlphaZReadPtr;
	RLEBlitTransLucent25AlphaZReadPtr = NULL;

	delete RLEBlitTransLucent75AlphaZReadWarpPtr;
	RLEBlitTransLucent75AlphaZReadWarpPtr = NULL;

	delete RLEBlitTransLucent50AlphaZReadWarpPtr;
	RLEBlitTransLucent50AlphaZReadWarpPtr = NULL;

	delete RLEBlitTransLucent25AlphaZReadWarpPtr;
	RLEBlitTransLucent25AlphaZReadWarpPtr = NULL;

	delete RLEBlitTransXlatAlphaZReadWritePtr;
	RLEBlitTransXlatAlphaZReadWritePtr = NULL;

	delete RLEBlitTransZRemapXlatAlphaZReadWritePtr;
	RLEBlitTransZRemapXlatAlphaZReadWritePtr = NULL;

	delete RLEBlitTransLucent75AlphaZReadWritePtr;
	RLEBlitTransLucent75AlphaZReadWritePtr = NULL;

	delete RLEBlitTransLucent50AlphaZReadWritePtr;
	RLEBlitTransLucent50AlphaZReadWritePtr = NULL;

	delete RLEBlitTransLucent25AlphaZReadWritePtr;
	RLEBlitTransLucent25AlphaZReadWritePtr = NULL;
}
// FUNCTION: GAMEMD 0x00490B90
Blitter const * ConvertClass::Blitter_From_Flags(ShapeFlags_Type flags) const
{
	static ShapeFlags_Type z_read_flags = ShapeFlags_Type(SHAPE_ZREAD|SHAPE_ZGRAD);

	if (PlainBlitter == NULL) {
		((ConvertClass *)this)->Create_Blitters();
	}

	if (flags & SHAPE_REMAP) {

		if (flags & SHAPE_ZWRITE) {

			if (flags & SHAPE_ALPHA) {
				return(BlitTransZRemapXlatAlphaZReadWritePtr);
			}

			return(BlitTransZRemapXlatZReadWritePtr);
		}

		if (flags & z_read_flags) {

			if (flags & SHAPE_ALPHA) {
				return(BlitTransZRemapXlatAlphaZReadPtr);
			}

			return(BlitTransZRemapXlatZReadPtr);
		}

		if (flags & SHAPE_ALPHA) {
			return(BlitTransZRemapXlatAlphaPtr);
		}

		return(RemapBlitter);
	}

	switch (flags & (SHAPE_TRANSLUCENT25 | SHAPE_TRANSLUCENT50 | SHAPE_TRANSLUCENT75)) {
		case SHAPE_TRANSLUCENT25:

			if (flags & SHAPE_ZWRITE) {

				if (flags & SHAPE_ALPHA) {
					return(BlitTransLucent25AlphaZReadWritePtr);
				}

				return(BlitTransLucent25ZReadWritePtr);
			}

			if (flags & z_read_flags) {

				if (flags & SHAPE_PREDATOR) {

					if (flags & SHAPE_ALPHA) {
						return(BlitTransLucent25AlphaZReadWarpPtr);
					}

					return(BlitTransLucent25ZReadWarpPtr);
				}

				if (flags & SHAPE_ALPHA) {
					return(BlitTransLucent25AlphaZReadPtr);
				}

				return(BlitTransLucent25ZReadPtr);
			}

			if (flags & SHAPE_ALPHA) {
				return(BlitTransLucent25AlphaPtr);
			}

			return(Translucent3Blitter);

		case SHAPE_TRANSLUCENT50:

			if (flags & SHAPE_ZWRITE) {

				if (flags & SHAPE_ALPHA) {
					return(BlitTransLucent50AlphaZReadWritePtr);
				}

				return(BlitTransLucent50ZReadWritePtr);
			}

			if (flags & z_read_flags) {

				if (flags & SHAPE_PREDATOR) {

					if (flags & SHAPE_ALPHA) {
						return(BlitTransLucent50AlphaZReadWarpPtr);
					}

					return(BlitTransLucent50ZReadWarpPtr);
				}

				if (flags & SHAPE_ALPHA) {
					return(BlitTransLucent50AlphaZReadPtr);
				}

				return(BlitTransLucent50ZReadPtr);
			}

			if (flags & SHAPE_ALPHA) {
				return(BlitTransLucent50AlphaPtr);
			}

			if (flags & SHAPE_ZERO_ALPHA) {
				return(BlitTranslucent50ZeroAlphaPtr);
			}

			if (flags & SHAPE_NONZERO_ALPHA) {
				return(BlitTranslucent50NonzeroAlphaPtr);
			}

			return(Translucent2Blitter);

		case SHAPE_TRANSLUCENT75:

			if (flags & SHAPE_ZWRITE) {

				if (flags & SHAPE_ALPHA) {
					return(BlitTransLucent75AlphaZReadWritePtr);
				}

				return(BlitTransLucent75ZReadWritePtr);
			}

			if (flags & z_read_flags) {

				if (flags & SHAPE_PREDATOR) {

					if (flags & SHAPE_ALPHA) {
						return(BlitTransLucent75AlphaZReadWarpPtr);
					}

					return(BlitTransLucent75ZReadWarpPtr);
				}

				if (flags & SHAPE_ALPHA) {
					return(BlitTransLucent75AlphaZReadPtr);
				}

				return(BlitTransLucent75ZReadPtr);
			}

			if (flags & SHAPE_ALPHA) {
				return(BlitTransLucent75AlphaPtr);
			}

			return(Translucent1Blitter);
	}

	if (flags & SHAPE_DARKEN) {

		if (flags & SHAPE_ZWRITE) {
			return(BlitTransDarkenZReadWritePtr);
		}

		if (flags & z_read_flags) {
			return(BlitTransDarkenZReadPtr);
		}

		return(ShadowBlitter);
	}

	if (flags & SHAPE_NOTRANS) {

		if (flags & SHAPE_ZWRITE) {

			if (flags & SHAPE_ALPHA) {
				return(BlitTransXlatAlphaZReadWritePtr);
			}

			return(BlitTransXlatZReadWritePtr);
		}

		if (flags & z_read_flags) {

			if (flags & SHAPE_ALPHA) {
				return(BlitTransXlatAlphaZReadPtr);
			}

			return(BlitTransXlatZReadPtr);
		}

		if (flags & SHAPE_ALPHA) {
			return(BlitPlainXlatAlphaPtr);
		}

		return(PlainBlitter);
	}

	if (flags & SHAPE_ZWRITE) {

		if (flags & SHAPE_ALPHA) {
			return(BlitTransXlatAlphaZReadWritePtr);
		}

		return(BlitTransXlatZReadWritePtr);
	}

	if (flags & z_read_flags) {

		if (flags & SHAPE_ALPHA) {
			return(BlitTransXlatAlphaZReadPtr);
		}

		return(BlitTransXlatZReadPtr);
	}

	if (flags & SHAPE_ALPHA_WRITE) {
		return(BlitTransXlatWriteAlphaPtr);
	}

	if (flags & SHAPE_ALPHA_WRITE_MULT) {
		return(BlitTransXlatMultWriteAlphaPtr);
	}

	if (flags & SHAPE_ALPHA_BLEND) {
		return(BlitTranslucentWriteAlphaPtr);
	}

	if (flags & SHAPE_ALPHA) {
		return(BlitTransXlatAlphaPtr);
	}

	return(TransBlitter);
}

// FUNCTION: GAMEMD 0x00490E50
RLEBlitter const * ConvertClass::RLEBlitter_From_Flags(ShapeFlags_Type flags) const
{
	static ShapeFlags_Type z_read_flags = ShapeFlags_Type(SHAPE_ZREAD|SHAPE_ZGRAD);

	if (PlainBlitter == NULL) {
		((ConvertClass *)this)->Create_Blitters();
	}

	if (flags & SHAPE_REMAP) {

		if (flags & SHAPE_ZWRITE) {

			if (flags & SHAPE_ALPHA) {
				return(RLEBlitTransZRemapXlatAlphaZReadWritePtr);
			}

			return(RLEBlitTransZRemapXlatZReadWritePtr);
		}

		if (flags & z_read_flags) {

			if (flags & SHAPE_ALPHA) {
				return(RLEBlitTransZRemapXlatAlphaZReadPtr);
			}

			return(RLEBlitTransZRemapXlatZReadPtr);
		}

		if (flags & SHAPE_ALPHA) {
			return(RLEBlitTransZRemapXlatAlphaPtr);
		}

		return(RLERemapBlitter);
	}

	switch (flags & (SHAPE_TRANSLUCENT25 | SHAPE_TRANSLUCENT50 | SHAPE_TRANSLUCENT75)) {
		case SHAPE_TRANSLUCENT25:

			if (flags & SHAPE_ZWRITE) {

				if (flags & SHAPE_ALPHA) {
					return(RLEBlitTransLucent25AlphaZReadWritePtr);
				}

				return(RLEBlitTransLucent25ZReadWritePtr);
			}

			if (flags & z_read_flags) {
				if (flags & SHAPE_PREDATOR) {

					if (flags & SHAPE_ALPHA) {
						return(RLEBlitTransLucent25AlphaZReadWarpPtr);
					}

					return(RLEBlitTransLucent25ZReadWarpPtr);
				}

				if (flags & SHAPE_ALPHA) {
					return(RLEBlitTransLucent25AlphaZReadPtr);
				}

				return(RLEBlitTransLucent25ZReadPtr);
			}

			if (flags & SHAPE_ALPHA) {
				return(RLEBlitTransLucent25AlphaPtr);
			}

			return(RLETranslucent3Blitter);

		case SHAPE_TRANSLUCENT50:

			if (flags & SHAPE_ZWRITE) {

				if (flags & SHAPE_ALPHA) {
					return(RLEBlitTransLucent50AlphaZReadWritePtr);
				}

				return(RLEBlitTransLucent50ZReadWritePtr);
			}

			if (flags & z_read_flags) {

				if (flags & SHAPE_PREDATOR) {

					if (flags & SHAPE_ALPHA) {
						return(RLEBlitTransLucent50AlphaZReadWarpPtr);
					}

					return(RLEBlitTransLucent50ZReadWarpPtr);
				}

				if (flags & SHAPE_ALPHA) {
					return(RLEBlitTransLucent50AlphaZReadPtr);
				}

				return(RLEBlitTransLucent50ZReadPtr);
			}

			if (flags & SHAPE_ALPHA) {
				return(RLEBlitTransLucent50AlphaPtr);
			}

			return(RLETranslucent2Blitter);

		case SHAPE_TRANSLUCENT75:

			if (flags & SHAPE_ZWRITE) {

				if (flags & SHAPE_ALPHA) {
					return(RLEBlitTransLucent75AlphaZReadWritePtr);
				}

				return(RLEBlitTransLucent75ZReadWritePtr);
			}

			if (flags & z_read_flags) {

				if (flags & SHAPE_PREDATOR) {

					if (flags & SHAPE_ALPHA) {
						return(RLEBlitTransLucent75AlphaZReadWarpPtr);
					}

					return(RLEBlitTransLucent75ZReadWarpPtr);
				}

				if (flags & SHAPE_ALPHA) {
					return(RLEBlitTransLucent75AlphaZReadPtr);
				}

				return(RLEBlitTransLucent75ZReadPtr);
			}

			if (flags & SHAPE_ALPHA) {
				return(RLEBlitTransLucent75AlphaPtr);
			}

			return(RLETranslucent1Blitter);
	}

	if (flags & SHAPE_DARKEN) {

		if (flags & SHAPE_ZWRITE) {
			return(RLEBlitTransDarkenZReadWritePtr);
		}

		if (flags & z_read_flags) {
			return(RLEBlitTransDarkenZReadPtr);
		}

		return(RLEShadowBlitter);
	}

	// No plain RLE blitter is returned here: shapes drawn through this path must not
	// be RLE compressed.
	if (flags & SHAPE_NOTRANS) {

		if (flags & SHAPE_ZWRITE) {

			if (flags & SHAPE_ALPHA) {
				return(RLEBlitTransXlatAlphaZReadWritePtr);
			}

			return(RLEBlitTransXlatZReadWritePtr);
		}

		if (flags & z_read_flags) {

			if (flags & SHAPE_ALPHA) {
				return(RLEBlitTransXlatAlphaZReadPtr);
			}

			return(RLEBlitTransXlatZReadPtr);
		}

		if (flags & SHAPE_ALPHA) {
			return(RLEBlitTransXlatAlphaPtr);
		}

		return(RLETransBlitter);

	}

	if (flags & SHAPE_ZWRITE) {

		if (flags & SHAPE_ALPHA) {
			return(RLEBlitTransXlatAlphaZReadWritePtr);
		}

		return(RLEBlitTransXlatZReadWritePtr);
	}

	if (flags & z_read_flags) {

		if (flags & SHAPE_ALPHA) {
			return(RLEBlitTransXlatAlphaZReadPtr);
		}

		return(RLEBlitTransXlatZReadPtr);
	}

	if (flags & SHAPE_ALPHA) {
		return(RLEBlitTransXlatAlphaPtr);
	}

	return(RLETransBlitter);
}

// FUNCTION: GAMEMD 0x00491100
void ConvertClass::Reinitialize_Hicolor_Tables(int redleft, int redright, int greenleft, int greenright, int blueleft, int blueright)
{
	for (int i = 0; i < Drawers.Count(); i++) {
		ConvertClass * converter = Drawers[i];
		if (converter->Bytes_Per_Pixel() == 2) {
			unsigned short * table = (unsigned short *)converter->IntensityTranslator;
			for (int index = 0; index < 256; index++) {
				unsigned short color = table[index];

				RGBClass rgb;
				rgb.Set_Blue((color >> redright) << redleft);
				rgb.Set_Green((color >> greenright) << greenleft);
				rgb.Set_Red((color >> blueright) << blueleft);

				table[index] = DSurface::Build_Hicolor_Pixel(rgb.Get_Blue(), rgb.Get_Green(), rgb.Get_Red());
			}

			converter->HalfbrightMask = DSurface::Get_Halfbright_Mask();
			converter->QuarterbrightMask = DSurface::Get_Quarterbright_Mask();

			converter->Destroy_Blitters();
			converter->Create_Blitters();
		}
	}
}

template class BlitTransXlat<unsigned short>;

// TEMPLATE: GAMEMD 0x004912B0
// ?BlitForward@?$BlitTransXlat@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00491300
// ?BlitBackward@?$BlitTransXlat@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00491330
// ?Blit_Copy_Tinted@?$BlitTransXlat@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00491360
// ?Blit_Move_Tinted@?$BlitTransXlat@G@@UBEXPAXPBXHH00HG@Z

// TEMPLATE: GAMEMD 0x00491390
// ?Clear@?$DynamicVectorClass@PAVConvertClass@@@@UAEXXZ
template class DynamicVectorClass<ConvertClass *>;

// TEMPLATE: GAMEMD 0x004913C0
// ?ID@?$DynamicVectorClass@PAVConvertClass@@@@UAEHPBQAVConvertClass@@@Z
// TEMPLATE: GAMEMD 0x004913E0
// ?GetItem@?$VectorClass@PAVConvertClass@@@@UBEPAVConvertClass@@H@Z

// TEMPLATE: GAMEMD 0x004913F0
// ?BlitForward@?$BlitPlainXlat@E@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00491430
// ?BlitBackward@?$BlitPlainXlat@E@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00491460
// ?Blit_Copy_Tinted@?$BlitPlainXlat@E@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00491490
// ?Blit_Move_Tinted@?$BlitPlainXlat@E@@UBEXPAXPBXHH00HG@Z
template class BlitPlainXlat<unsigned char>;

// TEMPLATE: GAMEMD 0x004914C0
// ?BlitForward@?$BlitTransXlat@E@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00491500
// ?BlitBackward@?$BlitTransXlat@E@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00491530
// ?Blit_Copy_Tinted@?$BlitTransXlat@E@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00491560
// ?Blit_Move_Tinted@?$BlitTransXlat@E@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlat<unsigned char>;

// TEMPLATE: GAMEMD 0x00491590
// ?BlitForward@?$BlitTransZRemapXlat@E@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004915E0
// ?BlitBackward@?$BlitTransZRemapXlat@E@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00491610
// ?Blit_Copy_Tinted@?$BlitTransZRemapXlat@E@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00491640
// ?Blit_Move_Tinted@?$BlitTransZRemapXlat@E@@UBEXPAXPBXHH00HG@Z
template class BlitTransZRemapXlat<unsigned char>;

// TEMPLATE: GAMEMD 0x00491670
// ?BlitForward@?$BlitTransRemapDest@E@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004916B0
// ?BlitBackward@?$BlitTransRemapDest@E@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004916E0
// ?Blit_Copy_Tinted@?$BlitTransRemapDest@E@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00491710
// ?Blit_Move_Tinted@?$BlitTransRemapDest@E@@UBEXPAXPBXHH00HG@Z
template class BlitTransRemapDest<unsigned char>;

// TEMPLATE: GAMEMD 0x00491740
// ?BlitForward@?$BlitTransRemapXlat@E@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00491790
// ?BlitBackward@?$BlitTransRemapXlat@E@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004917C0
// ?Blit_Copy_Tinted@?$BlitTransRemapXlat@E@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004917F0
// ?Blit_Move_Tinted@?$BlitTransRemapXlat@E@@UBEXPAXPBXHH00HG@Z
template class BlitTransRemapXlat<unsigned char>;

// TEMPLATE: GAMEMD 0x00491820
// ?Blit@?$RLEBlitTransXlat@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004918A0
// ?Blit_Copy_Tinted@?$RLEBlitTransXlat@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransXlat<unsigned char>;

// TEMPLATE: GAMEMD 0x004918E0
// ?Blit@?$RLEBlitTransZRemapXlat@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00491970
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlat@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlat<unsigned char>;

// TEMPLATE: GAMEMD 0x004919B0
// ?Blit@?$RLEBlitTransRemapDest@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00491A30
// ?Blit_Copy_Tinted@?$RLEBlitTransRemapDest@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransRemapDest<unsigned char>;

// TEMPLATE: GAMEMD 0x00491A70
// ?Blit@?$RLEBlitTransRemapXlat@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00491B00
// ?Blit_Copy_Tinted@?$RLEBlitTransRemapXlat@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransRemapXlat<unsigned char>;

// TEMPLATE: GAMEMD 0x00491B40
// ?Blit@?$RLEBlitTransXlatZRead@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00491C10
// ?Blit_Copy_Tinted@?$RLEBlitTransXlatZRead@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransXlatZRead<unsigned char>;

// TEMPLATE: GAMEMD 0x00491C50
// ?Blit@?$RLEBlitTransZRemapXlatZRead@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00491D50
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlatZRead@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlatZRead<unsigned char>;

// TEMPLATE: GAMEMD 0x00491D90
// ?Blit@?$RLEBlitTransRemapDestZRead@E@@UBEXPAXPBXHHH00HH0@Z
template class RLEBlitTransRemapDestZRead<unsigned char>;

// TEMPLATE: GAMEMD 0x00491E60
// ?Blit_Copy_Tinted@?$RLEBlitTransRemapDestZRead@E@@UBEXPAXPBXHHH00HH0G@Z

// TEMPLATE: GAMEMD 0x00491EA0
// ?Blit@?$RLEBlitTransRemapXlatZRead@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00491F90
// ?Blit_Copy_Tinted@?$RLEBlitTransRemapXlatZRead@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransRemapXlatZRead<unsigned char>;

// TEMPLATE: GAMEMD 0x00491FD0
// ?Blit@?$RLEBlitTransXlatZReadWrite@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004920C0
// ?Blit_Copy_Tinted@?$RLEBlitTransXlatZReadWrite@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransXlatZReadWrite<unsigned char>;

// TEMPLATE: GAMEMD 0x00492100
// ?Blit@?$RLEBlitTransZRemapXlatZReadWrite@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00492210
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlatZReadWrite@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlatZReadWrite<unsigned char>;

// TEMPLATE: GAMEMD 0x00492250
// ?Blit@?$RLEBlitTransRemapDestZReadWrite@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00492330
// ?Blit_Copy_Tinted@?$RLEBlitTransRemapDestZReadWrite@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransRemapDestZReadWrite<unsigned char>;

// TEMPLATE: GAMEMD 0x00492370
// ?Blit@?$RLEBlitTransRemapXlatZReadWrite@E@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00492480
// ?Blit_Copy_Tinted@?$RLEBlitTransRemapXlatZReadWrite@E@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransRemapXlatZReadWrite<unsigned char>;

// TEMPLATE: GAMEMD 0x004924C0
// ?BlitForward@?$BlitPlainXlat@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492500
// ?BlitBackward@?$BlitPlainXlat@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492530
// ?Blit_Copy_Tinted@?$BlitPlainXlat@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492560
// ?Blit_Move_Tinted@?$BlitPlainXlat@G@@UBEXPAXPBXHH00HG@Z
template class BlitPlainXlat<unsigned short>;

// TEMPLATE: GAMEMD 0x00492590
// ?BlitForward@?$BlitTransZRemapXlat@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004925E0
// ?BlitBackward@?$BlitTransZRemapXlat@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492610
// ?Blit_Copy_Tinted@?$BlitTransZRemapXlat@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492640
// ?Blit_Move_Tinted@?$BlitTransZRemapXlat@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransZRemapXlat<unsigned short>;

// TEMPLATE: GAMEMD 0x00492670
// ?BlitForward@?$BlitTransDarken@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004926B0
// ?BlitBackward@?$BlitTransDarken@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004926E0
// ?Blit_Copy_Tinted@?$BlitTransDarken@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492710
// ?Blit_Move_Tinted@?$BlitTransDarken@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransDarken<unsigned short>;

// TEMPLATE: GAMEMD 0x00492740
// ?BlitForward@?$BlitTransLucent75@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004927A0
// ?BlitBackward@?$BlitTransLucent75@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004927D0
// ?Blit_Copy_Tinted@?$BlitTransLucent75@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492800
// ?Blit_Move_Tinted@?$BlitTransLucent75@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75<unsigned short>;

// TEMPLATE: GAMEMD 0x00492890
// ?BlitBackward@?$BlitTransLucent50@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004928C0
// ?Blit_Copy_Tinted@?$BlitTransLucent50@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004928F0
// ?Blit_Move_Tinted@?$BlitTransLucent50@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50<unsigned short>;

// TEMPLATE: GAMEMD 0x00492920
// ?BlitForward@?$BlitTransLucent25@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492980
// ?BlitBackward@?$BlitTransLucent25@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004929B0
// ?Blit_Copy_Tinted@?$BlitTransLucent25@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004929E0
// ?Blit_Move_Tinted@?$BlitTransLucent25@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25<unsigned short>;

// TEMPLATE: GAMEMD 0x00492A10
// ?BlitForward@?$BlitPlainXlatZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492A70
// ?BlitBackward@?$BlitPlainXlatZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492AA0
// ?Blit_Copy_Tinted@?$BlitPlainXlatZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492AD0
// ?Blit_Move_Tinted@?$BlitPlainXlatZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitPlainXlatZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00492B00
// ?BlitForward@?$BlitTransXlatZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492B70
// ?BlitBackward@?$BlitTransXlatZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492BA0
// ?Blit_Copy_Tinted@?$BlitTransXlatZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492BD0
// ?Blit_Move_Tinted@?$BlitTransXlatZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlatZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00492C00
// ?BlitForward@?$BlitTransZRemapXlatZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492C90
// ?BlitBackward@?$BlitTransZRemapXlatZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492CC0
// ?Blit_Copy_Tinted@?$BlitTransZRemapXlatZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492CF0
// ?Blit_Move_Tinted@?$BlitTransZRemapXlatZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransZRemapXlatZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00492D20
// ?BlitForward@?$BlitTransDarkenZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492D80
// ?BlitBackward@?$BlitTransDarkenZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492DB0
// ?Blit_Copy_Tinted@?$BlitTransDarkenZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492DE0
// ?Blit_Move_Tinted@?$BlitTransDarkenZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransDarkenZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00492E10
// ?BlitForward@?$BlitTransLucent75ZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492EA0
// ?BlitBackward@?$BlitTransLucent75ZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492ED0
// ?Blit_Copy_Tinted@?$BlitTransLucent75ZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00492F00
// ?Blit_Move_Tinted@?$BlitTransLucent75ZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75ZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00492F30
// ?BlitForward@?$BlitTransLucent50ZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00492FC0
// ?BlitBackward@?$BlitTransLucent50ZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00492FF0
// ?Blit_Copy_Tinted@?$BlitTransLucent50ZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493020
// ?Blit_Move_Tinted@?$BlitTransLucent50ZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50ZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00493050
// ?BlitForward@?$BlitTransLucent25ZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004930E0
// ?BlitBackward@?$BlitTransLucent25ZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493110
// ?Blit_Copy_Tinted@?$BlitTransLucent25ZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493140
// ?Blit_Move_Tinted@?$BlitTransLucent25ZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25ZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00493170
// ?BlitForward@?$BlitTransLucent75ZReadWarp@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493210
// ?BlitBackward@?$BlitTransLucent75ZReadWarp@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493240
// ?Blit_Copy_Tinted@?$BlitTransLucent75ZReadWarp@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493270
// ?Blit_Move_Tinted@?$BlitTransLucent75ZReadWarp@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75ZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x004932A0
// ?BlitForward@?$BlitTransLucent50ZReadWarp@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493340
// ?BlitBackward@?$BlitTransLucent50ZReadWarp@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493370
// ?Blit_Copy_Tinted@?$BlitTransLucent50ZReadWarp@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004933A0
// ?Blit_Move_Tinted@?$BlitTransLucent50ZReadWarp@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50ZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x004933D0
// ?BlitForward@?$BlitTransLucent25ZReadWarp@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493470
// ?BlitBackward@?$BlitTransLucent25ZReadWarp@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004934A0
// ?Blit_Copy_Tinted@?$BlitTransLucent25ZReadWarp@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004934D0
// ?Blit_Move_Tinted@?$BlitTransLucent25ZReadWarp@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25ZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00493500
// ?BlitForward@?$BlitPlainXlatZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493570
// ?BlitBackward@?$BlitPlainXlatZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004935A0
// ?Blit_Copy_Tinted@?$BlitPlainXlatZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004935D0
// ?Blit_Move_Tinted@?$BlitPlainXlatZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitPlainXlatZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00493600
// ?BlitForward@?$BlitTransXlatZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493680
// ?BlitBackward@?$BlitTransXlatZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004936B0
// ?Blit_Copy_Tinted@?$BlitTransXlatZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004936E0
// ?Blit_Move_Tinted@?$BlitTransXlatZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlatZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00493710
// ?BlitForward@?$BlitTransZRemapXlatZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004937A0
// ?BlitBackward@?$BlitTransZRemapXlatZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004937D0
// ?Blit_Copy_Tinted@?$BlitTransZRemapXlatZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493800
// ?Blit_Move_Tinted@?$BlitTransZRemapXlatZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransZRemapXlatZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00493830
// ?BlitForward@?$BlitTransDarkenZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004938A0
// ?BlitBackward@?$BlitTransDarkenZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004938D0
// ?Blit_Copy_Tinted@?$BlitTransDarkenZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493900
// ?Blit_Move_Tinted@?$BlitTransDarkenZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransDarkenZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00493930
// ?BlitForward@?$BlitTransLucent75ZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004939D0
// ?BlitBackward@?$BlitTransLucent75ZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493A00
// ?Blit_Copy_Tinted@?$BlitTransLucent75ZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493A30
// ?Blit_Move_Tinted@?$BlitTransLucent75ZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75ZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00493A60
// ?BlitForward@?$BlitTransLucent50ZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493B00
// ?BlitBackward@?$BlitTransLucent50ZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493B30
// ?Blit_Copy_Tinted@?$BlitTransLucent50ZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493B60
// ?Blit_Move_Tinted@?$BlitTransLucent50ZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50ZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00493B90
// ?BlitForward@?$BlitTransLucent25ZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493C30
// ?BlitBackward@?$BlitTransLucent25ZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493C60
// ?Blit_Copy_Tinted@?$BlitTransLucent25ZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493C90
// ?Blit_Move_Tinted@?$BlitTransLucent25ZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25ZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00493CC0
// ?BlitForward@?$BlitPlainXlatAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493D60
// ?BlitBackward@?$BlitPlainXlatAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493D90
// ?Blit_Copy_Tinted@?$BlitPlainXlatAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493DC0
// ?Blit_Move_Tinted@?$BlitPlainXlatAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitPlainXlatAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00493DF0
// ?BlitForward@?$BlitTransXlatAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493EA0
// ?BlitBackward@?$BlitTransXlatAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00493ED0
// ?Blit_Copy_Tinted@?$BlitTransXlatAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00493F00
// ?Blit_Move_Tinted@?$BlitTransXlatAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlatAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00493F30
// ?BlitForward@?$BlitTransZRemapXlatAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00493FF0
// ?BlitBackward@?$BlitTransZRemapXlatAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494020
// ?Blit_Copy_Tinted@?$BlitTransZRemapXlatAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494050
// ?Blit_Move_Tinted@?$BlitTransZRemapXlatAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransZRemapXlatAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494080
// ?BlitForward@?$BlitTransLucent75Alpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494150
// ?BlitBackward@?$BlitTransLucent75Alpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494180
// ?Blit_Copy_Tinted@?$BlitTransLucent75Alpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004941B0
// ?Blit_Move_Tinted@?$BlitTransLucent75Alpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75Alpha<unsigned short>;

// TEMPLATE: GAMEMD 0x004941E0
// ?BlitForward@?$BlitTransLucent50Alpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004942A0
// ?BlitBackward@?$BlitTransLucent50Alpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004942D0
// ?Blit_Copy_Tinted@?$BlitTransLucent50Alpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494300
// ?Blit_Move_Tinted@?$BlitTransLucent50Alpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50Alpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494330
// ?BlitForward@?$BlitTransLucent25Alpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494400
// ?BlitBackward@?$BlitTransLucent25Alpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494430
// ?Blit_Copy_Tinted@?$BlitTransLucent25Alpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494460
// ?Blit_Move_Tinted@?$BlitTransLucent25Alpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25Alpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494490
// ?BlitForward@?$BlitTransXlatWriteAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494500
// ?BlitBackward@?$BlitTransXlatWriteAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494530
// ?Blit_Copy_Tinted@?$BlitTransXlatWriteAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494560
// ?Blit_Move_Tinted@?$BlitTransXlatWriteAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlatWriteAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494590
// ?BlitForward@?$BlitTransXlatMultWriteAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494600
// ?BlitBackward@?$BlitTransXlatMultWriteAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494630
// ?Blit_Copy_Tinted@?$BlitTransXlatMultWriteAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494660
// ?Blit_Move_Tinted@?$BlitTransXlatMultWriteAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlatMultWriteAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494690
// ?BlitForward@?$BlitTranslucentWriteAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494890
// ?BlitBackward@?$BlitTranslucentWriteAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004948C0
// ?Blit_Copy_Tinted@?$BlitTranslucentWriteAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004948F0
// ?Blit_Move_Tinted@?$BlitTranslucentWriteAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTranslucentWriteAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494920
// ?BlitForward@?$BlitTranslucent50NonzeroAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004949B0
// ?BlitBackward@?$BlitTranslucent50NonzeroAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004949E0
// ?Blit_Copy_Tinted@?$BlitTranslucent50NonzeroAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494A10
// ?Blit_Move_Tinted@?$BlitTranslucent50NonzeroAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTranslucent50NonzeroAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494A40
// ?BlitForward@?$BlitTranslucent50ZeroAlpha@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494AD0
// ?BlitBackward@?$BlitTranslucent50ZeroAlpha@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494B00
// ?Blit_Copy_Tinted@?$BlitTranslucent50ZeroAlpha@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494B30
// ?Blit_Move_Tinted@?$BlitTranslucent50ZeroAlpha@G@@UBEXPAXPBXHH00HG@Z
template class BlitTranslucent50ZeroAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00494B60
// ?BlitForward@?$BlitTransXlatAlphaZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494C40
// ?Blit_Copy_Tinted@?$BlitTransXlatAlphaZRead@G@@UBEXPAXPBXHH00HHG@Z
template<class T>
void BlitTransXlatAlphaZRead<T>::Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const
{
	unsigned short *zb = (unsigned short *)z_buff;
	unsigned short *ap = (unsigned short *)a_buff;
	const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

	for (int index = 0; index < length; index++) {
		if (z_min < *zb++) {
			unsigned char color = *(unsigned char const *)source;
			if (color != 0) {
				*((T *)dest) = TranslateTable[color | aLUT[*ap]] | tint;
			}
		}
		source = ((unsigned char *)source) + 1;
		dest = ((T *)dest) + 1;

		zb = Blit_Wrap_Z_Buffer(zb);
		ap++;
		ap = Blit_Wrap_A_Buffer(ap);
	}
}

// TEMPLATE: GAMEMD 0x00494D20
// ?BlitBackward@?$BlitTransXlatAlphaZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494D50
// ?Blit_Move_Tinted@?$BlitTransXlatAlphaZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlatAlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00494D90
// ?BlitForward@?$BlitTransZRemapXlatAlphaZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00494E90
// ?BlitBackward@?$BlitTransZRemapXlatAlphaZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00494EC0
// ?Blit_Copy_Tinted@?$BlitTransZRemapXlatAlphaZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00494EF0
// ?Blit_Move_Tinted@?$BlitTransZRemapXlatAlphaZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransZRemapXlatAlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00494F20
// ?BlitForward@?$BlitTransLucent75AlphaZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00495030
// ?BlitBackward@?$BlitTransLucent75AlphaZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00495060
// ?Blit_Copy_Tinted@?$BlitTransLucent75AlphaZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495090
// ?Blit_Move_Tinted@?$BlitTransLucent75AlphaZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75AlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x004950C0
// ?BlitForward@?$BlitTransLucent50AlphaZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004951C0
// ?BlitBackward@?$BlitTransLucent50AlphaZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004951F0
// ?Blit_Copy_Tinted@?$BlitTransLucent50AlphaZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495220
// ?Blit_Move_Tinted@?$BlitTransLucent50AlphaZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50AlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00495250
// ?BlitForward@?$BlitTransLucent25AlphaZRead@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00495360
// ?BlitBackward@?$BlitTransLucent25AlphaZRead@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00495390
// ?Blit_Copy_Tinted@?$BlitTransLucent25AlphaZRead@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004953C0
// ?Blit_Move_Tinted@?$BlitTransLucent25AlphaZRead@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25AlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x004953F0
// ?BlitForward@?$BlitTransLucent75AlphaZReadWarp@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00495500
// ?BlitBackward@?$BlitTransLucent75AlphaZReadWarp@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00495530
// ?Blit_Copy_Tinted@?$BlitTransLucent75AlphaZReadWarp@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495560
// ?Blit_Move_Tinted@?$BlitTransLucent75AlphaZReadWarp@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75AlphaZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00495590
// ?BlitForward@?$BlitTransLucent50AlphaZReadWarp@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004956A0
// ?BlitBackward@?$BlitTransLucent50AlphaZReadWarp@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004956D0
// ?Blit_Copy_Tinted@?$BlitTransLucent50AlphaZReadWarp@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495700
// ?Blit_Move_Tinted@?$BlitTransLucent50AlphaZReadWarp@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50AlphaZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00495730
// ?BlitForward@?$BlitTransLucent25AlphaZReadWarp@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00495840
// ?BlitBackward@?$BlitTransLucent25AlphaZReadWarp@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00495870
// ?Blit_Copy_Tinted@?$BlitTransLucent25AlphaZReadWarp@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x004958A0
// ?Blit_Move_Tinted@?$BlitTransLucent25AlphaZReadWarp@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25AlphaZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x004958D0
// ?BlitForward@?$BlitTransXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x004959C0
// ?BlitBackward@?$BlitTransXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x004959F0
// ?Blit_Copy_Tinted@?$BlitTransXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495A20
// ?Blit_Move_Tinted@?$BlitTransXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransXlatAlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00495A50
// ?BlitForward@?$BlitTransZRemapXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00495B30
// ?BlitBackward@?$BlitTransZRemapXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00495B60
// ?Blit_Copy_Tinted@?$BlitTransZRemapXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495B90
// ?Blit_Move_Tinted@?$BlitTransZRemapXlatAlphaZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransZRemapXlatAlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00495BC0
// ?BlitForward@?$BlitTransLucent75AlphaZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00495CD0
// ?BlitBackward@?$BlitTransLucent75AlphaZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00495D00
// ?Blit_Copy_Tinted@?$BlitTransLucent75AlphaZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495D30
// ?Blit_Move_Tinted@?$BlitTransLucent75AlphaZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent75AlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00495D60
// ?BlitForward@?$BlitTransLucent50AlphaZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00495E70
// ?BlitBackward@?$BlitTransLucent50AlphaZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00495EA0
// ?Blit_Copy_Tinted@?$BlitTransLucent50AlphaZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00495ED0
// ?Blit_Move_Tinted@?$BlitTransLucent50AlphaZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent50AlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00495F00
// ?BlitForward@?$BlitTransLucent25AlphaZReadWrite@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x00496010
// ?BlitBackward@?$BlitTransLucent25AlphaZReadWrite@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x00496040
// ?Blit_Copy_Tinted@?$BlitTransLucent25AlphaZReadWrite@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x00496070
// ?Blit_Move_Tinted@?$BlitTransLucent25AlphaZReadWrite@G@@UBEXPAXPBXHH00HG@Z
template class BlitTransLucent25AlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x004960A0
// ?Blit@?$RLEBlitTransXlat@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496130
// ?Blit_Copy_Tinted@?$RLEBlitTransXlat@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransXlat<unsigned short>;

// TEMPLATE: GAMEMD 0x00496170
// ?Blit@?$RLEBlitTransZRemapXlat@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496210
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlat@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlat<unsigned short>;

// TEMPLATE: GAMEMD 0x00496250
// ?Blit@?$RLEBlitTransDarken@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004962D0
// ?Blit_Copy_Tinted@?$RLEBlitTransDarken@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransDarken<unsigned short>;

// TEMPLATE: GAMEMD 0x00496310
// ?Blit@?$RLEBlitTransLucent75@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004963B0
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75<unsigned short>;

// TEMPLATE: GAMEMD 0x004963F0
// ?Blit@?$RLEBlitTransLucent50@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496490
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50<unsigned short>;

// TEMPLATE: GAMEMD 0x004964D0
// ?Blit@?$RLEBlitTransLucent25@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496570
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25<unsigned short>;

// TEMPLATE: GAMEMD 0x004965B0
// ?Blit@?$RLEBlitTransXlatZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496690
// ?Blit_Copy_Tinted@?$RLEBlitTransXlatZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransXlatZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x004966D0
// ?Blit@?$RLEBlitTransZRemapXlatZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004967E0
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlatZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlatZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00496820
// ?Blit@?$RLEBlitTransDarkenZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004968F0
// ?Blit_Copy_Tinted@?$RLEBlitTransDarkenZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransDarkenZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00496930
// ?Blit@?$RLEBlitTransLucent75ZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496A40
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75ZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75ZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00496A80
// ?Blit@?$RLEBlitTransLucent50ZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496B80
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50ZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50ZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00496BC0
// ?Blit@?$RLEBlitTransLucent25ZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496CD0
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25ZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25ZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00496D10
// ?Blit@?$RLEBlitTransLucent75ZReadWarp@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496E20
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75ZReadWarp@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75ZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00496E60
// ?Blit@?$RLEBlitTransLucent50ZReadWarp@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00496F70
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50ZReadWarp@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50ZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00496FB0
// ?Blit@?$RLEBlitTransLucent25ZReadWarp@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004970C0
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25ZReadWarp@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25ZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00497100
// ?Blit@?$RLEBlitTransXlatZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004971F0
// ?Blit_Copy_Tinted@?$RLEBlitTransXlatZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransXlatZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00497230
// ?Blit@?$RLEBlitTransZRemapXlatZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497350
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlatZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlatZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00497390
// ?Blit@?$RLEBlitTransDarkenZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497470
// ?Blit_Copy_Tinted@?$RLEBlitTransDarkenZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransDarkenZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x004974B0
// ?Blit@?$RLEBlitTransLucent75ZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004975D0
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75ZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75ZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00497610
// ?Blit@?$RLEBlitTransLucent50ZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497720
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50ZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50ZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00497760
// ?Blit@?$RLEBlitTransLucent25ZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497880
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25ZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25ZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x004978C0
// ?Blit@?$RLEBlitTransXlatAlpha@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004979D0
// ?Blit_Copy_Tinted@?$RLEBlitTransXlatAlpha@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransXlatAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00497A10
// ?Blit@?$RLEBlitTransZRemapXlatAlpha@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497B40
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlatAlpha@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlatAlpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00497B80
// ?Blit@?$RLEBlitTransLucent75Alpha@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497CB0
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75Alpha@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75Alpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00497CF0
// ?Blit@?$RLEBlitTransLucent50Alpha@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497E20
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50Alpha@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50Alpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00497E60
// ?Blit@?$RLEBlitTransLucent25Alpha@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00497F90
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25Alpha@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25Alpha<unsigned short>;

// TEMPLATE: GAMEMD 0x00497FD0
// ?Blit@?$RLEBlitTransXlatAlphaZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00498150
// ?Blit_Copy_Tinted@?$RLEBlitTransXlatAlphaZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template<class T>
void RLEBlitTransXlatAlphaZRead<T>::Blit_Copy_Tinted(void * dest, void const * source, int length, int leadskip, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, void * zshape, unsigned short tint) const
{
	unsigned char const * sptr = (unsigned char const *)source;
	T * dptr = (T *)dest;
	unsigned short* zp = (unsigned short*)(z_buff);
	unsigned short* ap = (unsigned short*)(a_buff);
	const signed char* zs = (const signed char*)(zshape);

	const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

	if (leadskip > 0) {
		int transcount = Skip_Leading_Pixels(sptr, leadskip);
		dptr += transcount;
		length -= transcount;

		zp += transcount;
		zp = Blit_Wrap_Z_Buffer(zp);

		ap += transcount;
		ap = Blit_Wrap_A_Buffer(ap);
	}

	while (length > 0) {
		unsigned char value = *sptr++;

		if (value == '\0') {
			value = *sptr++;
			length -= value;
			dptr += value;
			zp += value;
			zs += value;
			ap += value;
		} else {
			if ((z_min - (int)*zs++) < (int)*zp++) {
				*dptr++ = TranslateTable[value | aLUT[*ap]] | tint;
				length--;
			} else {
				dptr++;
				length--;
			}
			ap++;
		}
		zp = Blit_Wrap_Z_Buffer(zp);
		ap = Blit_Wrap_A_Buffer(ap);
	}
}

template class RLEBlitTransXlatAlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x004982E0
// ?Blit@?$RLEBlitTransZRemapXlatAlphaZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00498490
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlatAlphaZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlatAlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x004984D0
// ?Blit@?$RLEBlitTransLucent75AlphaZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00498690
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75AlphaZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75AlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x004986D0
// ?Blit@?$RLEBlitTransLucent50AlphaZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00498880
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50AlphaZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50AlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x004988C0
// ?Blit@?$RLEBlitTransLucent25AlphaZRead@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00498A80
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25AlphaZRead@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25AlphaZRead<unsigned short>;

// TEMPLATE: GAMEMD 0x00498AC0
// ?Blit@?$RLEBlitTransLucent75AlphaZReadWarp@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00498C90
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75AlphaZReadWarp@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75AlphaZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00498CD0
// ?Blit@?$RLEBlitTransLucent50AlphaZReadWarp@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00498E90
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50AlphaZReadWarp@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50AlphaZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x00498ED0
// ?Blit@?$RLEBlitTransLucent25AlphaZReadWarp@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x004990A0
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25AlphaZReadWarp@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25AlphaZReadWarp<unsigned short>;

// TEMPLATE: GAMEMD 0x004990E0
// ?Blit@?$RLEBlitTransXlatAlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00499280
// ?Blit_Copy_Tinted@?$RLEBlitTransXlatAlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template<class T>
void RLEBlitTransXlatAlphaZReadWrite<T>::Blit_Copy_Tinted(void * dest, void const * source, int length, int leadskip, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, void * zshape, unsigned short tint) const
{
	unsigned char const * sptr = (unsigned char const *)source;
	T * dptr = (T *)dest;
	unsigned short* zp = (unsigned short*)(z_buff);
	unsigned short* ap = (unsigned short*)(a_buff);
	const signed char* zs = (const signed char*)(zshape);

	const unsigned short* aLUT = AlphaLightingRemap->Get_Table(alpha_level);

	if (leadskip > 0) {
		int transcount = Skip_Leading_Pixels(sptr, leadskip);
		dptr += transcount;
		length -= transcount;

		zp += transcount;
		zp = Blit_Wrap_Z_Buffer(zp);

		ap += transcount;
		ap = Blit_Wrap_A_Buffer(ap);
	}

	while (length > 0) {
		unsigned char value = *sptr++;

		if (value == '\0') {
			value = *sptr++;
			length -= value;
			dptr += value;
			zp += value;
			zs += value;
			ap += value;
		} else {
			if ((z_min - (int)*zs) < (int)*zp) {
				*dptr++ = TranslateTable[value | aLUT[*ap]] | tint;
				*zp++  = z_min - *zs++;
				length--;
			} else {
				zs++;
				dptr++;
				zp++;
				length--;
			}
			ap++;
		}
		zp = Blit_Wrap_Z_Buffer(zp);
		ap = Blit_Wrap_A_Buffer(ap);
	}
}

template class RLEBlitTransXlatAlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00499430
// ?Blit@?$RLEBlitTransZRemapXlatAlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00499610
// ?Blit_Copy_Tinted@?$RLEBlitTransZRemapXlatAlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransZRemapXlatAlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00499650
// ?Blit@?$RLEBlitTransLucent75AlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00499820
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent75AlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent75AlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00499860
// ?Blit@?$RLEBlitTransLucent50AlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00499A50
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent50AlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent50AlphaZReadWrite<unsigned short>;

// TEMPLATE: GAMEMD 0x00499A90
// ?Blit@?$RLEBlitTransLucent25AlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0@Z
// TEMPLATE: GAMEMD 0x00499C60
// ?Blit_Copy_Tinted@?$RLEBlitTransLucent25AlphaZReadWrite@G@@UBEXPAXPBXHHH00HH0G@Z
template class RLEBlitTransLucent25AlphaZReadWrite<unsigned short>;


// SYNTHETIC: GAMEMD 0x00499CA0
// ??_G?$BlitTransXlat@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499CC0
// ??_GBlitter@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499CE0
// ??_G?$BlitPlainXlat@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499D00
// ??_G?$BlitTransXlat@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499D20
// ??_G?$BlitTransZRemapXlat@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499D40
// ??_G?$BlitTransRemapDest@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499D60
// ??_G?$BlitTransRemapXlat@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499D80
// ??_G?$RLEBlitTransXlat@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499DA0
// ??_GRLEBlitter@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499DC0
// ??_G?$RLEBlitTransZRemapXlat@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499DE0
// ??_G?$RLEBlitTransRemapDest@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499E00
// ??_G?$RLEBlitTransRemapXlat@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499E20
// ??_G?$RLEBlitTransXlatZRead@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499E40
// ??_G?$RLEBlitTransZRemapXlatZRead@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499E60
// ??_G?$RLEBlitTransRemapDestZRead@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499E80
// ??_G?$RLEBlitTransRemapXlatZRead@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499EA0
// ??_G?$RLEBlitTransXlatZReadWrite@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499EC0
// ??_G?$RLEBlitTransZRemapXlatZReadWrite@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499EE0
// ??_G?$RLEBlitTransRemapDestZReadWrite@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499F00
// ??_G?$RLEBlitTransRemapXlatZReadWrite@E@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499F20
// ??_G?$BlitPlainXlat@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499F40
// ??_G?$BlitTransZRemapXlat@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499F60
// ??_G?$BlitTransDarken@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499F80
// ??_G?$BlitTransLucent75@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499FA0
// ??_G?$BlitTransLucent50@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499FC0
// ??_G?$BlitTransLucent25@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00499FE0
// ??_G?$BlitPlainXlatZRead@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A000
// ??_G?$BlitTransXlatZRead@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A020
// ??_G?$BlitTransZRemapXlatZRead@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A040
// ??_G?$BlitTransDarkenZRead@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A060
// ??_G?$BlitTransLucent75ZRead@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A080
// ??_G?$BlitTransLucent50ZRead@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A0A0
// ??_G?$BlitTransLucent25ZRead@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A0C0
// ??_G?$BlitTransLucent75ZReadWarp@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A0E0
// ??_G?$BlitTransLucent50ZReadWarp@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A100
// ??_G?$BlitTransLucent25ZReadWarp@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A120
// ??_G?$BlitPlainXlatZReadWrite@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A140
// ??_G?$BlitTransXlatZReadWrite@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A160
// ??_G?$BlitTransZRemapXlatZReadWrite@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A180
// ??_G?$BlitTransDarkenZReadWrite@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A1A0
// ??_G?$BlitTransLucent75ZReadWrite@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A1C0
// ??_G?$BlitTransLucent50ZReadWrite@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A1E0
// ??_G?$BlitTransLucent25ZReadWrite@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A200
// ??_G?$BlitPlainXlatAlpha@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A240
// ??_G?$BlitTransXlatAlpha@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0049A280
// ??_G?$BlitTransZRemapXlatAlpha@G@@UAEPAXI@Z
