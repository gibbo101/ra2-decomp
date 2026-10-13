// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "win.h"

class Blitter {
	public:
		virtual ~Blitter(void) {}

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const = 0;
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, unsigned short tint) const = 0;
		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 1000) const = 0;
		virtual void Blit_Move_Tinted(void * dest, void const * source, int length, int z_min, void * z_buff, void * a_buff, int alpha_level, unsigned short tint) const = 0;

		void Blit(void * dest, void const * source, int length) const {if (dest < source) BlitBackward(dest, source, length); else BlitForward(dest, source, length);}
};

class RLEBlitter {
	public:
		virtual ~RLEBlitter(void) {}

		virtual void Blit(void * dest, void const * source, int length, int leadskip = 0, int z_min = 0, void * z_buff = NULL, void * a_buff = NULL, int alpha_level = 0, int warp_offset = 0, void * zshape = NULL) const = 0;
		virtual void Blit_Copy_Tinted(void * dest, void const * source, int length, int leadskip, int z_min, void * z_buff, void * a_buff, int alpha_level, int warp_offset, void * zshape, unsigned short tint) const = 0;
};
