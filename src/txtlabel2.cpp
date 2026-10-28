// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "rgb.h"
#include "msanim.h"
#include "convert.h"
#include "palette.h"
#include "ccfile.h"
#include "dsurface.h"
#include "_rect.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x0072A6B0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0072A6E0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0072A700 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0072A720 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0072A740 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0072A760 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0072A780 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0072A7C0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0072A7F0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0072A820 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0072A850 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0072A860 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0072A880 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x0072A8A0 SYMBOL
// _$E67
RGBClass DAT_00B0FB4C(255,255,0);

// SYNTHETIC: GAMEMD 0x0072A8C0 SYMBOL
// _$E70
RGBClass DAT_00B0FA94(72,0,0);

// SYNTHETIC: GAMEMD 0x0072A8E0 SYMBOL
// _$E73
RGBClass DAT_00B0F9FC(0,82,117);

// SYNTHETIC: GAMEMD 0x0072A900 SYMBOL
// _$E76
RGBClass DAT_00B0FB14(72,0,0);

// SYNTHETIC: GAMEMD 0x0072A920 SYMBOL
// _$E79
RGBClass DAT_00B0FB19(72,0,0);

// SYNTHETIC: GAMEMD 0x0072A940 SYMBOL
// _$E82
RGBClass DAT_00B0F9D8(164,210,255);

// SYNTHETIC: GAMEMD 0x0072A960 SYMBOL
// _$E85
RGBClass DAT_00B0FB04(255,255,0);

// SYNTHETIC: GAMEMD 0x0072A980 SYMBOL
// _$E88
RGBClass DAT_00B0FAA0(255,255,0);

// SYNTHETIC: GAMEMD 0x0072A9A0 SYMBOL
// _$E91
RGBClass DAT_00B0FA1C(0,0,0);

// FUNCTION: GAMEMD 0x0072A9C0
Point2D __fastcall FUN_0072A9C0(Rect const * rect)
{
	return Point2D(rect->X, rect->Y);
}

// FUNCTION: GAMEMD 0x0072A9D0
void __fastcall FUN_0072A9D0(Point2D * dest, Point2D const * src)
{
	*dest = *src;
}

// FUNCTION: GAMEMD 0x0072A9E0
void __fastcall FUN_0072A9E0(Rect * rect, int amount)
{
	rect->X += amount;
	rect->Y += amount;
	rect->Width -= amount * 2;
	rect->Height -= amount * 2;
}

// FUNCTION: GAMEMD 0x0072AA10
void __fastcall FUN_0072AA10(Rect * rect, int dx, int dy)
{
	rect->X += dx;
	rect->Y += dy;
	rect->Width -= dx * 2;
	rect->Height -= dy * 2;
}

struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
};
namespace OwnerDraw { extern SHPStruct * CloseButtonShape; }

void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & allocated);
void * Load_Alloc_Data(FileClass & file);
void __fastcall FUN_0072ADE0(char const * name, PaletteClass ** palette, ConvertClass ** convert);
void FUN_00534E50(void);
void FUN_00534DE0(void);
void __fastcall FUN_0072EC70(int width, int height);
void FUN_0072EF60(void);
void FUN_0072F140(void);
void FUN_00730100(void);
void FUN_00730400(void);
void FUN_00730530(void);
void FUN_0072B130(void);
void FUN_0072B230(void);
extern DSurface * DAT_00887310;
#include "goptions.h"
extern GameOptionsClass Options;
namespace Unsorted { extern bool ArmageddonMode; }
extern char const * DAT_00844C44;
extern char const * DAT_00844C48;
extern char const * DAT_00844C4C;
extern char const * DAT_00844C50;
extern char const * DAT_00844C54;
extern bool DAT_00B0FC90;
extern bool DAT_00B0FC91;
extern bool DAT_00B0FC92;
extern bool DAT_00B0FC93;
extern bool DAT_00B0FC94;
extern SHPStruct * DAT_00B0FC80;
extern SHPStruct * DAT_00B0FC84;
extern SHPStruct * DAT_00B0FC88;
extern SHPStruct * DAT_00B0FC8C;
extern char const * DAT_00844BA4;
extern char const * DAT_00844BA0;
extern char const * DAT_00844B9C;
extern char const * DAT_00844BA8;
extern char const * DAT_00844BE4;
extern char const * DAT_00844BE8;
extern char const * DAT_00844BEC;
extern PaletteClass * DAT_00B0FB64;
extern PaletteClass * DAT_00B0FB6C;
extern PaletteClass * DAT_00B0FB5C;
extern PaletteClass * DAT_00B0FB74;
extern PaletteClass * DAT_00B0FBC8;
extern PaletteClass * DAT_00B0FBD0;
extern PaletteClass * DAT_00B0FBD8;
extern ConvertClass * DAT_00B0FB68;
extern ConvertClass * DAT_00B0FB70;
extern ConvertClass * DAT_00B0FB60;
extern ConvertClass * DAT_00B0FB78;
extern ConvertClass * DAT_00B0FBCC;
extern ConvertClass * DAT_00B0FBD4;
extern ConvertClass * DAT_00B0FBDC;
extern bool DAT_00B0FBE0;
extern bool DAT_00B0FC0C;
extern char const * DAT_00844CD4;
extern char const * DAT_00844CD8;
extern char const * DAT_00844CDC;
extern char const * DAT_00844CE0;
extern char const * DAT_00844CE4;
extern char const * DAT_00844CE8;
extern char const * DAT_00844CEC;
extern char const * DAT_00844CF0;
extern char const * DAT_00844CF4;
extern char const * DAT_00844CF8;
extern bool DAT_00B0FC95;
extern bool DAT_00B0FC96;
extern bool DAT_00B0FC97;
extern bool DAT_00B0FC98;
extern bool DAT_00B0FC99;
extern bool DAT_00B0FC9A;
extern bool DAT_00B0FC9B;
extern bool DAT_00B0FC9C;
extern bool DAT_00B0FC9D;
extern bool DAT_00B0FC9E;
extern SHPStruct * DAT_00B0FAC4;
extern SHPStruct * DAT_00B0F9DC;
extern SHPStruct * DAT_00B0FAC0;
extern SHPStruct * DAT_00B0FB50;
extern SHPStruct * DAT_00B0FA04;
extern SHPStruct * DAT_00B0FAF8;
extern SHPStruct * DAT_00B0FA74;
extern SHPStruct * DAT_00B0FA38;
extern SHPStruct * DAT_00B0FAE8;
extern SHPStruct * DAT_00B0FA54;
extern void * DAT_00B0FC10;
extern void * DAT_00B0FC14;
extern void * DAT_00B0FC18;
extern void * DAT_00B0FC1C;
extern void * DAT_00B0FC20;
extern void * DAT_00B0FC24;
extern void * DAT_00B0FC28;
extern void * DAT_00B0FC2C;

