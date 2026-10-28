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
#include "_xmouse.h"
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
#undef LoadString
#include "stringtable.h"

#include <cmath>

namespace Unsorted { extern bool MoveFeedback; extern bool ArmageddonMode; }
extern bool DAT_00A8ED5D;
template<class T> inline T Bound2(T original, T minval, T maxval)
{
	T ret = original;
	if (ret < minval) ret = minval;
	if (ret >= maxval) ret = maxval-1;
	return(ret);
}

extern bool DAT_00884D44;
void __fastcall UnselectAll(void);
class BeaconManagerClass
{
	public:
		bool Can_Place_Beacon(int house);
};
extern BeaconManagerClass BeaconManager;

// SYNTHETIC: GAMEMD 0x004A8420 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004A8440 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004A8470 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004A8490 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004A84B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004A84D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004A84F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004A8510 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004A8550 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004A8580 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004A85B0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004A85E0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004A85F0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004A8610 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x004A8630 SYMBOL
// _$E240
// SYNTHETIC: GAMEMD 0x004A8680 SYMBOL
// _$E237
LayerClass DisplayClass::Layer[LAYER_COUNT];

void const * DisplayClass::ShadowShapes;
void const * DisplayClass::PlacementShapes;
void const * DAT_008A0418;

// SYNTHETIC: GAMEMD 0x004A86E0 SYMBOL
// _$E246
// SYNTHETIC: GAMEMD 0x004A8710 SYMBOL
// _$E243
DisplayClass::TacticalClass DisplayClass::TacButton;

// FUNCTION: GAMEMD 0x004A8730
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

// FUNCTION: GAMEMD 0x004A8850
void DisplayClass::One_Time(void)
{
	BASECLASS::One_Time();

	PlacementShapes = FileSystem::LoadFile("PLACE.SHP", false);
	ShadowShapes = FileSystem::LoadFile("SHADOW.SHP", false);

	Rect rect = FUN_0072AD20();
	Set_View_Dimensions(rect);
}

// FUNCTION: GAMEMD 0x004A88C0
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

// FUNCTION: GAMEMD 0x004A8930
void DisplayClass::Init_IO(void)
{
	BASECLASS::Init_IO();
	if (!Debug_Map) {
		TacButton.Zap();
		Add_A_Button(TacButton);
	}
}

// FUNCTION: GAMEMD 0x004A8960
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

// FUNCTION: GAMEMD 0x004A8BF0
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

// SYNTHETIC: GAMEMD 0x004A8D40 SYMBOL
// _$E249
// FUNCTION: GAMEMD 0x004A8D50
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

// SYNTHETIC: GAMEMD 0x004A8EA0 SYMBOL
// _$E251
// FUNCTION: GAMEMD 0x004A94F0
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

// FUNCTION: GAMEMD 0x004A95A0
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

// FUNCTION: GAMEMD 0x004A9650
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

// FUNCTION: GAMEMD 0x004A9700
void DisplayClass::AI(KeyNumType & input, Point2D const & xy)
{
	BASECLASS::AI(input, xy);
}

// FUNCTION: GAMEMD 0x004A9720
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

// FUNCTION: GAMEMD 0x004A9770
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

// FUNCTION: GAMEMD 0x004A9840
bool DisplayClass::Scroll_Map(FacingType facing, int & distance, bool really)
{
	if (distance == 0 && !really) return(false);

	if (!really) {
		return(TacticalMap->Scroll_Dir(facing) != FACING_NONE);
	}

	TacticalMap->Scroll_Map(facing, distance);
	return(true);
}

// FUNCTION: GAMEMD 0x004A9890
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

// FUNCTION: GAMEMD 0x004A9CA0
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

// FUNCTION: GAMEMD 0x004A9DD0
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

