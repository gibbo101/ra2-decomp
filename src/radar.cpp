// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "radar.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "bsurface.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "dbgprint.h"
#include "dialog.h"
#include "draw.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "language\language.h"
#include "lightcon.h"
#include "mixfile.h"
#include "movies.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "shapebtn.h"
#include "tactical.h"
#include "voc.h"
#include "vox.h"
#include "vqa.h"
#include "fastmath.h"
#include "coord.h"

extern const double ONE_RAD;
extern const double DAT_007F02A8;
extern const double DAT_00B048D8;
extern const double DAT_00B049F8;
extern const double DAT_00B049E0;
extern const double DAT_00B049D8;
extern const double DAT_00B048E8;
extern const double DAT_00B048A0;
extern const int DAT_00B04A34;
extern const double DAT_00B048F0;
extern const double DAT_00B04970;
extern const int DAT_00B049F4;
extern int DAT_00B04A30;
int __fastcall FUN_00753250(char const * name);

// FUNCTION: 0x00652680 $E109
const double DAT_00B048D8 = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);

// FUNCTION: 0x006526B0 $E112
const double DAT_00B049F8 = DAT_00B048D8 / DAT_007F02A8;

// FUNCTION: 0x006526D0 $E115
const double DAT_00B049E0 = 1.0 / DAT_00B049F8;

// FUNCTION: 0x006526F0 $E118
const double DAT_00B049D8 = ONE_RAD * 45;

// FUNCTION: 0x00652710 $E121
const double DAT_00B048E8 = ONE_RAD * 60;

// FUNCTION: 0x00652730 $E124
const double DAT_00B048A0 = ONE_RAD * 90;

// FUNCTION: 0x00652750 $E127
const int DAT_00B04A34 = (int)(fastmath::tan(DAT_00B048A0 - DAT_00B048E8) * DAT_00B048D8 * 0.5);

// FUNCTION: 0x00652790 $E130
const double DAT_00B048F0 = fastmath::atan(DAT_00B04A34 * (1 / (double)CELL_LEPTON_H));

// FUNCTION: 0x006527C0 $E133
const double DAT_00B04970 = fastmath::atan((DAT_00B04A34 * 2) / DAT_00B048D8);

// FUNCTION: 0x006527F0 $E136
const int DAT_00B049F4 = (int)((DAT_00B04A34 * BRIDGE_CELL_HEIGHT) + 0.5);

// FUNCTION: 0x00652820 $E139
Cell const DAT_00B048C0(0, 0);

// FUNCTION: 0x00652830 $E142
Coord const DAT_00B04900(0, 0, 0);

// FUNCTION: 0x00652850 $E145
Coord const DAT_00B049E8(128, 128, 0);

// FUNCTION: 0x00652870 $E151
// FUNCTION: 0x006528A0 $E148
RadarClass::RTacticalClass RadarClass::RadarButton;

struct RadarStatics {
	static ShapeButtonClass DAT_00B04978;
	static ShapeButtonClass DAT_00B04910;
};

// FUNCTION: 0x006528C0 $E156
Rect DAT_00B04A00;

// FUNCTION: 0x006528D0 $E161
// FUNCTION: 0x006528F0 $E159
ShapeButtonClass RadarStatics::DAT_00B04978;

// FUNCTION: 0x00652910 $E164
Rect DAT_00B04A20;

// FUNCTION: 0x00652920 $E169
// FUNCTION: 0x00652940 $E167
ShapeButtonClass RadarStatics::DAT_00B04910;

// FUNCTION: 0x00652960
RadarClass::RadarClass(void) :
	RadY(0),
	RadWidth(0),
	RadHeight(0),
	RadOffX(0),
	RadOffY(0),
	RadIWidth(0),
	RadIHeight(0),
	RadPWidth(0),
	LastDrawRect(RECT_NONE),
	RadarSurface(0),
	BackgroundSurface(0),
	BackgroundColors(NULL),
	RadarCellWidth(0),
	RadarCellHeight(0),
	CellRedrawRect(0,0,0,0),
	RadarTrackingTable(0),
	PixelFlags(0),
	ZoomFactor(0),
	RadarScale(1),
	RadarX(0),
	field_149C(0),
	RadarY(0),
	RadarRect(RECT_NONE),
	RadarState(0),
	RadarMode(0),
	SuspendedRadarMode(0),
	unknown_bool_14BC(false),
	unknown_bool_14BD(false),
	unknown_int_14D4(-1),
	DoesRadarExist(false),
	IsToRedraw(0),
	FullRedraw(0),
	RadarViewRect(RECT_NONE),
	RadarAnimFrame(0)
{
	Init_Radar();
	PixelStack.Set_Growth_Step(500);
	unknown_14C0.FUN_00405BE0();
}

