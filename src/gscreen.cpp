// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define DIRECTDRAW_VERSION 0x0300
#include "always.h"
#include <d3d.h>
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x004F4030 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004F4060 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004F4080 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004F40A0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004F40C0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004F40E0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004F4100 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004F4140 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004F4170 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004F41A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004F41D0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004F41E0 SYMBOL
// _$E35

#include "gscreen.h"
#include "_bench.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "_tooltip.h"
#include "_xmouse.h"
#include "bench.h"
#include "cctooltip.h"
#include "gadget.h"
#include "goptions.h"
#include "keyboard.h"
#include "session.h"
#include "surface.h"
#include "tactical.h"
#include "dsurface.h"
#include "bench.hh"

GadgetClass * GScreenClass::Buttons = NULL;

class Texture3D {
	public:
		int FUN_004A33A0(unsigned short * pixels);
};

struct Triangle3D {
	void Set_Color(int a, int b, int c);
	void Set_Vertex(int index, float x, float y, float z, float u, float v);

	D3DTLVERTEX Vertex[3];
};

class Texture3DCache {
	public:
		HRESULT Flush(LPDIRECTDRAWSURFACE target);
		int Add(Triangle3D const & triangle);
};

extern bool DAT_008A0DF0;
extern LPDIRECT3DDEVICE2 DAT_008A0DF8;
extern Texture3D * DAT_008A0E08;
extern Texture3D * DAT_008A0E0C;
extern Texture3DCache * DAT_008A0E10;
extern bool DAT_00A9FA94;

extern int DAT_00A8EF38;
extern int DAT_00A8EF3C;
extern int DAT_00A8EF40;

// FUNCTION: GAMEMD 0x004F4200
void FUN_004F4200(void)
{
	DAT_00A8EF40 = 0;
	DAT_00A8EF38 = 0x80;
	DAT_00A8EF3C = 0x80;
}

// FUNCTION: GAMEMD 0x004F4220
GScreenClass::GScreenClass(void) :
	ScreenX(0),
	ScreenY(0),
	DrawFlags(GS_REDRAW_ALL)
{
}

// FUNCTION: GAMEMD 0x004F4240
LONG GScreenClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	if (riid != IID_IUnknown && riid != IID_IGameMap) {
		return(E_NOINTERFACE);
	}

	*ppvObject = reinterpret_cast<IGameMap *>(this);

	reinterpret_cast<IUnknown *>(*ppvObject)->AddRef();
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004F42A0
void GScreenClass::One_Time(void)
{
	Buttons = 0;
}

// FUNCTION: GAMEMD 0x004F42B0
void GScreenClass::Init(TheaterType theater)
{
	Init_Clear();
	Init_IO();
}

// FUNCTION: GAMEMD 0x004F42D0
void GScreenClass::Init_Clear(void)
{
	DrawFlags = GS_REDRAW_ALL;
}

// FUNCTION: GAMEMD 0x004F42E0
void GScreenClass::Init_IO(void)
{
	Buttons = 0;
}

// FUNCTION: GAMEMD 0x004F42F0
void GScreenClass::Flag_To_Redraw(GScreenRedrawFlags flags)
{
	if (TacticalMap != NULL) {
		TacticalMap->IsToRedraw = true;
	}

	if (flags != GS_REDRAW_DIRTY) {
		if (DrawFlags != GS_REDRAW_ALL) {
			DrawFlags = flags;
		}
		Map.Increment_Redraw_Counter();
	}
}

// FUNCTION: GAMEMD 0x004F4320
void GScreenClass::Input(KeyNumType & key, int & x, int & y)
{
	x = Get_Mouse_X();
	y = Get_Mouse_Y();

	if (Buttons != NULL) {
		if (Buttons->Is_List_To_Redraw()) {
			Flag_To_Redraw();
		}

		Surface * oldpage = LogicalSurface;
		LogicalSurface = HiddenSurface;

		key = Buttons->Input();

		LogicalSurface = oldpage;

	} else {
		key = Keyboard->Check();

		if (key != 0) {
			key = Keyboard->Get();
		}
	}

	AI(key, Point2D(x, y));
}

// FUNCTION: GAMEMD 0x004F43F0
bool GScreenClass::SetButtons(GadgetClass & gadget)
{
	return Buttons == &gadget.Head_Of_List();
}

