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
#include "tracker.h"
#include "vector.h"

static bool BrightnessCalculated = false;

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E2B10;
extern const double DAT_007E2B18;

// FUNCTION: 0x004206F0 $E16
double DAT_0088A0C0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00420720 $E19
double DAT_0089A128 = DAT_0088A0C0 / DAT_007E2B10;

// FUNCTION: 0x00420740 $E22
double DAT_0088A110 = DAT_007E1718 / DAT_0089A128;

// FUNCTION: 0x00420760 $E25
double DAT_0088A108 = DAT_007E1720 * DAT_007E2B18;

// FUNCTION: 0x00420780 $E28
double DAT_0088A0C8 = DAT_007E1728 * DAT_007E2B18;

// FUNCTION: 0x004207A0 $E31
double DAT_0088A0A0 = DAT_007E1730 * DAT_007E2B18;

// FUNCTION: 0x004207C0 $E34
int DAT_0089A130 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0088A0A0 - DAT_0088A0C8) * DAT_0088A0C0));

// FUNCTION: 0x00420800 $E37
double DAT_0088A0D0 = FUN_004CADE0(DAT_0089A130 * DAT_007E1740);

// FUNCTION: 0x00420830 $E40
double DAT_0088A0E8 = FUN_004CADE0((DAT_0089A130 * 2) / DAT_0088A0C0);

// FUNCTION: 0x00420860 $E43
int DAT_0089A124 = (int)(DAT_0089A130 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00420890 $E46
ShortPair DAT_0088A0B8;

// FUNCTION: 0x004208A0 $E49
IntTriple DAT_0088A0D8;

// FUNCTION: 0x004208C0 $E52
struct IntTriple2 {
	int A, B, C;
	IntTriple2() { C = 0; A = 0x80; B = 0x80; }
};
IntTriple2 DAT_0089A118;

// FUNCTION: 0x004208E0 $E57
// FUNCTION: 0x00420920 $E55
DynamicVectorClass<AlphaShapeClass *> AlphaShapes;

unsigned char AlphaShapeClass::BrightnessTable[256][256];

// FUNCTION: 0x00420960
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

// FUNCTION: 0x00420AF0
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

// FUNCTION: 0x00420C50
AlphaShapeClass::AlphaShapeClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x00420C80
AlphaShapeClass::~AlphaShapeClass(void)
{
	AlphaShapes.Delete(this);
	ObjectPtrTracker.Delete(this);
}

// FUNCTION: 0x00420D40
HRESULT STDMETHODCALLTYPE AlphaShapeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AlphaShapeClass;
	return(S_OK);
}

// FUNCTION: 0x00420D80
RTTIType AlphaShapeClass::Fetch_RTTI(void) const
{
	return(RTTI_ALPHASHAPE);
}

// FUNCTION: 0x00420D90
int AlphaShapeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00420DA0
void AlphaShapeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(DrawRect.X);
	crc(DrawRect.Y);
	crc(DrawRect.Width);
	crc(DrawRect.Height);
}

// FUNCTION: 0x00420DE0
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

// FUNCTION: 0x00420E40
HRESULT STDMETHODCALLTYPE AlphaShapeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		return(S_OK);
	}
	return(result);
}

// FUNCTION: 0x00420E70
void AlphaShapeClass::Detach(AbstractClass const * target, bool all)
{
	if (target == Owner) {
		IsToDelete = true;
	}
}

// FUNCTION: 0x00420E90
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

// FUNCTION: 0x004216C0
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

// FUNCTION: 0x00421730 AlphaShapeClass::~AlphaShapeClass

// FUNCTION: 0x00421800 ?$DynamicVectorClass@PAVAlphaShapeClass@@::Clear
template class DynamicVectorClass<AlphaShapeClass *>;

// FUNCTION: 0x00421830 ?$DynamicVectorClass@PAVAlphaShapeClass@@::ID

// FUNCTION: 0x00421850 ?$VectorClass@PAVAlphaShapeClass@@::GetItem

// FUNCTION: 0x00421860 ?$VectorClass@PAVAlphaShapeClass@@::operator==

// FUNCTION: 0x004218A0 ?$VectorClass@PAVAlphaShapeClass@@::Resize

// FUNCTION: 0x00421950 ?$VectorClass@PAVAlphaShapeClass@@::Clear

// FUNCTION: 0x00421980 ?$VectorClass@PAVAlphaShapeClass@@::ID

// FUNCTION: 0x004219C0 ?$VectorClass@PAVAlphaShapeClass@@::ID

// FUNCTION: 0x004219E0 ?$DynamicVectorClass@PAVAlphaShapeClass@@::Resize

// FUNCTION: 0x00421A90 ?$DynamicVectorClass@PAVAlphaShapeClass@@::ID

// FUNCTION: 0x00421AC0 ?$DynamicVectorClass@PAVAlphaShapeClass@@::~?$DynamicVectorClass@PAVAlphaShapeClass@@

// FUNCTION: 0x00421B10 ?$VectorClass@PAVAlphaShapeClass@@::~?$VectorClass@PAVAlphaShapeClass@@

// FUNCTION: 0x00421B60 Intersect
template Rect const Intersect<int>(Rect const &, Rect const &, int *, int *);

// FUNCTION: 0x00421C90 $E60
Rect DAT_0089A140(0, 0, 0, 0);
