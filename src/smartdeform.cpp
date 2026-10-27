// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "smartdeform.h"
#include "_isotile.h"
#include "_map.h"
#include "_rect.h"
#include "_tactica.h"
#include "cell.h"
#include "inline.h"
#include "isotype.h"
#include "mapgen.h"
#include "object.h"
#include "smudtype.h"
#include "tactical.h"
#include "ramp.hh"
#include <math.h>


#define MAX_TERRAIN_HEIGHT (12)
#define NUM_CORNERS 4
#define RAMP_FULL_HEIGHT (30)
#define RAMP_HALF_HEIGHT (RAMP_FULL_HEIGHT / 2)

// SYNTHETIC: GAMEMD 0x006B21A0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006B21D0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006B21F0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006B2210 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006B2230 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006B2250 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006B2270 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006B22B0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006B22E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006B2310 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006B2340 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006B2350 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006B2370 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006B2390 SYMBOL
// _$E41

extern int * _ramp_corners[RAMP_COUNT];

struct DeformPointStruct {
	DeformPointStruct(void)
	{
		Height = 0;
		Rigid = false;
		Done = false;
	};

	DeformPointStruct(const DeformPointStruct & that)
	{
		Height = that.Height;
		Rigid = that.Rigid;
		Done = that.Done;
	};

	int Height;
	bool Rigid;
	bool Done;
	bool CanForce;
};

extern "C" {
DeformPointStruct * DeformPoints = NULL;

extern int DeformPointXAdd;
extern int DeformPointYAdd;

extern int DeformPointWidth;
extern int DeformPointHeight;

extern bool Asm_Ripple_Deform_Points(int startpointx, int startpointy, int general_direction, bool forced);
}

struct UndoInfoStruct {
	UndoInfoStruct(void)
	{
		CellPoint = Point2D(0, 0);
		OldHeight = 0;
	};

	UndoInfoStruct(const Point2D & point, int oldheight)
	{
		CellPoint = point;
		OldHeight = oldheight;
	};

	bool operator==(UndoInfoStruct const & rvalue) const { return(CellPoint == rvalue.CellPoint && OldHeight == rvalue.OldHeight); }
	bool operator!=(UndoInfoStruct const & rvalue) const { return(CellPoint != rvalue.CellPoint || OldHeight != rvalue.OldHeight); }

	Point2D CellPoint;
	int OldHeight;
};

// SYNTHETIC: GAMEMD 0x006B23B0 SYMBOL
// _$E197
// SYNTHETIC: GAMEMD 0x006B23F0 SYMBOL
// _$E195
DynamicVectorClass<UndoInfoStruct> UndoRecords;

bool Commit_Deform_Grid(Cell center, bool forced);

// FUNCTION: GAMEMD 0x006B2430
void Record_Deform_Undo(const Point2D & point, int oldheight)
{
	UndoRecords.Add(UndoInfoStruct(point, oldheight));
}

// FUNCTION: GAMEMD 0x006B24A0
void Undo_Deform(void)
{
	for (int i = UndoRecords.Count() - 1; i >= 0; i--) {
		DeformPointStruct & new_point = DeformPoints[UndoRecords[i].CellPoint.X + UndoRecords[i].CellPoint.Y * DeformPointWidth];
		new_point.Height = UndoRecords[i].OldHeight;
	}
	UndoRecords.Clear();
}

