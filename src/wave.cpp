// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "wave.h"
#include "_logic.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "dsurface.h"
#include "fastmath.h"
#include "globals.h"
#include "goptions.h"
#include "logic.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tracker.h"
#include "vector.h"
#include "warhead.h"
#include "weapon.h"
#include "zbuffer.h"

enum {
	_RADIUS_TABLE_SIZE = 300,
	_SINE_TABLE_SIZE = 500,
	_INTENSITY_TABLE_SIZE = 13,
	_INTENSITY_TABLE_OFFSET = 110,
};

extern unsigned short RadiusTable[_RADIUS_TABLE_SIZE][_RADIUS_TABLE_SIZE];
extern short SineTable[_SINE_TABLE_SIZE];
extern short SineTable2[496];
extern int IntensityTable[_INTENSITY_TABLE_SIZE];
extern bool _tables_calculated;

FacingType Facing_Between_Points(Point2D const & pt1, Point2D const & pt2);

// FUNCTION: 0x0075E950
WaveClass::WaveClass(Coord const & source_coord, Coord const & target_coord, TechnoClass * source, WaveType type, TechnoClass * target) :
	BASECLASS(),
	Target(target),
	Type(type),
	IsWaveActive(true),
	ShouldEnd(false),
	WaveEC(100),
	WaveCount(0),
	WaveProgress(0),
	FadeProgress(0),
	LaserEC(0),
	Source(source),
	AffectedCells(5)
{
	int dx = source_coord.X - target_coord.X;
	int dy = source_coord.Y - target_coord.Y;
	if (fastmath::sqrt((double)(dy * dy + dx * dx)) < 240.0) {
		Delete_Me();
		return;
	}

	Create_ID();
	Array.Add(this);

	if (!_tables_calculated) {
		Init_Statics();
	}

	if (Type == 3) {
		Draw_Magnetic(source_coord, target_coord);
	} else {
		Draw_NonMagnetic(source_coord, target_coord);
	}
	DrawData.Points = NULL;
	Direction = Facing_Between_Points(WaveStartMiddle, WaveEndRight);
	Unlimbo(StartCoord);

	if (Type > 0 && Type <= 2) {
		WaveProgress = 1.0;
		LaserEC = 160;
		WaveShape.Vertices[PolygonShapeStruct::END_LEFT] = WaveEndLeft;
		WaveShape.Vertices[PolygonShapeStruct::END_MIDDLE] = WaveEndMiddle;
		WaveShape.Vertices[PolygonShapeStruct::END_RIGHT] = WaveEndRight;
	}

	Update_Wave();
}

// FUNCTION: 0x0075EBE0
WaveClass::WaveClass(void) :
	BASECLASS(),
	Target(NULL),
	IsWaveActive(true),
	ShouldEnd(false),
	WaveEC(100),
	WaveProgress(0),
	FadeProgress(0),
	LaserEC(0),
	Source(NULL),
	AffectedCells(5)
{
	Array.Add(this);

	if (!_tables_calculated) {
		Init_Statics();
	}
}

// FUNCTION: 0x0075ED30
WaveClass::~WaveClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);
	AffectedCells.Clear();
}

// FUNCTION: 0x0075EDF0
void WaveClass::Set_Sonic_Pixel(int x, int xoff, int y, unsigned short * buffer) const
{
	int radius = RadiusTable[y][abs(x - WaveStartMiddle.X - xoff)];
	int amplitude = abs(SineTable[WaveEC + radius]);
	unsigned short color = buffer[WaveIntensityTable[amplitude]];
	int mult = IntensityTable[amplitude];

	RGBClass rgb = DSurface::Deconstruct_Hicolor_Pixel(color);
	int red = rgb.Get_Red();
	int green = rgb.Get_Green();
	int blue = rgb.Get_Blue();
	green += ((green * mult) >> 8);
	blue += ((blue * mult) >> 8);

	*buffer = DSurface::Build_Hicolor_Pixel(red, MIN(255, green), MIN(255, blue));
}

