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
extern DynamicVectorClass<TechnoClass *> DAT_00A8EC78;
#define Technos DAT_00A8EC78
extern HouseClass * PlayerPtr;
namespace Unsorted { extern int MuteSWLaunches; }
inline bool RadarTrackingStruct::Use_Head(void) const { return(Object->House == PlayerPtr); }
int __fastcall FUN_00753250(char const * name);
void __fastcall FUN_0055AF40(int value);

// SYNTHETIC: GAMEMD 0x00652680 SYMBOL
// _$E109
const double DAT_00B048D8 = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);

// SYNTHETIC: GAMEMD 0x006526B0 SYMBOL
// _$E112
const double DAT_00B049F8 = DAT_00B048D8 / DAT_007F02A8;

// SYNTHETIC: GAMEMD 0x006526D0 SYMBOL
// _$E115
const double DAT_00B049E0 = 1.0 / DAT_00B049F8;

// SYNTHETIC: GAMEMD 0x006526F0 SYMBOL
// _$E118
const double DAT_00B049D8 = ONE_RAD * 45;

// SYNTHETIC: GAMEMD 0x00652710 SYMBOL
// _$E121
const double DAT_00B048E8 = ONE_RAD * 60;

// SYNTHETIC: GAMEMD 0x00652730 SYMBOL
// _$E124
const double DAT_00B048A0 = ONE_RAD * 90;

// SYNTHETIC: GAMEMD 0x00652750 SYMBOL
// _$E127
const int DAT_00B04A34 = (int)(fastmath::tan(DAT_00B048A0 - DAT_00B048E8) * DAT_00B048D8 * 0.5);

// SYNTHETIC: GAMEMD 0x00652790 SYMBOL
// _$E130
const double DAT_00B048F0 = fastmath::atan(DAT_00B04A34 * (1 / (double)CELL_LEPTON_H));

// SYNTHETIC: GAMEMD 0x006527C0 SYMBOL
// _$E133
const double DAT_00B04970 = fastmath::atan((DAT_00B04A34 * 2) / DAT_00B048D8);

// SYNTHETIC: GAMEMD 0x006527F0 SYMBOL
// _$E136
const int DAT_00B049F4 = (int)((DAT_00B04A34 * BRIDGE_CELL_HEIGHT) + 0.5);

// SYNTHETIC: GAMEMD 0x00652820 SYMBOL
// _$E139
Cell const DAT_00B048C0(0, 0);

// SYNTHETIC: GAMEMD 0x00652830 SYMBOL
// _$E142
Coord const DAT_00B04900(0, 0, 0);

// SYNTHETIC: GAMEMD 0x00652850 SYMBOL
// _$E145
Coord const DAT_00B049E8(128, 128, 0);

// SYNTHETIC: GAMEMD 0x00652870 SYMBOL
// _$E151
// SYNTHETIC: GAMEMD 0x006528A0 SYMBOL
// _$E148
RadarClass::RTacticalClass RadarClass::RadarButton;

struct RadarStatics {
	static ShapeButtonClass DAT_00B04978;
	static ShapeButtonClass DAT_00B04910;
};

// SYNTHETIC: GAMEMD 0x006528C0 SYMBOL
// _$E156
Rect DAT_00B04A00;

// SYNTHETIC: GAMEMD 0x006528D0 SYMBOL
// _$E161
// SYNTHETIC: GAMEMD 0x006528F0 SYMBOL
// _$E159
ShapeButtonClass RadarStatics::DAT_00B04978;

// SYNTHETIC: GAMEMD 0x00652910 SYMBOL
// _$E164
Rect DAT_00B04A20;

// SYNTHETIC: GAMEMD 0x00652920 SYMBOL
// _$E169
// SYNTHETIC: GAMEMD 0x00652940 SYMBOL
// _$E167
ShapeButtonClass RadarStatics::DAT_00B04910;

// FUNCTION: GAMEMD 0x00652960
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

// FUNCTION: GAMEMD 0x00652C00
RadarClass::~RadarClass(void)
{
	unknown_14C0.FUN_00405C00();
	Clear_Radar();
}

// FUNCTION: GAMEMD 0x00652CD0
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
	short Count;
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

// FUNCTION: GAMEMD 0x00652CF0
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

// FUNCTION: GAMEMD 0x00652D90
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

// FUNCTION: GAMEMD 0x00652DE0
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

// FUNCTION: GAMEMD 0x00652E50
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

// FUNCTION: GAMEMD 0x00652E90
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

// FUNCTION: GAMEMD 0x00653010
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

extern void * DAT_00B0F9E0;
extern bool DAT_00B04964;
extern bool DAT_00B049CC;
extern bool DAT_00B0B518;
extern ShapeButtonClass DAT_00B0B3A0;
extern ShapeButtonClass DAT_00B07DF8;

