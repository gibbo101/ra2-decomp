// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "imap.h"
#include "keyboard.h"
#include "noinit.h"

#include "mouse.hh"
#include "theater.hh"

class GadgetClass;
class Surface;
class Point2D;
template<class T> class TRect;
typedef TRect<int> Rect;

extern Surface * HiddenSurface;

enum GScreenRedrawFlags
{
	GS_REDRAW_DIRTY,
	GS_REDRAW_TACTICAL,
	GS_REDRAW_ALL,
};


class GScreenClass : public IGameMap
{
	friend class Tactical;

	public:

		GScreenClass(void);
		GScreenClass(NoInitClass const &) {};
		virtual ~GScreenClass(void) {}

		virtual LONG STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override { return(1); }
		virtual ULONG STDMETHODCALLTYPE Release(void) override { return(1); }

		virtual void One_Time(void);
		virtual void Init(TheaterType = THEATER_NONE);
		virtual void Init_Clear(void);
		virtual void Init_IO(void);

		virtual void Input(KeyNumType & key, int & x, int & y);
		virtual void AI(KeyNumType &, Point2D const & xy);

		// True when the gadget's list is the input list.
		virtual bool SetButtons(GadgetClass & gadget);
		virtual bool Add_A_Button(GadgetClass & gadget);
		virtual bool Remove_A_Button(GadgetClass & gadget);

		virtual void Flag_To_Redraw(GScreenRedrawFlags flags = GS_REDRAW_DIRTY);
		virtual void Render(void);
		virtual void Draw_It(bool =false) {};
		virtual void Blit_Display(void);

		virtual void Set_Default_Mouse(MouseType mouse, bool wsmall) = 0;
		virtual bool Override_Mouse_Shape(MouseType mouse, bool wsmall) = 0;
		virtual void Revert_Mouse_Shape(void) = 0;
		virtual void Mouse_Small(bool wsmall) = 0;

		static GadgetClass * Buttons;

		int ScreenX;
		int ScreenY;

	private:
		GScreenRedrawFlags DrawFlags;
};

void Update_Visible_Surface(bool flip_mouse = true, Surface *surface = HiddenSurface, Rect *rect = NULL);
