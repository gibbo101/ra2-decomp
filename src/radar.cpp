// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "multiplayer.h"
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
#include "zbuffer.h"
#include "vqa.h"
#include "beacon.h"
#include "fastmath.h"
#include "coord.h"

// SYNTHETIC: GAMEMD 0x00652660 SYMBOL
// _$E2

extern ObjectClass * __fastcall Best_Selected_Object(Cell const * cell, ObjectClass const * target);
extern bool __fastcall FUN_00637AA0();
extern int __fastcall FUN_00639DA0();

extern const double ONE_RAD;
extern const double DAT_007F02A8;
extern BeaconManagerClass BeaconManager;
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
void __fastcall Set_Frame_Rate_Limit(int value);

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
Rect BriefingButtonRect;

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

extern int SidebarWidth;
extern int RadarAnimShape;
extern char const * BriefingButtonShapeName;
extern char const * OptionsButtonShapeName;
extern void * BriefingButtonShape;
extern void * OptionsButtonShape;
extern bool IsBriefingButtonShapeLoaded;
extern bool IsOptionsButtonShapeLoaded;
extern Rect OptionsButtonRect;
extern int DAT_00B04A1C;
extern int DAT_00B04A38;

namespace Game { extern int SpecialDialog; }
extern int DAT_00886FA8;
extern int DAT_00B45B68;
bool __fastcall FUN_0053BAE0(void);
void __fastcall Remove_Speech_Hold(void);

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
	if (IsBriefingButtonShapeLoaded) {
		delete (char *)BriefingButtonShape;
		IsBriefingButtonShapeLoaded = false;
	}
	BriefingButtonShape = NULL;
	if (IsOptionsButtonShapeLoaded) {
		delete (char *)OptionsButtonShape;
		IsOptionsButtonShapeLoaded = false;
	}
	OptionsButtonShape = NULL;
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
		RadHeight = (SidebarWidth - 144) / 2 + 4;
		BriefingButtonRect.X = RadX + 11;
		BriefingButtonRect.Y = RadY + 4;
		OptionsButtonRect.X = RadX + 83;
	} else if (side == 1) {
		RadHeight = (SidebarWidth - 145) / 2 + 5;
		BriefingButtonRect.X = RadX + 14;
		BriefingButtonRect.Y = RadY + 5;
		OptionsButtonRect.X = RadX + 86;
	} else {
		RadHeight = (SidebarWidth - 145) / 2 + 5;
		BriefingButtonRect.X = RadX + 14;
		BriefingButtonRect.Y = RadY + 5;
		OptionsButtonRect.X = RadX + 86;
	}
	OptionsButtonRect.Y = BriefingButtonRect.Y;
	DAT_00B04A1C = RadHeight;

	BriefingButtonShape = FileSystem::LoadWholeFileEx(BriefingButtonShapeName, IsBriefingButtonShapeLoaded);
	OptionsButtonShape = FileSystem::LoadWholeFileEx(OptionsButtonShapeName, IsOptionsButtonShapeLoaded);
	BriefingButtonRect.Width = ((ShpHeader *)BriefingButtonShape)->Width;
	BriefingButtonRect.Height = ((ShpHeader *)BriefingButtonShape)->Height;
	OptionsButtonRect.Width = ((ShpHeader *)OptionsButtonShape)->Width;
	OptionsButtonRect.Height = ((ShpHeader *)OptionsButtonShape)->Height;
	DAT_00B04A38 = RadarAnimShape;

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
		RadarStatics::DAT_00B04978.Set_Position(BriefingButtonRect.X, BriefingButtonRect.Y);
		RadarStatics::DAT_00B04978.Set_Shape((ShapeSet const *)BriefingButtonShape, 0, 0);
		RadarStatics::DAT_00B04978.Flag_To_Redraw();

		RadarStatics::DAT_00B04910.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		RadarStatics::DAT_00B04910.ID = 243;
		RadarStatics::DAT_00B04910.DrawOnSidebar = true;
		RadarStatics::DAT_00B04910.IsOn = false;
		RadarStatics::DAT_00B04910.IsToggleType = 0;
		RadarStatics::DAT_00B04910.Set_Flags(5);
		RadarStatics::DAT_00B04910.Set_Position(OptionsButtonRect.X, OptionsButtonRect.Y);
		RadarStatics::DAT_00B04910.Set_Shape((ShapeSet const *)OptionsButtonShape, 0, 0);
		RadarStatics::DAT_00B04910.Flag_To_Redraw();
	}
}

