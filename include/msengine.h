// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rect.h"
#include "vector.h"

class MSAnim;
class MSSfx;
template<class T> class TRect;
typedef TRect<int> Rect;
class Surface;

class MSEngine
{
	public:
		MSEngine(void);
		virtual ~MSEngine(void);
		virtual void Do_Custom_Draw(Surface * surface) {}
		virtual void Process_Idle(void) {}

		int Add_Animation(MSAnim * anim);
		void Replace_Anim(MSAnim * new_anim, MSAnim * old_anim);
		void Remove_Anim(MSAnim * anim);
		void Wait_For_Anim(MSAnim * anim, unsigned delay=300000);
		DynamicVectorClass<MSAnim *> * Get_Anims(void) { return(&Anims); }

		void Restore_And_Advance(void);
		void Advance(Surface * surface);
		void Restore_Anims(Rect const & rect);

		void Add_Update_Rect(Rect const & rect);
		void Blit_All(Surface * surface);
		void Blit_Rect(Surface * surface, Rect const & rect);

		void Wait_Delay(int delay);
		void Wait_For_Focus(void);

		bool Add_Sound_Effect(char const * name, char const * file_name);
		void Play_Sound_Effect(char const * name, int volume);

	protected:

		int RectCount;

		VectorClass<Rect> Rects;

		DynamicVectorClass<MSAnim *> Anims;

		DynamicVectorClass<MSSfx *> Sounds;
};