// FUNCTION: 0x0075EF30
void WaveClass::Set_Laser_Pixel(unsigned short * buffer, int mult) const
{
	RGBClass rgb = DSurface::Deconstruct_Hicolor_Pixel(*buffer);
	int red = rgb.Get_Red();
	int green = rgb.Get_Green();
	int blue = rgb.Get_Blue();
	red += ((mult * 64) >> 8);
	blue += ((mult * 96) >> 8);
	*buffer = DSurface::Build_Hicolor_Pixel(MIN(255, red), MIN(255, green), MIN(255, blue));
}

// FUNCTION: 0x0075F020
void WaveClass::Init_Statics(void)
{
	_tables_calculated = true;

	for (int x = 0; x < _RADIUS_TABLE_SIZE; x++) {
		for (int y = 0; y < _RADIUS_TABLE_SIZE; y++) {
			RadiusTable[x][y] = fastmath::sqrt(x * x + y * y);
		}
	}

	int i;
	for (i = 0; i < _SINE_TABLE_SIZE; i++) {
		SineTable[i] = (short)(fastmath::sin(i * 0.125) * 12 + 0.49);
	}

	for (i = 0; i < 496; i++) {
		SineTable2[i] = (short)(fastmath::sin(i * 0.1) * 12 + 0.49);
	}

	for (i = 0; i < _INTENSITY_TABLE_SIZE; i++) {
		IntensityTable[i] = _INTENSITY_TABLE_OFFSET + (i * 8);
	}
}

// FUNCTION: 0x0075F140
void WaveClass::Init_Offset_Tables(void)
{
	int stride = DSurface::Temp->Stride() >> 1;

	DirectionStrides[FACING_N]		= -stride;
	DirectionStrides[FACING_NE]	= 1 - stride;

	DirectionStrides[FACING_E]		= 1;
	DirectionStrides[FACING_SE]	= stride + 1;

	DirectionStrides[FACING_S]		= stride;
	DirectionStrides[FACING_SW]	= stride - 1;

	DirectionStrides[FACING_W]		= -1;
	DirectionStrides[FACING_NW]	= -1 - stride;

	WaveIntensityTable[0]	= 0;
	WaveIntensityTable[1]	= 0;

	WaveIntensityTable[2]	= 1 * DirectionStrides[Direction];
	WaveIntensityTable[3]	= 1 * DirectionStrides[Direction];
	WaveIntensityTable[4]	= 1 * DirectionStrides[Direction];
	WaveIntensityTable[5]	= 1 * DirectionStrides[Direction];
	WaveIntensityTable[6]	= 1 * DirectionStrides[Direction];

	WaveIntensityTable[7]	= 2 * DirectionStrides[Direction];
	WaveIntensityTable[8]	= 2 * DirectionStrides[Direction];
	WaveIntensityTable[9]	= 2 * DirectionStrides[Direction];

	WaveIntensityTable[10]	= 3 * DirectionStrides[Direction];
	WaveIntensityTable[11]	= 3 * DirectionStrides[Direction];
	WaveIntensityTable[12]	= 3 * DirectionStrides[Direction];
}

// FUNCTION: 0x0075F230
FacingType Facing_Between_Points(Point2D const & pt1, Point2D const & pt2)
{
	int xdiff = pt2.X - pt1.X;
	int ydiff = pt1.Y - pt2.Y;

	double tangent = fastmath::sin(DEG_TO_RAD(22.5)) / fastmath::cos(DEG_TO_RAD(22.5));

	if (xdiff != 0) {
		double slope = (double)ydiff / (double)xdiff;

		if (slope < 1.0 / tangent && slope >= tangent) {
			return((xdiff > 0) ? FACING_NE : FACING_SW);
		}

		if (slope < tangent && slope >= -tangent) {
			return((xdiff > 0) ? FACING_E : FACING_W);
		}

		if (slope < -tangent && slope >= -1.0 / tangent) {
			return((xdiff > 0) ? FACING_SE : FACING_NW);
		}
	}
	return((ydiff > 0) ? FACING_N : FACING_S);
}