extern char const * DAT_00844C34;
extern char const * DAT_00844C38;
extern char const * DAT_00844C3C;
extern char const * DAT_00844C40;
extern char const * DAT_00844BAC;
extern char const * DAT_00844BB0;
extern char const * DAT_00844BB4;
extern char const * DAT_00844BB8;
extern char const * DAT_00844BBC;
extern PaletteClass * DAT_00B0FB7C;
extern PaletteClass * DAT_00B0FB84;
extern PaletteClass * DAT_00B0FB8C;
extern PaletteClass * DAT_00B0FB94;
extern PaletteClass * DAT_00B0FB9C;
extern ConvertClass * DAT_00B0FB80;
extern ConvertClass * DAT_00B0FB88;
extern ConvertClass * DAT_00B0FB90;
extern ConvertClass * DAT_00B0FB98;
extern ConvertClass * DAT_00B0FBA0;

// FUNCTION: GAMEMD 0x0072AA40
void FUN_0072AA40(void)
{
	DAT_00B0FC80 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C44, DAT_00B0FC90);
	DAT_00B0FC84 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C48, DAT_00B0FC91);
	DAT_00B0FC88 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C4C, DAT_00B0FC92);
	DAT_00B0FC8C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C50, DAT_00B0FC93);
	OwnerDraw::CloseButtonShape = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C54, DAT_00B0FC94);
	FUN_0072ADE0(DAT_00844BA4, &DAT_00B0FB64, &DAT_00B0FB68);
	FUN_0072ADE0(DAT_00844BA0, &DAT_00B0FB6C, &DAT_00B0FB70);
	FUN_0072ADE0(DAT_00844B9C, &DAT_00B0FB5C, &DAT_00B0FB60);
	FUN_0072ADE0(DAT_00844BA8, &DAT_00B0FB74, &DAT_00B0FB78);
	if (!DAT_00B0FBE0) {
		FUN_00534E50();
		DAT_00B0FAC4 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CD4, DAT_00B0FC95);
		DAT_00B0F9DC = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CD8, DAT_00B0FC96);
		DAT_00B0FAC0 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CDC, DAT_00B0FC97);
		DAT_00B0FB50 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CE0, DAT_00B0FC98);
		DAT_00B0FA04 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CE4, DAT_00B0FC99);
		DAT_00B0FAF8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CE8, DAT_00B0FC9A);
		DAT_00B0FA74 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CEC, DAT_00B0FC9B);
		DAT_00B0FA38 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CF0, DAT_00B0FC9C);
		DAT_00B0FAE8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CF4, DAT_00B0FC9D);
		DAT_00B0FA54 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CF8, DAT_00B0FC9E);
		FUN_0072ADE0(DAT_00844BE4, &DAT_00B0FBC8, &DAT_00B0FBCC);
		FUN_0072ADE0(DAT_00844BE8, &DAT_00B0FBD0, &DAT_00B0FBD4);
		FUN_0072ADE0(DAT_00844BEC, &DAT_00B0FBD8, &DAT_00B0FBDC);
		FUN_0072EC70(Drawing::RenderWidth, Drawing::RenderHeight);
		DAT_00B0FBE0 = true;
	}
}

// FUNCTION: GAMEMD 0x0072AC40
void FUN_0072AC40(void)
{
	FUN_0072B130();
	FUN_0072B230();
	if (DAT_00B0FBE0) {
		DAT_00B0FBE0 = false;
		FUN_0072EF60();
		FUN_0072F140();
		delete DAT_00B0FC10;
		DAT_00B0FC10 = NULL;
		delete DAT_00B0FC14;
		DAT_00B0FC14 = NULL;
		delete DAT_00B0FC18;
		DAT_00B0FC18 = NULL;
		delete DAT_00B0FC1C;
		DAT_00B0FC1C = NULL;
		delete DAT_00B0FC20;
		DAT_00B0FC20 = NULL;
		delete DAT_00B0FC24;
		DAT_00B0FC24 = NULL;
		delete DAT_00B0FC28;
		DAT_00B0FC28 = NULL;
		delete DAT_00B0FC2C;
		DAT_00B0FC2C = NULL;
		FUN_00534DE0();
	}
	FUN_00730100();
	FUN_00730400();
	FUN_00730530();
	DAT_00B0FC0C = false;
}

// FUNCTION: GAMEMD 0x0072AD90
Rect __fastcall FUN_0072AD90(void)
{
	Rect r;
	r.Width = Options.ScreenWidth;
	r.X = 0;
	r.Y = 0;
	r.Height = Options.ScreenHeight;
	if (!Options.IsSidebarOnRight && !Unsorted::ArmageddonMode) {
		r.X = 0xa8;
	}
	r.Width -= 0xa8;
	r.Height -= 0x20;
	return r;
}

// FUNCTION: GAMEMD 0x0072AF10
void FUN_0072AF10(void)
{
	DAT_00B0FC80 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C44, DAT_00B0FC90);
	DAT_00B0FC84 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C48, DAT_00B0FC91);
	DAT_00B0FC88 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C4C, DAT_00B0FC92);
	DAT_00B0FC8C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C50, DAT_00B0FC93);
	OwnerDraw::CloseButtonShape = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C54, DAT_00B0FC94);
	FUN_0072ADE0(DAT_00844BA4, &DAT_00B0FB64, &DAT_00B0FB68);
	FUN_0072ADE0(DAT_00844BA0, &DAT_00B0FB6C, &DAT_00B0FB70);
	FUN_0072ADE0(DAT_00844B9C, &DAT_00B0FB5C, &DAT_00B0FB60);
	FUN_0072ADE0(DAT_00844BA8, &DAT_00B0FB74, &DAT_00B0FB78);
}