// FUNCTION: 0x00652C00
RadarClass::~RadarClass(void)
{
	unknown_14C0.FUN_00405C00();
	Clear_Radar();
}

// FUNCTION: 0x00652CD0
void RadarClass::Queue_Next_Movie(void)
{
	RadarState = RSTATE_NEXT_MOVIE;
	RadarAnimFrame = RADAR_ACTIVATED_FRAME;
}

#include "multiplayer.h"
#include "session.h"
#include "tooltip.h"
#undef LoadString
#include "stringtable.h"

extern int DAT_007F5BF8;
extern int DAT_00B0FA68;
extern char const * DAT_008391F4;
extern char const * DAT_008391F8;
extern void * DAT_00B048E0;
extern void * DAT_00B048AC;
extern bool DAT_00B048F9;
extern bool DAT_00B048F8;
extern Rect DAT_00B048C8;
extern int DAT_00B04A1C;
extern int DAT_00B04A38;

namespace Game { extern int SpecialDialog; }
extern bool DAT_00A8EB7C;
extern int DAT_00886FA8;
extern int DAT_00B45B68;
bool __fastcall FUN_0053BAE0(void);
void __fastcall FUN_00753580(void);

extern DSurface * DAT_00887300;
ConvertClass * __fastcall FUN_0072F510(void);

struct ShpHeader {
	short Zero;
	short Width;
	short Height;
};

struct FileSystem {
	static void * __fastcall LoadWholeFileEx(char const * name, bool & allocated);
	static ConvertClass * SIDEBAR_PAL;
};

struct Drawing {
	static unsigned char RedShiftRight;
	static unsigned char GreenShiftRight;
	static unsigned char BlueShiftRight;
	static int RedShiftLeft;
	static int GreenShiftLeft;
	static int BlueShiftLeft;
};

