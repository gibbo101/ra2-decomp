// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tactical.h"
#include "_alpha.h"
#include "_convert.h"
#include "_font.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "bitfont.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "coord.h"
#include "data.h"
#include "dialog.h"
#include "display.h"
#include "draw.h"
#include "dsurface.h"
#include "fog.h"
#include "font.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "ionblast.h"
#include "isotype.h"
#include "laser.h"
#include "layer.h"
#include "mixfile.h"
#include "mouse.h"
#include "overtype.h"
#include "ovrlight.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "stimer.h"
#include "sun.h"
#include "swap.h"
#include "swizzle.h"
#include "terrain.h"
#include "terrtype.h"
#include "vector.h"
#include "vein.h"
#include "waypoint.h"
#include "zbuffer.h"
#include "ramp.hh"
#include "scrspeed.hh"
#include "fastmath.h"

extern Rect TacticalDimensions;
void __fastcall FUN_00731D00(int mode);
extern ObjectClass const * DAT_00880978;
extern int DAT_0088097C;
extern int DAT_00880980;
namespace Unsorted { extern bool ArmageddonMode; }
namespace FileSystem { extern ShapeSet * GRFXTXT_SHP; extern ConvertClass * GRFXTXT_Convert; }
Point2D Lerp(Point2D const & a, Point2D const & b, float t);

DynamicVectorClass<DirtyAreaStruct> Tactical::DirtyAreas;

Tactical::Selectable Tactical::SelectableObjects[Tactical::MAX_SELECTABLE_OBJECTS];

DynamicVectorClass<ShadowControlClass *> Tactical::ShadowControls;

static double PixelsPerLepton;
static double ZPixelsPerLepton = fastmath::sin(RAD_60) * (PixelsPerLepton = 60.0 / CELL_LEPTON_DIAG);
static double ZLeptonsPerPixel = CELL_LEPTON_DIAG / (fastmath::sin(RAD_60) * 60.0);

// FUNCTION: 0x006D1A60 $E198

// FUNCTION: 0x006D1AA0 $E195

// FUNCTION: 0x006D1B00 $E201

// FUNCTION: 0x006D1B10 $E207

// FUNCTION: 0x006D1B50 $E204

// FUNCTION: 0x006D1BB0 $E210

// FUNCTION: 0x006D1BF0 $E213

// FUNCTION: 0x006D1C20
Tactical::Tactical(void) :
	LastAIFrame(-1),
	Flags(false, false),
	TacPixelX(0),
	TacPixelY(0),
	LastTacPixelX(0),
	LastTacPixelY(0),
	ZoomFactor(1.0),
	SelectableCount(0),
	MoveFrom(COORD_NONE),
	MoveTo(COORD_NONE),
	MoveSpeed(0),
	MoveFactor(0),
	CellRedrawCount(0),
	TacticalCoord(0,0),
	LastTacticalCoord(0,0),
	DesiredTacticalCoord(0,0),
	IsFirstRender(true),
	IsToRedraw(false),
	UnusedBool(false),
	VisibleCellRect(RECT_NONE),
	RubberBandStart(0,0),
	RubberBandEnd(0,0),
	WaypointAnimCounter(0),
	WaypointAnimTimer()
{
	for (int i = 0; i < ARRAY_SIZE(CellRedraw); i++) {
		CellRedraw[i] = NULL;
	}

	ScreenText[0] = '\0';
	EndGameGraphicsFrame = -1;

	CoordToPixelMatrix.Make_Identity();
	CoordToPixelMatrix.Rotate_X(RAD_60);
	CoordToPixelMatrix.Rotate_Z(RAD_45);
	CoordToPixelMatrix.Scale(60.0f / (float)CELL_LEPTON_DIAG);

	float x_scale = 8.5334f;
	float y_scale = 4.2667f;
	PixelToCoordMatrix.Set(
		 y_scale, x_scale, 0.0f, 0.0f,
		-y_scale, x_scale, 0.0f, 0.0f,
		 0.0f,    0.0f,    1.0f, 0.0f);

	TacticalMap = this;
}

