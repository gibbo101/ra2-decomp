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
#include "geometry.h"
#include "ownrdraw.h"
#include "tiprequest.h"
#include "pcx.h"
#include "rcptr.h"
#include "wdtnet.h"
#include "wonline.h"
#include "wstring.h"

#include <cmath>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
// SYNTHETIC: GAMEMD 0x007AF090 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x007AF0B0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x007AF0E0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x007AF100 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x007AF120 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x007AF140 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x007AF160 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x007AF180 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x007AF1C0 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x007AF1F0 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x007AF220 SYMBOL
// _$E62
// SYNTHETIC: GAMEMD 0x007AF250 SYMBOL
// _$E65
// SYNTHETIC: GAMEMD 0x007AF260 SYMBOL
// _$E68
// SYNTHETIC: GAMEMD 0x007AF280 SYMBOL
// _$E71

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

void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
int __fastcall Run_Dialog_Control(int resource, DLGPROC proc, OwnerDraw::DialogControl * control);
#include <stddef.h>
#include "stringtable.h"

void FUN_00607FD0(HWND dialog);

// FUNCTION: GAMEMD 0x007AF6C0
BOOL CALLBACK FUN_007AF6C0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam != 0) {
		SetWindowLong(window, DWL_USER, ((LONG *)lparam)[1]);
	}

	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		WorldDominationTour::FactionSelectDialogControl * control = (WorldDominationTour::FactionSelectDialogControl *)GetWindowLong(window, DWL_USER);

		switch (message) {
			case WM_NCHITTEST: {
				POINT point;
				point.x = LOWORD(lparam);
				point.y = HIWORD(lparam);
				control->Update_Hover(window, &point);
				return FALSE;
			}

			case WM_DRAWITEM:
				OwnerDraw::Draw_Item((LPDRAWITEMSTRUCT)lparam);
				return TRUE;

			case WM_PAINT:
				OwnerDraw::Draw_Dialog_Back(window);
				control->Draw(0);
				FUN_00607FD0(window);
				ValidateRect(window, NULL);
				return TRUE;

			case WM_ERASEBKGND:
				return TRUE;

			case WM_COMMAND:
				if (LOWORD(wparam) == 0x686) {
					control->F1(window, -1);
				}
				return FALSE;

			case 0x497:
				return FALSE;

			case 0x4E9: {
				TooltipRequest * request = (TooltipRequest *)lparam;
				if (request->Window == window) {
					wchar_t const * text;
					switch (control->FieldC) {
						case 2:
							text = StringTable::LoadString("WDT:FactionSelectAlliesToolTip", NULL, "D:\\ra2mdpost\\WorldDom.cpp", 0xb0);
							break;
						case 3:
							text = StringTable::LoadString("WDT:FactionSelectSovietToolTip", NULL, "D:\\ra2mdpost\\WorldDom.cpp", 0xb3);
							break;
						default:
							text = StringTable::LoadString("WDT:FactionSelectToolTip", NULL, "D:\\ra2mdpost\\WorldDom.cpp", 0xb6);
							break;
					}
					WideWstring tip(text);
					request->Text = tip;
				}
				return FALSE;
			}

			case WM_LBUTTONUP:
				if (control->FieldC != 0) {
					control->F1(window, 1);
				}
				return FALSE;
		}
		return FALSE;
	}
	return rc;
}

static inline void Load_Image(WorldDominationTour::FactionImage *& image, char const * name)
{
	CCFileClass file(name);
	image = Read_PCX_File(file);
}

