// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tactical.h"
#include "_alpha.h"
#include "_convert.h"
#include "_font.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "coord.h"
#include "data.h"
#include "dialog.h"
#include "display.h"
#include "draw.h"
#include "dsurface.h"
#include "fog.h"
#include "font.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "ionblast.h"
#include "isotype.h"
#include "laser.h"
#include "layer.h"
#include "mixfile.h"
#include "mouse.h"
#include "overtype.h"
#include "ovrlight.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "stimer.h"
#include "sun.h"
#include "swap.h"
#include "swizzle.h"
#include "terrain.h"
#include "terrtype.h"
#include "vector.h"
#include "vein.h"
#include "waypoint.h"
#include "zbuffer.h"
#include "ramp.hh"
#include "scrspeed.hh"
#include "fastmath.h"

DynamicVectorClass<DirtyAreaStruct> Tactical::DirtyAreas;

Tactical::Selectable Tactical::SelectableObjects[Tactical::MAX_SELECTABLE_OBJECTS];

DynamicVectorClass<ShadowControlClass *> Tactical::ShadowControls;

static double PixelsPerLepton;
static double ZPixelsPerLepton = fastmath::sin(RAD_60) * (PixelsPerLepton = 60.0 / CELL_LEPTON_DIAG);
static double ZLeptonsPerPixel = CELL_LEPTON_DIAG / (fastmath::sin(RAD_60) * 60.0);

// FUNCTION: 0x006D1A60 $E195
// FUNCTION: 0x006D1AA0 $E192
// FUNCTION: 0x006D1B00 $E198
// FUNCTION: 0x006D1B10 $E204
// FUNCTION: 0x006D1B50 $E201
// FUNCTION: 0x006D1BB0 $E207
// FUNCTION: 0x006D1BF0 $E210

// FUNCTION: 0x006D1C20
Tactical::Tactical(void) :
	LastAIFrame(-1),
	Flags(false, false),
	TacPixelX(0),
	TacPixelY(0),
	LastTacPixelX(0),
	LastTacPixelY(0),
	ZoomFactor(1.0),
	SelectableCount(0),
	MoveFrom(COORD_NONE),
	MoveTo(COORD_NONE),
	MoveSpeed(0),
	MoveFactor(0),
	CellRedrawCount(0),
	TacticalCoord(0,0),
	LastTacticalCoord(0,0),
	DesiredTacticalCoord(0,0),
	IsFirstRender(true),
	IsToRedraw(false),
	UnusedBool(false),
	VisibleCellRect(RECT_NONE),
	RubberBandStart(0,0),
	RubberBandEnd(0,0),
	WaypointAnimCounter(0),
	WaypointAnimTimer()
{
	for (int i = 0; i < ARRAY_SIZE(CellRedraw); i++) {
		CellRedraw[i] = NULL;
	}

	ScreenText[0] = '\0';
	EndGameGraphicsFrame = -1;

	CoordToPixelMatrix.Make_Identity();
	CoordToPixelMatrix.Rotate_X(RAD_60);
	CoordToPixelMatrix.Rotate_Z(RAD_45);
	CoordToPixelMatrix.Scale(60.0f / (float)CELL_LEPTON_DIAG);

	float x_scale = 8.5334f;
	float y_scale = 4.2667f;
	PixelToCoordMatrix.Set(
		 y_scale, x_scale, 0.0f, 0.0f,
		-y_scale, x_scale, 0.0f, 0.0f,
		 0.0f,    0.0f,    1.0f, 0.0f);

	TacticalMap = this;
}

