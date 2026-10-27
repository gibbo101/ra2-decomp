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
	int red = rgb.Get_Red();
	int green = rgb.Get_Green();
	int blue = rgb.Get_Blue();
	red += ((mult * 64) >> 8);
	blue += ((mult * 96) >> 8);
	*buffer = DSurface::Build_Hicolor_Pixel(MIN(255, red), MIN(255, green), MIN(255, blue));
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

// FUNCTION: GAMEMD 0x00762060
void WaveClass::FUN_00762060(void)
{
}

// FUNCTION: GAMEMD 0x00762AE0
void WaveClass::FUN_00762AE0(void)
{
}

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