// FUNCTION: GAMEMD 0x0072AFD0
void FUN_0072AFD0(void)
{
	FUN_0072B130();
	FUN_0072B230();
}

// FUNCTION: GAMEMD 0x0072AFE0
PaletteClass * FUN_0072AFE0(void)
{
	return DAT_00B0FB64;
}

// FUNCTION: GAMEMD 0x0072AFF0
ConvertClass * FUN_0072AFF0(void)
{
	return DAT_00B0FB68;
}

// FUNCTION: GAMEMD 0x0072B000
PaletteClass * FUN_0072B000(void)
{
	return DAT_00B0FB6C;
}

// FUNCTION: GAMEMD 0x0072B010
ConvertClass * FUN_0072B010(void)
{
	return DAT_00B0FB70;
}

// FUNCTION: GAMEMD 0x0072B020
PaletteClass * FUN_0072B020(void)
{
	return DAT_00B0FB5C;
}

// FUNCTION: GAMEMD 0x0072B030
ConvertClass * FUN_0072B030(void)
{
	return DAT_00B0FB60;
}

// FUNCTION: GAMEMD 0x0072B040
PaletteClass * FUN_0072B040(void)
{
	return DAT_00B0FB74;
}

// FUNCTION: GAMEMD 0x0072B050
ConvertClass * FUN_0072B050(void)
{
	return DAT_00B0FB78;
}

// FUNCTION: GAMEMD 0x0072B060
void FUN_0072B060(void)
{
	DAT_00B0FC80 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C44, DAT_00B0FC90);
	DAT_00B0FC84 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C48, DAT_00B0FC91);
	DAT_00B0FC88 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C4C, DAT_00B0FC92);
	DAT_00B0FC8C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C50, DAT_00B0FC93);
	OwnerDraw::CloseButtonShape = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C54, DAT_00B0FC94);
}

// FUNCTION: GAMEMD 0x0072B0D0
void FUN_0072B0D0(void)
{
	FUN_0072ADE0(DAT_00844BA4, &DAT_00B0FB64, &DAT_00B0FB68);
	FUN_0072ADE0(DAT_00844BA0, &DAT_00B0FB6C, &DAT_00B0FB70);
	FUN_0072ADE0(DAT_00844B9C, &DAT_00B0FB5C, &DAT_00B0FB60);
	FUN_0072ADE0(DAT_00844BA8, &DAT_00B0FB74, &DAT_00B0FB78);
}

// FUNCTION: GAMEMD 0x0072B130
void FUN_0072B130(void)
{
	if (DAT_00B0FC90) {
		if (DAT_00B0FC80) {
			delete DAT_00B0FC80;
		}
		DAT_00B0FC90 = false;
	}
	DAT_00B0FC80 = NULL;
	if (DAT_00B0FC91) {
		if (DAT_00B0FC84) {
			delete DAT_00B0FC84;
		}
		DAT_00B0FC91 = false;
	}
	DAT_00B0FC84 = NULL;
	if (DAT_00B0FC92) {
		if (DAT_00B0FC88) {
			delete DAT_00B0FC88;
		}
		DAT_00B0FC92 = false;
	}
	DAT_00B0FC88 = NULL;
	if (DAT_00B0FC93) {
		if (DAT_00B0FC8C) {
			delete DAT_00B0FC8C;
		}
		DAT_00B0FC93 = false;
	}
	DAT_00B0FC8C = NULL;
	if (DAT_00B0FC94) {
		if (OwnerDraw::CloseButtonShape) {
			delete OwnerDraw::CloseButtonShape;
		}
		DAT_00B0FC94 = false;
	}
	OwnerDraw::CloseButtonShape = NULL;
}

// FUNCTION: GAMEMD 0x0072B230
void FUN_0072B230(void)
{
	if (DAT_00B0FB6C) {
		delete DAT_00B0FB6C;
		DAT_00B0FB6C = NULL;
	}
	if (DAT_00B0FB70) {
		delete [] DAT_00B0FB70;
		DAT_00B0FB70 = NULL;
	}
	if (DAT_00B0FB64) {
		delete DAT_00B0FB64;
		DAT_00B0FB64 = NULL;
	}
	if (DAT_00B0FB68) {
		delete [] DAT_00B0FB68;
		DAT_00B0FB68 = NULL;
	}
	if (DAT_00B0FB5C) {
		delete DAT_00B0FB5C;
		DAT_00B0FB5C = NULL;
	}
	if (DAT_00B0FB60) {
		delete [] DAT_00B0FB60;
		DAT_00B0FB60 = NULL;
	}
	if (DAT_00B0FB74) {
		delete DAT_00B0FB74;
		DAT_00B0FB74 = NULL;
	}
	if (DAT_00B0FB78) {
		delete [] DAT_00B0FB78;
		DAT_00B0FB78 = NULL;
	}
}

// FUNCTION: GAMEMD 0x0072B2F0
char const * __fastcall FUN_0072B2F0(int width, int height, int side)
{
	char const * name = DAT_00844C34;
	if (width != 640) {
		name = DAT_00844C38;
	}
	return name;
}

// FUNCTION: GAMEMD 0x0072B310
char const * __fastcall FUN_0072B310(int width, int height, int side)
{
	char const * name = DAT_00844C3C;
	if (width != 640) {
		name = DAT_00844C40;
	}
	return name;
}

// FUNCTION: GAMEMD 0x0072B330
PaletteClass * FUN_0072B330(void)
{
	return DAT_00B0FB7C;
}

// FUNCTION: GAMEMD 0x0072B340
ConvertClass * FUN_0072B340(void)
{
	return DAT_00B0FB80;
}

// FUNCTION: GAMEMD 0x0072B350
PaletteClass * FUN_0072B350(void)
{
	return DAT_00B0FB84;
}

// FUNCTION: GAMEMD 0x0072B360
ConvertClass * FUN_0072B360(void)
{
	return DAT_00B0FB88;
}

// FUNCTION: GAMEMD 0x0072B370
PaletteClass * FUN_0072B370(void)
{
	return DAT_00B0FB8C;
}

// FUNCTION: GAMEMD 0x0072B380
ConvertClass * FUN_0072B380(void)
{
	return DAT_00B0FB90;
}

