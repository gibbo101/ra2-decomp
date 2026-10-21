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

// FUNCTION: 0x00491390 ?$DynamicVectorClass@PAVConvertClass@@::Clear
template class DynamicVectorClass<ConvertClass *>;

// FUNCTION: 0x004913C0 ?$VectorClass@PAVConvertClass@@::ID
// FUNCTION: 0x004913E0 ?$VectorClass@PAVConvertClass@@::GetItem

// FUNCTION: 0x004913F0 ?$BlitPlainXlat@E::BlitForward
// FUNCTION: 0x00491430 ?$BlitPlainXlat@E::BlitBackward
// FUNCTION: 0x00491460 ?$BlitPlainXlat@E::Blit_Copy_Tinted
// FUNCTION: 0x00491490 ?$BlitPlainXlat@E::Blit_Move_Tinted
template class BlitPlainXlat<unsigned char>;

// FUNCTION: 0x004914C0 ?$BlitTransXlat@E::BlitForward
// FUNCTION: 0x00491500 ?$BlitTransXlat@E::BlitBackward
// FUNCTION: 0x00491530 ?$BlitTransXlat@E::Blit_Copy_Tinted
// FUNCTION: 0x00491560 ?$BlitTransXlat@E::Blit_Move_Tinted
template class BlitTransXlat<unsigned char>;

// FUNCTION: 0x00491590 ?$BlitTransZRemapXlat@E::BlitForward
// FUNCTION: 0x004915E0 ?$BlitTransZRemapXlat@E::BlitBackward
// FUNCTION: 0x00491610 ?$BlitTransZRemapXlat@E::Blit_Copy_Tinted
// FUNCTION: 0x00491640 ?$BlitTransZRemapXlat@E::Blit_Move_Tinted
template class BlitTransZRemapXlat<unsigned char>;

// FUNCTION: 0x00491670 ?$BlitTransRemapDest@E::BlitForward
// FUNCTION: 0x004916B0 ?$BlitTransRemapDest@E::BlitBackward
// FUNCTION: 0x004916E0 ?$BlitTransRemapDest@E::Blit_Copy_Tinted
// FUNCTION: 0x00491710 ?$BlitTransRemapDest@E::Blit_Move_Tinted
template class BlitTransRemapDest<unsigned char>;

// FUNCTION: 0x00491740 ?$BlitTransRemapXlat@E::BlitForward
// FUNCTION: 0x00491790 ?$BlitTransRemapXlat@E::BlitBackward
// FUNCTION: 0x004917C0 ?$BlitTransRemapXlat@E::Blit_Copy_Tinted
// FUNCTION: 0x004917F0 ?$BlitTransRemapXlat@E::Blit_Move_Tinted
template class BlitTransRemapXlat<unsigned char>;

// FUNCTION: 0x00491820 ?$RLEBlitTransXlat@E::Blit
// FUNCTION: 0x004918A0 ?$RLEBlitTransXlat@E::Blit_Copy_Tinted
template class RLEBlitTransXlat<unsigned char>;

// FUNCTION: 0x004918E0 ?$RLEBlitTransZRemapXlat@E::Blit
// FUNCTION: 0x00491970 ?$RLEBlitTransZRemapXlat@E::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlat<unsigned char>;

// FUNCTION: 0x004919B0 ?$RLEBlitTransRemapDest@E::Blit
// FUNCTION: 0x00491A30 ?$RLEBlitTransRemapDest@E::Blit_Copy_Tinted
template class RLEBlitTransRemapDest<unsigned char>;

// FUNCTION: 0x00491A70 ?$RLEBlitTransRemapXlat@E::Blit
// FUNCTION: 0x00491B00 ?$RLEBlitTransRemapXlat@E::Blit_Copy_Tinted
template class RLEBlitTransRemapXlat<unsigned char>;

// FUNCTION: 0x00491B40 ?$RLEBlitTransXlatZRead@E::Blit
// FUNCTION: 0x00491C10 ?$RLEBlitTransXlatZRead@E::Blit_Copy_Tinted
template class RLEBlitTransXlatZRead<unsigned char>;

// FUNCTION: 0x00491C50 ?$RLEBlitTransZRemapXlatZRead@E::Blit
// FUNCTION: 0x00491D50 ?$RLEBlitTransZRemapXlatZRead@E::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlatZRead<unsigned char>;

// FUNCTION: 0x00491D90 ?$RLEBlitTransRemapDestZRead@E::Blit
template class RLEBlitTransRemapDestZRead<unsigned char>;

// FUNCTION: 0x00491E60 ?$RLEBlitTransRemapDestZRead@E::Blit_Copy_Tinted

// FUNCTION: 0x00491EA0 ?$RLEBlitTransRemapXlatZRead@E::Blit
// FUNCTION: 0x00491F90 ?$RLEBlitTransRemapXlatZRead@E::Blit_Copy_Tinted
template class RLEBlitTransRemapXlatZRead<unsigned char>;

