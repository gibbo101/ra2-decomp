// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
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

#include <cmath>

// FUNCTION: 0x004A8420 $E2
// FUNCTION: 0x004A8440 $E5
// FUNCTION: 0x004A8470 $E8
// FUNCTION: 0x004A8490 $E11
// FUNCTION: 0x004A84B0 $E14
// FUNCTION: 0x004A84D0 $E17
// FUNCTION: 0x004A84F0 $E20
// FUNCTION: 0x004A8510 $E23
// FUNCTION: 0x004A8550 $E26
// FUNCTION: 0x004A8580 $E29
// FUNCTION: 0x004A85B0 $E32
// FUNCTION: 0x004A85E0 $E35
// FUNCTION: 0x004A85F0 $E38
// FUNCTION: 0x004A8610 $E41

// FUNCTION: 0x004A8630 $E240
// FUNCTION: 0x004A8680 $E237
LayerClass DisplayClass::Layer[LAYER_COUNT];

void const * DisplayClass::ShadowShapes;
void const * DisplayClass::PlacementShapes;
void const * DAT_008A0418;

// FUNCTION: 0x004A86E0 $E246
// FUNCTION: 0x004A8710 $E243
DisplayClass::TacticalClass DisplayClass::TacButton;

// FUNCTION: 0x004A8730
DisplayClass::DisplayClass(void) :
	ZoneCell(CELL_NONE),
	ZoneOffset(0,0),
	CursorSize(0),
	ProximityCheck(false),
	ShroudCheck(false),
	CurrentFoundationCopy_CenterCell(CELL_NONE),
	CurrentFoundationCopy_TopLeftOffset(0,0),
	CurrentFoundationCopy_Data(0),
	CurrentBuildingCopy(0),
	CurrentBuildingTypeCopy(0),
	CurrentBuildingOwnerArrayIndexCopy(HOUSE_NONE),
	FollowingObject(false),
	FollowingObjectPtr(NULL),
	PendingObjectPtr(0),
	PendingObject(0),
	PendingHouse(HOUSE_NONE),
	IsRepairMode(false),
	IsSellMode(false),
	IsPowerMode(false),
	IsWaypointMode(false),
	PlaceBeaconMode(false),
	IsTargettingMode(SUPER_NONE),
	DraggedWaypoint(NULL),
	DraggedWaypointCoord(0,0,0),
	IsRubberBand(false),
	IsTentative(false),
	IsShadowPresent(false),
	Band(),
	NewX(0),
	NewY(0)
{
	ShadowShapes = NULL;
	PlacementShapes = NULL;
	DAT_008A0418 = NULL;
}

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};
extern Rect __fastcall FUN_0072AD20(void);

// FUNCTION: 0x004A8850
void DisplayClass::One_Time(void)
{
	BASECLASS::One_Time();

	PlacementShapes = FileSystem::LoadFile("PLACE.SHP", false);
	ShadowShapes = FileSystem::LoadFile("SHADOW.SHP", false);

	Rect rect = FUN_0072AD20();
	Set_View_Dimensions(rect);
}

// FUNCTION: 0x004A88C0
void DisplayClass::Init_Clear(void)
{
	BASECLASS::Init_Clear();

	PendingObjectPtr = 0;
	PendingObject = 0;
	PendingHouse = HOUSE_NONE;
	CursorSize = 0;
	IsTargettingMode = SUPER_NONE;
	IsRepairMode = false;
	IsRubberBand = false;
	IsTentative = false;
	IsSellMode = false;
	IsPowerMode = false;

	for (LayerType layer = LAYER_FIRST; layer < LAYER_COUNT; layer++) {
		Layer[layer].Init();
	}
}

// FUNCTION: 0x004A8930
void DisplayClass::Init_IO(void)
{
	BASECLASS::Init_IO();
	if (!Debug_Map) {
		TacButton.Zap();
		Add_A_Button(TacButton);
	}
}