// FUNCTION: GAMEMD 0x0072B390
void __fastcall FUN_0072B390(char const * name)
{
	if (DAT_00B0FB88) {
		delete DAT_00B0FB88;
		DAT_00B0FB88 = NULL;
	}
	if (DAT_00B0FB84) {
		delete DAT_00B0FB84;
		DAT_00B0FB84 = NULL;
	}
	FUN_0072ADE0(name, &DAT_00B0FB84, &DAT_00B0FB88);
}

// FUNCTION: GAMEMD 0x0072B3E0
void __fastcall FUN_0072B3E0(int side)
{
	if (!DAT_00B0FB80) {
		FUN_0072ADE0(DAT_00844BAC, &DAT_00B0FB7C, &DAT_00B0FB80);
	}
	if (!DAT_00B0FB88) {
		if (side == 0) {
			FUN_0072ADE0(DAT_00844BB0, &DAT_00B0FB84, &DAT_00B0FB88);
		} else if (side == 1) {
			FUN_0072ADE0(DAT_00844BB4, &DAT_00B0FB84, &DAT_00B0FB88);
		} else {
			FUN_0072ADE0(DAT_00844BB8, &DAT_00B0FB84, &DAT_00B0FB88);
		}
	}
	if (!DAT_00B0FB90) {
		FUN_0072ADE0(DAT_00844BBC, &DAT_00B0FB8C, &DAT_00B0FB90);
	}
}

// FUNCTION: GAMEMD 0x0072B460
void FUN_0072B460(void)
{
	if (DAT_00B0FB7C) {
		delete DAT_00B0FB7C;
		DAT_00B0FB7C = NULL;
	}
	if (DAT_00B0FB80) {
		delete DAT_00B0FB80;
		DAT_00B0FB80 = NULL;
	}
	if (DAT_00B0FB84) {
		delete DAT_00B0FB84;
		DAT_00B0FB84 = NULL;
	}
	if (DAT_00B0FB88) {
		delete DAT_00B0FB88;
		DAT_00B0FB88 = NULL;
	}
	if (DAT_00B0FB8C) {
		delete DAT_00B0FB8C;
		DAT_00B0FB8C = NULL;
	}
	if (DAT_00B0FB90) {
		delete DAT_00B0FB90;
		DAT_00B0FB90 = NULL;
	}
}

// FUNCTION: GAMEMD 0x0072B4F0
PaletteClass * FUN_0072B4F0(void)
{
	return DAT_00B0FB94;
}

// FUNCTION: GAMEMD 0x0072B500
ConvertClass * FUN_0072B500(void)
{
	return DAT_00B0FB98;
}

// FUNCTION: GAMEMD 0x0072B510
PaletteClass * FUN_0072B510(void)
{
	return DAT_00B0FB9C;
}

// FUNCTION: GAMEMD 0x0072B520
ConvertClass * FUN_0072B520(void)
{
	return DAT_00B0FBA0;
}

namespace Game { extern int ObserverMode; }
extern int DAT_00B0FD68;
extern char const * DAT_00844BC4;
extern char const * DAT_00844C0C;
extern char const * DAT_00844C1C;
extern char const * DAT_00844C10;
extern char const * DAT_00844C14;
extern char const * DAT_00844C28;
extern char const * DAT_00844C20;
extern char const * DAT_00844C18;
extern char const * DAT_00844C2C;
extern char const * DAT_00844C24;
extern char const * DAT_00844C30;

inline void Unload_Observer_Art(void)
{
	if (DAT_00B0FBA0) {
		delete DAT_00B0FBA0;
		DAT_00B0FBA0 = NULL;
	}
	if (DAT_00B0FB9C) {
		delete DAT_00B0FB9C;
		DAT_00B0FB9C = NULL;
	}
	if (DAT_00B0FB94) {
		delete DAT_00B0FB94;
		DAT_00B0FB94 = NULL;
	}
	if (DAT_00B0FB98) {
		delete DAT_00B0FB98;
		DAT_00B0FB98 = NULL;
	}
}