// FUNCTION: 0x00491FD0 ?$RLEBlitTransXlatZReadWrite@E::Blit
// FUNCTION: 0x004920C0 ?$RLEBlitTransXlatZReadWrite@E::Blit_Copy_Tinted
template class RLEBlitTransXlatZReadWrite<unsigned char>;

// FUNCTION: 0x00492100 ?$RLEBlitTransZRemapXlatZReadWrite@E::Blit
// FUNCTION: 0x00492210 ?$RLEBlitTransZRemapXlatZReadWrite@E::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlatZReadWrite<unsigned char>;

// FUNCTION: 0x00492250 ?$RLEBlitTransRemapDestZReadWrite@E::Blit
// FUNCTION: 0x00492330 ?$RLEBlitTransRemapDestZReadWrite@E::Blit_Copy_Tinted
template class RLEBlitTransRemapDestZReadWrite<unsigned char>;

// FUNCTION: 0x00492370 ?$RLEBlitTransRemapXlatZReadWrite@E::Blit
// FUNCTION: 0x00492480 ?$RLEBlitTransRemapXlatZReadWrite@E::Blit_Copy_Tinted
template class RLEBlitTransRemapXlatZReadWrite<unsigned char>;

// FUNCTION: 0x004924C0 ?$BlitPlainXlat@G::BlitForward
// FUNCTION: 0x00492500 ?$BlitPlainXlat@G::BlitBackward
// FUNCTION: 0x00492530 ?$BlitPlainXlat@G::Blit_Copy_Tinted
// FUNCTION: 0x00492560 ?$BlitPlainXlat@G::Blit_Move_Tinted
template class BlitPlainXlat<unsigned short>;

// FUNCTION: 0x00492590 ?$BlitTransZRemapXlat@G::BlitForward
// FUNCTION: 0x004925E0 ?$BlitTransZRemapXlat@G::BlitBackward
// FUNCTION: 0x00492610 ?$BlitTransZRemapXlat@G::Blit_Copy_Tinted
// FUNCTION: 0x00492640 ?$BlitTransZRemapXlat@G::Blit_Move_Tinted
template class BlitTransZRemapXlat<unsigned short>;

// FUNCTION: 0x00492670 ?$BlitTransDarken@G::BlitForward
// FUNCTION: 0x004926B0 ?$BlitTransDarken@G::BlitBackward
// FUNCTION: 0x004926E0 ?$BlitTransDarken@G::Blit_Copy_Tinted
// FUNCTION: 0x00492710 ?$BlitTransDarken@G::Blit_Move_Tinted
template class BlitTransDarken<unsigned short>;

// FUNCTION: 0x00492740 ?$BlitTransLucent75@G::BlitForward
// FUNCTION: 0x004927A0 ?$BlitTransLucent75@G::BlitBackward
// FUNCTION: 0x004927D0 ?$BlitTransLucent75@G::Blit_Copy_Tinted
// FUNCTION: 0x00492800 ?$BlitTransLucent75@G::Blit_Move_Tinted
template class BlitTransLucent75<unsigned short>;

// FUNCTION: 0x00492890 ?$BlitTransLucent50@G::BlitBackward
// FUNCTION: 0x004928C0 ?$BlitTransLucent50@G::Blit_Copy_Tinted
// FUNCTION: 0x004928F0 ?$BlitTransLucent50@G::Blit_Move_Tinted
template class BlitTransLucent50<unsigned short>;

// FUNCTION: 0x00492920 ?$BlitTransLucent25@G::BlitForward
// FUNCTION: 0x00492980 ?$BlitTransLucent25@G::BlitBackward
// FUNCTION: 0x004929B0 ?$BlitTransLucent25@G::Blit_Copy_Tinted
// FUNCTION: 0x004929E0 ?$BlitTransLucent25@G::Blit_Move_Tinted
template class BlitTransLucent25<unsigned short>;

// FUNCTION: 0x00492A10 ?$BlitPlainXlatZRead@G::BlitForward
// FUNCTION: 0x00492A70 ?$BlitPlainXlatZRead@G::BlitBackward
// FUNCTION: 0x00492AA0 ?$BlitPlainXlatZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00492AD0 ?$BlitPlainXlatZRead@G::Blit_Move_Tinted
template class BlitPlainXlatZRead<unsigned short>;

// FUNCTION: 0x00492B00 ?$BlitTransXlatZRead@G::BlitForward
// FUNCTION: 0x00492B70 ?$BlitTransXlatZRead@G::BlitBackward
// FUNCTION: 0x00492BA0 ?$BlitTransXlatZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00492BD0 ?$BlitTransXlatZRead@G::Blit_Move_Tinted
template class BlitTransXlatZRead<unsigned short>;

