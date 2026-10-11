// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "display.h"
#include "_alpha.h"
#include "_astar.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_tooltip.h"
#include "_zbuffer.h"
#include "airctype.h"
#include "animtype.h"
#include "astar.h"
#include "bsurface.h"
#include "building.h"
#include "builtype.h"
#include "bullettype.h"
#include "cctooltip.h"
#include "cell.h"
#include "conquer.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "effects.h"
#include "foot.h"
#include "globals.h"
#include "goptions.h"
#include "incdec.h"
#include "infatype.h"
#include "inline.h"
#include "isotype.h"
#include "keyboard.h"
#include "language\language.h"
#include "logic.h"
#include "mixfile.h"
#include "overtype.h"
#include "palette.h"
#include "queue.h"
#include "rules.h"
#include "session.h"
#include "smudtype.h"
#include "sidebar.h"
#include "suprtype.h"
#include "surface.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "terrtype.h"
#include "unittype.h"
#include "vein.h"
#include "vector3.h"
#include "vox.h"
#include "waypoint.h"
#include "xpipe.h"
#include "xstraw.h"
#include "zbuffer.h"
#include "color.hh"
#include "super.hh"

// FUNCTION: 0x004A94F0
Cell DisplayClass::Get_Occupy_Dimensions(Cell const * list) const
{
	int min_x = MAP_CELL_W;
	int max_x = -MAP_CELL_W;
	int min_y = MAP_CELL_H;
	int max_y = -MAP_CELL_H;
	int x,y;

	Cell cell(0, 0);

	if (list != NULL) {
		while (*list != REFRESH_EOL) {
			x = list->X;
			y = list->Y;

			max_x = MAX(max_x, x);
			min_x = MIN(min_x, x);
			max_y = MAX(max_y, y);
			min_y = MIN(min_y, y);

			list++;
		}

		cell.X = MAX(1, max_x - min_x + 1);
		cell.Y = MAX(1, max_y - min_y + 1);
	}
	return(cell);
}

// FUNCTION: 0x004A9700
void DisplayClass::AI(KeyNumType & input, Point2D const & xy)
{
	BASECLASS::AI(input, xy);
}

// FUNCTION: 0x004A9840
bool DisplayClass::Scroll_Map(FacingType facing, int & distance, bool really)
{
	if (distance == 0 && !really) return(false);

	if (!really) {
		return(TacticalMap->Scroll_Dir(facing) != FACING_NONE);
	}

	TacticalMap->Scroll_Map(facing, distance);
	return(true);
}

// FUNCTION: 0x004AA290
ObjectClass * DisplayClass::Cell_Object(Cell const & cell, Point2D const & point) const
{
	return(*this)[cell].Cell_Object(point);
}

// FUNCTION: 0x004ACA90
bool DisplayClass::Is_Spot_Free(Coord const & coord, bool bridge) const
{
	return(Map[coord].Is_Spot_Free(CellClass::Spot_Index(coord), bridge));
}

// FUNCTION: 0x004AE4C0
void DisplayClass::Update_Cell_Colors(void)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Recalc_Light();
		cptr = Map.Iterate();
	}
}

// FUNCTION: 0x004AE6B0
void DisplayClass::Reposition_Sidebar(void)
{
	if (UnusedWindow) {
		CloseWindow(UnusedWindow);
		UnusedWindow = 0;
	}
}

// FUNCTION: 0x004AE6D0
LRESULT DisplayClass::Windows_Message_Proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return(DefWindowProc(hWnd, Msg, wParam, lParam));
}

// FUNCTION: 0x004AE6F0
HRESULT DisplayClass::Load(IStream * stream)
{
	HRESULT result = S_OK;
	for (LayerType layer = LAYER_FIRST; layer < LAYER_COUNT; layer++) {
		result = Layer[layer].Load(stream);
	}
	return(result);
}

// FUNCTION: 0x004AE720
HRESULT DisplayClass::Save(IStream * stream)
{
	HRESULT result = S_OK;
	for (LayerType layer = LAYER_FIRST; layer < LAYER_COUNT; layer++) {
		result = Layer[layer].Save(stream);
		if (FAILED(result)) break;
	}
	return(result);
}
