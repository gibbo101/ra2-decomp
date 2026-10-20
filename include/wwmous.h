// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"
#include "noinit.h"
#include "rect.h"
#include "win.h"
#include "msanim.h"
#include "xmouse.h"

class DSurface;
class BSurface;

class WWMouseClass : public Mouse {
	public:
		WWMouseClass(DSurface * surface, HWND window);
		WWMouseClass(NoInitClass const &) {}
		virtual ~WWMouseClass(void) override;

		virtual void Set_Cursor(Point2D const & hotspot, ShapeSet const * cursor, int shape) override;
		virtual bool Is_Hidden(void) const override;
		virtual void Hide_Mouse(void) override;
		virtual void Show_Mouse(void) override;
		virtual void Release_Mouse(void) override;
		virtual void Capture_Mouse(void) override;
		virtual bool Is_Captured(void) const override;
		virtual void Conditional_Hide_Mouse(Rect region) override;
		virtual void Conditional_Show_Mouse(void) override;
		virtual int Get_Mouse_State(void) const override;
		virtual int Get_Mouse_X(void) const override;
		virtual int Get_Mouse_Y(void) const override;
		virtual Point2D Get_Mouse_Point(void) const override;
		virtual void Set_Mouse_Point(Point2D point) override;
		virtual void Draw_Mouse(Surface * scr, bool issidebarsurface = false) override;
		virtual void Erase_Mouse(Surface * scr, bool issidebarsurface = false) override;
		virtual void Convert_Coordinate(int & x, int & y) const override;

		void FUN_007B8960(void);
		bool FUN_007B8C50(void);
		bool FUN_007B8C60(void);
		Rect FUN_007B8E80(void);
		void FUN_007B93F0(DSurface * surface, int a, int b);
		void FUN_007BA090(void);

		SHPStruct * Image;
		int ImageFrameIndex;
		int RefCount;
		bool field_10;
		unsigned char field_11;
		unsigned char field_12;
		unsigned char field_13;
		int MouseX;
		int MouseY;
		int field_1C;
		int field_20;
		DSurface * Surface;
		HWND hWnd;
		Rect Rect0;
		int MouseXHot;
		int MouseYHot;
		BSurface * field_44;
		Rect Rect1;
		BSurface * field_58;
		Rect Rect2;
		BSurface * field_6C;
		Rect Rect3;
		Rect Rect4;
		unsigned long field_90;
		unsigned long field_94;
};