extern void * RadarFrameShape;
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
		ShapeSet const * frame_shape = (ShapeSet const *)RadarFrameShape;
		Draw_Shape(*SidebarSurface, *FileSystem::SIDEBAR_PAL, frame_shape, 0, Point2D(RadX, RadY), SidebarSurface->Get_Rect());
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
					Draw_Shape(*SidebarSurface, *drawer, (ShapeSet const *)DAT_00B04A38, RadarAnimFrame, Point2D(RadX, RadWidth), SidebarSurface->Get_Rect());
					LastDrawRect = Rect(RadX, RadWidth, SidebarWidth, 110);
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
						Draw_Shape(*SidebarSurface, *drawer, (ShapeSet const *)DAT_00B04A38, 32, Point2D(RadX, RadWidth), SidebarSurface->Get_Rect());
						Draw_Names();
						LastDrawRect = Rect(RadX, RadWidth, SidebarWidth, 110);
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
					Draw_Shape(*SidebarSurface, *drawer, (ShapeSet const *)DAT_00B04A38, 0, Point2D(RadX, RadWidth), SidebarSurface->Get_Rect());
					LastDrawRect = Rect(RadX, RadWidth, SidebarWidth, 110);
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
		LastDrawRect = Union(LastDrawRect, BriefingButtonRect);
		DAT_00B0B518 = true;
	}

	if (DAT_00B04964) {
		LastDrawRect = Union(LastDrawRect, OptionsButtonRect);
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

	Rect r1 = BriefingButtonRect;
	r1.X += offset;
	if (r1.Is_Point_Within(xy)) {
		Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
	}

	Rect r2 = OptionsButtonRect;
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
		Update_Transmission();
	} else if (!FUN_0053BAE0()) {
		Remove_Speech_Hold();
	}
}

