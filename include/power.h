// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "radar.h"


class PowerClass : public RadarClass
{
		typedef RadarClass BASECLASS;

	public:
		PowerClass(void);
		PowerClass(NoInitClass const & x) : BASECLASS(x), FlashTimer(x) {};

		virtual void One_Time(void) override;
		virtual void Init_Clear(void) override;
		virtual void Draw_It(bool complete=false) override;
		virtual void AI(KeyNumType &input, Point2D const & xy) override;
		void Flash_Power(void);

		virtual void DisposeOfArt(void) override;
		virtual void Init_For_House(void) override;
		virtual void Reposition_Sidebar(void) override;
		virtual char const * Help_Text(int id) override;

		bool IsToRedraw;

	protected:
		enum PowerEnums {
			GADGET_POWER=999,
			POWER_X=8,
			POWER_Y=25,
			POWER_WIDTH=12,
			POWER_PIP_HEIGHT=3,
			POWER_PIP_EMPTY=0,
			POWER_PIP_GREEN=1,
			POWER_PIP_YELLOW=2,
			POWER_PIP_RED=3,
			POWER_PIP_WHITE=4,
			POWER_FLASH_COUNT=10,
			POWER_FLASH_RATE=3,
		};

	private:
		static int Max_Power_Height(void);
		int Desired_Power_Height(void);
		int Desired_Levels(int & green, int & yellow, int & red);
		int Update_Delay(void);
		void Remove_Pip(void);
		void Add_Pip(void);

		CDTimerClass<SystemTimerClass> FlashTimer;
		int FlashCount;

		CDTimerClass<SystemTimerClass> UpdateTimer;

		int GreenPipCount;
		int YellowPipCount;
		int RedPipCount;

		bool HasChanged;

		int RecordedDrain;
		int RecordedPower;

		static void const * PowerPipShape;
};