struct TextColor {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

struct CCToolTip {
	static TextColor ToolTipTextColor;
	static ToolTipManager * Instance;
};

// FUNCTION: 0x00652CF0
void RadarClass::One_Time(void)
{
	DebugString("RadarClass::One_Time()\n");
	RadX = 0;
	RadY = 16;
	RadWidth = 48;
	RadIHeight = 140;
	RadPWidth = 108;
	RadOffY = 140;
	RadIWidth = 108;
	RadOffX = 49;

	BASECLASS::One_Time();

	DAT_00B04A20.X = RadOffX;
	DAT_00B04A20.Y = RadIHeight;
	DAT_00B04A20.Width = RadPWidth;
	DAT_00B04A30 = 223;
	unknown_int_14D4 = FUN_00753250("EVA_IncomingTransmission");
}

// FUNCTION: 0x00652D90
void RadarClass::DisposeOfArt(void)
{
	if (DAT_00B048F9) {
		delete (char *)DAT_00B048E0;
		DAT_00B048F9 = false;
	}
	DAT_00B048E0 = NULL;
	if (DAT_00B048F8) {
		delete (char *)DAT_00B048AC;
		DAT_00B048F8 = false;
	}
	DAT_00B048AC = NULL;
}

// FUNCTION: 0x00652DE0
void RadarClass::Init_Clear(void)
{
	BASECLASS::Init_Clear();
	DebugString("RadarClass::Init_Clear\n");

	RadarState = RSTATE_INACTIVE;
	RadarMode = RMODE_UNAVAILABLE;
	unknown_bool_14BD = false;
	unknown_bool_14BC = false;
	RadarAnimFrame = 0;
	DoesRadarExist = false;
	IsToRedraw = true;
	BackgroundStack.Clear();
	PixelStack.Clear();
	DebugString("RadarClass::Init_Clear done\n");
}

// FUNCTION: 0x00652E50
void RadarClass::Set_Buttons(bool on)
{
	if (on) {
		Add_A_Button(RadarStatics::DAT_00B04978);
		Add_A_Button(RadarStatics::DAT_00B04910);
	} else {
		Remove_A_Button(RadarStatics::DAT_00B04978);
		Remove_A_Button(RadarStatics::DAT_00B04910);
	}
}

inline int Pack_Color(TextColor const & c)
{
	return (c.R >> Drawing::RedShiftRight << Drawing::RedShiftLeft)
		| (c.G >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft)
		| (c.B >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft);
}

// FUNCTION: 0x00652E90
void RadarClass::Init_For_House(void)
{
	DebugString("RadarClass::Init_For_House()\n");
	DisposeOfArt();

	int side = *(int *)((char *)Scen + 0x34b8);
	if (side == 0) {
		RadHeight = (DAT_007F5BF8 - 144) / 2 + 4;
		DAT_00B04A00.X = RadX + 11;
		DAT_00B04A00.Y = RadY + 4;
		DAT_00B048C8.X = RadX + 83;
	} else if (side == 1) {
		RadHeight = (DAT_007F5BF8 - 145) / 2 + 5;
		DAT_00B04A00.X = RadX + 14;
		DAT_00B04A00.Y = RadY + 5;
		DAT_00B048C8.X = RadX + 86;
	} else {
		RadHeight = (DAT_007F5BF8 - 145) / 2 + 5;
		DAT_00B04A00.X = RadX + 14;
		DAT_00B04A00.Y = RadY + 5;
		DAT_00B048C8.X = RadX + 86;
	}
	DAT_00B048C8.Y = DAT_00B04A00.Y;
	DAT_00B04A1C = RadHeight;

	DAT_00B048E0 = FileSystem::LoadWholeFileEx(DAT_008391F4, DAT_00B048F9);
	DAT_00B048AC = FileSystem::LoadWholeFileEx(DAT_008391F8, DAT_00B048F8);
	DAT_00B04A00.Width = ((ShpHeader *)DAT_00B048E0)->Width;
	DAT_00B04A00.Height = ((ShpHeader *)DAT_00B048E0)->Height;
	DAT_00B048C8.Width = ((ShpHeader *)DAT_00B048AC)->Width;
	DAT_00B048C8.Height = ((ShpHeader *)DAT_00B048AC)->Height;
	DAT_00B04A38 = DAT_00B0FA68;

	RadPHeight = Pack_Color(CCToolTip::ToolTipTextColor);
}

// FUNCTION: 0x00653010
void RadarClass::Init_IO(void)
{
	BASECLASS::Init_IO();

	if (!Debug_Map) {
		RadarStatics::DAT_00B04978.ID = 242;
		RadarStatics::DAT_00B04978.DrawOnSidebar = true;
		RadarStatics::DAT_00B04978.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		RadarStatics::DAT_00B04978.IsOn = false;
		RadarStatics::DAT_00B04978.IsToggleType = 0;
		RadarStatics::DAT_00B04978.Set_Flags(5);
		RadarStatics::DAT_00B04978.Set_Position(DAT_00B04A00.X, DAT_00B04A00.Y);
		RadarStatics::DAT_00B04978.Set_Shape((ShapeSet const *)DAT_00B048E0, 0, 0);
		RadarStatics::DAT_00B04978.Flag_To_Redraw();

		RadarStatics::DAT_00B04910.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		RadarStatics::DAT_00B04910.ID = 243;
		RadarStatics::DAT_00B04910.DrawOnSidebar = true;
		RadarStatics::DAT_00B04910.IsOn = false;
		RadarStatics::DAT_00B04910.IsToggleType = 0;
		RadarStatics::DAT_00B04910.Set_Flags(5);
		RadarStatics::DAT_00B04910.Set_Position(DAT_00B048C8.X, DAT_00B048C8.Y);
		RadarStatics::DAT_00B04910.Set_Shape((ShapeSet const *)DAT_00B048AC, 0, 0);
		RadarStatics::DAT_00B04910.Flag_To_Redraw();
	}
}

// FUNCTION: 0x00653750
void RadarClass::Noop(int, int)
{
}

// FUNCTION: 0x00653760
Cell RadarClass::Click_Cell_Calc(Point2D const & point) const
{
	Point2D pixel = point;
	Coord coord = TacticalMap->Pixel_To_Coord(pixel);

	if (coord != COORD_NONE) {
		if (Debug_Map) {
			return(TacticalMap->Pixel_To_Cell(pixel));
		} else {
			return(TacticalMap->Pixel_To_Cell(pixel));
		}
	}

	return(CELL_NONE);
}

// FUNCTION: 0x00653810
bool RadarClass::Map_Cell(Cell const & cell, HouseClass * house)
{
	if (BASECLASS::Map_Cell(cell, house)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00653830
bool RadarClass::RevealFogShroud(Cell const & cell, HouseClass * house, bool increase)
{
	if (BASECLASS::RevealFogShroud(cell, house, increase)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00653850
void RadarClass::AI(KeyNumType & input, Point2D const & xy)
{
	BASECLASS::AI(input, xy);

	int offset = DAT_00A8EB7C ? DAT_00886FA8 : 0;

	Rect r1 = DAT_00B04A00;
	r1.X += offset;
	if (r1.Is_Point_Within(xy)) {
		Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
	}

	Rect r2 = DAT_00B048C8;
	r2.X += offset;
	if (r2.Is_Point_Within(xy)) {
		Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
	}

	if (input == 0x80F2) {
		if (Session.Type != GAME_NORMAL && !((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode && Session.MPGameMode->vt_entry_08() && Session.MPGameMode->vt_entry_04())) {
			Game::SpecialDialog = 8;
		} else {
			Game::SpecialDialog = 9;
		}
	} else if (input == 0x80F3) {
		Game::SpecialDialog = 1;
	}

	if (DAT_00B45B68 > 0 && RadarMode != 3 && RadarMode != 4) {
		SuspendedRadarMode = RadarMode;
		Radar_Activate(4, 0);
		unknown_14B8 = 0;
	}

	if (RadarMode == 4) {
		Play_Movie();
	} else if (!FUN_0053BAE0()) {
		FUN_00753580();
	}
}

// FUNCTION: 0x00653F50
void RadarClass::Set_Map_Dimensions(Rect const & size, bool reset_cells, int cell_height, bool refresh_map)
{
	BASECLASS::Set_Map_Dimensions(size, reset_cells, cell_height, refresh_map);
}

// FUNCTION: 0x00653F70
void RadarClass::Set_Tactical_Position(Coord const & coord)
{
	TacticalMap->Set_Tactical_Position(coord);
}

// FUNCTION: 0x00653F90
bool RadarClass::Cell_On_Radar(Cell const & cell)
{
	return(In_Local_Radar(cell));
}

// FUNCTION: 0x00653FA0
void RadarClass::Draw_Names(void)
{
	int c_idx;
	int house;
	HouseClass * ptr;
	int y;
	wchar_t txt[40];
	int h;
	int kills;
	ColorScheme * color;
	int style;

	if (!Map.IsSidebarActive) {
		return;
	}

	ConvertClass * drawer = FUN_0072F510();
	Draw_Shape(*DAT_00887300, *drawer, (ShapeSet const *)DAT_00B04A38, 32, Point2D(RadX, RadWidth), DAT_00887300->Get_Rect());

	y = RadOffX + 2;
	Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), StringTable::LoadString("TXT_NAME_COLON", NULL, "D:\\ra2mdpost\\Radar.CPP", 1268), DAT_00887300, &DAT_00887300->Get_Rect(), &(Point2D const &)Point2D(RadHeight, y), Fetch_Scheme_By_Name("Grey", 1), 0, TextPrintType(0x19));
	Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), StringTable::LoadString("TXT_KILLS_COLON", NULL, "D:\\ra2mdpost\\Radar.CPP", 1270), DAT_00887300, &DAT_00887300->Get_Rect(), &(Point2D const &)Point2D(RadOffY + RadHeight - 2, y), Fetch_Scheme_By_Name("Grey", 1), 0, TextPrintType(0x219));

	y += 7;
	DAT_00887300->Draw_Line(Point2D(RadHeight, y), Point2D(RadOffY + RadHeight - 1, y), FileSystem::SIDEBAR_PAL->Convert_Pixel(14));
	y += 4;

	for (house = 0; house < Houses.Count(); house++) {
		ptr = Houses[house];

		if (!ptr || !ptr->Class->IsMultiplay) continue;

		c_idx = ptr->Scheme;

		if (ptr->IsDefeated) {
			color = Fetch_Scheme_By_Name("Grey", 1);
			style = 0x19;
		} else {
			color = ColorSchemes[c_idx];
			style = 0x4019;
		}

		txt[0] = 0;
		swprintf(txt, L"%s", ptr->UIName);

		if (wcslen(txt) == 0) {
			wcscpy(txt, L"________");
		}

		if (wcslen(txt) > 18) {
			txt[18] = '.';
			txt[19] = 0;
		}
		Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), txt, DAT_00887300, &DAT_00887300->Get_Rect(), &(Point2D const &)Point2D(RadHeight, y), color, 0, TextPrintType(style));

		kills = 0;
		for (h = 0; h < 20; h++) {
			kills += ptr->UnitsKilled[h];
			kills += ptr->BuildingsKilled[h];
		}
		swprintf(txt, L"%2d", kills);
		Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), txt, DAT_00887300, &DAT_00887300->Get_Rect(), &(Point2D const &)Point2D(RadOffY + RadHeight - 2, y), color, 0, TextPrintType(style | 0x200));

		y += 9;
	}
}