// FUNCTION: GAMEMD 0x006539D0
int RadarClass::RTacticalClass::Action(unsigned flags, KeyNumType & key, int modifier)
{
	if (flags & (RIGHTHELD | LEFTHELD)) {
		return(false);
	}
	if (Map.Is_Playing_Movie()) {
		return(false);
	}
	if (!Map.Is_Radar_Tactical()) {
		return(false);
	}

	int x, y;
	if (flags & (LEFTPRESS | LEFTRELEASE | RIGHTPRESS | RIGHTRELEASE)) {
		x = Keyboard->MouseQX;
		y = Keyboard->MouseQY;
	} else {
		x = Get_Mouse_X();
		y = Get_Mouse_Y();
	}
	x -= Options.IsSidebarOnRight ? TacticalRect.Width : 0;
	if (x < Map.RadarRect.X || x >= Map.RadarRect.X + Map.RadarRect.Width ||
	    y < Map.RadarRect.Y || y >= Map.RadarRect.Y + Map.RadarRect.Height) {
		Map.Override_Mouse_Shape(MOUSE_NORMAL);
		return(false);
	}

	Point2D click(x, y);
	Cell cell(0, 0);
	bool shadow;
	ObjectClass * object = NULL;
	ActionType action;
	Map.Resolve_Radar_Point(click, cell, object);
	if (cell != CELL_NONE) {
		Coord coord = cell.As_Coord();
		coord.Z = Map.Get_Height_GL(coord);
		shadow = (Map.Is_Shrouded(coord) && MainWindow);

		bool multi_missile = Map.IsTargettingMode == SUPER_MULTI_MISSILE;
		bool iron_curtain = Map.IsTargettingMode == SUPER_IRON_CURTAIN;
		bool lightning_storm = Map.IsTargettingMode == SUPER_LIGHTNING_STORM;
		bool para_drop = Map.IsTargettingMode == SUPER_PARA_DROP;
		bool amer_para_drop = Map.IsTargettingMode == SUPER_AMER_PARA_DROP;
		bool psychic_dominator = Map.IsTargettingMode == SUPER_PSYCHIC_DOMINATOR;
		bool spy_plane = Map.IsTargettingMode == SUPER_SPY_PLANE;
		bool genetic_converter = Map.IsTargettingMode == SUPER_GENETIC_CONVERTER;
		bool psychic_reveal = Map.IsTargettingMode == SUPER_PSYCHIC_REVEAL;
		bool right = (flags & (RIGHTPRESS | RIGHTRELEASE)) != 0;
		if (!right && (CurrentObject.Count() || multi_missile || iron_curtain || lightning_storm ||
	               para_drop || psychic_dominator || spy_plane || genetic_converter ||
	               psychic_reveal || amer_para_drop)) {
			if (multi_missile) {
				action = ACTION_NUKE_BOMB;
			} else if (iron_curtain) {
				action = ACTION_IRON_CURTAIN;
			} else if (lightning_storm) {
				action = ACTION_LIGHTNING_STORM;
			} else if (psychic_dominator) {
				action = ACTION_PSYCHIC_DOMINATOR;
			} else if (spy_plane) {
				action = ACTION_SPY_PLANE;
			} else if (genetic_converter) {
				action = ACTION_GENETIC_CONVERTER;
			} else if (psychic_reveal) {
				action = ACTION_PSYCHIC_REVEAL;
			} else if (para_drop) {
				action = ACTION_PARA_DROP;
			} else if (amer_para_drop) {
				action = ACTION_AMER_PARA_DROP;
			} else if (object) {
				action = Best_Selected_Object(NULL, object)->What_Action(object);
			} else {
				action = Best_Selected_Object(&cell, NULL)->What_Action(cell);
			}

			switch (action) {
				case ACTION_MOVE:
				case ACTION_NOMOVE:
				case ACTION_ENTER:
				case ACTION_ATTACK:
				case ACTION_HARVEST:
				case ACTION_CAPTURE:
				case ACTION_SABOTAGE:
				case ACTION_NUKE_BOMB:
				case ACTION_GUARD_AREA:
				case ACTION_LIGHTNING_STORM:
				case ACTION_PARA_DROP:
				case ACTION_AMER_PARA_DROP:
				case ACTION_PSYCHIC_DOMINATOR:
				case ACTION_SPY_PLANE:
				case ACTION_PSYCHIC_REVEAL:
					break;
				default:
					action = ACTION_NONE;
					object = NULL;
					break;
			}
			if ((!FUN_00637AA0() || FUN_00639DA0() == -1) && action != ACTION_NONE) {
				if (flags & LEFTUP) {
					Map.Mouse_Left_Up(cell, shadow, object, action, true);
				}
				if (flags & LEFTRELEASE && !drag_select_aborted) {
					Map.Mouse_Left_Release(Coord(cell), cell, object, action, true);
				}
				goto finish_input;
			}
		}
		if (!right) {
			Map.Set_Default_Mouse(MOUSE_NORMAL, true);
		}
		if ((flags & LEFTPRESS) || (flags & RIGHTPRESS)) {
			if (cell != Cell(0, 0)) {
				const int tactical_tile_width = 60;
				const int tactical_tile_height = 30;
				int side_edge = Map.PlayRect.Width - (TacticalRect.Width / tactical_tile_width + 2) / 2 - 1;
				int half_view_height = TacticalRect.Height / (2 * tactical_tile_height);
				int top_edge = half_view_height + Map.PlayRect.Width + 1;
				int bottom_edge = 2 * Map.PlayRect.Height - half_view_height + Map.PlayRect.Width - 1;
				int adjust;

				if ((cell.Y - cell.X) > side_edge) {
					adjust = (cell.Y - cell.X) - side_edge;
					cell.Y -= adjust;
					cell.X += adjust;
				}
				if ((cell.X - cell.Y) > side_edge - 1) {
					adjust = (cell.X - cell.Y) - side_edge + 1;
					cell.Y += adjust;
					cell.X -= adjust;
				}
				if (cell.X + cell.Y < top_edge) {
					adjust = top_edge - cell.Y - cell.X;
					cell.X += adjust;
					cell.Y += adjust;
				}
				if ((cell.X + cell.Y) > bottom_edge) {
					adjust = (cell.X + cell.Y) - bottom_edge;
					cell.X -= adjust;
					cell.Y -= adjust;
				}
				Coord coord = Map[cell].Cell_Coord();
				TacticalMap->Set_Tactical_Position(coord);
				Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
				if (DepthBuffer) {
					DepthBuffer->Set_Scroll(ZBUFFER_MAX);
				}
			}
		}
	}

finish_input:
	GadgetClass::Action(0, key, 0);
	return(true);
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
	Draw_Shape(*SidebarSurface, *drawer, (ShapeSet const *)DAT_00B04A38, 32, Point2D(RadX, RadWidth), SidebarSurface->Get_Rect());

	y = RadOffX + 2;
	Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), StringTable::LoadString("TXT_NAME_COLON", NULL, "D:\\ra2mdpost\\Radar.CPP", 1268), SidebarSurface, &SidebarSurface->Get_Rect(), &(Point2D const &)Point2D(RadHeight, y), Fetch_Scheme_By_Name("Grey", 1), 0, TextPrintType(0x19));
	Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), StringTable::LoadString("TXT_KILLS_COLON", NULL, "D:\\ra2mdpost\\Radar.CPP", 1270), SidebarSurface, &SidebarSurface->Get_Rect(), &(Point2D const &)Point2D(RadOffY + RadHeight - 2, y), Fetch_Scheme_By_Name("Grey", 1), 0, TextPrintType(0x219));

	y += 7;
	SidebarSurface->Draw_Line(Point2D(RadHeight, y), Point2D(RadOffY + RadHeight - 1, y), FileSystem::SIDEBAR_PAL->Convert_Pixel(14));
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
		Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), txt, SidebarSurface, &SidebarSurface->Get_Rect(), &(Point2D const &)Point2D(RadHeight, y), color, 0, TextPrintType(style));

		kills = 0;
		for (h = 0; h < 20; h++) {
			kills += ptr->UnitsKilled[h];
			kills += ptr->BuildingsKilled[h];
		}
		swprintf(txt, L"%2d", kills);
		Fancy_Text_Print_Wide((Point2D *)&(Point2D const &)Point2D(), txt, SidebarSurface, &SidebarSurface->Get_Rect(), &(Point2D const &)Point2D(RadOffY + RadHeight - 2, y), color, 0, TextPrintType(style | 0x200));

		y += 9;
	}
}

