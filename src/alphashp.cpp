// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "_rect.h"
#include "alphashp.h"
#include "_alpha.h"
#include "_tactica.h"
#include "crc.h"
#include "globals.h"
#include "object.h"
#include "objtype.h"
#include "msanim.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "geometry.h"
#include "tracker.h"
#include "vector.h"

// SYNTHETIC: GAMEMD 0x004206D0 SYMBOL
// _$E2

static bool BrightnessCalculated = false;

#include <math.h>

// SYNTHETIC: GAMEMD 0x004206F0 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x00420720 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x00420740 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x00420760 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x00420780 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x004207A0 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x004207C0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x00420800 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x00420830 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x00420860 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x00420890 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x004208A0 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x004208C0 SYMBOL
// _$E52

// SYNTHETIC: GAMEMD 0x004208E0 SYMBOL
// _$E57
// SYNTHETIC: GAMEMD 0x00420920 SYMBOL
// _$E55
DynamicVectorClass<AlphaShapeClass *> AlphaShapes;

unsigned char AlphaShapeClass::BrightnessTable[256][256];

// FUNCTION: GAMEMD 0x00420960
AlphaShapeClass::AlphaShapeClass(ObjectClass * owner, int x, int y) :
	BASECLASS(),
	Owner(owner),
	ImageData(NULL),
	IsToDelete(false)
{
	SHPStruct const * image = (SHPStruct const *)owner->Class_Of()->AlphaImageData;
	ImageData = image;
	DrawRect = Rect(x, y, image->Get_Width(), image->Get_Height());
	AlphaShapes.Add(this);
	ObjectPtrTracker.Add(this);

	if (!BrightnessCalculated) {
		Calculate_Brightness_Table();
	}
}

// FUNCTION: GAMEMD 0x00420AF0
AlphaShapeClass::AlphaShapeClass(void) :
	BASECLASS(),
	Owner(NULL),
	DrawRect(0, 0, 0, 0),
	ImageData(NULL),
	IsToDelete(false)
{
	AlphaShapes.Add(this);
	ObjectPtrTracker.Add(this);

	if (!BrightnessCalculated) {
		Calculate_Brightness_Table();
	}
}

// FUNCTION: GAMEMD 0x00420C50
AlphaShapeClass::AlphaShapeClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x00420C80
AlphaShapeClass::~AlphaShapeClass(void)
{
	AlphaShapes.Delete(this);
	ObjectPtrTracker.Delete(this);
}

// FUNCTION: GAMEMD 0x00420D40
HRESULT STDMETHODCALLTYPE AlphaShapeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AlphaShapeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00420D80
RTTIType AlphaShapeClass::Fetch_RTTI(void) const
{
	return(RTTI_ALPHASHAPE);
}

// FUNCTION: GAMEMD 0x00420D90
int AlphaShapeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00420DA0
void AlphaShapeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(DrawRect.X);
	crc(DrawRect.Y);
	crc(DrawRect.Width);
	crc(DrawRect.Height);
}

// FUNCTION: GAMEMD 0x00420DE0
HRESULT STDMETHODCALLTYPE AlphaShapeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) AlphaShapeClass(NoInitClass());

		Swizzle_Pointer(&Owner);
		ImageData = NULL;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00420E40
HRESULT STDMETHODCALLTYPE AlphaShapeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		return(S_OK);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00420E70
void AlphaShapeClass::Detach(AbstractClass const * target, bool all)
{
	if (target == Owner) {
		IsToDelete = true;
	}
}

// FUNCTION: GAMEMD 0x00420E90
void AlphaShapeClass::Update_All(void)
{
	if (!BrightnessCalculated) {
		Calculate_Brightness_Table();
	}

	for (int i = AlphaShapes.Count() - 1; i >= 0; i--) {
		if (AlphaShapes[i]->IsToDelete) {
			delete AlphaShapes[i];
		}
	}
}


