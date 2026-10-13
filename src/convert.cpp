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

// FUNCTION: 0x0048E6A0 $E29
// FUNCTION: 0x0048E6E0 $E26

// FUNCTION: 0x0048E740
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

// FUNCTION: 0x00490400 ConvertClass::~ConvertClass
// FUNCTION: 0x00491210 ConvertClass::~ConvertClass
ConvertClass::~ConvertClass(void)
{
	Destroy_Blitters();

	delete [] IntensityTranslator;
	IntensityTranslator = NULL;

	delete [] ShadowTable;
	ShadowTable = NULL;

	Drawers.Delete(this);
}

// FUNCTION: 0x00490B90
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

// FUNCTION: 0x00490E50
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

	// This should be fixed to return the RLEPlainBlitter when one is available
	// but if you need to use this in the mean time just don't RLE compress the
	// shape (since it only compresses transparent pixels and the reason we compress
	// them is so we can skip them easily.)
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

// FUNCTION: 0x00491100
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

// FUNCTION: 0x004912B0 ?$BlitTransXlat@G::BlitForward
// FUNCTION: 0x00491300 ?$BlitTransXlat@G::BlitBackward
// FUNCTION: 0x00491330 ?$BlitTransXlat@G::Blit_Copy_Tinted
// FUNCTION: 0x00491360 ?$BlitTransXlat@G::Blit_Move_Tinted