// FUNCTION: GAMEMD 0x004F4410
bool GScreenClass::Add_A_Button(GadgetClass & gadget)
{
	if (SetButtons(gadget)) {
		return false;
	}

	if (Buttons) {
		gadget.Add_Tail(*Buttons);
	} else {
		Buttons = &gadget;
	}
	return true;
}

// FUNCTION: GAMEMD 0x004F4450
bool GScreenClass::Remove_A_Button(GadgetClass & gadget)
{
	if (!SetButtons(gadget)) {
		return false;
	}

	Buttons = (GadgetClass *)gadget.Remove();
	return true;
}

namespace Unsorted { extern bool ArmageddonMode; }
extern bool FUN_0053BAE0(void);
extern void FUN_0055F1E0(bool);


// FUNCTION: GAMEMD 0x004F4480
void GScreenClass::Render(void)
{
	Surface * oldpage = LogicalSurface;
	LogicalSurface = CompositeSurface;

	MouseCursor->Erase_Mouse(CompositeSurface);

	bool redraw = DrawFlags != GS_REDRAW_DIRTY;
	bool complete = DrawFlags == GS_REDRAW_ALL;
	DrawFlags = GS_REDRAW_DIRTY;

	if (!FUN_0053BAE0()) {
		TacticalMap->Render(*CompositeSurface, redraw, DRAW_PASS_PAN);
		TacticalMap->Render(*CompositeSurface, redraw, DRAW_PASS_BACKGROUND);
		Draw_It(complete);
		TacticalMap->Render(*CompositeSurface, redraw, DRAW_PASS_FOREGROUND);
	}

	if (SidebarClass::IsToEraseSidebarMouse && !Unsorted::ArmageddonMode) {
		MouseCursor->Erase_Mouse(SidebarSurface, true);
		SidebarClass::IsToEraseSidebarMouse = false;
	}

	if (Buttons) Buttons->Draw_All(false);

	Session.Messages.Draw();

	if (ToolTips != NULL) {
		ToolTips->Draw_Current();
	}

	if (Session.EnableMultiplayerDebug) {
		FUN_0055F1E0(false);
	}

	MouseCursor->Draw_Mouse(CompositeSurface);
	Blit_Display();
	LogicalSurface = oldpage;
}

// FUNCTION: GAMEMD 0x004F45B0
void GScreenClass::Blit_Display(void)
{
	if (DAT_008A0DF0 && DAT_008A0E08 != NULL && DAT_008A0DF8->BeginScene() == 0) {
		if (DAT_008A0E08->FUN_004A33A0(NULL) == 0) {
			if (DAT_008A0E0C == NULL || DAT_008A0E0C->FUN_004A33A0(NULL) == 0) {
				if (DAT_008A0E10 != NULL) {
					if (DAT_008A0E0C != NULL) {
						Triangle3D a;
						Triangle3D b;
						a.Set_Color(0x80, 0x80, 0x80);
						b.Set_Color(0x80, 0x80, 0x80);
						a.Set_Vertex(0, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f);
						a.Set_Vertex(1, 0.0f, 255.0f, 0.01f, 0.0f, 1.0f);
						a.Set_Vertex(2, 255.0f, 255.0f, 0.01f, 1.0f, 1.0f);
						b.Set_Vertex(0, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f);
						b.Set_Vertex(1, 255.0f, 255.0f, 0.01f, 1.0f, 1.0f);
						b.Set_Vertex(2, 255.0f, 0.0f, 0.01f, 1.0f, 0.0f);
						DAT_008A0E10->Add(a);
						DAT_008A0E10->Add(b);
					}
					DAT_008A0E10->Flush(DSurface::Composite->Get_DD_Surface());
				}
			}
		}
		DAT_008A0DF8->EndScene();
	}

	if (!DAT_00A9FA94) {
		Update_Visible_Surface(false, CompositeSurface, NULL);
		DAT_00A9FA94 = false;
	}
}

extern bool DAT_00884D2E;
extern bool TacticalActive;
extern Rect * DAT_00B0FC58;