// FUNCTION: 0x006D1E30
Tactical::Tactical(const NoInitClass &x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x006D1E80
Tactical::~Tactical(void)
{
	TacticalMap = NULL;
}

// FUNCTION: 0x006D1EB0
Point2D Tactical::Rectangular_To_Isometric(Point2D point)
{
	Point2D iso;
	iso.X = point.X * 60 / 2;
	iso.Y = point.X * 30 / 2;
	iso.X += point.Y * 60 / -2;
	iso.Y += point.Y * 30 / 2;
	return(iso);
}

static int Lepton_To_Z_Pixel(LEPTON lepton)
{
	int fudge = 0;
	if (lepton >= 728) {
		fudge = 1;
	}
	return(int(lepton * ZPixelsPerLepton + fudge + 0.5));
}

// FUNCTION: 0x006D1F10
Point2D Tactical::Coord_To_Pixel_Absolute(Coord const & coord)
{
	Point2D pixel = Rectangular_To_Isometric(Point2D(coord.X, coord.Y));
	pixel.X /= 256;
	pixel.Y /= 256;
	pixel.Y -= Lepton_To_Z_Pixel(coord.Z);
	return(pixel);
}

// FUNCTION: 0x006D20E0
int Tactical::Z_Lepton_To_Pixel(LEPTON lepton)
{
	return(Lepton_To_Z_Pixel(lepton));
}

// FUNCTION: 0x006D2790
void Tactical::Register_Dirty_Area(Rect area, bool refresh_shroud)
{
	int best_index = -1;
	int best = 0x7FFFFFFF;

	for (int i = 0; i < DirtyAreas.Count(); i++) {
		Rect rect = DirtyAreas[i].Area;
		Rect inter = Intersect(rect, area);
		Rect whole = Union(rect, area);

		if (inter.Is_Valid()) {
			if (inter == area) {
				if (!refresh_shroud || DirtyAreas[i].IsToRefreshShroud) {
					return;
				}
			} else if (inter == rect) {
				if (refresh_shroud || !DirtyAreas[i].IsToRefreshShroud) {
					DirtyAreas.Delete_Index(i);
					i--;
				}
			} else {
				int size = whole.Size();
				if (size < (rect.Size() + area.Size()) && size < best) {
					best = size;
					best_index = i;
				}
			}
		}
	}

	if (best_index == -1) {
		DirtyAreas.Add(DirtyAreaStruct(area, refresh_shroud));
	} else {
		DirtyAreas[best_index] = DirtyAreaStruct(Union(area, DirtyAreas[best_index].Area), refresh_shroud || DirtyAreas[best_index].IsToRefreshShroud);
	}
}

// FUNCTION: 0x006D5F30
bool Tactical::Blit_Tiles(void)
{
	return(LogicalSurface->Blit_From(TacticalRect, *TileSurface, TacticalRect));
}

// FUNCTION: 0x006D6A90
int Tactical::Get_Cell_Height(Coord const & coord)
{
	CellClass *cellptr = &Map[coord];
	return(cellptr->Get_Height(coord));
}

// FUNCTION: 0x006D6AF0
void Tactical::Add_Shadow_Control(ShadowControlClass control)
{
	ShadowControlClass *ctrl = new ShadowControlClass(control);
	ShadowControls.Add(ctrl);
}

// FUNCTION: 0x006D84D0
Point2D Tactical::Clamp_Pixel_To_Tactical(Point2D const & pixel)
{
	Point2D clamped = pixel;
	if (Clamp_To_Tactical_Rect(clamped) && !Debug_Map) {
		return(clamped);
	} else {
		return(pixel);
	}
}

// FUNCTION: 0x006D9910
void Tactical::Draw_Fogged_Objects(Rect const & area)
{
	::Draw_Fogged_Objects(area);
}

// FUNCTION: 0x006DA360
Cell Tactical::Clamp_Cursor_To_Tactical(Cell const & cell, Cell const * list)
{
	return(cell);
}

// FUNCTION: 0x006DA980
void Tactical::Reset_Dirty_Rectangles(void)
{
	while (DirtyAreas.Count() > 0) {
		DirtyAreas.Delete_Index(0);
	}
}

// FUNCTION: 0x006DBCE0
HRESULT STDMETHODCALLTYPE Tactical::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TacticalMapClass;
	return(S_OK);
}

// FUNCTION: 0x006DBE00
HRESULT STDMETHODCALLTYPE Tactical::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
