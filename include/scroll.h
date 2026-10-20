// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "tab.h"

class ScrollClass: public TabClass
{
		typedef TabClass BASECLASS;

		friend class RadarClass::RTacticalClass;

	private:

		static CDTimerClass<SystemTimerClass> Counter;

		int	Inertia;

		bool IsCoastScrollAllowed;

		Point2D RightPressPoint;

		bool IsDragOperation;

		bool IsEdgeScrollAllowed;

		bool IsMouseDown;

	public:
		ScrollClass(void);
		ScrollClass(NoInitClass const & x) : BASECLASS(x) {};

		void Set_Scroll_Coasting_Allowed(bool coasting) { IsCoastScrollAllowed = coasting; }

		virtual void AI(KeyNumType &input, Point2D const & xy) override;
		virtual void Init_IO(void) override {BASECLASS::Init_IO();};
		virtual bool Is_Scrolling(void) const override;
		virtual void Abort_Drag_Select(void) override;

		bool Resolve_Point(Point2D const & point, Cell & cell, Coord & coord, ObjectClass * & object, bool & fog, bool & shadow);

		ActionType What_Action(Cell const & cell, ObjectClass * object, bool check_fog);

		void Message_Handler(HWND hwnd, UINT & message, UINT & wParam, LONG & lParam);

	protected:
		virtual void Mouse_Right_Press(Point2D const & point = Point2D()) override;
		virtual void Mouse_Right_Release(Point2D const & point = Point2D()) override;

		void Scroll_AI(void);

		void Scroll_Edge(Point2D const & point);
		void Scroll_Coast(Point2D const & point);
};
