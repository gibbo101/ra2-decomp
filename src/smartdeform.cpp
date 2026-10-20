// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
extern const double DAT_007F32B8;
extern const double DAT_007F32C0;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};
#define MAX_TERRAIN_HEIGHT (12)
#define NUM_CORNERS 4
#define RAMP_FULL_HEIGHT (30)
#define RAMP_HALF_HEIGHT (RAMP_FULL_HEIGHT / 2)

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

struct DeformUndoStruct {
	DeformUndoStruct(void)
	{
		CellPoint = Point2D(0, 0);
		OldHeight = 0;
	};

	DeformUndoStruct(const Point2D & point, int oldheight)
	{
		CellPoint = point;
		OldHeight = oldheight;
	};

	bool operator==(DeformUndoStruct const & rvalue) const { return(CellPoint == rvalue.CellPoint && OldHeight == rvalue.OldHeight); }
	bool operator!=(DeformUndoStruct const & rvalue) const { return(CellPoint != rvalue.CellPoint || OldHeight != rvalue.OldHeight); }

	Point2D CellPoint;
	int OldHeight;
};

// FUNCTION: 0x006B23B0 $E152
// FUNCTION: 0x006B23F0 $E150
DynamicVectorClass<DeformUndoStruct> UndoRecords;

bool Commit_Deform_Grid(Cell center, bool forced);

// FUNCTION: 0x006B2430
void Record_Deform_Undo(const Point2D & point, int oldheight)
{
	UndoRecords.Add(DeformUndoStruct(point, oldheight));
}

// FUNCTION: 0x006B24A0
void Undo_Deform(void)
{
	for (int i = UndoRecords.Count() - 1; i >= 0; i--) {
		DeformPointStruct & new_point = DeformPoints[UndoRecords[i].CellPoint.X + UndoRecords[i].CellPoint.Y * DeformPointWidth];
		new_point.Height = UndoRecords[i].OldHeight;
	}
	UndoRecords.Clear();
}

// FUNCTION: 0x006B2520
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

// FUNCTION: 0x006B25C0
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

// FUNCTION: 0x006B2700
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

// FUNCTION: 0x006B2A70
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

// FUNCTION: 0x006B3850
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

// FUNCTION: 0x006B3A80
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

// FUNCTION: 0x006B3CD0
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

// FUNCTION: 0x006B3E60
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

// FUNCTION: 0x006B4100
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

// FUNCTION: 0x006B4240
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

// FUNCTION: 0x006B43B0 ?$DynamicVectorClass@UDeformUndoStruct@@::Clear
// FUNCTION: 0x006B43E0 ?$DynamicVectorClass@UDeformUndoStruct@@::ID
// FUNCTION: 0x006B4410 ?$VectorClass@UDeformUndoStruct@@::GetItem
// FUNCTION: 0x006B4440 ?$VectorClass@UDeformUndoStruct@@::operator==
// FUNCTION: 0x006B44A0 ?$VectorClass@UDeformUndoStruct@@::Resize
// FUNCTION: 0x006B45A0 ?$VectorClass@UDeformUndoStruct@@::Clear
// FUNCTION: 0x006B45D0 ?$VectorClass@UDeformUndoStruct@@::ID
// FUNCTION: 0x006B4620 ?$VectorClass@UDeformUndoStruct@@::ID
template class VectorClass<DeformUndoStruct>;

// FUNCTION: 0x006B4650 ?$DynamicVectorClass@UDeformUndoStruct@@::Resize
// FUNCTION: 0x006B4760 ?$DynamicVectorClass@UDeformUndoStruct@@::ID
template class DynamicVectorClass<DeformUndoStruct>;

// FUNCTION: 0x006B47A0 ?$VectorClass@UDeformUndoStruct@@::~?$VectorClass@UDeformUndoStruct@@
// FUNCTION: 0x006B47F0 ?$DynamicVectorClass@UDeformUndoStruct@@::~?$DynamicVectorClass@UDeformUndoStruct@@

// FUNCTION: 0x006B4840 $E157
double DAT_00B0B710 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x006B4870 $E160
double DAT_00B0B760 = DAT_00B0B710 / DAT_007F32B8;

// FUNCTION: 0x006B4890 $E163
double DAT_00B0B748 = DAT_007E1718 / DAT_00B0B760;

// FUNCTION: 0x006B48B0 $E166
double DAT_00B0B740 = DAT_007E1720 * DAT_007F32C0;

// FUNCTION: 0x006B48D0 $E169
double DAT_00B0B718 = DAT_007E1728 * DAT_007F32C0;

// FUNCTION: 0x006B48F0 $E172
double DAT_00B0B6F0 = DAT_007E1730 * DAT_007F32C0;

// FUNCTION: 0x006B4910 $E175
int DAT_00B0B768 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0B6F0 - DAT_00B0B718) * DAT_00B0B710));

// FUNCTION: 0x006B4950 $E178
double DAT_00B0B720 = FUN_004CADE0(DAT_00B0B768 * DAT_007E1740);

// FUNCTION: 0x006B4980 $E181
double DAT_00B0B738 = FUN_004CADE0((DAT_00B0B768 * 2) / DAT_00B0B710);

// FUNCTION: 0x006B49B0 $E184
int DAT_00B0B75C = (int)(DAT_00B0B768 * 4 + DAT_007E1738);

// FUNCTION: 0x006B49E0 $E187
ShortPair DAT_00B0B708;

// FUNCTION: 0x006B49F0 $E190
IntTriple DAT_00B0B728;
