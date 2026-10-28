// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "crate.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_tactica.h"
#include "ccrand.h"
#include "cell.h"
#include "overlay.h"
#include "rules.h"
#include "tactical.h"
#include "dsurface.h"

// SYNTHETIC: GAMEMD 0x004A1540 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x004A1560 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004A1590 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004A15B0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004A15D0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004A15F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004A1610 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004A1630 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004A1670 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004A16A0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004A16D0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004A1700 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004A1710 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004A1730 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x004A1750
bool CrateClass::Remove_It(void)
{
	if (Is_Valid()) {
		Get_Crate(Location);
		Make_Invalid();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004A17C0
bool CrateClass::Create_Crate(Cell const & cell)
{
	Remove_It();

	Cell loc = cell;
	if (Put_Crate(loc)) {
		TacticalMap->Flag_Cell(Map[cell]);
		Location = loc;
		Timer = int(Random_Double(Rule->CrateTime * (TICKS_PER_MINUTE / 2), Rule->CrateTime * (TICKS_PER_MINUTE * 2)));
		Timer.Start();
		return (true);
	}
	return (false);
}

// FUNCTION: GAMEMD 0x004A18F0
bool CrateClass::Put_Crate(Cell & cell)
{
	int old = ScenarioInit;
	ScenarioInit = 0;

	if (Map.In_Local_Radar(cell)) {
		CellClass * cellptr = &Map[cell];

		if (cellptr->Overlay == OVERLAY_NONE) {
			if (Map[cell].Land_Type() == LAND_WATER) {
				new OverlayClass(Rule->WaterCrateImg, cell);
			} else {
				new OverlayClass(Rule->WoodCrateImg, cell);
			}

			Rect rect = Union(cellptr->Overlay_Render_Rect(), cellptr->Overlay_Shadow_Render_Rect());
			Point2D pt = DSurface::ViewBounds.Top_Left();
			TacticalMap->Register_Dirty_Area(Rect(rect.X - pt.X, rect.Y - pt.Y, rect.Width, rect.Height));

			ScenarioInit = old;
			return (true);
		} else {
			ScenarioInit = old;
			return (false);
		}
	}

	ScenarioInit = old;
	return (false);
}

// FUNCTION: GAMEMD 0x004A1AA0
bool CrateClass::Get_Crate(Cell const & cell)
{
	if (Map.In_Radar(cell)) {
		CellClass * cellptr = &Map[cell];

		if (cellptr->Overlay != OVERLAY_NONE) {
			if (OverlayTypes[cellptr->Overlay] == Rule->WoodCrateImg ||
				OverlayTypes[cellptr->Overlay] == Rule->CrateImg ||
				OverlayTypes[cellptr->Overlay] == Rule->WaterCrateImg) {

				Rect dirty = Union(cellptr->Overlay_Render_Rect(), cellptr->Overlay_Shadow_Render_Rect());
				dirty.Y -= DSurface::ViewBounds.Y;
				TacticalMap->Register_Dirty_Area(dirty);

				cellptr->Overlay = OVERLAY_NONE;
				cellptr->OverlayData = 0;
				return (true);
			}
		}
	}
	return (false);
}
