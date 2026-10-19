// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "worlddom.h"

#include "_mixfile.h"
#include "_pk.h"
#include "addon.h"
#include "grphmenu.h"
#include "language\language.h"
#include "mapgen.h"
#include "mixfile.h"
#include "ownrdraw.h"
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

namespace OwnerDraw {
	class DialogControl {
	public:
		virtual ~DialogControl(void);
		virtual int F1(void) = 0;
		virtual int F2(void) = 0;
		virtual int F3(void) = 0;
	};
	class SimpleDialogControl : public DialogControl {
	public:
		virtual ~SimpleDialogControl(void);
		int Field4;
		char Field8;
	};
}

namespace WorldDominationTour {
	struct FactionImage { virtual ~FactionImage(void); };
	class FactionSelectDialogControl : public OwnerDraw::SimpleDialogControl {
	public:
		virtual ~FactionSelectDialogControl(void);
		int FieldC;
		FactionImage * Image1;
		FactionImage * Image2;
		FactionImage * Image3;
	};
}

// FUNCTION: 0x007AF500
WorldDominationTour::FactionSelectDialogControl::~FactionSelectDialogControl(void)
{
	delete Image1;
	delete Image2;
	delete Image3;
}
