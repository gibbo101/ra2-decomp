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

extern int FUN_00407000(void);

// FUNCTION: 0x005D1AA0
MSEngine::MSEngine(void)
{
	RectCount = 0;

	Rects.Resize(20);
	Anims.Clear();
	Sounds.Clear();
}

// FUNCTION: 0x005D1B30
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

// FUNCTION: 0x005D1C20
int MSEngine::Add_Animation(MSAnim * anim)
{
	if (anim != NULL) {
		if (Anims.Add(anim)) {
			return(Anims.ID(anim));
		}
	}
	return(-1);
}

// FUNCTION: 0x005D1C90
void MSEngine::Replace_Anim(MSAnim * new_anim, MSAnim * old_anim)
{
	Anims.Insert_After(Anims.ID(old_anim), new_anim);
	Remove_Anim(old_anim);
}

// FUNCTION: 0x005D1D80
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

// FUNCTION: 0x005D1DF0
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

// FUNCTION: 0x005D1E50
void MSEngine::Restore_And_Advance(void)
{
	SurfacesRestored = true;
	Advance(HiddenSurface);
}

// FUNCTION: 0x005D1E70
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


// FUNCTION: 0x005D2260
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

// FUNCTION: 0x005D2350
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

// FUNCTION: 0x005D2410
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

// FUNCTION: 0x005D2530
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

// FUNCTION: 0x005D25D0
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

// FUNCTION: 0x005D2650
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

// FUNCTION: 0x005D26C0
void MSEngine::Restore_Anims(Rect const & rect)
{
	for (int i = 0; i < Anims.Count(); i++) {
		Anims[i]->Restore(rect);
	}
}

// FUNCTION: 0x005D26F0 MSEngine::Process_Idle

// FUNCTION: 0x005D2700 MSEngine::~MSEngine

// FUNCTION: 0x005D2720 ?$DynamicVectorClass@PAVMSAnim@@::Clear
template class DynamicVectorClass<MSAnim *>;

// FUNCTION: 0x005D2750 ?$DynamicVectorClass@PAVMSAnim@@::ID

// FUNCTION: 0x005D2770 ?$VectorClass@PAVMSAnim@@::GetItem

// FUNCTION: 0x005D2780 ?$DynamicVectorClass@PAVMSSfx@@::Clear
template class DynamicVectorClass<MSSfx *>;

// FUNCTION: 0x005D27B0 ?$DynamicVectorClass@PAVMSSfx@@::ID

// FUNCTION: 0x005D27D0 ?$VectorClass@PAVMSSfx@@::GetItem

// FUNCTION: 0x005D27E0 ?$VectorClass@PAVMSAnim@@::operator==

// FUNCTION: 0x005D2820 ?$VectorClass@PAVMSAnim@@::Resize

// FUNCTION: 0x005D28D0 ?$VectorClass@PAVMSAnim@@::Clear

// FUNCTION: 0x005D2900 ?$VectorClass@PAVMSAnim@@::ID

// FUNCTION: 0x005D2940 ?$VectorClass@PAVMSSfx@@::ID

// FUNCTION: 0x005D2960 ?$DynamicVectorClass@PAVMSAnim@@::Resize

// FUNCTION: 0x005D2A10 ?$DynamicVectorClass@PAVMSAnim@@::ID

// FUNCTION: 0x005D2A40 ?$VectorClass@PAVMSSfx@@::operator==

// FUNCTION: 0x005D2A80 ?$VectorClass@PAVMSSfx@@::Resize

// FUNCTION: 0x005D2B30 ?$VectorClass@PAVMSSfx@@::Clear
