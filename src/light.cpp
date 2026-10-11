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
#include "sun.h"
#include "tracker.h"
#include "vector.h"

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

// FUNCTION: 0x00555080
HRESULT STDMETHODCALLTYPE LightSourceClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_LightSource;
	return(S_OK);
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