// FUNCTION: 0x006D1E30
Tactical::Tactical(const NoInitClass &x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x006D1E80
Tactical::~Tactical(void)
{
	TacticalMap = NULL;
}

// FUNCTION: 0x006D1EB0
Point2D Tactical::Rectangular_To_Isometric(Point2D point)
{
	Point2D iso;
	iso.X = point.X * 60 / 2;
	iso.Y = point.X * 30 / 2;
	iso.X += point.Y * 60 / -2;
	iso.Y += point.Y * 30 / 2;
	return(iso);
}

static int Lepton_To_Z_Pixel(LEPTON lepton)
{
	int fudge = 0;
	if (lepton >= 728) {
		fudge = 1;
	}
	return(int(lepton * ZPixelsPerLepton + fudge + 0.5));
}

// FUNCTION: 0x006D1F10
Point2D Tactical::Coord_To_Pixel_Absolute(Coord const & coord)
{
	Point2D pixel = Rectangular_To_Isometric(Point2D(coord.X, coord.Y));
	pixel.X /= 256;
	pixel.Y /= 256;
	pixel.Y -= Lepton_To_Z_Pixel(coord.Z);
	return(pixel);
}

// FUNCTION: 0x006D1FE0
Point2D Tactical::Coord_To_Pixel_Absolute(Point2D const & point)
{
	Point2D pixel = Rectangular_To_Isometric(point);
	pixel.X /= 256;
	pixel.Y /= 256;
	return(pixel);
}

// FUNCTION: 0x006D2070
Point2D Tactical::Pixel_To_Lepton(Point2D const & pixel)
{
	Vector3 vector(pixel.X, pixel.Y, 0.0);
	vector = PixelToCoordMatrix * vector;
	return(Point2D(vector.X, vector.Y));
}

// FUNCTION: 0x006D20E0
int Tactical::Z_Lepton_To_Pixel(LEPTON lepton)
{
	return(Lepton_To_Z_Pixel(lepton));
}

// FUNCTION: 0x006D2120
LEPTON Tactical::Pixel_To_Z_Lepton(int pixel)
{
	return(LEPTON(ZLeptonsPerPixel * (pixel - 0.5)));
}

// FUNCTION: 0x006D2140
bool Tactical::Coord_To_Pixel(Coord const & coord, Point2D & pixel)
{
	pixel = Coord_To_Pixel_Absolute(coord);
	pixel -= Point2D(TacPixelX, TacPixelY);

	if (pixel.X < -360) {
		return(false);
	}

	if (pixel.X > TacticalDimensions.Width + 360) {
		return(false);
	}

	if (pixel.Y < -180) {
		return(false);
	}

	if (pixel.Y > TacticalDimensions.Height + 180) {
		return(false);
	}

	return(true);
}

// FUNCTION: 0x006D2280
Coord Tactical::Pixel_To_Coord(Point2D const & pixel)
{
	if (pixel.X < TacticalDimensions.Width && pixel.Y < TacticalDimensions.Height + TacticalDimensions.Y) {
		Point2D lepton = Pixel_To_Lepton(pixel + Point2D(TacPixelX, TacPixelY));
		return(Coord(lepton.X, lepton.Y, 0));
	}

	return(COORD_NONE);
}

// FUNCTION: 0x006D2360
Coord Tactical::Pixel_To_Coord_Absolute(Point2D const & pixel)
{
	if (pixel.X < TacticalDimensions.Width && pixel.Y < TacticalDimensions.Height) {
		Point2D lepton = Pixel_To_Lepton(pixel);
		return(Coord(lepton.X, lepton.Y, 0));
	}

	return(COORD_NONE);
}

// FUNCTION: 0x006D2420
void Tactical::Setup_Trigger_Scroll(Coord const & coord, int speed)
{
	static float _speeds[SCROLL_SPEED_COUNT] = {
		0.0015f,
		0.0030f,
		0.0075f,
		0.0300f,
		0.0600f
	};

	MoveFactor = 0;
	MoveSpeed = _speeds[speed];

	Point2D to;
	Coord_To_Pixel(coord, to);
	to.X += TacPixelX;
	to.Y += TacPixelY;
	MoveTo = to;
	MoveFrom = TacticalCoord;
}

// FUNCTION: 0x006D2540
void Tactical::AI(void)
{
	bool coordchanged = false;

	if (Session.Play) {
		return;
	}

	if (LastAIFrame != Frame) {
		if (ScenarioActive) {
			if (MoveTo != COORD_NONE && MoveSpeed != 0) {
				MoveFactor += MoveSpeed;
				if (MoveFactor > 1.0) {
					MoveFactor = 1.0;
				}

				Point2D move = Lerp(MoveFrom, MoveTo, MoveFactor);

				if (MoveFactor >= 1.0) {
					MoveTo = Point2D(0,0);
					MoveFrom = Point2D(0,0);
					MoveSpeed = 0;
					MoveFactor = 0;
				}

				Set_Tactical_Position(move);
				coordchanged = true;
			}
		}

		if (WaypointAnimTimer == 0) {
			WaypointAnimTimer = Rule->WaypointAnimationSpeed;
			WaypointAnimCounter++;
		}
	}

	LastAIFrame = Frame;

	if (DesiredTacticalCoord != TacticalCoord && !coordchanged) {
		Set_Tactical_Position(DesiredTacticalCoord);
	}
}

// FUNCTION: 0x006D2790
void Tactical::Register_Dirty_Area(Rect area, bool refresh_shroud)
{
	int best_index = -1;
	int best = 0x7FFFFFFF;

	for (int i = 0; i < DirtyAreas.Count(); i++) {
		Rect rect = DirtyAreas[i].Area;
		Rect inter = Intersect(rect, area);
		Rect whole = Union(rect, area);

		if (inter.Is_Valid()) {
			if (inter == area) {
				if (!refresh_shroud || DirtyAreas[i].IsToRefreshShroud) {
					return;
				}
			} else if (inter == rect) {
				if (refresh_shroud || !DirtyAreas[i].IsToRefreshShroud) {
					DirtyAreas.Delete_Index(i);
					i--;
				}
			} else {
				int size = whole.Size();
				if (size < (rect.Size() + area.Size()) && size < best) {
					best = size;
					best_index = i;
				}
			}
		}
	}

	if (best_index == -1) {
		DirtyAreas.Add(DirtyAreaStruct(area, refresh_shroud));
	} else {
		DirtyAreas[best_index] = DirtyAreaStruct(Union(area, DirtyAreas[best_index].Area), refresh_shroud || DirtyAreas[best_index].IsToRefreshShroud);
	}
}

// FUNCTION: 0x006D2B50
void Tactical_noop(int, int, int, int)
{
}

// FUNCTION: 0x006D2B60
void Tactical::Wipe_Depth(bool fullredraw, int xoff, int yoff, Rect const & cliprect)
{
	if (!fullredraw && DirtyAreas.Count()) {
		for (int i = DirtyAreas.Count() - 1; i >= 0; i--) {
			Rect redraw = DirtyAreas[i].Area;
			redraw += Point2D(xoff, yoff);
			redraw = Intersect(TacticalRect - TacticalRect.TopLeft, redraw);
			DirtyAreas[i].Area = redraw;

			if (!redraw.Is_Valid()) {
				DirtyAreas.Delete_Index(i);
			} else {
				DepthBuffer->Update(redraw);
			}
		}
	}

	if (CellRedrawCount > 0 && !fullredraw) {
		for (int i = 0; i < CellRedrawCount; i++) {
			Coord coord = CellRedraw[i]->Cell_Coord();
			coord.Z = 0;
			Point2D pixel;
			Coord_To_Pixel(coord, pixel);
			Point2D at = TacticalRect.TopLeft + (pixel + Point2D(-30, -15));

			if (MainWindow) {
				CellRedraw[i]->Wipe_Depth(at, cliprect);
			}
		}
	}
}

// FUNCTION: 0x006D2DE0
void Tactical::Render_Tiles(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw)
{
	if (fullredraw) {
		Draw_Tiles(TacticalRect, TacticalRect);
	} else {
		if (xpanrect.Is_Valid()) {
			Draw_Tiles(xpanrect, xpanrect);
		}
		if (ypanrect.Is_Valid()) {
			Draw_Tiles(ypanrect, ypanrect);
		}
	}

	if (CellRedrawCount > 0 && !fullredraw) {
		for (int i = 0; i < CellRedrawCount; i++) {
			Coord coord = Coord_Whole(Coord(CellRedraw[i]->Fetch_CellID()));
			Point2D pixel;
			Coord_To_Pixel(coord, pixel);
			Rect rect;
			(Point2D &)rect = Point2D(pixel.X - 30, pixel.Y);
			((Point2D *)&rect)[1] = Point2D(60, 30);
			rect.Y += TacticalRect.Y;

			Rect inter = Intersect(TacticalRect, rect);

			if (inter.Is_Valid()) {
				Draw_Tiles(CellRedraw[i]->Fetch_CellID(), rect);
			}
		}
	}

	if (DirtyAreas.Count() && !fullredraw) {
		for (int j = DirtyAreas.Count() - 1; j >= 0; j--) {
			Rect redraw = DirtyAreas[j].Area;
			redraw.Y += TacticalRect.Y;
			redraw = Intersect(TacticalRect, redraw);

			if (redraw.Is_Valid()) {
				Draw_Tiles(redraw, redraw);
			}
		}
	}
}

// FUNCTION: 0x006D3290
void Tactical::Render_Overlays(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw)
{
	if (fullredraw) {
		Draw_Overlays(TacticalRect);
	} else {
		if (xpanrect.Is_Valid()) {
			Draw_Overlays(xpanrect);
		}
		if (ypanrect.Is_Valid()) {
			Draw_Overlays(ypanrect);
		}
	}

	if (CellRedrawCount > 0 && !fullredraw) {
		for (int i = 0; i < CellRedrawCount; i++) {
			Coord coord = Coord_Whole(Coord(CellRedraw[i]->Fetch_CellID()));
			Point2D pixel;
			Coord_To_Pixel(coord, pixel);

			Rect rect;
			rect.X = pixel.X - 30;
			rect.Width = 60;
			rect.Height = 30;
			rect.Y = pixel.Y;
			Rect rect2 = rect;
			rect2.Y += TacticalRect.Y;

			Draw_Overlays(rect2);
		}
	}

	if (DirtyAreas.Count()) {
		for (int i = DirtyAreas.Count() - 1; i >= 0; i--) {
			Rect redraw = DirtyAreas[i].Area;
			redraw.Y += TacticalRect.Y;
			redraw = Intersect(TacticalRect, redraw);

			if (redraw.Is_Valid()) {
				Draw_Overlays(redraw);
			}
		}
	}
}

static inline void Fogged_Rect(Rect const & r)
{
	::Draw_Fogged_Objects(r);
}

// FUNCTION: 0x006D3470
void Tactical::Render_Fogged_Objects(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw)
{
	if (fullredraw) {
		::Draw_Fogged_Objects(TacticalRect);
	} else {
		if (xpanrect.Is_Valid()) {
			::Draw_Fogged_Objects(xpanrect);
		}
		if (ypanrect.Is_Valid()) {
			::Draw_Fogged_Objects(ypanrect);
		}
	}

	if (CellRedrawCount > 0 && !fullredraw) {
		for (int i = 0; i < CellRedrawCount; i++) {
			Coord coord = Coord_Whole(Coord(CellRedraw[i]->Fetch_CellID()));
			Point2D pixel;
			Coord_To_Pixel(coord, pixel);

			Rect rect;
			rect.X = pixel.X - 30;
			rect.Width = 60;
			rect.Height = 30;
			rect.Y = pixel.Y;
			Rect rect2 = rect;
			rect2.Y += TacticalRect.Y;

			Fogged_Rect(rect2);
		}
	}

	if (DirtyAreas.Count()) {
		for (int i = DirtyAreas.Count() - 1; i >= 0; i--) {
			Rect redraw = DirtyAreas[i].Area;
			redraw.Y += TacticalRect.Y;
			redraw = Intersect(TacticalRect, redraw);

			if (redraw.Is_Valid()) {
				::Draw_Fogged_Objects(redraw);
			}
		}
	}
}

// FUNCTION: 0x006D3660
void Tactical::Render_Shroud(Rect const & xpanrect, Rect const & ypanrect, Rect const & cliprect, bool fullredraw)
{
	if (CellRedrawCount > 0 && !fullredraw) {
		for (int i = 0; i < CellRedrawCount; i++) {
			Coord coord = CellRedraw[i]->Cell_Coord();
			coord.Z = 0;
			Point2D raw;
			Coord_To_Pixel(coord, raw);
			Point2D pixel = raw - Point2D(30, 15) + TacticalRect.TopLeft;

			if (MainWindow) {
				CellRedraw[i]->Draw_Shroud_And_Fog(pixel, cliprect);
			}
			AlphaShapeClass::Draw_In_Area(pixel, cliprect);
		}
	} else if (fullredraw) {
		Draw_Shroud(TacticalRect);
	}

	if (xpanrect.Is_Valid() && !fullredraw) {
		Draw_Shroud(xpanrect);
	}
	if (ypanrect.Is_Valid() && !fullredraw) {
		Draw_Shroud(ypanrect);
	}

	for (int i = 0; i < DirtyAreas.Count(); i++) {
		Rect redraw = DirtyAreas[i].Area + TacticalRect.TopLeft;
		Rect inter = Intersect(redraw, cliprect);

		if (DirtyAreas[i].IsToRefreshShroud) {
			AlphaBuffer->Update(inter);
			Draw_Shroud(inter);
		}
	}
}

BitFont * __fastcall FUN_004A60D0(int flags);
namespace Game { unsigned __int64 __fastcall AudioGetTime(void); }

struct TimerFlashSource {
	int Unused[5];
	ColorScheme * Scheme;
};
extern TimerFlashSource * DAT_00B054D4;

static inline Point2D * Print_Timer(wchar_t const * text, Point2D const & pt, ColorScheme * scheme)
{
	Point2D ret;
	return Fancy_Text_Print_Wide(&ret, text, DSurface::Composite, &DSurface::Composite->Get_Rect(), &pt, scheme, 0, TextPrintType(0x4618));
}

// FUNCTION: 0x006D4B50
int Tactical::DrawTimer(int index, ColorScheme * scheme, int time, wchar_t const * text, unsigned __int64 * flashtime, bool * flashstate)
{
	wchar_t buf1[256];
	wchar_t buf2[256];
	wchar_t buf3[256];
	BitFont * font = FUN_004A60D0(0x4618);

	int total_minutes = time / 60;
	int hours = total_minutes / 60;
	int seconds = time % 60;
	int minutes = total_minutes % 60;

	if (hours) {
		swprintf(buf1, L"%s  %d:%02d:%02d", text, hours, minutes, seconds);
		swprintf(buf2, L"%s  ", text);
		swprintf(buf3, L"%d:%02d:%02d", hours, minutes, seconds);
	} else {
		swprintf(buf1, L"%s  %02d:%02d", text, minutes, seconds);
		swprintf(buf2, L"%s  ", text);
		swprintf(buf3, L"%02d:%02d", minutes, seconds);
	}

	int width = 0;
	font->GetTextDimension(buf3, &width, NULL, TacticalRect.Width);

	ColorScheme * flash = scheme;
	if (time == 0 && flashtime && flashstate) {
		if (*flashtime <= Game::AudioGetTime()) {
			*flashtime = Game::AudioGetTime() + 1000;
			*flashstate = !*flashstate;
		}
		if (*flashstate) {
			flash = DAT_00B054D4->Scheme;
		}
	}

	Point2D pt1(TacticalRect.Width - width - 3, TacticalRect.Height - (font->field_1C + 2) * (index + 1));
	Print_Timer(buf2, pt1, scheme);

	Point2D pt2(TacticalRect.Width - 3, TacticalRect.Height - (font->field_1C + 2) * (index + 1));
	return (int)Print_Timer(buf3, pt2, flash);
}

// FUNCTION: 0x006D4DB0
void Tactical::Set_Caption_Text(wchar_t const * text, int frame)
{
	if (!text) {
		Clear_Caption_Text();
	} else {
		wcscpy(ScreenText, text);
	}
	EndGameGraphicsFrame = frame;
}

// FUNCTION: 0x006D4E00
void Tactical::Clear_Caption_Text(void)
{
	EndGameGraphicsFrame = -1;
	ScreenText[0] = L'\0';
}

// FUNCTION: 0x006D4E20
void Tactical::Draw_Screen_Text(wchar_t const * text)
{
	if (Unsorted::ArmageddonMode) {
		return;
	}
	if (text == NULL || wcslen(text) == 0) {
		if (EndGameGraphicsFrame == -1) {
			return;
		}
	}
	if (!DSurface::Composite->Is_Direct_Draw()) {
		return;
	}

	int frame = EndGameGraphicsFrame;
	if (frame != -1 && FileSystem::GRFXTXT_Convert && FileSystem::GRFXTXT_SHP && FileSystem::GRFXTXT_SHP->Get_Count() > frame) {
		Point2D point((TacticalRect.Width - FileSystem::GRFXTXT_SHP->Get_Width()) / 2, (TacticalRect.Height - FileSystem::GRFXTXT_SHP->Get_Height()) / 2);
		Draw_Shape(*DSurface::Composite, *FileSystem::GRFXTXT_Convert, FileSystem::GRFXTXT_SHP, frame, point, TacticalRect);
		return;
	}

	if (text && wcslen(text) > 0) {
		Rect rect = TacticalRect;
		unsigned color = DSurface::Build_Hicolor_Pixel(255, 255, 255);
		BitFont * font = BitFont::Instance;
		int width;
		int height;
		font->GetTextDimension(text, &width, &height, rect.Width);

		int x = rect.X + (rect.Width - width) / 2;
		int y = rect.Y + (rect.Height - height) / 2;
		LTRBStruct bounds = { rect.X, rect.Y, rect.X + rect.Width, rect.Y + rect.Height };
		font->SetClipMode(true);
		font->SetBounds(&bounds);
		font->SetColor(color);
		BitText::Instance->Print(font, DSurface::Composite, text, x, y, 0, 0);
	}
}

// FUNCTION: 0x006D5F30
bool Tactical::Blit_Tiles(void)
{
	return(LogicalSurface->Blit_From(TacticalRect, *TileSurface, TacticalRect));
}

// FUNCTION: 0x006D5F60
void Tactical::Set_View_Dimensions(Rect const & dimensions)
{
	TacticalDimensions = dimensions;
	Set_Tactical_Position(DesiredTacticalCoord);
}

// FUNCTION: 0x006D6000
void Tactical::Set_Tactical_Position(Point2D const & point)
{
	Point2D clamped = point;
	Point2D result;
	if (Clamp_To_Tactical_Rect(clamped) && !Debug_Map) {
		result = clamped;
	} else {
		result = point;
	}
	TacticalCoord = result;
	DesiredTacticalCoord = result;
	Update_Visible_Cells();
	IsToRedraw = true;
}

// FUNCTION: 0x006D6170
Point2D Tactical::Get_Tactical_Position(void)
{
	return(TacticalCoord);
}

// FUNCTION: 0x006D6190
Point2D Tactical::Get_Relative_Tactical_Position(void)
{
	Point2D tac_offset = Point2D(TacPixelX, TacPixelY);
	return(TacticalCoord - tac_offset);
}

// FUNCTION: 0x006D61C0
void Tactical::Rectangular_To_Isometric(int xin, int yin, int & xout, int & yout)
{
	xout = xin * 60 / 2;
	yout = xin * 30 / 2;
	xout += yin * 60 / -2;
	yout += yin * 30 / 2;
}

// FUNCTION: 0x006D6220
void Tactical::Isometric_To_Rectangular(int xin, int yin, int & xout, int & yout)
{
	xout = ((60 / 2) * yin + (30 / 2) * xin) / 576 - 65536;
	yout = ((60 / 2) * yin - (30 / 2) * xin) / 576 + 65536;
}

// FUNCTION: 0x006D6280
void Tactical::Cell_To_Rectangular(int xin, int yin, int & xout, int & yout)
{
	int xsh = xin << 8;
	int ysh = yin << 8;
	xout = ((60 / 2) * ysh + (30 / 2) * xsh) / 576 - 65536;
	yout = ((60 / 2) * ysh - (30 / 2) * xsh) / 576 + 65536;
}

// FUNCTION: 0x006D62E0
void Tactical::Lepton_To_Map_Pixel(int xin, int yin, int & xout, int & yout)
{
	Rectangular_To_Isometric(xin, yin, xout, yout);

	xout = xout / 256;
	yout = yout / 256;

	xout += (60 / 2) * 512;
}

// FUNCTION: 0x006D6370
void Tactical::Cell_To_Rectangular_Centered(int xin, int yin, int & xout, int & yout)
{
	static int xoff = -1;
	static int yoff = -1;

	int x, y;
	Cell_To_Rectangular(xin, yin, x, y);

	if (xoff == -1 && yoff == -1) {
		xoff = -65536;
		yoff = 65536;
	}

	x -= xoff;
	y -= yoff;

	xout = x;
	yout = y;
}

// FUNCTION: 0x006D6A90
int Tactical::Get_Cell_Height(Coord const & coord)
{
	CellClass *cellptr = &Map[coord];
	return(cellptr->Get_Height(coord));
}

// FUNCTION: 0x006D6AD0
int Tactical::Get_Cell_Ramp(Coord const & coord)
{
	return(Map[coord].Ramp);
}

// FUNCTION: 0x006D6AF0
void Tactical::Add_Shadow_Control(ShadowControlClass control)
{
	ShadowControlClass *ctrl = new ShadowControlClass(control);
	ShadowControls.Add(ctrl);
}

// FUNCTION: 0x006D6BB0
CellClass * Tactical::Find_Bridge_Owner_Cell(Cell const & cell)
{
	CellClass * cellptr;
	Cell scan;
	int i;

	// Scan south along the two columns an east-west span could reach this cell from.
	scan = cell + Cell(2, 0);
	for (i = 0; i < 5; ++i) {
		cellptr = &Map[scan];
		if (cellptr->IsBridgeDeck && cellptr->Overlay && cellptr->IsBridgeEastWest) {
			return(cellptr);
		}
		scan.Y++;
	}

	scan = cell + Cell(1, 0);
	for (i = 0; i < 5; ++i) {
		cellptr = &Map[scan];
		if (cellptr->IsBridgeDeck && cellptr->Overlay && cellptr->IsBridgeEastWest) {
			return(cellptr);
		}
		scan.Y++;
	}

	// Then scan east along the two rows a north-south span could reach it from.
	scan = cell + Cell(0, 2);
	for (i = 0; i < 5; ++i) {
		cellptr = &Map[scan];
		if (cellptr->IsBridgeDeck && cellptr->Overlay && !cellptr->IsBridgeEastWest) {
			return(cellptr);
		}
		scan.X++;
	}

	scan = cell + Cell(0, 1);
	for (i = 0; i < 5; ++i) {
		cellptr = &Map[scan];
		if (cellptr->IsBridgeDeck && cellptr->Overlay && !cellptr->IsBridgeEastWest) {
			return(cellptr);
		}
		scan.X++;
	}

	return(NULL);
}

// FUNCTION: 0x006D84D0
Point2D Tactical::Clamp_Pixel_To_Tactical(Point2D const & pixel)
{
	Point2D clamped = pixel;
	if (Clamp_To_Tactical_Rect(clamped) && !Debug_Map) {
		return(clamped);
	} else {
		return(pixel);
	}
}

// FUNCTION: 0x006D8530
void Tactical::Scroll_Map(FacingType facing, int distance)
{
	static Point2D _scroll[FACING_COUNT] = 	{
		Point2D(0, -1),
		Point2D(1, -1),
		Point2D(1, 0),
		Point2D(1, 1),
		Point2D(0, 1),
		Point2D(-1, 1),
		Point2D(-1, 0),
		Point2D(-1, -1)
	};

	int & old_x = TacticalCoord.X;
	int & old_y = TacticalCoord.Y;

	int new_x = old_x + distance * _scroll[facing].X;
	int new_y = old_y + distance * _scroll[facing].Y;

	if (old_x != new_x || old_y != new_y) {
		new_x = new_x - old_x;
		new_y = new_y - old_y;
		DesiredTacticalCoord.X += new_x;
		DesiredTacticalCoord.Y += new_y;
	}
}

// FUNCTION: 0x006D8630 $E216

// FUNCTION: 0x006D8640
bool Tactical::Clamp_To_Tactical_Rect(Point2D & pixel)
{
	int xmin = TacticalRect.Width / 2 - (60 >> 1) * (Map.PlayRect.Width - 2 * Map.LocalRect.X);
	int xmax = xmin + 60 * Map.LocalRect.Width - TacticalRect.Width;

	int ymin = TacticalRect.Height / 2 + (30 >> 1) * (Map.PlayRect.Width + 2 * Map.LocalRect.Y - 5);
	int ymax = ymin + 30 * (2 * Map.LocalRect.Height + 9) / 2 - TacticalRect.Height;

	bool clamped = false;

	if (pixel.Y < ymin) {
		clamped = true;
		pixel.Y = ymin;
	} else if (pixel.Y > ymax) {
		pixel.Y = ymax;
		clamped = true;
	}

	if (pixel.X < xmin) {
		pixel.X = xmin;
		clamped = true;
	} else if (pixel.X > xmax) {
		pixel.X = xmax;
		clamped = true;
	}

	return(clamped);
}

// FUNCTION: 0x006D8700
int Tactical::Cell_Shadow(Cell const & cell, bool fog)
{
	static char const _shadow[1 << FACING_COUNT]={
		-1,33, 2, 2,34,37, 2, 2,
		 4,26, 6, 6, 4,26, 6, 6,
		35,45,17,17,38,41,17,17,
		 4,26, 6, 6, 4,26, 6, 6,
		 8,21,10,10,27,31,10,10,
		12,23,14,14,12,23,14,14,
		 8,21,10,10,27,31,10,10,
		12,23,14,14,12,23,14,14,

		32,36,25,25,44,40,25,25,
		19,30,20,20,19,30,20,20,
		39,43,29,29,42,46,29,29,
		19,30,20,20,19,30,20,20,
		 8,21,10,10,27,31,10,10,
		12,23,14,14,12,23,14,14,
		 8,21,10,10,27,31,10,10,
		12,23,14,14,12,23,14,14,

		 1, 1, 3, 3,16,16, 3, 3,
		 5, 5, 7, 7, 5, 5, 7, 7,
		24,24,18,18,28,28,18,18,
		 5, 5, 7, 7, 5, 5, 7, 7,
		 9, 9,11,11,22,22,11,11,
		13,13,-2,-2,13,13,-2,-2,
		 9, 9,11,11,22,22,11,11,
		13,13,-2,-2,13,13,-2,-2,

		 1, 1, 3, 3,16,16, 3, 3,
		 5, 5, 7, 7, 5, 5, 7, 7,
		24,24,18,18,28,28,18,18,
		 5, 5, 7, 7, 5, 5, 7, 7,
		 9, 9,11,11,22,22,11,11,
		13,13,-2,-2,13,13,-2,-2,
		 9, 9,11,11,22,22,11,11,
		13,13,-2,-2,13,13,-2,-2
	};

	int index = 0, value = -1;

	CellClass const * cellptr = &Map[cell];

	if (fog) {
		// Presume solid black if that is what is here already.
		if (!cellptr->IsFogVisible && !cellptr->IsFogMapped) value = -2;

		if (cellptr->IsFogMapped) {
			// Index the lookup table by which of the 8 surrounding cells are black; bit 0x80 is
			// directly north and the bits run clockwise from the upper-right corner.
			Cell c;
			c = cell + Cell(-1, -1);
			if (!Map[c].IsFogMapped) index |= 0x40;
			c = cell + Cell(+0, -1);
			if (!Map[c].IsFogMapped) index |= 0x80;
			c = cell + Cell(+1, -1);
			if (!Map[c].IsFogMapped) index |= 0x01;
			c = cell + Cell(-1, +0);
			if (!Map[c].IsFogMapped) index |= 0x20;
			c = cell + Cell(+1, +0);
			if (!Map[c].IsFogMapped) index |= 0x02;
			c = cell + Cell(-1, +1);
			if (!Map[c].IsFogMapped) index |= 0x10;
			c = cell + Cell(+0, +1);
			if (!Map[c].IsFogMapped) index |= 0x08;
			c = cell + Cell(+1, +1);
			if (!Map[c].IsFogMapped) index |= 0x04;

			value = _shadow[index];
		}
	} else {

		// Presume solid black if that is what is here already.
		if (!cellptr->IsVisible && !cellptr->IsMapped) value = -2;

		if (cellptr->IsMapped) {
			// Index the lookup table by which of the 8 surrounding cells are black; bit 0x80 is
			// directly north and the bits run clockwise from the upper-right corner.
			Cell c;
			c = cell + Cell(-1, -1);
			if (!Map[c].IsMapped) index |= 0x40;
			c = cell + Cell(+0, -1);
			if (!Map[c].IsMapped) index |= 0x80;
			c = cell + Cell(+1, -1);
			if (!Map[c].IsMapped) index |= 0x01;
			c = cell + Cell(-1, +0);
			if (!Map[c].IsMapped) index |= 0x20;
			c = cell + Cell(+1, +0);
			if (!Map[c].IsMapped) index |= 0x02;
			c = cell + Cell(-1, +1);
			if (!Map[c].IsMapped) index |= 0x10;
			c = cell + Cell(+0, +1);
			if (!Map[c].IsMapped) index |= 0x08;
			c = cell + Cell(+1, +1);
			if (!Map[c].IsMapped) index |= 0x04;

			value = _shadow[index];
		}
	}
	return(value);
}

// FUNCTION: 0x006D8AE0
bool Tactical::Is_Cell_Visible(Cell const & cell)
{
	Point2D point = Point2D(cell.X, cell.Y);
	if (VisibleCellRect.Is_Point_Within(point)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006D8B30
void Tactical::Update_Visible_Cells(void)
{
	TacPixelX = TacticalCoord.X;
	TacPixelY = TacticalCoord.Y;
	TacPixelX -= TacticalDimensions.Width / 2;
	TacPixelY -= TacticalDimensions.Height / 2;

	Point2D lepton = Pixel_To_Lepton(Point2D(TacPixelX - 60, TacPixelY - 30));

	VisibleCellRect.X = lepton.X / CELL_LEPTON;
	VisibleCellRect.Y = lepton.Y / CELL_LEPTON;
	VisibleCellRect.Width = TacticalDimensions.Width / 60 + 2;
	VisibleCellRect.Height = TacticalDimensions.Height / 15 + 4;
	do { } while (0);
}

// FUNCTION: 0x006D8C60
void Tactical::Draw_Render_Hooks(ObjectClass * object, Rect const & cliprect)
{
	Point2D pixel;
	if (TacticalMap->Coord_To_Pixel(object->Center_Coord(), pixel)) {
		object->Draw_Pre_Render(pixel, cliprect);
		object->Draw_Post_Render(pixel, cliprect);
	}
}

// FUNCTION: 0x006D97D0
void Tactical::Draw_Terrain(bool forced, Rect const & area, Rect const & cliprect)
{
	Rect inter = Intersect(area, cliprect);
	for (int i = DisplayClass::Layer[LAYER_GROUND].Count() - 1; i >= 0; i--) {
		ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][i];
		if (obj->RTTI == RTTI_TERRAIN && !obj->Is_Inactive() && !((TerrainClass*)obj)->Is_Animating()) {
			Rect render = obj->Get_Render_Rect() + Point2D(TacticalRect.X, TacticalRect.Y);
			if (render.Is_Overlapping(inter)) {
				Rect clip = cliprect;
				obj->Render(clip, forced, false);
				obj->IsToDisplay = false;
			}
		}
	}
}

// FUNCTION: 0x006D9910
void Tactical::Draw_Fogged_Objects(Rect const & area)
{
	::Draw_Fogged_Objects(area);
}

// FUNCTION: 0x006D9920
void Tactical::Draw_Buildings(bool forced, Rect const & area, Rect const & cliprect)
{
	Rect inter = Intersect(area, cliprect);
	for (int i = 0; i < DisplayClass::Layer[LAYER_GROUND].Count(); i++) {
		ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][i];
		if (obj->RTTI == RTTI_BUILDING && (!((BuildingClass*)obj)->IsFogged || Unsorted::ArmageddonMode)) {
			Rect render = obj->Get_Render_Rect() + Point2D(TacticalRect.X, TacticalRect.Y);
			if (render.Is_Overlapping(inter)) {
				Rect clip = cliprect;
				obj->Render(clip, forced, false);
				obj->IsToDisplay = false;
			}
		}
	}
}

// FUNCTION: 0x006D9A50
void Tactical::Register_Dirty_Buildings(Rect area)
{
	for (int i = 0; i < DisplayClass::Layer[LAYER_GROUND].Count(); i++) {
		ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][i];
		if (obj->RTTI == RTTI_BUILDING && obj->IsToDisplay) {
			Rect render = obj->Get_Render_Rect();
			if (area.Is_Overlapping(render + Point2D(TacticalRect.X, TacticalRect.Y))) {
				Register_Dirty_Area(render);
				obj->IsToDisplay = false;
			}
		}
	}
}

