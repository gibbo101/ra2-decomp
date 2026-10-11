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
#define MAX_TERRAIN_HEIGHT (12)
#define NUM_CORNERS 4
#define RAMP_FULL_HEIGHT (ISO_TILE_PIXEL_H)
#define RAMP_HALF_HEIGHT (RAMP_FULL_HEIGHT / 2)

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

DynamicVectorClass<DeformUndoStruct> UndoRecords;

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
