// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "light.h"
#include "_map.h"
#include "_milsectmr.h"
#include "cell.h"
#include "crc.h"
#include "globals.h"
#include "isotype.h"
#include "lightcon.h"
#include "map.h"
#include "milsectmr.h"
#include "goptions.h"
#include "sun.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x00554640 $E125
// FUNCTION: 0x00554680 $E123
DynamicVectorClass<LightSourceClass *> LightSources;
// FUNCTION: 0x005546C0 $E131
// FUNCTION: 0x00554700 $E128
DynamicVectorClass<LightSourceClass::PendingCellClass *> LightSourceClass::PendingCells;

bool LightSourceClass::Recalc = true;

// FUNCTION: 0x00554760
LightSourceClass::LightSourceClass(Coord coord, int visibility, int intensity, int red, int green, int blue) :
	BASECLASS(),
	Intensity(intensity),
	RedTint(red),
	GreenTint(green),
	BlueTint(blue),
	DetailLevel(2),
	Position(coord),
	Visibility(visibility),
	IsEnabled(false)
{
	LightSources.Add(this);
}

// FUNCTION: 0x00554830
LightSourceClass::LightSourceClass(void) :
	BASECLASS(),
	Intensity(0),
	RedTint(0),
	GreenTint(0),
	BlueTint(0),
	Position(COORD_NONE),
	Visibility(0),
	IsEnabled(false)
{
	LightSources.Add(this);
}

// FUNCTION: 0x005548E0
LightSourceClass::LightSourceClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x00554910
LightSourceClass::~LightSourceClass(void)
{
	Detach_This_From_All(this);
	LightSources.Delete(this);

	Disable();
}

// FUNCTION: 0x005549A0
void LightSourceClass::Reset(void)
{
	Recalc = false;

	while (LightSources.Count() != 0) {
		delete LightSources[0];
		LightSources.Delete_Index(0);
	}

	while (PendingCells.Count() != 0) {
		delete PendingCells[0];
		PendingCells.Delete_Index(0);
	}

	Recalc = true;
}

// FUNCTION: 0x00554A60
void LightSourceClass::Enable(bool defer)
{
	if (!IsEnabled) {
		IsEnabled = true;
		Recalculate_Affected_Cells(defer);
	}
}

// FUNCTION: 0x00554A80
void LightSourceClass::Disable(bool defer)
{
	if (IsEnabled) {
		IsEnabled = false;
		Recalculate_Affected_Cells(defer);
	}
}

// FUNCTION: 0x00554AA0
void LightSourceClass::ChangeLevels(int intensity, int red, int green, int blue, bool defer)
{
	if (Intensity != intensity || RedTint != red || RedTint != blue) {
		Intensity = intensity;
		RedTint = red;
		GreenTint = green;
		BlueTint = blue;
		if (IsEnabled) {
			Recalculate_Affected_Cells(defer);
		}
	}
}

// FUNCTION: 0x00554AF0
void LightSourceClass::Recalculate_Affected_Cells(bool defer)
{
	if (Recalc && Options.DetailLevel >= DetailLevel) {
		if (defer && PendingCells.Count() != 0) {
			LightSourceClass::Process_Lighting(0, true);
		}

		Cell center = Position.As_Cell();
		int radius = Visibility / CELL_LEPTON + 1;

		for (int y = -radius; y <= radius; y++) {
			for (int x = -radius; x <= radius; x++) {
				Cell test_cell = center + Cell(x, y);
				if (test_cell.X >= 0 && test_cell.X < MAP_CELL_W && test_cell.Y >= 0 && test_cell.Y < MAP_CELL_H && Map.Is_Valid(test_cell)) {
					Coord cell_coord = test_cell.As_Coord();
					int dx = cell_coord.X - Position.X;
					int dy = cell_coord.Y - Position.Y;
					int dist = dx * dx + dy * dy;

					if (int(fastmath::sqrt(dist)) <= Visibility) {
						if (defer) {
							PendingCells.Add(new PendingCellClass(test_cell));
						} else {
							Map[test_cell].Init_Drawer();
						}
					}
				}
			}
		}

		Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
	}
}