// FUNCTION: 0x00654320
void RadarClass::Reposition_Sidebar(void)
{
	BASECLASS::Reposition_Sidebar();

	int offset = DAT_00A8EB7C ? DAT_00886FA8 : 0;

	RadarStatics::DAT_00B04978.DrawOffset.X = -offset;
	RadarStatics::DAT_00B04978.Set_Position(DAT_00B04A00.X + offset, DAT_00B04A00.Y);
	RadarStatics::DAT_00B04978.Flag_To_Redraw();

	RadarStatics::DAT_00B04910.DrawOffset.X = -offset;
	RadarStatics::DAT_00B04910.Set_Position(DAT_00B048C8.X + offset, DAT_00B048C8.Y);
	RadarStatics::DAT_00B04910.Flag_To_Redraw();

	RadarButton.Set_Position(RadHeight + offset, RadOffX);
	RadarButton.Flag_To_Redraw();

	ToolTip tip;
	tip.ID = 242;
	tip.Text = (int)"Tip:BriefingButton";
	if (Session.Type != GAME_NORMAL) {
		tip.Text = (int)"Tip:DiplomacyButton";
	}
	tip.Region.X = RadarStatics::DAT_00B04978.X;
	tip.Region.Y = RadarStatics::DAT_00B04978.Y;
	tip.Region.Width = RadarStatics::DAT_00B04978.Width;
	tip.Region.Height = RadarStatics::DAT_00B04978.Height;
	CCToolTip::Instance->Remove(242);
	CCToolTip::Instance->Add(&tip);

	tip.ID = 243;
	tip.Text = (int)"Tip:OptionsButton";
	tip.Region.X = RadarStatics::DAT_00B04910.X;
	tip.Region.Y = RadarStatics::DAT_00B04910.Y;
	tip.Region.Width = RadarStatics::DAT_00B04910.Width;
	tip.Region.Height = RadarStatics::DAT_00B04910.Height;
	CCToolTip::Instance->Remove(243);
	CCToolTip::Instance->Add(&tip);

	FullRedraw = true;
}

