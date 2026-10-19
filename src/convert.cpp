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
template class BlitTransDarkenZRead<unsigned short>;
