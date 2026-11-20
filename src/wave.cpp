// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "rect.h"
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

// SYNTHETIC: GAMEMD 0x0075E740 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0075E770 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0075E790 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0075E7B0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0075E7D0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0075E7F0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0075E810 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0075E850 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0075E880 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0075E8B0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0075E8E0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0075E8F0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0075E910 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0075E930 SYMBOL
// _$E41

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

static const float WaveLengthFactor[] = { 1.0f, 1.05f, 1.05f, 1.0f };
const float WaveStep = 0.05f;

FacingType Facing_Between_Points(Point2D const & pt1, Point2D const & pt2);

// FUNCTION: GAMEMD 0x0075E950
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

// FUNCTION: GAMEMD 0x0075EBE0
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

// FUNCTION: GAMEMD 0x0075ED30
WaveClass::~WaveClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);
	AffectedCells.Clear();
}

// FUNCTION: GAMEMD 0x0075EDF0
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

// FUNCTION: GAMEMD 0x0075EF30
void WaveClass::Set_Laser_Pixel(unsigned short * buffer, int mult) const
{
	RGBClass rgb = DSurface::Deconstruct_Hicolor_Pixel(*buffer);
	int red = rgb.Get_Red() + ((mult * 64) >> 8);
	int green = rgb.Get_Green();
	int blue = rgb.Get_Blue() + ((mult * 96) >> 8);
	*buffer = DSurface::Build_Hicolor_Pixel(
		MIN(255, red), MIN(255, green), MIN(255, blue));
}

// FUNCTION: GAMEMD 0x0075F020
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

// FUNCTION: GAMEMD 0x0075F140
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

// FUNCTION: GAMEMD 0x0075F230
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

// FUNCTION: GAMEMD 0x0075F330
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

// FUNCTION: GAMEMD 0x0075F4C0
void WaveClass::Sonic_Add_Cell(Cell const & cell)
{
	CellClass * cptr = &Map[cell];
	if (AffectedCells.ID(cptr) == -1) {
		AffectedCells.Add(cptr);
	}
}

// FUNCTION: GAMEMD 0x0075F540
Coord Lerp(Coord const & a, Coord const & b, float t)
{
	double inv = 1.0 - t;
	return(Coord(int((a.X * inv) + (b.X * t)),
				 int((a.Y * inv) + (b.Y * t)),
				 int((a.Z * inv) + (b.Z * t))));
}

// FUNCTION: GAMEMD 0x0075F5C0
Point2D Lerp(Point2D const & a, Point2D const & b, float t)
{
	double inv = 1.0 - t;
	int x = int((a.X * inv) + (b.X * t));
	int y = int((a.Y * inv) + (b.Y * t));
	return(Point2D(x,y));
}

// FUNCTION: GAMEMD 0x0075F610
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

// FUNCTION: GAMEMD 0x0075F650
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

// FUNCTION: GAMEMD 0x0075F7D0
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

// FUNCTION: GAMEMD 0x0075F840
HRESULT STDMETHODCALLTYPE WaveClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WaveClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0075F880
int WaveClass::Fetch_Object_Size(void) const
{
	return sizeof(*this);
}

// FUNCTION: GAMEMD 0x0075F890
LayerType WaveClass::In_Which_Layer(void) const
{
	return LAYER_AIR;
}

// FUNCTION: GAMEMD 0x0075F8A0
ObjectTypeClass const * WaveClass::Class_Of(void) const
{
	return NULL;
}

