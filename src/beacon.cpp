// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0042FF80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0042FFA0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0042FFD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0042FFF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00430010 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00430030 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00430050 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00430070 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004300B0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004300E0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00430110 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00430140 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00430150 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00430170 SYMBOL
// _$E41

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
#include "session.h"
#include "shapeset.h"
#include "surface.h"
#include "tactical.h"

namespace Drawing { Rect __fastcall Intersect(Rect const & a, Rect const & b, int * x, int * y); }
extern Tactical * TacticalMap;
extern MouseClass Map;
extern ShapeSet * BeaconArt;
extern ShapeSet * RadarBeaconArt;
namespace Unsorted { extern int CurrentFrame; }

// SYNTHETIC: GAMEMD 0x00430190 SYMBOL
// _$E140
// SYNTHETIC: GAMEMD 0x004301C0 SYMBOL
// _$E138
BeaconManagerClass BeaconManager;

// FUNCTION: GAMEMD 0x00430910
BeaconManagerClass::BeaconManagerClass(void)
{
	memset(Beacons, 0, sizeof(Beacons));
	AllocatedCount = 0;
	RadarBeaconAnimPeriod = 0;
}

// FUNCTION: GAMEMD 0x00430930
BeaconManagerClass::~BeaconManagerClass(void)
{
	Reset();
}

// FUNCTION: GAMEMD 0x00430980
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

// FUNCTION: GAMEMD 0x00430210
BeaconClass::BeaconClass(void)
{
	BeaconCoord = COORD_NONE;
	IsActive = 0;
	IsFlashing = 0;
	memset(Text, 0, sizeof(Text));
}