// FUNCTION: 0x006D9B50
bool Tactical::Contains_Selectable_Buildings(Rect rect)
{
	Point2D top_left;
	Point2D bottom_right;
	top_left.X = rect.X + TacPixelX;
	top_left.Y = rect.Y + TacPixelY;
	bottom_right.X = rect.X + TacPixelX + rect.Width;
	bottom_right.Y = rect.Y + TacPixelY + rect.Height;

	for (int i = 0; i < Buildings.Count(); i++) {
		BuildingClass * bptr = Buildings[i];
		if (bptr->IsActive && bptr->IsToDisplay && bptr->IsDown) {
			Point2D point = Coord_To_Pixel_Absolute(bptr->Render_Coord());
			if (point.X >= top_left.X && point.X <= bottom_right.X && point.Y >= top_left.Y && point.Y <= bottom_right.Y) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: 0x006D9CE0
void Tactical::Add_Buildings_To_Selectable(Rect rect)
{
	Point2D top_left;
	Point2D bottom_right;
	top_left.X = rect.X + TacPixelX;
	top_left.Y = rect.Y + TacPixelY;
	bottom_right.X = rect.X + TacPixelX + rect.Width;
	bottom_right.Y = rect.Y + TacPixelY + rect.Height;

	for (int i = 0; i < Buildings.Count(); i++) {
		BuildingClass *bptr = Buildings[i];
		if (bptr->IsActive  && bptr->IsDown && !bptr->Class->IsInvisibleInGame) {
			Point2D point = Coord_To_Pixel_Absolute(bptr->PositionCoord);
			if (point.X >= top_left.X && point.X <= bottom_right.X && point.Y >= top_left.Y && point.Y <= bottom_right.Y) {
				Add_To_Selectables(bptr, point - Point2D(TacPixelX, TacPixelY));
			}
		}
	}
}

// FUNCTION: 0x006D9EF0
bool Tactical::Add_To_Selectables(ObjectClass * object, Point2D point)
{
	if (SelectableCount != MAX_SELECTABLE_OBJECTS) {
		if (point.X >= -32 && point.X <= TacticalRect.Width + 32 && point.Y >= -32 && point.Y <= TacticalRect.Height + 32) {
			SelectableObjects[SelectableCount].Object = object;
			SelectableObjects[SelectableCount].Position = point + Point2D(TacPixelX, TacPixelY);
			SelectableCount++;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x006D9F80
void Tactical::Start_Rubber_Band(Point2D const & point)
{
	if (RubberBandStart == Point2D(0, 0)) {
		RubberBandStart = point;
		RubberBandEnd = point;
	}
}

// FUNCTION: 0x006D9FC0
void Tactical::Modify_Rubber_Band(Point2D const & point)
{
	if (RubberBandStart != Point2D(0, 0)) {
		RubberBandEnd = point;
	}
}

// FUNCTION: 0x006D9FF0
void Tactical::Select_Rubber_Band(void (*select_callback)(ObjectClass * object))
{
	if (RubberBandStart != Point2D(0, 0)) {

		Point2D start = RubberBandStart;
		Point2D end = RubberBandEnd;

		if (start.X > end.X) {
			swap(start.X, end.X);
		}
		if (start.Y > end.Y) {
			swap(start.Y, end.Y);
		}

		Select_These(Rect(start, end.X - start.X + 1, end.Y - start.Y + 1), select_callback);
		RubberBandStart = Point2D(0, 0);
	}
}

// FUNCTION: 0x006DA080
int Tactical::Rubber_Band_Contains_Selectable(void)
{
	Point2D start = RubberBandStart;
	Point2D end = RubberBandEnd;

	if (start.X > end.X) {
		swap(start.X, end.X);
	}
	if (start.Y > end.Y) {
		swap(start.Y, end.Y);
	}

	Rect rect(start, end.X - start.X + 1, end.Y - start.Y + 1);
	if (rect.Is_Valid()) {
		for (int index = 0; index < SelectableCount; index++) {
			if (SelectableObjects[index].Object != NULL && SelectableObjects[index].Object->IsActive) {
				Point2D pos = SelectableObjects[index].Position - Point2D(TacPixelX, TacPixelY);
				if (rect.Is_Point_Within(pos)) {
					return(true);
				}
			}
		}
	}
	return(false);
}

// FUNCTION: 0x006DA160
void Tactical::End_Rubber_Band(void)
{
	RubberBandStart = Point2D(0, 0);
	RubberBandEnd = Point2D(0, 0);
}

// FUNCTION: 0x006DA180
void Tactical::Draw_Rubber_Band(void)
{
	if (RubberBandStart != Point2D(0, 0)) {

		Point2D start = RubberBandStart;
		Point2D end = RubberBandEnd;

		if (end.X < start.X) {
			swap(start.X, end.X);
		}
		if (end.Y < start.Y) {
			swap(start.Y, end.Y);
		}

		int w = (end.X - start.X) + 1;
		int h = (end.Y - start.Y) + 1;
		int color = NormalDrawer->Convert_Pixel(15);
		Rect rect(start, w, h);
		LogicalSurface->Draw_Rect(TacticalRect, rect, color);
	}
}

// FUNCTION: 0x006DA230
FacingType Tactical::Scroll_Dir(FacingType dir)
{
	static Point2D _scroll[FACING_COUNT] = 	{
		Point2D(0, -1),
		Point2D(1, -1),
		Point2D(1, 0),
		Point2D(1, 1),
		Point2D(0, 1),
		Point2D(-1, 1),
		Point2D(-1, 0),
		Point2D(-1, -1)
	};

	static FacingType _newdir[9] = {
		FACING_NW, FACING_N, FACING_NE,
		FACING_W, FACING_NONE, FACING_E,
		FACING_SW, FACING_S, FACING_SE
	};

	Point2D oldpos = TacticalCoord;
	Point2D newpos = oldpos + _scroll[dir];

	if (newpos != oldpos) {
		Point2D diff = TacticalCoord - Clamp_Pixel_To_Tactical(newpos);
		return(_newdir[(diff.Y + 1) * 3 + diff.X + 1]);
	}
	return(dir);
}

// FUNCTION: 0x006DA350 $E218

// FUNCTION: 0x006DA360
Cell Tactical::Clamp_Cursor_To_Tactical(Cell const & cell, Cell const * list)
{
	return(cell);
}

// FUNCTION: 0x006DA370
void Tactical::Noop(int)
{
}

// FUNCTION: 0x006DA380
ObjectClass * Tactical::Get_Selectable_Object(Point2D const & point)
{
	ObjectClass * best = NULL;
	unsigned int bestdist = -1;

	int pixelx = point.X + TacPixelX;
	int pixely = point.Y + TacPixelY;

	for (int index = 0; index < SelectableCount; index++) {
		Selectable & sel = SelectableObjects[index];
		ObjectClass * obj = sel.Object;

		if (obj != NULL) {

			TechnoClass const * tech = Dynamic_Cast<TechnoClass const *>((AbstractClass const *)obj);
			bool pass = false;

			if (tech != NULL) {
				if (obj->IsActive && !tech->IsInLimbo) {
					if (tech->IsOwnedByPlayer || tech->Cloak != CLOAKED || Map[tech->Center_Coord()].Is_Sensed(PlayerPtr->HeapID)) {
						pass = true;
					}
				}
			}

			if (tech == NULL) {
				if (obj->Class_Of() != NULL && obj->Class_Of()->RTTI == RTTI_TERRAINTYPE) {
					TerrainTypeClass * type = (TerrainTypeClass *)obj->Class_Of();
					if (type->IsVeinhole) {
						pass = true;
					}
				}
			}

			if (pass) {
				Point2D pos = sel.Position - Point2D(pixelx, pixely);
				unsigned int dist = int(double(pos.X * pos.X) + double(pos.Y * pos.Y) * 0.5);
				if (dist < bestdist && dist < 200) {
					bestdist = dist;
					best = obj;
				}
			}
		}
	}
	if (best != NULL) {
		return(best);
	}

	return(Map[Cell(Pixel_To_Cell(TacticalRect.Top_Left() + point))].Cell_Occupier());
}

// FUNCTION: 0x006DA560
void Tactical::Detach(AbstractClass const * target, bool all)
{
	for (int index = 0; index < MAX_SELECTABLE_OBJECTS; index++) {
		Selectable & sel = SelectableObjects[index];
		if (sel.Object == (ObjectClass *)target) {
			sel.Object = NULL;
		}
	}

	if (target == DAT_00880978) {
		Map.FUN_004A8D50(NULL);
		DAT_00880978 = NULL;
		DAT_0088097C = 0;
		DAT_00880980 = -1;
	}
}

bool FUN_00732D00(void);
namespace Game { void __fastcall UICommands_TypeSelect_7327D0(char const * name); }

// FUNCTION: 0x006DA5C0
void Tactical::Select_These(Rect const & rect, void (*select_callback)(ObjectClass * object))
{
	AllowVoice = true;

	if (rect.Is_Valid()) {

		for (int index = 0; index < SelectableCount; index++) {
			ObjectClass * obj = SelectableObjects[index].Object;

			if (obj == NULL || !obj->IsActive) {
				continue;
			}

			Point2D pos = SelectableObjects[index].Position - Point2D(TacPixelX, TacPixelY);

			if (!rect.Is_Point_Within(pos)) {
				continue;
			}

			if (FUN_00732D00()) {
				Game::UICommands_TypeSelect_7327D0(obj->Class_Of()->IniName);
				continue;
			}

			if (select_callback == NULL) {

				bool force = false;

				if (obj->RTTI == RTTI_BUILDING) {
					BuildingTypeClass * type = ((BuildingClass *)obj)->Class;
					if (type->UndeploysInto && type->IsVehicle()) {
						force = true;
					}
				}

				HouseClass * hptr = obj->Owner_HouseClass();
				if (hptr != NULL && hptr->Is_Player_Control() &&
					obj->CanBeSelected() &&
					(obj->RTTI != RTTI_BUILDING || force)) {
					if (obj->Select()) {
						AllowVoice = false;
					}
				}
			} else {
				select_callback(obj);
			}
		}
	}
	AllowVoice = true;
}

// FUNCTION: 0x006DA740
void __fastcall FUN_006DA740(Tactical * tactical)
{
	while (CurrentObject.Count()) {
		CurrentObject[0]->Unselect();
	}
	FUN_00731D00(0);
}

// FUNCTION: 0x006DA7D0
void Tactical::Flag_Cell(CellClass & cell)
{
	if (Map.DrawFlags == GS_REDRAW_DIRTY && cell.LastRedrawFrame != Frame && (cell.IsMapped || cell.VisibilityChanged)) {
		cell.LastRedrawFrame = Frame;

		Coord coord = cell.Cell_Coord();

		Coord ground = Coord(coord.X, coord.Y, 0);

		Point2D pixel;
		if (cell.IsMapped && cell.IsVisible) {
			Coord_To_Pixel(ground, pixel);
		} else {
			Coord_To_Pixel(ground, pixel);
		}

		if (pixel.X >= DSurface::ViewBounds.X - 30
			&& pixel.Y >= -30
			&& pixel.X <= DSurface::ViewBounds.X + DSurface::ViewBounds.Width + 30
			&& pixel.Y <= DSurface::ViewBounds.Y + DSurface::ViewBounds.Height + 15) {

			if (CellRedrawCount < 799) {
				CellRedraw[CellRedrawCount] = &cell;
				CellRedrawCount++;
			}
			cell.VisibilityChanged = false;
			IsToRedraw = true;
		}
	}
}

// FUNCTION: 0x006DA980
void Tactical::Reset_Dirty_Rectangles(void)
{
	while (DirtyAreas.Count() > 0) {
		DirtyAreas.Delete_Index(0);
	}
}

extern DynamicVectorClass<ObjectClass *> DAT_00A8E360;

// FUNCTION: 0x006DB210
void Tactical::Debug_Draw_Occupier_Links(void)
{
	Map.Reset_Iterator();
	CellClass * cellptr = Map.Iterate();
	int ocolor1 = DSurface::Build_Hicolor_Pixel(255, 255, 0);
	int lcolor = DSurface::Build_Hicolor_Pixel(255, 0, 255);
	int ocolor2 = DSurface::Build_Hicolor_Pixel(0, 255, 255);
	while (cellptr != NULL) {
		ObjectClass * object;

		object = cellptr->Cell_Occupier(false);
		if (object != NULL) {
			Coord coord1 = cellptr->Cell_Coord();
			Point2D start;
			Coord_To_Pixel(coord1, start);
			Coord coord2 = object->Center_Coord();
			Point2D end;
			Coord_To_Pixel(coord2, end);
			Rect rect(start.X - 1, start.Y - 1, 3, 3);
			DSurface::Temp->Fill_Rect(DSurface::ViewBounds, rect, ocolor1);
			DSurface::Temp->Draw_Line(DSurface::ViewBounds, start, end, ocolor1);
		}

		object = cellptr->Cell_Occupier(true);
		if (object != NULL) {
			Coord coord1 = cellptr->Cell_Coord();
			Point2D start;
			Coord_To_Pixel(coord1, start);
			start += Point2D(0, -60);
			Coord coord2 = object->Center_Coord();
			Point2D end;
			Coord_To_Pixel(coord2, end);
			Rect rect(start.X - 1, start.Y - 1, 3, 3);
			DSurface::Temp->Fill_Rect(DSurface::ViewBounds, rect, ocolor2);
			DSurface::Temp->Draw_Line(DSurface::ViewBounds, start, end, ocolor2);
		}
		cellptr = Map.Iterate();
	}

	for (int i = DAT_00A8E360.Count() - 1; i >= 0; i--) {
		ObjectClass * object = DAT_00A8E360[i];
		if (object->Next != NULL) {
			Coord coord1 = object->Center_Coord();
			Point2D start;
			Coord_To_Pixel(coord1, start);
			Coord coord2 = object->Next->Center_Coord();
			Point2D end;
			Coord_To_Pixel(coord2, end);
			DSurface::Temp->Draw_Line(DSurface::ViewBounds, start, end, lcolor);
		}
	}
}

// FUNCTION: 0x006DBB60
bool Tactical::Draw_3D_Line(Coord const & coord1, Coord const & coord2, int color, bool write_depth)
{
	Point2D start;
	Coord_To_Pixel(coord1, start);
	Point2D end;
	Coord_To_Pixel(coord2, end);

	DSurface::Temp->Draw_Depth_Shaded_Line(DSurface::ViewBounds, start, end, color,
		14 - Z_Lepton_To_Pixel(coord1.Z), 14 - Z_Lepton_To_Pixel(coord2.Z), write_depth);
	return(true);
}

// FUNCTION: 0x006DBCE0
HRESULT STDMETHODCALLTYPE Tactical::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TacticalMapClass;
	return(S_OK);
}

// FUNCTION: 0x006DBD20
HRESULT STDMETHODCALLTYPE Tactical::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) Tactical(NoInitClass());

		SelectableCount = 0;
		for (int i = 0; i < CellRedrawCount; i++) {
			Swizzle_Pointer(&CellRedraw[i]);
		}

		Set_Tactical_Position(TacticalCoord);
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006DBE00
HRESULT STDMETHODCALLTYPE Tactical::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006DC400
void Tactical_noop2(void)
{
}

// FUNCTION: 0x006DC410
void Tactical_noop3(void)
{
}

// FUNCTION: 0x006DC420
int Tactical::Selectable_Count(void)
{
	return(SelectableCount);
}

// FUNCTION: 0x006DC430
ObjectClass * __stdcall Tactical::Selectable_Object(int index)
{
	return(SelectableObjects[index].Object);
}