// FUNCTION: GAMEMD 0x0072B530
void __fastcall FUN_0072B530(int country)
{
	if (DAT_00B0FD68 == 1 && Game::ObserverMode) {
		Unload_Observer_Art();
	}
	if (DAT_00B0FD68 == 2 && !Game::ObserverMode) {
		Unload_Observer_Art();
		goto zero;
	}
	if (!DAT_00B0FB98) {
zero:
		DAT_00B0FD68 = 0;
	} else if (DAT_00B0FD68 != 0) {
		return;
	}
	if (Game::ObserverMode) {
		FUN_0072ADE0(DAT_00844BC4, &DAT_00B0FB94, &DAT_00B0FB98);
		DAT_00B0FD68 = 2;
	} else {
		if (country == 0) {
			FUN_0072ADE0(DAT_00844C0C, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 1) {
			FUN_0072ADE0(DAT_00844C1C, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 2) {
			FUN_0072ADE0(DAT_00844C10, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 3) {
			FUN_0072ADE0(DAT_00844C14, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 4) {
			FUN_0072ADE0(DAT_00844C28, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 5) {
			FUN_0072ADE0(DAT_00844C20, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 6) {
			FUN_0072ADE0(DAT_00844C18, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 7) {
			FUN_0072ADE0(DAT_00844C2C, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 8) {
			FUN_0072ADE0(DAT_00844C24, &DAT_00B0FB94, &DAT_00B0FB98);
		} else if (country == 9) {
			FUN_0072ADE0(DAT_00844C30, &DAT_00B0FB94, &DAT_00B0FB98);
		}
		DAT_00B0FD68 = 1;
	}
}

// FUNCTION: GAMEMD 0x0072B820
void __fastcall FUN_0072B820(char const * name)
{
	if (DAT_00B0FBA0) {
		delete DAT_00B0FBA0;
		DAT_00B0FBA0 = NULL;
	}
	if (DAT_00B0FB9C) {
		delete DAT_00B0FB9C;
		DAT_00B0FB9C = NULL;
	}
	FUN_0072ADE0(name, &DAT_00B0FB9C, &DAT_00B0FBA0);
}

// FUNCTION: GAMEMD 0x0072B870
void FUN_0072B870(void)
{
	if (DAT_00B0FBA0) {
		delete DAT_00B0FBA0;
		DAT_00B0FBA0 = NULL;
	}
	if (DAT_00B0FB9C) {
		delete DAT_00B0FB9C;
		DAT_00B0FB9C = NULL;
	}
	if (DAT_00B0FB94) {
		delete DAT_00B0FB94;
		DAT_00B0FB94 = NULL;
	}
	if (DAT_00B0FB98) {
		delete DAT_00B0FB98;
		DAT_00B0FB98 = NULL;
	}
}

extern bool DAT_00B0FD5D;
extern bool DAT_00B0FD5C;
extern SHPStruct * DAT_00B0FB54;
extern PaletteClass * DAT_00B0FD60;
extern ConvertClass * DAT_00B0FD64;
extern char const * DAT_00844DC4;
extern char const * DAT_00844DC8;

// FUNCTION: GAMEMD 0x0072B8E0
void FUN_0072B8E0(void)
{
	if (!DAT_00B0FD5D) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FB54 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DC4, DAT_00B0FD5C);
		}
		FUN_0072ADE0(DAT_00844DC8, &DAT_00B0FD60, &DAT_00B0FD64);
		DAT_00B0FD5D = true;
	}
}

// FUNCTION: GAMEMD 0x0072B930
void FUN_0072B930(void)
{
	if (DAT_00B0FD5D) {
		if (DAT_00B0FD5C) {
			if (DAT_00B0FB54) {
				delete DAT_00B0FB54;
			}
			DAT_00B0FD5C = false;
		}
		DAT_00B0FB54 = NULL;
		if (DAT_00B0FD60) {
			delete DAT_00B0FD60;
			DAT_00B0FD60 = NULL;
		}
		if (DAT_00B0FD64) {
			delete DAT_00B0FD64;
			DAT_00B0FD64 = NULL;
		}
	}
	DAT_00B0FD5D = false;
}

// FUNCTION: GAMEMD 0x0072B9B0
bool FUN_0072B9B0(void)
{
	return DAT_00B0FD5D;
}

// FUNCTION: GAMEMD 0x0072B9C0
PaletteClass * FUN_0072B9C0(void)
{
	return DAT_00B0FD60;
}

// FUNCTION: GAMEMD 0x0072B9D0
ConvertClass * FUN_0072B9D0(void)
{
	return DAT_00B0FD64;
}

// FUNCTION: GAMEMD 0x0072B9E0
void FUN_0072B9E0(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FB54 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DC4, DAT_00B0FD5C);
	}
}

// FUNCTION: GAMEMD 0x0072BA10
void FUN_0072BA10(void)
{
	FUN_0072ADE0(DAT_00844DC8, &DAT_00B0FD60, &DAT_00B0FD64);
}

// FUNCTION: GAMEMD 0x0072BA30
void FUN_0072BA30(void)
{
	if (DAT_00B0FD5C) {
		if (DAT_00B0FB54) {
			delete DAT_00B0FB54;
		}
		DAT_00B0FD5C = false;
	}
	DAT_00B0FB54 = NULL;
}

// FUNCTION: GAMEMD 0x0072BA80
void FUN_0072BA80(void)
{
	if (DAT_00B0FD60) {
		delete DAT_00B0FD60;
		DAT_00B0FD60 = NULL;
	}
	if (DAT_00B0FD64) {
		delete DAT_00B0FD64;
		DAT_00B0FD64 = NULL;
	}
}

extern bool DAT_00B0FD51;
extern bool DAT_00B0FD50;
extern SHPStruct * DAT_00B0FB2C;
extern PaletteClass * DAT_00B0FD54;
extern ConvertClass * DAT_00B0FD58;
extern char const * DAT_00844DBC;
extern char const * DAT_00844DC0;

// FUNCTION: GAMEMD 0x0072BAC0
void FUN_0072BAC0(void)
{
	if (!DAT_00B0FD51) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FB2C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DBC, DAT_00B0FD50);
		}
		FUN_0072ADE0(DAT_00844DC0, &DAT_00B0FD54, &DAT_00B0FD58);
		DAT_00B0FD51 = true;
	}
}

// FUNCTION: GAMEMD 0x0072BB10
void FUN_0072BB10(void)
{
	if (DAT_00B0FD51) {
		if (DAT_00B0FD50) {
			if (DAT_00B0FB2C) {
				delete DAT_00B0FB2C;
			}
			DAT_00B0FD50 = false;
		}
		DAT_00B0FB2C = NULL;
		if (DAT_00B0FD54) {
			delete DAT_00B0FD54;
			DAT_00B0FD54 = NULL;
		}
		if (DAT_00B0FD58) {
			delete DAT_00B0FD58;
			DAT_00B0FD58 = NULL;
		}
	}
	DAT_00B0FD51 = false;
}

// FUNCTION: GAMEMD 0x0072BB90
bool FUN_0072BB90(void)
{
	return DAT_00B0FD51;
}

// FUNCTION: GAMEMD 0x0072BBA0
PaletteClass * FUN_0072BBA0(void)
{
	return DAT_00B0FD54;
}

// FUNCTION: GAMEMD 0x0072BBB0
ConvertClass * FUN_0072BBB0(void)
{
	return DAT_00B0FD58;
}

// FUNCTION: GAMEMD 0x0072BBC0
void FUN_0072BBC0(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FB2C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DBC, DAT_00B0FD50);
	}
}

// FUNCTION: GAMEMD 0x0072BBF0
void FUN_0072BBF0(void)
{
	FUN_0072ADE0(DAT_00844DC0, &DAT_00B0FD54, &DAT_00B0FD58);
}

// FUNCTION: GAMEMD 0x0072BC10
void FUN_0072BC10(void)
{
	if (DAT_00B0FD50) {
		if (DAT_00B0FB2C) {
			delete DAT_00B0FB2C;
		}
		DAT_00B0FD50 = false;
	}
	DAT_00B0FB2C = NULL;
}

// FUNCTION: GAMEMD 0x0072BC60
void FUN_0072BC60(void)
{
	if (DAT_00B0FD54) {
		delete DAT_00B0FD54;
		DAT_00B0FD54 = NULL;
	}
	if (DAT_00B0FD58) {
		delete DAT_00B0FD58;
		DAT_00B0FD58 = NULL;
	}
}