// FUNCTION: GAMEMD 0x00654320
void RadarClass::Reposition_Sidebar(void)
{
	BASECLASS::Reposition_Sidebar();

	int offset = Options.IsSidebarOnRight ? DAT_00886FA8 : 0;

	RadarStatics::DAT_00B04978.DrawOffset.X = -offset;
	RadarStatics::DAT_00B04978.Set_Position(BriefingButtonRect.X + offset, BriefingButtonRect.Y);
	RadarStatics::DAT_00B04978.Flag_To_Redraw();

	RadarStatics::DAT_00B04910.DrawOffset.X = -offset;
	RadarStatics::DAT_00B04910.Set_Position(OptionsButtonRect.X + offset, OptionsButtonRect.Y);
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

static inline void Radar_Pixel_Bounds(Rect const &rect, float zoom, int width, int height,
                                     int &left, int &right, int &top, int &bottom)
{
	left = (int)(rect.X * zoom);
	right = MIN((int)((rect.X + rect.Width) * zoom) + 1, width);
	top = (int)(rect.Y * zoom);
	bottom = MIN((int)((rect.Y + rect.Height) * zoom) + 1, height);
}

// FUNCTION: GAMEMD 0x006547C0
Rect RadarClass::Compute_Background(Rect const & cell_rect, Rect & update_rect, bool fill_in)
{
	if (fill_in && !update_rect.Is_Valid()) {
		update_rect = cell_rect;
	}
	if (BackgroundColors == NULL) {
		BackgroundColors = new RGBClass[cell_rect.Height * cell_rect.Width];
		memset(BackgroundColors, 0, cell_rect.Height * cell_rect.Width * sizeof(RGBClass));
		RadarCellWidth = cell_rect.Width;
		RadarCellHeight = cell_rect.Height;
	}
	if (fill_in) {
		Fill_In_Background(cell_rect, update_rect);
	}

	int cell_width = cell_rect.Width;
	float zoom_scale = ZoomFactor;
	int surface_width;
	int surface_height;
	if (BackgroundSurface == NULL) {
		zoom_scale = 140 / (float)cell_width;
		float scaled_height = cell_rect.Height * zoom_scale;
		if (scaled_height < 108) {
			surface_width = 140;
			surface_height = scaled_height;
		} else {
			zoom_scale = 108 / (float)cell_rect.Height;
			float scaled_width = cell_width * zoom_scale;
			surface_width = scaled_width;
			surface_height = 108;
		}
		ZoomFactor = zoom_scale;
		BackgroundSurface = new BSurface(surface_width, surface_height, 2);
		BackgroundSurface->Fill(TBLACK);
	} else {
		surface_width = BackgroundSurface->Get_Width();
		surface_height = BackgroundSurface->Get_Height();
	}

	float pixels_per_cell_x = (float)cell_rect.Width / (float)surface_width;
	float pixels_per_cell_y = (float)cell_rect.Height / (float)surface_height;
	float inv_area = 1.0 / (pixels_per_cell_x * pixels_per_cell_y);
	unsigned short *data = (unsigned short *)BackgroundSurface->Lock();
	int surf_left, surf_right, surf_top, surf_bottom;
	Radar_Pixel_Bounds(update_rect, zoom_scale, surface_width, surface_height, surf_left, surf_right, surf_top, surf_bottom);
	float src_x = surf_left / zoom_scale;
	float src_y = surf_top / zoom_scale;
	if (surf_top < surf_bottom) {
		for (int surfy = surf_top; surfy < surf_bottom; ++surfy) {
			int src_y_min = MIN(RadarCellHeight, (int)src_y);
			float src_y_end = src_y + pixels_per_cell_y;
			int src_y_max = MIN(RadarCellHeight, (int)src_y_end + 1);
			if (surf_left < surf_right) {
				unsigned short *pixel_data = data + surf_left + surface_width * surfy;
				for (int surfx = surf_left; surfx < surf_right; ++surfx) {
					float accum_r = 0.0f;
					float accum_g = 0.0f;
					float accum_b = 0.0f;
					int src_x_min = MIN(RadarCellWidth, (int)src_x);
					float src_x_end = src_x + pixels_per_cell_x;
					int src_x_max = MIN(RadarCellWidth, (int)src_x_end + 1);
					for (int srcy = src_y_min; srcy < src_y_max; ++srcy) {
						float y_weight;
						if (src_y_max - src_y_min <= 1) {
							y_weight = pixels_per_cell_y;
						} else if (srcy == src_y_min) {
							y_weight = (float)(srcy + 1) - src_y;
						} else if (srcy == src_y_max - 1) {
							y_weight = src_y_end - (float)srcy;
						} else {
							y_weight = 1.0f;
						}
						int idx = src_x_min + srcy * cell_width;
						for (int srcx = src_x_min; srcx < src_x_max; ++srcx, ++idx) {
							float x_weight;
							if (src_x_max - src_x_min <= 1) {
								x_weight = pixels_per_cell_x;
							} else if (srcx == src_x_min) {
								x_weight = (float)(srcx + 1) - src_x;
							} else if (srcx == src_x_max - 1) {
								x_weight = src_x_end - (float)srcx;
							} else {
								x_weight = 1.0f;
							}
							float weight = x_weight * y_weight * inv_area;
							if (idx < RadarCellHeight * RadarCellWidth) {
								RGBClass &color = BackgroundColors[idx];
								accum_r += (float)color.Get_Red() * weight;
								accum_g += (float)color.Get_Green() * weight;
								accum_b += (float)color.Get_Blue() * weight;
							}
						}
					}
					*pixel_data++ = DSurface::Build_Hicolor_Pixel(
						MIN(255, (int)(accum_r + 0.5)),
						MIN(255, (int)(accum_g + 0.5)),
						MIN(255, (int)(accum_b + 0.5)));
					src_x = src_x_end;
				}
			}
			src_x = surf_left / zoom_scale;
			src_y = src_y_end;
		}
	}

	BackgroundSurface->Unlock();
	int pixel_count = (surface_width * surface_height) / 8 + 1;
	if (PixelFlags == NULL) {
		PixelFlags = new unsigned char[pixel_count];
		memset(PixelFlags, 0, pixel_count);
	}
	Rect result(surf_left, surf_top, surf_right - surf_left, surf_bottom - surf_top);
	return result;
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
		Set_Frame_Rate_Limit(Rule->DetailMinFrameRateNormal);
	}
}

