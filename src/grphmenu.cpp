// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmenu.h"
#include "_keyboar.h"
#include "_surface.h"
#include "ccfile.h"
#include "globals.h"
#include "grphmitm.h"
#include "ini.h"
#include "keyboard.h"
#include "msanim.h"
#include "ownrdraw.h"
#include "theme.h"


GraphicMenu * _Graphic_Menu(INIClass & ini, Wstring const & name);

// FUNCTION: 0x004F1B80
GraphicMenu * __fastcall Do_Graphic_Menu(Wstring const & ini, Wstring const & name)
{
	CCFileClass file(ini);
	if (!file.Is_Available()) {
		return(NULL);
	}
	INIClass _ini;
	GraphicMenu * menu = _ini.Load(file) > 0 ? _Graphic_Menu(_ini, name) : NULL;
	return(menu);
}

struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
};

GraphicMenuItem * GM_Create_Item_From_INI(Wstring const & name, INIClass const & ini, MSEngine & engine, Point2D & image_size);

// FUNCTION: 0x004F1CA0
GraphicMenu * __fastcall _Graphic_Menu(INIClass & ini, Wstring const & name)
{
	if (ini.GetSection(name.text()) == NULL) {
		return(NULL);
	}
	GraphicMenu * menu = new GraphicMenu;
	if (menu == NULL) {
		return(NULL);
	}

	char entry[8];
	Wstring buffer;
	bool has_background = ini.Get_String(name, "Background", "", buffer) > 0;
	Wstring pcx = buffer + ".PCX";
	menu->BackgroundName = pcx;

	Point2D pt(0,0);

	if (has_background) {
		Wstring vqa = buffer + ".VQA";
		MSAnim * anim = NULL;
		if (CCFileClass(vqa).Is_Available(false)) {
			anim = new MSVQAnim(vqa, AlternateSurface, menu->Engine.Get_Anims(), true);
		}
		if (anim == NULL) {
			Wstring bik = buffer + ".BIK";
			if (CCFileClass(bik).Is_Available(false)) {
				Rect rect(0, 0, Drawing::RenderWidth, Drawing::RenderHeight);
				anim = new MSButtonAnim(bik, (int)AlternateSurface, menu->Engine.Get_Anims(), rect, true);
			}
		}
		if (anim == NULL) {
			anim = new MSPCXAnim(pcx.get(), menu->Engine.Get_Anims(), true);
		}

		menu->Set_Animation(anim);
		pt = anim->Get_Rect().TopLeft;
	}

	if (ini.Get_String(name, "Theme", "", buffer) > 0) {
		menu->Set_Theme_Name(buffer);
	}

	int item_max = ini.Get_Int(name, "ItemMax", 100);
	for (int i = 0; i <= item_max; i++) {
		sprintf(entry, "%d", i);
		if (ini.Get_String(name, entry, "", buffer) > 0) {
			GraphicMenuItem * item = GM_Create_Item_From_INI(buffer, ini, menu->Engine, pt);
			if (item != NULL) {
				menu->Add_Item(item);
			}
		}
	}

	return(menu);
}

// FUNCTION: 0x004F2140
GraphicMenu::GraphicMenu(void) :
	Engine(),
	BackgroundName("Title.PCX"),
	ThemeName("Intro"),
	CurrentAnim(NULL),
	Items()
{
}

// FUNCTION: 0x004F21A0
GraphicMenu::~GraphicMenu(void)
{
	for (ITEM_ITER iter = Items; iter; iter++) {
		delete *iter;
	}
	AlternateSurface->Fill(0);
	HiddenSurface->Fill(0);
	VisibleSurface->Blit_From(*HiddenSurface, false, true);
}

// FUNCTION: 0x004F2280
void GraphicMenu::Set_Item_Enabled(int id, bool enabled)
{
	ITEM_ITER it = Items;
	for (; it; it++) {
		if (it->Get_ID() == id) {
			it->Set_Enabled(enabled);
		}
	}
}

// FUNCTION: 0x004F2640
void GraphicMenu::Set_Animation(MSAnim *anim)
{
	MSAnim * old = CurrentAnim;
	if (old != NULL) {
		Engine.Replace_Anim(old, anim);
	} else {
		Engine.Add_Animation(anim);
	}
	CurrentAnim = anim;
}


// FUNCTION: 0x004F2680
void GraphicMenu::Set_Theme_Name(Wstring const & name)
{
	ThemeName = name;
}

// FUNCTION: 0x004F2690
void GraphicMenu::Add_Item(GraphicMenuItem * item)
{
	Items.Add(item);
}

// FUNCTION: 0x004F26E0
GraphicMenuItem * GraphicMenu::Get_Item_Under_Mouse(Point2D const & mouse)
{
	ITEM_ITER it = Items;
	for (; it; it++) {
		if (it->Is_Mouse_Over(mouse)) {
			break;
		}
	}
	if (it) {
		return(*it);
	}
	return(NULL);
}

// FUNCTION: 0x004F2770
GraphicMenuItem * GraphicMenu::Get_Item_For_Key(KeyNumType key)
{
	ITEM_ITER it = Items;
	for (; it; it++) {
		if (it->Is_Input_Key(key)) {
			break;
		}
	}
	if (it) {
		return(*it);
	}
	return(NULL);
}

// FUNCTION: 0x004F2800
void GraphicMenu::Set_Background_Name(Wstring const & name)
{
	BackgroundName = name;
}

// FUNCTION: 0x004F2810 INIClass::~INIClass

// FUNCTION: 0x004F2830 GraphicMenu::~GraphicMenu

// FUNCTION: 0x004F2850 ?$DynamicVectorClass@PAVGraphicMenuItem@@::Clear

// FUNCTION: 0x004F2880 ?$DynamicVectorClass@PAVGraphicMenuItem@@::ID

// FUNCTION: 0x004F28A0 ?$VectorClass@PAVGraphicMenuItem@@::GetItem
// FUNCTION: 0x004F28B0 ?B?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@
// FUNCTION: 0x004F28C0 ?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@::~?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@
// FUNCTION: 0x004F28E0 ?$VectorClass@PAVGraphicMenuItem@@::operator==
// FUNCTION: 0x004F2920 ?$VectorClass@PAVGraphicMenuItem@@::Resize
// FUNCTION: 0x004F29D0 ?$VectorClass@PAVGraphicMenuItem@@::Clear
// FUNCTION: 0x004F2A00 ?$VectorClass@PAVGraphicMenuItem@@::ID
// FUNCTION: 0x004F2A40 ?$VectorClass@PAVGraphicMenuItem@@::ID
// FUNCTION: 0x004F2A60 ?$DynamicVectorClass@PAVGraphicMenuItem@@::Resize
// FUNCTION: 0x004F2B10 ?$DynamicVectorClass@PAVGraphicMenuItem@@::ID
// FUNCTION: 0x004F2B40 ?E?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@
// FUNCTION: 0x004F2B50 ?F?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@
template class VectorClass<GraphicMenuItem *>;
template class VectorCursor<GraphicMenuItem *, DynamicVectorClass<GraphicMenuItem *> >;
template class DynamicVectorClass<GraphicMenuItem *>;
