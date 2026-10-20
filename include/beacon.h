// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "rect.h"

class Surface;

class BeaconClass
{
	public:
		BeaconClass(void);

		void Draw(Surface * surface, Rect bounds);
		void SetCoordAndHouse(Coord coord, int house);
		void SetFlashing(bool flashing);
		void ClearActive(void);
		void SetText(wchar_t const * text);
		void DrawRadar(Surface * surface, Rect bounds, bool toclear = false);
		bool VisibleToPlayer(void) const;

		Coord BeaconCoord;
		unsigned char IsActive : 1;
		unsigned char IsFlashing : 1;
		unsigned char Pad_C : 6;
		unsigned char Pad_D;
		wchar_t Text[128];
		unsigned short Pad_10E;
		int HouseID;
};

class BeaconManagerClass
{
	public:
		BeaconManagerClass(void);
		~BeaconManagerClass(void);

		void Reset(void);
		void LoadArt(void);
		void Draw(Surface * surface, Rect bounds);
		void PlaceBeacon(int house, Coord coord, int id = -1);
		bool CanPlaceBeacon(int house);
		bool SelectBeacon(int x, int y, int z);
		void ClearSelection(void);
		BeaconClass * FindBeaconAt(int x, int y, int z);
		BeaconClass * FindSelected(int house);

		BeaconClass * Beacons[8][3];
		int AllocatedCount;
		Point2D BeaconSize;
		int BeaconFrameCount;
		Point2D RadarBeaconSize;
		int RadarBeaconFrameCount;
		int RadarBeaconAnimPeriod;
};
