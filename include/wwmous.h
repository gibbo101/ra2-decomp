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

class DSurface;
class BSurface;

class Mouse {
	public:
		virtual ~Mouse(void) {}

		virtual void Draw(Point2D const & coords, SHPStruct const * image, int frame) = 0;
		virtual bool IsRefCountNegative(void) = 0;
		virtual void HideCursor(void) = 0;
		virtual void ShowCursor(void) = 0;
		virtual void ReleaseMouse(void) = 0;
		virtual void CaptureMouse(void) = 0;
		virtual unsigned char IsCaptured(void) = 0;
		virtual void func_20(Rect useless) = 0;
		virtual void CallFunc10(void) = 0;
		virtual unsigned long GetRefCount(void) = 0;
		virtual int GetX(void) = 0;
		virtual int GetY(void) = 0;
		virtual Point2D * GetCoords(Point2D * buffer) = 0;
		virtual void SetCoords(Point2D coords) = 0;
		virtual void func_3C(DSurface * surface, bool flag) = 0;
		virtual void func_40(DSurface * surface, bool flag) = 0;
		virtual void func_44(int * arg1, int * arg2) = 0;
};

class WWMouseClass : public Mouse {
	public:
		WWMouseClass(DSurface * surface, HWND window);
		WWMouseClass(NoInitClass const &) {}
		virtual ~WWMouseClass(void) override;

		virtual void Draw(Point2D const & coords, SHPStruct const * image, int frame) override;
		virtual bool IsRefCountNegative(void) override;
		virtual void HideCursor(void) override;
		virtual void ShowCursor(void) override;
		virtual void ReleaseMouse(void) override;
		virtual void CaptureMouse(void) override;
		virtual unsigned char IsCaptured(void) override;
		virtual void func_20(Rect useless) override;
		virtual void CallFunc10(void) override;
		virtual unsigned long GetRefCount(void) override;
		virtual int GetX(void) override;
		virtual int GetY(void) override;
		virtual Point2D * GetCoords(Point2D * buffer) override;
		virtual void SetCoords(Point2D coords) override;
		virtual void func_3C(DSurface * surface, bool flag) override;
		virtual void func_40(DSurface * surface, bool flag) override;
		virtual void func_44(int * arg1, int * arg2) override;

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