// FUNCTION: 0x00492C00 ?$BlitTransZRemapXlatZRead@G::BlitForward
// FUNCTION: 0x00492C90 ?$BlitTransZRemapXlatZRead@G::BlitBackward
// FUNCTION: 0x00492CC0 ?$BlitTransZRemapXlatZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00492CF0 ?$BlitTransZRemapXlatZRead@G::Blit_Move_Tinted
template class BlitTransZRemapXlatZRead<unsigned short>;

// FUNCTION: 0x00492D20 ?$BlitTransDarkenZRead@G::BlitForward
// FUNCTION: 0x00492D80 ?$BlitTransDarkenZRead@G::BlitBackward
// FUNCTION: 0x00492DB0 ?$BlitTransDarkenZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00492DE0 ?$BlitTransDarkenZRead@G::Blit_Move_Tinted
template class BlitTransDarkenZRead<unsigned short>;

// FUNCTION: 0x00492E10 ?$BlitTransLucent75ZRead@G::BlitForward
// FUNCTION: 0x00492EA0 ?$BlitTransLucent75ZRead@G::BlitBackward
// FUNCTION: 0x00492ED0 ?$BlitTransLucent75ZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00492F00 ?$BlitTransLucent75ZRead@G::Blit_Move_Tinted
template class BlitTransLucent75ZRead<unsigned short>;

// FUNCTION: 0x00492F30 ?$BlitTransLucent50ZRead@G::BlitForward
// FUNCTION: 0x00492FC0 ?$BlitTransLucent50ZRead@G::BlitBackward
// FUNCTION: 0x00492FF0 ?$BlitTransLucent50ZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00493020 ?$BlitTransLucent50ZRead@G::Blit_Move_Tinted
template class BlitTransLucent50ZRead<unsigned short>;

// FUNCTION: 0x00493050 ?$BlitTransLucent25ZRead@G::BlitForward
// FUNCTION: 0x004930E0 ?$BlitTransLucent25ZRead@G::BlitBackward
// FUNCTION: 0x00493110 ?$BlitTransLucent25ZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00493140 ?$BlitTransLucent25ZRead@G::Blit_Move_Tinted
template class BlitTransLucent25ZRead<unsigned short>;

// FUNCTION: 0x00493170 ?$BlitTransLucent75ZReadWarp@G::BlitForward
// FUNCTION: 0x00493210 ?$BlitTransLucent75ZReadWarp@G::BlitBackward
// FUNCTION: 0x00493240 ?$BlitTransLucent75ZReadWarp@G::Blit_Copy_Tinted
// FUNCTION: 0x00493270 ?$BlitTransLucent75ZReadWarp@G::Blit_Move_Tinted
template class BlitTransLucent75ZReadWarp<unsigned short>;

// FUNCTION: 0x004932A0 ?$BlitTransLucent50ZReadWarp@G::BlitForward
// FUNCTION: 0x00493340 ?$BlitTransLucent50ZReadWarp@G::BlitBackward
// FUNCTION: 0x00493370 ?$BlitTransLucent50ZReadWarp@G::Blit_Copy_Tinted
// FUNCTION: 0x004933A0 ?$BlitTransLucent50ZReadWarp@G::Blit_Move_Tinted
template class BlitTransLucent50ZReadWarp<unsigned short>;

// FUNCTION: 0x004933D0 ?$BlitTransLucent25ZReadWarp@G::BlitForward
// FUNCTION: 0x00493470 ?$BlitTransLucent25ZReadWarp@G::BlitBackward
// FUNCTION: 0x004934A0 ?$BlitTransLucent25ZReadWarp@G::Blit_Copy_Tinted
// FUNCTION: 0x004934D0 ?$BlitTransLucent25ZReadWarp@G::Blit_Move_Tinted
template class BlitTransLucent25ZReadWarp<unsigned short>;

// FUNCTION: 0x00493500 ?$BlitPlainXlatZReadWrite@G::BlitForward
// FUNCTION: 0x00493570 ?$BlitPlainXlatZReadWrite@G::BlitBackward
// FUNCTION: 0x004935A0 ?$BlitPlainXlatZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x004935D0 ?$BlitPlainXlatZReadWrite@G::Blit_Move_Tinted
template class BlitPlainXlatZReadWrite<unsigned short>;

// FUNCTION: 0x00493600 ?$BlitTransXlatZReadWrite@G::BlitForward
// FUNCTION: 0x00493680 ?$BlitTransXlatZReadWrite@G::BlitBackward
// FUNCTION: 0x004936B0 ?$BlitTransXlatZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x004936E0 ?$BlitTransXlatZReadWrite@G::Blit_Move_Tinted
template class BlitTransXlatZReadWrite<unsigned short>;