extern bool DAT_00B0FD45;
extern bool DAT_00B0FD44;
extern SHPStruct * DAT_00B0FA98;
extern PaletteClass * DAT_00B0FD48;
extern ConvertClass * DAT_00B0FD4C;
extern char const * DAT_00844DB4;
extern char const * DAT_00844DB8;

// FUNCTION: GAMEMD 0x0072BCA0
void FUN_0072BCA0(void)
{
	if (!DAT_00B0FD45) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FA98 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DB4, DAT_00B0FD44);
		}
		FUN_0072ADE0(DAT_00844DB8, &DAT_00B0FD48, &DAT_00B0FD4C);
		DAT_00B0FD45 = true;
	}
}

// FUNCTION: GAMEMD 0x0072BCF0
void FUN_0072BCF0(void)
{
	if (DAT_00B0FD45) {
		if (DAT_00B0FD44) {
			if (DAT_00B0FA98) {
				delete DAT_00B0FA98;
			}
			DAT_00B0FD44 = false;
		}
		DAT_00B0FA98 = NULL;
		if (DAT_00B0FD48) {
			delete DAT_00B0FD48;
			DAT_00B0FD48 = NULL;
		}
		if (DAT_00B0FD4C) {
			delete DAT_00B0FD4C;
			DAT_00B0FD4C = NULL;
		}
	}
	DAT_00B0FD45 = false;
}

// FUNCTION: GAMEMD 0x0072BD70
bool FUN_0072BD70(void)
{
	return DAT_00B0FD45;
}

// FUNCTION: GAMEMD 0x0072BD80
PaletteClass * FUN_0072BD80(void)
{
	return DAT_00B0FD48;
}

// FUNCTION: GAMEMD 0x0072BD90
ConvertClass * FUN_0072BD90(void)
{
	return DAT_00B0FD4C;
}

// FUNCTION: GAMEMD 0x0072BDA0
void FUN_0072BDA0(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FA98 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DB4, DAT_00B0FD44);
	}
}

// FUNCTION: GAMEMD 0x0072BDD0
void FUN_0072BDD0(void)
{
	FUN_0072ADE0(DAT_00844DB8, &DAT_00B0FD48, &DAT_00B0FD4C);
}

// FUNCTION: GAMEMD 0x0072BDF0
void FUN_0072BDF0(void)
{
	if (DAT_00B0FD44) {
		if (DAT_00B0FA98) {
			delete DAT_00B0FA98;
		}
		DAT_00B0FD44 = false;
	}
	DAT_00B0FA98 = NULL;
}

// FUNCTION: GAMEMD 0x0072BE40
void FUN_0072BE40(void)
{
	if (DAT_00B0FD48) {
		delete DAT_00B0FD48;
		DAT_00B0FD48 = NULL;
	}
	if (DAT_00B0FD4C) {
		delete DAT_00B0FD4C;
		DAT_00B0FD4C = NULL;
	}
}

extern bool DAT_00B0FD39;
extern bool DAT_00B0FD38;
extern SHPStruct * DAT_00B0FA9C;
extern PaletteClass * DAT_00B0FD3C;
extern ConvertClass * DAT_00B0FD40;
extern char const * DAT_00844DAC;
extern char const * DAT_00844DB0;

// FUNCTION: GAMEMD 0x0072BE80
void FUN_0072BE80(void)
{
	if (!DAT_00B0FD39) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FA9C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DAC, DAT_00B0FD38);
		}
		FUN_0072ADE0(DAT_00844DB0, &DAT_00B0FD3C, &DAT_00B0FD40);
		DAT_00B0FD39 = true;
	}
}

// FUNCTION: GAMEMD 0x0072BED0
void FUN_0072BED0(void)
{
	if (DAT_00B0FD39) {
		if (DAT_00B0FD38) {
			if (DAT_00B0FA9C) {
				delete DAT_00B0FA9C;
			}
			DAT_00B0FD38 = false;
		}
		DAT_00B0FA9C = NULL;
		if (DAT_00B0FD3C) {
			delete DAT_00B0FD3C;
			DAT_00B0FD3C = NULL;
		}
		if (DAT_00B0FD40) {
			delete DAT_00B0FD40;
			DAT_00B0FD40 = NULL;
		}
	}
	DAT_00B0FD39 = false;
}

// FUNCTION: GAMEMD 0x0072BF50
bool FUN_0072BF50(void)
{
	return DAT_00B0FD39;
}

// FUNCTION: GAMEMD 0x0072BF60
PaletteClass * FUN_0072BF60(void)
{
	return DAT_00B0FD3C;
}

// FUNCTION: GAMEMD 0x0072BF70
ConvertClass * FUN_0072BF70(void)
{
	return DAT_00B0FD40;
}

// FUNCTION: GAMEMD 0x0072BF80
void FUN_0072BF80(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FA9C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DAC, DAT_00B0FD38);
	}
}

// FUNCTION: GAMEMD 0x0072BFB0
void FUN_0072BFB0(void)
{
	FUN_0072ADE0(DAT_00844DB0, &DAT_00B0FD3C, &DAT_00B0FD40);
}

// FUNCTION: GAMEMD 0x0072BFD0
void FUN_0072BFD0(void)
{
	if (DAT_00B0FD38) {
		if (DAT_00B0FA9C) {
			delete DAT_00B0FA9C;
		}
		DAT_00B0FD38 = false;
	}
	DAT_00B0FA9C = NULL;
}

// FUNCTION: GAMEMD 0x0072C020
void FUN_0072C020(void)
{
	if (DAT_00B0FD3C) {
		delete DAT_00B0FD3C;
		DAT_00B0FD3C = NULL;
	}
	if (DAT_00B0FD40) {
		delete DAT_00B0FD40;
		DAT_00B0FD40 = NULL;
	}
}

extern bool DAT_00B0FD2D;
extern bool DAT_00B0FD2C;
extern SHPStruct * DAT_00B0FA4C;
extern PaletteClass * DAT_00B0FD30;
extern ConvertClass * DAT_00B0FD34;
extern char const * DAT_00844DA4;
extern char const * DAT_00844DA8;

// FUNCTION: GAMEMD 0x0072C060
void FUN_0072C060(void)
{
	if (!DAT_00B0FD2D) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FA4C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DA4, DAT_00B0FD2C);
		}
		FUN_0072ADE0(DAT_00844DA8, &DAT_00B0FD30, &DAT_00B0FD34);
		DAT_00B0FD2D = true;
	}
}