// FUNCTION: GAMEMD 0x0075F8B0
bool WaveClass::Unlimbo(Coord const & coord, Dir256 facing)
{
	if (GameActive && IsInLimbo && !IsDown) {
		if (ScenarioInit || Can_Enter_Cell(&Map[coord], FACING_NONE, coord.Z) == MOVE_OK) {
			IsInLimbo = false;
			IsToDisplay = false;
			PositionCoord = coord;

			if (Mark(MARK_DOWN)) {
				if (IsActive) {
					if (In_Which_Layer() != LAYER_NONE) {
						Map.Submit(this);
					}
					Logic.Submit(this);
				}
				return true;
			}
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x0075F980
bool WaveClass::Limbo(void)
{
	if (GameActive && !IsInLimbo) {
		Detach_All();
		Mark(MARK_UP);

		Map.Remove(this);
		Logic.Remove(this);

		Hidden();
		IsInLimbo = true;
		IsToDisplay = false;
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x0075F9F0
void WaveClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	if (Scen->Special.IsFogOfWar && Map.Is_Fogged(StartCoord) && Map.Is_Fogged(EndCoord)) return;

	switch (Type) {
		case WAVE_SONIC:
			((WaveClass *)this)->Draw_Sonic(point, cliprect);
			break;
		case 3:
			((WaveClass *)this)->FUN_007602E0(point, cliprect);
			break;
		case WAVE_BIG_LASER:
		case WAVE_LASER:
			((WaveClass *)this)->Draw_Laser(point, cliprect);
			break;
	}
}

// FUNCTION: GAMEMD 0x0075FA90
void WaveClass::Draw_Sonic(Point2D const & point, Rect const & cliprect)
{
	int y;
	unsigned short zval;
	int zpix = Tactical::Z_Lepton_To_Pixel(StartCoord.Z);

	unsigned short * surfptr = (unsigned short *)LogicalSurface->Lock();
	if (surfptr != NULL) {

		int top = cliprect.Y + 3;
		int bottom = cliprect.Y + cliprect.Height - 3;
		int left = cliprect.X + 3;
		int right = cliprect.X + cliprect.Width - 3;

		int xoff = point.X - WaveStartMiddle.X;
		int yoff = point.Y - WaveStartMiddle.Y;

		Rasterize_Polygon(WaveShape, DrawData);

		if (DrawData.Points != NULL) {

			int starty = yoff + TacticalRect.Y + DrawData.BaseY;
			int endy = MIN(DrawData.Count + starty - 1, cliprect.Height + cliprect.Y);

			Init_Offset_Tables();

			int xleft;
			int zwidth;
			unsigned short * dest;
			int xstop;
			int x;
			int ypos;
			int width;
			int index;
			unsigned short base_z;
			unsigned int zoffset;
			unsigned int depth_address;
			Point2D * points;
			Point2D depth_position;
			if (Direction > FACING_NE && Direction < FACING_W) {
				base_z = DepthBuffer->Get_Scroll_Delta(zpix);
				zval = base_z - (unsigned short)starty - 2;
				zoffset = DepthBuffer->Get_Buffer_Offset(Point2D(0, starty - TacticalRect.Y));
				width = LogicalSurface->Get_Width();
				zwidth = DepthBuffer->Get_Buffer_Width();

				if (zoffset + 2 * (width + zwidth * (endy - starty + 1)) < DepthBuffer->Get_Buffer_End()) {

					int stride = LogicalSurface->Stride() >> 1;
					dest = surfptr + starty * stride;
					points = DrawData.Points;

					for (y = starty; y <= endy; y++) {
						if (y >= top && y <= bottom) {
							xstop = MIN(xoff + points->Y, right);
							xleft = MAX(points->X + xoff, left);

							zoffset += 2 * xleft;
							dest += xleft;

							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xleft; x <= xstop; x++) {
								if (*(unsigned short *)zoffset > zval) {
									Set_Sonic_Pixel(x, xoff, ypos, dest);
								}
								zoffset += 2;
								dest++;
							}
							zoffset -= 2 * x;
							dest -= x;
						}
						dest += stride;
						zoffset += 2 * zwidth;
						zval--;
						points++;
					}

				} else {

					index = 0;
					for (y = starty; y <= endy; zval--, y++, index++) {
						if (y >= cliprect.Y + 2 && y <= cliprect.Y + cliprect.Height - 2) {
							points = &DrawData.Points[index];
							xstop = MIN(xoff + points->Y, cliprect.X + cliprect.Width - 2);
							xleft = MAX(points->X + xoff, cliprect.X + 2);

							dest = surfptr + xleft + y * (LogicalSurface->Stride() >> 1);
							depth_position.X = xleft;
							depth_position.Y = y - TacticalRect.Y;
							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xleft; x <= xstop; depth_position.X++, x++, dest++) {
								depth_address = DepthBuffer->Get_Buffer_Offset(depth_position);
								if (*(unsigned short *)depth_address > zval) {
									Set_Sonic_Pixel(x, xoff, ypos, dest);
								}
							}
						}
					}
				}

			} else {

				zoffset = DepthBuffer->Get_Buffer_Offset(Point2D(0, starty - TacticalRect.Y));
				width = LogicalSurface->Get_Width();
				ZBuffer * depth = DepthBuffer;
				zwidth = depth->Get_Buffer_Width();
				int rows = endy - starty;

				if (zoffset + 2 * (width + zwidth * (rows + 1)) < depth->Get_Buffer_End()) {

					int stride = LogicalSurface->Stride() >> 1;
					dest = surfptr + endy * stride;
					points = &DrawData.Points[rows];
					base_z = DepthBuffer->Get_Scroll_Delta(endy);
					zval = base_z - (unsigned short)zpix - 2;
					zoffset = DepthBuffer->Get_Buffer_Offset(Point2D(0, endy - TacticalRect.Y));

					for (y = endy; y >= starty; zval++, points--, y--) {
						if (y >= top && y <= bottom) {
							xstop = MIN(xoff + points->Y, right);
							xleft = MAX(xoff + points->X, left);

							dest += xstop;
							zoffset += 2 * xstop;

							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xstop; x >= xleft; x--) {
								if (*(unsigned short *)zoffset > zval) {
									Set_Sonic_Pixel(x, xoff, ypos, dest);
								}
								zoffset -= 2;
								dest--;
							}
							zoffset -= 2 * x;
							dest -= x;
						}
						zoffset -= 2 * zwidth;
						dest -= stride;
					}

				} else {

					base_z = depth->Get_Scroll_Delta(endy);
					zval = base_z - (unsigned short)zpix - 2;
					index = rows;
					for (y = endy; y >= starty; zval++, y--, index--) {
						if (y >= cliprect.Y + 2 && y <= cliprect.Y + cliprect.Height - 2) {
							points = &DrawData.Points[index];
							xstop = MIN(points->Y + xoff, cliprect.X + cliprect.Width - 2);
							xleft = MAX(xoff + points->X, cliprect.X + 2);

							dest = surfptr + xstop + y * (LogicalSurface->Stride() >> 1);
							depth_position.X = xstop;
							depth_position.Y = y - TacticalRect.Y;
							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xstop; x >= xleft; depth_position.X--, x--, dest--) {
								depth_address = DepthBuffer->Get_Buffer_Offset(depth_position);
								if (*(unsigned short *)depth_address > zval) {
									Set_Sonic_Pixel(x, xoff, ypos, dest);
								}
							}
						}
					}
				}
			}
		}

		LogicalSurface->Unlock();

		if (DrawData.Points != NULL) {
			delete [] DrawData.Points;
			DrawData.Points = NULL;
		}
	}
}

