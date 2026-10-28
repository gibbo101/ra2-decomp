// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors

#include <stdlib.h>
#include "always.h"
#include "geometry.h"
#include "pixelfx.h"

// FUNCTION: GAMEMD 0x00631D40
void PixelFXClass::Init(int type)
{
	PixelFXTypeStruct * t = &DAT_008367C8[type];
	unsigned r = rand();
	OffsetX = (r & 0x3F) - 0x1F;
	OffsetY = ((r >> 5) & 0x1F) - 0xF;
	ColorA[0] = t->Color[0];
	ColorA[1] = t->Color[1];
	ColorA[2] = t->Color[2];
	if (t->Jitter) {
		int mask = (1 << t->Jitter) - 1;
		r = rand();
		ColorA[0] -= mask & r;
		r >>= t->Jitter;
		ColorA[1] -= mask & r;
		r >>= t->Jitter;
		ColorA[2] -= mask & r;
	}
	Current[0] = ColorB[0] = t->Dim[0];
	Current[1] = ColorB[1] = t->Dim[1];
	Current[2] = ColorB[2] = t->Dim[2];
	Phase = 0;
	unsigned range = t->RateMax - t->RateMin + 1;
	Rate = rand() % range + t->RateMin;
	Life = rand() & t->LifeMask;
}

// FUNCTION: GAMEMD 0x00631E10
PixelFXClass::PixelFXClass(void)
{
	Init(0);
}

// FUNCTION: GAMEMD 0x00631E30
PixelFXClass::PixelFXClass(int type)
{
	Init(type);
}

// FUNCTION: GAMEMD 0x00631E50
void PixelFXClass::Update(int frames)
{
	Phase += Rate * frames;
	if (Phase > 0x2000) {
		Phase = 0x2000;
	}
	int t = Phase & 0xFFF;
	if (Phase & 0x1000) {
		t = 0x1000 - t;
	}
	Current[0] = (ColorB[0] * (0x1000 - t) + ColorA[0] * t) >> 12;
	Current[1] = (ColorB[1] * (0x1000 - t) + ColorA[1] * t) >> 12;
	Current[2] = (ColorB[2] * (0x1000 - t) + ColorA[2] * t) >> 12;
}

// FUNCTION: GAMEMD 0x00631EE0
bool PixelFXClass::Expire(int frames)
{
	Life -= frames;
	if (Life <= 0) {
		Life = 0;
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00631F10
void PixelFXClass::Set_Offset(int x, int y)
{
	OffsetX = x;
	OffsetY = y;
}

// FUNCTION: GAMEMD 0x00631F30
PixelFXClass::~PixelFXClass(void)
{
}

// SYNTHETIC: GAMEMD 0x00631F40
// ??_GPixelFXClass@@UAEPAXI@Z
