// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x00554430 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00554460 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00554480 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005544A0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005544C0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005544E0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00554500 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00554540 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00554570 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005545A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005545D0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00554600 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00554620 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x00554640 SYMBOL
// _$E170
// SYNTHETIC: GAMEMD 0x00554680 SYMBOL
// _$E168
DynamicVectorClass<LightSourceClass *> LightSources;
// SYNTHETIC: GAMEMD 0x005546C0 SYMBOL
// _$E176
// SYNTHETIC: GAMEMD 0x00554700 SYMBOL
// _$E173
DynamicVectorClass<LightSourceClass::PendingCellClass *> LightSourceClass::PendingCells;

bool LightSourceClass::Recalc = true;

// FUNCTION: GAMEMD 0x00554760
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

// FUNCTION: GAMEMD 0x00554830
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

// FUNCTION: GAMEMD 0x005548E0
LightSourceClass::LightSourceClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x00554910
LightSourceClass::~LightSourceClass(void)
{
	Detach_This_From_All(this);
	LightSources.Delete(this);

	Disable();
}

// FUNCTION: GAMEMD 0x005549A0
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

// FUNCTION: GAMEMD 0x00554A60
void LightSourceClass::Enable(bool defer)
{
	if (!IsEnabled) {
		IsEnabled = true;
		Recalculate_Affected_Cells(defer);
	}
}

// FUNCTION: GAMEMD 0x00554A80
void LightSourceClass::Disable(bool defer)
{
	if (IsEnabled) {
		IsEnabled = false;
		Recalculate_Affected_Cells(defer);
	}
}

// FUNCTION: GAMEMD 0x00554AA0
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

// FUNCTION: GAMEMD 0x00554AF0
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

// FUNCTION: GAMEMD 0x00554D50
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

// FUNCTION: GAMEMD 0x00555070
void LightSourceClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: GAMEMD 0x00555080
HRESULT STDMETHODCALLTYPE LightSourceClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_LightSource;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x005550C0
HRESULT STDMETHODCALLTYPE LightSourceClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) LightSourceClass(NoInitClass());

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00555110
HRESULT STDMETHODCALLTYPE LightSourceClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00555130
int LightSourceClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00555140
RTTIType LightSourceClass::Fetch_RTTI(void) const
{
	return(RTTI_LIGHTSOURCE);
}

// SYNTHETIC: GAMEMD 0x00555150
// ??_GLightSourceClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00555200
// ?Clear@?$DynamicVectorClass@PAVLightSourceClass@@@@UAEXXZ
template class DynamicVectorClass<LightSourceClass *>;

// TEMPLATE: GAMEMD 0x00555230
// ?ID@?$DynamicVectorClass@PAVLightSourceClass@@@@UAEHPBQAVLightSourceClass@@@Z

// TEMPLATE: GAMEMD 0x00555250
// ?GetItem@?$VectorClass@PAVLightSourceClass@@@@UBEPAVLightSourceClass@@H@Z

// TEMPLATE: GAMEMD 0x00555260
// ?Clear@?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEXXZ
template class DynamicVectorClass<LightSourceClass::PendingCellClass *>;

// TEMPLATE: GAMEMD 0x00555290
// ?ID@?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEHPBQAVPendingCellClass@LightSourceClass@@@Z

// TEMPLATE: GAMEMD 0x005552B0
// ?GetItem@?$VectorClass@PAVPendingCellClass@LightSourceClass@@@@UBEPAVPendingCellClass@LightSourceClass@@H@Z

// TEMPLATE: GAMEMD 0x005552C0
// ??8?$VectorClass@PAVLightSourceClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00555300
// ?Resize@?$VectorClass@PAVLightSourceClass@@@@UAE_NHPBQAVLightSourceClass@@@Z
// TEMPLATE: GAMEMD 0x005553B0
// ?Clear@?$VectorClass@PAVLightSourceClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005553E0
// ?ID@?$VectorClass@PAVLightSourceClass@@@@UAEHABQAVLightSourceClass@@@Z
// TEMPLATE: GAMEMD 0x00555420
// ?ID@?$VectorClass@PAVLightSourceClass@@@@UAEHPBQAVLightSourceClass@@@Z
template class VectorClass<LightSourceClass *>;

// TEMPLATE: GAMEMD 0x00555440
// ?Resize@?$DynamicVectorClass@PAVLightSourceClass@@@@UAE_NHPBQAVLightSourceClass@@@Z
// TEMPLATE: GAMEMD 0x005554F0
// ?ID@?$DynamicVectorClass@PAVLightSourceClass@@@@UAEHABQAVLightSourceClass@@@Z
template class DynamicVectorClass<LightSourceClass *>;

// TEMPLATE: GAMEMD 0x00555520
// ??8?$VectorClass@PAVPendingCellClass@LightSourceClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00555560
// ?Resize@?$VectorClass@PAVPendingCellClass@LightSourceClass@@@@UAE_NHPBQAVPendingCellClass@LightSourceClass@@@Z
// TEMPLATE: GAMEMD 0x00555610
// ?Clear@?$VectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00555640
// ?ID@?$VectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEHABQAVPendingCellClass@LightSourceClass@@@Z
// TEMPLATE: GAMEMD 0x00555680
// ?ID@?$VectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEHPBQAVPendingCellClass@LightSourceClass@@@Z
template class VectorClass<LightSourceClass::PendingCellClass *>;

// TEMPLATE: GAMEMD 0x005556A0
// ?Resize@?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@@@UAE_NHPBQAVPendingCellClass@LightSourceClass@@@Z
// TEMPLATE: GAMEMD 0x00555750
// ?ID@?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEHABQAVPendingCellClass@LightSourceClass@@@Z
template class DynamicVectorClass<LightSourceClass::PendingCellClass *>;

// SYNTHETIC: GAMEMD 0x00555780
// ??_G?$VectorClass@PAVLightSourceClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005557D0
// ??_G?$DynamicVectorClass@PAVLightSourceClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00555820
// ??_G?$VectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00555870
// ??_G?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@@@UAEPAXI@Z