// FUNCTION: GAMEMD 0x00655C50
void RadarClass::Plot_Radar_Pixel(Point2D const & point)
{
	if (PlayerPtr == NULL || !RadarSurface->Get_Rect().Is_Point_Within(point)) {
		return;
	}

	Coord coord = Radar_Pixel_To_Cell(point);
	coord.Z = Map.Get_Height_GL(coord);
	bool shadow = MainWindow && Map.Is_Shrouded(coord);
	bool fog = MainWindow && Map.FUN_005864A0(coord);

	int hash = (point.X - 5 * point.Y) & 0xFF;
	RADAR_HASH_TABLE::BucketType & bucket = RadarTrackingTable->Buckets[hash];
	int count = bucket.Count();
	RADAR_HASH_TABLE::ObjectType * entries = &bucket[0];
	TechnoClass * techno = NULL;
	for (int index = 0; index < count; ++index) {
		RadarTrackingStruct & track = entries[index].Key;
		TechnoClass * candidate = track.Object;
		if (track.Position != point) continue;
		if (shadow && !candidate->House->Is_Player_Control()) continue;
		if (candidate->Techno_Type_Class()->IsStealthy && !PlayerPtr->Is_Ally(candidate)) continue;
		if (candidate->Techno_Type_Class()->IsInsignificant && !candidate->Techno_Type_Class()->IsRadarVisible &&
		    (candidate->House == NULL || candidate->House->Class->IsMultiplayPassive)) continue;

		techno = candidate;
		break;
	}

	if (techno != NULL) {
		HouseClass * house = techno->House;
		if (techno->IsDisguised()) {
			house = techno->GetDisguiseHouse(false);
		}
		int color = house ? DSurface::Build_Hicolor_Pixel(house->RemapColorRGB) : Fetch_Scheme_By_Name("LightGrey", 1)->Bright;
		int flash = techno->RadarFlashTimer;
		if (flash > 0 && (flash - 1) / Rule->FlashFrameTime % 2 == 1 && techno->House == PlayerPtr) {
			RadarSurface->Put_Pixel(point, ~color);
		} else {
			RadarSurface->Put_Pixel(point, color);
		}
	} else if (fog) {
		RGBClass rgb = DSurface::Deconstruct_Hicolor_Pixel(BackgroundSurface->Get_Pixel(point));
		RadarSurface->Put_Pixel(point, DSurface::Build_Hicolor_Pixel(rgb.Get_Red() / 2, rgb.Get_Green() / 2, rgb.Get_Blue() / 2));
	} else if (!shadow) {
		RadarSurface->Put_Pixel(point, BackgroundSurface->Get_Pixel(point));
	} else {
		RadarSurface->Put_Pixel(point, 0);
	}

	LastDrawRect = Union(LastDrawRect, Rect(point, 1, 1) + RadarRect.Top_Left());
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

// FUNCTION: GAMEMD 0x00656750
void RadarClass::Resolve_Radar_Point(Point2D const & point, Cell & cell, ObjectClass *& object)
{
	Point2D pt = point - RadarRect.TopLeft;
	RadarTrackingStruct track;
	track.Position = pt;

	RADAR_HASH_TABLE::BucketType & bucket = RadarTrackingTable->Buckets[track.Hash()];
	int count = bucket.Count();
	object = NULL;
	for (int index = count - 1; index >= 0; --index) {
		RadarTrackingStruct entry = bucket[index].Key;
		if (entry == track) {
			object = entry.Object;
			break;
		}
	}

	if (object != NULL) {
		cell = object->Destination_Coord();
	} else {
		cell = Radar_Pixel_To_Cell(pt);
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
void Add_Speech_Hold(void);
bool Is_Speech_Finished(int index);
void FUN_00408390(void);
int FUN_00408410(void);
void FUN_005BF580(void);

// FUNCTION: GAMEMD 0x00656EC0
void RadarClass::Render_Radar(void)
{
	int i;
	Rect prev_rect = LastDrawRect;
	bool is_tactical = Is_Radar_Tactical();

	Cell center = Map[TacticalMap->Pixel_To_Cell(Point2D(TacticalRect.X + TacticalRect.Width / 2, TacticalRect.Y + TacticalRect.Height / 2))].Fetch_CellID();

	RadarViewRect = Cell_To_Radar_Pixel(center);
	RadarViewRect.Width = ((2 * TacticalRect.Width) / (60 / ZoomFactor) + 1.0f);
	RadarViewRect.Height = ((2 * TacticalRect.Height) / (30 / ZoomFactor));
	RadarViewRect -= Point2D(TacticalRect.Width / (60 / ZoomFactor), (2 * TacticalRect.Height) / (30 / ZoomFactor) * 0.5f);

	if (RadarViewRect.X < RadarRect.X) {
		RadarViewRect.X = RadarRect.X;
	} else {
		if (RadarViewRect.X + RadarViewRect.Width >= RadarRect.X + RadarRect.Width) {
			RadarViewRect.X = RadarRect.X - (RadarViewRect.Width - RadarRect.Width) - 1;
		}
	}

	if (RadarViewRect.Y < RadarRect.Y) {
		RadarViewRect.Y = RadarRect.Y;
	} else {
		if (RadarViewRect.Y + RadarViewRect.Height >= RadarRect.Y + RadarRect.Height) {
			RadarViewRect.Y = RadarRect.Y - (RadarViewRect.Height - RadarRect.Height) - 1;
		}
	}

	if (FullRedraw) {
		OldRadarViewRect = RadarViewRect;
	}

	if (IsToRedraw || OldRadarViewRect != RadarViewRect || !No_Radar_Events_Submitted() || PixelStack.Count() > 0 || BackgroundStack.Count() > 0 || BeaconManager.RadarBeaconPulsing()) {
		if (is_tactical) {
			IsToRedraw = false;
		}

		Plot_Radar_Background();

		bool moved = false;
		if (OldRadarViewRect != RadarViewRect) {
			Point2D pt;
			for (i = 0; i < OldRadarViewRect.Height; i++) {
				pt = Point2D(OldRadarViewRect.X, i + OldRadarViewRect.Y) - Point2D(RadarRect.X, RadarRect.Y);
				Radar_Pixel(pt);
				pt = Point2D(OldRadarViewRect.X + OldRadarViewRect.Width - 1, i + OldRadarViewRect.Y) - Point2D(RadarRect.X, RadarRect.Y);
				Radar_Pixel(pt);
			}
			for (i = 0; i < OldRadarViewRect.Width; i++) {
				pt = Point2D(i + OldRadarViewRect.X, OldRadarViewRect.Y) - Point2D(RadarRect.X, RadarRect.Y);
				Radar_Pixel(pt);
				pt = Point2D(i + OldRadarViewRect.X, OldRadarViewRect.Height + OldRadarViewRect.Y - 1) - Point2D(RadarRect.X, RadarRect.Y);
				Radar_Pixel(pt);
			}
			moved = true;
		}

		if (CellRedrawRect.Is_Valid()) {
			RadarSurface->Blit_From(CellRedrawRect, *BackgroundSurface, CellRedrawRect);
			LastDrawRect = Union(LastDrawRect, CellRedrawRect + RadarRect.TopLeft);

			for (int x = CellRedrawRect.X; x < CellRedrawRect.X + CellRedrawRect.Width; x++) {
				for (int y = CellRedrawRect.Y; y < CellRedrawRect.Y + CellRedrawRect.Height; y++) {
					Plot_Radar_Pixel(Point2D(x, y));
				}
			}
			CellRedrawRect = Rect(0, 0, 0, 0);
		}

		// Render all pixels in the "to redraw" stack.
		for (i = PixelStack.Count() - 1; i >= 0; i--) {
			Plot_Radar_Pixel(PixelStack[i]);
		}
		PixelStack.Clear();

		RadarEventClass::Draw_Events();
		BeaconManager.DrawRadar(RadarSurface, RadarSurface->Get_Rect());

		if (is_tactical) {
			if (FullRedraw) {
				ConvertClass * drawer = FUN_0072F510();
				LastDrawRect = RadarRect;
				Draw_Shape(*SidebarSurface, *drawer, (ShapeSet const *)RadarAnim, 32, Point2D(RadX, RadWidth), SidebarSurface->Get_Rect());
			}

			if (LastDrawRect.Is_Valid()) {
				SidebarSurface->Blit_From(LastDrawRect, *RadarSurface, LastDrawRect - Point2D(RadarRect.X, RadarRect.Y));
			}

			SidebarSurface->Draw_Rect(RadarViewRect, RadPHeight);
			Rect area = RadarRect;
			Rect rr(area.X - 1, area.Y - 1, area.Width + 2, area.Height + 2);
			SidebarSurface->Draw_Rect(rr, RadPHeight);
		}

		if (moved) {
			Rect const & inner = Union(OldRadarViewRect, LastDrawRect);
			LastDrawRect = Union(RadarViewRect, inner);
		}

		OldRadarViewRect = RadarViewRect;
		RadarEventClass::Remove_Finished();
	}

	if (PixelFlags != NULL) {
		memset(PixelFlags, 0, RadarSurface->Get_Height() * RadarSurface->Get_Width() / 8 + 1);
	}

	if (!is_tactical) {
		LastDrawRect = prev_rect;
	}
}

// FUNCTION: GAMEMD 0x006578F0
void RadarClass::Update_Transmission(void)
{
	switch (unknown_14B8) {
		case 0:
			FUN_00752370();
			FUN_00752480(unknown_int_14D4, 0, 3);
			Add_Speech_Hold();
			unknown_14B8 = 1;
			break;
		case 1:
			if (Is_Speech_Finished(unknown_int_14D4)) {
				if (DAT_00B45B68) {
					FUN_00408390();
				}
				unknown_14B8 = 2;
			}
			break;
		case 2:
			if (DAT_00B45B68 == 0 || FUN_00408410()) {
				if (DAT_00B45B68) {
					Remove_Speech_Hold();
					FUN_005BF580();
				}
				Radar_Activate(3, false);
				unknown_14C0.EndLooping();
				unknown_14C0.SetEvent((AudioEvent *)VocClass::PlayGlobal(Rule->MovieOn, 1.0f, 0x2000), NULL);
				Set_Frame_Rate_Limit(Rule->DetailMinFrameRateMovie);
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
		Draw_Shape(*SidebarSurface, *drawer, (ShapeSet const *)DAT_00B04A38, 32, Point2D(RadX, RadWidth), SidebarSurface->Get_Rect());
		LastDrawRect = Rect(RadX, RadWidth, SidebarWidth, 110);
		DebugString("Radar: Movie full redrawn\n");
	}

	if (has) {
		if (IngameVQ.Count() > 0 && (movie = IngameVQ[0]) != NULL) {
			if (movie->AdvanceFrame() == true) {
				LastDrawRect = Rect(RadX, RadWidth, SidebarWidth, 110);
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
		Set_Frame_Rate_Limit(Rule->DetailMinFrameRateNormal);
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

extern HouseClass * DAT_00AC1198;
int FUN_004E3320(int side);
LRESULT FUN_00603DD0(HWND window, bool allied);
namespace UI
{
int StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
void FUN_00731840(int);
LRESULT FUN_00603DB0(HWND window, bool enabled);
extern wchar_t DAT_00887734[];
static const int DiplomacyNameIDs[8] = {1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048};
static const int DiplomacyKillsIDs[8] = {1049, 1050, 1051, 1052, 1053, 1054, 1055, 1056};
static const int DiplomacyAllianceIDs[8] = {-1, 1743, 1744, 1745, 1746, 1747, 1748, 1749};
static const int DiplomacyChatIDs[8] = {-1, 1750, 1751, 1752, 1753, 1762, 1763, 1764};

static inline void Clear_Diplomacy_Label(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)DAT_00887734);
}
static void Hide_Diplomacy_Control(HWND parent, int id)
{
	HWND control = GetDlgItem(parent, id);
	EnableWindow(control, FALSE);
	ShowWindow(control, SW_HIDE);
}
static int Find_Diplomacy_Control(int const (&controls)[8], int id)
{
	for (int i = 0; i < 8; ++i) {
		if (controls[i] == id) {
			return i;
		}
	}
	return -1;
}
static bool Is_Diplomacy_Control(int const (&controls)[8], int id)
{
	for (int i = 0; i < 8; ++i) {
		if (controls[i] == id) {
			return true;
		}
	}
	return false;
}
// FUNCTION: GAMEMD 0x00657DA0
LRESULT CALLBACK Diplomacy_Dialog_Proc(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
{
	int handled = UI::StandardWndProc(dialog, message, wparam, lparam);
	if (handled) {
		return handled;
	}
	switch (message) {
	case 0x497: {
		int slot;
		for (slot = 0; slot < 8; ++slot) {
			Clear_Diplomacy_Label(dialog, DiplomacyNameIDs[slot]);
			Clear_Diplomacy_Label(dialog, DiplomacyKillsIDs[slot]);
		}
		bool hasAllianceControls = false, hasChatControls = false;
		int count = Houses.Count();
		for (slot = 1; slot < 8; slot++) {
			int allianceID = DiplomacyAllianceIDs[slot];
			HWND allianceControl = GetDlgItem(dialog, allianceID);
			int chatID = DiplomacyChatIDs[slot];
			HWND chatControl = GetDlgItem(dialog, chatID);
			int houseIndex = slot;
			if (houseIndex == PlayerPtr->HeapID) {
				houseIndex = 0;
			}
			HouseClass * house;
			if (houseIndex < count && (house = Houses[houseIndex]) != NULL && house->Class->IsMultiplay) {
				if (!Rule->AlliesAllowed || DAT_00AC1198 == PlayerPtr || Session.unknown_0C == 2) {
					ShowWindow(allianceControl, SW_HIDE);
				} else {
					FUN_00603DB0(allianceControl, true);
					bool ally = PlayerPtr->Is_Ally(house);
					SendMessage(allianceControl, BM_SETCHECK, ally, 0);
					FUN_00603DD0(allianceControl, house->Is_Ally(PlayerPtr));
					hasAllianceControls = true;
					if (Unsorted::MuteSWLaunches || !house->FUN_0050B730()) {
						EnableWindow(allianceControl, FALSE);
					}
				}
				if (!house->FUN_0050B730()) {
					ShowWindow(chatControl, SW_HIDE);
				} else {
					if (!Unsorted::MuteSWLaunches) {
						SendMessage(chatControl, BM_SETCHECK, Session.PlayerChatEnabled[houseIndex], 0);
					} else {
						SendMessage(chatControl, BM_SETCHECK, 1, 0);
						EnableWindow(chatControl, 0);
					}
					hasChatControls = true;
				}
			} else {
				Hide_Diplomacy_Control(dialog, FUN_004E3320(slot));
				Hide_Diplomacy_Control(dialog, DiplomacyNameIDs[slot]);
				EnableWindow(allianceControl, 0);
				ShowWindow(allianceControl, SW_HIDE);
				EnableWindow(chatControl, 0);
				ShowWindow(chatControl, SW_HIDE);
				Hide_Diplomacy_Control(dialog, DiplomacyKillsIDs[slot]);
			}
		}
		if (!hasAllianceControls) {
			HWND heading = GetDlgItem(dialog, 0x597);
			ShowWindow(heading, SW_HIDE);
		}
		if (!hasChatControls) {
			HWND heading = GetDlgItem(dialog, 0x425);
			ShowWindow(heading, SW_HIDE);
		}
		HWND taunts = GetDlgItem(dialog, 0x730);
		if (taunts) {
			if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
				ShowWindow(taunts, SW_SHOW);
				if (Session.Type == GAME_IPX) {
					SendMessage(taunts, BM_SETCHECK, Session.LANTaunts, 0);
				} else if (Session.Type == GAME_INTERNET) {
					SendMessage(taunts, BM_SETCHECK, Session.WOLTaunts, 0);
				}
			} else {
				ShowWindow(taunts, SW_HIDE);
			}
		}
		Map.FUN_00658640(dialog);
		if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
			HWND modeLabel = GetDlgItem(dialog, 0x426);
			SendMessage(modeLabel, 0x4b2, 0, (LPARAM)Session.MPGameMode->UIName.text());
		}
		HWND label = GetDlgItem(dialog, 0x71B);
		SendMessage(label, 0x4b2, 0, (LPARAM)Session.Options.ScenarioDescription);
		SetTimer(dialog, 0, 250, NULL);
		break;
	}
	case WM_TIMER:
		Map.FUN_00658640(dialog);
		InvalidateRect(dialog, NULL, TRUE);
		break;
	case WM_COMMAND: {
		HWND control = (HWND)lparam;
		int id = GetDlgCtrlID(control);
		if (id == 0x423) {
			if (!HIWORD(wparam)) {
				KillTimer(dialog, 0);
				*(int *)GetWindowLong(dialog, DWL_USER) = 0x423;
			}
		} else if (Is_Diplomacy_Control(DiplomacyAllianceIDs, id)) {
			int code = HIWORD(wparam);
			if (code == 1 || code == 0) {
				int houseIndex = Find_Diplomacy_Control(DiplomacyAllianceIDs, id);
				if (houseIndex == PlayerPtr->HeapID) {
					houseIndex = 0;
				}
				if (code == 0) {
					SendMessage(control, BM_SETCHECK, 1, 0);
				} else if (code == 1) {
					SendMessage(control, BM_SETCHECK, 0, 0);
				}
				FUN_00731840(houseIndex);
			}
		} else if (Is_Diplomacy_Control(DiplomacyChatIDs, id)) {
			int code = HIWORD(wparam);
			if (code == 1 || code == 0) {
				bool listen = HIWORD(wparam) == 1;
				int houseIndex = Find_Diplomacy_Control(DiplomacyChatIDs, id);
				if (houseIndex == PlayerPtr->HeapID) {
					houseIndex = 0;
				}
				Session.PlayerChatEnabled[houseIndex] = listen;
			}
		} else if (id == 0x730) {
			int code = HIWORD(wparam);
			if (code == 1 || code == 0) {
				if (Session.Type == GAME_IPX) {
					Session.LANTaunts = code == 1;
				} else if (Session.Type == GAME_INTERNET) {
					Session.WOLTaunts = code == 1;
				}
			}
		}
		break;
	}
	}
	return 0;
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

// FUNCTION: GAMEMD 0x006586D0
void RadarClass::FUN_006586D0(void)
{
	int result;

	HWND dialog = UI::BeginDialog(0x73, Diplomacy_Dialog_Proc, 0);
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