// FUNCTION: GAMEMD 0x007AF970
int FUN_007AF970(void)
{
	FUN_0052FEC0(NULL, true);
	WorldDominationTour::FactionSelectDialogControl control;
	Run_Dialog_Control(0x125, FUN_007AF6C0, &control);
	return control.FieldC;
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


#include "goptions.h"

namespace Drawing { extern int RenderWidth; }
int ODMessageBox(const wchar_t * text, int type, bool (*callback)(void), bool large);
bool FUN_0077F900(void);
bool Test_Display_Mode_Dialog(int width, int height);
bool FUN_00797800(void);
int FUN_0079FC40(void);
void Play_Movie(char const * name, ThemeType theme, bool clrscrn_after, bool stretch, bool clrscrn_before, bool fullscreen);
extern int DAT_00B77F94;
extern int DAT_00B77F98;
extern int DAT_00B77DC4;
rc_ptr<WorldDominationTour::Campaign> * __cdecl FUN_007B0370(void);
rc_ptr<WorldDominationTour::Campaign> * __cdecl FUN_007B0380(void);

// FUNCTION: GAMEMD 0x007AFB90
int __fastcall FUN_007AFB90(bool first_time)
{
	using namespace WorldDominationTour;

	if (Drawing::RenderWidth > 640) {
		WideWstring query(StringTable::LoadString("WOL:WDTResolutionQuery", NULL, "D:\\ra2mdpost\\WorldDom.cpp", 0x181));
		if (ODMessageBox(query.get(), 4, FUN_0077F900, true) != 6) {
			return 0;
		}
		if (!Test_Display_Mode_Dialog(640, 480)) {
			return 0;
		}
		Options.ScreenWidth = 640;
		Options.ScreenHeight = 480;
		Options.Save_Settings();
	}

	MFCD * wdtmix = NULL;
	MFCD * wdtvoxmix = NULL;
	MFCD * localmix = NULL;

	if (CCFileClass("WDT.MIX").Is_Available()) {
		wdtmix = new MFCD("WDT.MIX", &FastKey);
	}
	if (CCFileClass("WDTVOX.MIX").Is_Available()) {
		wdtvoxmix = new MFCD("WDTVOX.MIX", &FastKey);
	}
	if (CCFileClass("Local.MIX").Is_Available()) {
		localmix = new MFCD("Local.MIX", &FastKey);
	}

	if (wdtmix != NULL && localmix != NULL) {
		int state = first_time ? 4 : 1;
		while (state != 0 && state != 6) {
			switch (state) {
				case 1: {
					int side = FUN_007AF970();
					DAT_00B77F94 = side;
					state = side ? 4 : 0;
					break;
				}

				case 2: {
					char name[128];
					sprintf(name, "WDTTheater%02.VQA", DAT_00B77F94);
					if (CCFileClass(name).Is_Available()) {
						Play_Movie(name, THEME_NONE, false, true, false, false);
					}
					state = 3;
					break;
				}

				case 3:
					DAT_00B77F98 = WDT_Theater_Choice_Menu(DAT_00B77F94);
					state = DAT_00B77F98 ? 4 : 1;
					break;

				case 4:
					if (!FUN_00797800()) {
						state = 0;
					} else {
						rc_ptr<Campaign> campaign = *FUN_007B0370();
						if (campaign.Get() == NULL) {
							return 0;
						}
						campaign.Get()->Set_Faction(DAT_00B77F94);
						if (campaign.Get()->Is_Different_Cycle()) {
							rc_ptr<Campaign> rcampaign = *FUN_007B0380();
							if (rcampaign.Get() != NULL && !rcampaign.Get()->Is_Different_Cycle()) {
								FUN_0076ECD0(&rcampaign, DAT_00B77F98);
							}
						}
						Session.House = DAT_00B77F94 == 3;
						if (FUN_0076ED10(&campaign, DAT_00B77F98)) {
							DAT_00B77DC4 = 0;
							RandomMapGen.SeedData.Fixup_WDT_Settings();
							state = 5;
						} else {
							state = 1;
						}
					}
					break;

				case 5:
					if (FUN_0079FC40() == 1) {
						state = 6;
					} else {
						state = 4;
					}
					break;
			}
		}

		delete wdtmix;
		delete localmix;
		delete wdtvoxmix;

		return state == 6;
	}

	delete wdtmix;
	delete localmix;
	delete wdtvoxmix;

	return 0;
}

// SYNTHETIC: GAMEMD 0x007B0090 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x007B00B0 SYMBOL
// _$E89
WDTPointer<void> g_WDTUnusedPointer1;

// SYNTHETIC: GAMEMD 0x007B00D0 SYMBOL
// _$E96
// SYNTHETIC: GAMEMD 0x007B00F0 SYMBOL
// _$E94
WDTPointer<void> g_WDTUnusedPointer2;

// SYNTHETIC: GAMEMD 0x007B0110 SYMBOL
// _$E101
// SYNTHETIC: GAMEMD 0x007B0130 SYMBOL
// _$E99
rc_ptr<WDTState> DAT_00B77F58;

// SYNTHETIC: GAMEMD 0x007B0150 SYMBOL
// _$E106
// SYNTHETIC: GAMEMD 0x007B0170 SYMBOL
// _$E104
rc_ptr<WDTState> DAT_00B77F70;

// SYNTHETIC: GAMEMD 0x007B0190 SYMBOL
// _$E111
// SYNTHETIC: GAMEMD 0x007B01B0 SYMBOL
// _$E109
rc_ptr<WorldDominationTour::Campaign> DAT_00B77F68;

// SYNTHETIC: GAMEMD 0x007B01D0 SYMBOL
// _$E116
// SYNTHETIC: GAMEMD 0x007B01F0 SYMBOL
// _$E114
rc_ptr<WorldDominationTour::Campaign> DAT_00B77F78;

extern Wstring DAT_00B73E40;
rc_ptr<WorldDominationTour::CampaignProperties> __fastcall FUN_0076BE90(rc_ptr<WDTState> const & state);
rc_ptr<WorldDominationTour::Campaign> __fastcall FUN_00764B30(Wstring const & name, rc_ptr<WorldDominationTour::CampaignProperties> const & props);

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
rc_ptr<WorldDominationTour::Campaign> * __cdecl FUN_007B0370(void)
{
	return &DAT_00B77F68;
}

// FUNCTION: GAMEMD 0x007B0380
rc_ptr<WorldDominationTour::Campaign> * __cdecl FUN_007B0380(void)
{
	return &DAT_00B77F78;
}

// FUNCTION: GAMEMD 0x007B0390
rc_ptr<WDTTerritory> __fastcall FUN_007B0390(int id)
{
	WorldDominationTour::CONFLICT_ITER it = (*FUN_007B0370())->Get_Conflict_Iterator(id);
	return(it ? (*it)->Territory : rc_ptr<WDTTerritory>());
}

// FUNCTION: GAMEMD 0x007B0430
bool __cdecl FUN_007B0430(void)
{
	return true;
}

// SYNTHETIC: GAMEMD 0x007B0440
// ??_GFactionSelectDialogControl@WorldDominationTour@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x007B0490
inline WorldDominationTour::FactionSelectDialogControl::FactionSelectDialogControl(void)
	: FieldC(0), Image1(NULL), Image2(NULL), Image3(NULL)
{
	if (Image1 == NULL) {
		Load_Image(Image1, "WDTFSBSm.pcx");
	}
	if (Image2 == NULL) {
		Load_Image(Image2, "WDTFSAsm.pcx");
	}
	if (Image3 == NULL) {
		Load_Image(Image3, "WDTFSSsm.pcx");
	}
	Current = Image1;
}