// FUNCTION: 0x004A8960
void DisplayClass::Set_View_Dimensions(Rect const & dimensions)
{
	DebugString("Set_View_Dimensions(%d,%d,%d,%d)\n", dimensions.X, dimensions.Y, dimensions.Width, dimensions.Height);

	DSurface::ViewBounds = dimensions;

	if (TacticalMap != NULL) {
		TacticalMap->Set_View_Dimensions(DSurface::ViewBounds);

		Hide_Mouse();

		if (DepthBuffer != NULL) {
			DebugString("Deleting ZBuffer\n");
			delete DepthBuffer;
			DepthBuffer = NULL;
		}

		DepthBuffer = new ZBuffer(DSurface::ViewBounds);
		DepthBuffer->Set_Scroll(ZBUFFER_MAX);
		DebugString("Allocating ZBuffer (%dx%d)\n", DSurface::ViewBounds.Width, DSurface::ViewBounds.Height);

		if (AlphaBuffer != NULL) {
			DebugString("Deleting ABuffer\n");
			delete AlphaBuffer;
			AlphaBuffer = NULL;
		}

		AlphaBuffer = new ABuffer(DSurface::ViewBounds);
		DebugString("Allocating ABuffer (%dx%d)\n", DSurface::ViewBounds.Width, DSurface::ViewBounds.Height);

		Show_Mouse();
	}

	Map.Reposition_Sidebar();

	TacButton.X = DSurface::ViewBounds.X;
	TacButton.Y = DSurface::ViewBounds.Y;
	TacButton.Width = DSurface::ViewBounds.Width;
	TacButton.Height = DSurface::ViewBounds.Height;

	if (ToolTips != NULL) {
		ToolTip tooltip;
		tooltip.ID = 500;
		tooltip.Text = TXT_NONE;
		tooltip.Region = DSurface::ViewBounds;
		ToolTips->Remove(500);
		ToolTips->Add(&tooltip);
	}

	Session.Messages.Init(
		DSurface::ViewBounds.X + 3, DSurface::ViewBounds.Y,
		6,
		MAX_MESSAGE_LENGTH-14,
		7 * 2,
		-1, -1,
		0,
		20,
		MAX_MESSAGE_LENGTH - 14,
		DSurface::ViewBounds.Width - 6);

	Session.Messages.Set_Width(DSurface::ViewBounds.Width);

	DebugString("Set_View_Dimensions(exit)\n");
}

// FUNCTION: 0x004A8BF0
void DisplayClass::SetActiveFoundation(Cell const * list)
{
	if (CursorSize) {
		Cursor_Mark(ZoneCell+ZoneOffset, false);
	}
	ZoneOffset = CELL_NONE;

	if (list) {
		static Cell _list[120];

		memcpy(_list, list, sizeof(_list));
		CursorSize = _list;
		Cell dim = Get_Occupy_Dimensions(CursorSize);
		dim -= Cell(1,1);
		ZoneOffset = -dim / 2;
		Cursor_Mark(ZoneCell+ZoneOffset, true);
	} else {
		CursorSize = 0;
	}
}

// FUNCTION: 0x004A8D40 $E249
// FUNCTION: 0x004A8D50
void DisplayClass::FUN_004A8D50(Cell const * list)
{
	if (CurrentFoundationCopy_Data) {
		Mark_Foundation_Copy(CurrentFoundationCopy_CenterCell+CurrentFoundationCopy_TopLeftOffset, false);
	}
	CurrentFoundationCopy_TopLeftOffset = CELL_NONE;

	if (list) {
		static Cell _list[50];

		memcpy(_list, list, sizeof(_list));
		CurrentFoundationCopy_Data = _list;
		Cell dim = Get_Occupy_Dimensions(CurrentFoundationCopy_Data);
		dim -= Cell(1,1);
		CurrentFoundationCopy_TopLeftOffset = -dim / 2;
		Mark_Foundation_Copy(CurrentFoundationCopy_CenterCell+CurrentFoundationCopy_TopLeftOffset, true);
	} else {
		CurrentFoundationCopy_Data = 0;
	}
}

// FUNCTION: 0x004A8EA0 $E251
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

// FUNCTION: 0x004A95A0
void DisplayClass::Cursor_Mark(Cell const & pos, bool on)
{
	Cell const * ptr;
	CellClass * cellptr;

	if (pos == CELL_NONE) return;

	ptr = CursorSize;
	while (*ptr != REFRESH_EOL) {
		Cell cell = Cell(pos + *ptr++);
		if (In_Radar(cell)) {
			cellptr = &(*this)[cell];
			if (on) {
				cellptr->IsCursorHere = true;
			} else {
				cellptr->IsCursorHere = false;
			}
		}
	}
}

// FUNCTION: 0x004A9650
void DisplayClass::Mark_Foundation_Copy(Cell const & pos, bool on)
{
	Cell const * ptr;
	CellClass * cellptr;

	if (pos == CELL_NONE) return;

	ptr = CurrentFoundationCopy_Data;
	while (*ptr != REFRESH_EOL) {
		Cell cell = Cell(pos + *ptr++);
		if (In_Radar(cell)) {
			cellptr = &(*this)[cell];
			if (on) {
				cellptr->IsFoundationCopyHere = true;
			} else {
				cellptr->IsFoundationCopyHere = false;
			}
		}
	}
}