// FUNCTION: GAMEMD 0x006B2520
bool Can_Deform_Cell_RMG(CellClass * cellptr, bool)
{
	if (!My_In_Radar(cellptr->Fetch_CellID())) {
		return(false);
	}

	if (cellptr->Overlay != OVERLAY_NONE || cellptr->Cell_Occupier()) {
		return(false);
	}

	if (MapRegionClass::Get_Cell_Data(cellptr->Fetch_CellID()).Inviolate) {
		return(false);
	}

	IsometricTileType tile_type = cellptr->ITType;
	if (tile_type >= IsometricTileTypes.Count()) {
		tile_type = TILE_NONE;
	}

	if (tile_type != TILE_NONE) {
		IsometricTileTypeClass const * ittype = IsometricTileTypes[tile_type];
		return(ittype->IsMorphable);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x006B25C0
bool Can_Deform_Cell(CellClass * cellptr, bool)
{
	ObjectClass * obj = cellptr->Cell_Occupier();
	while (obj) {
		if (obj->IsActive && obj->RTTI != RTTI_INFANTRY && obj->RTTI != RTTI_UNIT) {
			return(false);
		}
		obj = obj->Next;
	}

	if ((cellptr->Cell_Bridge_Occupier() || cellptr->IsUnderBridge || cellptr->WasUnderBridge)) {
		return(false);
	}

	CellClass * adj_cellptr = &Map[Adjacent_Cell(cellptr->CellID, FACING_N)];
	if ((adj_cellptr->WasUnderBridge || adj_cellptr->IsUnderBridge) && adj_cellptr->IsBridgeEastWest) {
		return(false);
	}

	adj_cellptr = &Map[Adjacent_Cell(cellptr->CellID, FACING_W)];
	if ((adj_cellptr->WasUnderBridge || adj_cellptr->IsUnderBridge) && !adj_cellptr->IsBridgeEastWest) {
		return(false);
	}

	if (cellptr->Overlay != OVERLAY_NONE) {
		return(false);
	}

	if (!Map.In_Radar(cellptr->CellID)) return(false);

	IsometricTileType tile_type = cellptr->ITType;
	if (tile_type >= IsometricTileTypes.Count()) {
		tile_type = TILE_NONE;
	}

	if (tile_type != TILE_NONE) {
		IsometricTileTypeClass const * ittype = IsometricTileTypes[tile_type];
		return(ittype->IsMorphable);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x006B2700
void Init_Deform_Grid(Cell center, bool forced)
{
	if (DeformPoints) {
		delete[] DeformPoints;
		DeformPoints = NULL;
	}

	DeformPoints = new DeformPointStruct[(Map.MapRect.Width + 1) * (Map.MapRect.Height + 1)];
	DeformPointWidth = Map.MapRect.Width + 1;
	DeformPointHeight = Map.MapRect.Height + 1;

	DeformPointXAdd = Map.MapRect.X;
	DeformPointYAdd = Map.MapRect.Y;

	int point_height;
	int height;
	int ramp;

	Cell top_left = center - Cell(MAX_TERRAIN_HEIGHT + 2, MAX_TERRAIN_HEIGHT + 2);
	Cell bottom_right = center + Cell(MAX_TERRAIN_HEIGHT + 2, MAX_TERRAIN_HEIGHT + 2);

	top_left.X = MAX(0, top_left.X);
	top_left.Y = MAX(0, top_left.Y);

	bottom_right.X = MIN(Map.MapRect.Width - Map.MapRect.X, bottom_right.X);
	bottom_right.Y = MIN(Map.MapRect.Height - Map.MapRect.Y, bottom_right.Y);

	for (int y = top_left.Y; y <= bottom_right.Y; y++) {
		for (int x = top_left.X; x <= bottom_right.X; x++) {

			DeformPointStruct & deform_point = DeformPoints[x + y * DeformPointWidth];
			deform_point.Rigid = false;
			deform_point.Done = false;
			deform_point.CanForce = false;

			point_height = -1;
			Cell deform_cell(x + DeformPointXAdd, y + DeformPointYAdd);

			if (deform_cell.X >= Map.MapRect.X && deform_cell.X <= Map.MapRect.X + Map.MapRect.Width) {
				if (deform_cell.Y >= Map.MapRect.Y && deform_cell.Y <= Map.MapRect.Y + Map.MapRect.Height) {
					ramp = Map[deform_cell].Ramp;
					height = Map[deform_cell].Height * RAMP_HALF_HEIGHT;

					int * ramptype = _ramp_corners[ramp];
					height += ramptype[0];

					point_height = height;

					if (!Can_Deform_Cell(&Map[deform_cell], forced)) {
						deform_point.Rigid = true;
					} else {
						if (!Can_Deform_Cell(&Map[deform_cell + Cell(0, -1)], forced)) {
							deform_point.Rigid = true;
						} else {
							if (!Can_Deform_Cell(&Map[deform_cell + Cell(-1, -1)], forced)) {
								deform_point.Rigid = true;
							} else {
								if (!Can_Deform_Cell(&Map[deform_cell + Cell(-1, 0)], forced)) {
									deform_point.Rigid = true;
								}
							}
						}
					}

					if (deform_cell == center ||
						Cell(deform_cell.X, deform_cell.Y - 1) == center ||
						Cell(deform_cell.X - 1, deform_cell.Y - 1) == center ||
						Cell(deform_cell.X - 1, deform_cell.Y) == center) {
						deform_point.CanForce = true;
					}
				}
			}

			deform_point.Height = point_height;
		}
	}
}

// FUNCTION: GAMEMD 0x006B2A70
void Init_Deform_Grid_RMG(void)
{
	if (DeformPoints != NULL) {
		delete [] DeformPoints;
		DeformPoints = NULL;
	}

	DeformPoints = new DeformPointStruct[(Map.MapRect.Width + 1) * (Map.MapRect.Height + 1)];
	DeformPointWidth = Map.MapRect.Width + 1;
	DeformPointHeight = Map.MapRect.Height + 1;

	DeformPointXAdd = Map.MapRect.X;
	DeformPointYAdd = Map.MapRect.Y;

	int point_height;
	int height;
	int ramp;

	Cell radial_cell_array[3] = {
		Cell(0, -1),
		Cell(-1, -1),
		Cell(-1, 0)
	};

	for (int y = 0; y < DeformPointHeight; y++) {
		for (int x = 0; x < DeformPointWidth; x++) {

			DeformPointStruct & deform_point = DeformPoints[x + y * DeformPointWidth];
			deform_point.Rigid = false;
			deform_point.Done = false;

			point_height = 0;
			Cell cell(x + DeformPointXAdd, y + DeformPointYAdd);
			Cell loop_cell = cell;
			int i = 0;

			while (!My_In_Radar(loop_cell) && i < 3) {
				loop_cell = cell + radial_cell_array[i];
				i++;
			}

			if (My_In_Radar(loop_cell)) {
				ramp = Map[loop_cell].Ramp;
				height = Map[loop_cell].Height * RAMP_HALF_HEIGHT;

				int * ramptype = _ramp_corners[ramp];
				height += ramptype[0];

				point_height = height;

				if (My_In_Radar(cell + Cell(0, -1)) && !Can_Deform_Cell_RMG(&Map[cell + Cell(0, -1)], true)) deform_point.Rigid = true;
				if (My_In_Radar(cell + Cell(-1, -1)) && !Can_Deform_Cell_RMG(&Map[cell + Cell(-1, -1)], true)) deform_point.Rigid = true;
				if (My_In_Radar(cell + Cell(-1, 0)) && !Can_Deform_Cell_RMG(&Map[cell + Cell(-1, 0)], true)) deform_point.Rigid = true;
				if (My_In_Radar(cell) && !Can_Deform_Cell_RMG(&Map[cell], true)) deform_point.Rigid = true;
			} else {
				deform_point.Rigid = true;
			}
			deform_point.Height = point_height;
		}
	}
}

// FUNCTION: GAMEMD 0x006B2F30
bool Commit_Deform_Grid(Cell center, bool forced)
{
	Cell top_left = center - Cell(MAX_TERRAIN_HEIGHT + 2, MAX_TERRAIN_HEIGHT + 2);
	Cell bottom_right = center + Cell(MAX_TERRAIN_HEIGHT + 2, MAX_TERRAIN_HEIGHT + 2);

	top_left.X = MAX(0, top_left.X);
	top_left.Y = MAX(0, top_left.Y);

	bottom_right.X = MIN(Map.MapRect.Width - Map.MapRect.X, bottom_right.X);
	bottom_right.Y = MIN(Map.MapRect.Height - Map.MapRect.Y, bottom_right.Y);

	Rect bound_rect(0, 0, 0, 0);

	Cell least_cell(32767, 32767);
	Cell most_cell(0, 0);

	DynamicVectorClass<ObjectClass *> objects;

	for (int y = top_left.Y; y <= bottom_right.Y; y++) {
		for (int x = top_left.X; x <= bottom_right.X; x++) {

			CellClass * cellptr = &Map[Cell(x + DeformPointXAdd, y + DeformPointYAdd)];

			if (!Can_Deform_Cell(cellptr, forced)) continue;

			DeformPointStruct & deform_point1 = DeformPoints[x + y * DeformPointWidth];
			DeformPointStruct & deform_point2 = DeformPoints[x + 1 + y * DeformPointWidth];
			DeformPointStruct & deform_point3 = DeformPoints[x + 1 + (y + 1) * DeformPointWidth];
			DeformPointStruct & deform_point4 = DeformPoints[x + (y + 1) * DeformPointWidth];

			if (deform_point1.Done || deform_point2.Done || deform_point3.Done || deform_point4.Done) {

				int desired_corners[NUM_CORNERS];
				desired_corners[0] = deform_point1.Height;
				desired_corners[1] = deform_point2.Height;
				desired_corners[2] = deform_point3.Height;
				desired_corners[3] = deform_point4.Height;

				int min_height = 1000;
				int max_height = 0;

				for (int i = 0; i < NUM_CORNERS; i++) {
					if (desired_corners[i] < min_height) min_height = desired_corners[i];
					if (desired_corners[i] > max_height) max_height = desired_corners[i];
				}

				if (max_height - min_height > RAMP_HALF_HEIGHT) continue;

				objects.Clear();
				ObjectClass * obj = cellptr->Cell_Occupier();
				while (obj) {
					objects.Add(obj);
					obj = obj->Next;
				}
				int objindex;
				for (objindex = 0; objindex < objects.Count(); objindex++) {
					objects[objindex]->Mark(MARK_UP);
				}

				Rect cell_bound;

				cell_bound = cellptr->Cell_Render_Rect();
				int oldheight = cellptr->Height;
				cellptr->Height = (char)(min_height / RAMP_HALF_HEIGHT);
				Map.FUN_00588960(cellptr->Height);

				if (oldheight != cellptr->Height) {
					cellptr->Remove_Fogged_Objects();
				}
				cellptr->Recalc_Attributes();

				if (cellptr->Smudge != SMUDGE_NONE) {
					SmudgeTypeClass * smudge_type = SmudgeTypes[cellptr->Smudge];
					Cell smudge_cell = cellptr->CellID;
					smudge_cell -= Cell(cellptr->SmudgeData % smudge_type->Width, cellptr->SmudgeData / smudge_type->Width);
					for (int iindex = 0; iindex < smudge_type->Height; iindex++) {
						for (int jindex = 0; jindex < smudge_type->Width; jindex++) {
							Map[smudge_cell + Cell(jindex, iindex)].Smudge = SMUDGE_NONE;
						}
					}
				}

				Cell cur_cell(cellptr->Fetch_CellID());
				Map.Flag_Background_Update(cur_cell);

				if (cur_cell.X < least_cell.X) least_cell.X = cur_cell.X;
				if (cur_cell.X > most_cell.X) most_cell.X = cur_cell.X;
				if (cur_cell.Y < least_cell.Y) least_cell.Y = cur_cell.Y;
				if (cur_cell.Y > most_cell.Y) most_cell.Y = cur_cell.Y;

				for (int j = 0; j < NUM_CORNERS; j++) {
					desired_corners[j] -= min_height;
				}

				for (int ramp = 0; ramp < RAMP_COUNT - 2; ramp++) {
					int * ramptype = _ramp_corners[ramp];
					if (
						ramptype[0] == desired_corners[0] &&
						ramptype[1] == desired_corners[1] &&
						ramptype[2] == desired_corners[2] &&
						ramptype[3] == desired_corners[3]
					) {
						cellptr->Ramp = (char)ramp;
						if (ramp == RAMP_NONE) {
							cellptr->ITType = TILE_CLEAR;
						} else {
							cellptr->ITType = IsometricTileType(ramp - 1 + IsometricTileTypeClass::RampStart);
						}
						cellptr->Recalc_Attributes();
						Cell id(cellptr->Fetch_CellID());
						Map.Flag_Background_Update(id);

						cell_bound = Union(cell_bound, cellptr->Cell_Render_Rect());
						if (id.X < least_cell.X) least_cell.X = id.X;
						if (id.X > most_cell.X) most_cell.X = id.X;
						if (id.Y < least_cell.Y) least_cell.Y = id.Y;
						if (id.Y > most_cell.Y) most_cell.Y = id.Y;
						break;
					}
				}
				bound_rect = Union(bound_rect, cell_bound);

				for (objindex = 0; objindex < objects.Count(); objindex++) {
					objects[objindex]->Mark(MARK_DOWN);
					objects[objindex]->HeightAGL = 0;
				}
			}
		}
	}

	delete[] DeformPoints;
	DeformPoints = NULL;

	bound_rect.Y -= TacticalRect.Y;

	TacticalMap->Register_Dirty_Area(bound_rect);

	for (int i = least_cell.Y - 1; i <= most_cell.Y + 1; i++) {
		for (int j = least_cell.X - 1; j <= most_cell.X + 1; j++) {
			Cell cell(j, i);
			CellClass * cptr = &Map[cell];
			Rect lat_rect = cptr->Cell_Render_Rect();

			if (cptr->Fixup_LAT()) {
				lat_rect = Union(lat_rect, cptr->Cell_Render_Rect());
				lat_rect = lat_rect - TacticalRect.Top_Left();
				if (lat_rect.Is_Valid()) {
					TacticalMap->Register_Dirty_Area(lat_rect);
				}
			}
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x006B3850
bool Commit_Deform_Grid_RMG(void)
{
	Map.Reset_Iterator();
	CellClass * cellptr = NULL;

	for (;;) {

		cellptr = Map.Iterate();
		if (!cellptr) {
			break;
		}

		int x = cellptr->Fetch_CellID().X - DeformPointXAdd;
		int y = cellptr->Fetch_CellID().Y - DeformPointYAdd;

		DeformPointStruct & deform_point1 = DeformPoints[x + y * DeformPointWidth];
		DeformPointStruct & deform_point2 = DeformPoints[x + 1 + y * DeformPointWidth];
		DeformPointStruct & deform_point3 = DeformPoints[x + 1 + (y + 1) * DeformPointWidth];
		DeformPointStruct & deform_point4 = DeformPoints[x + (y + 1) * DeformPointWidth];

		if (deform_point1.Done || deform_point2.Done || deform_point3.Done || deform_point4.Done) {

			int desired_corners[NUM_CORNERS];
			desired_corners[0] = deform_point1.Height;
			desired_corners[1] = deform_point2.Height;
			desired_corners[2] = deform_point3.Height;
			desired_corners[3] = deform_point4.Height;

			int min_height = 1000;
			int max_height = 0;

			for (int i = 0; i < NUM_CORNERS; i++) {
				if (desired_corners[i] < min_height) min_height = desired_corners[i];
				if (desired_corners[i] > max_height) max_height = desired_corners[i];
			}

			if (max_height - min_height > RAMP_HALF_HEIGHT) continue;

			if (Can_Deform_Cell_RMG(cellptr, true)) {
				cellptr->Height = (char)(min_height / RAMP_HALF_HEIGHT);
				Cell cur_cell(cellptr->Fetch_CellID());
			} else {
				continue;
			}

			for (int j = 0; j < NUM_CORNERS; j++) {
				desired_corners[j] -= min_height;
			}

			bool found_matching_ramp = false;

			for (int ramp = 0; ramp < RAMP_COUNT - 2; ramp++) {
				int * ramptype = _ramp_corners[ramp];
				if (
					ramptype[0] == desired_corners[0] &&
					ramptype[1] == desired_corners[1] &&
					ramptype[2] == desired_corners[2] &&
					ramptype[3] == desired_corners[3]
				) {

					cellptr->Ramp = (char)ramp;
					if (ramp == RAMP_NONE) {
						cellptr->ITType = TILE_CLEAR;
					} else {
						cellptr->ITType = IsometricTileType(ramp - 1 + IsometricTileTypeClass::RampStart);
					}
					found_matching_ramp = true;
					break;
				}
			}
		}
	}

	delete[] DeformPoints;
	DeformPoints = NULL;

	return(true);
}

// FUNCTION: GAMEMD 0x006B3A80
bool Ripple_Deform_Points(Point2D start_point, int general_direction, bool forced)
{
	DeformPointStruct & start_deform_point = DeformPoints[start_point.X + start_point.Y * DeformPointWidth];
	int startheight = start_deform_point.Height;

	for (int y = -1; y < 2; y++) {
		for (int x = -1; x < 2; x++) {

			if (!(x || y)) continue;

			Point2D test_point = Point2D(x, y) + start_point;

			if (test_point.X < 0 || test_point.Y < 0 || test_point.X >= DeformPointWidth || test_point.Y >= DeformPointHeight) {
				if (forced) continue;
				return(false);
			}

			DeformPointStruct & new_point = DeformPoints[test_point.X + test_point.Y * DeformPointWidth];

			if (!new_point.Rigid || (forced && new_point.CanForce)) {

				if (abs(new_point.Height - startheight) > RAMP_HALF_HEIGHT) {
					if (general_direction == 1) {
						if (new_point.Height < startheight) {
							Record_Deform_Undo(test_point, new_point.Height);
							new_point.Height = startheight - RAMP_HALF_HEIGHT;
							new_point.Done = true;
						}
					} else {
						if (new_point.Height > startheight) {
							Record_Deform_Undo(test_point, new_point.Height);
							new_point.Height = startheight + RAMP_HALF_HEIGHT;
							new_point.Done = true;
						}
					}

					if (!new_point.Rigid) {
						if (!Ripple_Deform_Points(test_point, general_direction, forced)) {
							if (!forced) {
								return(false);
							}
						}
					}
				}
			} else {
				if (abs(new_point.Height - startheight) > RAMP_HALF_HEIGHT) {
					return(false);
				}
			}
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x006B3CD0
bool Deform_Cell(Cell original_cell, int general_direction, bool forced, int point_mask)
{
	UndoRecords.Clear();

	Init_Deform_Grid(original_cell, forced);

	int new_point_mask = point_mask & 3;
	new_point_mask |= (point_mask & 4) ? 8 : 0;
	new_point_mask |= (point_mask & 8) ? 4 : 0;

	int x = original_cell.X - DeformPointXAdd;
	int y = original_cell.Y - DeformPointYAdd;
	int point_bits = 1;

	for (int yy = y; yy < y + 2; yy++) {
		for (int xx = x; xx < x + 2; xx++) {

			if (point_bits & new_point_mask) {
				DeformPointStruct & deform_point = DeformPoints[xx + yy * DeformPointWidth];
				if (!deform_point.Rigid || forced) {
					deform_point.Height += general_direction * RAMP_HALF_HEIGHT;
					if (deform_point.Height < 0 || deform_point.Height > MAX_TERRAIN_HEIGHT * RAMP_HALF_HEIGHT) deform_point.Height -= general_direction * RAMP_HALF_HEIGHT;
					deform_point.Done = true;
					if (!Ripple_Deform_Points(Point2D(xx, yy), general_direction, forced)) {
						if (!forced) {
							delete[] DeformPoints;
							DeformPoints = NULL;
							return(false);
						}
					}
				} else {
					if (!forced) {
						delete[] DeformPoints;
						DeformPoints = NULL;
						return(false);
					}
				}
			}

			point_bits = point_bits << 1;
		}
	}

	return(Commit_Deform_Grid(original_cell, forced));
}

// FUNCTION: GAMEMD 0x006B3E60
bool Deform_Cell_RMG(Cell original_cell, int general_direction, bool forced, int point_mask)
{
	UndoRecords.Clear();

	int new_point_mask = point_mask & 3;
	new_point_mask |= (point_mask & 4) ? 8 : 0;
	new_point_mask |= (point_mask & 8) ? 4 : 0;

	int x = original_cell.X - DeformPointXAdd;
	int y = original_cell.Y - DeformPointYAdd;
	int point_bits = 1;

	for (int yy = y; yy < y + 2; yy++) {
		for (int xx = x; xx < x + 2; xx++) {

			if (point_bits & new_point_mask) {
				DeformPointStruct & deform_point = DeformPoints[xx + yy * DeformPointWidth];
				if (!deform_point.Rigid) {
					deform_point.Height += general_direction * RAMP_HALF_HEIGHT;
					if (deform_point.Height < 0 || deform_point.Height > MAX_TERRAIN_HEIGHT * RAMP_HALF_HEIGHT) {
						deform_point.Height -= general_direction * RAMP_HALF_HEIGHT;
					} else {
						Record_Deform_Undo(Point2D(xx, yy), deform_point.Height - general_direction * RAMP_HALF_HEIGHT);
					}
					deform_point.Done = true;
					if (!Ripple_Deform_Points(Point2D(xx, yy), general_direction, forced)) {
						if (!forced) {
							Undo_Deform();
							return(false);
						}
					}
				} else {
					if (!forced) {
						Undo_Deform();
						return(false);
					}
				}
			}
			point_bits = point_bits << 1;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x006B4100
int Get_Deformed_Cell_Height(CellClass * cellptr)
{
	int x = cellptr->Fetch_CellID().X - DeformPointXAdd;
	int y = cellptr->Fetch_CellID().Y - DeformPointYAdd;

	DeformPointStruct & deform_point1 = DeformPoints[x + y * DeformPointWidth];
	DeformPointStruct & deform_point2 = DeformPoints[x + 1 + y * DeformPointWidth];
	DeformPointStruct & deform_point3 = DeformPoints[x + 1 + (y + 1) * DeformPointWidth];
	DeformPointStruct & deform_point4 = DeformPoints[x + (y + 1) * DeformPointWidth];

	int desired_corners[NUM_CORNERS];
	desired_corners[0] = deform_point1.Height;
	desired_corners[1] = deform_point2.Height;
	desired_corners[2] = deform_point3.Height;
	desired_corners[3] = deform_point4.Height;

	int min_height = 1000;

	for (int i = 0; i < NUM_CORNERS; i++) {
		if (desired_corners[i] < min_height) min_height = desired_corners[i];
	}

	if (Can_Deform_Cell_RMG(cellptr, true)) {
		return(min_height / RAMP_HALF_HEIGHT);
	} else {
		return(cellptr->Height);
	}
}

// FUNCTION: GAMEMD 0x006B4240
int Get_Deformable_Cell_Corners(CellClass * cellptr, int direction)
{
	int i;
	int x = cellptr->Fetch_CellID().X - DeformPointXAdd;
	int y = cellptr->Fetch_CellID().Y - DeformPointYAdd;

	DeformPointStruct cells[NUM_CORNERS] = {
		DeformPoints[x + y * DeformPointWidth],
		DeformPoints[x + 1 + y * DeformPointWidth],
		DeformPoints[x + 1 + (y + 1) * DeformPointWidth],
		DeformPoints[x + (y + 1) * DeformPointWidth]
	};

	int desired_corners[NUM_CORNERS];
	desired_corners[0] = cells[0].Height;
	desired_corners[1] = cells[1].Height;
	desired_corners[2] = cells[2].Height;
	desired_corners[3] = cells[3].Height;
	int min_height = 1000;
	int max_height = -1;
	int minindex = -1;
	int maxindex = -1;

	for (i = 0; i < NUM_CORNERS; i++) {
		if (desired_corners[i] < min_height) {
			minindex = i;
			min_height = desired_corners[i];
		}
		if (desired_corners[i] > max_height) {
			maxindex = i;
			max_height = desired_corners[i];
		}
	}

	int mask = 0;
	if (max_height == min_height) {
		for (i = 0; i < NUM_CORNERS; i++) {
			if (!cells[i].Rigid) {
				mask += (1 << i);
			}
		}
	} else if (direction > 0) {
		for (i = 0; i < NUM_CORNERS; i++) {
			if (desired_corners[i] < max_height && !cells[i].Rigid) {
				mask += (1 << i);
			}
		}
	} else {
		for (i = 0; i < NUM_CORNERS; i++) {
			if (desired_corners[i] > min_height && !cells[i].Rigid) {
				mask += (1 << i);
			}
		}
	}

	return(mask);
}

// TEMPLATE: GAMEMD 0x006B43B0
// ?Clear@?$DynamicVectorClass@UUndoInfoStruct@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006B43E0
// ?ID@?$DynamicVectorClass@UUndoInfoStruct@@@@UAEHPBUUndoInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x006B4410
// ?GetItem@?$VectorClass@UUndoInfoStruct@@@@UBE?AUUndoInfoStruct@@H@Z
// TEMPLATE: GAMEMD 0x006B4440
// ??8?$VectorClass@UUndoInfoStruct@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x006B44A0
// ?Resize@?$VectorClass@UUndoInfoStruct@@@@UAE_NHPBUUndoInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x006B45A0
// ?Clear@?$VectorClass@UUndoInfoStruct@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006B45D0
// ?ID@?$VectorClass@UUndoInfoStruct@@@@UAEHABUUndoInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x006B4620
// ?ID@?$VectorClass@UUndoInfoStruct@@@@UAEHPBUUndoInfoStruct@@@Z
template class VectorClass<UndoInfoStruct>;

// TEMPLATE: GAMEMD 0x006B4650
// ?Resize@?$DynamicVectorClass@UUndoInfoStruct@@@@UAE_NHPBUUndoInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x006B4760
// ?ID@?$DynamicVectorClass@UUndoInfoStruct@@@@UAEHABUUndoInfoStruct@@@Z
template class DynamicVectorClass<UndoInfoStruct>;

// SYNTHETIC: GAMEMD 0x006B47A0
// ??_G?$VectorClass@UUndoInfoStruct@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x006B47F0
// ??_G?$DynamicVectorClass@UUndoInfoStruct@@@@UAEPAXI@Z
