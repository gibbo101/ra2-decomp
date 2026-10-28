// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "grphmenu2.h"
#include "ccfile.h"
#include "ini.h"
#include "convert.h"
#include "msanim.h"

// SYNTHETIC: GAMEMD 0x004F2C50 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x004F2C70
GraphicMenuItem * __fastcall GM_Read_Anim_Item(Wstring const & name, INIClass const & ini, MSEngine & engine, Point2D & image_size)
{
	int id = ini.Get_Int(name, "ID", -1);
	if (id == -1) {
		return(NULL);
	}

	Wstring image;
	ini.Get_String(name, "Image", "", image);
	if (image.length() == 0) {
		return(NULL);
	}

	Wstring palette;
	ini.Get_String(name, "Palette", "", palette);
	if (palette.length() == 0) {
		return(NULL);
	}

	Point2D origin(0,0);
	origin = ini.Get_Point(name, "Origin", origin);
	origin += image_size;

	Rect active_rect(0,0,0,0);
	active_rect = ini.Get_Rect(name, "ActiveRect", active_rect);
	active_rect += image_size;

	bool loop = ini.Get_Bool(name.text(), "Loop", true);
	int rate = ini.Get_Int(name, "Rate", 5);

	return(new GraphicMenuAnimItem(id, engine, origin, active_rect, image, palette, rate, loop));
}

// FUNCTION: GAMEMD 0x004F2F10
GraphicMenuAnimItem::GraphicMenuAnimItem(int id, MSEngine & engine, Point2D const & origin, Rect const & rect, Wstring const & image, Wstring const & palette, int rate, bool loop) :
	GraphicMenuItem(id),
	ActiveRect(rect),
	Reserved(0),
	Drawer(NULL)
{
	CCFileClass image_file(image);
	CCFileClass palette_file(palette);

	if (image_file.Is_Available(0) && palette_file.Is_Available(0)) {
		Drawer = Create_Drawer(palette.text());
		MSShapeAnim * anim = new MSShapeAnim(image, origin.X, origin.Y, Drawer, rate, loop, SHAPE_NORMAL);
		if (anim != NULL) {
			engine.Add_Animation(anim);
		}
	}
}

// FUNCTION: GAMEMD 0x004F3070
GraphicMenuAnimItem::~GraphicMenuAnimItem(void)
{
	delete(Drawer);
}

// FUNCTION: GAMEMD 0x004F3090
bool GraphicMenuAnimItem::Is_Mouse_Over(Point2D const & mouse)
{
	return(Enabled && ActiveRect.Is_Point_Within(mouse));
}

// SYNTHETIC: GAMEMD 0x004F30E0
// ??_GGraphicMenuAnimItem@@UAEPAXI@Z