// FUNCTION: 0x00493710 ?$BlitTransZRemapXlatZReadWrite@G::BlitForward
// FUNCTION: 0x004937A0 ?$BlitTransZRemapXlatZReadWrite@G::BlitBackward
// FUNCTION: 0x004937D0 ?$BlitTransZRemapXlatZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00493800 ?$BlitTransZRemapXlatZReadWrite@G::Blit_Move_Tinted
template class BlitTransZRemapXlatZReadWrite<unsigned short>;

// FUNCTION: 0x00493830 ?$BlitTransDarkenZReadWrite@G::BlitForward
// FUNCTION: 0x004938A0 ?$BlitTransDarkenZReadWrite@G::BlitBackward
// FUNCTION: 0x004938D0 ?$BlitTransDarkenZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00493900 ?$BlitTransDarkenZReadWrite@G::Blit_Move_Tinted
template class BlitTransDarkenZReadWrite<unsigned short>;

// FUNCTION: 0x00493930 ?$BlitTransLucent75ZReadWrite@G::BlitForward
// FUNCTION: 0x004939D0 ?$BlitTransLucent75ZReadWrite@G::BlitBackward
// FUNCTION: 0x00493A00 ?$BlitTransLucent75ZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00493A30 ?$BlitTransLucent75ZReadWrite@G::Blit_Move_Tinted
template class BlitTransLucent75ZReadWrite<unsigned short>;

// FUNCTION: 0x00493A60 ?$BlitTransLucent50ZReadWrite@G::BlitForward
// FUNCTION: 0x00493B00 ?$BlitTransLucent50ZReadWrite@G::BlitBackward
// FUNCTION: 0x00493B30 ?$BlitTransLucent50ZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00493B60 ?$BlitTransLucent50ZReadWrite@G::Blit_Move_Tinted
template class BlitTransLucent50ZReadWrite<unsigned short>;

// FUNCTION: 0x00493B90 ?$BlitTransLucent25ZReadWrite@G::BlitForward
// FUNCTION: 0x00493C30 ?$BlitTransLucent25ZReadWrite@G::BlitBackward
// FUNCTION: 0x00493C60 ?$BlitTransLucent25ZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00493C90 ?$BlitTransLucent25ZReadWrite@G::Blit_Move_Tinted
template class BlitTransLucent25ZReadWrite<unsigned short>;

// FUNCTION: 0x00493CC0 ?$BlitPlainXlatAlpha@G::BlitForward
// FUNCTION: 0x00493D60 ?$BlitPlainXlatAlpha@G::BlitBackward
// FUNCTION: 0x00493D90 ?$BlitPlainXlatAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00493DC0 ?$BlitPlainXlatAlpha@G::Blit_Move_Tinted
template class BlitPlainXlatAlpha<unsigned short>;

// FUNCTION: 0x00493DF0 ?$BlitTransXlatAlpha@G::BlitForward
// FUNCTION: 0x00493EA0 ?$BlitTransXlatAlpha@G::BlitBackward
// FUNCTION: 0x00493ED0 ?$BlitTransXlatAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00493F00 ?$BlitTransXlatAlpha@G::Blit_Move_Tinted
template class BlitTransXlatAlpha<unsigned short>;

// FUNCTION: 0x00493F30 ?$BlitTransZRemapXlatAlpha@G::BlitForward
// FUNCTION: 0x00493FF0 ?$BlitTransZRemapXlatAlpha@G::BlitBackward
// FUNCTION: 0x00494020 ?$BlitTransZRemapXlatAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00494050 ?$BlitTransZRemapXlatAlpha@G::Blit_Move_Tinted
template class BlitTransZRemapXlatAlpha<unsigned short>;

// FUNCTION: 0x00494080 ?$BlitTransLucent75Alpha@G::BlitForward
// FUNCTION: 0x00494150 ?$BlitTransLucent75Alpha@G::BlitBackward
// FUNCTION: 0x00494180 ?$BlitTransLucent75Alpha@G::Blit_Copy_Tinted
// FUNCTION: 0x004941B0 ?$BlitTransLucent75Alpha@G::Blit_Move_Tinted
template class BlitTransLucent75Alpha<unsigned short>;

// FUNCTION: 0x004941E0 ?$BlitTransLucent50Alpha@G::BlitForward
// FUNCTION: 0x004942A0 ?$BlitTransLucent50Alpha@G::BlitBackward
// FUNCTION: 0x004942D0 ?$BlitTransLucent50Alpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00494300 ?$BlitTransLucent50Alpha@G::Blit_Move_Tinted
template class BlitTransLucent50Alpha<unsigned short>;

// FUNCTION: 0x00494330 ?$BlitTransLucent25Alpha@G::BlitForward
// FUNCTION: 0x00494400 ?$BlitTransLucent25Alpha@G::BlitBackward
// FUNCTION: 0x00494430 ?$BlitTransLucent25Alpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00494460 ?$BlitTransLucent25Alpha@G::Blit_Move_Tinted
template class BlitTransLucent25Alpha<unsigned short>;

