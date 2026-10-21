// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class ShapeSet;

#include "scroll.h"
#include "stage.h"

class MouseClass: public ScrollClass
{
		typedef ScrollClass BASECLASS;

	public:
		virtual HRESULT Load(IStream * stream) override;
		virtual HRESULT Save(IStream * stream) override;

	public:
		MouseClass(void);
		MouseClass(NoInitClass const & x);

		virtual void One_Time(void) override;
		virtual void Init_Clear(void) override;

		virtual void AI(KeyNumType &input, Point2D const & xy) override;
		virtual bool Override_Mouse_Shape(MouseType mouse, bool wsmall=false) override;
		virtual void Revert_Mouse_Shape(void) override;
		virtual MouseType Get_Mouse_Shape(void) const override {return(NormalMouseShape);};
		virtual void Mouse_Small(bool wsmall) override;

		virtual void Set_Default_Mouse(MouseType mouse, bool wsmall = false) override;

		int Get_Mouse_Current_Frame(MouseType mouse, bool wsmall) const;
		Point2D Get_Mouse_Hotspot(MouseType mouse) const;
		int Get_Mouse_Start_Frame(MouseType mouse, bool wsmall) const;
		int Get_Mouse_Start_Frame(MouseType mouse) const;
		int Get_Mouse_Frame_Count(MouseType mouse) const;

		friend class TacticalClass;

		static ShapeSet const * MouseShapes;

	private:

		struct MouseStruct
		{
			int StartFrame;
			int FrameCount;
			int FrameRate;
			int SmallFrame;
			int SmallFrameCount;
			int X,Y;
		};

		static MouseStruct MouseControl[MOUSE_COUNT];

	public:

		bool IsSmall;
		BYTE unknown_byte_5559[3];

	private:

		MouseType CurrentMouseShape;
		MouseType NormalMouseShape;

		static CDTimerClass<SystemTimerClass> Timer;
		int Frame;
};