// FUNCTION: GAMEMD 0x0072C0B0
void FUN_0072C0B0(void)
{
	if (DAT_00B0FD2D) {
		if (DAT_00B0FD2C) {
			if (DAT_00B0FA4C) {
				delete DAT_00B0FA4C;
			}
			DAT_00B0FD2C = false;
		}
		DAT_00B0FA4C = NULL;
		if (DAT_00B0FD30) {
			delete DAT_00B0FD30;
			DAT_00B0FD30 = NULL;
		}
		if (DAT_00B0FD34) {
			delete DAT_00B0FD34;
			DAT_00B0FD34 = NULL;
		}
	}
	DAT_00B0FD2D = false;
}

// FUNCTION: GAMEMD 0x0072C130
bool FUN_0072C130(void)
{
	return DAT_00B0FD2D;
}

// FUNCTION: GAMEMD 0x0072C140
PaletteClass * FUN_0072C140(void)
{
	return DAT_00B0FD30;
}

// FUNCTION: GAMEMD 0x0072C150
ConvertClass * FUN_0072C150(void)
{
	return DAT_00B0FD34;
}

// FUNCTION: GAMEMD 0x0072C160
void FUN_0072C160(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FA4C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844DA4, DAT_00B0FD2C);
	}
}

// FUNCTION: GAMEMD 0x0072C190
void FUN_0072C190(void)
{
	FUN_0072ADE0(DAT_00844DA8, &DAT_00B0FD30, &DAT_00B0FD34);
}

// FUNCTION: GAMEMD 0x0072C1B0
void FUN_0072C1B0(void)
{
	if (DAT_00B0FD2C) {
		if (DAT_00B0FA4C) {
			delete DAT_00B0FA4C;
		}
		DAT_00B0FD2C = false;
	}
	DAT_00B0FA4C = NULL;
}

// FUNCTION: GAMEMD 0x0072C200
void FUN_0072C200(void)
{
	if (DAT_00B0FD30) {
		delete DAT_00B0FD30;
		DAT_00B0FD30 = NULL;
	}
	if (DAT_00B0FD34) {
		delete DAT_00B0FD34;
		DAT_00B0FD34 = NULL;
	}
}

extern bool DAT_00B0FD21;
extern bool DAT_00B0FD20;
extern SHPStruct * DAT_00B0FA30;
extern PaletteClass * DAT_00B0FD24;
extern ConvertClass * DAT_00B0FD28;
extern char const * DAT_00844D9C;
extern char const * DAT_00844DA0;

// FUNCTION: GAMEMD 0x0072C240
void FUN_0072C240(void)
{
	if (!DAT_00B0FD21) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FA30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D9C, DAT_00B0FD20);
		}
		FUN_0072ADE0(DAT_00844DA0, &DAT_00B0FD24, &DAT_00B0FD28);
		DAT_00B0FD21 = true;
	}
}

// FUNCTION: GAMEMD 0x0072C290
void FUN_0072C290(void)
{
	if (DAT_00B0FD21) {
		if (DAT_00B0FD20) {
			if (DAT_00B0FA30) {
				delete DAT_00B0FA30;
			}
			DAT_00B0FD20 = false;
		}
		DAT_00B0FA30 = NULL;
		if (DAT_00B0FD24) {
			delete DAT_00B0FD24;
			DAT_00B0FD24 = NULL;
		}
		if (DAT_00B0FD28) {
			delete DAT_00B0FD28;
			DAT_00B0FD28 = NULL;
		}
	}
	DAT_00B0FD21 = false;
}

// FUNCTION: GAMEMD 0x0072C310
bool FUN_0072C310(void)
{
	return DAT_00B0FD21;
}

// FUNCTION: GAMEMD 0x0072C320
PaletteClass * FUN_0072C320(void)
{
	return DAT_00B0FD24;
}

// FUNCTION: GAMEMD 0x0072C330
ConvertClass * FUN_0072C330(void)
{
	return DAT_00B0FD28;
}

// FUNCTION: GAMEMD 0x0072C340
void FUN_0072C340(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FA30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D9C, DAT_00B0FD20);
	}
}

// FUNCTION: GAMEMD 0x0072C370
void FUN_0072C370(void)
{
	FUN_0072ADE0(DAT_00844DA0, &DAT_00B0FD24, &DAT_00B0FD28);
}

// FUNCTION: GAMEMD 0x0072C390
void FUN_0072C390(void)
{
	if (DAT_00B0FD20) {
		if (DAT_00B0FA30) {
			delete DAT_00B0FA30;
		}
		DAT_00B0FD20 = false;
	}
	DAT_00B0FA30 = NULL;
}

// FUNCTION: GAMEMD 0x0072C3E0
void FUN_0072C3E0(void)
{
	if (DAT_00B0FD24) {
		delete DAT_00B0FD24;
		DAT_00B0FD24 = NULL;
	}
	if (DAT_00B0FD28) {
		delete DAT_00B0FD28;
		DAT_00B0FD28 = NULL;
	}
}

extern bool DAT_00B0FD15;
extern bool DAT_00B0FD14;
extern SHPStruct * DAT_00B0FA64;
extern PaletteClass * DAT_00B0FD18;
extern ConvertClass * DAT_00B0FD1C;
extern char const * DAT_00844D94;
extern char const * DAT_00844D98;

// FUNCTION: GAMEMD 0x0072C420
void FUN_0072C420(void)
{
	if (!DAT_00B0FD15) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FA64 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D94, DAT_00B0FD14);
		}
		FUN_0072ADE0(DAT_00844D98, &DAT_00B0FD18, &DAT_00B0FD1C);
		DAT_00B0FD15 = true;
	}
}

// FUNCTION: GAMEMD 0x0072C470
void FUN_0072C470(void)
{
	if (DAT_00B0FD15) {
		if (DAT_00B0FD14) {
			if (DAT_00B0FA64) {
				delete DAT_00B0FA64;
			}
			DAT_00B0FD14 = false;
		}
		DAT_00B0FA64 = NULL;
		if (DAT_00B0FD18) {
			delete DAT_00B0FD18;
			DAT_00B0FD18 = NULL;
		}
		if (DAT_00B0FD1C) {
			delete DAT_00B0FD1C;
			DAT_00B0FD1C = NULL;
		}
	}
	DAT_00B0FD15 = false;
}