// FUNCTION: GAMEMD 0x00760190
void WaveClass::FUN_00760190(int x, int xoff, int y, unsigned short * buffer) const
{
	int radius = RadiusTable[y][abs(x - WaveStartMiddle.X - xoff)];
	int amplitude = abs(SineTable2[WaveEC + radius]);
	if (amplitude >= 9) amplitude = 9;
	unsigned short color = buffer[WaveIntensityTable[amplitude]];
	int mult = IntensityTable[amplitude];

	RGBClass rgb = DSurface::Deconstruct_Hicolor_Pixel(color);
	int red = rgb.Get_Red();
	int green = rgb.Get_Green();
	int blue = rgb.Get_Blue();
	red += ((red * (mult / 2)) >> 8);
	blue += ((blue * mult * 4) >> 8);

	*buffer = DSurface::Build_Hicolor_Pixel(MIN(255, red), green, MIN(255, blue));
}

// FUNCTION: GAMEMD 0x007602E0
void WaveClass::FUN_007602E0(Point2D const & point, Rect const & cliprect)
{
	int y;
	unsigned short zval;
	int zpix = Tactical::Z_Lepton_To_Pixel(StartCoord.Z);

	unsigned short * surfptr = (unsigned short *)LogicalSurface->Lock();
	if (surfptr != NULL) {

		int top = cliprect.Y + 3;
		int bottom = cliprect.Y + cliprect.Height - 3;
		int left = cliprect.X + 3;
		int right = cliprect.X + cliprect.Width - 3;

		int xoff = point.X - WaveStartMiddle.X;
		int yoff = point.Y - WaveStartMiddle.Y;

		Rasterize_Polygon(WaveShape, DrawData);

		if (DrawData.Points != NULL) {

			int starty = yoff + TacticalRect.Y + DrawData.BaseY;
			int endy = MIN(DrawData.Count + starty - 1, cliprect.Height + cliprect.Y);

			Init_Offset_Tables();

			int xleft;
			int zwidth;
			unsigned short * dest;
			int xstop;
			int x;
			int ypos;
			int width;
			int index;
			unsigned short base_z;
			unsigned int zoffset;
			unsigned int depth_address;
			Point2D * points;
			Point2D depth_position;
			if (Direction > FACING_NE && Direction < FACING_W) {
				base_z = DepthBuffer->Get_Scroll_Delta(zpix);
				zval = base_z - (unsigned short)starty - 2;
				zoffset = DepthBuffer->Get_Buffer_Offset(Point2D(0, starty - TacticalRect.Y));
				width = LogicalSurface->Get_Width();
				zwidth = DepthBuffer->Get_Buffer_Width();

				if (zoffset + 2 * (width + zwidth * (endy - starty + 1)) < DepthBuffer->Get_Buffer_End()) {

					int stride = LogicalSurface->Stride() >> 1;
					dest = surfptr + starty * stride;
					points = DrawData.Points;

					for (y = starty; y <= endy; y++) {
						if (y >= top && y <= bottom) {
							xstop = MIN(xoff + points->Y, right);
							xleft = MAX(points->X + xoff, left);

							zoffset += 2 * xleft;
							dest += xleft;

							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xleft; x <= xstop; x++) {
								if (*(unsigned short *)zoffset > zval) {
									FUN_00760190(x, xoff, ypos, dest);
								}
								zoffset += 2;
								dest++;
							}
							zoffset -= 2 * x;
							dest -= x;
						}
						dest += stride;
						zoffset += 2 * zwidth;
						zval--;
						points++;
					}

				} else {

					index = 0;
					for (y = starty; y <= endy; zval--, y++, index++) {
						if (y >= cliprect.Y + 2 && y <= cliprect.Y + cliprect.Height - 2) {
							points = &DrawData.Points[index];
							xstop = MIN(xoff + points->Y, cliprect.X + cliprect.Width - 2);
							xleft = MAX(points->X + xoff, cliprect.X + 2);

							dest = surfptr + xleft + y * (LogicalSurface->Stride() >> 1);
							depth_position.X = xleft;
							depth_position.Y = y - TacticalRect.Y;
							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xleft; x <= xstop; depth_position.X++, x++, dest++) {
								depth_address = DepthBuffer->Get_Buffer_Offset(depth_position);
								if (*(unsigned short *)depth_address > zval) {
									FUN_00760190(x, xoff, ypos, dest);
								}
							}
						}
					}
				}

			} else {

				zoffset = DepthBuffer->Get_Buffer_Offset(Point2D(0, starty - TacticalRect.Y));
				width = LogicalSurface->Get_Width();
				ZBuffer * depth = DepthBuffer;
				zwidth = depth->Get_Buffer_Width();
				int rows = endy - starty;

				if (zoffset + 2 * (width + zwidth * (rows + 1)) < depth->Get_Buffer_End()) {

					int stride = LogicalSurface->Stride() >> 1;
					dest = surfptr + endy * stride;
					points = &DrawData.Points[rows];
					base_z = DepthBuffer->Get_Scroll_Delta(endy);
					zval = base_z - (unsigned short)zpix - 2;
					zoffset = DepthBuffer->Get_Buffer_Offset(Point2D(0, endy - TacticalRect.Y));

					for (y = endy; y >= starty; zval++, points--, y--) {
						if (y >= top && y <= bottom) {
							xstop = MIN(xoff + points->Y, right);
							xleft = MAX(xoff + points->X, left);

							dest += xstop;
							zoffset += 2 * xstop;

							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xstop; x >= xleft; x--) {
								if (*(unsigned short *)zoffset > zval) {
									FUN_00760190(x, xoff, ypos, dest);
								}
								zoffset -= 2;
								dest--;
							}
							zoffset -= 2 * x;
							dest -= x;
						}
						zoffset -= 2 * zwidth;
						dest -= stride;
					}

				} else {

					base_z = depth->Get_Scroll_Delta(endy);
					zval = base_z - (unsigned short)zpix - 2;
					index = rows;
					for (y = endy; y >= starty; zval++, y--, index--) {
						if (y >= cliprect.Y + 2 && y <= cliprect.Y + cliprect.Height - 2) {
							points = &DrawData.Points[index];
							xstop = MIN(points->Y + xoff, cliprect.X + cliprect.Width - 2);
							xleft = MAX(xoff + points->X, cliprect.X + 2);

							dest = surfptr + xstop + y * (LogicalSurface->Stride() >> 1);
							depth_position.X = xstop;
							depth_position.Y = y - TacticalRect.Y;
							ypos = abs(y - yoff - WaveStartMiddle.Y);
							for (x = xstop; x >= xleft; depth_position.X--, x--, dest--) {
								depth_address = DepthBuffer->Get_Buffer_Offset(depth_position);
								if (*(unsigned short *)depth_address > zval) {
									FUN_00760190(x, xoff, ypos, dest);
								}
							}
						}
					}
				}
			}
		}

		LogicalSurface->Unlock();

		if (DrawData.Points != NULL) {
			delete [] DrawData.Points;
			DrawData.Points = NULL;
		}
	}
}