// FUNCTION: 0x00494490 ?$BlitTransXlatWriteAlpha@G::BlitForward
// FUNCTION: 0x00494500 ?$BlitTransXlatWriteAlpha@G::BlitBackward
// FUNCTION: 0x00494530 ?$BlitTransXlatWriteAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00494560 ?$BlitTransXlatWriteAlpha@G::Blit_Move_Tinted
template class BlitTransXlatWriteAlpha<unsigned short>;

// FUNCTION: 0x00494590 ?$BlitTransXlatMultWriteAlpha@G::BlitForward
// FUNCTION: 0x00494600 ?$BlitTransXlatMultWriteAlpha@G::BlitBackward
// FUNCTION: 0x00494630 ?$BlitTransXlatMultWriteAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00494660 ?$BlitTransXlatMultWriteAlpha@G::Blit_Move_Tinted
template class BlitTransXlatMultWriteAlpha<unsigned short>;

// FUNCTION: 0x00494690 ?$BlitTranslucentWriteAlpha@G::BlitForward
// FUNCTION: 0x00494890 ?$BlitTranslucentWriteAlpha@G::BlitBackward
// FUNCTION: 0x004948C0 ?$BlitTranslucentWriteAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x004948F0 ?$BlitTranslucentWriteAlpha@G::Blit_Move_Tinted
template class BlitTranslucentWriteAlpha<unsigned short>;

// FUNCTION: 0x00494920 ?$BlitTranslucent50NonzeroAlpha@G::BlitForward
// FUNCTION: 0x004949B0 ?$BlitTranslucent50NonzeroAlpha@G::BlitBackward
// FUNCTION: 0x004949E0 ?$BlitTranslucent50NonzeroAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00494A10 ?$BlitTranslucent50NonzeroAlpha@G::Blit_Move_Tinted
template class BlitTranslucent50NonzeroAlpha<unsigned short>;

// FUNCTION: 0x00494A40 ?$BlitTranslucent50ZeroAlpha@G::BlitForward
// FUNCTION: 0x00494AD0 ?$BlitTranslucent50ZeroAlpha@G::BlitBackward
// FUNCTION: 0x00494B00 ?$BlitTranslucent50ZeroAlpha@G::Blit_Copy_Tinted
// FUNCTION: 0x00494B30 ?$BlitTranslucent50ZeroAlpha@G::Blit_Move_Tinted
template class BlitTranslucent50ZeroAlpha<unsigned short>;

// FUNCTION: 0x00494B60 ?$BlitTransXlatAlphaZRead@G::BlitForward
// FUNCTION: 0x00494C40 ?$BlitTransXlatAlphaZRead@G::Blit_Copy_Tinted
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

// FUNCTION: 0x00494D20 ?$BlitTransXlatAlphaZRead@G::BlitBackward
// FUNCTION: 0x00494D50 ?$BlitTransXlatAlphaZRead@G::Blit_Move_Tinted
template class BlitTransXlatAlphaZRead<unsigned short>;

// FUNCTION: 0x00494D90 ?$BlitTransZRemapXlatAlphaZRead@G::BlitForward
// FUNCTION: 0x00494E90 ?$BlitTransZRemapXlatAlphaZRead@G::BlitBackward
// FUNCTION: 0x00494EC0 ?$BlitTransZRemapXlatAlphaZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00494EF0 ?$BlitTransZRemapXlatAlphaZRead@G::Blit_Move_Tinted
template class BlitTransZRemapXlatAlphaZRead<unsigned short>;

// FUNCTION: 0x00494F20 ?$BlitTransLucent75AlphaZRead@G::BlitForward
// FUNCTION: 0x00495030 ?$BlitTransLucent75AlphaZRead@G::BlitBackward
// FUNCTION: 0x00495060 ?$BlitTransLucent75AlphaZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00495090 ?$BlitTransLucent75AlphaZRead@G::Blit_Move_Tinted
template class BlitTransLucent75AlphaZRead<unsigned short>;

// FUNCTION: 0x004950C0 ?$BlitTransLucent50AlphaZRead@G::BlitForward
// FUNCTION: 0x004951C0 ?$BlitTransLucent50AlphaZRead@G::BlitBackward
// FUNCTION: 0x004951F0 ?$BlitTransLucent50AlphaZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x00495220 ?$BlitTransLucent50AlphaZRead@G::Blit_Move_Tinted
template class BlitTransLucent50AlphaZRead<unsigned short>;

