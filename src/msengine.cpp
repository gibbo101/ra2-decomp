// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msengine.h"
#include "_surface.h"
#include "_xmouse.h"
#include "conquer.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "misc.h"
#include "msanim.h"
#include "msgloop.h"
#include "mssfx.h"
#include "rect.h"
#include "surface.h"
#include "win.h"
#include "color.hh"
#include <climits>

// SYNTHETIC: GAMEMD 0x005D1A80 SYMBOL
// _$E2

extern int FUN_00407000(void);

// FUNCTION: GAMEMD 0x005D1AA0
MSEngine::MSEngine(void)
{
	RectCount = 0;

	Rects.Resize(20);
	Anims.Clear();
	Sounds.Clear();
}

// FUNCTION: GAMEMD 0x005D1B30
MSEngine::~MSEngine(void)
{
	int index;

	for (index = 0; index < Sounds.Count(); index++) {
		delete Sounds[index];
	}
	Sounds.Clear();

	for (index = 0; index < Anims.Count(); index++) {
		delete Anims[index];
	}
	Anims.Clear();

	Rects.Clear();
}

// FUNCTION: GAMEMD 0x005D1C20
int MSEngine::Add_Animation(MSAnim * anim)
{
	if (anim != NULL) {
		if (Anims.Add(anim)) {
			return(Anims.ID(anim));
		}
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x005D1C90
void MSEngine::Replace_Anim(MSAnim * new_anim, MSAnim * old_anim)
{
	Anims.Insert_After(Anims.ID(old_anim), new_anim);
	Remove_Anim(old_anim);
}

// FUNCTION: GAMEMD 0x005D1D80
void MSEngine::Remove_Anim(MSAnim * anim)
{
	if (anim) {
		if (Anims.ID(anim) != -1) {
			Anims.Delete(anim);
			if (anim) {
				delete anim;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005D1DF0
void MSEngine::Wait_For_Anim(MSAnim * anim, unsigned delay)
{
	CDTimerClass<SystemTimerClass> timer = delay;

	while (true) {
		int id = Anims.ID(anim);
		if (id == -1) {
			break;
		}

		if (Anims[id]->Has_Finished()) {
			break;
		}

		if (delay <= 0) {
			break;
		}

		Wait_Delay(1);
	}
}

// FUNCTION: GAMEMD 0x005D1E50
void MSEngine::Restore_And_Advance(void)
{
	SurfacesRestored = true;
	Advance(HiddenSurface);
}

// FUNCTION: GAMEMD 0x005D1E70
void MSEngine::Advance(Surface * surface)
{
	if (SurfacesRestored == true) {
		SurfacesRestored = false;
		surface->Fill(TBLACK);
		Rect rect = AlternateSurface->Get_Rect();
		rect.X = (surface->Get_Width() - rect.Width) / 2;
		rect.Y = (surface->Get_Height() - rect.Height) / 2;
		surface->Blit_From(AlternateSurface->Get_Rect(), *AlternateSurface, rect);
		for (int i = 0; i < Anims.Count(); i++) {
			Anims[i]->Redraw(surface);
		}
		RectCount = 0;
		Add_Update_Rect(surface->Get_Rect());
		Do_Custom_Draw(surface);
	}

	for (int i = 0; i < Anims.Count(); i++) {
		Rect rect;
		if (Anims[i]->Advance(surface, rect) == true) {
			delete Anims[i];
			Anims.Delete_Index(i);
		}
		Add_Update_Rect(rect);
	}
}


static inline bool Overlaps(Rect const & rect1, Rect const & rect2)
{
	bool overlapping = false;
	if (rect1.X < rect2.X) {
		if (rect1.X + rect1.Width >= rect2.X) {
			overlapping = true;
		}
	} else {
		if (rect2.X + rect2.Width >= rect1.X) {
			overlapping = true;
		}
	}

	if (rect1.Y < rect2.Y) {
		if (rect1.Y + rect1.Height < rect2.Y) {
			overlapping = false;
		}
	} else {
		if (rect2.Y + rect2.Height < rect1.Y) {
			overlapping = false;
		}
	}
	return(overlapping);
}

// FUNCTION: GAMEMD 0x005D1FD0
void MSEngine::Add_Update_Rect(Rect const & rect)
{
	int index;

	if (!rect.Is_Valid()) {
		return;
	}

	for (index = 0; index < RectCount; index++) {
		const Rect &update_rect = Rects[index];
		if (Overlaps(rect, update_rect) == true) {
			int rect_right = rect.X + rect.Width;
			int rect_bottom = rect.Y + rect.Height;
			int i_rect_right = update_rect.X + update_rect.Width;
			int i_rect_bottom = update_rect.Y + update_rect.Height;

			Rect new_rect;
			new_rect.X = MIN(rect.X, update_rect.X);
			new_rect.Y = MIN(rect.Y, update_rect.Y);
			new_rect.Width = rect_right > i_rect_right ? rect_right - new_rect.X : i_rect_right - new_rect.X;
			new_rect.Height = rect_bottom > i_rect_bottom ? rect_bottom - new_rect.Y : i_rect_bottom - new_rect.Y;

			RectCount--;
			for (; index < RectCount; index++) {
				Rects[index] = Rects[index + 1];
			}

			Add_Update_Rect(new_rect);
			return;
		}
	}

	if (RectCount < Rects.Length()) {
		Rects[RectCount] = rect;
		RectCount++;
		return;
	}

	unsigned min_size = UINT_MAX;
	Rect min_rect(0, 0, 0, 0);
	int min_id = 0;

	for (index = 0; index < RectCount; index++) {
		const Rect &entry = Rects[index];
		int entry_left = entry.X;
		int entry_right = entry_left + entry.Width;
		int entry_top = entry.Y;
		int entry_bottom = entry_top + entry.Height;

		int left = MIN(rect.X, entry_left);
		int top = MIN(rect.Y, entry_top);
		int rect_right = rect.X + rect.Width;
		int width = rect_right > entry_right ? rect_right - left : entry_right - left;
		int bottom = MAX(rect.Y + rect.Height, entry_bottom);
		unsigned area = width * (bottom - top);

		if (area < min_size) {
			min_size = area;
			min_id = index;
			min_rect.Set(left, top, width, bottom - top);
		}
	}

	Rects[min_id] = min_rect;
}

// FUNCTION: GAMEMD 0x005D2260
void MSEngine::Blit_All(Surface * surface)
{
	RECT client_rect;

	if (RectCount > 0) {

		GetClientRect(MainWindow, (LPRECT)&client_rect);
		ClientToScreen(MainWindow, (LPPOINT)&client_rect.left);

		if (MouseCursor) {
			MouseCursor->Draw_Mouse(surface);
		}

		for (int i = 0; i < RectCount; i++) {
			Rect offset_rect = Rects[i];
			offset_rect.X += client_rect.left;
			offset_rect.Y += client_rect.top;
			VisibleSurface->Blit_From(offset_rect, *surface, Rects[i]);
		}

		RectCount = 0;

		if (MouseCursor) {
			MouseCursor->Erase_Mouse(surface);
		}
	}
}

// FUNCTION: GAMEMD 0x005D2350
void MSEngine::Blit_Rect(Surface * surface, Rect const & rect)
{
	RECT client_rect;

	if (rect.Is_Valid()) {

		GetClientRect(MainWindow, (LPRECT)&client_rect);
		ClientToScreen(MainWindow, (LPPOINT)&client_rect.left);

		if (MouseCursor) {
			MouseCursor->Draw_Mouse(surface);
		}

		Rect offset_rect = rect;
		offset_rect.X += client_rect.left;
		offset_rect.Y += client_rect.top;
		VisibleSurface->Blit_From(offset_rect, *surface, rect);

		if (MouseCursor) {
			MouseCursor->Erase_Mouse(surface);
		}
	}
}

// FUNCTION: GAMEMD 0x005D2410
void MSEngine::Wait_Delay(int delay)
{
	CDTimerClass<SystemTimerClass> timer = 1;

	do {
		do {
			Call_Back();
			Process_Idle();
			Wait_Blit();
			Advance(HiddenSurface);
			Blit_All(HiddenSurface);
			Windows_Message_Handler();

			if (!GameInFocus) {
				timer.Stop();
				Wait_For_Focus();
				timer.Start();
			}

			Sleep(0);

		} while (timer.Value() > 0);

		timer = 1;
		delay--;

	} while (delay != 0);
}

// FUNCTION: GAMEMD 0x005D2530
void MSEngine::Wait_For_Focus(void)
{
	int index;

	if (GameInFocus != true) {
		DebugString("MSEngine - Pausing animations\n");
		for (index = 0; index < Anims.Count(); index++) {
			Anims[index]->Pause();
		}

		while (!GameInFocus) {
			DebugString("MSEngine - Sleeping\n");
			Sleep(500);
			Windows_Message_Handler();
		}

		DebugString("MSEngine - Resuming animations\n");
		for (index = 0; index < Anims.Count(); index++) {
			Anims[index]->Resume();
		}
	}
}

// FUNCTION: GAMEMD 0x005D25D0
bool MSEngine::Add_Sound_Effect(char const * name, char const * file_name)
{
	if (name == NULL || file_name == NULL) {
		return(false);
	}

	if (Sounds.Add(new MSSfx(name, file_name))) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x005D2650
void MSEngine::Play_Sound_Effect(char const * name, int volume)
{
	if (name == NULL || !FUN_00407000()) {
		return;
	}

	for (int i = 0; i < Sounds.Count(); i++) {
		MSSfx * sfx = Sounds[i];
		if (stricmp(name, sfx->Get_Name()) == 0) {
			sfx->Play(int(volume * Options.SoundVolume));
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x005D26C0
void MSEngine::Restore_Anims(Rect const & rect)
{
	for (int i = 0; i < Anims.Count(); i++) {
		Anims[i]->Restore(rect);
	}
}

// FUNCTION: GAMEMD 0x005D26F0
// MSEngine::Process_Idle

// SYNTHETIC: GAMEMD 0x005D2700
// ??_GMSEngine@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x005D2720
// ?Clear@?$DynamicVectorClass@PAVMSAnim@@@@UAEXXZ
template class DynamicVectorClass<MSAnim *>;

// TEMPLATE: GAMEMD 0x005D2750
// ?ID@?$DynamicVectorClass@PAVMSAnim@@@@UAEHPBQAVMSAnim@@@Z

// TEMPLATE: GAMEMD 0x005D2770
// ?GetItem@?$VectorClass@PAVMSAnim@@@@UBEPAVMSAnim@@H@Z

// TEMPLATE: GAMEMD 0x005D2780
// ?Clear@?$DynamicVectorClass@PAVMSSfx@@@@UAEXXZ
template class DynamicVectorClass<MSSfx *>;

// TEMPLATE: GAMEMD 0x005D27B0
// ?ID@?$DynamicVectorClass@PAVMSSfx@@@@UAEHPBQAVMSSfx@@@Z

// TEMPLATE: GAMEMD 0x005D27D0
// ?GetItem@?$VectorClass@PAVMSSfx@@@@UBEPAVMSSfx@@H@Z

// TEMPLATE: GAMEMD 0x005D27E0
// ??8?$VectorClass@PAVMSAnim@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x005D2820
// ?Resize@?$VectorClass@PAVMSAnim@@@@UAE_NHPBQAVMSAnim@@@Z

// TEMPLATE: GAMEMD 0x005D28D0
// ?Clear@?$VectorClass@PAVMSAnim@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x005D2900
// ?ID@?$VectorClass@PAVMSAnim@@@@UAEHABQAVMSAnim@@@Z

// TEMPLATE: GAMEMD 0x005D2940
// ?ID@?$VectorClass@PAVMSAnim@@@@UAEHPBQAVMSAnim@@@Z

// TEMPLATE: GAMEMD 0x005D2960
// ?Resize@?$DynamicVectorClass@PAVMSAnim@@@@UAE_NHPBQAVMSAnim@@@Z

// TEMPLATE: GAMEMD 0x005D2A10
// ?ID@?$DynamicVectorClass@PAVMSAnim@@@@UAEHABQAVMSAnim@@@Z

// TEMPLATE: GAMEMD 0x005D2A40
// ??8?$VectorClass@PAVMSSfx@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x005D2A80
// ?Resize@?$VectorClass@PAVMSSfx@@@@UAE_NHPBQAVMSSfx@@@Z

// TEMPLATE: GAMEMD 0x005D2B30
// ?Clear@?$VectorClass@PAVMSSfx@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x005D2B60
// ?ID@?$VectorClass@PAVMSSfx@@@@UAEHABQAVMSSfx@@@Z

// TEMPLATE: GAMEMD 0x005D2BA0
// ?ID@?$VectorClass@PAVMSSfx@@@@UAEHPBQAVMSSfx@@@Z

// TEMPLATE: GAMEMD 0x005D2BC0
// ?Resize@?$DynamicVectorClass@PAVMSSfx@@@@UAE_NHPBQAVMSSfx@@@Z

// TEMPLATE: GAMEMD 0x005D2C70
// ?ID@?$DynamicVectorClass@PAVMSSfx@@@@UAEHABQAVMSSfx@@@Z

// SYNTHETIC: GAMEMD 0x005D2CA0
// ??_G?$VectorClass@PAVMSAnim@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005D2CF0
// ??_G?$DynamicVectorClass@PAVMSAnim@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005D2D40
// ??_G?$VectorClass@PAVMSSfx@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005D2D90
// ??_G?$DynamicVectorClass@PAVMSSfx@@@@UAEPAXI@Z