// FUNCTION: 0x00655890
int RadarTrackingStruct::Hash_Old(RadarTrackingStruct const & s)
{
	return(((unsigned int)s.Object) + 251 * s.Position.X);
}

// FUNCTION: 0x006558B0
int RadarTrackingStruct::Hash2(RadarTrackingStruct const & s)
{
	return(s.Position.X + 251 * s.Position.Y);
}

// FUNCTION: 0x006558D0
void RadarClass::Init_Radar(void)
{
	RadarTrackingTable = new RADAR_HASH_TABLE(10, 256, RadarTrackingStruct::Hash2);
}

// FUNCTION: 0x00656CA0
bool RadarClass::Is_Radar_Active(void)
{
	return(RadarState == RSTATE_ACTIVE);
}

// FUNCTION: 0x00656DC0
bool RadarClass::Is_Radar_Tactical(void)
{
	return(RadarMode == RMODE_TACTICAL && RadarState == RSTATE_ACTIVE);
}

// FUNCTION: 0x00656DE0
bool RadarClass::Is_Radar_Existing(void)
{
	return(DoesRadarExist);
}

// FUNCTION: 0x00656E50
bool RadarClass::Is_Player_Names(void)
{
	return(RadarMode == RMODE_PLAYER_NAMES && RadarState == RSTATE_ACTIVE);
}

// FUNCTION: 0x00656E70
bool RadarClass::Is_Playing_Movie(void)
{
	return(RadarMode == RMODE_MOVIE && RadarState == RSTATE_ACTIVE);
}
