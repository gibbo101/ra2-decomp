// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ahandle.h"
#include <ctype.h>
#include <math.h>
#include "dsaudio.h"
#include "assert.h"
#include "globals.h"
#include "goptions.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x00412660 SYMBOL
// _$E10
// SYNTHETIC: GAMEMD 0x00412690 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x004126B0 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x004126D0 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x004126F0 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x00412710 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x00412730 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x00412770 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x004127A0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x004127D0 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x00412800 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x00412810 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x00412830 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x00412850 SYMBOL
// _$E49
const Rect RECT_NONE_2(0, 0, 0, 0);

#include "foot.h"

inline AircraftTrackerClass::~AircraftTrackerClass(void)
{
	Clear();
}

// SYNTHETIC: GAMEMD 0x00412870 SYMBOL
// _$E79
AircraftTrackerClass AircraftTracker;

// SYNTHETIC: GAMEMD 0x004128F0 SYMBOL
// _$E77

#include "swizzle.h"
#include "cell.h"
#include "_map.h"

void __cdecl FUN_004068E0(char const * text, ...);

// FUNCTION: GAMEMD 0x004129B0
void __fastcall FUN_004129B0(char const * text)
{
	FUN_004068E0(text);
}

// FUNCTION: GAMEMD 0x004129C0
void AircraftTrackerClass::FUN_004129C0(int index)
{
	DynamicVectorClass<TechnoClass *> & vec = TrackerVectors[index];
	int count = vec.Count();
	for (int i = 0; i < count; i++) {
		CurrentVector.Add(vec.GetItem(i));
	}
}

// FUNCTION: GAMEMD 0x00412A40
int AircraftTrackerClass::FUN_00412A40(int index, Point2D offset)
{
	int x = index % 20;
	int y = index / 20;
	x = MIN(19, MAX(0, x + offset.X));
	y = MIN(19, MAX(0, y + offset.Y));
	return x + y * 20;
}

// FUNCTION: GAMEMD 0x00412AC0
int AircraftTrackerClass::GetVectorIndex(Cell pos)
{
	int x = (pos.X < 0) ? 0 : pos.X;
	int y = (pos.Y < 0) ? 0 : pos.Y;
	int qx = x / (Map.MapRect.Width / 20);
	int cx = (qx > 19) ? 19 : qx;
	int qy = y / (Map.MapRect.Height / 20);
	int cy = (qy > 19) ? 19 : qy;
	return cx + cy * 20;
}

// FUNCTION: GAMEMD 0x00413490
void AircraftTrackerClass::FUN_00413490(AbstractClass *)
{
}

// FUNCTION: GAMEMD 0x004134A0
void AircraftTrackerClass::Add(TechnoClass * foot)
{
	Cell cell = foot->Get_Cell();
	foot->SetLastFlightMapCoords(cell);
	TrackerVectors[GetVectorIndex(cell)].Add(foot);
}

// FUNCTION: GAMEMD 0x00413590
int FUN_00413590(void)
{
	return 0x2EE;
}

// FUNCTION: GAMEMD 0x004135A0
bool AircraftTrackerClass::IsJumpjet(TechnoClass * foot)
{
	FootClass * occupier = foot->Get_Cell_Ptr()->Jumpjet;
	if (occupier != NULL) {
		if (occupier != foot) return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x004135D0
void AircraftTrackerClass::Remove(TechnoClass * foot)
{
	Cell cell = foot->GetLastFlightMapCoords();
	foot->SetLastFlightMapCoords(CELL_NONE);
	TrackerVectors[GetVectorIndex(cell)].Delete(foot);
}

// FUNCTION: GAMEMD 0x004136C0
HRESULT AircraftTrackerClass::Load(IStream * stream)
{
	int i;
	int j;
	int count;
	TechnoClass * techno;
	HRESULT hr;
	for (i = 0; i < 400; i++) {
		hr = stream->Read(&count, sizeof(count), NULL);
		if (hr < 0) return(hr);
		for (j = 0; j < count; j++) {
			hr = stream->Read(&techno, sizeof(techno), NULL);
			if (hr < 0) return(hr);
			TrackerVectors[i].Add(techno);
		}
		for (j = 0; j < count; j++) {
			Swizzler.Swizzle((void **)&TrackerVectors[i][j]);
		}
	}
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004137A0
FootClass * AircraftTrackerClass::Get(void)
{
	if (CurrentVector.Count() == 0) return(NULL);
	TechnoClass * foot = CurrentVector[0];
	CurrentVector.Delete(foot);
	return((FootClass *)foot);
}

// FUNCTION: GAMEMD 0x00413800
void AircraftTrackerClass::Clear(void)
{
	for (int i = 0; i < 400; i++) {
		TrackerVectors[i].Delete_All();
	}
	CurrentVector.Delete_All();
}

// FUNCTION: GAMEMD 0x00413850
HRESULT AircraftTrackerClass::Save(IStream * stream)
{
	int i;
	int j;
	int count;
	HRESULT hr;
	for (i = 0; i < 400; i++) {
		count = TrackerVectors[i].Count();
		hr = stream->Write(&count, sizeof(count), NULL);
		if (hr < 0) return(hr);
		for (j = 0; j < count; j++) {
			hr = stream->Write(&TrackerVectors[i][j], sizeof(TechnoClass *), NULL);
			if (hr < 0) return(hr);
		}
	}
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004138C0
void AircraftTrackerClass::Update(TechnoClass * foot, Cell oldcell, Cell newcell)
{
	foot->SetLastFlightMapCoords(newcell);
	int oldindex = GetVectorIndex(oldcell);
	int newindex = GetVectorIndex(newcell);
	if (oldindex != newindex) {
		TrackerVectors[oldindex].Delete(foot);
		TrackerVectors[newindex].Add(foot);
	}
}

// TEMPLATE: GAMEMD 0x00413A70
// ?Add@?$DynamicVectorClass@PAVTechnoClass@@@@QAE_NABQAVTechnoClass@@@Z
template class DynamicVectorClass<TechnoClass *>;
