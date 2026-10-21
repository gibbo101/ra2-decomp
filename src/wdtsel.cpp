// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "language\language.h"

#include "_keyboar.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "data.h"
#include "globals.h"
#include "ini.h"
#include "keyboard.h"
#include "mschoice.h"
#include "msfont.h"
#include "newmenu.h"
#include "netshare.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "session.h"
#include "theme.h"
#include "vector.h"
#include "wdtnet.h"
#include "worlddom.h"

#define IID_DEFINED
#include "wolapi\wolapi.h"

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F72A0;
extern const double DAT_007F72A8;

// SYNTHETIC: GAMEMD 0x0076C0A0 SYMBOL
// _$E31
double DAT_00B72A30 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0076C0D0 SYMBOL
// _$E34
double DAT_00B72A80 = DAT_00B72A30 / DAT_007F72A0;

// SYNTHETIC: GAMEMD 0x0076C0F0 SYMBOL
// _$E37
double DAT_00B72A68 = DAT_007E1718 / DAT_00B72A80;

// SYNTHETIC: GAMEMD 0x0076C110 SYMBOL
// _$E40
double DAT_00B72A60 = DAT_007E1720 * DAT_007F72A8;

// SYNTHETIC: GAMEMD 0x0076C130 SYMBOL
// _$E43
double DAT_00B72A38 = DAT_007E1728 * DAT_007F72A8;

// SYNTHETIC: GAMEMD 0x0076C150 SYMBOL
// _$E46
double DAT_00B72A10 = DAT_007E1730 * DAT_007F72A8;

// SYNTHETIC: GAMEMD 0x0076C170 SYMBOL
// _$E49
int DAT_00B72A88 = (int)(fastmath::tan(DAT_00B72A10 - DAT_00B72A38) * DAT_007E1738 * DAT_00B72A30);