// FUNCTION: GAMEMD 0x007609E0
void WaveClass::Draw_Laser(Point2D const & point, Rect const & cliprect)
{
	if (Options.DetailLevel == 2) {
		int zpix = Tactical::Z_Lepton_To_Pixel(StartCoord.Z);

		unsigned short * buffer = (unsigned short *)LogicalSurface->Lock();
		if (buffer != NULL) {
			int top = cliprect.Y;
			int bottom = cliprect.Y + cliprect.Height;
			int left = cliprect.X;
			int right = cliprect.X + cliprect.Width;

			int xoff = point.X - WaveStartMiddle.X;
			int yoff = point.Y - WaveStartMiddle.Y + TacticalRect.Y;

			Rasterize_Polygon(WaveShape, DrawData);

			if (DrawData.Points != NULL) {
				int ystart = yoff + DrawData.BaseY;
				int yend = MIN(
					DrawData.Count + ystart - 1,
					cliprect.Y + cliprect.Height);

				unsigned short base_z = DepthBuffer->Get_Scroll_Delta(ystart);
				unsigned short depth = base_z - zpix - 2;
				unsigned int zoff = DepthBuffer->Get_Buffer_Offset(
					Point2D(0, ystart - TacticalRect.Y));
				int y;
				int surfwidth = LogicalSurface->Get_Width();

				if (zoff + 2 * (surfwidth + (yend - ystart + 1)
					* DepthBuffer->Get_Buffer_Width())
					< DepthBuffer->Get_Buffer_End()) {
					int zwidth = DepthBuffer->Get_Buffer_Width();
					int stride = LogicalSurface->Stride() >> 1;
					unsigned short * scrptr = &buffer[ystart * stride];
					Point2D * points = DrawData.Points;

					for (y = ystart; y <= yend; y++) {
						if (y >= top && y <= bottom) {
							int xstop = MIN(points->Y + xoff, right);
							int xstart = MAX(points->X + xoff, left);

							zoff += 2 * xstart;
							scrptr += xstart;

							int x = xstart;
							for (; x <= xstop; x++) {
								if (*(unsigned short *)zoff > depth) {
									Set_Laser_Pixel(scrptr, LaserEC);
								}
								zoff += 2;
								scrptr++;
							}

							zoff -= 2 * x;
							scrptr -= x;
						}
						scrptr += stride;
						zoff += 2 * zwidth;
						depth--;
						points++;
					}
				} else {
					int i = 0;
					for (y = ystart; y <= yend; y++, i++) {
						if (y >= cliprect.Y + 2
							&& y <= cliprect.Y + cliprect.Height - 2) {
							Point2D const & span = DrawData.Points[i];
							int xstop = MIN(
								span.Y + xoff,
								cliprect.Width + cliprect.X - 2);
							int xstart = MAX(span.X + xoff, cliprect.X + 2);

							unsigned short * scrptr = &buffer[
								xstart + y * (LogicalSurface->Stride() >> 1)];
							Point2D zpos(xstart, y - TacticalRect.Y);

							if (zpos.X <= xstop) {
								int remaining = xstop - zpos.X + 1;
								do {
									if (*(unsigned short *)DepthBuffer
										->Get_Buffer_Offset(zpos) > depth) {
										Set_Laser_Pixel(scrptr, LaserEC);
									}
									zpos.X++;
									scrptr++;
								} while (--remaining != 0);
							}
						}
						depth--;
					}
				}
			}

			LogicalSurface->Unlock();

			if (DrawData.Points != NULL) {
				delete [] DrawData.Points;
				DrawData.Points = NULL;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00760F50
void WaveClass::AI(void)
{
	int i;
	switch (Type) {
		case 3:
			Update_Wave();
			WaveEC -= 1;
			if (WaveEC < 0) {
				Delete_Me();
				return;
			}
			BASECLASS::AI();
			return;
		case WAVE_SONIC:
			Update_Wave();
			for (i = 0; i < AffectedCells.Count(); i++) {
				Sonic_Damage(AffectedCells[i]->Cell_Coord());
			}
			WaveEC -= 1;
			if (WaveEC < 0) {
				Delete_Me();
				return;
			}
			BASECLASS::AI();
			return;
		case WAVE_BIG_LASER:
		case WAVE_LASER:
			LaserEC -= 6;
			if (LaserEC < 32) {
				Delete_Me();
			}
			return;
	}
}

// FUNCTION: GAMEMD 0x00761030
void WaveClass::Sonic_AI(void)
{
	Update_Wave();
	for (int i = 0; i < AffectedCells.Count(); i++) {
		Sonic_Damage(AffectedCells[i]->Cell_Coord());
	}
	WaveEC -= 1;
	if (WaveEC < 0) {
		Delete_Me();
	} else {
		BASECLASS::AI();
	}
}

// FUNCTION: GAMEMD 0x007610A0
void WaveClass::FUN_007610A0(void)
{
	Update_Wave();
	WaveEC -= 1;
	if (WaveEC < 0) {
		Delete_Me();
	} else {
		BASECLASS::AI();
	}
}

// FUNCTION: GAMEMD 0x007610D0
void WaveClass::Laser_AI(void)
{
	LaserEC -= 6;
	if (LaserEC < 32) {
		Delete_Me();
	}
}

static inline bool _Differs(Cell const & a, Cell const & b)
{
	return a != b;
}

static inline void _Set(Cell & c, Coord const & coord)
{
	c = coord.As_Cell();
}

static inline bool _Differs(Cell const & a, Coord const & c)
{
	return a != c.As_Cell();
}

// FUNCTION: GAMEMD 0x007610F0
void WaveClass::Wave_Recalc_Affected_Cells(void)
{
	AffectedCells.Clear();

	if (IsWaveActive) {
		Cell start = StartCoord.As_Cell();

		for (double t = WaveStep; t <= ((WaveStep / 2.0) + WaveProgress); t += WaveStep) {
			Coord coord = Lerp(StartCoord, EndCoord, t);
			Cell cell = coord.As_Cell();
			if (start != cell) {
				start = coord.As_Cell();
				Sonic_Add_Cell(start);
			}
		}
	} else {
		Cell left = WaveEndLeftCoord.As_Cell();
		Cell right = WaveEndRightCoord.As_Cell();
		Cell middle = EndCoord.As_Cell();

		for (double t = FadeProgress; t <= ((WaveStep / 2.0) + WaveProgress); t += WaveStep) {
			Coord coord = COORD_NONE;

			if (Direction < FACING_S) {
				coord = Lerp(WaveStartLeftCoord, WaveEndLeftCoord, t);
				if (left != coord.As_Cell()) {
					left = coord.As_Cell();
					Sonic_Add_Cell(left);
				}
			}

			coord = Lerp(StartCoord, EndCoord, t);
			if (_Differs(middle, coord.As_Cell())) {
				_Set(middle, coord);
				Sonic_Add_Cell(middle);
			}

			if (Direction >= FACING_S) {
				coord = Lerp(WaveStartRightCoord, WaveEndRightCoord, t);
				if (_Differs(right, coord)) {
					_Set(right, coord);
					Sonic_Add_Cell(right);
				}
			}
		}
	}
}

static inline void Lerp_From(Coord & point, Coord const & origin, float t)
{
	double inv = 1.0 - t;
	point = Coord(origin.X * inv + point.X * t, origin.Y * inv + point.Y * t, origin.Z * inv + point.Z * t);
}

// FUNCTION: GAMEMD 0x00761640
void WaveClass::Draw_NonMagnetic(Coord const & source_coord, Coord const & target_coord)
{
	static Vector3 WaveSize[][4] = {
		{Vector3(-30.0, -100.0, 0.0), Vector3(-30.0, 100.0, 0.0), Vector3(30.0, -100.0, 0.0),
		 Vector3(30.0, 100.0, 0.0)},
		{Vector3(-34.0, -44.0, 0.0), Vector3(-34.0, 44.0, 0.0), Vector3(34.0, -44.0, 0.0),
		 Vector3(34.0, 44.0, 0.0)},
		{Vector3(-27.0, -34.0, 0.0), Vector3(-27.0, 34.0, 0.0), Vector3(27.0, -34.0, 0.0),
		 Vector3(27.0, 34.0, 0.0)},
		{Vector3(-30, -50, 0), Vector3(-30, 50, 0), Vector3(30, -50, 0), Vector3(30, 50, 0)}};

	Coord start_coord = source_coord;
	Coord end_coord = Lerp(start_coord, target_coord, WaveLengthFactor[Type]);
	if (Type == WAVE_SONIC) {
		end_coord.Z += 50;
	}

	if (Type == 3) { // Magnetic wave
		end_coord.Z += 50;
	}
	Lerp_From(start_coord, target_coord, WaveLengthFactor[Type]);

	TPoint3D<float> planar_delta(start_coord.X - end_coord.X, start_coord.Y - end_coord.Y, 0);
	int distance2d =
		fastmath::sqrt((double)planar_delta.Y * planar_delta.Y + (double)planar_delta.X * planar_delta.X);

	// Set initial coordinates
	StartCoord = start_coord;
	Set_Coord(StartCoord);
	EndCoord = end_coord;

	WaveShape.Count = 6;
	WaveShape.Vertices = &ActiveWave.StartMiddle;

	// Calculate wave points
	TacticalMap->Coord_To_Pixel(start_coord, WaveStartMiddle);
	TacticalMap->Coord_To_Pixel(end_coord, WaveEndMiddle);

	WaveShape.Vertices[PolygonShapeStruct::END_MIDDLE] = WaveEndMiddle;
	WaveShape.Vertices[PolygonShapeStruct::START_MIDDLE] = WaveStartMiddle;

	double distance2d_f = distance2d;

	Vector3 far_offset(distance2d_f, 0, 0);
	Vector3 near_offset(0, 0, 0);
	Vector3 end_left = WaveSize[Type][0] + far_offset;
	Vector3 end_right = WaveSize[Type][1] + far_offset;
	Vector3 start_left = WaveSize[Type][2] + near_offset;
	Vector3 start_right = WaveSize[Type][3] + near_offset;

	double dz = end_coord.Z - start_coord.Z;

	end_left.Z = end_left.X * dz / distance2d_f + start_coord.Z;
	end_right.Z = end_right.X * dz / distance2d_f + start_coord.Z;
	start_left.Z = start_left.X * dz / distance2d_f + start_coord.Z;
	start_right.Z = start_right.X * dz / distance2d_f + start_coord.Z;

	TPoint3D<float> heading(end_coord.X - start_coord.X, end_coord.Y - start_coord.Y, 0);
	double dx = heading.X;
	double dist = heading.Length();

	if (dist < dx) {
		dx = dist;
	}
	if (-dx > dist) {
		dx = -dist;
	}

	double theta = fastmath::acos(dx / dist);
	if (start_coord.Y > end_coord.Y) {
		theta = -theta;
	}

	// Calculate wave vectors
	Matrix3D matrix;
	matrix.Make_Identity();
	matrix.Rotate_Z(theta);

	end_left = matrix * end_left;
	end_right = matrix * end_right;
	start_left = matrix * start_left;
	start_right = matrix * start_right;

	// Set wave coordinates
	WaveEndLeftCoord = Coord(start_coord.X + end_left.X, start_coord.Y + end_left.Y, end_left.Z);
	WaveEndRightCoord = Coord(start_coord.X + end_right.X, start_coord.Y + end_right.Y, end_right.Z);
	WaveStartLeftCoord = Coord(start_coord.X + start_left.X, start_coord.Y + start_left.Y, start_left.Z);
	WaveStartRightCoord = Coord(start_coord.X + start_right.X, start_coord.Y + start_right.Y, start_right.Z);

	// Convert coordinates to pixels
	TacticalMap->Coord_To_Pixel(WaveEndLeftCoord, WaveEndLeft);
	TacticalMap->Coord_To_Pixel(WaveEndRightCoord, WaveEndRight);
	TacticalMap->Coord_To_Pixel(WaveStartLeftCoord, WaveStartLeft);
	TacticalMap->Coord_To_Pixel(WaveStartRightCoord, WaveStartRight);

	// Set wave points
	WaveShape.Vertices[PolygonShapeStruct::END_LEFT] = WaveEndLeft;
	WaveShape.Vertices[PolygonShapeStruct::END_RIGHT] = WaveEndRight;
	WaveShape.Vertices[PolygonShapeStruct::START_RIGHT] = WaveStartRight;
	WaveShape.Vertices[PolygonShapeStruct::START_LEFT] = WaveStartLeft;

	// Copy facing
	Facing = Source->SecondaryFacing;
}

// SYNTHETIC: GAMEMD 0x00762060 SYMBOL
// _$E138

// FUNCTION: GAMEMD 0x00762070
void WaveClass::Draw_Magnetic(Coord const & source_coord, Coord const & target_coord)
{
	static Vector3 WaveSize[][4] = {
		{Vector3(-30.0, -100.0, 0.0), Vector3(-30.0, 100.0, 0.0), Vector3(30.0, -100.0, 0.0),
		 Vector3(30.0, 100.0, 0.0)},
		{Vector3(-34.0, -44.0, 0.0), Vector3(-34.0, 44.0, 0.0), Vector3(34.0, -44.0, 0.0),
		 Vector3(34.0, 44.0, 0.0)},
		{Vector3(-27.0, -34.0, 0.0), Vector3(-27.0, 34.0, 0.0), Vector3(27.0, -34.0, 0.0),
		 Vector3(27.0, 34.0, 0.0)},
		{Vector3(0, -50, 0), Vector3(0, 50, 0), Vector3(0, -50, 0), Vector3(0, 50, 0)}};

	Coord start_coord = source_coord;
	Coord end_coord = Lerp(start_coord, target_coord, WaveLengthFactor[Type]);
	if (Type == WAVE_SONIC) {
		end_coord.Z += 50;
	}

	if (Type == 3) { // Magnetic wave
		end_coord.Z += 50;
	}
	Lerp_From(start_coord, target_coord, WaveLengthFactor[Type]);

	float distance2d = TPoint3D<float>(start_coord.X - end_coord.X, start_coord.Y - end_coord.Y, 0).Length();
	int distance3d =
		TPoint3D<float>(start_coord.X - end_coord.X, start_coord.Y - end_coord.Y, start_coord.Z - end_coord.Z)
			.Length();

	// Set initial coordinates
	StartCoord = start_coord;
	Set_Coord(StartCoord);
	EndCoord = end_coord;

	WaveShape.Count = 4;
	WaveShape.Vertices = (Point2D *)MagPoints;

	// Calculate wave points
	TacticalMap->Coord_To_Pixel(start_coord, WaveStartMiddle);
	TacticalMap->Coord_To_Pixel(end_coord, WaveEndMiddle);

	double distance2d_f = int(distance2d);
	Vector3 far_offset(distance2d_f, 0, 0);
	Vector3 near_offset(0, 0, 0);

	Vector3 end_left = WaveSize[Type][0] + far_offset;
	Vector3 end_right = WaveSize[Type][1] + far_offset;
	Vector3 start_left = WaveSize[Type][2] + near_offset;
	Vector3 start_right = WaveSize[Type][3] + near_offset;

	double dz = end_coord.Z - start_coord.Z;

	end_left.Z = end_left.X * dz / distance3d + start_coord.Z;
	end_right.Z = end_right.X * dz / distance3d + start_coord.Z;
	start_left.Z = start_left.X * dz / distance3d + start_coord.Z;
	start_right.Z = start_right.X * dz / distance3d + start_coord.Z;

	TPoint3D<float> heading(end_coord.X - start_coord.X, end_coord.Y - start_coord.Y, 0);
	double dx = heading.X;
	double dist = heading.Length();

	if (dist < dx) {
		dx = dist;
	}
	if (-dx > dist) {
		dx = -dist;
	}

	double theta = fastmath::acos(dx / dist);
	if (start_coord.Y > end_coord.Y) {
		theta = -theta;
	}

	// Calculate wave vectors
	Matrix3D matrix;
	matrix.Make_Identity();
	matrix.Rotate_Z(theta);

	end_left = matrix * end_left;
	end_right = matrix * end_right;
	start_left = matrix * start_left;
	start_right = matrix * start_right;

	// Set wave coordinates
	WaveEndLeftCoord = Coord(start_coord.X + end_left.X, start_coord.Y + end_left.Y, end_left.Z);
	WaveEndRightCoord = Coord(start_coord.X + end_right.X, start_coord.Y + end_right.Y, end_right.Z);
	WaveStartLeftCoord = Coord(start_coord.X + start_left.X, start_coord.Y + start_left.Y, start_left.Z);
	WaveStartRightCoord = Coord(start_coord.X + start_right.X, start_coord.Y + start_right.Y, start_right.Z);

	// Convert coordinates to pixels
	TacticalMap->Coord_To_Pixel(WaveEndLeftCoord, WaveEndLeft);
	TacticalMap->Coord_To_Pixel(WaveEndRightCoord, WaveEndRight);
	TacticalMap->Coord_To_Pixel(WaveStartLeftCoord, WaveStartLeft);
	TacticalMap->Coord_To_Pixel(WaveStartRightCoord, WaveStartRight);

	// Set wave points
	WaveShape.Vertices[0] = WaveEndLeft;
	WaveShape.Vertices[1] = WaveEndRight;
	WaveShape.Vertices[2] = WaveStartRight;
	WaveShape.Vertices[3] = WaveStartLeft;

	// Copy facing
	Facing = Source->SecondaryFacing;
}

// SYNTHETIC: GAMEMD 0x00762AE0 SYMBOL
// _$E140

// FUNCTION: GAMEMD 0x00762AF0
void WaveClass::Update_Wave(void)
{
	if (WaveProgress - FadeProgress > 0.1 && Type == 0) {
		ShouldEnd = true;
	}

	if (WaveEC == (int)(1.0 / WaveStep) && Type == 3) {
		WaveEC = 64;
		WaveCount++;
	}

	if (Target != NULL && Source != NULL && WaveEC != (int)(1.0 / WaveStep) && Source->TarCom == Target) {
		if (Type != 3) {
			if ((Source->PositionCoord - Target->As_Coord()).Length() > CELL_LEPTON_DIAG * 6.0) {
				IsWaveActive = false;
				ShouldEnd = true;
			}
		}
	} else {
		IsWaveActive = false;
		ShouldEnd = true;
	}

	if (IsWaveActive) {
		if (Type == 3 && Target != NULL && Target->RTTI == RTTI_UNIT) {
			Draw_Magnetic(Target->As_Coord(), Source->Fire_Coord(0, Coord(0, 0, 0)));
		} else if (Type == 3) {
			Draw_Magnetic(Source->Fire_Coord(0, Coord(0, 0, 0)), Target->As_Coord());
		} else {
			Draw_NonMagnetic(Source->Fire_Coord(0, Coord(0, 0, 0)), Target->As_Coord());
		}
	}

	if (Type == 3) {
		if (WaveProgress < 1.0 && IsWaveActive) {
			WaveProgress += WaveStep;
			if (WaveProgress > 0.98) {
				WaveProgress = 1.0;
			}
			WaveShape.Vertices[0] = Lerp(WaveStartLeft, WaveEndLeft, WaveProgress);
			WaveShape.Vertices[1] = Lerp(WaveStartRight, WaveEndRight, WaveProgress);
		}
		if (ShouldEnd && WaveStep * 0.5 + WaveProgress >= FadeProgress) {
			FadeProgress += WaveStep;
			if (FadeProgress >= WaveProgress) {
				Delete_Me();
				return;
			}
			WaveShape.Vertices[2] = Lerp(WaveStartRight, WaveEndRight, FadeProgress);
			WaveShape.Vertices[3] = Lerp(WaveStartLeft, WaveEndLeft, FadeProgress);
		}
	} else {
		if (WaveProgress < 1.0 && IsWaveActive) {
			WaveProgress += WaveStep;
			if (WaveProgress > 0.98) {
				WaveProgress = 1.0;
			}
			WaveShape.Vertices[PolygonShapeStruct::END_LEFT] = Lerp(WaveStartLeft, WaveEndLeft, WaveProgress);
			WaveShape.Vertices[PolygonShapeStruct::END_MIDDLE] = Lerp(WaveStartMiddle, WaveEndMiddle, WaveProgress);
			WaveShape.Vertices[PolygonShapeStruct::END_RIGHT] = Lerp(WaveStartRight, WaveEndRight, WaveProgress);
		}
		if (ShouldEnd && WaveStep * 0.5 + WaveProgress >= FadeProgress) {
			FadeProgress += WaveStep;
			if (FadeProgress >= WaveProgress) {
				Delete_Me();
				return;
			}
			WaveShape.Vertices[PolygonShapeStruct::START_RIGHT] = Lerp(WaveStartRight, WaveEndRight, FadeProgress);
			WaveShape.Vertices[PolygonShapeStruct::START_MIDDLE] = Lerp(WaveStartMiddle, WaveEndMiddle, FadeProgress);
			WaveShape.Vertices[PolygonShapeStruct::START_LEFT] = Lerp(WaveStartLeft, WaveEndLeft, FadeProgress);
		}
	}

	Wave_Recalc_Affected_Cells();
}

// FUNCTION: GAMEMD 0x007631F0
RTTIType WaveClass::Fetch_RTTI(void) const
{
	return RTTI_WAVE;
}

// SYNTHETIC: GAMEMD 0x00763200
// ??_GWaveClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x007632D0
// ??0?$DynamicVectorClass@PAVCellClass@@@@QAE@IPBQAVCellClass@@@Z
// TEMPLATE: GAMEMD 0x00763330
// ?Add@?$DynamicVectorClass@PAVCellClass@@@@QAE_NABQAVCellClass@@@Z
template class DynamicVectorClass<CellClass *>;