// FUNCTION: GAMEMD 0x004AA050
bool DisplayClass::Shadow_Map_Cell(Cell const & cell, HouseClass * house)
{
	CellClass * cellptr = &(*this)[cell];

	bool changed = !cellptr->IsMapped;
	bool newlymapped = changed;

	cellptr->IsMapped = true;

	signed char sframe = TacticalMap->Cell_Shadow(cell, false);
	if (sframe != cellptr->ShadowFrame) {
		changed = true;
		cellptr->ShadowFrame = sframe;
	}
	if (cellptr->ShadowFrame == -1) {
		cellptr->IsVisible = true;
	}

	if (changed) {
		TacticalMap->Flag_Cell(*cellptr);
	}

	for (int dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
		int shadow;

		Cell c = AdjacentCell[dir & 7] + cell;
		CellClass * cptr = &(*this)[c];

		if (c != cell && !cptr->IsVisible) {
			shadow = TacticalMap->Cell_Shadow(c, false);

			if (shadow == -1) {
				if (!cptr->IsMapped) {
					Shadow_Map_Cell(c, house);
				} else {
					cptr->IsVisible = true;
					TacticalMap->Flag_Cell(*cptr);
					for (int dir2 = FACING_FIRST; dir2 < FACING_COUNT; dir2++) {
						Cell cc = Adjacent_Cell(c, (FacingType)dir2);
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
					Shadow_Map_Cell(c, house);
				} else {
					if (shadow >= 0 && shadow != cptr->ShadowFrame) {
						cptr->ShadowFrame = shadow;
						TacticalMap->Flag_Cell(*cptr);
					}
				}
			}
		}
	}

	if (changed) {
		Map.Reveal_Nearby_Technos(cellptr, house, newlymapped);
	}

	return(changed);
}

// FUNCTION: GAMEMD 0x004AA290
ObjectClass * DisplayClass::Cell_Object(Cell const & cell, Point2D const & point) const
{
	return(*this)[cell].Cell_Object(point);
}

// FUNCTION: GAMEMD 0x004AA2B0
ObjectClass * DisplayClass::Next_Object(ObjectClass * object) const
{
	ObjectClass * firstobj = NULL;
	bool foundmatch = false;

	if (object == NULL) {
		foundmatch = true;
	}

	static LayerType _layers[] = {
		LAYER_GROUND,
		LAYER_AIR,
		LAYER_TOP
	};

	for (int lindex = 0; lindex < ARRAY_SIZE(_layers); lindex++) {
		for (int uindex = 0; uindex < Layer[_layers[lindex]].Count(); uindex++) {
			ObjectClass * obj = Layer[_layers[lindex]][uindex];

			if (obj != NULL) {
				TechnoClass * techno = (obj->AbstractFlags & 1) ? (TechnoClass *)obj : NULL;
				if (obj->Is_Players_Army() && techno != NULL && techno->CanBeSelectedNow()) {
					if (firstobj == NULL) firstobj = obj;
					if (foundmatch) return(obj);
					if (object == obj) foundmatch = true;
				}
			}
		}
	}
	return(firstobj);
}

// FUNCTION: GAMEMD 0x004AA380
ObjectClass * DisplayClass::Prev_Object(ObjectClass * object) const
{
	ObjectClass * firstobj = NULL;
	bool foundmatch = false;

	if (object == NULL) {
		foundmatch = true;
	}

	static LayerType _layers[] = {
		LAYER_TOP,
		LAYER_AIR,
		LAYER_GROUND
	};

	for (int lindex = 0; lindex < ARRAY_SIZE(_layers); lindex++) {
		for (int uindex = Layer[_layers[lindex]].Count() - 1; uindex >= 0; uindex--) {
			ObjectClass * obj = Layer[_layers[lindex]][uindex];

			if (obj != NULL) {
				TechnoClass * techno = (obj->AbstractFlags & 1) ? (TechnoClass *)obj : NULL;
				if (obj->Is_Players_Army() && techno != NULL && techno->CanBeSelectedNow()) {
					if (firstobj == NULL) firstobj = obj;
					if (foundmatch) return(obj);
					if (object == obj) foundmatch = true;
				}
			}
		}
	}
	return(firstobj);
}

// FUNCTION: GAMEMD 0x004AAB30
bool DisplayClass::Good_Reinforcement_Cell(Cell const & outcell, Cell const & incell, SpeedType loco, int zone, MZoneType mzone) const
{
	if (loco == SPEED_WINGED) {
		return(true);
	}

	if (!(*this)[outcell].Is_Clear_To_Move(loco, true, true)) {
		return(false);
	}

	if (!(*this)[incell].Is_Clear_To_Move(loco, true, true, zone, mzone)) {
		return(false);
	}

	if ((*this)[outcell].Cell_Techno() != NULL) {
		return(false);
	}
	if ((*this)[incell].Cell_Techno() != NULL) return(false);

	return(true);
}

// FUNCTION: GAMEMD 0x004AAC10
int DisplayClass::TacticalClass::Action(unsigned flags, KeyNumType & key, int modifier)
{
	int x, y;
	unsigned char fog, shadow;
	ObjectClass * object = NULL;
	Point2D pixel(0, 0);

	if (flags & (LEFTPRESS|LEFTRELEASE|RIGHTPRESS|RIGHTRELEASE)) {
		x = Keyboard->MouseQX;
		y = Keyboard->MouseQY;
		pixel = Point2D(x, y);
	} else {
		x = Get_Mouse_X();
		y = Get_Mouse_Y();
		pixel = Get_Mouse_Point();
	}

	Coord coord;
	Cell cell;
	TacticalMap->Pixel_To_Coord(pixel);

	pixel -= DSurface::ViewBounds.Top_Left();

	Map.ProcessClickCoords(&pixel, &cell, &coord, &object, &fog, &shadow);

	if (cell != Map.ZoneCell) {
		Map.Set_Cursor_Pos(cell);
	}

	return(GadgetClass::Action(0, key, 0));
}

// FUNCTION: GAMEMD 0x004AAD20
void DisplayClass::Mouse_Right_Press(Point2D const & point)
{
}

// FUNCTION: GAMEMD 0x004AAD30
void DisplayClass::Mouse_Right_Release(Point2D const & point)
{
	if (PendingObjectPtr && PendingObjectPtr->Is_Techno()) {
		PendingObjectPtr = NULL;
		PendingObject = NULL;
		PendingHouse = HOUSE_NONE;
		SetActiveFoundation(NULL);
		Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
		return;
	}
	if (IsRepairMode) {
		Repair_Mode_Control(0);
		Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
		return;
	}
	if (IsSellMode) {
		Sell_Mode_Control(0);
		Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
		return;
	}
	if (IsPowerMode) {
		Power_Mode_Control(0);
		Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
		return;
	}
	if (IsTargettingMode != SUPER_NONE) {
		IsTargettingMode = SUPER_NONE;
		Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
		return;
	}
	if (IsWaypointMode) {
		Waypoint_Mode_Control(0);
		Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
		return;
	}
	if (PlaceBeaconMode) {
		Place_Beacon_Mode_Control(0);
		Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
		return;
	}
	UnselectAll();
	Set_Default_Mouse(MOUSE_NORMAL, DAT_00884D44);
}

// FUNCTION: GAMEMD 0x004AC2B0
void __fastcall Bandbox_Selection_Callback(ObjectClass * object)
{
	HouseClass * owner = object->Owner_HouseClass();
	if (owner && owner->Is_Player_Control() && object->CanBeSelected() && object->CanBeSelectedNow() && !object->IsInLimbo && object->Select()) {
		Unsorted::MoveFeedback = false;
	}
}

// FUNCTION: GAMEMD 0x004AC310
void DisplayClass::Mouse_Left_Press(Point2D const & point)
{
	if (!IsRepairMode && !IsPowerMode && !IsSellMode && IsTargettingMode == SUPER_NONE && !PendingObject) {
		IsTentative = true;
		Band.BandX = point.X;
		Band.BandY = point.Y;
		NewX = point.X;
		NewY = point.Y;
	}
}

// FUNCTION: GAMEMD 0x004AC380
void DisplayClass::Mouse_Left_Held(Point2D const & point)
{
	if (IsRubberBand && !IsWaypointMode && !PlaceBeaconMode) {
		Rect rect = DSurface::ViewBounds;
		Point2D npoint = point;
		int x = Bound2(npoint.X, 0, rect.Width);
		int y = Bound2(npoint.Y, 0, rect.Height);
		npoint.X = x;
		npoint.Y = y;
		if (npoint != (Point2D &)NewX) {
			TacticalMap->IsToRedraw = true;
			TacticalMap->Modify_Rubber_Band(npoint);
		}
	} else {
		if (IsTentative) {
			if ((point - (Point2D &)Band).Length() > 4) {
				IsRubberBand = true;
				IsTentative = false;
				if (!IsWaypointMode && !PlaceBeaconMode) {
					TacticalMap->IsToRedraw = true;
					TacticalMap->Start_Rubber_Band((Point2D &)Band);
					Map.Override_Mouse_Shape(MOUSE_NORMAL);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004AC660
void DisplayClass::Sell_Mode_Control(int control)
{
	bool mode = IsSellMode;
	switch (control) {
		case 0:
			mode = false;
			break;

		case -1:
			mode = (IsSellMode == false);
			break;

		case 1:
			mode = true;
			break;
	}

	if (mode != IsSellMode && !PendingObject) {
		Set_Default_Mouse(MOUSE_NORMAL, false);
		IsRepairMode = false;
		IsPowerMode = false;
		IsWaypointMode = false;
		PlaceBeaconMode = false;
		if (mode && PlayerPtr->CurBuildings > 0) {
			IsSellMode = true;
			UnselectAll();
		} else {
			IsSellMode = false;
			Revert_Mouse_Shape();
		}
	}
}

// FUNCTION: GAMEMD 0x004AC700
void DisplayClass::Waypoint_Mode_Control(int control, bool edit_selected_path)
{
	bool mode = IsWaypointMode;
	switch (control) {
		case 0:
			mode = false;
			break;

		case -1:
			mode = (mode == false);
			break;

		case 1:
			mode = true;
			break;
	}

	if (mode != IsWaypointMode && !PendingObject) {
		IsRepairMode = false;
		IsSellMode = false;
		IsPowerMode = false;
		PlaceBeaconMode = false;
		PathType path = PlayerPtr->New_Waypoint_Path();
		if (mode) {
			if (path == PATH_NONE && (!edit_selected_path || PlayerPtr->SelectedPath == PATH_NONE)) {
				IsWaypointMode = false;
				Revert_Mouse_Shape();
				DraggedWaypoint = NULL;
			} else {
				IsWaypointMode = true;
				if (!edit_selected_path) {
					PlayerPtr->SelectedPath = path;
				}
				UnselectAll();
			}
		} else {
			IsWaypointMode = false;
			Revert_Mouse_Shape();
			WaypointClass *wptr = DraggedWaypoint;
			if (wptr != NULL) {
				DraggedWaypoint->Location = DraggedWaypointCoord;
				DraggedWaypoint = NULL;
				MouseCursor->Show_Mouse();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004AC820
void DisplayClass::Power_Mode_Control(int control)
{
	bool mode = IsPowerMode;
	switch (control) {
		case 0:
			mode = false;
			break;

		case -1:
			mode = (IsPowerMode == false);
			break;

		case 1:
			mode = true;
			break;
	}

	if (mode != IsPowerMode && !PendingObject) {
		Set_Default_Mouse(MOUSE_NORMAL, false);
		IsRepairMode = false;
		IsSellMode = false;
		IsWaypointMode = false;
		PlaceBeaconMode = false;
		if (mode && PlayerPtr->CurBuildings > 0) {
			IsPowerMode = true;
			UnselectAll();
		} else {
			IsPowerMode = false;
			Revert_Mouse_Shape();
		}
	}
}

// FUNCTION: GAMEMD 0x004AC8C0
void DisplayClass::Repair_Mode_Control(int control)
{
	bool mode = IsRepairMode;
	switch (control) {
		case 0:
			mode = false;
			break;

		case -1:
			mode = (IsRepairMode == false);
			break;

		case 1:
			mode = true;
			break;
	}

	if (mode != IsRepairMode && !PendingObject) {
		IsSellMode = false;
		IsPowerMode = false;
		IsWaypointMode = false;
		PlaceBeaconMode = false;
		Set_Default_Mouse(MOUSE_NORMAL, false);
		if (mode && PlayerPtr->CurBuildings > 0) {
			IsRepairMode = true;
			UnselectAll();
		} else {
			IsRepairMode = false;
			Revert_Mouse_Shape();
		}
	}
}

// FUNCTION: GAMEMD 0x004AC960
void DisplayClass::Place_Beacon_Mode_Control(int control)
{
	bool mode = IsRepairMode;
	switch (control) {
		case 0:
			mode = false;
			break;

		case -1:
			mode = (mode == false);
			break;

		case 1:
			mode = true;
			break;
	}

	if (mode != PlaceBeaconMode && !PendingObject) {
		if (mode && !BeaconManager.Can_Place_Beacon(PlayerPtr->HeapID)) {
			mode = false;
		}
		IsSellMode = false;
		IsPowerMode = false;
		IsWaypointMode = false;
		IsRepairMode = false;
		Set_Default_Mouse(MOUSE_NORMAL, false);
		if (mode) {
			PlaceBeaconMode = true;
			UnselectAll();
		} else {
			PlaceBeaconMode = false;
			Revert_Mouse_Shape();
		}
	}
}

// FUNCTION: GAMEMD 0x004ACA10
Coord DisplayClass::Closest_Free_Spot(Coord const & coord, bool any) const
{
	CellClass *cellptr = &Map[coord];
	return(cellptr->Closest_Free_Spot(coord, any, (cellptr->IsUnderBridge && coord.Z >= LEVEL_LEPTON_H * (cellptr->Height + BRIDGE_CELL_HEIGHT))));
}

// FUNCTION: GAMEMD 0x004ACA90
bool DisplayClass::Is_Spot_Free(Coord const & coord, bool bridge) const
{
	return(Map[coord].Is_Spot_Free(CellClass::Spot_Index(coord), bridge));
}

// FUNCTION: GAMEMD 0x004ACAC0
void DisplayClass::Encroach_Shadow(void)
{
	int x;
	int y;
	Cell cell;

	for (y = 0; y < MAP_CELL_H; y++) {
		for (x = 0; x < MAP_CELL_W; x++) {
			cell.X = x;
			cell.Y = y;
			if (!In_Radar(cell)) continue;
			Cell c;
			c.X = x;
			c.Y = y;
			CellClass * cellptr = &(*this)[c];
			if (cellptr->IsVisible || !cellptr->IsMapped) continue;

			cellptr->IsToShroud = true;
		}
	}

	for (y = 0; y < MAP_CELL_H; y++) {
		for (x = 0; x < MAP_CELL_W; x++) {
			cell.X = x;
			cell.Y = y;
			if (!In_Radar(cell)) continue;

			if ((*this)[cell].IsToShroud) {
				(*this)[cell].IsToShroud = false;
				Shroud_Cell(cell);
			}
		}
	}

	All_To_Look();

	Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: GAMEMD 0x004ACBC0
void DisplayClass::Encroach_Fog(void)
{
	Reset_Iterator();
	CellClass *cellptr = Iterate();
	while (cellptr) {
		if (!cellptr->IsFogVisible && cellptr->IsFogMapped) {
			cellptr->IsToFog = true;
		}
		cellptr = Iterate();
	}
	All_To_Look(false, true);
	Reset_Iterator();
	cellptr = Iterate();
	while (cellptr) {
		if (cellptr->IsToFog) {
			cellptr->IsToFog = false;
			Fog_Cell(cellptr->Fetch_CellID());
		}
		cellptr = Iterate();
	}
}

// FUNCTION: GAMEMD 0x004ACC50
void DisplayClass::Fog_Cell(Cell const & cell)
{
	if (!In_Radar(cell)) return;

	CellClass * cellptr = &(*this)[cell];
	bool fog = false;

	if (cellptr->IsFogMapped || cellptr->IsFogVisible) {
		fog = true;
	}

	cellptr->IsFogMapped = false;
	cellptr->IsFogVisible = false;
	cellptr->FogFrame = -2;

	if (cellptr->IsMapped) {
		TacticalMap->Flag_Cell(*cellptr);
	}

	for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
		Cell c = Adjacent_Cell(cell, dir);
		CellClass * cptr = &(*this)[c];
		int fog = TacticalMap->Cell_Shadow(cptr->Fetch_CellID(), true);

		if (fog == -2 && cptr->FogFrame != -2) {
			Fog_Cell(c);
		} else {
			if ((cptr->IsFogVisible || fog != cptr->FogFrame) && fog >= 0 && cptr->FogFrame >= -1) {
				cptr->FogFrame = fog;
				cptr->IsFogMapped = true;
				cptr->IsFogVisible = false;
				TacticalMap->Flag_Cell(*cptr);
			}
		}
	}
	if (fog) {
		cellptr->Fog_Cell();
	}
}

// FUNCTION: GAMEMD 0x004ACDA0
void DisplayClass::Shroud_Cell(Cell const & cell)
{
	if (!In_Radar(cell)) return;

	CellClass * cellptr = &(*this)[cell];
	if (cellptr->IsMapped) {
		cellptr->IsMapped = false;
		cellptr->IsVisible = false;
		TacticalMap->Flag_Cell(*cellptr);

		for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
			Cell c = Adjacent_Cell(cell, dir);
			CellClass * cptr = &(*this)[c];
			if (c != cell) {
				cptr->IsVisible = false;
				TacticalMap->Flag_Cell(*cptr);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004ACE70
void DisplayClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();

	char const * const name = "Map";

	Rect rect = ini.Get_Rect(name, "Size", Rect(1, 1, 50, 50));

	Init_Cells();

	Set_Map_Dimensions(rect, true, 0, true);

	int cell_height = ini.Get_Int(name, "Level", 0);

	char fill[32];
	IsometricTileType ittype = IsometricTileTypeClass::ClearTile;
	int variants = 0;
	ini.Get_String(name, "Fill", "Clear", fill, sizeof(fill));
	if (stricmp(fill, "Water") == 0) {
		variants = 3;
		ittype = IsometricTileTypeClass::WaterSet;
	}

	Reset_Iterator();
	CellClass *cptr = Iterate();
	while (cptr != NULL) {
		cptr->ITType = (IsometricTileType)(ittype + Random_Pick(0, variants));
		cptr->SubTile = 0;
		cptr->Height += cell_height;
		cptr = Iterate();
	}

	Scen->Theater = ini.Get_TheaterType(name, "Theater", THEATER_TEMPERATE);

	Init(Scen->Theater);

	Session.Update_Progress(63);
	Call_Back();

	int uniqueid = Scen->UniqueID;
	TerrainTypeClass::Init(Scen->Theater);
	if (Scen->Theater != LastTheater) {
		IsometricTileTypeClass::Read_Control_File(Scen->Theater, true);
	} else {
		IsometricTileTypeClass::Clear_Use_Counts();
	}
	Scen->UniqueID = uniqueid + 10000;
	OverlayTypeClass::Init(Scen->Theater);
	BuildingTypeClass::Init(Scen->Theater);
	AnimTypeClass::Init(Scen->Theater);
	SmudgeTypeClass::Init(Scen->Theater);

	Session.Update_Progress(65);
	Call_Back();

	Scen->Read_Waypoints(ini);

	if (!Scen->Is_Valid_Waypoint(Scen->Home)) {
		Scen->Set_Waypoint(Scen->Home, Cell((PlayRect.Width + PlayRect.Height) / 2, (PlayRect.Width + PlayRect.Height) / 2));
	}

	Cell homecell = Scen->Get_Waypoint_Cell(Scen->GlobalFlags[0].Value ? Scen->AltHome : Scen->Home);
	Scen->Views[0] = Scen->Views[1] = Scen->Views[2] = Scen->Views[3] = homecell;
	Coord homecoord = homecell;
	TacticalMap->Set_Tactical_Position(homecoord);

	char const * const celltags = "CellTags";
	int len = ini.Entry_Count(celltags);
	for (int index = 0; index < len; index++) {
		char const * cellentry = ini.Get_Entry(celltags, index);
		TagTypeClass * tp;
		Cell cell;

		char buffer[128];
		if (ini.Get_String(celltags, cellentry, "", buffer, sizeof(buffer))) {
			tp = TagTypeClass::Find_Or_Make(buffer);
		} else {
			tp = NULL;
		}

		int val = atoi(cellentry);

		if (NewINIFormat >= 4) {
			cell = Cell(val % 1000, val / 1000);
		} else {
			cell = Cell(val % 128, val / 128);
		}

		if (tp != NULL && !(*this)[cell].Tag) {
			TagClass * tt = Find_Or_Make(tp);
			if (tt) {
				tt->Set_Position(cell);
				(*this)[cell].Attach_Tag(tt);
				TaggedCells.Add(cell);
			}
		}
	}
	Session.Update_Progress(67);
	Call_Back();

	BSurface staging_buffer(640, 400, 2);

	int size = (staging_buffer.Width * staging_buffer.BBP) * staging_buffer.Height;

	static char const * const ISOMAPPACK1 = "IsoMapPack";
	len = ini.Get_UUBlock(ISOMAPPACK1, staging_buffer.Lock(), size);
	if (len > 0) {
		BufferStraw bstraw(staging_buffer.Lock(), len);
		Map.Read_Binary_1(bstraw);
		staging_buffer.Unlock();
	}
	staging_buffer.Unlock();

	static char const * const ISOMAPPACK2 = "IsoMapPack2";
	len = ini.Get_UUBlock(ISOMAPPACK2, staging_buffer.Lock(), size);
	if (len > 0) {
		BufferStraw bstraw(staging_buffer.Lock(), len);
		Map.Read_Binary_2(bstraw);
		staging_buffer.Unlock();
	}
	staging_buffer.Unlock();

	static char const * const ISOMAPPACK3 = "IsoMapPack3";
	len = ini.Get_UUBlock(ISOMAPPACK3, staging_buffer.Lock(), size);
	if (len > 0) {
		BufferStraw bstraw(staging_buffer.Lock(), len);
		Map.Read_Binary_3(bstraw);
		staging_buffer.Unlock();
	}
	staging_buffer.Unlock();

	static char const * const ISOMAPPACK4 = "IsoMapPack4";
	len = ini.Get_UUBlock(ISOMAPPACK4, staging_buffer.Lock(), size);
	if (len > 0) {
		BufferStraw bstraw(staging_buffer.Lock(), len);
		Map.Read_Binary_4(bstraw);
		staging_buffer.Unlock();
	}
	staging_buffer.Unlock();

	static char const * const ISOMAPPACK5 = "IsoMapPack5";
	len = ini.Get_UUBlock(ISOMAPPACK5, staging_buffer.Lock(), size);
	if (len > 0) {
		BufferStraw bstraw(staging_buffer.Lock(), len);
		Map.Read_Binary_5(bstraw);
		staging_buffer.Unlock();
	}
	staging_buffer.Unlock();

	Session.Update_Progress(68);
	Call_Back();

	IsometricTileTypeClass::Load_Tiles(Unsorted::ArmageddonMode || DAT_00A8ED5D, Scen->IsRandom);

	Session.Update_Progress(69);
	Call_Back();

	LocalRect = ini.Get_Rect(name, "LocalSize", PlayRect);

	Map.Set_Local_Dimensions(LocalRect);
	Map.FUN_00588990();

	LastTheater = Scen->Theater;
}

// FUNCTION: GAMEMD 0x004AD7E0
void DisplayClass::Write_INI(CCINIClass & ini)
{
	char entry[20];

	static char const * const NAME = "Map";
	ini.Clear(NAME);
	ini.Put_TheaterType(NAME, "Theater", Scen->Theater);
	ini.Put_Rect(NAME, "Size", PlayRect);
	ini.Put_Rect(NAME, "LocalSize", LocalRect);

	Scen->Write_Waypoints(ini);

	static char const * const CELLTRIG = "CellTags";
	ini.Clear(CELLTRIG);
	for (int y = 0; y < MAP_CELL_H; y++) {
		for (int x = 0; x < MAP_CELL_W; x++) {
			if ((*this)[Cell(x,y)].Tag != NULL) {
				TagClass * tp = (*this)[Cell(x,y)].Tag;
				if (tp != NULL && tp->Class != NULL) {
					wsprintf(entry, "%d", x + (y * 1000));
					ini.Put_String(CELLTRIG, entry, tp->Class->Name());
				}
			}
		}
	}

	static char const * const MAPPACK1 = "IsoMapPack";
	static char const * const MAPPACK2 = "IsoMapPack2";
	static char const * const MAPPACK3 = "IsoMapPack3";
	static char const * const MAPPACK4 = "IsoMapPack4";
	static char const * const MAPPACK5 = "IsoMapPack5";
	BufferPipe bpipe(AlternateSurface->Lock(), AlternateSurface->Get_Width() * AlternateSurface->Get_Height() * AlternateSurface->Bytes_Per_Pixel());
	int len = Map.Write_Binary_5(bpipe);
	ini.Clear(MAPPACK1);
	ini.Clear(MAPPACK2);
	ini.Clear(MAPPACK3);
	ini.Clear(MAPPACK4);
	ini.Clear(MAPPACK5);
	if (len > 0) {
		ini.Put_UUBlock(MAPPACK5, AlternateSurface->Lock(), len);
		AlternateSurface->Unlock();
	}
	AlternateSurface->Unlock();
}

// FUNCTION: GAMEMD 0x004ADA30
int DisplayClass::Stash_Map_State(void * stash, int)
{
	unsigned char *data = (unsigned char *)stash;

	(*(unsigned int *)data) = Scen->Theater;
	data += sizeof(Scen->Theater);

	(*(Rect *)data) = PlayRect;
	data += sizeof(PlayRect);

	(*(Rect *)data) = LocalRect;
	data += sizeof(LocalRect);

	int *wcount = ((int *)data);
	data += sizeof(*wcount);

	int count = 0;

	for (int i = 0; i < WAYPT_COUNT; i++) {
		if (Scen->Is_Valid_Waypoint((WAYPOINT)i)) {
			(*(int *)data) = i;
			data += sizeof(i);

			int val = Scen->Get_Waypoint_Cell((WAYPOINT)i).X + Scen->Get_Waypoint_Cell((WAYPOINT)i).Y * 1000;
			(*(int *)data) = val;
			data += sizeof(val);

			count++;
		}
	}

	int *nptr = (int *)data;

	*wcount = count;

	data += sizeof(*nptr);

	int cnum = 0;
	Reset_Iterator();
	CellClass * cptr = Iterate();
	while (cptr != NULL) {
		(*(Cell *)data) = cptr->CellID;
		data += sizeof(cptr->CellID);

		(*(IsometricTileType *)data) = cptr->ITType;
		data += sizeof(cptr->ITType);

		(*(unsigned char *)data) = cptr->SubTile;
		data += sizeof(cptr->SubTile);

		(*(unsigned char *)data) = cptr->Height;
		data += sizeof(cptr->Height);

		(*(unsigned char *)data) = cptr->Ramp;
		data += sizeof(cptr->Ramp);

		(*(OverlayType *)data) = cptr->Overlay;
		data += sizeof(cptr->Overlay);

		(*(unsigned char *)data) = cptr->OverlayData;
		data += sizeof(cptr->OverlayData);

		(*(unsigned char *)data) = cptr->IsIceGrowthAllowed;
		data += sizeof(cptr->IsIceGrowthAllowed);

		unsigned int tag = 0;
		if (cptr->Tag != NULL) {
			if (cptr->Tag->Class != NULL) {
				tag = (unsigned int)cptr->Tag->Class;
			}
		}

		(*(unsigned int *)data) = tag;
		data += sizeof(tag);

		cnum++;
		cptr = Iterate();
	}
	*nptr = cnum;

	return(data - (unsigned char *)stash);
}

// FUNCTION: GAMEMD 0x004ADBA0
void DisplayClass::Restore_Map_State(void * stash)
{
	unsigned char *data = (unsigned char *)stash;

	Scen->Theater = (TheaterType)(*(unsigned int *)data);
	data += sizeof(Scen->Theater);

	PlayRect = (*(Rect *)data);
	data += sizeof(PlayRect);

	LocalRect = (*(Rect *)data);
	data += sizeof(LocalRect);

	int wcount = (*(int *)data);
	data += sizeof(int);

	while (wcount > 0) {
		int wp = (*(int *)data);
		data += sizeof(wp);

		int val = (*(int *)data);
		data += sizeof(val);

		Cell cell(val % 1000, val / 1000);
		Scen->Set_Waypoint(wp, cell);
		wcount--;
	}

	int num = (*(int *)data);
	data += sizeof(num);

	Reset_Iterator();
	CellClass * cptr = Iterate();
	while (cptr != NULL) {
		data += sizeof(cptr->CellID);

		cptr->ITType = (*(IsometricTileType *)data);
		data += sizeof(cptr->ITType);

		cptr->SubTile = (*(unsigned char *)data);
		data += sizeof(cptr->SubTile);

		cptr->Height = (*(unsigned char *)data);
		data += sizeof(cptr->Height);

		cptr->Ramp = (*(unsigned char *)data);
		data += sizeof(cptr->Ramp);

		cptr->Overlay = (*(OverlayType *)data);
		data += sizeof(cptr->Overlay);

		cptr->OverlayData = (*(unsigned char *)data);
		data += sizeof(cptr->OverlayData);

		cptr->IsIceGrowthAllowed = (*(unsigned char *)data);
		data += sizeof(cptr->IsIceGrowthAllowed);

		int tag = (*(unsigned int *)data);
		data += sizeof(tag);

		cptr = Iterate();
	}
}

extern DynamicVectorClass<TechnoClass *> DAT_00A8EC78;

// FUNCTION: GAMEMD 0x004ADCD0
void DisplayClass::FUN_004ADCD0(bool units_only, bool is_fog)
{
	int count = DAT_00A8EC78.Count();
	for (int index = 0; index < count; index++) {
		TechnoClass * tech = DAT_00A8EC78[index];
		if (tech != NULL && !tech->IsInLimbo) {
			if (tech->Fetch_RTTI() == RTTI_BUILDING && units_only) continue;

			if (tech->House->Is_Player_Control()) {
				if (tech->IsDiscoveredByPlayer) {
					tech->UpdateSight(false, false, false, NULL, 0);
					Coord c = tech->Position;
					Map.RevealArea3(c, 0, tech->LastSightRange + 3, 0);
					if (tech->Techno_Type_Class()->GapGenerator) {
						tech->CreateGap();
					}
				}
			} else if (tech->Fetch_RTTI() == RTTI_BUILDING && Rule->IsAllyReveal && tech->House->Is_Ally(PlayerPtr)) {
				tech->UpdateSight(false, is_fog, false, NULL, 0);
				Coord c = tech->Position;
					Map.RevealArea3(c, 0, tech->LastSightRange + 3, 0);
			} else if (tech->Fetch_RTTI() == RTTI_BUILDING && tech->Techno_Type_Class()->RevealToAll && !tech->House->Is_Player_Control()) {
				tech->UpdateSight(false, false, true, PlayerPtr, 0);
				Coord c = tech->Position;
					Map.RevealArea3(c, 0, tech->LastSightRange + 3, 0);
			} else if (tech->Techno_Type_Class()->GapGenerator) {
				tech->CreateGap();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004ADEE0
void DisplayClass::FUN_004ADEE0(bool units_only, bool is_fog)
{
	int count = DAT_00A8EC78.Count();
	for (int index = 0; index < count; index++) {
		TechnoClass * tech = DAT_00A8EC78[index];
		if (tech != NULL && !tech->IsInLimbo) {
			if (tech->Fetch_RTTI() == RTTI_BUILDING && units_only) continue;

			if (tech->House->Is_Player_Control()) {
				if (tech->IsDiscoveredByPlayer) {
					tech->vt_entry_48C(false, false, false, NULL);
					if (tech->Techno_Type_Class()->GapGenerator) {
						tech->DestroyGap();
					}
				}
			} else if (tech->Fetch_RTTI() == RTTI_BUILDING && Rule->IsAllyReveal && tech->House->Is_Ally(PlayerPtr)) {
				tech->vt_entry_48C(false, is_fog, false, NULL);
			} else if (tech->Techno_Type_Class()->GapGenerator) {
				tech->DestroyGap();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004ADFF0
void DisplayClass::FUN_004ADFF0(bool units_only, bool is_fog)
{
	for (int index = 0; index < Layer[LAYER_GROUND].Count(); index++) {
		ObjectClass * object = Layer[LAYER_GROUND][index];
		if (object != NULL && (object->AbstractFlags & 1)) {
			TechnoClass * tech = (TechnoClass *)object;
			if (tech->Fetch_RTTI() == RTTI_BUILDING && units_only) continue;

			if (tech->House->Is_Player_Control()) {
				if (tech->IsDiscoveredByPlayer) {
					tech->Look(false, is_fog);
				}
			} else if (tech->Fetch_RTTI() == RTTI_BUILDING && Rule->IsAllyReveal && tech->House->Is_Ally(PlayerPtr)) {
				tech->Look(is_fog, false);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004AE0B0
void DisplayClass::Constrained_Look(Coord const & center, LEPTON distance)
{
	for (int index = 0; index < Layer[LAYER_GROUND].Count(); index++) {
		TechnoClass * tech = Dynamic_Cast<TechnoClass *>(Layer[LAYER_GROUND][index]);
		if (tech != NULL) {
			if (tech->House->Is_Player_Control()) {
				if (tech->IsDiscoveredByPlayer && Distance(center, tech->Center_Coord()) <= (tech->Techno_Type_Class()->SightRange * CELL_LEPTON_W) + distance) {
					tech->Look();
				}
			} else {
				if (tech->Fetch_RTTI() == RTTI_BUILDING && Rule->IsAllyReveal && tech->House->Is_Ally(PlayerPtr) &&
					Distance(tech->Center_Coord(), center) <= (tech->Techno_Type_Class()->SightRange * CELL_LEPTON_W) + distance) {
					tech->Look();
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004AE290
void DisplayClass::Center_Map(void)
{
	int index;

	if (CurrentObject.Count() > 0) {

		Coord center(0,0,0);
		for (index = 0; index < CurrentObject.Count(); index++) {
			Coord pos = CurrentObject[index]->Position;
			center += pos;
		}

		Coord coord = center;

		center /= CurrentObject.Count();

		if (CurrentObject.Count() > 2) {
			Coord bestc(0,0,0);
			int bestd = 0;
			for (index = 0; index < CurrentObject.Count(); index++) {
				Coord pos = CurrentObject[index]->Position;
				int d = center.Distance_To(pos);
				if (d > bestd) {
					bestc = CurrentObject[index]->Position;
					bestd = d;
				}
			}

			coord -= bestc;
			center = coord / (CurrentObject.Count() - 1);
		}

		TacticalMap->Set_Tactical_Position(center);
	}
}

// FUNCTION: GAMEMD 0x004AE450
void DisplayClass::Reinit_Cell_Drawers(void)
{
	IsometricTileTypeClass::Init_Drawers();
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	BlubCell.Drawer = NULL;
	while (cptr != NULL) {
		cptr->Init_Drawer();
		cptr = Map.Iterate();
	}
	Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: GAMEMD 0x004AE4C0
void DisplayClass::Update_Cell_Colors(void)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Recalc_Light();
		cptr = Map.Iterate();
	}
}

extern bool DAT_00A8F7D8;
extern wchar_t DAT_008A0630[];

static inline TechnoClass * Help_Cast(ObjectClass * object)
{
	if (object->AbstractFlags & 1) return((TechnoClass *)object);
	return(NULL);
}

// FUNCTION: GAMEMD 0x004AE4F0
char const * DisplayClass::Help_Text(int id)
{
	if (id < 500 || id > 900 || Is_Scrolling()) {
		return(NULL);
	}

	Cell cell;
	Coord coord;
	ObjectClass * object;
	unsigned char fog, shadow;

	int bx = DSurface::ViewBounds.X;
	int by = DSurface::ViewBounds.Y;
	Point2D const & mp = MouseCursor->Get_Mouse_Point();
	Point2D point(mp.X - bx, mp.Y - by);
	Map.ProcessClickCoords(&point, &cell, &coord, &object, &fog, &shadow);

	if (DAT_00A8F7D8) {
		swprintf(DAT_008A0630, L"(%d,%d)", coord.X / 256, coord.Y / 256);
		return((char const *)DAT_008A0630);
	}

	if (!Map[coord].IsMapped && MainWindow) {
		return((char const *)StringTable::LoadString("TXT_SHADOW", NULL, "D:\\ra2mdpost\\Display.CPP", 0x13B8));
	}

	if (object == NULL) {
		return(NULL);
	}
	TechnoClass * techno = Help_Cast(object);
	if (techno != NULL && !techno->IsOwnedByPlayer) {
		if (techno->Cloak == CLOAKED && !Map[techno->Center_Coord()].Is_Sensed(PlayerPtr->HeapID)) {
			return(NULL);
		}
		if (techno->Techno_Type_Class()->IsInvisible) {
			return(NULL);
		}
	}

	if (object != NULL) {
		wchar_t const * text = object->Full_Name();
		if (techno != NULL) return((char const *)text);
		return((char const *)(*this)[cell].Tiberium_Name());
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x004AE6B0
void DisplayClass::Reposition_Sidebar(void)
{
	if (UnusedWindow) {
		CloseWindow(UnusedWindow);
		UnusedWindow = 0;
	}
}

// FUNCTION: GAMEMD 0x004AE6D0
LRESULT DisplayClass::Windows_Message_Proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return(DefWindowProc(hWnd, Msg, wParam, lParam));
}

// FUNCTION: GAMEMD 0x004AE6F0
HRESULT DisplayClass::Load(IStream * stream)
{
	HRESULT result = S_OK;
	for (LayerType layer = LAYER_FIRST; layer < LAYER_COUNT; layer++) {
		result = Layer[layer].Load(stream);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004AE720
HRESULT DisplayClass::Save(IStream * stream)
{
	HRESULT result = S_OK;
	for (LayerType layer = LAYER_FIRST; layer < LAYER_COUNT; layer++) {
		result = Layer[layer].Save(stream);
		if (FAILED(result)) break;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004AE9B0
ActionType DisplayClass::Action_To_Waypoint_Action(ActionType action, Cell const & cell)
{
	if (PlayerPtr->Waypoint_At(cell) != NULL) {
		switch (action) {
			case ACTION_MOVE:
			case ACTION_NOMOVE:
				return(ACTION_FOLLOW_WAYPOINT);
			case ACTION_ATTACK:
			case ACTION_HARVEST:
				return(ACTION_ATTACK_WAYPOINT);
			case ACTION_ENTER:
			case ACTION_CAPTURE:
			case ACTION_EATEN:
				return(ACTION_ENTER_WAYPOINT);
			default:
				return(action);
		}
	}
	return(action);
}

// FUNCTION: GAMEMD 0x004AEA20
void DisplayClass::Update_Waypoint_Color(int index)
{
	int offset;
	if (index == -1) {
		offset = 0;
	} else {
		offset = 8 * (index % 12);
	}
	unsigned short * table = (unsigned short *)MouseDrawer->Get_Translate_Table();
	for (int i = 0; i < 8; i++) {
		const RGBClass rgb = WaypointPalette[i + offset];
		table[i + 1] = DSurface::Build_Hicolor_Pixel(rgb.Get_Red(), rgb.Get_Green(), rgb.Get_Blue());
	}
}

extern bool DAT_00A8ED9D;

// FUNCTION: GAMEMD 0x004AEAD0
void DisplayClass::Abort_Drag_Select(void)
{
	if (TacticalMap != NULL) {
		TacticalMap->End_Rubber_Band();
	}

	IsRubberBand = false;
	IsTentative = false;
	DAT_00A8ED9D = true;
	Set_Default_Mouse(MOUSE_NORMAL, false);
}

// FUNCTION: GAMEMD 0x004AEB10
ObjectClass * DisplayClass::Object_To_Follow(void) const
{
	if (FollowingObject && FollowingObjectPtr != NULL) {
		return(FollowingObjectPtr);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x004AEB30
void DisplayClass::Set_To_Follow(ObjectClass * object)
{
	FollowingObjectPtr = object;
	FollowingObject = object != NULL ? true : false;
}

// SYNTHETIC: GAMEMD 0x004AEB50
// ??_GLayerClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x004AEBA0
// GadgetClass::Get_ID

// SYNTHETIC: GAMEMD 0x004AEBB0
// ??_GTacticalClass@DisplayClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x004AEBD0
// GScreenClass::Draw_It

// FUNCTION: GAMEMD 0x004AEBE0
// MapClass::Is_Scrolling

// SYNTHETIC: GAMEMD 0x004AEBF0
// ??_GDisplayClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x004AEC10
// ??_GStraw@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x004AEC30
// ??_GBufferStraw@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x004AEC60
// Surface::Surface

// SYNTHETIC: GAMEMD 0x004AEC80
// ??_GPipe@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x004AECA0
// ??_GBufferPipe@@UAEPAXI@Z
