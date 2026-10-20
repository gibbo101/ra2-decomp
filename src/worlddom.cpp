// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "worlddom.h"

#include "_mixfile.h"
#include "_pk.h"
#include "_surface.h"
#include "addon.h"
#include "grphmenu.h"
#include "language\language.h"
#include "mapgen.h"
#include "mixfile.h"
#include "ownrdraw.h"
#include "rcptr.h"
#include "wdtnet.h"
#include "wonline.h"
#include "wstring.h"

#include <cmath>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F7908;
extern const double DAT_007F7910;

// FUNCTION: 0x007AF0B0 $E35
double DAT_00B77EE8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x007AF0E0 $E38
double DAT_00B77F88 = DAT_00B77EE8 / DAT_007F7908;

// FUNCTION: 0x007AF100 $E41
double DAT_00B77F40 = DAT_007E1718 / DAT_00B77F88;

// FUNCTION: 0x007AF120 $E44
double DAT_00B77F38 = DAT_007E1720 * DAT_007F7910;

// FUNCTION: 0x007AF140 $E47
double DAT_00B77EF8 = DAT_007E1728 * DAT_007F7910;

// FUNCTION: 0x007AF160 $E50
double DAT_00B77EB8 = DAT_007E1730 * DAT_007F7910;

// FUNCTION: 0x007AF180 $E53
int DAT_00B77F90 = (int)(DAT_007E1738 * (fastmath::tan(DAT_00B77EB8 - DAT_00B77EF8) * DAT_00B77EE8));

// FUNCTION: 0x007AF1C0 $E56
double DAT_00B77F00 = fastmath::atan(DAT_00B77F90 * DAT_007E1740);

// FUNCTION: 0x007AF1F0 $E59
double DAT_00B77F28 = fastmath::atan((DAT_00B77F90 * 2) / DAT_00B77EE8);

// FUNCTION: 0x007AF220 $E62
int DAT_00B77F80 = (int)(DAT_00B77F90 * 4 + DAT_007E1738);

