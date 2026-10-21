// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "straw.h"

class LCWStraw : public Straw
{
		typedef Straw BASECLASS;

	public:
		enum CompControl {
			COMPRESS,
			DECOMPRESS
		};

		LCWStraw(CompControl control, int blocksize=1024*8);
		virtual ~LCWStraw(void) override;

		virtual int Get(void * source, int slen) override;

	private:

		CompControl Control;

		int Counter;

		char * Buffer;
		char * Buffer2;

		int BlockSize;

		int SafetyMargin;

		struct {
			unsigned short CompCount;
			unsigned short UncompCount;
		} BlockHeader;

		LCWStraw(LCWStraw & rvalue);
		LCWStraw & operator = (LCWStraw const & pipe);
};
