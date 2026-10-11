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

// FUNCTION: 0x005D26C0
void MSEngine::Restore_Anims(Rect const & rect)
{
	for (int i = 0; i < Anims.Count(); i++) {
		Anims[i]->Restore(rect);
	}
}