// FUNCTION: 0x0075F330
void WaveClass::Sonic_Damage(Coord const & coord)
{
	if (Source != NULL) {
		CellClass *cptr = &Map[coord.As_Cell()];

		WeaponTypeClass * weapon = Source->PrimaryWeapon;
		WarheadTypeClass const * warhead = weapon->WarheadPtr;

		int damage = weapon->AmbientDamage;

		bool isbridge = (cptr->IsUnderBridge && StartCoord.Z >= LEVEL_LEPTON_H * (cptr->Height + BRIDGE_CELL_HEIGHT));

		ObjectClass * obj = cptr->Cell_Occupier(isbridge);
		while (obj != NULL) {
			if (obj != Source) {
				if (obj->IsActive && obj->IsDown && !obj->IsInLimbo && obj->Strength > 0) {
					obj->Take_Damage(damage, 0, warhead, Source);
				}
			}
			obj = obj->Next;
		}

		if (cptr->Overlay != OVERLAY_NONE) {
			OverlayTypeClass *optr = OverlayTypes[cptr->Overlay];
			if (optr->IsChainReaction) {
				Cell cell = cptr->CellID;
				Chain_Reaction_Damage(cell);
			}
			if (optr->IsWall) {
				cptr->Reduce_Wall(weapon->AmbientDamage);
			}
		}

		if (cptr->Is_Tile_Destroyable_Cliff()) {
			int chance = Rule->CollapseChance;
			if (Random_Pick(0, 99) < chance) {
				Map.Collapse_Cliff(cptr);
			}
		}
	}
}

// FUNCTION: 0x0075F4C0
void WaveClass::Sonic_Add_Cell(Cell const & cell)
{
	CellClass * cptr = &Map[cell];
	if (AffectedCells.ID(cptr) == -1) {
		AffectedCells.Add(cptr);
	}
}

// FUNCTION: 0x0075F540
Coord Lerp(Coord const & a, Coord const & b, float t)
{
	double inv = 1.0 - t;
	return(Coord(int((a.X * inv) + (b.X * t)),
				 int((a.Y * inv) + (b.Y * t)),
				 int((a.Z * inv) + (b.Z * t))));
}

// FUNCTION: 0x0075F5C0
Point2D Lerp(Point2D const & a, Point2D const & b, float t)
{
	double inv = 1.0 - t;
	int x = int((a.X * inv) + (b.X * t));
	int y = int((a.Y * inv) + (b.Y * t));
	return(Point2D(x,y));
}

// FUNCTION: 0x0075F610
void WaveClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);

	if (Source == target) {
		Source = NULL;
	}
	if (Target == target) {
		Target = NULL;
	}
}

// FUNCTION: 0x0075F650
HRESULT STDMETHODCALLTYPE WaveClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) WaveClass(NoInitClass());

		if (Type == 3) {
			WaveShape.Vertices = (Point2D *)MagPoints;
		} else {
			WaveShape.Vertices = &ActiveWave.StartMiddle;
		}

		Swizzle_Pointer(&Target);
		Swizzle_Pointer(&Source);

		int index;
		int count;

		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			CellClass * obj;
			result = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(result)) {
				return(result);
			}
			AffectedCells.Add(obj);
		}

		for (index = 0; index < count; index++) {
			Swizzle_Pointer(&AffectedCells[index]);
		}
	}
	return(result);
}

// FUNCTION: 0x0075F7D0
HRESULT STDMETHODCALLTYPE WaveClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);

	int count = AffectedCells.Count();
	result = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(result)) {
		return(result);
	}

	for (int index = 0; index < count; index++) {
		result = stream->Write(&AffectedCells[index], sizeof(AffectedCells[index]), NULL);
		if (FAILED(result)) {
			return(result);
		}
	}
	return(result);
}

// FUNCTION: 0x0075F840
HRESULT STDMETHODCALLTYPE WaveClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WaveClass;
	return(S_OK);
}
