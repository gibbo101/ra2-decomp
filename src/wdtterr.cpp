// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F7320;
extern const double DAT_007F7328;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0076F720 SYMBOL
// _$E2
double DAT_00B72B30 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0076F750 SYMBOL
// _$E5
double DAT_00B72B80 = DAT_00B72B30 / DAT_007F7320;

// SYNTHETIC: GAMEMD 0x0076F770 SYMBOL
// _$E8
double DAT_00B72B68 = DAT_007E1718 / DAT_00B72B80;

// SYNTHETIC: GAMEMD 0x0076F790 SYMBOL
// _$E11
double DAT_00B72B60 = DAT_007E1720 * DAT_007F7328;

// SYNTHETIC: GAMEMD 0x0076F7B0 SYMBOL
// _$E14
double DAT_00B72B38 = DAT_007E1728 * DAT_007F7328;

// SYNTHETIC: GAMEMD 0x0076F7D0 SYMBOL
// _$E17
double DAT_00B72B10 = DAT_007E1730 * DAT_007F7328;

// SYNTHETIC: GAMEMD 0x0076F7F0 SYMBOL
// _$E20
int DAT_00B72B88 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72B10 - DAT_00B72B38) * DAT_00B72B30));

// SYNTHETIC: GAMEMD 0x0076F830 SYMBOL
// _$E23
double DAT_00B72B40 = FUN_004CADE0(DAT_00B72B88 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0076F860 SYMBOL
// _$E26
double DAT_00B72B58 = FUN_004CADE0((DAT_00B72B88 * 2) / DAT_00B72B30);

// SYNTHETIC: GAMEMD 0x0076F890 SYMBOL
// _$E29
int DAT_00B72B7C = (int)(DAT_00B72B88 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0076F8C0 SYMBOL
// _$E32
ShortPair DAT_00B72B28;

// SYNTHETIC: GAMEMD 0x0076F8D0 SYMBOL
// _$E35
IntTriple DAT_00B72B48;

struct Triple2 {
	int A, B, C;
	Triple2() { C = 0; A = 128; B = 128; }
};

// SYNTHETIC: GAMEMD 0x0076F8F0 SYMBOL
// _$E38
Triple2 DAT_00B72B70;

#include "always.h"

#include <stddef.h>

#include "ccfile.h"
#include "ini.h"
#include "stringtable.h"
#include "wdtnet.h"

using namespace WorldDominationTour;

struct Drawing {
	static int RenderWidth;
};

Wstring operator+(Wstring const & a, Wstring const & b);

// TEMPLATE: GAMEMD 0x0076F910
// ?FUN_0076F910@WorldDominationTour@@YI?AV?$rc_ptr@VTerritory@WorldDominationTour@@@@HABVINIClass@@ABVWstring@@11@Z
rc_ptr<Territory> __fastcall WorldDominationTour::FUN_0076F910(int id, INIClass const & ini, Wstring const & section, Wstring const & fileprefix, Wstring const & labelprefix)
{
	return rc_ptr<Territory>(new Territory(id, ini, section, fileprefix, labelprefix));
}

// FUNCTION: GAMEMD 0x0076F970
// WorldDominationTour::Territory::Territory
Territory::Territory(int id, INIClass const & ini, Wstring const & section, Wstring const & fileprefix, Wstring const & labelprefix) :
	ID(id),
	Target(0, 0),
	Origin(0, 0),
	OwnerAnim(NULL),
	HighlightAnim(NULL),
	TargetAnim(NULL)
{
	FileName = operator+(fileprefix, Wstring_FromInt(id, 2)) + ".shp";

	Wstring label = operator+(labelprefix, Wstring_FromInt(id, 2));
	Name = StringTable::LoadString(label.get(), NULL, "D:\\ra2mdpost\\WDTTerr.cpp", 0x27);

	Wstring width = Wstring_FromInt(Drawing::RenderWidth);
	Target = ini.Get_Point(section, "Target_" + width, Target);
	Origin = ini.Get_Point(section, "Origin_" + width, Origin);

	if (Drawing::RenderWidth > 800) {
		if (Target.X == 0 && Target.Y == 0) {
			Target = ini.Get_Point(section, "Target_800", Target);
		}
		if (Origin.X == 0 && Origin.Y == 0) {
			Origin = ini.Get_Point(section, "Origin_800", Origin);
		}
	}
}

// FUNCTION: GAMEMD 0x0076FB90
// WorldDominationTour::Territory::FUN_0076FB90
void Territory::FUN_0076FB90(Selection * engine, ConvertClass * drawer, Point2D const & pos)
{
	int x = Origin.X + pos.X;
	int y = Origin.Y + pos.Y;
	CCFileClass file(FileName);
	if (file.Is_Available(0)) {
		OwnerAnim = new MSOverlayAnim(FileName, x, y, drawer, 0, engine->Get_Anims(), true, -1, false);
		OwnerAnim->Set_Active(false);
		engine->Add_Animation(OwnerAnim);
	}
}

// FUNCTION: GAMEMD 0x0076FC50
// WorldDominationTour::Territory::FUN_0076FC50
void Territory::FUN_0076FC50(Selection * engine, ConvertClass * drawer, Point2D const & pos)
{
	int x = Origin.X + pos.X;
	int y = Origin.Y + pos.Y;
	CCFileClass file(FileName);
	if (file.Is_Available(0)) {
		HighlightAnim = new MSOverlayAnim(FileName, x, y, drawer, 0, engine->Get_Anims(), true, 0, false);
		HighlightAnim->Set_Active(false);
		engine->Add_Animation(HighlightAnim);
	}
}

// FUNCTION: GAMEMD 0x0076FD10
// WorldDominationTour::Territory::~Territory
Territory::~Territory(void)
{
}

// FUNCTION: GAMEMD 0x0076FD40
// WorldDominationTour::Territory::FUN_0076FD40
void Territory::FUN_0076FD40(int state, Selection * engine, bool wait)
{
	if (OwnerAnim != NULL) {
		int frame = -1;
		switch (state) {
			case 1:
				frame = 3;
				break;
			case 2:
				frame = 2;
				break;
			case 3:
				frame = 1;
				break;
		}
		Rect oldanimrect = OwnerAnim->Get_Rect();
		if (frame >= 0) {
			OwnerAnim->Set_Frame(frame);
			OwnerAnim->Set_Active(true);
			OwnerAnim->Restart();
		} else {
			OwnerAnim->Set_Active(false);
		}

		Rect newanimrect = OwnerAnim->Get_Rect();
		Rect rect = Union(newanimrect, oldanimrect);
		engine->Restore_Anims(rect);
		engine->Restore_And_Advance();
		engine->Wait_Delay(1);
	}
}

// FUNCTION: GAMEMD 0x0076FEB0
// WorldDominationTour::Territory::FUN_0076FEB0
void Territory::FUN_0076FEB0(bool active, Selection * engine)
{
	if (HighlightAnim != NULL) {
		HighlightAnim->Set_Active(active);
		Rect rect = HighlightAnim->Get_Rect();
		engine->Restore_Anims(rect);
		engine->Restore_And_Advance();
		engine->Wait_For_Anim(HighlightAnim);
	}
}

// FUNCTION: GAMEMD 0x0076FF20
// WorldDominationTour::Territory::FUN_0076FF20
void Territory::FUN_0076FF20(MSShapeAnim * anim)
{
	TargetAnim = anim;
}

// FUNCTION: GAMEMD 0x0076FF30
// WorldDominationTour::Territory::FUN_0076FF30
MSShapeAnim * Territory::FUN_0076FF30(void)
{
	MSShapeAnim * old = TargetAnim;
	TargetAnim = NULL;
	return(old);
}

// SYNTHETIC: GAMEMD 0x00770020
// ??_GTerritory@WorldDominationTour@@UAEPAXI@Z
