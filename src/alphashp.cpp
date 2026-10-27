// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "alphashp.h"
#include "_alpha.h"
#include "_tactica.h"
#include "crc.h"
#include "globals.h"
#include "object.h"
#include "objtype.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "geometry.h"
#include "tracker.h"
#include "vector.h"

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
	ShapeSet const * image = (ShapeSet const *)owner->Class_Of()->AlphaImageData;
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

// SYNTHETIC: GAMEMD 0x00421C90 SYMBOL
// _$E60
Rect DAT_0089A140(0, 0, 0, 0);
