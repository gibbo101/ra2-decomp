// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "point.h"
#include "rect.h"

class MixFileClass;
class DSurface;
class SHPStruct;
class Surface;

class LoadProgressManager
{
	public:
		LoadProgressManager(void);

		void FUN_00554350(Surface * target);
		void FUN_00554370(void);

		virtual bool FUN_00554400(int message, int extra);


		static void Create(void);
		static void Destroy(void);

		void Compute_Rects(void);
		Point2D Text_Origin(void);
		void Load_Mixes(void);

		static LoadProgressManager * Instance;

		MixFileClass * LoadMix;
		MixFileClass * LoadMDMix;
		Rect TitleBarRect;
		Rect LoadBarSHPRect;
		Rect LoadScreenSHPRect;
		wchar_t * LoadMessage;
		wchar_t * LoadBriefing;
		SHPStruct * TitleBarSHP;
		SHPStruct * LoadScreenSHP;
		SHPStruct * LoadBarSHP;
		bool TitleBarSHP_loaded;
		bool LoadScreenSHP_loaded;
		bool LoadBarSHP_loaded;
		int Field54;
		int Field58;
		int Field5C;
		DSurface * ProgressSurface;
};