// FUNCTION: GAMEMD 0x0072C4F0
bool FUN_0072C4F0(void)
{
	return DAT_00B0FD15;
}

// FUNCTION: GAMEMD 0x0072C500
PaletteClass * FUN_0072C500(void)
{
	return DAT_00B0FD18;
}

// FUNCTION: GAMEMD 0x0072C510
ConvertClass * FUN_0072C510(void)
{
	return DAT_00B0FD1C;
}

// FUNCTION: GAMEMD 0x0072C520
void FUN_0072C520(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FA64 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D94, DAT_00B0FD14);
	}
}

// FUNCTION: GAMEMD 0x0072C550
void FUN_0072C550(void)
{
	FUN_0072ADE0(DAT_00844D98, &DAT_00B0FD18, &DAT_00B0FD1C);
}

// FUNCTION: GAMEMD 0x0072C570
void FUN_0072C570(void)
{
	if (DAT_00B0FD14) {
		if (DAT_00B0FA64) {
			delete DAT_00B0FA64;
		}
		DAT_00B0FD14 = false;
	}
	DAT_00B0FA64 = NULL;
}

// FUNCTION: GAMEMD 0x0072C5C0
void FUN_0072C5C0(void)
{
	if (DAT_00B0FD18) {
		delete DAT_00B0FD18;
		DAT_00B0FD18 = NULL;
	}
	if (DAT_00B0FD1C) {
		delete DAT_00B0FD1C;
		DAT_00B0FD1C = NULL;
	}
}

extern bool DAT_00B0FD09;
extern bool DAT_00B0FD08;
extern SHPStruct * DAT_00B0FB0C;
extern PaletteClass * DAT_00B0FD0C;
extern ConvertClass * DAT_00B0FD10;
extern char const * DAT_00844D8C;
extern char const * DAT_00844D90;

// FUNCTION: GAMEMD 0x0072C600
void FUN_0072C600(void)
{
	if (!DAT_00B0FD09) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FB0C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D8C, DAT_00B0FD08);
		}
		FUN_0072ADE0(DAT_00844D90, &DAT_00B0FD0C, &DAT_00B0FD10);
		DAT_00B0FD09 = true;
	}
}

// FUNCTION: GAMEMD 0x0072C650
void FUN_0072C650(void)
{
	if (DAT_00B0FD09) {
		if (DAT_00B0FD08) {
			if (DAT_00B0FB0C) {
				delete DAT_00B0FB0C;
			}
			DAT_00B0FD08 = false;
		}
		DAT_00B0FB0C = NULL;
		if (DAT_00B0FD0C) {
			delete DAT_00B0FD0C;
			DAT_00B0FD0C = NULL;
		}
		if (DAT_00B0FD10) {
			delete DAT_00B0FD10;
			DAT_00B0FD10 = NULL;
		}
	}
	DAT_00B0FD09 = false;
}

// FUNCTION: GAMEMD 0x0072C6D0
bool FUN_0072C6D0(void)
{
	return DAT_00B0FD09;
}

// FUNCTION: GAMEMD 0x0072C6E0
PaletteClass * FUN_0072C6E0(void)
{
	return DAT_00B0FD0C;
}

// FUNCTION: GAMEMD 0x0072C6F0
ConvertClass * FUN_0072C6F0(void)
{
	return DAT_00B0FD10;
}

// FUNCTION: GAMEMD 0x0072C700
void FUN_0072C700(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FB0C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D8C, DAT_00B0FD08);
	}
}

// FUNCTION: GAMEMD 0x0072C730
void FUN_0072C730(void)
{
	FUN_0072ADE0(DAT_00844D90, &DAT_00B0FD0C, &DAT_00B0FD10);
}

// FUNCTION: GAMEMD 0x0072C750
void FUN_0072C750(void)
{
	if (DAT_00B0FD08) {
		if (DAT_00B0FB0C) {
			delete DAT_00B0FB0C;
		}
		DAT_00B0FD08 = false;
	}
	DAT_00B0FB0C = NULL;
}

// FUNCTION: GAMEMD 0x0072C7A0
void FUN_0072C7A0(void)
{
	if (DAT_00B0FD0C) {
		delete DAT_00B0FD0C;
		DAT_00B0FD0C = NULL;
	}
	if (DAT_00B0FD10) {
		delete DAT_00B0FD10;
		DAT_00B0FD10 = NULL;
	}
}

extern bool DAT_00B0FCFD;
extern bool DAT_00B0FCFC;
extern SHPStruct * DAT_00B0F9E8;
extern PaletteClass * DAT_00B0FD00;
extern ConvertClass * DAT_00B0FD04;
extern char const * DAT_00844D84;
extern char const * DAT_00844D88;

// FUNCTION: GAMEMD 0x0072C7E0
void FUN_0072C7E0(void)
{
	if (!DAT_00B0FCFD) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0F9E8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D84, DAT_00B0FCFC);
		}
		FUN_0072ADE0(DAT_00844D88, &DAT_00B0FD00, &DAT_00B0FD04);
		DAT_00B0FCFD = true;
	}
}

// FUNCTION: GAMEMD 0x0072C830
void FUN_0072C830(void)
{
	if (DAT_00B0FCFD) {
		if (DAT_00B0FCFC) {
			if (DAT_00B0F9E8) {
				delete DAT_00B0F9E8;
			}
			DAT_00B0FCFC = false;
		}
		DAT_00B0F9E8 = NULL;
		if (DAT_00B0FD00) {
			delete DAT_00B0FD00;
			DAT_00B0FD00 = NULL;
		}
		if (DAT_00B0FD04) {
			delete DAT_00B0FD04;
			DAT_00B0FD04 = NULL;
		}
	}
	DAT_00B0FCFD = false;
}

// FUNCTION: GAMEMD 0x0072C8B0
bool FUN_0072C8B0(void)
{
	return DAT_00B0FCFD;
}

// FUNCTION: GAMEMD 0x0072C8C0
PaletteClass * FUN_0072C8C0(void)
{
	return DAT_00B0FD00;
}

// FUNCTION: GAMEMD 0x0072C8D0
ConvertClass * FUN_0072C8D0(void)
{
	return DAT_00B0FD04;
}
