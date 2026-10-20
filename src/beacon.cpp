// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"

// FUNCTION: 0x0042FF80 $E2
// FUNCTION: 0x0042FFA0 $E5
// FUNCTION: 0x0042FFD0 $E8
// FUNCTION: 0x0042FFF0 $E11
// FUNCTION: 0x00430010 $E14
// FUNCTION: 0x00430030 $E17
// FUNCTION: 0x00430050 $E20
// FUNCTION: 0x00430070 $E23
// FUNCTION: 0x004300B0 $E26
// FUNCTION: 0x004300E0 $E29
// FUNCTION: 0x00430110 $E32
// FUNCTION: 0x00430140 $E35
// FUNCTION: 0x00430150 $E38
// FUNCTION: 0x00430170 $E41

#include "_convert.h"
#include "mouse.h"
#include <windows.h>
#include <mmsystem.h>
#include <wchar.h>
#include "beacon.h"
#include "bitfont.h"
#include "convert.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "house.h"
#include "scheme.h"
#include "shapeset.h"
#include "surface.h"
#include "tactical.h"

namespace Drawing { Rect __fastcall Intersect(Rect const & a, Rect const & b, int * x, int * y); }
extern Tactical * TacticalMap;
extern MouseClass Map;
extern bool DAT_00A8BC79;
extern ShapeSet * BeaconArt;
extern ShapeSet * RadarBeaconArt;
namespace Unsorted { extern int CurrentFrame; }

// FUNCTION: 0x00430190 $E126
// FUNCTION: 0x004301C0 $E124
BeaconManagerClass BeaconManager;

// FUNCTION: 0x00430910
BeaconManagerClass::BeaconManagerClass(void)
{
	memset(Beacons, 0, sizeof(Beacons));
	AllocatedCount = 0;
	RadarBeaconAnimPeriod = 0;
}

// FUNCTION: 0x00430930
BeaconManagerClass::~BeaconManagerClass(void)
{
	Reset();
}

// FUNCTION: 0x00430980
void BeaconManagerClass::Reset(void)
{
	if (AllocatedCount) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				if (Beacons[i][j]) {
					delete Beacons[i][j];
					Beacons[i][j] = 0;
				}
			}
		}
		AllocatedCount = 0;
	}
}

// FUNCTION: 0x00430210
BeaconClass::BeaconClass(void)
{
	BeaconCoord = COORD_NONE;
	IsActive = 0;
	IsFlashing = 0;
	memset(Text, 0, sizeof(Text));
}

// FUNCTION: 0x00430250
void BeaconClass::Draw(Surface * surface, Rect bounds)
{
	ColorScheme * scheme = ColorSchemes[Houses[HouseID]->Scheme];
	Point2D pt;
	if (!TacticalMap->Coord_To_Pixel(BeaconCoord, pt)) {
		return;
	}

	int half = BeaconManager.BeaconFrameCount / 2;
	Draw_Shape(*surface, *(ConvertClass *)scheme->Converter, BeaconArt, Unsorted::CurrentFrame % half + (IsFlashing ? half : 0), pt, bounds, (ShapeFlags_Type)0x600, 0, 0, ZGRAD_GROUND, 1000, 0, 0, 0, Point2D(0, 0));

	if (wcslen(Text) == 0) {
		return;
	}
	wchar_t text[128];
	memcpy(text, Text, sizeof(text));

	if (IsFlashing) {
		if (DAT_00A8BC79 && timeGetTime() % 500 < 250) {
			wchar_t * cursor = wcsrchr(text, L'_');
			if (cursor && wcslen(cursor) == 1) {
				*cursor = L' ';
			}
		}
	}
	if (!IsFlashing || !DAT_00A8BC79) {
		wchar_t * cursor = wcsrchr(text, L'_');
		if (!cursor || wcslen(cursor) != 1) {
			cursor = wcsrchr(text, L' ');
			if (cursor && wcslen(cursor) == 1) {
				*cursor = 0;
			}
		} else {
			*cursor = 0;
		}
	}

	BitFont * font = BitFont::Instance;
	int width = 0;
	int height = 0;
	font->GetTextDimension(text, &width, &height, 200);
	Rect box(pt.X - width / 2 - 4, pt.Y + 32, width + 8, height + 8);
	RGBClass rgb = scheme->HSV;
	Rect clip = Drawing::Intersect(box, bounds, 0, 0);
	int color = DSurface::Build_Hicolor_Pixel(rgb);
	surface->Fill_Rect(clip, 0);
	surface->Draw_Rect(clip, color);
	font->SetClipMode(true);
	LTRBStruct ltrb = { bounds.X, bounds.Y, bounds.X + bounds.Width - 1, bounds.Y + bounds.Height - 1 };
	font->SetBounds(&ltrb);
	font->SetColor(color);
	BitText::Instance->DrawText(font, surface, text, box.X + 4, box.Y + 4, 200, box.Height, false, 0, 0);
}

