// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmitm.h"
#include "ini.h"
#include "mschoice.h"

GraphicMenuItem * __fastcall GM_Read_Image_Item(Wstring const & name, INIClass const & ini, MSEngine & engine, Point2D & image_size);
GraphicMenuItem * __fastcall GM_Read_Shortcut_Item(Wstring const & name, INIClass const & ini);
GraphicMenuItem * __fastcall GM_Read_Anim_Item(Wstring const & name, INIClass const & ini, MSEngine & engine, Point2D & image_size);

// FUNCTION: 0x004F38B0
GraphicMenuItem * __fastcall GM_Create_Item_From_INI(Wstring const & name, INIClass const & ini, MSEngine & engine, Point2D & image_size)
{
	Wstring type;
	{
		Wstring defvalue("");
		Wstring entry("Type");
		ini.Get_String(name, entry, defvalue, type);
	}
	GraphicMenuItem * item = NULL;

	if (type == "Image") {
		item = GM_Read_Image_Item(name, ini, engine, image_size);
	} else if (type == "Shortcut") {
		item = GM_Read_Shortcut_Item(name, ini);
	} else if (type == "Anim") {
		item = GM_Read_Anim_Item(name, ini, engine, image_size);
	}

	if (item != NULL) {
		Wstring sound;
		bool found;
		{
			Wstring defvalue("");
			Wstring entry("SelectSound");
			found = ini.Get_String(name, entry, defvalue, sound) > 0;
		}
		if (found) {
			item->Set_Select_Sound(new MSSfxEntry("SelectSound", sound));
		}
	}
	return(item);
}

// FUNCTION: 0x004F3A50
GraphicMenuItem::GraphicMenuItem(int id) :
	ID(id),
	Selected(false),
	Enabled(true),
	SelectSound(NULL)
{
}

// FUNCTION: 0x004F3A70
GraphicMenuItem::~GraphicMenuItem(void)
{
	if (SelectSound != NULL) {
		delete SelectSound;
	}
}

// FUNCTION: 0x004F3A90
void GraphicMenuItem::Set_Selected(bool selected)
{
	if (selected != Selected) {
		Selected = selected;
		On_Selected_Change(selected);
	}
}

// FUNCTION: 0x004F3AB0
void GraphicMenuItem::Set_Enabled(bool enabled)
{
	if (enabled != Enabled) {
		Enabled = enabled;
		On_Enabled_Change(enabled);
	}
}

// FUNCTION: 0x004F3AD0
void GraphicMenuItem::On_Selected_Change(bool)
{
}

// FUNCTION: 0x004F3AE0
void GraphicMenuItem::On_Enabled_Change(bool)
{
}

// FUNCTION: 0x004F3AF0
bool GraphicMenuItem::Is_Input_Key(KeyNumType key)
{
	return(false);
}

// FUNCTION: 0x004F3B00
bool GraphicMenuItem::Is_Mouse_Over(Point2D const & mouse)
{
	return(false);
}

// FUNCTION: 0x004F3B10
void GraphicMenuItem::Action(MSEngine * engine)
{
	if (SelectSound != NULL) {
		SelectSound->Play();
	}
}

// FUNCTION: 0x004F3B20 GraphicMenuItem::~GraphicMenuItem

// FUNCTION: 0x004F3B60 $E2