// FUNCTION: 0x00495250 ?$BlitTransLucent25AlphaZRead@G::BlitForward
// FUNCTION: 0x00495360 ?$BlitTransLucent25AlphaZRead@G::BlitBackward
// FUNCTION: 0x00495390 ?$BlitTransLucent25AlphaZRead@G::Blit_Copy_Tinted
// FUNCTION: 0x004953C0 ?$BlitTransLucent25AlphaZRead@G::Blit_Move_Tinted
template class BlitTransLucent25AlphaZRead<unsigned short>;

// FUNCTION: 0x004953F0 ?$BlitTransLucent75AlphaZReadWarp@G::BlitForward
// FUNCTION: 0x00495500 ?$BlitTransLucent75AlphaZReadWarp@G::BlitBackward
// FUNCTION: 0x00495530 ?$BlitTransLucent75AlphaZReadWarp@G::Blit_Copy_Tinted
// FUNCTION: 0x00495560 ?$BlitTransLucent75AlphaZReadWarp@G::Blit_Move_Tinted
template class BlitTransLucent75AlphaZReadWarp<unsigned short>;

// FUNCTION: 0x00495590 ?$BlitTransLucent50AlphaZReadWarp@G::BlitForward
// FUNCTION: 0x004956A0 ?$BlitTransLucent50AlphaZReadWarp@G::BlitBackward
// FUNCTION: 0x004956D0 ?$BlitTransLucent50AlphaZReadWarp@G::Blit_Copy_Tinted
// FUNCTION: 0x00495700 ?$BlitTransLucent50AlphaZReadWarp@G::Blit_Move_Tinted
template class BlitTransLucent50AlphaZReadWarp<unsigned short>;

// FUNCTION: 0x00495730 ?$BlitTransLucent25AlphaZReadWarp@G::BlitForward
// FUNCTION: 0x00495840 ?$BlitTransLucent25AlphaZReadWarp@G::BlitBackward
// FUNCTION: 0x00495870 ?$BlitTransLucent25AlphaZReadWarp@G::Blit_Copy_Tinted
// FUNCTION: 0x004958A0 ?$BlitTransLucent25AlphaZReadWarp@G::Blit_Move_Tinted
template class BlitTransLucent25AlphaZReadWarp<unsigned short>;

// FUNCTION: 0x004958D0 ?$BlitTransXlatAlphaZReadWrite@G::BlitForward
// FUNCTION: 0x004959C0 ?$BlitTransXlatAlphaZReadWrite@G::BlitBackward
// FUNCTION: 0x004959F0 ?$BlitTransXlatAlphaZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00495A20 ?$BlitTransXlatAlphaZReadWrite@G::Blit_Move_Tinted
template class BlitTransXlatAlphaZReadWrite<unsigned short>;

// FUNCTION: 0x00495A50 ?$BlitTransZRemapXlatAlphaZReadWrite@G::BlitForward
// FUNCTION: 0x00495B30 ?$BlitTransZRemapXlatAlphaZReadWrite@G::BlitBackward
// FUNCTION: 0x00495B60 ?$BlitTransZRemapXlatAlphaZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00495B90 ?$BlitTransZRemapXlatAlphaZReadWrite@G::Blit_Move_Tinted
template class BlitTransZRemapXlatAlphaZReadWrite<unsigned short>;

// FUNCTION: 0x00495BC0 ?$BlitTransLucent75AlphaZReadWrite@G::BlitForward
// FUNCTION: 0x00495CD0 ?$BlitTransLucent75AlphaZReadWrite@G::BlitBackward
// FUNCTION: 0x00495D00 ?$BlitTransLucent75AlphaZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00495D30 ?$BlitTransLucent75AlphaZReadWrite@G::Blit_Move_Tinted
template class BlitTransLucent75AlphaZReadWrite<unsigned short>;

// FUNCTION: 0x00495D60 ?$BlitTransLucent50AlphaZReadWrite@G::BlitForward
// FUNCTION: 0x00495E70 ?$BlitTransLucent50AlphaZReadWrite@G::BlitBackward
// FUNCTION: 0x00495EA0 ?$BlitTransLucent50AlphaZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00495ED0 ?$BlitTransLucent50AlphaZReadWrite@G::Blit_Move_Tinted
template class BlitTransLucent50AlphaZReadWrite<unsigned short>;

// FUNCTION: 0x00495F00 ?$BlitTransLucent25AlphaZReadWrite@G::BlitForward
// FUNCTION: 0x00496010 ?$BlitTransLucent25AlphaZReadWrite@G::BlitBackward
// FUNCTION: 0x00496040 ?$BlitTransLucent25AlphaZReadWrite@G::Blit_Copy_Tinted
// FUNCTION: 0x00496070 ?$BlitTransLucent25AlphaZReadWrite@G::Blit_Move_Tinted
template class BlitTransLucent25AlphaZReadWrite<unsigned short>;

