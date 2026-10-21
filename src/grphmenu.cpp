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

// FUNCTION: GAMEMD 0x004F1B80
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

// FUNCTION: GAMEMD 0x004F1CA0
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

// FUNCTION: GAMEMD 0x004F2140
GraphicMenu::GraphicMenu(void) :
	Engine(),
	BackgroundName("Title.PCX"),
	ThemeName("Intro"),
	CurrentAnim(NULL),
	Items()
{
}

// FUNCTION: GAMEMD 0x004F21A0
GraphicMenu::~GraphicMenu(void)
{
	for (ITEM_ITER iter = Items; iter; iter++) {
		delete *iter;
	}
	AlternateSurface->Fill(0);
	HiddenSurface->Fill(0);
	VisibleSurface->Blit_From(*HiddenSurface, false, true);
}

// FUNCTION: GAMEMD 0x004F2280
void GraphicMenu::Set_Item_Enabled(int id, bool enabled)
{
	ITEM_ITER it = Items;
	for (; it; it++) {
		if (it->Get_ID() == id) {
			it->Set_Enabled(enabled);
		}
	}
}

// FUNCTION: GAMEMD 0x004F2640
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


// FUNCTION: GAMEMD 0x004F2680
void GraphicMenu::Set_Theme_Name(Wstring const & name)
{
	ThemeName = name;
}

// FUNCTION: GAMEMD 0x004F2690
void GraphicMenu::Add_Item(GraphicMenuItem * item)
{
	Items.Add(item);
}

// FUNCTION: GAMEMD 0x004F26E0
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

// FUNCTION: GAMEMD 0x004F2770
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

// FUNCTION: GAMEMD 0x004F2800
void GraphicMenu::Set_Background_Name(Wstring const & name)
{
	BackgroundName = name;
}

// SYNTHETIC: GAMEMD 0x004F2810
// ??_GINIClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x004F2830
// ??_GGraphicMenu@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004F2850
// ?Clear@?$DynamicVectorClass@PAVGraphicMenuItem@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x004F2880
// ?ID@?$DynamicVectorClass@PAVGraphicMenuItem@@@@UAEHPBQAVGraphicMenuItem@@@Z

// TEMPLATE: GAMEMD 0x004F28A0
// ?GetItem@?$VectorClass@PAVGraphicMenuItem@@@@UBEPAVGraphicMenuItem@@H@Z
// TEMPLATE: GAMEMD 0x004F28B0
// ??B?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@@@UBE_NXZ
// SYNTHETIC: GAMEMD 0x004F28C0
// ??_G?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x004F28E0
// ??8?$VectorClass@PAVGraphicMenuItem@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004F2920
// ?Resize@?$VectorClass@PAVGraphicMenuItem@@@@UAE_NHPBQAVGraphicMenuItem@@@Z
// TEMPLATE: GAMEMD 0x004F29D0
// ?Clear@?$VectorClass@PAVGraphicMenuItem@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004F2A00
// ?ID@?$VectorClass@PAVGraphicMenuItem@@@@UAEHABQAVGraphicMenuItem@@@Z
// TEMPLATE: GAMEMD 0x004F2A40
// ?ID@?$VectorClass@PAVGraphicMenuItem@@@@UAEHPBQAVGraphicMenuItem@@@Z
// TEMPLATE: GAMEMD 0x004F2A60
// ?Resize@?$DynamicVectorClass@PAVGraphicMenuItem@@@@UAE_NHPBQAVGraphicMenuItem@@@Z
// TEMPLATE: GAMEMD 0x004F2B10
// ?ID@?$DynamicVectorClass@PAVGraphicMenuItem@@@@UAEHABQAVGraphicMenuItem@@@Z
// TEMPLATE: GAMEMD 0x004F2B40
// ??E?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x004F2B50
// ??F?$VectorCursor@PAVGraphicMenuItem@@V?$DynamicVectorClass@PAVGraphicMenuItem@@@@@@UAEAAV0@XZ
// SYNTHETIC: GAMEMD 0x004F2B60
// ??_G?$VectorClass@PAVGraphicMenuItem@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x004F2BB0
// ??_G?$DynamicVectorClass@PAVGraphicMenuItem@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x004F2C00
// ??0?$VectorClass@PAVGraphicMenuItem@@@@QAE@HPBQAVGraphicMenuItem@@@Z
template class VectorClass<GraphicMenuItem *>;
template class VectorCursor<GraphicMenuItem *, DynamicVectorClass<GraphicMenuItem *> >;
template class DynamicVectorClass<GraphicMenuItem *>;

// SYNTHETIC: GAMEMD 0x004F2C50 SYMBOL
// _$E12
Rect DAT_00A8EE98(0, 0, 0, 0);