// FUNCTION: 0x00554D50
void LightSourceClass::Process_Lighting(int time_budget_ms, bool force)
{
	static unsigned _update_frames = 0;

	double update_start_time = MillisecondTimer;

	static unsigned _update_interval = 50;
	static unsigned _last_update_frame = 0;
	static int _pending_index = 0;
	static int _pending_remaining = 0;
	static bool _commit_ready = false;

	_update_frames++;

	if (PendingCells.Count() && !_commit_ready) {
		if (_pending_index <= 0) {
			_pending_remaining = PendingCells.Count();
			_pending_index = _pending_remaining - 1;
		}
		for (; _pending_index >= 0; _pending_index--) {
			if (!force && ((unsigned)_pending_index % 16) == 15) {
				if (MillisecondTimer - update_start_time >= time_budget_ms) {
					break;
				}
			}

			if (PendingCells[_pending_index]->Converter != NULL) {
				_pending_remaining--;
			} else {
				LightConvertClass * drawer;
				int intensity, ambient, brightness, tile_brightness, alt_brightness;
				Map[PendingCells[_pending_index]->CellID].Pick_Drawer(drawer, intensity, ambient, brightness, tile_brightness, alt_brightness);
				PendingCells[_pending_index]->Converter = drawer;
				PendingCells[_pending_index]->Intensity = intensity;
				PendingCells[_pending_index]->Ambient = ambient;
				PendingCells[_pending_index]->Brightness = brightness;
				PendingCells[_pending_index]->TileBrightness = tile_brightness;
				PendingCells[_pending_index]->AltBrightness = alt_brightness;
				_pending_remaining--;
			}
		}
		if (_pending_remaining <= 0) {
			_commit_ready = true;
		}
	}

	double time_left = time_budget_ms - (MillisecondTimer - update_start_time);

	if (_commit_ready) {
		if (force || time_left >= time_budget_ms - 1) {
			for (int i = 0; i < PendingCells.Count(); i++) {
				PendingCellClass * pending = PendingCells[i];
				Map[pending->CellID].Init_Drawer(pending->Converter, pending->Intensity, pending->Ambient, pending->Brightness, pending->TileBrightness, pending->AltBrightness);
				delete pending;
			}
			PendingCells.Clear();
			_commit_ready = false;
			Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
		}
	}

	time_left = time_budget_ms - (MillisecondTimer - update_start_time);

	if (time_left > 0.0 && _update_frames > (_last_update_frame + _update_interval)) {
		bool more_to_free = IsometricTileTypeClass::Free_Unused_Drawers(MIN(time_left * 0.5, 1.0), false);
		_last_update_frame = _update_frames;
		_update_interval = more_to_free ? 1 : 50;
	}
}

// FUNCTION: 0x00555070
void LightSourceClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: 0x00555080
HRESULT STDMETHODCALLTYPE LightSourceClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_LightSource;
	return(S_OK);
}

// FUNCTION: 0x005550C0
HRESULT STDMETHODCALLTYPE LightSourceClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) LightSourceClass(NoInitClass());

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00555110
HRESULT STDMETHODCALLTYPE LightSourceClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00555130
int LightSourceClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00555140
RTTIType LightSourceClass::Fetch_RTTI(void) const
{
	return(RTTI_LIGHTSOURCE);
}

// FUNCTION: 0x00555150 LightSourceClass::~LightSourceClass

// FUNCTION: 0x00555200 ?$DynamicVectorClass@PAVLightSourceClass@@::Clear
template class DynamicVectorClass<LightSourceClass *>;

// FUNCTION: 0x00555230 ?$DynamicVectorClass@PAVLightSourceClass@@::ID

// FUNCTION: 0x00555250 ?$VectorClass@PAVLightSourceClass@@::GetItem

// FUNCTION: 0x00555260 ?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@::Clear
template class DynamicVectorClass<LightSourceClass::PendingCellClass *>;

// FUNCTION: 0x00555290 ?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@::ID

// FUNCTION: 0x005552B0 ?$VectorClass@PAVPendingCellClass@LightSourceClass@@::GetItem