// FUNCTION: GAMEMD 0x00420F40
void AlphaShapeClass::Draw_In_Area(
	Point2D const &point,
	Rect const &cliprect)
{
#define ALPHA_BLOCK4 "\xDB\xDB\xDB\xDB"
	static const unsigned char tile_mask[] =
		"                                                            "
		"                            " ALPHA_BLOCK4 "                            "
		"                          " ALPHA_BLOCK4 ALPHA_BLOCK4 "                          "
		"                        " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                        "
		"                      " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                      "
		"                    " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                    "
		"                  " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                  "
		"                " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                "
		"              " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "              "
		"            " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "            "
		"          " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "          "
		"        " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "        "
		"      " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "      "
		"    " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "    "
		"  " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "  "
		ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4
		"  " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "  "
		"    " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "    "
		"      " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "      "
		"        " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "        "
		"          " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "          "
		"            " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "            "
		"              " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "              "
		"                " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                "
		"                  " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                  "
		"                    " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                    "
		"                      " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                      "
		"                        " ALPHA_BLOCK4 ALPHA_BLOCK4 ALPHA_BLOCK4 "                        "
		"                          " ALPHA_BLOCK4 ALPHA_BLOCK4 "                          "
		"                            " ALPHA_BLOCK4 "                            ";
#undef ALPHA_BLOCK4

	for (int i = 0; i < AlphaShapes.Count(); i++) {
		if (!AlphaShapes[i]->IsToDelete) {
			Rect r1 = AlphaShapes[i]->DrawRect;
			r1 -= Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
			r1 += TacticalRect.Top_Left();

			Rect r2 = Intersect(r1, cliprect);
			Rect r3(point.X, point.Y, ISO_TILE_PIXEL_W, ISO_TILE_PIXEL_H);
			Rect r4 = Intersect(r2, r3);

			if (r4.Is_Valid()) {
				Point2D offset = r4.Top_Left() - r3.Top_Left();

				Point2D origin = r1.Top_Left();
				SHPStruct const *shape = AlphaShapes[i]->ImageData;
				Rect shape_rect = shape->GetFrameBounds(0);

				int top = MAX(r4.Y, shape_rect.Y + origin.Y);
				int src_y = top - origin.Y - shape_rect.Y;
				int bottom = MIN(
					r4.Y + r4.Height,
					origin.Y + shape_rect.Height + shape_rect.Y);
				int left = MAX(r4.X, shape_rect.X + origin.X);
				int src_x = left - origin.X - shape_rect.X;
				int right = MIN(
					r4.X + r4.Width,
					origin.X + shape_rect.Width + shape_rect.X);

				int shape_skip =
					origin.X - right + shape_rect.Width
					+ src_x + shape_rect.X;
				int alpha_skip =
					AlphaBuffer->Get_Buffer_Width() - right + left;
				int mask_skip = ISO_TILE_PIXEL_W - r4.Width;

				const unsigned char *maskptr =
					&tile_mask[ISO_TILE_PIXEL_W * offset.Y + offset.X];
				unsigned char *shapeptr =
					(unsigned char *)shape->GetPixels(0);

				unsigned short *alphaptr =
					(unsigned short *)AlphaBuffer->Get_Buffer_Offset(
						Point2D(left, top - TacticalRect.Y));

				shapeptr += src_x + src_y * shape_rect.Width;
				unsigned char pixel;

				if (&alphaptr[
						right - left
						+ (bottom - top) * AlphaBuffer->Get_Buffer_Width()
						+ 2]
					>= (unsigned short *)AlphaBuffer->Get_Buffer_End()) {
					for (int i = top; i < bottom; i++) {
						for (int j = left; j < right; j++) {
							pixel = *shapeptr++;
							if (*maskptr++ != '\x20') {
								*alphaptr = BrightnessTable[pixel][*alphaptr];
							}
							alphaptr++;
							alphaptr = (unsigned short *)
								AlphaBuffer->Wrap_Overflow((unsigned)alphaptr);
						}
						shapeptr += shape_skip;
						maskptr += mask_skip;
						alphaptr += alpha_skip;
						alphaptr = (unsigned short *)
							AlphaBuffer->Wrap_Overflow((unsigned)alphaptr);
					}
				} else {
					for (int i = top; i < bottom; i++) {
						for (int j = left; j < right; j++) {
							pixel = *shapeptr++;
							if (*maskptr++ != '\x20') {
								*alphaptr = BrightnessTable[pixel][*alphaptr];
							}
							++alphaptr;
						}
						shapeptr += shape_skip;
						alphaptr += alpha_skip;
						maskptr += mask_skip;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00421350
void AlphaShapeClass::Draw_All(Rect const & cliprect)
{
	unsigned char pixel;

	for (int i = 0; i < AlphaShapes.Count(); i++) {
		{
			Rect r1 = AlphaShapes[i]->DrawRect;
			r1 -= Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
			r1 += TacticalRect.Top_Left();

			Rect clip = cliprect;
			Rect r2 = Intersect(r1, clip);

			if (r2.Is_Valid()) {
				Point2D origin = r1.Top_Left();

				SHPStruct const * shape = AlphaShapes[i]->ImageData;
				if (shape == NULL) {
					AlphaShapes[i]->ImageData = (SHPStruct const *)AlphaShapes[i]->Owner->Class_Of()->AlphaImageData;
					shape = AlphaShapes[i]->ImageData;
				}
				Rect shape_rect = shape->GetFrameBounds(0);

				int top = MAX(r2.Y, origin.Y + shape_rect.Y);
				int src_y = top - origin.Y - shape_rect.Y;
				int bottom = MIN(clip.Y + clip.Height, origin.Y + shape_rect.Height + shape_rect.Y);
				int left = MAX(r2.X, origin.X + shape_rect.X);
				int src_x = left - origin.X - shape_rect.X;
				int right = MIN(clip.X + clip.Width, origin.X + shape_rect.Width + shape_rect.X);

				int shape_skip = origin.X - right + shape_rect.Width + src_x + shape_rect.X;
				int alpha_skip = AlphaBuffer->Get_Buffer_Width() - right + left;

				unsigned char * shapeptr = (unsigned char *)shape->GetPixels(0);

				unsigned short * alphaptr = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(Point2D(left, top) - TacticalRect.Top_Left());

				shapeptr += src_x + src_y * shape_rect.Width;
				if (&alphaptr[right - left + (bottom - top) * AlphaBuffer->Get_Buffer_Width() + 2] >= (unsigned short *)AlphaBuffer->Get_Buffer_End()) {
					for (int i = top; i < bottom; i++) {
						for (int j = left; j < right; j++) {
							pixel = *shapeptr++;
							*alphaptr = BrightnessTable[pixel][*alphaptr];
							alphaptr++;
							alphaptr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned)alphaptr);
						}
						shapeptr += shape_skip;
						alphaptr += alpha_skip;
						alphaptr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned)alphaptr);
					}
				} else {
					for (int i = top; i < bottom; i++) {
						for (int j = left; j < right; j++) {
							pixel = *shapeptr++;
							*alphaptr = BrightnessTable[pixel][*alphaptr];
							++alphaptr;
						}
						shapeptr += shape_skip;
						alphaptr += alpha_skip;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004216C0
void AlphaShapeClass::Calculate_Brightness_Table(void)
{
	BrightnessCalculated = true;
	for (int i = 0; i < 256*256; i++) {
		int low = i % 256;
		int high = i / 256;
		int brightness = MAX(0, MIN(255, low * high / 127));
		BrightnessTable[0][i] = brightness;
	}
}

// SYNTHETIC: GAMEMD 0x00421730
// ??_GAlphaShapeClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00421800
// ?Clear@?$DynamicVectorClass@PAVAlphaShapeClass@@@@UAEXXZ
template class DynamicVectorClass<AlphaShapeClass *>;

// TEMPLATE: GAMEMD 0x00421830
// ?ID@?$DynamicVectorClass@PAVAlphaShapeClass@@@@UAEHPBQAVAlphaShapeClass@@@Z

// TEMPLATE: GAMEMD 0x00421850
// ?GetItem@?$VectorClass@PAVAlphaShapeClass@@@@UBEPAVAlphaShapeClass@@H@Z

// TEMPLATE: GAMEMD 0x00421860
// ??8?$VectorClass@PAVAlphaShapeClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x004218A0
// ?Resize@?$VectorClass@PAVAlphaShapeClass@@@@UAE_NHPBQAVAlphaShapeClass@@@Z

// TEMPLATE: GAMEMD 0x00421950
// ?Clear@?$VectorClass@PAVAlphaShapeClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00421980
// ?ID@?$VectorClass@PAVAlphaShapeClass@@@@UAEHABQAVAlphaShapeClass@@@Z

// TEMPLATE: GAMEMD 0x004219C0
// ?ID@?$VectorClass@PAVAlphaShapeClass@@@@UAEHPBQAVAlphaShapeClass@@@Z

// TEMPLATE: GAMEMD 0x004219E0
// ?Resize@?$DynamicVectorClass@PAVAlphaShapeClass@@@@UAE_NHPBQAVAlphaShapeClass@@@Z

// TEMPLATE: GAMEMD 0x00421A90
// ?ID@?$DynamicVectorClass@PAVAlphaShapeClass@@@@UAEHABQAVAlphaShapeClass@@@Z

// SYNTHETIC: GAMEMD 0x00421AC0
// ??_G?$VectorClass@PAVAlphaShapeClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00421B10
// ??_G?$DynamicVectorClass@PAVAlphaShapeClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00421B60
// ?Intersect@@YI?BV?$TRect@H@@ABV1@0PAH1@Z
template Rect const Intersect<int>(Rect const &, Rect const &, int *, int *);
