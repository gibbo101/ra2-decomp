// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

class PixelFXClass
{
	public:
		PixelFXClass(void);
		PixelFXClass(int type);
		virtual ~PixelFXClass(void);

		void Init(int type);
		void Update(int frames);
		bool Expire(int frames);
		void Set_Offset(int x, int y);

	public:
		int Current[3];
		int Phase;
		int Rate;
		int ColorA[3];
		int ColorB[3];
		int OffsetX;
		int OffsetY;
		int Life;
};
