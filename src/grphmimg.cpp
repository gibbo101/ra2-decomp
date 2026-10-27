// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmimg.h"
#include "_surface.h"
#include "ccfile.h"
#include "ini.h"
#include "msanim.h"
#include "mschoice.h"

// FUNCTION: GAMEMD 0x004F3140
GraphicMenuItem * __fastcall GM_Read_Image_Item(Wstring const & name, INIClass const & ini, MSEngine & engine, Point2D & image_size)
{
	int id = ini.Get_Int(name, "ID", -1);
	if (id == -1) {
		return(NULL);
	}

	Point2D origin(0,0);
	origin = ini.Get_Point(name, "Origin", origin);
	origin += image_size;

	Rect active_rect(0,0,0,0);
	active_rect = ini.Get_Rect(name, "ActiveRect", active_rect);
	active_rect += image_size;

	Wstring image;
	Wstring highlighted;
	Wstring disabled;

	ini.Get_String(name, "Image", "", image);
	ini.Get_String(name, "Highlighted", "", highlighted);
	ini.Get_String(name, "Disabled", "", disabled);
	Wstring highlight_sound;
	ini.Get_String(name, "HighlightSound", "", highlight_sound);
	Wstring select_vq;
	ini.Get_String(name, "SelectVQ", "", select_vq);

	return(new GraphicMenuImageItem(id, engine, origin, active_rect, image, highlighted, disabled, highlight_sound, select_vq));
}

// FUNCTION: GAMEMD 0x004F3460
GraphicMenuImageItem::GraphicMenuImageItem(int id, MSEngine & engine, Point2D const & origin, Rect const & rect, Wstring const & image, Wstring const & highlight_image, Wstring const & disabled_image, Wstring const & highlight_sound, Wstring const & select_vq) :
	GraphicMenuItem(id),
	Engine(&engine),
	ActiveRect(rect),
	Image(NULL),
	HighlightImage(NULL),
	DisabledImage(NULL),
	HighlightSound(NULL)
{
	HighlightSound = highlight_sound.length() > 0 ? new MSSfxEntry("HighlightSound", highlight_sound) : NULL;

	SelectVQ = select_vq;

	if (highlight_image.length() > 0) {
		HighlightImage = new MSPCXAnim(highlight_image, engine.Get_Anims(), origin, true);
		if (HighlightImage != NULL) {
			HighlightImage->Set_Active(false);
			engine.Add_Animation(HighlightImage);
		}
	}

	if (image.length() > 0) {
		Image = new MSPCXAnim(image, engine.Get_Anims(), origin, true);
		if (Image != NULL) {
			engine.Add_Animation(Image);
		}
	}

	if (disabled_image.length() > 0) {
		DisabledImage = new MSPCXAnim(disabled_image, engine.Get_Anims(), origin, true);
		if (DisabledImage != NULL) {
			DisabledImage->Set_Active(false);
			engine.Add_Animation(DisabledImage);
		}
	}
}

// FUNCTION: GAMEMD 0x004F3600
GraphicMenuImageItem::~GraphicMenuImageItem(void)
{
	delete(HighlightSound);
}

// FUNCTION: GAMEMD 0x004F3640
bool GraphicMenuImageItem::Is_Mouse_Over(Point2D const & mouse)
{
	return(Enabled && ActiveRect.Is_Point_Within(mouse));
}

// FUNCTION: GAMEMD 0x004F3690
void GraphicMenuImageItem::On_Selected_Change(bool selected)
{
	if (Image != NULL) {
		Image->Set_Active(Enabled && !selected);
	}
	if (HighlightImage != NULL) {
		HighlightImage->Set_Active(Enabled && selected);
	}
	if (DisabledImage != NULL) {
		DisabledImage->Set_Active(Enabled == false);
	}
	Engine->Restore_Anims(ActiveRect);
	Engine->Restore_And_Advance();
	if (selected) {
		if (HighlightSound != NULL) {
			HighlightSound->Play();
		}
	}
}

// FUNCTION: GAMEMD 0x004F3720
void GraphicMenuImageItem::On_Enabled_Change(bool active)
{
	if (Image != NULL) {
		Image->Set_Active(active && !Selected);
	}
	if (HighlightImage != NULL) {
		HighlightImage->Set_Active(active && Selected);
	}
	if (DisabledImage != NULL) {
		DisabledImage->Set_Active(active == false);
	}
	Engine->Restore_Anims(ActiveRect);
	Engine->Restore_And_Advance();
}

// FUNCTION: GAMEMD 0x004F37A0
void GraphicMenuImageItem::Action(MSEngine * engine)
{
	GraphicMenuItem::Action(engine);
	if (CCFileClass(SelectVQ).Is_Available()) {
		MSAnim * anim = new MSVQAnim(SelectVQ, AlternateSurface, engine->Get_Anims(), true);
		if (anim != NULL) {
			engine->Wait_For_Anim(anim);
		}
	}
}

// SYNTHETIC: GAMEMD 0x004F3840
// ??_GGraphicMenuImageItem@@UAEPAXI@Z
