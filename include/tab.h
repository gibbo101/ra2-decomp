// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "credits.h"
#include "notice.h"
#include "sidebar.h"

class TabClass: public SidebarClass, public INoticeSink
{
		typedef SidebarClass BASECLASS;

	public:
		TabClass(void);
		TabClass(NoInitClass const & x) : BASECLASS(x), Credits(x), FlasherTimer(x), MoneyFlashTimer(x) {};

		virtual void Init_IO(void) override;
		virtual void AI(KeyNumType &input, Point2D const & xy) override;
		virtual void Draw_It(bool complete=false) override;
		virtual char const * Help_Text(int id) override;
		virtual void DisposeOfArt(void) override;
		virtual bool Activate(int control) override;
		virtual bool INoticeSink_Unknown(DWORD unknown, DWORD unknown2) override;

		void Draw_Credits_Tab(void);
		static void Hilite_Tab(int tab);
		void Flash_Money(void);

		virtual void One_Time(void) override;
		void Redraw_Tab(void);
		virtual void Init_For_House(void) override;

		CreditClass Credits;

		CDTimerClass<FrameTimerClass> FlasherTimer;

	private:
		void Set_Active(int select);

	public:
		void FUN_006D0FD0(void);
		void FUN_006D1200(void);
		void FUN_006D04A0(void);
		void FUN_006D04D0(void);
		void Close_Thumb(void);
		void Hide_Buttons(void);
		void Open_Thumb(void);
		void FUN_006D14F0(void);
		void FUN_006D1610(void);
		void FUN_006D1660(void);
		void FUN_006D1570(void);
		void FUN_006D15F0(void);

	private:

		CDTimerClass<FrameTimerClass> MoneyFlashTimer;

	public:
		bool ThumbActive;

	protected:
		bool IsToRedraw;

	public:
		unsigned char unknown_byte_5546;

	private:
		static ShapeSet const * TabShape;
};