// FUNCTION: 0x00430580
void __stdcall FUN_00430580(int, int, int, int, int, int, int)
{
}

// FUNCTION: 0x00430590
void BeaconClass::SetCoordAndHouse(Coord coord, int house)
{
	if (!(coord == COORD_NONE)) {
		BeaconCoord = coord;
	}
	if (house < 8) {
		HouseID = house;
		IsActive = 1;
	}
}

// FUNCTION: 0x004305E0
void BeaconClass::SetFlashing(bool flashing)
{
	IsFlashing = flashing;
}

// FUNCTION: 0x00430600
void FUN_00430600(void)
{
}

// FUNCTION: 0x00430610
void BeaconClass::ClearActive(void)
{
	IsActive = 0;
}

// FUNCTION: 0x00430620
void BeaconClass::SetText(wchar_t const * text)
{
	memset(Text, 0, sizeof(Text));
	if (text) {
		wcsncpy(Text, text, 127);
	}
}

// FUNCTION: 0x00430650
void BeaconClass::DrawRadar(Surface * surface, Rect bounds, bool toclear)
{
	int frame = Unsorted::CurrentFrame % BeaconManager.RadarBeaconAnimPeriod;
	if (frame >= BeaconManager.RadarBeaconFrameCount + 1 && !toclear) {
		return;
	}

	Point2D pt = Map.Coord_To_Radar_Pixel(BeaconCoord, true);
	if (frame < BeaconManager.RadarBeaconFrameCount && !toclear) {
		Draw_Shape(*surface, *(ConvertClass *)ColorSchemes[Houses[HouseID]->Scheme]->Converter, RadarBeaconArt, frame, pt, bounds, (ShapeFlags_Type)0x600, 0, 0, ZGRAD_GROUND, 1000, 0, 0, 0, Point2D(0, 0));
	}

	int w = BeaconManager.RadarBeaconSize.X;
	int h = BeaconManager.RadarBeaconSize.Y;
	Rect box(pt.X - w / 2, pt.Y - h / 2, w, h);
	Map.LastDrawRect = Drawing::Intersect(Union(Map.LastDrawRect, box.Bias_To(Map.RadarRect)), Map.RadarRect, 0, 0);

	for (int y = box.Y; y < box.Y + h; y++) {
		for (int x = box.X; x < box.X + w; x++) {
			Map.Radar_Pixel(Point2D(x, y));
		}
	}
}

// FUNCTION: 0x004308B0
bool BeaconClass::VisibleToPlayer(void) const
{
	if (IsActive && PlayerPtr->Is_Ally((HousesType)HouseID) && Houses[HouseID]->Is_Ally(PlayerPtr) && !Houses[HouseID]->IsDefeated) {
		return true;
	}
	return false;
}

#include "rawfile.h"
struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};
void * Load_Alloc_Data(FileClass & file);
extern double __cdecl FUN_004CAC40(double);
void __cdecl FUN_004068E0(char const * format, ...);
void __fastcall UnselectAll(void);