// SYNTHETIC: GAMEMD 0x0076C1B0 SYMBOL
// _$E52
double DAT_00B72A40 = fastmath::atan(DAT_00B72A88 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0076C1E0 SYMBOL
// _$E55
double DAT_00B72A58 = fastmath::atan((DAT_00B72A88 * 2) / DAT_00B72A30);

// SYNTHETIC: GAMEMD 0x0076C210 SYMBOL
// _$E58
int DAT_00B72A7C = (int)(DAT_00B72A88 * 4 + DAT_007E1738);

struct WDTShortPair { short a; short b; WDTShortPair(void) { a = 0; b = 0; } };
struct WDTIntTriple { int a; int b; int c; WDTIntTriple(void) { a = 0; b = 0; c = 0; } };

// SYNTHETIC: GAMEMD 0x0076C240 SYMBOL
// _$E61
WDTShortPair DAT_00B72A28;

// SYNTHETIC: GAMEMD 0x0076C250 SYMBOL
// _$E64
WDTIntTriple DAT_00B72A48;

struct WDTAnchor { int a; int b; int c; WDTAnchor(void) { c = 0; a = 0x80; b = 0x80; } };

// SYNTHETIC: GAMEMD 0x0076C270 SYMBOL
// _$E67
WDTAnchor DAT_00B72A70;

using namespace WorldDominationTour;

// FUNCTION: GAMEMD 0x0076C970
// WorldDominationTour::Selection::Init_Voices
void Selection::Init_Voices(INIClass const & ini, Wstring const & section)
{
	VoicePlayer.FUN_00770550(ini, section);
	{ Wstring key("WinningThreshold"); WinningThreshold = ini.Get_Int(section, key, WinningThreshold); }
	{ Wstring key("MegaWinningThreshold"); MegaWinningThreshold = ini.Get_Int(section, key, MegaWinningThreshold); }
	{ Wstring key("LosingThreshold"); LosingThreshold = ini.Get_Int(section, key, LosingThreshold); }
	{ Wstring key("MegaLosingThreshold"); MegaLosingThreshold = ini.Get_Int(section, key, MegaLosingThreshold); }
}

// FUNCTION: GAMEMD 0x0076CA60
// WorldDominationTour::Selection::Init_Sounds
void Selection::Init_Sounds(INIClass const & ini, Wstring const & section)
{
	Wstring buffer;

	int count = ini.Entry_Count(section);
	for (int i = 0; i < count; i++) {
		Wstring entry = ini.Get_Entry(section, i);
		if (entry != (char const *)NULL) {
			bool found;
			{
				Wstring none((char const *)NULL);
				found = ini.Get_String(section, entry, none, buffer) > 0;
			}
			if (found) {
				if (entry == "Theme") {
					ThemeName = buffer;
					break;
				}
				MSSfxEntry * sfx = new MSSfxEntry(entry, buffer);
				if (sfx->Get_Name() == (char const *)NULL) {
					delete sfx;
				} else {
					Add_Sfx_Entry(sfx);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0076CBF0
// WorldDominationTour::Selection::Init_Target
void Selection::Init_Target(INIClass const & ini, Wstring const & section)
{
	Wstring buffer;
	int faction = TourCampaign.Get()->PlayerFaction;
	Wstring faction_name = *FUN_007AF2A0(faction);

	bool found;
	{
		Wstring none((char const *)NULL);
		found = ini.Get_String(section, faction_name + "ZoomingTarget", none, buffer) > 0;
	}
	if (found) {
		ZoomingTarget = new MSAnimEntry(buffer);
	}

	{
		Wstring none((char const *)NULL);
		ini.Get_String(section, faction_name + "TargetPalette", none, buffer);
	}
	TargetDrawer = Create_Drawer(buffer);

	{
		Wstring none((char const *)NULL);
		ini.Get_String(section, faction_name + "Target", none, buffer);
	}
	ThrobbingTarget = new MSAnimEntry(buffer);

	ThrobbingTargetDividingFrame = ini.Get_Int(section, faction_name + "TargetDividingFrame", 32);
}

// FUNCTION: GAMEMD 0x0076D470
// WorldDominationTour::Selection::Update_Hovered_Territory
void Selection::Update_Hovered_Territory(Point2D const & point)
{
	int id = WorldMap.Get()->FUN_00769330(point);
	if (id != HoveredTerritoryID) {
		HoveredTerritoryID = id;
		Select_Territory(WorldMap.Get()->FUN_0076F000(id));
	}
}

extern bool UserInputLocked;

class InputManagerClass {
public:
	static InputManagerClass * Instance;
	short FUN_0054F000(void);
	short FUN_0054F050(void);
	void FUN_0054F720(void);
};

typedef VectorCursor<MSSfxEntry *, DynamicVectorClass<MSSfxEntry *> > WDT_SFX_ITER;

static inline void Play_Sfx_Named(Selection * sel, char const * text)
{
	Wstring name(text);
	if (name != (char const *)NULL) {
		WDT_SFX_ITER it(sel->SfxEntries, 0);
		for (; it; it++) {
			if (!(name == it->Get_Name())) {
				break;
			}
		}
		if (it) {
			it->Play();
		}
	}
}

// FUNCTION: GAMEMD 0x0076DFD0
// WorldDominationTour::Selection::On_Map_Enter
void Selection::On_Map_Enter(void)
{
	Play_Sfx_Named(this, "MapEnter");
}

// FUNCTION: GAMEMD 0x0076E090
// WorldDominationTour::Selection::On_Map_Leave
void Selection::On_Map_Leave(void)
{
	Play_Sfx_Named(this, "MapLeave");
	HoveredTerritoryID = -1;
	rc_ptr<Territory> none;
	Select_Territory(none);
}

// FUNCTION: GAMEMD 0x0076E9D0
// WorldDominationTour::Selection::Check_For_Breakout
void Selection::Check_For_Breakout(void)
{
	Wait_For_Focus();
	while (InputManagerClass::Instance->FUN_0054F000()) {
		if (InputManagerClass::Instance->FUN_0054F050() == VK_ESCAPE) {
			Brokeout = true;
		}
	}
}

// FUNCTION: GAMEMD 0x0076EAF0
// WorldDominationTour::Selection::Set_Tooltip
void Selection::Set_Tooltip(WideWstring const & text)
{
	Rect rect = WorldMap.Get()->TooltipRect;
	if (TooltipAnim != NULL) {
		Remove_Anim(TooltipAnim);
		HiddenSurface->Blit_From(rect, *AlternateSurface, rect);
		Add_Update_Rect(rect);
		TooltipAnim = NULL;
	}

	if (text.length() > 0) {
		WideWstring copy(text);
		TooltipAnim = new YRTextAnim(copy, rect, false);
		TooltipAnim->FUN_005CD3C0(true);
		Add_Animation(TooltipAnim);
	}
}

// FUNCTION: GAMEMD 0x0076EBE0
// WorldDominationTour::Selection::Play_SFX
void Selection::Play_SFX(Wstring const & name)
{
	if (name != (char const *)NULL) {
		SFX_ITER it(SfxEntries, 0);
		for (; it; it++) {
			if (!(name == it->Get_Name())) {
				break;
			}
		}
		if (it) {
			it->Play();
		}
	}
}

// FUNCTION: GAMEMD 0x0076ECD0
void __fastcall FUN_0076ECD0(rc_ptr<Campaign> * campaign, int theater)
{
	InputManagerClass::Instance->FUN_0054F720();
	Selection sel(*campaign, theater);
	sel.Presentation(theater);
}

// FUNCTION: GAMEMD 0x0076EEB0
// WorldDominationTour::Selection::FUN_0076EEB0
void Selection::FUN_0076EEB0(void)
{
	UserInputLocked = false;
	if (ThemeName != (char const *)NULL) {
		Theme.Play_Song(Theme.From_Name(ThemeName.get()));
		Theme.Set_Repeat(true);
	}
	MouseCursor->Hide_Mouse();
	AlternateSurface->Fill(0);
	HiddenSurface->Fill(0);
	VisibleSurface->Blit_From(*HiddenSurface);
	InputManagerClass::Instance->FUN_0054F720();
	WorldMap.Get()->FUN_007692F0(this, &VoicePlayer);
	while (MouseCursor->Get_Mouse_State() < 0) {
		MouseCursor->Show_Mouse();
	}
}

// FUNCTION: GAMEMD 0x0076EF80
// WorldDominationTour::Selection::End
void Selection::End(void)
{
	MouseCursor->Hide_Mouse();
	Theme.Queue_Song((ThemeType)-3);
	AlternateSurface->Fill(0);
	HiddenSurface->Fill(0);
	while (MouseCursor->Get_Mouse_State() < 0) {
		MouseCursor->Show_Mouse();
	}
}

// TEMPLATE: GAMEMD 0x0076F000
// ?FUN_0076F000@Map@WorldDominationTour@@QAE?AV?$rc_ptr@VTerritory@WorldDominationTour@@@@H@Z
rc_ptr<Territory> Map::FUN_0076F000(int id)
{
	return(FUN_0076FF40(&Territories, id));
}

// TEMPLATE: GAMEMD 0x0076F020
// ?Get@?$rc_ptr@VCampaignProperties@WorldDominationTour@@@@QBEPAVCampaignProperties@WorldDominationTour@@XZ
template class rc_ptr<CampaignProperties>;

// TEMPLATE: GAMEMD 0x0076F030
// ??0?$rc_ptr@VTerritory@WorldDominationTour@@@@QAE@ABV0@@Z
// TEMPLATE: GAMEMD 0x0076F060
// ??1?$rc_ptr@VTerritory@WorldDominationTour@@@@UAE@XZ
// TEMPLATE: GAMEMD 0x0076F070
// ?Get@?$rc_ptr@VTerritory@WorldDominationTour@@@@QBEPAVTerritory@WorldDominationTour@@XZ
template class rc_ptr<Territory>;

// TEMPLATE: GAMEMD 0x0076F080
// ?Get@?$rc_ptr@VMap@WorldDominationTour@@@@QBEPAVMap@WorldDominationTour@@XZ
template class rc_ptr<Map>;

// TEMPLATE: GAMEMD 0x0076F090
// ??0?$VectorCursor@V?$TRect@H@@V?$DynamicVectorClass@V?$TRect@H@@@@@@QAE@AAV?$DynamicVectorClass@V?$TRect@H@@@@H@Z
// TEMPLATE: GAMEMD 0x0076F0B0
// ??B?$VectorCursor@V?$TRect@H@@V?$DynamicVectorClass@V?$TRect@H@@@@@@UBE_NXZ
// TEMPLATE: GAMEMD 0x0076F0C0
// ??0?$VectorCursor@VPoint2D@@V?$DynamicVectorClass@VPoint2D@@@@@@QAE@AAV?$DynamicVectorClass@VPoint2D@@@@H@Z
// TEMPLATE: GAMEMD 0x0076F0E0
// ??B?$VectorCursor@VPoint2D@@V?$DynamicVectorClass@VPoint2D@@@@@@UBE_NXZ
// TEMPLATE: GAMEMD 0x0076F0F0
// ??B?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UBE_NXZ
// SYNTHETIC: GAMEMD 0x0076F100
// ??_G?$VectorCursor@V?$TRect@H@@V?$DynamicVectorClass@V?$TRect@H@@@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0076F120
// ??_G?$VectorCursor@VPoint2D@@V?$DynamicVectorClass@VPoint2D@@@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0076F140
// ??_G?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0076F160
// ??E?$VectorCursor@V?$TRect@H@@V?$DynamicVectorClass@V?$TRect@H@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F170
// ??F?$VectorCursor@V?$TRect@H@@V?$DynamicVectorClass@V?$TRect@H@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F180
// ??E?$VectorCursor@VPoint2D@@V?$DynamicVectorClass@VPoint2D@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F190
// ??F?$VectorCursor@VPoint2D@@V?$DynamicVectorClass@VPoint2D@@@@@@UAEAAV0@XZ
template class VectorCursor<Rect, DynamicVectorClass<Rect> >;
template class VectorCursor<Point2D, DynamicVectorClass<Point2D> >;
// TEMPLATE: GAMEMD 0x0076F1A0
// ??E?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F1B0
// ??F?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F1C0
// ??0?$VectorClass@PAVMSAnim@@@@QAE@HPBQAVMSAnim@@@Z
// TEMPLATE: GAMEMD 0x0076F210
// ??0?$VectorClass@PAVMSSfxEntry@@@@QAE@HPBQAVMSSfxEntry@@@Z
template class VectorClass<MSAnim *>;
template class VectorClass<MSSfxEntry *>;
template class VectorCursor<MSSfxEntry *, DynamicVectorClass<MSSfxEntry *> >;

int DAT_00B72A98;
int DAT_00B72A9C;
int DAT_00B72AA0;
int DAT_00B72AA4;

// FUNCTION: GAMEMD 0x0076F260
void FUN_0076F260(void)
{
	DAT_00B72A98 = 0;
	DAT_00B72A9C = 0;
	DAT_00B72AA0 = 0;
	DAT_00B72AA4 = 0;
}