// FUNCTION: 0x004A9700
void DisplayClass::AI(KeyNumType & input, Point2D const & xy)
{
	BASECLASS::AI(input, xy);
}

// FUNCTION: 0x004A9720
void DisplayClass::Submit(ObjectClass const * object)
{
	if (object) {
		if (object->Layer != LAYER_NONE) {
			void (DisplayClass::*remove)(ObjectClass const *) = &DisplayClass::Remove;
			(this->*remove)(object);
		}
		LayerType layer = object->In_Which_Layer();
		if (layer != LAYER_NONE) {
			if (Layer[layer].Submit(object, (layer == LAYER_GROUND))) {
				((ObjectClass *)object)->Layer = layer;
			}
		}
	}
}

// FUNCTION: 0x004A9770
void DisplayClass::Remove(ObjectClass const * object)
{
	if (object) {
		if (object->Layer != LAYER_NONE) {
			if (Layer[object->Layer].Delete((ObjectClass *)object)) {
				((ObjectClass *)object)->Layer = LAYER_NONE;
			}

			if (object->Layer != LAYER_NONE) {
				for (int l = LAYER_FIRST; l < LAYER_COUNT; l++) {
					while (Layer[l].Delete((ObjectClass *)object)) { }
				}
				((ObjectClass *)object)->Layer = LAYER_NONE;
			}
		}
	}
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

// FUNCTION: 0x004A9890
bool DisplayClass::Map_Cell(Cell const & cell, HouseClass * house)
{
	CellClass * cellptr = &(*this)[cell];

	bool wasfogged = cellptr->IsFogMapped == false;
	bool changed = !cellptr->IsFogMapped || !cellptr->IsMapped;
	bool newlymapped = changed;

	cellptr->IsToFog = false;

	cellptr->IsFogMapped = true;
	cellptr->IsMapped = true;

	signed char sframe = TacticalMap->Cell_Shadow(cell, false);
	if (sframe != cellptr->ShadowFrame) {
		changed = true;
		cellptr->ShadowFrame = sframe;
	}
	if (cellptr->ShadowFrame == -1) {
		cellptr->IsVisible = true;
	}

	signed char fframe = TacticalMap->Cell_Shadow(cell, true);
	if (fframe != cellptr->FogFrame) {
		changed = true;
		cellptr->FogFrame = fframe;
	}
	if (cellptr->FogFrame == -1) {
		cellptr->IsFogVisible = true;
	}

	if (changed) {
		TacticalMap->Flag_Cell(*cellptr);
	}

	for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
		int	shadow;
		int fog;

		Cell c = AdjacentCell[(int)dir & 7] + cell;
		CellClass * cptr = &(*this)[c];

		if (c != cell && !cptr->IsVisible) {
			shadow = TacticalMap->Cell_Shadow(c, false);

			if (shadow == -1) {
				if (!cptr->IsMapped) {
					Map_Cell(c, house);
				} else {
					cptr->IsVisible = true;
					TacticalMap->Flag_Cell(*cptr);
					for (FacingType dir2 = FACING_FIRST; dir2 < FACING_COUNT; dir2++) {
						Cell cc = AdjacentCell[(int)dir2 & 7] + c;
						CellClass * scptr = &(*this)[cc];
						signed char sframe = TacticalMap->Cell_Shadow(cc, false);
						if (sframe != scptr->ShadowFrame) {
							scptr->ShadowFrame = sframe;
							TacticalMap->Flag_Cell(*scptr);
						}
					}
				}
			} else {
				if (shadow != -2 && !cptr->IsMapped) {
					Map_Cell(c, house);
				} else {
					if (shadow >= 0 && shadow != cptr->ShadowFrame) {
						cptr->ShadowFrame = shadow;
						TacticalMap->Flag_Cell(*cptr);
					}
				}
			}
		}

		if (c != cell && !cptr->IsFogVisible) {
			fog = TacticalMap->Cell_Shadow(c, true);

			if (fog == -1) {
				if (!cptr->IsFogMapped) {
					Map_Cell(c, house);
				} else {
					cptr->IsFogVisible = true;
					TacticalMap->Flag_Cell(*cptr);
					for (FacingType dir2 = FACING_FIRST; dir2 < FACING_COUNT; dir2++) {
						Cell cc = AdjacentCell[(int)dir2 & 7] + c;
						CellClass * fcptr = &(*this)[cc];
						signed char fframe = TacticalMap->Cell_Shadow(cc, true);
						if (fframe != fcptr->FogFrame) {
							fcptr->FogFrame = fframe;
							TacticalMap->Flag_Cell(*fcptr);
						}
					}
				}
			} else {
				if (fog != -2 && !cptr->IsFogMapped) {
					Map_Cell(c, house);
				} else {
					if (fog >= 0 && fog != cptr->FogFrame) {
						cptr->FogFrame = fog;
						TacticalMap->Flag_Cell(*cptr);
					}
				}
			}
		}
	}

	if (changed) {
		Map.Reveal_Nearby_Technos(cellptr, house, newlymapped);
	}

	if (cellptr->IsFogMapped && wasfogged && Scen->Special.IsFogOfWar) {
		cellptr->Unfog_Cell();
	}

	return(changed);
}