// FUNCTION: 0x004309D0
void BeaconManagerClass::LoadArt(void)
{
	RawFileClass file("PBEACON.SHP");
	BeaconArt = (ShapeSet *)Load_Alloc_Data(file);
	if (!BeaconArt) {
		BeaconArt = (ShapeSet *)FileSystem::LoadFile("PBEACON.SHP", false);
	}
	if (BeaconArt) {
		BeaconSize.X = BeaconArt->Get_Width();
		BeaconSize.Y = BeaconArt->Get_Height();
		BeaconFrameCount = BeaconArt->Get_Count();
	} else {
		FUN_004068E0("Failed to load BeaconArt");
	}

	RawFileClass radarfile("RDRBEACN.SHP");
	RadarBeaconArt = (ShapeSet *)Load_Alloc_Data(radarfile);
	if (!RadarBeaconArt) {
		RadarBeaconArt = (ShapeSet *)FileSystem::LoadFile("RDRBEACN.SHP", false);
	}
	if (RadarBeaconArt) {
		RadarBeaconSize.X = RadarBeaconArt->Get_Width();
		RadarBeaconSize.Y = RadarBeaconArt->Get_Height();
		RadarBeaconFrameCount = RadarBeaconArt->Get_Count();
		RadarBeaconAnimPeriod = RadarBeaconFrameCount * 4;
	} else {
		FUN_004068E0("Failed to load RadarBeaconArt");
	}
}

// FUNCTION: 0x00430AC0
void BeaconManagerClass::Draw(Surface * surface, Rect bounds)
{
	if (AllocatedCount) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				BeaconClass * beacon = Beacons[i][j];
				if (beacon && beacon->VisibleToPlayer()) {
					beacon->Draw(surface, bounds);
				}
			}
		}
	}
}

// FUNCTION: 0x00430F30
bool BeaconManagerClass::CanPlaceBeacon(int house)
{
	for (int i = 0; i < 3; i++) {
		if (!Beacons[house][i]) {
			return true;
		}
	}
	return false;
}

// FUNCTION: 0x00430F60
Point2D __stdcall FUN_00430F60(void)
{
	return Point2D(0, 0);
}

static inline int Coord_Dist(Coord const & a, Coord const & b)
{
	Coord c = a - b;
	return c.Length();
}
static inline int Coord_Dist2(int x, int y, int z, Coord const & b)
{
	Coord c = Coord(x, y, z) - b;
	return c.Length();
}
static inline int Coord_Dist3(Coord a, Coord b)
{
	return (a - b).Length();
}

// FUNCTION: 0x00430F70
bool BeaconManagerClass::SelectBeacon(int x, int y, int z)
{
	int i;
	int j;
	if (AllocatedCount) {
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 3; j++) {
				if (Beacons[i][j] && Coord_Dist3(Coord(x, y, z), Beacons[i][j]->BeaconCoord) < 128) {
					goto found;
				}
			}
		}
	}
	return false;
found:
	BeaconClass * beacon = Beacons[i][j];
	if (!beacon) {
		return false;
	}
	UnselectAll();
	beacon->IsFlashing = 1;
	return true;
}

// FUNCTION: 0x00431060
void BeaconManagerClass::ClearSelection(void)
{
	if (AllocatedCount) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				if (Beacons[i][j]) {
					Beacons[i][j]->IsFlashing = 0;
				}
			}
		}
	}
}

// FUNCTION: 0x00431090
BeaconClass * BeaconManagerClass::FindBeaconAt(Coord coord)
{
	if (AllocatedCount) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				if (Beacons[i][j] && Coord_Dist3(coord, Beacons[i][j]->BeaconCoord) < 128) {
					return Beacons[i][j];
				}
			}
		}
	}
	return 0;
}

// FUNCTION: 0x00431170
BeaconClass * BeaconManagerClass::FindSelected(int house)
{
	if (AllocatedCount) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				BeaconClass * beacon = Beacons[i][j];
				if (beacon && beacon->IsFlashing) {
					if (house == -1 || house == beacon->HouseID) {
						return beacon;
					}
				}
			}
		}
	}
	return 0;
}