// FUNCTION: GAMEMD 0x00653100
void RadarClass::Draw_It(bool forced)
{
	bool timer_fired = false;

	if (Debug_Map) {
		FullRedraw = false;
		IsToRedraw = false;
		return;
	}

	FullRedraw = FullRedraw == true || forced == true;
	IsToRedraw = IsToRedraw == true || FullRedraw == true;

	if (IsToRedraw) {
		ShapeSet const * frame_shape = (ShapeSet const *)DAT_00B0F9E0;
		Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, frame_shape, 0, Point2D(RadX, RadY), DAT_00887300->Get_Rect());
		RadarStatics::DAT_00B04978.Draw_Me(true);
		RadarStatics::DAT_00B04910.Draw_Me(true);
	}

	if (RadarState != RSTATE_ACTIVE) {
		if (RadarState != RSTATE_INACTIVE) {
			if (!RadarAnimTimer) {
				RadarAnimTimer = 4;
				timer_fired = true;

				if (RadarState == RSTATE_DEACTIVATING) {
					if (--RadarAnimFrame <= 0) {
						RadarState = RSTATE_INACTIVE;
						unknown_14C0.EndLooping();
						RadarAnimFrame = 0;
					}
				} else {
					if (++RadarAnimFrame >= 32) {
						unknown_14C0.EndLooping();
						RadarState = RSTATE_ACTIVE;
						FullRedraw = true;
						RadarAnimFrame = 32;
					}
				}
			}
		}

		if (RadarState != RSTATE_ACTIVE) {
			if (IsToRedraw == true || timer_fired) {
				if (RadarAnimFrame >= 0 && RadarAnimFrame < ((ShpHeader *)DAT_00B04A38)->Count) {
					ConvertClass * drawer = FUN_0072F510();
					Draw_Shape(*DAT_00887300, *drawer, (ShapeSet const *)DAT_00B04A38, RadarAnimFrame, Point2D(RadX, RadWidth), DAT_00887300->Get_Rect());
					LastDrawRect = Rect(RadX, RadWidth, DAT_007F5BF8, 110);
				}
			}
		}
	}

	Process_Radar_Events();
	Render_Radar();

	if (RadarState == RSTATE_ACTIVE) {
		switch (RadarMode) {
			case RMODE_TACTICAL:
				break;

			case RMODE_PLAYER_NAMES:
				if (IsToRedraw) {
					bool full = FullRedraw;
					IsToRedraw = false;
					if (full) {
						ConvertClass * drawer = FUN_0072F510();
						Draw_Shape(*DAT_00887300, *drawer, (ShapeSet const *)DAT_00B04A38, 32, Point2D(RadX, RadWidth), DAT_00887300->Get_Rect());
						Draw_Names();
						LastDrawRect = Rect(RadX, RadWidth, DAT_007F5BF8, 110);
					} else {
						Draw_Names();
						LastDrawRect = Rect(RadHeight, RadOffX, RadIHeight, RadPWidth);
					}
				}
				break;

			case RMODE_MOVIE:
				Play_Movie();
				break;

			case RMODE_4:
				break;

			default:
				if (IsToRedraw) {
					IsToRedraw = false;
					ConvertClass * drawer = FUN_0072F510();
					Draw_Shape(*DAT_00887300, *drawer, (ShapeSet const *)DAT_00B04A38, 0, Point2D(RadX, RadWidth), DAT_00887300->Get_Rect());
					LastDrawRect = Rect(RadX, RadWidth, DAT_007F5BF8, 110);
				}
				break;
		}
	}

	if (FullRedraw) {
		FullRedraw = false;
		DAT_00B0B3A0.Draw_Me(true);
		DAT_00B07DF8.Draw_Me(true);
	}

	if (DAT_00B049CC) {
		LastDrawRect = Union(LastDrawRect, DAT_00B04A00);
		DAT_00B0B518 = true;
	}

	if (DAT_00B04964) {
		LastDrawRect = Union(LastDrawRect, DAT_00B048C8);
		DAT_00B0B518 = true;
	}
}

// FUNCTION: GAMEMD 0x00653750
void RadarClass::Noop(int, int)
{
}

// FUNCTION: GAMEMD 0x00653760
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

