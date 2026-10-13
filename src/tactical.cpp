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

// FUNCTION: 0x006D6A90
int Tactical::Get_Cell_Height(Coord const & coord)
{
	CellClass *cellptr = &Map[coord];
	return(cellptr->Get_Height(coord));
}

// FUNCTION: 0x006D6AF0
void Tactical::Add_Shadow_Control(ShadowControlClass control)
{
	ShadowControlClass *ctrl = new ShadowControlClass(control);
	ShadowControls.Add(ctrl);
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

// FUNCTION: 0x006D9910
void Tactical::Draw_Fogged_Objects(Rect const & area)
{
	::Draw_Fogged_Objects(area);
}

// FUNCTION: 0x006DA360
Cell Tactical::Clamp_Cursor_To_Tactical(Cell const & cell, Cell const * list)
{
	return(cell);
}

// FUNCTION: 0x006DA980
void Tactical::Reset_Dirty_Rectangles(void)
{
	while (DirtyAreas.Count() > 0) {
		DirtyAreas.Delete_Index(0);
	}
}

// FUNCTION: 0x006DBCE0
HRESULT STDMETHODCALLTYPE Tactical::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TacticalMapClass;
	return(S_OK);
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