// FUNCTION: GAMEMD 0x004F4780
void __fastcall Update_Visible_Surface(bool flip_mouse, Surface *surface, Rect *rect)
{
	Rect default_rect;

	if (rect == NULL) {
		default_rect = surface->Get_Rect();
		rect = &default_rect;
	}

	RECT client_rect;

	if (!GetClientRect(MainWindow, &client_rect)) {
		return;
	}

	POINT screen_pt;

	screen_pt.x = client_rect.left;
	screen_pt.y = client_rect.top;
	if (!ClientToScreen(MainWindow, &screen_pt)) {
		return;
	}

	Rect dest_rect(screen_pt.x, screen_pt.y, surface->Get_Width(), surface->Get_Height());

	if (Map.ScreenX == 0 && Map.ScreenY == 0) {
	} else {
		if (Map.ScreenX > 0) {
			dest_rect.X += Map.ScreenX;
			dest_rect.Width -= Map.ScreenX;
		} else if (Map.ScreenX < 0) {
			dest_rect.Width += Map.ScreenX;
		}
		if (Map.ScreenY > 0) {
			dest_rect.Y += Map.ScreenY;
			dest_rect.Height -= Map.ScreenY;
		} else if (Map.ScreenY < 0) {
			dest_rect.Height += Map.ScreenY;
		}
	}

	if (!Options.IsSidebarOnRight && !Unsorted::ArmageddonMode) {
		dest_rect.X += MAX(MIN(SidebarSurface->Get_Width(), VisibleRect.Width - dest_rect.Width), 0);
	}

	{
	Rect src_rect = *rect;

	if (TacticalMap && (TacticalMap->ZoomFactor != 1.0)) {
		int zoom_surface_width = surface->Get_Width();
		int zoom_surface_height = surface->Get_Height();

		Rect tmp;
		double zoomed_width = (double)zoom_surface_width / TacticalMap->ZoomFactor;
		tmp.X = (int)(((double)zoom_surface_width - zoomed_width) / 2.0);
		double zoomed_height = (double)zoom_surface_height / TacticalMap->ZoomFactor;
		tmp.Y = (int)(((double)zoom_surface_height - zoomed_height) / 2.0);
		tmp.Width = (int)zoomed_width;
		tmp.Height = (int)zoomed_height;

		src_rect = tmp;

	} else {
		if (src_rect.Width >= dest_rect.Width) {
			src_rect.Width = dest_rect.Width;
		}
		if (src_rect.Height >= dest_rect.Height) {
			src_rect.Height = dest_rect.Height;
		}
	}

	if (Map.ScreenY < 0) {
		src_rect.Y -= Map.ScreenY;
	}

	if (Map.ScreenX < 0) {
		src_rect.X -= Map.ScreenX;
	}

	if (TacticalActive) {
		bool showing = ToolTips != NULL && ToolTips->IsToolTipShowing();
		if (!DAT_00884D2E && Map.ScreenY == 0 && !showing) {
			dest_rect.Height -= DAT_00B0FC58->Height;
			src_rect.Height -= DAT_00B0FC58->Height;
		}
	}

	if (Map.ScreenX != 0) {
		Rect fill_rect(screen_pt.x, screen_pt.y, abs(Map.ScreenX), surface->Get_Height() - DAT_00B0FC58->Height);
		fill_rect.X = Map.ScreenX < 0 ? dest_rect.X + dest_rect.Width : dest_rect.X - Map.ScreenX;

		VisibleSurface->Fill_Rect(VisibleSurface->Get_Rect(), fill_rect, 0);
	}

	if (Map.ScreenY != 0) {
		Rect fill_rect(screen_pt.x, 0, surface->Get_Width(), abs(Map.ScreenY));
		fill_rect.Y = Map.ScreenY < 0 ? dest_rect.Y + dest_rect.Height : dest_rect.Y - Map.ScreenY;

		VisibleSurface->Fill_Rect(VisibleSurface->Get_Rect(), fill_rect, 0);
	}

	if (flip_mouse && MouseCursor != NULL) {
		MouseCursor->Draw_Mouse(surface, false);
	}

	VisibleSurface->Blit_From(dest_rect, *surface, src_rect, false, _dialog_count == 0);

	if (flip_mouse && MouseCursor != NULL) {
		Sleep(50);
		MouseCursor->Erase_Mouse(surface, false);
	}
	}
}

namespace Unsorted { extern int CurrentFrame; }

// FUNCTION: GAMEMD 0x004F4BB0
void GScreenClass::AI(KeyNumType &, Point2D const &)
{
	if (!(Unsorted::CurrentFrame & 1)) {
		if (ScreenX < 0) {
			ScreenX = -1 - ScreenX;
		} else if (ScreenX > 0) {
			ScreenX = 1 - ScreenX;
		}
		if (ScreenY < 0) {
			ScreenY = -1 - ScreenY;
		} else if (ScreenY > 0) {
			ScreenY = 1 - ScreenY;
		}
	}
}
