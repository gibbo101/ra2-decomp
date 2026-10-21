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
#include "pcx.h"
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

// SYNTHETIC: GAMEMD 0x007AF0B0 SYMBOL
// _$E35
double DAT_00B77EE8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x007AF0E0 SYMBOL
// _$E38
double DAT_00B77F88 = DAT_00B77EE8 / DAT_007F7908;

// SYNTHETIC: GAMEMD 0x007AF100 SYMBOL
// _$E41
double DAT_00B77F40 = DAT_007E1718 / DAT_00B77F88;

// SYNTHETIC: GAMEMD 0x007AF120 SYMBOL
// _$E44
double DAT_00B77F38 = DAT_007E1720 * DAT_007F7910;

// SYNTHETIC: GAMEMD 0x007AF140 SYMBOL
// _$E47
double DAT_00B77EF8 = DAT_007E1728 * DAT_007F7910;

// SYNTHETIC: GAMEMD 0x007AF160 SYMBOL
// _$E50
double DAT_00B77EB8 = DAT_007E1730 * DAT_007F7910;

// SYNTHETIC: GAMEMD 0x007AF180 SYMBOL
// _$E53
int DAT_00B77F90 = (int)(DAT_007E1738 * (fastmath::tan(DAT_00B77EB8 - DAT_00B77EF8) * DAT_00B77EE8));

