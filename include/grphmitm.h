// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "grphmenu.h"

class GraphicMenuItem
{

	public:
		GraphicMenuItem(int id = -1);
		virtual ~GraphicMenuItem(void);
		virtual bool Is_Input_Key(KeyNumType key);
		virtual bool Is_Mouse_Over(Point2D const & mouse);
		virtual void Action(MSEngine * engine);
		virtual void On_Selected_Change(bool);
		virtual void On_Enabled_Change(bool);

		void Set_Selected(bool selected);
		void Set_Enabled(bool enabled);

		int Get_ID(void) const { return(ID); }
		void Set_Select_Sound(MSSfxEntry * sound) { SelectSound = sound; }

	protected:

		int ID;

		bool Selected;

		bool Enabled;

		MSSfxEntry * SelectSound;
};