// FUNCTION: 0x004960A0 ?$RLEBlitTransXlat@G::Blit
// FUNCTION: 0x00496130 ?$RLEBlitTransXlat@G::Blit_Copy_Tinted
template class RLEBlitTransXlat<unsigned short>;

// FUNCTION: 0x00496170 ?$RLEBlitTransZRemapXlat@G::Blit
// FUNCTION: 0x00496210 ?$RLEBlitTransZRemapXlat@G::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlat<unsigned short>;

// FUNCTION: 0x00496250 ?$RLEBlitTransDarken@G::Blit
// FUNCTION: 0x004962D0 ?$RLEBlitTransDarken@G::Blit_Copy_Tinted
template class RLEBlitTransDarken<unsigned short>;

// FUNCTION: 0x00496310 ?$RLEBlitTransLucent75@G::Blit
// FUNCTION: 0x004963B0 ?$RLEBlitTransLucent75@G::Blit_Copy_Tinted
template class RLEBlitTransLucent75<unsigned short>;

// FUNCTION: 0x004963F0 ?$RLEBlitTransLucent50@G::Blit
// FUNCTION: 0x00496490 ?$RLEBlitTransLucent50@G::Blit_Copy_Tinted
template class RLEBlitTransLucent50<unsigned short>;

// FUNCTION: 0x004964D0 ?$RLEBlitTransLucent25@G::Blit
// FUNCTION: 0x00496570 ?$RLEBlitTransLucent25@G::Blit_Copy_Tinted
template class RLEBlitTransLucent25<unsigned short>;

// FUNCTION: 0x004965B0 ?$RLEBlitTransXlatZRead@G::Blit
// FUNCTION: 0x00496690 ?$RLEBlitTransXlatZRead@G::Blit_Copy_Tinted
template class RLEBlitTransXlatZRead<unsigned short>;

// FUNCTION: 0x004966D0 ?$RLEBlitTransZRemapXlatZRead@G::Blit
// FUNCTION: 0x004967E0 ?$RLEBlitTransZRemapXlatZRead@G::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlatZRead<unsigned short>;

// FUNCTION: 0x00496820 ?$RLEBlitTransDarkenZRead@G::Blit
// FUNCTION: 0x004968F0 ?$RLEBlitTransDarkenZRead@G::Blit_Copy_Tinted
template class RLEBlitTransDarkenZRead<unsigned short>;

// FUNCTION: 0x00496930 ?$RLEBlitTransLucent75ZRead@G::Blit
// FUNCTION: 0x00496A40 ?$RLEBlitTransLucent75ZRead@G::Blit_Copy_Tinted
template class RLEBlitTransLucent75ZRead<unsigned short>;

// FUNCTION: 0x00496A80 ?$RLEBlitTransLucent50ZRead@G::Blit
// FUNCTION: 0x00496B80 ?$RLEBlitTransLucent50ZRead@G::Blit_Copy_Tinted
template class RLEBlitTransLucent50ZRead<unsigned short>;

// FUNCTION: 0x00496BC0 ?$RLEBlitTransLucent25ZRead@G::Blit
// FUNCTION: 0x00496CD0 ?$RLEBlitTransLucent25ZRead@G::Blit_Copy_Tinted
template class RLEBlitTransLucent25ZRead<unsigned short>;

// FUNCTION: 0x00496D10 ?$RLEBlitTransLucent75ZReadWarp@G::Blit
// FUNCTION: 0x00496E20 ?$RLEBlitTransLucent75ZReadWarp@G::Blit_Copy_Tinted
template class RLEBlitTransLucent75ZReadWarp<unsigned short>;

// FUNCTION: 0x00496E60 ?$RLEBlitTransLucent50ZReadWarp@G::Blit
// FUNCTION: 0x00496F70 ?$RLEBlitTransLucent50ZReadWarp@G::Blit_Copy_Tinted
template class RLEBlitTransLucent50ZReadWarp<unsigned short>;

// FUNCTION: 0x00496FB0 ?$RLEBlitTransLucent25ZReadWarp@G::Blit
// FUNCTION: 0x004970C0 ?$RLEBlitTransLucent25ZReadWarp@G::Blit_Copy_Tinted
template class RLEBlitTransLucent25ZReadWarp<unsigned short>;

// FUNCTION: 0x00497100 ?$RLEBlitTransXlatZReadWrite@G::Blit
// FUNCTION: 0x004971F0 ?$RLEBlitTransXlatZReadWrite@G::Blit_Copy_Tinted
template class RLEBlitTransXlatZReadWrite<unsigned short>;

// FUNCTION: 0x00497230 ?$RLEBlitTransZRemapXlatZReadWrite@G::Blit
// FUNCTION: 0x00497350 ?$RLEBlitTransZRemapXlatZReadWrite@G::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlatZReadWrite<unsigned short>;