// FUNCTION: GAMEMD 0x00430250
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
		if (Session.Messages.Is_Edit() && timeGetTime() % 500 < 250) {
			wchar_t * cursor = wcsrchr(text, L'_');
			if (cursor && wcslen(cursor) == 1) {
				*cursor = L' ';
			}
		}
	}
	if (!IsFlashing || !Session.Messages.Is_Edit()) {
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

// FUNCTION: GAMEMD 0x00430580
void __stdcall FUN_00430580(int, int, int, int, int, int, int)
{
}

// FUNCTION: GAMEMD 0x00430590
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

// FUNCTION: GAMEMD 0x004305E0
void BeaconClass::SetFlashing(bool flashing)
{
	IsFlashing = flashing;
}

// FUNCTION: GAMEMD 0x00430600
void FUN_00430600(void)
{
}

// FUNCTION: GAMEMD 0x00430610
void BeaconClass::ClearActive(void)
{
	IsActive = 0;
}

// FUNCTION: GAMEMD 0x00430620
void BeaconClass::SetText(wchar_t const * text)
{
	memset(Text, 0, sizeof(Text));
	if (text) {
		wcsncpy(Text, text, 127);
	}
}

// FUNCTION: GAMEMD 0x00430650
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

// FUNCTION: GAMEMD 0x004308B0
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

// FUNCTION: GAMEMD 0x004309D0
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

// FUNCTION: GAMEMD 0x00430AC0
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

// FUNCTION: GAMEMD 0x00430F30
bool BeaconManagerClass::CanPlaceBeacon(int house)
{
	for (int i = 0; i < 3; i++) {
		if (!Beacons[house][i]) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00430F60
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

// FUNCTION: GAMEMD 0x00430F70
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

// FUNCTION: GAMEMD 0x00431060
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

// FUNCTION: GAMEMD 0x00431090
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

// FUNCTION: GAMEMD 0x00431170
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

#include "ipxmgr.h"
#include "revent.h"
extern Surface * DAT_00880A04;
char * __fastcall FUN_007350C0(wchar_t const * text);
wchar_t * __fastcall FUN_00735120(char const * text);
extern char DAT_00889F64[];
extern IPXManagerClass Ipx;

#pragma pack(push, 1)
struct BeaconPacket {
	int Command;
	char Name[55];
	unsigned char ID;
	unsigned char House;
	char Rest[456 - 0x3d];
};
#pragma pack(pop)

// FUNCTION: GAMEMD 0x004311C0
void BeaconManagerClass::DeleteBeacon(int house, int id)
{
	if (!AllocatedCount) {
		goto end;
	}
	bool selected;
	BeaconClass * beacon;
	BeaconPacket packet;
	int beaconhouse;
	int k;
	selected = house == -1 && id == -1;
	if (selected) {
		beacon = FindSelected(-1);
	} else {
		beacon = Beacons[house][id];
	}
	if (!beacon) {
		return;
	}
	beaconhouse = beacon->HouseID;
	if (beaconhouse != PlayerPtr->HeapID && selected) {
		beacon->IsActive = 0;
		beacon->IsFlashing = 0;
		return;
	}
	if (beacon->IsActive && PlayerPtr->Is_Ally((HousesType)beaconhouse) && Houses[beacon->HouseID]->Is_Ally(PlayerPtr) && !Houses[beacon->HouseID]->IsDefeated) {
		beacon->DrawRadar(DAT_00880A04, DAT_00880A04->Get_Rect(), true);
	}
	delete beacon;
	for (k = 0; k < 3; k++) {
		if (beacon == Beacons[beaconhouse][k]) {
			Beacons[beaconhouse][k] = 0;
			AllocatedCount--;
			if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
				goto end;
			}
			if (beaconhouse == PlayerPtr->HeapID && selected) {
				packet.Command = 0x21;
				strcpy(packet.Name, FUN_007350C0(Session.Players[0]->Name));
				packet.House = (unsigned char)beaconhouse;
				packet.ID = (unsigned char)k;
				for (int p = 1; p < Session.Players.Count(); p++) {
					FUN_004068E0("Sending beacon delete to %s\n", Session.Players[p]);
					Ipx.Send_Global_Message(&packet, 0x3d, 1, &Session.Players[p]->Address, 0, 0);
				}
			}
			goto end;
		}
	}
end:
	return;
}

// FUNCTION: GAMEMD 0x00431410
void BeaconManagerClass::DeleteAllBeacons(int house)
{
	if (AllocatedCount) {
		for (int i = 0; i < 3; i++) {
			if (Beacons[house][i]) {
				DeleteBeacon(house, i);
			}
		}
	}
}

#pragma pack(push, 1)
struct BeaconTextPacket {
	int Command;
	char Name[43];
	unsigned char ID;
	char House;
	wchar_t Text[128];
	char Rest[456 - 0x131];
};
#pragma pack(pop)

// FUNCTION: GAMEMD 0x00431450
void BeaconManagerClass::EditBeaconMessage(wchar_t * message, int house, int id, bool broadcast)
{
	bool selected = house == -1 && id == -1;
	BeaconClass * beacon;
	BeaconTextPacket packet;
	if (selected) {
		beacon = FindSelected(-1);
	} else {
		beacon = Beacons[house][id];
	}
	if (!beacon) {
		return;
	}
	beacon->SetText(message);
	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
		return;
	}
	if (selected && beacon->HouseID == PlayerPtr->HeapID && broadcast) {
		packet.Command = 0x22;
		strcpy(packet.Name, FUN_007350C0(Session.Players[0]->Name));
		if (message) {
			wcsncpy(packet.Text, message, 128);
		} else {
			wcscpy(packet.Text, FUN_00735120(DAT_00889F64));
		}
		packet.House = (char)PlayerPtr->HeapID;
		packet.ID = 0xff;
		for (int j = 0; j < 3; j++) {
			if (Beacons[packet.House][j] == beacon) {
				packet.ID = j;
			}
		}
		if (packet.ID == 0xff) {
			return;
		}
		for (int p = 1; p < Session.Players.Count(); p++) {
			FUN_004068E0("Sending beacon text to %s\n", Session.Players[p]);
			Ipx.Send_Global_Message(&packet, 0x1c7, 1, &Session.Players[p]->Address, 0, 0);
		}
	} else if (beacon->HouseID != PlayerPtr->HeapID) {
		if (beacon->VisibleToPlayer()) {
			Coord coord = beacon->BeaconCoord;
			RadarEventClass::Create((RadarEventType)0xb, coord.As_Cell());
		}
	}
}

// FUNCTION: GAMEMD 0x00431700
void BeaconManagerClass::DrawRadar(Surface * surface, Rect bounds)
{
	if (AllocatedCount) {
		if (Unsorted::CurrentFrame % RadarBeaconAnimPeriod < BeaconManager.RadarBeaconFrameCount + 1) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 3; j++) {
					BeaconClass * beacon = Beacons[i][j];
					if (beacon && beacon->VisibleToPlayer()) {
						beacon->DrawRadar(surface, bounds, false);
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00431800
bool BeaconManagerClass::RadarBeaconPulsing(void)
{
	if (AllocatedCount) {
		bool any = false;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				BeaconClass * beacon = Beacons[i][j];
				if (beacon && beacon->VisibleToPlayer()) {
					any = true;
					break;
				}
			}
		}
		if (any) {
			if (Unsorted::CurrentFrame % RadarBeaconAnimPeriod < BeaconManager.RadarBeaconFrameCount + 1) {
				return true;
			}
		}
	}
	return false;
}

// SYNTHETIC: GAMEMD 0x004318D0 SYMBOL
// _$E154
Rect DAT_0089C480(0, 0, 0, 0);