// FUNCTION: GAMEMD 0x00653810
bool RadarClass::Map_Cell(Cell const & cell, HouseClass * house)
{
	if (BASECLASS::Map_Cell(cell, house)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00653830
bool RadarClass::RevealFogShroud(Cell const & cell, HouseClass * house, bool increase)
{
	if (BASECLASS::RevealFogShroud(cell, house, increase)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00653850
void RadarClass::AI(KeyNumType & input, Point2D const & xy)
{
	BASECLASS::AI(input, xy);

	int offset = Options.IsSidebarOnRight ? DAT_00886FA8 : 0;

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
		FUN_006578F0();
	} else if (!FUN_0053BAE0()) {
		FUN_00753580();
	}
}

// FUNCTION: GAMEMD 0x00653F50
void RadarClass::Set_Map_Dimensions(Rect const & size, bool reset_cells, int cell_height, bool refresh_map)
{
	BASECLASS::Set_Map_Dimensions(size, reset_cells, cell_height, refresh_map);
}

// FUNCTION: GAMEMD 0x00653F70
void RadarClass::Set_Tactical_Position(Coord const & coord)
{
	TacticalMap->Set_Tactical_Position(coord);
}

// FUNCTION: GAMEMD 0x00653F90
bool RadarClass::Cell_On_Radar(Cell const & cell)
{
	return(In_Local_Radar(cell));
}

// FUNCTION: GAMEMD 0x00653FA0
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

// FUNCTION: GAMEMD 0x00654320
void RadarClass::Reposition_Sidebar(void)
{
	BASECLASS::Reposition_Sidebar();

	int offset = Options.IsSidebarOnRight ? DAT_00886FA8 : 0;

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

// FUNCTION: GAMEMD 0x00654490
void RadarClass::Set_Local_Dimensions(Rect const & size)
{
	BASECLASS::Set_Local_Dimensions(size);

	RadarRect.Y = 10000;
	RadarRect.X = 10000;
	RadarRect.Height = 0;
	RadarRect.Width = 0;

	Cell corner(LocalRect.Y + LocalRect.X + 1, LocalRect.Y + PlayRect.Width - LocalRect.X);
	Cell corner2(corner);
	int x = LocalRect.Width - 1;
	int y = 1 - LocalRect.Width;
	Cell opposite = corner + Cell(x, y);

	RadarX = corner2.Y - corner.X;
	field_149C = opposite.X - opposite.Y;
	RadarY = -1;

	Map.Reset_Iterator();
	CellClass * iter = Iterate();

	while (iter != NULL) {
		Cell cell = iter->CellID;

		if (In_Local_Radar(cell)) {

			if (RadarY == -1) {
				RadarY = iter->CellID.X + iter->CellID.Y;
			}

			cell = iter->CellID;
			Rect r = Cell_Radar_Rect(cell);

			if (r.X < RadarRect.X) {
				RadarRect.X = r.X;
			} else if (r.X + r.Width > RadarRect.X + RadarRect.Width) {
				RadarRect.Width = r.X + r.Width - RadarRect.X;
			}

			if (r.Y < RadarRect.Y) {
				RadarRect.Y = r.Y;
			} else if (r.Y + r.Height > RadarRect.Y + RadarRect.Height) {
				RadarRect.Height = r.Y + r.Height - RadarRect.Y;
			}
		}
		iter = Iterate();
	}
}

// FUNCTION: GAMEMD 0x00654650
void RadarClass::Compute_Radar_Image(void)
{
	if (!RadarRect.Is_Valid()) {
		return;
	}

	RadarRect.Y = 0;
	RadarRect.X = 0;

	if (BackgroundSurface != NULL) {
		delete BackgroundSurface;
		BackgroundSurface = NULL;
	}
	if (BackgroundColors != NULL) {
		delete BackgroundColors;
		BackgroundColors = NULL;
	}
	if (PixelFlags != NULL) {
		delete [] PixelFlags;
		PixelFlags = NULL;
	}

	Compute_Background(RadarRect, RadarRect, true);

	RadarRect.Width = BackgroundSurface->Get_Width();
	RadarRect.Height = BackgroundSurface->Get_Height();

	RadarRect.X = RadHeight;
	if (RadarRect.Width < 140) {
		RadarRect.X += (140 - RadarRect.Width) / 2;
	}

	RadarRect.Y = RadOffX;
	if (RadarRect.Height < 108) {
		RadarRect.Y += (108 - RadarRect.Height) / 2;
	}

	if (RadarSurface != NULL) {
		delete RadarSurface;
		RadarSurface = NULL;
	}

	RadarSurface = new DSurface(BackgroundSurface->Get_Width(), BackgroundSurface->Get_Height(), true);
	RadarSurface->Fill(TBLACK);

	Compute_Foundations();
}

// FUNCTION: GAMEMD 0x00654EA0
void RadarClass::Fill_In_Background(Rect const & cell_rect, Rect const & update_rect)
{
	RGBClass low(0, 0, 0);
	RGBClass high(0, 0, 0);

	Reset_Iterator();
	CellClass * iter = Map.Iterate();

	int a = 2 * Map.LocalRect.X - Map.PlayRect.Width + 1;

	while (iter != NULL) {

		int x = iter->CellID.X;
		int y = iter->CellID.Y;

		if (x + y >= RadarY) {
			int origx = x - y - a;
			int origy = x + y - RadarY;
			Rect mrect(origx, origy, 2, 1);
			mrect = Intersect(mrect, update_rect);
			if (mrect.Is_Valid()) {
				int idx = mrect.X + mrect.Y * cell_rect.Width;
				iter->Cell_Color(low, high);
				if (origx == update_rect.X - 1) {
					BackgroundColors[idx] = high;
				} else if (origx == update_rect.Width + update_rect.X - 1) {
					BackgroundColors[idx] = low;
				} else {
					BackgroundColors[idx] = low;
					BackgroundColors[idx+1] = high;
				}
			}
		}
		iter = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x00655050
Rect RadarClass::Cell_Radar_Rect(Cell const & cell)
{
	int cellx = cell.X;
	int celly = cell.Y;
	int x = cellx - celly + RadarX;
	int y = cellx + celly - RadarY;
	int width = 2;
	if (x == -1) {
		x = 0;
		width = 1;
	} else if (x == width * LocalRect.Width - 1) {
		width = 1;
	}
	return(Rect(x, y, width, 1));
}

// FUNCTION: GAMEMD 0x006550C0
Rect RadarClass::Cell_To_Radar_Pixel(Cell const & cell)
{
	int cellx = cell.X;
	int celly = cell.Y;
	int x = (cellx - celly + RadarX) * ZoomFactor + RadarRect.X;
	int y = (cellx + celly - RadarY) * ZoomFactor + RadarRect.Y;
	if (x == RadarRect.X - 1) x++;
	return(Rect(x, y, 1, 1));
}

// FUNCTION: GAMEMD 0x00655150
Cell RadarClass::Radar_Pixel_To_Cell(Point2D const& point)
{
	float x = (point.X / ZoomFactor) - RadarX;
	float y = (point.Y / ZoomFactor) + RadarY;

	Cell cell;
	cell.X = int(((x + y) / 2) + 0.5);
	cell.Y = int(((y - x) / 2) + 0.5);

	return(cell);
}

// FUNCTION: GAMEMD 0x006551C0
void RadarClass::Radar_Background(Cell const & cell)
{
	for (int i = BackgroundStack.Count() - 1; i >= 0; i--) {
		if (BackgroundStack[i] == cell) {
			return;
		}
	}
	BackgroundStack.Add(cell);
	IsToRedraw = true;
}

// FUNCTION: GAMEMD 0x00655250
void RadarClass::Plot_Radar_Background(void)
{
	Rect r(0, 0, 0, 0);
	for (int i = BackgroundStack.Count() - 1; i >= 0; i--) {

		Cell cell = BackgroundStack[i];
		if (In_Local_Radar(cell)) {
			Rect update_rect = Cell_Radar_Rect(cell);

			int x = update_rect.X;
			Rect bounds(0, 0, RadarCellWidth, RadarCellHeight);
			update_rect = Intersect(update_rect, bounds);

			if (update_rect.Is_Valid()) {
				RGBClass low(0, 0, 0);
				RGBClass high(0, 0, 0);

				CellClass * cptr = &Map[cell];
				int idx = update_rect.X + RadarCellWidth * update_rect.Y;
				cptr->Cell_Color(low, high);

				if (x == 0 && update_rect.Width == 1) {
					BackgroundColors[idx+1] = high;
				} else if (update_rect.Width == 1) {
					BackgroundColors[idx] = low;
				} else {
					BackgroundColors[idx] = low;
					BackgroundColors[idx+1] = high;
				}

				Rect radarcellrect(0, 0, RadarCellWidth, RadarCellHeight);
				Rect rr = Compute_Background(radarcellrect, update_rect, false);
				r = Union(r, rr);
			}
		}
	}

	BackgroundStack.Clear();
	CellRedrawRect = r;
}

// FUNCTION: GAMEMD 0x00655560
void RadarClass::Radar_Track(TechnoClass * techno, Point2D point)
{
	Rect surfacerect = RadarSurface->Get_Rect();

	if (!surfacerect.Is_Point_Within(point)) {
		if (techno->RTTI == RTTI_BUILDING) return;

		if (point.X < surfacerect.X) {
			point.X = surfacerect.X;
		}
		if (point.X >= surfacerect.X + surfacerect.Width) {
			point.X = surfacerect.X + surfacerect.Width - 1;
		}
		if (point.Y < surfacerect.Y) {
			point.Y = surfacerect.Y;
		}
		if (point.Y >= surfacerect.Y + surfacerect.Height) {
			point.Y = surfacerect.Y + surfacerect.Height - 1;
		}
		techno->RadarPos = point;
	}

	RADAR_HASH_TABLE::ObjectType track;
	track.Key.Position = point;
	track.Key.Object = techno;
	track.Value = techno;

	if (RadarTrackingTable->Add_Object(track, true)) {
		Radar_Pixel(point);
		IsToRedraw = true;
	}
}

// FUNCTION: GAMEMD 0x00655740
void RadarClass::Radar_Untrack(TechnoClass * techno, Point2D point)
{
	RADAR_HASH_TABLE::BucketType * bucket = &RadarTrackingTable->Buckets[RadarTrackingStruct(techno, point.X, point.Y).Hash()];
	RADAR_HASH_TABLE::ObjectType track;
	track.Key.Position = point;
	track.Key.Object = techno;
	track.Value = techno;
	if (bucket->Delete(track)) {
		Radar_Pixel(point);
		IsToRedraw = true;
	}
}

// FUNCTION: GAMEMD 0x006557F0
Point2D RadarClass::Coord_To_Radar_Pixel(Coord const & coord, bool clip)
{
	Point2D pt = coord;
	int x = (pt.X - pt.Y + (RadarX * CELL_LEPTON_W)) * ZoomFactor / CELL_LEPTON_W;
	int y = (pt.X + pt.Y - (RadarY * CELL_LEPTON_H)) * ZoomFactor / CELL_LEPTON_H;
	if (clip) {
		if (x < 0) {
			x = 0;
		}
		if (x >= RadarRect.Width) {
			x = RadarRect.Width - 1;
		}
		if (y < 0) {
			y = 0;
		}
		if (y >= RadarRect.Height) {
			y = RadarRect.Height - 1;
		}
	}
	return(Point2D(x, y));
}

// FUNCTION: GAMEMD 0x00655890
int RadarTrackingStruct::Hash_Old(RadarTrackingStruct const & s)
{
	return(((unsigned int)s.Object) + 251 * s.Position.X);
}

// FUNCTION: GAMEMD 0x006558B0
int RadarTrackingStruct::Hash2(RadarTrackingStruct const & s)
{
	return(s.Position.X + 251 * s.Position.Y);
}

// FUNCTION: GAMEMD 0x006558D0
void RadarClass::Init_Radar(void)
{
	RadarTrackingTable = new RADAR_HASH_TABLE(10, 256, RadarTrackingStruct::Hash2);
}

// FUNCTION: GAMEMD 0x00655990
void RadarClass::Reset_Radar(void)
{
	if (RadarTrackingTable != NULL) {
		delete RadarTrackingTable;
	}

	Init_Radar();
	Map.Set_Local_Dimensions(Map.LocalRect);
	Compute_Radar_Image();

	for (int i = 0; i < Technos.Count(); i++) {
		Technos[i]->IsRadarTracked = false;
	}
}

// FUNCTION: GAMEMD 0x00655A90
void RadarClass::Clear_Radar(void)
{
	if (RadarSurface != NULL) {
		delete RadarSurface;
		RadarSurface = NULL;
	}
	if (BackgroundSurface != NULL) {
		delete BackgroundSurface;
		BackgroundSurface = NULL;
	}
	if (BackgroundColors != NULL) {
		delete BackgroundColors;
		BackgroundColors = NULL;
	}
	if (RadarTrackingTable != NULL) {
		delete RadarTrackingTable;
		RadarTrackingTable = NULL;
	}
	if (PixelFlags != NULL) {
		delete [] PixelFlags;
		PixelFlags = NULL;
	}
}

// FUNCTION: GAMEMD 0x00655B20
void RadarClass::Post_Load_Radar_Fixup(void)
{
	RadarSurface = NULL;
	BackgroundSurface = NULL;
	BackgroundColors = NULL;
	RadarTrackingTable = NULL;
	PixelFlags = NULL;

	Init_Radar();

	Map.Set_Local_Dimensions(Map.LocalRect);
	Compute_Radar_Image();

	for (int i = 0; i < Technos.Count(); i++) {
		Technos[i]->IsRadarTracked = false;
	}
	if (RadarMode == RMODE_MOVIE) {
		RadarState = RSTATE_MOVIE_DONE;
		Radar_Activate(SuspendedRadarMode, 0);
		FUN_0055AF40(Rule->DetailMinFrameRateNormal);
	}
}

// FUNCTION: GAMEMD 0x00656150
void RadarClass::Render_Tracked_Objects(void)
{
	memset(PixelFlags, 0, RadarSurface->Get_Width() * RadarSurface->Get_Height() / 8 + 1);

	for (int b = 0; b < 256; b++) {

		RADAR_HASH_TABLE::BucketType & bucket = RadarTrackingTable->Buckets[b];

		int count = bucket.Count();

		RADAR_HASH_TABLE::ObjectType * ptr = &bucket[0];

		for (int t = 0; t < count; t++) {
			RadarTrackingStruct * track = &ptr[t].Key;
			TechnoClass * tech = track->Object;

			int id = track->Position.X + track->Position.Y * RadarSurface->Get_Width();
			int i = id >> 3;
			int bit = 1 << (id & 7);

			if ((PixelFlags[i] & bit) == 0) {
				PixelFlags[i] |= bit;

				ColorScheme * scheme = ColorSchemes[tech->House->Scheme];

				if (tech->IsDisguised()) {
					if (tech->GetDisguiseHouse(false) != NULL) {
						scheme = ColorSchemes[tech->GetDisguiseHouse(false)->Scheme];
					} else {
						scheme = Fetch_Scheme_By_Name("LightGrey", 1);
					}
				}

				int color = scheme->Bright;
				ConvertClass * drawer = scheme->Converter;
				if (drawer->Bytes_Per_Pixel() == 1) {
					unsigned char * translator = (unsigned char *)drawer->Get_Translate_Table();
					color = translator[color];
				} else {
					unsigned short * translator = (unsigned short *)drawer->Get_Translate_Table();
					color = translator[color];
				}

				RadarSurface->Put_Pixel(track->Position, color);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006562D0
void RadarClass::Radar_Pixel(Point2D const & point)
{
	if (point.X >= 0 && point.X < RadarSurface->Get_Width() && point.Y >= 0 && point.Y < RadarSurface->Get_Height()) {
		int id = point.X + point.Y * RadarSurface->Get_Width();
		int i = id >> 3;
		int bit = 1 << (id & 7);
		if ((PixelFlags[i] & bit) == 0) {
			PixelFlags[i] |= bit;
			PixelStack.Add(point);
			IsToRedraw = true;
		}
	}
}

// FUNCTION: GAMEMD 0x006563B0
void RadarClass::Compute_Foundations(void)
{
	int i;
	for (i = 0; i < BSIZE_COUNT; i++) {
		Foundation[i].Clear();

		int bwidth = BuildingTypeClass::SizeWidth[i];
		int bheight = BuildingTypeClass::SizeHeight[i];

		int zw;
		if (bwidth == 1) {
			zw = max(1.0, ZoomFactor + 0.5);
		} else {
			zw = max(2.0, bwidth * ZoomFactor + 0.5);
		}

		int zh;
		if (bheight == 1) {
			zh = max(1.0, ZoomFactor + 0.5);
		} else {
			zh = max(2.0, bheight * ZoomFactor + 0.5);
		}

		int fy = 0;
		int zs = zh + zw - 1;

		if (zs > 0) {
			int left_edge = 0;
			while (true) {
				int xbegin;
				if (fy < zh) {
					xbegin = left_edge;
				} else {
					xbegin = fy - 2 * zh + 2;
				}
				int xend;
				if (fy < zw) {
					xend = fy;
				} else {
					xend = 2 * zw - fy - 2;
				}
				int fx = xbegin;

				while (fx <= xend) {
					Foundation[i].Add(Point2D(fx, fy));
					++fx;
				}
				++fy;
				--left_edge;
				if (fy >= zs) {
					break;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00656580
FOUNDATION_LIST const & RadarClass::Get_Foundation(BuildingTypeClass * btype)
{
	return(Foundation[btype->Size]);
}

// FUNCTION: GAMEMD 0x006565A0
void RadarClass::Radar_Cell(Cell const & cell)
{
	if (RadarSurface != NULL) {
		Rect r1 = Cell_Radar_Rect(cell);
		r1 = Intersect(r1, Rect(0, 0, RadarCellWidth, RadarCellHeight));
		int swidth = RadarSurface->Get_Width();
		int sheight = RadarSurface->Get_Height();

		if (r1.Is_Valid()) {

			int zx = (int)(r1.X * ZoomFactor);
			int startx = (zx - 1) & ((zx - 1 < 0) - 1);
			int width = min((int)((double)(r1.X + r1.Width) * ZoomFactor) + 1, swidth);

			int zy = (int)(r1.Y * ZoomFactor);
			int starty = (zy - 1) & ((zy - 1 < 0) - 1);
			int height = min((int)((double)(r1.Y + r1.Height) * ZoomFactor) + 1, sheight);

			for (int y = starty; y < height; y++) {
				for (int x = startx; x < width; x++) {
					Radar_Pixel(Point2D(x, y));
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006568A0
HRESULT RadarClass::Load(IStream * stream)
{
	int i;

	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		int count;
		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		unknown_14C0.FUN_00405BE0();

		new (&PixelStack) FOUNDATION_LIST;

		for (i = 0; i < count; i++) {
			Point2D point;
			result = stream->Read(&point, sizeof(point), NULL);
			if (FAILED(result)) {
				return(result);
			}
			PixelStack.Add(point);
		}

		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		new (&BackgroundStack) DynamicVectorClass<Cell>;

		for (i = 0; i < count; i++) {
			Cell cell;
			result = stream->Read(&cell, sizeof(cell), NULL);
			if (FAILED(result)) {
				return(result);
			}
			BackgroundStack.Add(cell);
		}

		for (i = 0; i < BSIZE_COUNT; i++) {
			new (&Foundation[i]) FOUNDATION_LIST;
		}

		result = stream->Read(&RadarMode, sizeof(RadarMode), NULL);
		if (FAILED(result)) {
			return(result);
		}
		result = stream->Read(&RadarState, sizeof(RadarState), NULL);
		if (FAILED(result)) {
			return(result);
		}
		result = stream->Read(&SuspendedRadarMode, sizeof(SuspendedRadarMode), NULL);
		if (FAILED(result)) {
			return(result);
		}
		result = stream->Read(&unknown_14B8, sizeof(unknown_14B8), NULL);
		if (FAILED(result)) {
			return(result);
		}

		unknown_14C0.FUN_00405BE0();
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00656AC0
HRESULT RadarClass::Save(IStream * stream)
{
	int i;

	HRESULT result = BASECLASS::Save(stream);
	if (SUCCEEDED(result)) {
		int count = PixelStack.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (i = 0; i < count; i++) {
			result = stream->Write(&PixelStack[i], sizeof(PixelStack[i]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		count = BackgroundStack.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (i = 0; i < count; i++) {
			result = stream->Write(&BackgroundStack[i], sizeof(BackgroundStack[i]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		result = stream->Write(&RadarMode, sizeof(RadarMode), NULL);
		if (FAILED(result)) {
			return(result);
		}
		result = stream->Write(&RadarState, sizeof(RadarState), NULL);
		if (FAILED(result)) {
			return(result);
		}
		result = stream->Write(&SuspendedRadarMode, sizeof(SuspendedRadarMode), NULL);
		if (FAILED(result)) {
			return(result);
		}
		result = stream->Write(&unknown_14B8, sizeof(unknown_14B8), NULL);
		return(SUCCEEDED(result) ? S_OK : result);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00656BE0
void RadarClass::Set_Radar_State(bool on, bool sound)
{
	if (on == true) {
		if (RadarState == RSTATE_INACTIVE || RadarState == RSTATE_DEACTIVATING) {
			RadarState = RSTATE_ACTIVATING;
			unknown_14C0.EndLooping();
			if (sound) {
				VocClass::PlayGlobal(Rule->RadarOn, 1.0f, 0x2000);
			}
			DebugString("Radar: ACTIVATING\n");
		}
	} else {
		unknown_14C0.EndLooping();
		RadarState = RSTATE_DEACTIVATING;
		if (sound) {
			VocClass::PlayGlobal(Rule->RadarOff, 1.0f, 0x2000);
		}
		DebugString("Radar: DEACTIVING\n");
	}
	RadarAnimFrame = RadarAnimFrame;
}

// FUNCTION: GAMEMD 0x00656CA0
bool RadarClass::Is_Radar_Active(void)
{
	return(RadarState == RSTATE_ACTIVE);
}

// FUNCTION: GAMEMD 0x00656CB0
void RadarClass::Radar_Activate(int control, bool flag)
{
	if (Session.Type != GAME_NORMAL && PlayerPtr->IsDefeated && Unsorted::MuteSWLaunches) {
		control = 1;
	}
	if (RadarMode == control) {
		return;
	}
	if (RadarMode == RMODE_MOVIE || (RadarMode == RMODE_4 && control != RMODE_MOVIE)) {
		if (RadarState != RSTATE_MOVIE_DONE) {
			SuspendedRadarMode = control;
			return;
		}
	}
	switch (control) {
		case 0:
			Set_Radar_State(false, RadarMode == RMODE_TACTICAL);
			break;
		case 1:
			if ((RadarState == RSTATE_ACTIVE || RadarState == RSTATE_MOVIE_DONE) && Is_Radar_Existing() == true) {
				Queue_Next_Movie();
				break;
			}
			Set_Radar_State(Is_Radar_Existing(), flag);
			break;
		case 4:
			break;
		default:
			if (RadarState == RSTATE_ACTIVE || RadarState == RSTATE_MOVIE_DONE) {
				Queue_Next_Movie();
				break;
			}
			Set_Radar_State(true, false);
			break;
	}
	RadarMode = control;
}

// FUNCTION: GAMEMD 0x00656DC0
bool RadarClass::Is_Radar_Tactical(void)
{
	return(RadarMode == RMODE_TACTICAL && RadarState == RSTATE_ACTIVE);
}

// FUNCTION: GAMEMD 0x00656DE0
bool RadarClass::Is_Radar_Existing(void)
{
	return(DoesRadarExist);
}

// FUNCTION: GAMEMD 0x00656DF0
void RadarClass::Toggle_Radar(bool on)
{
	if (DoesRadarExist != on) {
		DoesRadarExist = on;
		char const * txt = on ? "on" : "off";
		DebugString("Radar: TacticalMap availability is %s\n", txt);
		if (RadarMode == RMODE_TACTICAL) {
			Set_Radar_State(on, true);
		} else {
			Radar_Activate(1, true);
		}
	}
}

// FUNCTION: GAMEMD 0x00656E50
bool RadarClass::Is_Player_Names(void)
{
	return(RadarMode == RMODE_PLAYER_NAMES && RadarState == RSTATE_ACTIVE);
}

// FUNCTION: GAMEMD 0x00656E70
bool RadarClass::Is_Playing_Movie(void)
{
	return(RadarMode == RMODE_MOVIE && RadarState == RSTATE_ACTIVE);
}

// FUNCTION: GAMEMD 0x00656E90
void RadarClass::Redraw_Radar(bool complete)
{
	IsToRedraw = true;
	FullRedraw = FullRedraw || complete;
}

void FUN_00752370(void);
void FUN_00752480(int index, int position, int flags);
void FUN_00753570(void);
bool FUN_007532A0(int index);
void FUN_00408390(void);
int FUN_00408410(void);
void FUN_005BF580(void);

// FUNCTION: GAMEMD 0x006578F0
void RadarClass::FUN_006578F0(void)
{
	switch (unknown_14B8) {
		case 0:
			FUN_00752370();
			FUN_00752480(unknown_int_14D4, 0, 3);
			FUN_00753570();
			unknown_14B8 = 1;
			break;
		case 1:
			if (FUN_007532A0(unknown_int_14D4)) {
				if (DAT_00B45B68) {
					FUN_00408390();
				}
				unknown_14B8 = 2;
			}
			break;
		case 2:
			if (DAT_00B45B68 == 0 || FUN_00408410()) {
				if (DAT_00B45B68) {
					FUN_00753580();
					FUN_005BF580();
				}
				Radar_Activate(3, false);
				unknown_14C0.EndLooping();
				unknown_14C0.SetEvent((AudioEvent *)VocClass::PlayGlobal(Rule->MovieOn, 1.0f, 0x2000), NULL);
				FUN_0055AF40(Rule->DetailMinFrameRateMovie);
			}
			break;
	}
}

void FUN_007529E0(void);
void FUN_005BF530(void);
void __fastcall Hold_Scenario(int seconds);
void Release_Scenario(void);

// FUNCTION: GAMEMD 0x006579E0
void RadarClass::Play_Movie(void)
{
	MovieHandle * movie = NULL;
	bool hold = false;
	bool has = IngameVQ.Count() > 0;
	bool finished;

	if (!unknown_bool_14BD && has) {
		FUN_007529E0();
	}

	if (FullRedraw == true && !unknown_bool_14BD) {
		ConvertClass * drawer = FUN_0072F510();
		Draw_Shape(*DAT_00887300, *drawer, (ShapeSet const *)DAT_00B04A38, 32, Point2D(RadX, RadWidth), DAT_00887300->Get_Rect());
		LastDrawRect = Rect(RadX, RadWidth, DAT_007F5BF8, 110);
		DebugString("Radar: Movie full redrawn\n");
	}

	if (has) {
		if (IngameVQ.Count() > 0 && (movie = IngameVQ[0]) != NULL) {
			if (movie->AdvanceFrame() == true) {
				LastDrawRect = Rect(RadX, RadWidth, DAT_007F5BF8, 110);
			}
			finished = movie->FramesLeft();
			hold = movie->State;
		} else {
			finished = true;
		}
	} else {
		finished = false;
	}

	if (finished == true) {
		if (movie != NULL) {
			IngameVQ.Delete(movie);
			delete movie;
		}
		has = IngameVQ.Count() != 0;
		if (has) {
			DebugString("Radar: Next movie.\n");
			unknown_14C0.EndLooping();
			unknown_14C0.SetEvent((AudioEvent *)VocClass::PlayGlobal(Rule->MovieOn, 1.0f, 0x2000), NULL);
			RadarState = RSTATE_NEXT_MOVIE;
			RadarAnimFrame = RADAR_ACTIVATED_FRAME;
		}
	}

	IsToRedraw = false;

	if (!has) {
		DebugString("Radar: Movie done.\n");
		RadarState = RSTATE_MOVIE_DONE;
		Radar_Activate(SuspendedRadarMode, false);
		FUN_005BF530();
		hold = has;
		unknown_14C0.EndLooping();
		unknown_14C0.SetEvent((AudioEvent *)VocClass::PlayGlobal(Rule->MovieOff, 1.0f, 0x2000), NULL);
		FUN_0055AF40(Rule->DetailMinFrameRateNormal);
		IsToRedraw = true;
	}

	unknown_bool_14BD = !finished;
	if (unknown_bool_14BC != hold) {
		if (hold) {
			Hold_Scenario(0);
			unknown_bool_14BC = true;
		} else {
			Release_Scenario();
			unknown_bool_14BC = false;
		}
	}
}

// FUNCTION: GAMEMD 0x00657CE0
void RadarClass::Complete_Radar_Refresh(void)
{
	if (MainWindow == NULL) {
		RadarSurface->Blit_From(RadarSurface->Get_Rect(), *BackgroundSurface, BackgroundSurface->Get_Rect());
		Render_Tracked_Objects();
	} else {
		int width = RadarSurface->Get_Width();
		int height = RadarSurface->Get_Height();
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Plot_Radar_Pixel(Point2D(x, y));
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00658640
void RadarClass::FUN_00658640(HWND dialog)
{
	int local = -1;
	int count = Houses.Count();
	for (int i = 0; i < count; i++) {
		HouseClass * house = Houses[i];
		if (house) {
			if (house->Is_Player_Control()) {
				local = i;
			} else if (house->Class->IsMultiplayPassive) {
				continue;
			}
			FUN_00658330(dialog, i, house);
		}
	}
	HouseClass * player = Houses[local];
	HouseClass * first = Houses[0];
	FUN_00658330(dialog, 0, player);
	FUN_00658330(dialog, local, first);
}

namespace UI {
	HWND __fastcall BeginDialog(int resource, WNDPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	bool Updated(void);
}
class InputManagerClass {
	public:
		void DoSomething(void);
		static InputManagerClass * Instance;
};
extern bool UserInputLocked;
#include "scenario.h"
#include "geometry.h"
LRESULT CALLBACK FUN_00657DA0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x006586D0
void RadarClass::FUN_006586D0(void)
{
	int result;

	HWND dialog = UI::BeginDialog(0x73, FUN_00657DA0, 0);
	UserInputLocked = true;
	InputManagerClass::Instance->DoSomething();
	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&result);
		UI::FocusOnWindow(dialog);
		result = 0;
		while (true) {
			if (UI::Updated()) {
				result = 0x423;
				break;
			}
			if (result) {
				break;
			}
		}
		UI::EndDialog(dialog);
	}
	InputManagerClass::Instance->DoSomething();
	UserInputLocked = Scen->IsInputLocked;
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}

// FUNCTION: GAMEMD 0x00658770
char const * RadarClass::Help_Text(int id)
{
	return(BASECLASS::Help_Text(id));
}

// SYNTHETIC: GAMEMD 0x00658780
// ??_GRTacticalClass@RadarClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006587A0
// ??_GRadarClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00658890
// ?Clear@?$DynamicVectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006588C0
// ?ID@?$DynamicVectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEHPBU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@Z
// TEMPLATE: GAMEMD 0x006588E0
// ?GetItem@?$VectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UBE?AU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@H@Z
// TEMPLATE: GAMEMD 0x00658910
// ??8?$VectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00658970
// ?Resize@?$VectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAE_NHPBU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@Z
// TEMPLATE: GAMEMD 0x00658A70
// ?Clear@?$VectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00658AA0
// ?ID@?$VectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEHABU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@Z
// TEMPLATE: GAMEMD 0x00658AF0
// ?ID@?$VectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEHPBU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@Z
// TEMPLATE: GAMEMD 0x00658B10
// ?Resize@?$DynamicVectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAE_NHPBU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@Z
// TEMPLATE: GAMEMD 0x00658C20
// ?ID@?$DynamicVectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEHABU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@Z
// SYNTHETIC: GAMEMD 0x00658C60
// ??_E?$DynamicVectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEPAXI@Z
template class DynamicVectorClass<HashObject<RadarTrackingStruct, TechnoClass *> >;

// SYNTHETIC: GAMEMD 0x00658D10
// ??_G?$VectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00658D60
// ??0?$VectorClass@VPoint2D@@@@QAE@HPBVPoint2D@@@Z
// TEMPLATE: GAMEMD 0x00658DE0
// ??0?$DynamicVectorClass@U?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@@QAE@IPBU?$HashObject@URadarTrackingStruct@@PAVTechnoClass@@@@@Z

// SYNTHETIC: GAMEMD 0x00658E80 SYMBOL
// _$E192
// SYNTHETIC: GAMEMD 0x00658EB0 SYMBOL
// _$E195
// SYNTHETIC: GAMEMD 0x00658ED0 SYMBOL
// _$E198
// SYNTHETIC: GAMEMD 0x00658EF0 SYMBOL
// _$E201
// SYNTHETIC: GAMEMD 0x00658F10 SYMBOL
// _$E204
// SYNTHETIC: GAMEMD 0x00658F30 SYMBOL
// _$E207
// SYNTHETIC: GAMEMD 0x00658F50 SYMBOL
// _$E210
// SYNTHETIC: GAMEMD 0x00658F90 SYMBOL
// _$E213
// SYNTHETIC: GAMEMD 0x00658FC0 SYMBOL
// _$E216
// SYNTHETIC: GAMEMD 0x00658FF0 SYMBOL
// _$E219
// SYNTHETIC: GAMEMD 0x00659020 SYMBOL
// _$E222
// SYNTHETIC: GAMEMD 0x00659030 SYMBOL
// _$E225
// SYNTHETIC: GAMEMD 0x00659050 SYMBOL
// _$E228

// SYNTHETIC: GAMEMD 0x00659070 SYMBOL
// _$E231
Rect DAT_00B04A48(0, 0, 0, 0);