// FUNCTION: 0x00497390 ?$RLEBlitTransDarkenZReadWrite@G::Blit
// FUNCTION: 0x00497470 ?$RLEBlitTransDarkenZReadWrite@G::Blit_Copy_Tinted
template class RLEBlitTransDarkenZReadWrite<unsigned short>;

// FUNCTION: 0x004974B0 ?$RLEBlitTransLucent75ZReadWrite@G::Blit
// FUNCTION: 0x004975D0 ?$RLEBlitTransLucent75ZReadWrite@G::Blit_Copy_Tinted
template class RLEBlitTransLucent75ZReadWrite<unsigned short>;

// FUNCTION: 0x00497610 ?$RLEBlitTransLucent50ZReadWrite@G::Blit
// FUNCTION: 0x00497720 ?$RLEBlitTransLucent50ZReadWrite@G::Blit_Copy_Tinted
template class RLEBlitTransLucent50ZReadWrite<unsigned short>;

// FUNCTION: 0x00497760 ?$RLEBlitTransLucent25ZReadWrite@G::Blit
// FUNCTION: 0x00497880 ?$RLEBlitTransLucent25ZReadWrite@G::Blit_Copy_Tinted
template class RLEBlitTransLucent25ZReadWrite<unsigned short>;

// FUNCTION: 0x004978C0 ?$RLEBlitTransXlatAlpha@G::Blit
// FUNCTION: 0x004979D0 ?$RLEBlitTransXlatAlpha@G::Blit_Copy_Tinted
template class RLEBlitTransXlatAlpha<unsigned short>;

// FUNCTION: 0x00497A10 ?$RLEBlitTransZRemapXlatAlpha@G::Blit
// FUNCTION: 0x00497B40 ?$RLEBlitTransZRemapXlatAlpha@G::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlatAlpha<unsigned short>;

// FUNCTION: 0x00497B80 ?$RLEBlitTransLucent75Alpha@G::Blit
// FUNCTION: 0x00497CB0 ?$RLEBlitTransLucent75Alpha@G::Blit_Copy_Tinted
template class RLEBlitTransLucent75Alpha<unsigned short>;

// FUNCTION: 0x00497CF0 ?$RLEBlitTransLucent50Alpha@G::Blit
// FUNCTION: 0x00497E20 ?$RLEBlitTransLucent50Alpha@G::Blit_Copy_Tinted
template class RLEBlitTransLucent50Alpha<unsigned short>;

// FUNCTION: 0x00497E60 ?$RLEBlitTransLucent25Alpha@G::Blit
// FUNCTION: 0x00497F90 ?$RLEBlitTransLucent25Alpha@G::Blit_Copy_Tinted
template class RLEBlitTransLucent25Alpha<unsigned short>;

// FUNCTION: 0x00497FD0 ?$RLEBlitTransXlatAlphaZRead@G::Blit
// FUNCTION: 0x00498150 ?$RLEBlitTransXlatAlphaZRead@G::Blit_Copy_Tinted
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

// FUNCTION: 0x004982E0 ?$RLEBlitTransZRemapXlatAlphaZRead@G::Blit
// FUNCTION: 0x00498490 ?$RLEBlitTransZRemapXlatAlphaZRead@G::Blit_Copy_Tinted
template class RLEBlitTransZRemapXlatAlphaZRead<unsigned short>;

// FUNCTION: 0x004984D0 ?$RLEBlitTransLucent75AlphaZRead@G::Blit
// FUNCTION: 0x00498690 ?$RLEBlitTransLucent75AlphaZRead@G::Blit_Copy_Tinted
template class RLEBlitTransLucent75AlphaZRead<unsigned short>;

// FUNCTION: 0x004986D0 ?$RLEBlitTransLucent50AlphaZRead@G::Blit
// FUNCTION: 0x00498880 ?$RLEBlitTransLucent50AlphaZRead@G::Blit_Copy_Tinted
template class RLEBlitTransLucent50AlphaZRead<unsigned short>;

// FUNCTION: 0x004988C0 ?$RLEBlitTransLucent25AlphaZRead@G::Blit
// FUNCTION: 0x00498A80 ?$RLEBlitTransLucent25AlphaZRead@G::Blit_Copy_Tinted
template class RLEBlitTransLucent25AlphaZRead<unsigned short>;

// FUNCTION: 0x00498AC0 ?$RLEBlitTransLucent75AlphaZReadWarp@G::Blit
// FUNCTION: 0x00498C90 ?$RLEBlitTransLucent75AlphaZReadWarp@G::Blit_Copy_Tinted
template class RLEBlitTransLucent75AlphaZReadWarp<unsigned short>;

// FUNCTION: 0x00498CD0 ?$RLEBlitTransLucent50AlphaZReadWarp@G::Blit
template class RLEBlitTransLucent50AlphaZReadWarp<unsigned short>;
