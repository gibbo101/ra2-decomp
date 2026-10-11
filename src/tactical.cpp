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
