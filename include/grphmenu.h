// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "keyboard.h"
#include "msanim.h"
#include "msengine.h"
#include "rect.h"
#include "wstring.h"

class GraphicMenuItem;
class INIClass;
class MSSfxEntry;
template<class T> class DynamicVectorClass;

class GraphicMenu
{
	friend GraphicMenu * _Graphic_Menu(INIClass & ini, Wstring const & name);

	typedef DynamicVectorClass<GraphicMenuItem *> ITEM_LIST;
	typedef VectorCursor<GraphicMenuItem *, ITEM_LIST> ITEM_ITER;

	public:
		GraphicMenu(void);
		virtual ~GraphicMenu(void);

		void Set_Animation(MSAnim * anim);
		void Set_Theme_Name(Wstring const & name);
		void Set_Background_Name(Wstring const & name);

		void Add_Item(GraphicMenuItem * item);

		void Set_Item_Enabled(int item_id, bool enabled);
		int Presentation(void);
		GraphicMenuItem * Get_Item_Under_Mouse(Point2D const & mouse);
		GraphicMenuItem * Get_Item_For_Key(KeyNumType key);

	public:

		MSEngine Engine;

		Wstring BackgroundName;

		Wstring ThemeName;

	private:

		MSAnim * CurrentAnim;

		ITEM_LIST Items;
};

GraphicMenu * Do_Graphic_Menu(Wstring const & ini, Wstring const & name);
