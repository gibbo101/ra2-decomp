// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ftimer.h"
#include "timer.h"

class StageClass {

		unsigned Stage;

		bool HasChanged;

		CDTimerClass<FrameTimerClass> Timer;

		int Rate;

		int Step;

	public:
		StageClass(void) : Stage(0), HasChanged(false), Timer(0), Rate(0), Step(1) {};
		StageClass(NoInitClass const & x) : Timer(x) {};

		int Fetch_Stage(void) const {return(Stage);};
		int Fetch_Rate(void) const {return(Rate);};
		int Fetch_Step(void) const {return(Step);};
		void Set_Stage(int stage) {Stage = stage;};
		void Set_Rate(int rate) {Timer = rate; Rate = rate;};
		void Set_Step(int step) {Step = step;};
		void Adjust_Rate(int rate) {if (rate != Rate) Rate = rate;}
		void Just_Set_Rate(int rate) {Rate = rate;}
		void AI(void) {};
		bool About_To_Change(void) const {return(Timer == 0 && Rate != 0);}
		bool Graphic_Logic(void) {
			if (About_To_Change()) {
				Stage += Step;
				HasChanged = true;
				Timer = Rate;
				return(true);
			}
			HasChanged = false;
			return(false);
		};

};