// FUNCTION: 0x004A9CA0
bool DisplayClass::RevealFogShroud(Cell const & cell, HouseClass * house, bool increase)
{
	CellClass * cellptr = &(*this)[cell];

	bool wasfogged = cellptr->IsFogMapped == false;
	bool changed = !cellptr->IsFogMapped || !cellptr->IsMapped;
	bool newlymapped = changed;

	cellptr->IsToFog = false;
	cellptr->IsFogMapped = true;

	if (increase) {
		cellptr->IncreaseShroudCounter();
	} else {
		cellptr->ReduceShroudCounter();
	}

	signed char sframe = TacticalMap->Cell_Shadow(cell, false);
	if (sframe != cellptr->ShadowFrame) {
		changed = true;
		cellptr->ShadowFrame = sframe;
	}
	if (cellptr->ShadowFrame == -1) {
		cellptr->IsVisible = true;
	}

	signed char fframe = TacticalMap->Cell_Shadow(cell, true);
	if (fframe != cellptr->FogFrame) {
		changed = true;
		cellptr->FogFrame = fframe;
	}
	if (cellptr->FogFrame == -1) {
		cellptr->IsFogVisible = true;
	}

	if (changed) {
		TacticalMap->Flag_Cell(*cellptr);
		Map.Reveal_Nearby_Technos(cellptr, house, newlymapped);
	}

	if (cellptr->IsFogMapped && wasfogged && Scen->Special.IsFogOfWar) {
		cellptr->Unfog_Cell();
	}

	return(changed);
}

// FUNCTION: 0x004A9DD0
bool DisplayClass::Fog_Map_Cell(Cell const & cell, HouseClass * house)
{
	CellClass * cellptr = &(*this)[cell];

	bool wasfogged = cellptr->IsFogMapped == false;
	bool changed = !cellptr->IsFogMapped;
	bool newlymapped = changed;

	cellptr->IsToFog = false;

	cellptr->IsFogMapped = true;

	signed char fframe = TacticalMap->Cell_Shadow(cell, true);
	if (fframe != cellptr->FogFrame) {
		changed = true;
		cellptr->FogFrame = fframe;
	}
	if (cellptr->FogFrame == -1) {
		cellptr->IsFogVisible = true;
	}

	if (changed) {
		TacticalMap->Flag_Cell(*cellptr);
	}

	for (int dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
		int fog;

		Cell c = Adjacent_Cell(cell, (FacingType)dir);
		CellClass * cptr = &(*this)[c];

		if (c != cell && !cptr->IsFogVisible) {
			fog = TacticalMap->Cell_Shadow(c, true);

			if (fog == -1) {
				if (!cptr->IsFogMapped) {
					Fog_Map_Cell(c, house);
				} else {
					cptr->IsFogVisible = true;
					TacticalMap->Flag_Cell(*cptr);
					for (int dir2 = FACING_FIRST; dir2 < FACING_COUNT; dir2++) {
						Cell cc = Adjacent_Cell(c, (FacingType)dir2);
						CellClass * fcptr = &(*this)[cc];
						signed char fframe = TacticalMap->Cell_Shadow(cc, true);
						if (fframe != fcptr->FogFrame) {
							fcptr->FogFrame = fframe;
							TacticalMap->Flag_Cell(*fcptr);
						}
					}
				}
			} else {
				if (fog != -2 && !cptr->IsFogMapped) {
					Fog_Map_Cell(c, house);
				} else {
					if (fog >= 0 && fog != cptr->FogFrame) {
						cptr->FogFrame = fog;
						TacticalMap->Flag_Cell(*cptr);
					}
				}
			}
		}
	}

	if (changed) {
		Map.Reveal_Nearby_Technos(cellptr, house, newlymapped);
	}

	if (cellptr->IsFogMapped && wasfogged && Scen->Special.IsFogOfWar) {
		cellptr->Unfog_Cell();
	}

	return(changed);
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