// SYNTHETIC: GAMEMD 0x007AF1C0 SYMBOL
// _$E56
double DAT_00B77F00 = fastmath::atan(DAT_00B77F90 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x007AF1F0 SYMBOL
// _$E59
double DAT_00B77F28 = fastmath::atan((DAT_00B77F90 * 2) / DAT_00B77EE8);

// SYNTHETIC: GAMEMD 0x007AF220 SYMBOL
// _$E62
int DAT_00B77F80 = (int)(DAT_00B77F90 * 4 + DAT_007E1738);

struct WDTShortPair { short a; short b; WDTShortPair(void) { a = 0; b = 0; } };
struct WDTIntTriple { int a; int b; int c; WDTIntTriple(void) { a = 0; b = 0; c = 0; } };

// SYNTHETIC: GAMEMD 0x007AF250 SYMBOL
// _$E65
WDTShortPair DAT_00B77EE0;

// SYNTHETIC: GAMEMD 0x007AF260 SYMBOL
// _$E68
WDTIntTriple DAT_00B77F10;

struct WDTCenter { int x; int y; int z; WDTCenter(void) { z = 0; x = 0x80; y = 0x80; } };

// SYNTHETIC: GAMEMD 0x007AF280 SYMBOL
// _$E71
WDTCenter DAT_00B77F48;

// FUNCTION: GAMEMD 0x007AF2A0
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

// SYNTHETIC: GAMEMD 0x007AF390 SYMBOL
// _$E77
// SYNTHETIC: GAMEMD 0x007AF3A0 SYMBOL
// _$E76
// SYNTHETIC: GAMEMD 0x007AF3B0 SYMBOL
// _$E75
// SYNTHETIC: GAMEMD 0x007AF3C0 SYMBOL
// _$E74

// FUNCTION: GAMEMD 0x007AF3D0
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

// SYNTHETIC: GAMEMD 0x007AF4C0 SYMBOL
// _$E82
// SYNTHETIC: GAMEMD 0x007AF4D0 SYMBOL
// _$E81
// SYNTHETIC: GAMEMD 0x007AF4E0 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x007AF4F0 SYMBOL
// _$E79

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

// FUNCTION: GAMEMD 0x007AF500
WorldDominationTour::FactionSelectDialogControl::~FactionSelectDialogControl(void)
{
	delete Image1;
	delete Image2;
	delete Image3;
}

// FUNCTION: GAMEMD 0x007AF540
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

// FUNCTION: GAMEMD 0x007AF640
void WorldDominationTour::FactionSelectDialogControl::Draw(int unused)
{
	if (AlternateSurface != NULL && Current != NULL) {
		Rect rect = Current->Get_Rect();
		AlternateSurface->Fill_Rect(rect, 0);
		AlternateSurface->Blit_From(rect, *Current, rect, false, true);
	}
}

GraphicMenu * __fastcall FUN_004F1B80(Wstring const & ini, Wstring const & name);

// FUNCTION: GAMEMD 0x007AFAD0
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


// SYNTHETIC: GAMEMD 0x007B0090 SYMBOL
// _$E86
// SYNTHETIC: GAMEMD 0x007B00B0 SYMBOL
// _$E84
WDTPointer<void> g_WDTUnusedPointer1;

// SYNTHETIC: GAMEMD 0x007B00D0 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x007B00F0 SYMBOL
// _$E89
WDTPointer<void> g_WDTUnusedPointer2;

// SYNTHETIC: GAMEMD 0x007B0110 SYMBOL
// _$E96
// SYNTHETIC: GAMEMD 0x007B0130 SYMBOL
// _$E94
rc_ptr<WDTState> DAT_00B77F58;

// SYNTHETIC: GAMEMD 0x007B0150 SYMBOL
// _$E101
// SYNTHETIC: GAMEMD 0x007B0170 SYMBOL
// _$E99
rc_ptr<WDTState> DAT_00B77F70;

// SYNTHETIC: GAMEMD 0x007B0190 SYMBOL
// _$E106
// SYNTHETIC: GAMEMD 0x007B01B0 SYMBOL
// _$E104
rc_ptr<WorldDominationTour::History> DAT_00B77F68;

// SYNTHETIC: GAMEMD 0x007B01D0 SYMBOL
// _$E111
// SYNTHETIC: GAMEMD 0x007B01F0 SYMBOL
// _$E109
rc_ptr<WorldDominationTour::History> DAT_00B77F78;

extern Wstring DAT_00B73E40;
rc_ptr<WorldDominationTour::CampaignProperties> __fastcall FUN_0076BE90(rc_ptr<WDTState> const & state);
rc_ptr<WorldDominationTour::History> __fastcall FUN_00764B30(Wstring const & name, rc_ptr<WorldDominationTour::CampaignProperties> const & props);

// FUNCTION: GAMEMD 0x007B0210
rc_ptr<WDTState> * __cdecl FUN_007B0210(void)
{
	return &DAT_00B77F58;
}

// FUNCTION: GAMEMD 0x007B0220
void __fastcall FUN_007B0220(rc_ptr<WDTState> const & state)
{
	DAT_00B77F58 = state;
	DAT_00B77F68 = FUN_00764B30(DAT_00B73E40, FUN_0076BE90(state));
}

// FUNCTION: GAMEMD 0x007B0290
rc_ptr<WDTState> * __cdecl FUN_007B0290(void)
{
	return &DAT_00B77F70;
}

// FUNCTION: GAMEMD 0x007B02A0
void __fastcall FUN_007B02A0(rc_ptr<WDTState> const & state)
{
	DAT_00B77F70 = state;
	DAT_00B77F78 = FUN_00764B30(DAT_00B73E40, FUN_0076BE90(state));
}

char * DAT_00B77F54;

// FUNCTION: GAMEMD 0x007B0310
char * __cdecl FUN_007B0310(void)
{
	return DAT_00B77F54;
}

// FUNCTION: GAMEMD 0x007B0320
void __fastcall FUN_007B0320(char * state)
{
	if (DAT_00B77F54 != NULL) {
		delete DAT_00B77F54;
	}
	DAT_00B77F54 = state;
}

char * DAT_00B77F60;

// FUNCTION: GAMEMD 0x007B0340
char * __cdecl FUN_007B0340(void)
{
	return DAT_00B77F60;
}

// FUNCTION: GAMEMD 0x007B0350
void __fastcall FUN_007B0350(char * state)
{
	if (DAT_00B77F60 != NULL) {
		delete DAT_00B77F60;
	}
	DAT_00B77F60 = state;
}

// FUNCTION: GAMEMD 0x007B0370
rc_ptr<WorldDominationTour::History> * __cdecl FUN_007B0370(void)
{
	return &DAT_00B77F68;
}

// FUNCTION: GAMEMD 0x007B0380
rc_ptr<WorldDominationTour::History> * __cdecl FUN_007B0380(void)
{
	return &DAT_00B77F78;
}

// FUNCTION: GAMEMD 0x007B0430
bool __cdecl FUN_007B0430(void)
{
	return true;
}

// SYNTHETIC: GAMEMD 0x007B0440
// ??_GFactionSelectDialogControl@WorldDominationTour@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x007B0490
WorldDominationTour::FactionSelectDialogControl::FactionSelectDialogControl(void)
	: FieldC(0), Image1(NULL), Image2(NULL), Image3(NULL)
{
	if (Image1 == NULL) {
		CCFileClass file("WDTFSBSm.pcx");
		Image1 = Read_PCX_File(file);
	}
	if (Image2 == NULL) {
		CCFileClass file("WDTFSAsm.pcx");
		Image2 = Read_PCX_File(file);
	}
	if (Image3 == NULL) {
		CCFileClass file("WDTFSSsm.pcx");
		Image3 = Read_PCX_File(file);
	}
	Current = Image1;
}

int DAT_00B77FA0, DAT_00B77FA4, DAT_00B77FA8, DAT_00B77FAC;

// FUNCTION: GAMEMD 0x007B05A0
void __cdecl FUN_007B05A0(void)
{
	DAT_00B77FA0 = 0;
	DAT_00B77FA4 = 0;
	DAT_00B77FA8 = 0;
	DAT_00B77FAC = 0;
}