struct WDTShortPair { short a; short b; WDTShortPair(void) { a = 0; b = 0; } };
struct WDTIntTriple { int a; int b; int c; WDTIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x007AF250 $E65
WDTShortPair DAT_00B77EE0;

// FUNCTION: 0x007AF260 $E68
WDTIntTriple DAT_00B77F10;

struct WDTCenter { int x; int y; int z; WDTCenter(void) { z = 0; x = 0x80; y = 0x80; } };

// FUNCTION: 0x007AF280 $E71
WDTCenter DAT_00B77F48;

// FUNCTION: 0x007AF2A0
Wstring & __fastcall WDT_Faction_Name(int side)
{
	static Wstring dis("DIS");
	static Wstring all("ALL");
	static Wstring sov("SOV");

	switch (side) {
		case 1: return dis;
		case 2: return all;
		case 3: return sov;
	}
	static Wstring xx("xx");
	return xx;
}

// FUNCTION: 0x007AF390 $E77
// FUNCTION: 0x007AF3A0 $E76
// FUNCTION: 0x007AF3B0 $E75
// FUNCTION: 0x007AF3C0 $E74

// FUNCTION: 0x007AF3D0
Wstring & __fastcall WDT_Country_Name(int side)
{
	static Wstring na("NA");
	static Wstring eu("EU");
	static Wstring as("AS");

	switch (side) {
		case 1: return na;
		case 2: return eu;
		case 3: return as;
	}
	static Wstring xx("XX");
	return xx;
}

// FUNCTION: 0x007AF4C0 $E82
// FUNCTION: 0x007AF4D0 $E81
// FUNCTION: 0x007AF4E0 $E80
// FUNCTION: 0x007AF4F0 $E79

namespace WorldDominationTour {
	typedef Surface FactionImage;
	class FactionSelectDialogControl : public OwnerDraw::SimpleDialogControl {
	public:
		FactionSelectDialogControl(void);
		virtual ~FactionSelectDialogControl(void);
		int FieldC;
		FactionImage * Image1;
		FactionImage * Image2;
		FactionImage * Image3;
		FactionImage * Current;
		void Update_Hover(HWND window, POINT * point);
		void Draw(int unused);
	};
}

// FUNCTION: 0x007AF500
WorldDominationTour::FactionSelectDialogControl::~FactionSelectDialogControl(void)
{
	delete Image1;
	delete Image2;
	delete Image3;
}

// FUNCTION: 0x007AF540
void WorldDominationTour::FactionSelectDialogControl::Update_Hover(HWND window, POINT * point)
{
	if (F2()) {
		return;
	}
	Current = Image1;
	int side = 0;
	int x = point->x;
	if ((x >= 16 && x < 364 && point->y >= 20 && point->y < 136) ||
		(x >= 130 && x < 300 && point->y >= 100 && point->y < 216) ||
		(x >= 175 && x < 245 && point->y >= 216 && point->y < 262)) {
		goto two;
	}
	if ((x >= 252 && x < 466 && point->y >= 180 && point->y < 382) ||
		(x >= 212 && x < 496 && point->y >= 262 && point->y < 422)) {
		side = 3;
		Current = Image3;
	} else if (false) {
two:
		side = 2;
		Current = Image2;
	}
	if (FieldC != side) {
		FieldC = side;
		InvalidateRect(window, NULL, FALSE);
	}
}

// FUNCTION: 0x007AF640
void WorldDominationTour::FactionSelectDialogControl::Draw(int unused)
{
	if (AlternateSurface != NULL && Current != NULL) {
		Rect rect = Current->Get_Rect();
		AlternateSurface->Fill_Rect(rect, 0);
		AlternateSurface->Blit_From(rect, *Current, rect, false, true);
	}
}

GraphicMenu * __fastcall FUN_004F1B80(Wstring const & ini, Wstring const & name);

// FUNCTION: 0x007AFAD0
int __fastcall WDT_Theater_Choice_Menu(int side)
{
	char buffer[128];

	sprintf(buffer, "TheaterChoice%sMenu", *WorldDominationTour::FUN_007AF2A0(side));
	GraphicMenu * gmenu = FUN_004F1B80("WDTChoiceMD.ini", buffer);
	if (gmenu == NULL) {
		return 0;
	}
	int rc = gmenu->Presentation();
	delete gmenu;
	switch (rc) {
		case 1: return 1;
		case 2: return 2;
		case 3: return 3;
	}
	return 0;
}


// FUNCTION: 0x007B0090 $E86
// FUNCTION: 0x007B00B0 $E84
WDTPointer<void> g_WDTUnusedPointer1;

// FUNCTION: 0x007B00D0 $E91
// FUNCTION: 0x007B00F0 $E89
WDTPointer<void> g_WDTUnusedPointer2;

// FUNCTION: 0x007B0110 $E96
// FUNCTION: 0x007B0130 $E94
rc_ptr<WDTState> DAT_00B77F58;

// FUNCTION: 0x007B0150 $E101
// FUNCTION: 0x007B0170 $E99
rc_ptr<WDTState> DAT_00B77F70;

// FUNCTION: 0x007B0190 $E106
// FUNCTION: 0x007B01B0 $E104
rc_ptr<WorldDominationTour::History> DAT_00B77F68;

// FUNCTION: 0x007B01D0 $E111
// FUNCTION: 0x007B01F0 $E109
rc_ptr<WorldDominationTour::History> DAT_00B77F78;

extern Wstring DAT_00B73E40;
rc_ptr<WorldDominationTour::CampaignProperties> __fastcall FUN_0076BE90(rc_ptr<WDTState> const & state);
rc_ptr<WorldDominationTour::History> __fastcall FUN_00764B30(Wstring const & name, rc_ptr<WorldDominationTour::CampaignProperties> const & props);

// FUNCTION: 0x007B0210
rc_ptr<WDTState> * __cdecl FUN_007B0210(void)
{
	return &DAT_00B77F58;
}

// FUNCTION: 0x007B0220
void __fastcall FUN_007B0220(rc_ptr<WDTState> const & state)
{
	DAT_00B77F58 = state;
	DAT_00B77F68 = FUN_00764B30(DAT_00B73E40, FUN_0076BE90(state));
}

// FUNCTION: 0x007B0290
rc_ptr<WDTState> * __cdecl FUN_007B0290(void)
{
	return &DAT_00B77F70;
}

// FUNCTION: 0x007B02A0
void __fastcall FUN_007B02A0(rc_ptr<WDTState> const & state)
{
	DAT_00B77F70 = state;
	DAT_00B77F78 = FUN_00764B30(DAT_00B73E40, FUN_0076BE90(state));
}

char * DAT_00B77F54;

// FUNCTION: 0x007B0310
char * __cdecl FUN_007B0310(void)
{
	return DAT_00B77F54;
}

// FUNCTION: 0x007B0320
void __fastcall FUN_007B0320(char * state)
{
	if (DAT_00B77F54 != NULL) {
		delete DAT_00B77F54;
	}
	DAT_00B77F54 = state;
}
