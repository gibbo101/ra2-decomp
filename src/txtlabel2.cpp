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
void FUN_0072EB50(void);
extern int DAT_00B0FA20;
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
extern Rect * DAT_00B0FC10;
extern Rect * DAT_00B0FC14;
extern Rect * DAT_00B0FC18;
extern Rect * DAT_00B0FC1C;
extern Rect * DAT_00B0FC20;
extern Rect * DAT_00B0FC24;
extern Rect * DAT_00B0FC28;
extern Rect * DAT_00B0FC2C;
void __fastcall Uncache_Shape(char const * name);
extern Rect * DAT_00B0FC30;
extern Rect * DAT_00B0FC34;
extern Rect * DAT_00B0FC38;
extern Rect * DAT_00B0FC3C;
extern Rect * DAT_00B0FC40;
extern Rect * DAT_00B0FC44;
extern Rect * DAT_00B0FC48;
extern Rect * DAT_00B0FC4C;
extern Rect * DAT_00B0FC50;
extern Rect * DAT_00B0FC54;
extern Rect * DAT_00B0FC58;
extern Rect * DAT_00B0FC5C;
extern Rect * DAT_00B0FC60;
extern Rect * DAT_00B0FC64;
extern Rect * DAT_00B0FC68;
extern Rect * DAT_00B0FC6C;

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

// FUNCTION: GAMEMD 0x0072C8E0
void FUN_0072C8E0(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0F9E8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D84, DAT_00B0FCFC);
	}
}

// FUNCTION: GAMEMD 0x0072C910
void FUN_0072C910(void)
{
	FUN_0072ADE0(DAT_00844D88, &DAT_00B0FD00, &DAT_00B0FD04);
}

// FUNCTION: GAMEMD 0x0072C930
void FUN_0072C930(void)
{
	if (DAT_00B0FCFC) {
		if (DAT_00B0F9E8) {
			delete DAT_00B0F9E8;
		}
		DAT_00B0FCFC = false;
	}
	DAT_00B0F9E8 = NULL;
}

// FUNCTION: GAMEMD 0x0072C980
void FUN_0072C980(void)
{
	if (DAT_00B0FD00) {
		delete DAT_00B0FD00;
		DAT_00B0FD00 = NULL;
	}
	if (DAT_00B0FD04) {
		delete DAT_00B0FD04;
		DAT_00B0FD04 = NULL;
	}
}

extern bool DAT_00B0FCF0;
extern bool DAT_00B0FCF1;
extern SHPStruct * DAT_00B0FA60;
extern PaletteClass * DAT_00B0FCF4;
extern ConvertClass * DAT_00B0FCF8;
extern char const * DAT_00844D7C;
extern char const * DAT_00844D80;

// FUNCTION: GAMEMD 0x0072C9C0
void FUN_0072C9C0(void)
{
	if (!DAT_00B0FCF1) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FA60 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D7C, DAT_00B0FCF0);
		}
		FUN_0072ADE0(DAT_00844D80, &DAT_00B0FCF4, &DAT_00B0FCF8);
		DAT_00B0FCF1 = true;
	}
}

// FUNCTION: GAMEMD 0x0072CA10
void FUN_0072CA10(void)
{
	if (DAT_00B0FCF1) {
		if (DAT_00B0FCF0) {
			if (DAT_00B0FA60) {
				delete DAT_00B0FA60;
			}
			DAT_00B0FCF0 = false;
		}
		DAT_00B0FA60 = NULL;
		if (DAT_00B0FCF4) {
			delete DAT_00B0FCF4;
			DAT_00B0FCF4 = NULL;
		}
		if (DAT_00B0FCF8) {
			delete DAT_00B0FCF8;
			DAT_00B0FCF8 = NULL;
		}
	}
	DAT_00B0FCF1 = false;
}

// FUNCTION: GAMEMD 0x0072CA90
bool FUN_0072CA90(void)
{
	return DAT_00B0FCF1;
}

// FUNCTION: GAMEMD 0x0072CAA0
PaletteClass * FUN_0072CAA0(void)
{
	return DAT_00B0FCF4;
}

// FUNCTION: GAMEMD 0x0072CAB0
ConvertClass * FUN_0072CAB0(void)
{
	return DAT_00B0FCF8;
}

// FUNCTION: GAMEMD 0x0072CAC0
void FUN_0072CAC0(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FA60 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D7C, DAT_00B0FCF0);
	}
}

// FUNCTION: GAMEMD 0x0072CAF0
void FUN_0072CAF0(void)
{
	FUN_0072ADE0(DAT_00844D80, &DAT_00B0FCF4, &DAT_00B0FCF8);
}

// FUNCTION: GAMEMD 0x0072CB10
void FUN_0072CB10(void)
{
	if (DAT_00B0FCF0) {
		if (DAT_00B0FA60) {
			delete DAT_00B0FA60;
		}
		DAT_00B0FCF0 = false;
	}
	DAT_00B0FA60 = NULL;
}

// FUNCTION: GAMEMD 0x0072CB60
void FUN_0072CB60(void)
{
	if (DAT_00B0FCF4) {
		delete DAT_00B0FCF4;
		DAT_00B0FCF4 = NULL;
	}
	if (DAT_00B0FCF8) {
		delete DAT_00B0FCF8;
		DAT_00B0FCF8 = NULL;
	}
}
extern bool DAT_00B0FC73;
extern bool DAT_00B0FC7C;
extern SHPStruct * DAT_00B0FAD0;
extern PaletteClass * DAT_00B0FC74;
extern ConvertClass * DAT_00B0FC78;

// FUNCTION: GAMEMD 0x0072CBA0
void FUN_0072CBA0(void)
{
	if (!DAT_00B0FC7C) {
		DAT_00B0FAD0 = (SHPStruct *)FileSystem_LoadWholeFileEx("WAITCYLO.SHP", DAT_00B0FC73);
		FUN_0072ADE0("WAITCYLO.PAL", &DAT_00B0FC74, &DAT_00B0FC78);
		DAT_00B0FC7C = true;
	}
}

// FUNCTION: GAMEMD 0x0072CBE0
void FUN_0072CBE0(void)
{
	if (DAT_00B0FC7C) {
		if (DAT_00B0FC73) {
			if (DAT_00B0FAD0) {
				delete DAT_00B0FAD0;
			}
			DAT_00B0FC73 = false;
		}
		DAT_00B0FAD0 = NULL;
		if (DAT_00B0FC74) {
			delete DAT_00B0FC74;
			DAT_00B0FC74 = NULL;
		}
		if (DAT_00B0FC78) {
			delete DAT_00B0FC78;
			DAT_00B0FC78 = NULL;
		}
	}
	DAT_00B0FC7C = false;
}

// FUNCTION: GAMEMD 0x0072CC60
bool FUN_0072CC60(void)
{
	return DAT_00B0FC7C;
}

// FUNCTION: GAMEMD 0x0072CC70
PaletteClass * FUN_0072CC70(void)
{
	return DAT_00B0FC74;
}

// FUNCTION: GAMEMD 0x0072CC80
ConvertClass * FUN_0072CC80(void)
{
	return DAT_00B0FC78;
}

// FUNCTION: GAMEMD 0x0072CC90
void FUN_0072CC90(void)
{
	DAT_00B0FAD0 = (SHPStruct *)FileSystem_LoadWholeFileEx("WAITCYLO.SHP", DAT_00B0FC73);
}

// FUNCTION: GAMEMD 0x0072CCB0
void FUN_0072CCB0(void)
{
	FUN_0072ADE0("WAITCYLO.PAL", &DAT_00B0FC74, &DAT_00B0FC78);
}

// FUNCTION: GAMEMD 0x0072CCD0
void FUN_0072CCD0(void)
{
	if (DAT_00B0FC73) {
		if (DAT_00B0FAD0) {
			delete DAT_00B0FAD0;
		}
		DAT_00B0FC73 = false;
	}
	DAT_00B0FAD0 = NULL;
}

// FUNCTION: GAMEMD 0x0072CD20
void FUN_0072CD20(void)
{
	if (DAT_00B0FC74) {
		delete DAT_00B0FC74;
		DAT_00B0FC74 = NULL;
	}
	if (DAT_00B0FC78) {
		delete DAT_00B0FC78;
		DAT_00B0FC78 = NULL;
	}
}
extern bool DAT_00B0FCE4;
extern bool DAT_00B0FCE5;
extern SHPStruct * DAT_00B0FAAC;
extern PaletteClass * DAT_00B0FCE8;
extern ConvertClass * DAT_00B0FCEC;
extern char const * DAT_00844D74;
extern char const * DAT_00844D78;

// FUNCTION: GAMEMD 0x0072CD60
void FUN_0072CD60(void)
{
	if (!DAT_00B0FCE5) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FAAC = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D74, DAT_00B0FCE4);
		}
		FUN_0072ADE0(DAT_00844D78, &DAT_00B0FCE8, &DAT_00B0FCEC);
		DAT_00B0FCE5 = true;
	}
}

// FUNCTION: GAMEMD 0x0072CDB0
void FUN_0072CDB0(void)
{
	if (DAT_00B0FCE5) {
		if (DAT_00B0FCE4) {
			if (DAT_00B0FAAC) {
				delete DAT_00B0FAAC;
			}
			DAT_00B0FCE4 = false;
		}
		DAT_00B0FAAC = NULL;
		if (DAT_00B0FCE8) {
			delete DAT_00B0FCE8;
			DAT_00B0FCE8 = NULL;
		}
		if (DAT_00B0FCEC) {
			delete DAT_00B0FCEC;
			DAT_00B0FCEC = NULL;
		}
	}
	DAT_00B0FCE5 = false;
}

// FUNCTION: GAMEMD 0x0072CE30
bool FUN_0072CE30(void)
{
	return DAT_00B0FCE5;
}

// FUNCTION: GAMEMD 0x0072CE40
PaletteClass * FUN_0072CE40(void)
{
	return DAT_00B0FCE8;
}

// FUNCTION: GAMEMD 0x0072CE50
ConvertClass * FUN_0072CE50(void)
{
	return DAT_00B0FCEC;
}

// FUNCTION: GAMEMD 0x0072CE60
void FUN_0072CE60(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FAAC = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D74, DAT_00B0FCE4);
	}
}

// FUNCTION: GAMEMD 0x0072CE90
void FUN_0072CE90(void)
{
	FUN_0072ADE0(DAT_00844D78, &DAT_00B0FCE8, &DAT_00B0FCEC);
}

// FUNCTION: GAMEMD 0x0072CEB0
void FUN_0072CEB0(void)
{
	if (DAT_00B0FCE4) {
		if (DAT_00B0FAAC) {
			delete DAT_00B0FAAC;
		}
		DAT_00B0FCE4 = false;
	}
	DAT_00B0FAAC = NULL;
}

// FUNCTION: GAMEMD 0x0072CF00
void FUN_0072CF00(void)
{
	if (DAT_00B0FCE8) {
		delete DAT_00B0FCE8;
		DAT_00B0FCE8 = NULL;
	}
	if (DAT_00B0FCEC) {
		delete DAT_00B0FCEC;
		DAT_00B0FCEC = NULL;
	}
}
extern bool DAT_00B0FCD8;
extern bool DAT_00B0FCD9;
extern SHPStruct * DAT_00B0FA18;
extern PaletteClass * DAT_00B0FCDC;
extern ConvertClass * DAT_00B0FCE0;
extern char const * DAT_00844D6C;
extern char const * DAT_00844D70;

// FUNCTION: GAMEMD 0x0072CF40
void FUN_0072CF40(void)
{
	if (!DAT_00B0FCD9) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FA18 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D6C, DAT_00B0FCD8);
		}
		FUN_0072ADE0(DAT_00844D70, &DAT_00B0FCDC, &DAT_00B0FCE0);
		DAT_00B0FCD9 = true;
	}
}

// FUNCTION: GAMEMD 0x0072CF90
void FUN_0072CF90(void)
{
	if (DAT_00B0FCD9) {
		if (DAT_00B0FCD8) {
			if (DAT_00B0FA18) {
				delete DAT_00B0FA18;
			}
			DAT_00B0FCD8 = false;
		}
		DAT_00B0FA18 = NULL;
		if (DAT_00B0FCDC) {
			delete DAT_00B0FCDC;
			DAT_00B0FCDC = NULL;
		}
		if (DAT_00B0FCE0) {
			delete DAT_00B0FCE0;
			DAT_00B0FCE0 = NULL;
		}
	}
	DAT_00B0FCD9 = false;
}

// FUNCTION: GAMEMD 0x0072D010
bool FUN_0072D010(void)
{
	return DAT_00B0FCD9;
}

// FUNCTION: GAMEMD 0x0072D020
PaletteClass * FUN_0072D020(void)
{
	return DAT_00B0FCDC;
}

// FUNCTION: GAMEMD 0x0072D030
ConvertClass * FUN_0072D030(void)
{
	return DAT_00B0FCE0;
}

// FUNCTION: GAMEMD 0x0072D040
void FUN_0072D040(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FA18 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D6C, DAT_00B0FCD8);
	}
}

// FUNCTION: GAMEMD 0x0072D070
void FUN_0072D070(void)
{
	FUN_0072ADE0(DAT_00844D70, &DAT_00B0FCDC, &DAT_00B0FCE0);
}

// FUNCTION: GAMEMD 0x0072D090
void FUN_0072D090(void)
{
	if (DAT_00B0FCD8) {
		if (DAT_00B0FA18) {
			delete DAT_00B0FA18;
		}
		DAT_00B0FCD8 = false;
	}
	DAT_00B0FA18 = NULL;
}

// FUNCTION: GAMEMD 0x0072D0E0
void FUN_0072D0E0(void)
{
	if (DAT_00B0FCDC) {
		delete DAT_00B0FCDC;
		DAT_00B0FCDC = NULL;
	}
	if (DAT_00B0FCE0) {
		delete DAT_00B0FCE0;
		DAT_00B0FCE0 = NULL;
	}
}
extern bool DAT_00B0FCCD;
extern bool DAT_00B0FCCE;
extern SHPStruct * DAT_00B0FAB8;
extern PaletteClass * DAT_00B0FCD0;
extern ConvertClass * DAT_00B0FCD4;
extern char const * DAT_00844D64;
extern char const * DAT_00844D68;

// FUNCTION: GAMEMD 0x0072D120
void FUN_0072D120(void)
{
	if (!DAT_00B0FCCE) {
		if (Drawing::RenderWidth == 0x320) {
			DAT_00B0FAB8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D64, DAT_00B0FCCD);
		}
		FUN_0072ADE0(DAT_00844D68, &DAT_00B0FCD0, &DAT_00B0FCD4);
		DAT_00B0FCCE = true;
	}
}

// FUNCTION: GAMEMD 0x0072D170
void FUN_0072D170(void)
{
	if (DAT_00B0FCCE) {
		if (DAT_00B0FCCD) {
			if (DAT_00B0FAB8) {
				delete DAT_00B0FAB8;
			}
			DAT_00B0FCCD = false;
		}
		DAT_00B0FAB8 = NULL;
		if (DAT_00B0FCD0) {
			delete DAT_00B0FCD0;
			DAT_00B0FCD0 = NULL;
		}
		if (DAT_00B0FCD4) {
			delete DAT_00B0FCD4;
			DAT_00B0FCD4 = NULL;
		}
	}
	DAT_00B0FCCE = false;
}

// FUNCTION: GAMEMD 0x0072D1F0
bool FUN_0072D1F0(void)
{
	return DAT_00B0FCCE;
}

// FUNCTION: GAMEMD 0x0072D200
PaletteClass * FUN_0072D200(void)
{
	return DAT_00B0FCD0;
}

// FUNCTION: GAMEMD 0x0072D210
ConvertClass * FUN_0072D210(void)
{
	return DAT_00B0FCD4;
}

// FUNCTION: GAMEMD 0x0072D220
void FUN_0072D220(void)
{
	if (Drawing::RenderWidth == 0x320) {
		DAT_00B0FAB8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D64, DAT_00B0FCCD);
	}
}

// FUNCTION: GAMEMD 0x0072D250
void FUN_0072D250(void)
{
	FUN_0072ADE0(DAT_00844D68, &DAT_00B0FCD0, &DAT_00B0FCD4);
}

// FUNCTION: GAMEMD 0x0072D270
void FUN_0072D270(void)
{
	if (DAT_00B0FCCD) {
		if (DAT_00B0FAB8) {
			delete DAT_00B0FAB8;
		}
		DAT_00B0FCCD = false;
	}
	DAT_00B0FAB8 = NULL;
}

// FUNCTION: GAMEMD 0x0072D2C0
void FUN_0072D2C0(void)
{
	if (DAT_00B0FCD0) {
		delete DAT_00B0FCD0;
		DAT_00B0FCD0 = NULL;
	}
	if (DAT_00B0FCD4) {
		delete DAT_00B0FCD4;
		DAT_00B0FCD4 = NULL;
	}
}

extern bool DAT_00B0FBAC;
extern bool DAT_00B0FC70;
extern bool DAT_00B0FC71;
extern bool DAT_00B0FC72;
extern SHPStruct * DAT_00B0FB34;
extern SHPStruct * DAT_00B0FB00;
extern SHPStruct * DAT_00B0FB30;
extern PaletteClass * DAT_00B0FBA4;
extern ConvertClass * DAT_00B0FBA8;
extern char const * DAT_00844BC8;
extern char const * DAT_00844BCC;
extern char const * DAT_00844BD0;
extern char const * DAT_00844C58;
extern char const * DAT_00844C5C;
extern char const * DAT_00844C60;
extern char const * DAT_00844C64;
extern char const * DAT_00844C68;
extern char const * DAT_00844C6C;
extern char const * DAT_00844C70;
extern char const * DAT_00844C74;
extern char const * DAT_00844C78;
extern char const * DAT_00844C7C;
extern char const * DAT_00844C80;
extern char const * DAT_00844C84;
extern char const * DAT_00844C88;
extern char const * DAT_00844C8C;
extern char const * DAT_00844C90;
extern char const * DAT_00844C94;
extern char const * DAT_00844C98;
extern char const * DAT_00844C9C;

void __fastcall FUN_0072D460(int index);

// FUNCTION: GAMEMD 0x0072D300
void __fastcall FUN_0072D300(int index)
{
	if (!DAT_00B0FBAC) {
		FUN_0072D460(index);
		char const * name;
		if (index == 0) {
			name = DAT_00844BC8;
		} else {
			name = DAT_00844BCC;
			if (index != 1) {
				name = DAT_00844BD0;
			}
		}
		FUN_0072ADE0(name, &DAT_00B0FBA4, &DAT_00B0FBA8);
		DAT_00B0FBAC = true;
	}
}

// FUNCTION: GAMEMD 0x0072D350
void FUN_0072D350(void)
{
	if (DAT_00B0FBAC) {
		DAT_00B0FBAC = false;
		if (DAT_00B0FC70) {
			if (DAT_00B0FB34) {
				delete DAT_00B0FB34;
			}
			DAT_00B0FC70 = false;
		}
		DAT_00B0FB34 = NULL;
		if (DAT_00B0FC71) {
			if (DAT_00B0FB00) {
				delete DAT_00B0FB00;
			}
			DAT_00B0FC71 = false;
		}
		DAT_00B0FB00 = NULL;
		if (DAT_00B0FC72) {
			if (DAT_00B0FB30) {
				delete DAT_00B0FB30;
			}
			DAT_00B0FC72 = false;
		}
		DAT_00B0FB30 = NULL;
		if (DAT_00B0FBA4) {
			delete DAT_00B0FBA4;
			DAT_00B0FBA4 = NULL;
		}
		if (DAT_00B0FBA8) {
			delete DAT_00B0FBA8;
			DAT_00B0FBA8 = NULL;
		}
	}
}

// FUNCTION: GAMEMD 0x0072D430
bool FUN_0072D430(void)
{
	return DAT_00B0FBAC;
}

// FUNCTION: GAMEMD 0x0072D440
PaletteClass * FUN_0072D440(void)
{
	return DAT_00B0FBA4;
}

// FUNCTION: GAMEMD 0x0072D450
ConvertClass * FUN_0072D450(void)
{
	return DAT_00B0FBA8;
}

// FUNCTION: GAMEMD 0x0072D460
void __fastcall FUN_0072D460(int index)
{
	if (index == 0) {
		if (Drawing::RenderWidth == 0x280) {
			DAT_00B0FB34 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C58, DAT_00B0FC70);
			DAT_00B0FB00 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C60, DAT_00B0FC71);
			DAT_00B0FB30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C68, DAT_00B0FC72);
		} else {
			DAT_00B0FB34 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C5C, DAT_00B0FC70);
			DAT_00B0FB00 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C64, DAT_00B0FC71);
			DAT_00B0FB30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C6C, DAT_00B0FC72);
		}
	} else if (index == 1) {
		if (Drawing::RenderWidth == 0x280) {
			DAT_00B0FB34 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C70, DAT_00B0FC70);
			DAT_00B0FB00 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C78, DAT_00B0FC71);
			DAT_00B0FB30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C80, DAT_00B0FC72);
		} else {
			DAT_00B0FB34 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C74, DAT_00B0FC70);
			DAT_00B0FB00 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C7C, DAT_00B0FC71);
			DAT_00B0FB30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C84, DAT_00B0FC72);
		}
	} else {
		if (Drawing::RenderWidth == 0x280) {
			DAT_00B0FB34 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C88, DAT_00B0FC70);
			DAT_00B0FB00 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C90, DAT_00B0FC71);
			DAT_00B0FB30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C98, DAT_00B0FC72);
		} else {
			DAT_00B0FB34 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C8C, DAT_00B0FC70);
			DAT_00B0FB00 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C94, DAT_00B0FC71);
			DAT_00B0FB30 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844C9C, DAT_00B0FC72);
		}
	}
}

// FUNCTION: GAMEMD 0x0072D610
void __fastcall FUN_0072D610(int index)
{
	if (index == 0) {
		FUN_0072ADE0(DAT_00844BC8, &DAT_00B0FBA4, &DAT_00B0FBA8);
	} else {
		FUN_0072ADE0((index == 1) ? DAT_00844BCC : DAT_00844BD0, &DAT_00B0FBA4, &DAT_00B0FBA8);
	}
}

// FUNCTION: GAMEMD 0x0072D650
void FUN_0072D650(void)
{
	if (DAT_00B0FC70) {
		if (DAT_00B0FB34) {
			delete DAT_00B0FB34;
		}
		DAT_00B0FC70 = false;
	}
	DAT_00B0FB34 = NULL;
	if (DAT_00B0FC71) {
		if (DAT_00B0FB00) {
			delete DAT_00B0FB00;
		}
		DAT_00B0FC71 = false;
	}
	DAT_00B0FB00 = NULL;
	if (DAT_00B0FC72) {
		if (DAT_00B0FB30) {
			delete DAT_00B0FB30;
		}
		DAT_00B0FC72 = false;
	}
	DAT_00B0FB30 = NULL;
}

// FUNCTION: GAMEMD 0x0072D6F0
void FUN_0072D6F0(void)
{
	if (DAT_00B0FBA4) {
		delete DAT_00B0FBA4;
		DAT_00B0FBA4 = NULL;
	}
	if (DAT_00B0FBA8) {
		delete DAT_00B0FBA8;
		DAT_00B0FBA8 = NULL;
	}
}

extern bool DAT_00B0FBB8;
extern bool DAT_00B0FC7D;
extern SHPStruct * DAT_00B0FB1C;
extern PaletteClass * DAT_00B0FBB0;
extern ConvertClass * DAT_00B0FBB4;
extern char const * DAT_00844BD4;
extern char const * DAT_00844BD8;
extern char const * DAT_00844BDC;
extern char const * DAT_00844CA0;
extern char const * DAT_00844CA4;
extern char const * DAT_00844CA8;
extern char const * DAT_00844CAC;
extern char const * DAT_00844CB0;
extern char const * DAT_00844CB4;

void __fastcall FUN_0072D830(int index);

// FUNCTION: GAMEMD 0x0072D780
void FUN_0072D780(void)
{
	if (DAT_00B0FBB8) {
		DAT_00B0FBB8 = false;
		if (DAT_00B0FC7D) {
			if (DAT_00B0FB1C) {
				delete DAT_00B0FB1C;
			}
			DAT_00B0FC7D = false;
		}
		DAT_00B0FB1C = NULL;
		if (DAT_00B0FBB0) {
			delete DAT_00B0FBB0;
			DAT_00B0FBB0 = NULL;
		}
		if (DAT_00B0FBB4) {
			delete DAT_00B0FBB4;
			DAT_00B0FBB4 = NULL;
		}
	}
}

// FUNCTION: GAMEMD 0x0072D800
bool FUN_0072D800(void)
{
	return DAT_00B0FBB8;
}

// FUNCTION: GAMEMD 0x0072D810
PaletteClass * FUN_0072D810(void)
{
	return DAT_00B0FBB0;
}

// FUNCTION: GAMEMD 0x0072D820
ConvertClass * FUN_0072D820(void)
{
	return DAT_00B0FBB4;
}

// FUNCTION: GAMEMD 0x0072D830
void __fastcall FUN_0072D830(int index)
{
	if (index == 0) {
		if (Drawing::RenderWidth == 0x280) {
			DAT_00B0FB1C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CA0, DAT_00B0FC7D);
			return;
		}
		DAT_00B0FB1C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CA4, DAT_00B0FC7D);
		return;
	}
	if (index == 1) {
		if (Drawing::RenderWidth == 0x280) {
			DAT_00B0FB1C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CA8, DAT_00B0FC7D);
			return;
		}
		DAT_00B0FB1C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CAC, DAT_00B0FC7D);
		return;
	}
	if (Drawing::RenderWidth == 0x280) {
		DAT_00B0FB1C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CB0, DAT_00B0FC7D);
	} else {
		DAT_00B0FB1C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CB4, DAT_00B0FC7D);
	}
}

extern SHPStruct * DAT_00B0FB48;
extern bool DAT_00B0FCC8;
extern char const * DAT_00844CB8;
extern char const * DAT_00844D4C;
extern SHPStruct * DAT_00B0FB10;
extern bool DAT_00B0FCCA;
extern char const * DAT_00844CC4;
extern char const * DAT_00844D54;
extern SHPStruct * DAT_00B0F9F8;
extern bool DAT_00B0FCCB;
extern char const * DAT_00844CC8;
extern char const * DAT_00844CCC;
extern SHPStruct * DAT_00B0FA6C;
extern bool DAT_00B0FCCC;
extern char const * DAT_00844CD0;
extern char const * DAT_00844D5C;
extern char const * DAT_00844BE0;
extern PaletteClass * DAT_00B0FBBC;
extern ConvertClass * DAT_00B0FBC0;
extern char const * DAT_00844D50;
extern PaletteClass * DAT_00B0FCB0;
extern ConvertClass * DAT_00B0FCB4;
extern char const * DAT_00844D58;
extern PaletteClass * DAT_00B0FCB8;
extern ConvertClass * DAT_00B0FCBC;
extern char const * DAT_00844D60;
extern PaletteClass * DAT_00B0FCC0;
extern ConvertClass * DAT_00B0FCC4;

// FUNCTION: GAMEMD 0x0072D8D0
void __fastcall FUN_0072D8D0(int index)
{
	if (index == 0) {
		FUN_0072ADE0(DAT_00844BD4, &DAT_00B0FBB0, &DAT_00B0FBB4);
	} else {
		FUN_0072ADE0((index == 1) ? DAT_00844BD8 : DAT_00844BDC, &DAT_00B0FBB0, &DAT_00B0FBB4);
	}
}

// FUNCTION: GAMEMD 0x0072D910
void FUN_0072D910(void)
{
	if (DAT_00B0FC7D) {
		if (DAT_00B0FB1C) {
			delete DAT_00B0FB1C;
		}
		DAT_00B0FC7D = false;
	}
	DAT_00B0FB1C = NULL;
}

// FUNCTION: GAMEMD 0x0072D960
void FUN_0072D960(void)
{
	if (DAT_00B0FBB0) {
		delete DAT_00B0FBB0;
		DAT_00B0FBB0 = NULL;
	}
	if (DAT_00B0FBB4) {
		delete DAT_00B0FBB4;
		DAT_00B0FBB4 = NULL;
	}
}

// FUNCTION: GAMEMD 0x0072D9A0
void FUN_0072D9A0(void)
{
	if (Drawing::RenderWidth == 0x280) {
		DAT_00B0FB48 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CB8, DAT_00B0FCC8);
		DAT_00B0FB10 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CC4, DAT_00B0FCCA);
		DAT_00B0F9F8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CC8, DAT_00B0FCCB);
		DAT_00B0FA6C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CD0, DAT_00B0FCCC);
	} else {
		DAT_00B0FB48 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D4C, DAT_00B0FCC8);
		DAT_00B0FB10 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D54, DAT_00B0FCCA);
		DAT_00B0F9F8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CCC, DAT_00B0FCCB);
		DAT_00B0FA6C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D5C, DAT_00B0FCCC);
	}
	FUN_0072ADE0(DAT_00844BE0, &DAT_00B0FBBC, &DAT_00B0FBC0);
	FUN_0072ADE0(DAT_00844D50, &DAT_00B0FCB0, &DAT_00B0FCB4);
	FUN_0072ADE0(DAT_00844D58, &DAT_00B0FCB8, &DAT_00B0FCBC);
	FUN_0072ADE0(DAT_00844D60, &DAT_00B0FCC0, &DAT_00B0FCC4);
}

void FUN_0072DC20(void);
void FUN_0072DCF0(void);

// FUNCTION: GAMEMD 0x0072DAA0
void FUN_0072DAA0(void)
{
	FUN_0072DC20();
	FUN_0072DCF0();
}

// FUNCTION: GAMEMD 0x0072DAB0
PaletteClass * FUN_0072DAB0(void)
{
	return DAT_00B0FBBC;
}

// FUNCTION: GAMEMD 0x0072DAC0
ConvertClass * FUN_0072DAC0(void)
{
	return DAT_00B0FCB4;
}

// FUNCTION: GAMEMD 0x0072DAD0
ConvertClass * FUN_0072DAD0(void)
{
	return DAT_00B0FCBC;
}

// FUNCTION: GAMEMD 0x0072DAE0
ConvertClass * FUN_0072DAE0(void)
{
	return DAT_00B0FCC4;
}

// FUNCTION: GAMEMD 0x0072DAF0
ConvertClass * FUN_0072DAF0(void)
{
	return DAT_00B0FBC0;
}

// FUNCTION: GAMEMD 0x0072DB00
void FUN_0072DB00(void)
{
	if (Drawing::RenderWidth == 0x280) {
		DAT_00B0FB48 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CB8, DAT_00B0FCC8);
		DAT_00B0FB10 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CC4, DAT_00B0FCCA);
		DAT_00B0F9F8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CC8, DAT_00B0FCCB);
		DAT_00B0FA6C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CD0, DAT_00B0FCCC);
	} else {
		DAT_00B0FB48 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D4C, DAT_00B0FCC8);
		DAT_00B0FB10 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D54, DAT_00B0FCCA);
		DAT_00B0F9F8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CCC, DAT_00B0FCCB);
		DAT_00B0FA6C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D5C, DAT_00B0FCCC);
	}
}

// FUNCTION: GAMEMD 0x0072DBC0
void FUN_0072DBC0(void)
{
	FUN_0072ADE0(DAT_00844BE0, &DAT_00B0FBBC, &DAT_00B0FBC0);
	FUN_0072ADE0(DAT_00844D50, &DAT_00B0FCB0, &DAT_00B0FCB4);
	FUN_0072ADE0(DAT_00844D58, &DAT_00B0FCB8, &DAT_00B0FCBC);
	FUN_0072ADE0(DAT_00844D60, &DAT_00B0FCC0, &DAT_00B0FCC4);
}

// FUNCTION: GAMEMD 0x0072DC20
void FUN_0072DC20(void)
{
	if (DAT_00B0FCC8) {
		if (DAT_00B0FB48) {
			delete DAT_00B0FB48;
		}
		DAT_00B0FCC8 = false;
	}
	DAT_00B0FB48 = NULL;
	if (DAT_00B0FCCA) {
		if (DAT_00B0FB10) {
			delete DAT_00B0FB10;
		}
		DAT_00B0FCCA = false;
	}
	DAT_00B0FB10 = NULL;
	if (DAT_00B0FCCB) {
		if (DAT_00B0F9F8) {
			delete DAT_00B0F9F8;
		}
		DAT_00B0FCCB = false;
	}
	DAT_00B0F9F8 = NULL;
	if (DAT_00B0FCCC) {
		if (DAT_00B0FA6C) {
			delete DAT_00B0FA6C;
		}
		DAT_00B0FCCC = false;
	}
	DAT_00B0FA6C = NULL;
}

// FUNCTION: GAMEMD 0x0072DCF0
void FUN_0072DCF0(void)
{
	if (DAT_00B0FCB0) {
		delete DAT_00B0FCB0;
		DAT_00B0FCB0 = NULL;
	}
	if (DAT_00B0FCB4) {
		delete DAT_00B0FCB4;
		DAT_00B0FCB4 = NULL;
	}
	if (DAT_00B0FCB8) {
		delete DAT_00B0FCB8;
		DAT_00B0FCB8 = NULL;
	}
	if (DAT_00B0FCBC) {
		delete DAT_00B0FCBC;
		DAT_00B0FCBC = NULL;
	}
	if (DAT_00B0FCC0) {
		delete DAT_00B0FCC0;
		DAT_00B0FCC0 = NULL;
	}
	if (DAT_00B0FCC4) {
		delete DAT_00B0FCC4;
		DAT_00B0FCC4 = NULL;
	}
	if (DAT_00B0FBBC) {
		delete DAT_00B0FBBC;
		DAT_00B0FBBC = NULL;
	}
	if (DAT_00B0FBC0) {
		delete DAT_00B0FBC0;
		DAT_00B0FBC0 = NULL;
	}
}

// FUNCTION: GAMEMD 0x0072DDB0
void FUN_0072DDB0(void)
{
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

// FUNCTION: GAMEMD 0x0072DEF0
void FUN_0072DEF0(void)
{
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
}

// FUNCTION: GAMEMD 0x0072DFB0
void FUN_0072DFB0(void)
{
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

// FUNCTION: GAMEMD 0x0072E1B0
void __fastcall FUN_0072E1B0(int width, int height)
{
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
	FUN_0072EC70(width, height);
}

// FUNCTION: GAMEMD 0x0072E260
bool FUN_0072E260(void)
{
	return DAT_00B0FBE0;
}

// FUNCTION: GAMEMD 0x0072E270
PaletteClass * FUN_0072E270(void)
{
	return DAT_00B0FBC8;
}

// FUNCTION: GAMEMD 0x0072E280
ConvertClass * FUN_0072E280(void)
{
	return DAT_00B0FBCC;
}

// FUNCTION: GAMEMD 0x0072E290
PaletteClass * FUN_0072E290(void)
{
	return DAT_00B0FBD0;
}

// FUNCTION: GAMEMD 0x0072E2A0
ConvertClass * FUN_0072E2A0(void)
{
	return DAT_00B0FBD4;
}

// FUNCTION: GAMEMD 0x0072E2B0
PaletteClass * FUN_0072E2B0(void)
{
	return DAT_00B0FBD8;
}

namespace OwnerDraw { ConvertClass * GetSmallButtonAnimConvert(void); }

// FUNCTION: GAMEMD 0x0072E2C0
ConvertClass * OwnerDraw::GetSmallButtonAnimConvert(void)
{
	return DAT_00B0FBDC;
}

void __fastcall FUN_0072E2D0(Surface * surface, Rect const * area);

#include "draw.h"

// FUNCTION: GAMEMD 0x0072E730
void __fastcall FUN_0072E730(Surface * surface, Rect const * area, ConvertClass * convert, SHPStruct * shape800, SHPStruct * shape)
{
	Point2D pt(0, 0);
	Rect r = *area;
	if (r.Width > 0x320) {
		r.Width = (r.Width - 0x320) / 2 + 0x320;
	}
	if (r.Height > 0x258) {
		r.Height = (r.Height - 0x258) / 2 + 0x258;
	}
	pt.X = DAT_00B0FC1C->X;
	pt.Y = DAT_00B0FC1C->Y;
	if (Drawing::RenderWidth == 0x280) {
		Draw_Shape(*surface, *convert, (ShapeSet const *)shape800, 0, pt, r, SHAPE_WIN_REL);
	} else {
		Draw_Shape(*surface, *convert, (ShapeSet const *)shape, 0, pt, r, SHAPE_WIN_REL);
	}
}

void __fastcall FUN_0072E450(Surface * surface, Rect const * area, bool flag);

// FUNCTION: GAMEMD 0x0072E820
void __fastcall FUN_0072E820(Surface * surface, Rect const * area, bool flag)
{
	FUN_0072E450(surface, area, flag);
	FUN_0072E730(surface, area, DAT_00B0FBCC, DAT_00B0FB50, DAT_00B0FA04);
}

// FUNCTION: GAMEMD 0x0072E860
void __fastcall FUN_0072E860(Surface * surface, Rect const * clip)
{
	Point2D pt(0, 0);
	pt.X = DAT_00B0FC14->X;
	pt.Y = DAT_00B0FC14->Y;
	Draw_Shape(*surface, *DAT_00B0FBCC, (ShapeSet const *)DAT_00B0F9DC, 0, pt, *clip, SHAPE_WIN_REL);
}

// FUNCTION: GAMEMD 0x0072E8C0
void __fastcall FUN_0072E8C0(Surface * surface, Rect const * clip)
{
	Point2D pt(0, 0);
	pt.X = DAT_00B0FC20->X;
	pt.Y = DAT_00B0FC20->Y;
	Draw_Shape(*surface, *DAT_00B0FBCC, (ShapeSet const *)DAT_00B0FAF8, 1, pt, *clip, SHAPE_WIN_REL);
}

// FUNCTION: GAMEMD 0x0072E990
void __fastcall FUN_0072E990(Surface * surface, Rect const * clip)
{
	Point2D pt(0, 0);
	pt.X = DAT_00B0FC1C->X;
	pt.Y = DAT_00B0FC1C->Y;
	Draw_Shape(*surface, *DAT_00B0FBA8, (ShapeSet const *)DAT_00B0FB34, 0, pt, *clip, SHAPE_WIN_REL);
}

// FUNCTION: GAMEMD 0x0072E9F0
void __fastcall FUN_0072E9F0(Surface * surface, Rect const * clip)
{
	FUN_0072E2D0(surface, clip);
	FUN_0072E450(surface, clip, false);
	Point2D pt(0, 0);
	pt.X = DAT_00B0FC1C->X;
	pt.Y = DAT_00B0FC1C->Y;
	Draw_Shape(*surface, *DAT_00B0FBA8, (ShapeSet const *)DAT_00B0FB34, 0, pt, *clip, SHAPE_WIN_REL);
}

// FUNCTION: GAMEMD 0x0072EA70
void __fastcall FUN_0072EA70(Surface * surface, Rect const * clip)
{
	Point2D pt(0, 0);
	pt.X = DAT_00B0FC1C->X;
	pt.Y = DAT_00B0FC1C->Y;
	Draw_Shape(*surface, *DAT_00B0FBB4, (ShapeSet const *)DAT_00B0FB1C, 0, pt, *clip, SHAPE_WIN_REL);
}

// FUNCTION: GAMEMD 0x0072EAD0
void __fastcall FUN_0072EAD0(Surface * surface, Rect const * clip)
{
	FUN_0072E2D0(surface, clip);
	FUN_0072E450(surface, clip, false);
	Point2D pt(0, 0);
	pt.X = DAT_00B0FC1C->X;
	pt.Y = DAT_00B0FC1C->Y;
	Draw_Shape(*surface, *DAT_00B0FBB4, (ShapeSet const *)DAT_00B0FB1C, 0, pt, *clip, SHAPE_WIN_REL);
}

// FUNCTION: GAMEMD 0x0072EB50
void FUN_0072EB50(void)
{
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
}

// FUNCTION: GAMEMD 0x0072EC30
void FUN_0072EC30(void)
{
	FUN_0072ADE0(DAT_00844BE4, &DAT_00B0FBC8, &DAT_00B0FBCC);
	FUN_0072ADE0(DAT_00844BE8, &DAT_00B0FBD0, &DAT_00B0FBD4);
	FUN_0072ADE0(DAT_00844BEC, &DAT_00B0FBD8, &DAT_00B0FBDC);
}

// FUNCTION: GAMEMD 0x0072EF60
void FUN_0072EF60(void)
{
	if (DAT_00B0FC95) {
		if (DAT_00B0FAC4) {
			delete DAT_00B0FAC4;
		}
		DAT_00B0FC95 = false;
	}
	DAT_00B0FAC4 = NULL;
	if (DAT_00B0FC96) {
		if (DAT_00B0F9DC) {
			delete DAT_00B0F9DC;
		}
		DAT_00B0FC96 = false;
	}
	DAT_00B0F9DC = NULL;
	if (DAT_00B0FC97) {
		if (DAT_00B0FAC0) {
			delete DAT_00B0FAC0;
		}
		DAT_00B0FC97 = false;
	}
	DAT_00B0FAC0 = NULL;
	if (DAT_00B0FC98) {
		if (DAT_00B0FB50) {
			delete DAT_00B0FB50;
		}
		DAT_00B0FC98 = false;
	}
	DAT_00B0FB50 = NULL;
	if (DAT_00B0FC99) {
		if (DAT_00B0FA04) {
			delete DAT_00B0FA04;
		}
		DAT_00B0FC99 = false;
	}
	DAT_00B0FA04 = NULL;
	if (DAT_00B0FC9A) {
		if (DAT_00B0FAF8) {
			delete DAT_00B0FAF8;
		}
		DAT_00B0FC9A = false;
	}
	DAT_00B0FAF8 = NULL;
	if (DAT_00B0FC9B) {
		if (DAT_00B0FA74) {
			delete DAT_00B0FA74;
		}
		DAT_00B0FC9B = false;
	}
	DAT_00B0FA74 = NULL;
	if (DAT_00B0FC9C) {
		if (DAT_00B0FA38) {
			delete DAT_00B0FA38;
		}
		DAT_00B0FC9C = false;
	}
	DAT_00B0FA38 = NULL;
	if (DAT_00B0FC9D) {
		if (DAT_00B0FAE8) {
			delete DAT_00B0FAE8;
		}
		DAT_00B0FC9D = false;
	}
	DAT_00B0FAE8 = NULL;
	if (DAT_00B0FC9E) {
		if (DAT_00B0FA54) {
			delete DAT_00B0FA54;
		}
		DAT_00B0FC9E = false;
	}
	DAT_00B0FA54 = NULL;
}

// FUNCTION: GAMEMD 0x0072F140
void FUN_0072F140(void)
{
	if (DAT_00B0FBC8) {
		delete DAT_00B0FBC8;
		DAT_00B0FBC8 = NULL;
	}
	if (DAT_00B0FBCC) {
		delete DAT_00B0FBCC;
		DAT_00B0FBCC = NULL;
	}
	if (DAT_00B0FBD0) {
		delete DAT_00B0FBD0;
		DAT_00B0FBD0 = NULL;
	}
	if (DAT_00B0FBD4) {
		delete DAT_00B0FBD4;
		DAT_00B0FBD4 = NULL;
	}
	if (DAT_00B0FBD8) {
		delete DAT_00B0FBD8;
		DAT_00B0FBD8 = NULL;
	}
	if (DAT_00B0FBDC) {
		delete DAT_00B0FBDC;
		DAT_00B0FBDC = NULL;
	}
}

#include "scenario.h"
extern char const * DAT_00844BF0;
extern char const * DAT_00844BF4;
extern char const * DAT_00844BF8;
extern char const * DAT_00844BFC;
extern char const * DAT_00844C00;
extern char const * DAT_00844C04;
extern char const * DAT_00844C08;
extern PaletteClass * DAT_00B0FBE4;
extern ConvertClass * DAT_00B0FBE8;
extern PaletteClass * DAT_00B0FBEC;
extern ConvertClass * DAT_00B0FBF0;
extern PaletteClass * DAT_00B0FBF4;
extern ConvertClass * DAT_00B0FBF8;
extern PaletteClass * DAT_00B0FBFC;
extern ConvertClass * DAT_00B0FC00;
extern PaletteClass * DAT_00B0FC04;
extern ConvertClass * DAT_00B0FC08;
void __fastcall FUN_0072FC60(int width, int height);
void FUN_0072FA10(void);

// FUNCTION: GAMEMD 0x0072F1D0
void FUN_0072F1D0(void)
{
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
}

// FUNCTION: GAMEMD 0x0072F270
void FUN_0072F270(void)
{
	FUN_0072FA10();
	if (Scen->PlayerSideIndex == 2) {
		FUN_0072ADE0(DAT_00844BF8, &DAT_00B0FBEC, &DAT_00B0FBF0);
		FUN_0072ADE0(DAT_00844BFC, &DAT_00B0FBF4, &DAT_00B0FBF8);
	} else {
		FUN_0072ADE0(DAT_00844C00, &DAT_00B0FBF4, &DAT_00B0FBF8);
		FUN_0072ADE0(DAT_00844BF4, &DAT_00B0FBEC, &DAT_00B0FBF0);
	}
	FUN_0072ADE0(DAT_00844BF0, &DAT_00B0FBE4, &DAT_00B0FBE8);
	FUN_0072ADE0(DAT_00844C04, &DAT_00B0FBFC, &DAT_00B0FC00);
	FUN_0072ADE0(DAT_00844C08, &DAT_00B0FC04, &DAT_00B0FC08);
	FUN_0072FC60(Options.ScreenWidth, Options.ScreenHeight);
	DAT_00B0FC0C = true;
}

// FUNCTION: GAMEMD 0x0072F330
void FUN_0072F330(void)
{
	FUN_00730100();
	FUN_00730400();
	FUN_00730530();
	DAT_00B0FC0C = false;
}

// FUNCTION: GAMEMD 0x0072F350
void FUN_0072F350(void)
{
	FUN_00730100();
	FUN_00730400();
	FUN_00730530();
	DAT_00B0FC0C = false;
	FUN_0072FA10();
	if (Scen->PlayerSideIndex == 2) {
		FUN_0072ADE0(DAT_00844BF8, &DAT_00B0FBEC, &DAT_00B0FBF0);
		FUN_0072ADE0(DAT_00844BFC, &DAT_00B0FBF4, &DAT_00B0FBF8);
	} else {
		FUN_0072ADE0(DAT_00844C00, &DAT_00B0FBF4, &DAT_00B0FBF8);
		FUN_0072ADE0(DAT_00844BF4, &DAT_00B0FBEC, &DAT_00B0FBF0);
	}
	FUN_0072ADE0(DAT_00844BF0, &DAT_00B0FBE4, &DAT_00B0FBE8);
	FUN_0072ADE0(DAT_00844C04, &DAT_00B0FBFC, &DAT_00B0FC00);
	FUN_0072ADE0(DAT_00844C08, &DAT_00B0FC04, &DAT_00B0FC08);
	FUN_0072FC60(Options.ScreenWidth, Options.ScreenHeight);
	DAT_00B0FC0C = true;
}

// FUNCTION: GAMEMD 0x0072F430
bool FUN_0072F430(void)
{
	return DAT_00B0FC0C;
}

// FUNCTION: GAMEMD 0x0072F440
void __fastcall FUN_0072F440(int index)
{
	if (index == 0) {
		DAT_00B0FA1C = DAT_00B0F9D8;
	} else if (index == 1) {
		DAT_00B0FA1C = DAT_00B0FB04;
	} else {
		DAT_00B0FA1C = DAT_00B0FAA0;
	}
}

// FUNCTION: GAMEMD 0x0072F4A0
PaletteClass * FUN_0072F4A0(void)
{
	return DAT_00B0FBE4;
}

namespace OwnerDraw { ConvertClass * GetSideButtonConvert(void); }

// FUNCTION: GAMEMD 0x0072F4B0
ConvertClass * OwnerDraw::GetSideButtonConvert(void)
{
	return DAT_00B0FBE8;
}

// FUNCTION: GAMEMD 0x0072F4C0
PaletteClass * FUN_0072F4C0(void)
{
	return DAT_00B0FC04;
}

// FUNCTION: GAMEMD 0x0072F4D0
ConvertClass * FUN_0072F4D0(void)
{
	return DAT_00B0FC08;
}

// FUNCTION: GAMEMD 0x0072F4E0
PaletteClass * FUN_0072F4E0(void)
{
	return DAT_00B0FBFC;
}

// FUNCTION: GAMEMD 0x0072F4F0
ConvertClass * FUN_0072F4F0(void)
{
	return DAT_00B0FC00;
}

// FUNCTION: GAMEMD 0x0072F500
PaletteClass * FUN_0072F500(void)
{
	return DAT_00B0FBF4;
}

// FUNCTION: GAMEMD 0x0072F510
ConvertClass * FUN_0072F510(void)
{
	return DAT_00B0FBF8;
}

// FUNCTION: GAMEMD 0x0072F520
PaletteClass * FUN_0072F520(void)
{
	return DAT_00B0FBEC;
}

// FUNCTION: GAMEMD 0x0072F530
ConvertClass * FUN_0072F530(void)
{
	return DAT_00B0FBF0;
}

extern char const * DAT_00844CFC;
extern char const * DAT_00844D00;
extern char const * DAT_00844D04;
extern char const * DAT_00844D08;
extern char const * DAT_00844D0C;
extern char const * DAT_00844D10;
extern char const * DAT_00844D14;
extern char const * DAT_00844D18;
extern char const * DAT_00844D1C;
extern char const * DAT_00844D20;
extern char const * DAT_00844D24;
extern char const * DAT_00844D28;
extern char const * DAT_00844D2C;
extern char const * DAT_00844D30;
extern char const * DAT_00844D34;
extern char const * DAT_00844D38;
extern char const * DAT_00844D3C;
extern char const * DAT_00844D40;
extern char const * DAT_00844D44;
extern char const * DAT_00844D48;
extern bool DAT_00B0FC9F;
extern bool DAT_00B0FCA0;
extern bool DAT_00B0FCA1;
extern bool DAT_00B0FCA2;
extern bool DAT_00B0FCA3;
extern bool DAT_00B0FCA4;
extern bool DAT_00B0FCA5;
extern bool DAT_00B0FCA6;
extern bool DAT_00B0FCA7;
extern bool DAT_00B0FCA8;
extern bool DAT_00B0FCA9;
extern bool DAT_00B0FCAA;
extern bool DAT_00B0FCAB;
extern bool DAT_00B0FCAC;
extern bool DAT_00B0FCAD;
extern bool DAT_00B0FCAE;
extern SHPStruct * DAT_00B0F9E0;
extern SHPStruct * DAT_00B0FA00;
extern SHPStruct * DAT_00B0FA3C;
extern SHPStruct * DAT_00B0FA48;
extern SHPStruct * DAT_00B0FA50;
extern SHPStruct * DAT_00B0FA68;
extern SHPStruct * DAT_00B0FA70;
extern SHPStruct * DAT_00B0FA8C;
extern SHPStruct * DAT_00B0FA90;
extern SHPStruct * DAT_00B0FAA8;
extern SHPStruct * DAT_00B0FABC;
extern SHPStruct * DAT_00B0FAC8;
extern SHPStruct * DAT_00B0FAD4;
extern SHPStruct * DAT_00B0FAFC;
extern SHPStruct * DAT_00B0FB08;
namespace OwnerDraw { extern SHPStruct * SideButtonShape; }

void FUN_0072FBC0(void);

// FUNCTION: GAMEMD 0x0072FA10
void FUN_0072FA10(void)
{
	if (Scen->PlayerSideIndex == 2) {
		DAT_00B0FA68 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D48, DAT_00B0FCA5);
		DAT_00B0FAC8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D3C, DAT_00B0FCA1);
		DAT_00B0FAD4 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D40, DAT_00B0FCA0);
		DAT_00B0FA50 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D44, DAT_00B0FCA2);
	} else {
		DAT_00B0FA68 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D14, DAT_00B0FCA5);
		DAT_00B0FAC8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D04, DAT_00B0FCA1);
		DAT_00B0FAD4 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D00, DAT_00B0FCA0);
		DAT_00B0FA50 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D08, DAT_00B0FCA2);
	}
	OwnerDraw::SideButtonShape = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844CFC, DAT_00B0FC9F);
	DAT_00B0FB08 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D0C, DAT_00B0FCA3);
	DAT_00B0F9E0 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D10, DAT_00B0FCA4);
	DAT_00B0FA70 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D18, DAT_00B0FCA6);
	DAT_00B0FAFC = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D1C, DAT_00B0FCA7);
	DAT_00B0FA00 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D20, DAT_00B0FCA8);
	DAT_00B0FA8C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D24, DAT_00B0FCA9);
	DAT_00B0FA48 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D28, DAT_00B0FCAA);
	DAT_00B0FA3C = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D2C, DAT_00B0FCAB);
	DAT_00B0FA90 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D30, DAT_00B0FCAC);
	DAT_00B0FAA8 = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D34, DAT_00B0FCAD);
	DAT_00B0FABC = (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00844D38, DAT_00B0FCAE);
}

// FUNCTION: GAMEMD 0x0072FBC0
void FUN_0072FBC0(void)
{
	if (Scen->PlayerSideIndex == 2) {
		FUN_0072ADE0(DAT_00844BF8, &DAT_00B0FBEC, &DAT_00B0FBF0);
		FUN_0072ADE0(DAT_00844BFC, &DAT_00B0FBF4, &DAT_00B0FBF8);
	} else {
		FUN_0072ADE0(DAT_00844C00, &DAT_00B0FBF4, &DAT_00B0FBF8);
		FUN_0072ADE0(DAT_00844BF4, &DAT_00B0FBEC, &DAT_00B0FBF0);
	}
	FUN_0072ADE0(DAT_00844BF0, &DAT_00B0FBE4, &DAT_00B0FBE8);
	FUN_0072ADE0(DAT_00844C04, &DAT_00B0FBFC, &DAT_00B0FC00);
	FUN_0072ADE0(DAT_00844C08, &DAT_00B0FC04, &DAT_00B0FC08);
}

extern int DAT_00B0FADC;
extern int DAT_00B0F9E4;
extern int DAT_00B0FAD8;
extern int DAT_00B0FAA4;
extern int DAT_00B0FA78;

// FUNCTION: GAMEMD 0x0072FC60
void __fastcall FUN_0072FC60(int width, int height)
{
	int x0 = width;
	SHPStruct * shape;
	if (width == 0x280) {
		shape = DAT_00B0FAD4;
	} else {
		shape = DAT_00B0FAC8;
		if (width != 0x320) {
			shape = DAT_00B0FA50;
		}
	}
	int w = shape->Width;
	int h = shape->Height;
	DAT_00B0FC30 = new Rect(0, 0, w, h);
	w = DAT_00B0FB08->Width;
	h = DAT_00B0FB08->Height;
	x0 -= w;
	DAT_00B0FC34 = new Rect(x0, 0, w, h);
	int h2 = DAT_00B0F9E0->Height;
	DAT_00B0FC38 = new Rect(x0, h, w, h2);
	h += h2;
	h2 = DAT_00B0FA68->Height;
	DAT_00B0FC3C = new Rect(x0, h, w, h2);
	h += h2;
	h2 = DAT_00B0FA70->Height;
	DAT_00B0FC44 = new Rect(x0, h, w, h2);
	h += h2;
	h2 = DAT_00B0FAFC->Height;
	DAT_00B0FC48 = new Rect(x0, h, w, h2);
	h2 = DAT_00B0FA8C->Height;
	DAT_00B0FC4C = new Rect(x0, h, w, h2);
	h2 = DAT_00B0FA48->Height;
	DAT_00B0FC50 = new Rect(x0, h, w, h2);
	DAT_00B0FADC = (height - DAT_00B0FC4C->Height - DAT_00B0FC44->Height - DAT_00B0FC3C->Height - DAT_00B0FC38->Height - DAT_00B0FC34->Height) / DAT_00B0FC48->Height;
	h += DAT_00B0FC48->Height * DAT_00B0FADC;
	DAT_00B0FC4C->Y = h;
	DAT_00B0FC50->Y = DAT_00B0FC4C->Height + h;
	int bottom = height - 0x20;
	int bar = FUN_0072AD90().Width;
	DAT_00B0FC58 = new Rect(0, bottom, bar, 0x20);
	w = DAT_00B0FA3C->Width;
	h = DAT_00B0FA3C->Height;
	DAT_00B0FC5C = new Rect(0, bottom, w, h);
	w = DAT_00B0FA90->Width;
	h = DAT_00B0FA90->Height;
	DAT_00B0FC60 = new Rect(0, bottom, w, h);
	DAT_00B0FC64 = new Rect(0, bottom, w, h);
	w = DAT_00B0FAA8->Width;
	h = DAT_00B0FAA8->Height;
	DAT_00B0FC68 = new Rect(0, bottom, w, h);
	w = DAT_00B0FABC->Width;
	h = DAT_00B0FABC->Height;
	DAT_00B0FC6C = new Rect(0, bottom, w, h);
	DAT_00B0F9E4 = (width - DAT_00B0FC60->Width - DAT_00B0FC34->Width - DAT_00B0FC6C->Width) / DAT_00B0FC68->Width;
	int x = DAT_00B0FC34->X - DAT_00B0FC6C->Width;
	DAT_00B0FC6C->X = x;
	x -= DAT_00B0FC68->Width * DAT_00B0F9E4;
	DAT_00B0FC68->X = x;
	x -= DAT_00B0FC60->Width;
	DAT_00B0FC60->X = x;
	DAT_00B0FAD8 = x;
	x = DAT_00B0FC6C->X - DAT_00B0FC64->Width;
	DAT_00B0FC64->X = x;
	DAT_00B0FAA4 = x;
	DAT_00B0FA78 = DAT_00B0FC6C->X - DAT_00B0FC60->Width - DAT_00B0FC60->X;
	int rx = DAT_00B0FC38->X;
	int ry = DAT_00B0FC38->Y;
	int rw = DAT_00B0FC38->Width;
	int rh = DAT_00B0FC3C->Height + DAT_00B0FC38->Height;
	DAT_00B0FC40 = new Rect(rx, ry, rw, rh);
	rx = DAT_00B0FC44->X;
	ry = DAT_00B0FC44->Y;
	rw = DAT_00B0FC44->Width;
	rh = DAT_00B0FC48->Height * DAT_00B0FADC + DAT_00B0FC4C->Height + DAT_00B0FC44->Height;
	DAT_00B0FC54 = new Rect(rx, ry, rw, rh);
}

// FUNCTION: GAMEMD 0x00730100
void FUN_00730100(void)
{
	if (DAT_00B0FC9F) {
		if (OwnerDraw::SideButtonShape) {
			delete OwnerDraw::SideButtonShape;
		}
		DAT_00B0FC9F = false;
	}
	OwnerDraw::SideButtonShape = NULL;
	if (DAT_00B0FCA0) {
		if (DAT_00B0FAD4) {
			delete DAT_00B0FAD4;
		}
		DAT_00B0FCA0 = false;
	}
	DAT_00B0FAD4 = NULL;
	if (DAT_00B0FCA1) {
		if (DAT_00B0FAC8) {
			delete DAT_00B0FAC8;
		}
		DAT_00B0FCA1 = false;
	}
	DAT_00B0FAC8 = NULL;
	if (DAT_00B0FCA2) {
		if (DAT_00B0FA50) {
			delete DAT_00B0FA50;
		}
		DAT_00B0FCA2 = false;
	}
	DAT_00B0FA50 = NULL;
	if (DAT_00B0FCA3) {
		if (DAT_00B0FB08) {
			delete DAT_00B0FB08;
		}
		DAT_00B0FCA3 = false;
	}
	DAT_00B0FB08 = NULL;
	if (DAT_00B0FCA4) {
		if (DAT_00B0F9E0) {
			delete DAT_00B0F9E0;
		}
		DAT_00B0FCA4 = false;
	}
	DAT_00B0F9E0 = NULL;
	if (DAT_00B0FCA5) {
		if (DAT_00B0FA68) {
			delete DAT_00B0FA68;
		}
		DAT_00B0FCA5 = false;
	}
	DAT_00B0FA68 = NULL;
	if (DAT_00B0FCA6) {
		if (DAT_00B0FA70) {
			delete DAT_00B0FA70;
		}
		DAT_00B0FCA6 = false;
	}
	DAT_00B0FA70 = NULL;
	if (DAT_00B0FCA7) {
		if (DAT_00B0FAFC) {
			delete DAT_00B0FAFC;
		}
		DAT_00B0FCA7 = false;
	}
	DAT_00B0FAFC = NULL;
	if (DAT_00B0FCA8) {
		if (DAT_00B0FA00) {
			delete DAT_00B0FA00;
		}
		DAT_00B0FCA8 = false;
	}
	DAT_00B0FA00 = NULL;
	if (DAT_00B0FCA9) {
		if (DAT_00B0FA8C) {
			delete DAT_00B0FA8C;
		}
		DAT_00B0FCA9 = false;
	}
	DAT_00B0FA8C = NULL;
	if (DAT_00B0FCAA) {
		if (DAT_00B0FA48) {
			delete DAT_00B0FA48;
		}
		DAT_00B0FCAA = false;
	}
	DAT_00B0FA48 = NULL;
	if (DAT_00B0FCAB) {
		if (DAT_00B0FA3C) {
			delete DAT_00B0FA3C;
		}
		DAT_00B0FCAB = false;
	}
	DAT_00B0FA3C = NULL;
	if (DAT_00B0FCAC) {
		if (DAT_00B0FA90) {
			delete DAT_00B0FA90;
		}
		DAT_00B0FCAC = false;
	}
	DAT_00B0FA90 = NULL;
	if (DAT_00B0FCAD) {
		if (DAT_00B0FAA8) {
			delete DAT_00B0FAA8;
		}
		DAT_00B0FCAD = false;
	}
	DAT_00B0FAA8 = NULL;
	if (DAT_00B0FCAE) {
		if (DAT_00B0FABC) {
			delete DAT_00B0FABC;
		}
		DAT_00B0FCAE = false;
	}
	DAT_00B0FABC = NULL;
}

// FUNCTION: GAMEMD 0x00730400
void FUN_00730400(void)
{
	if (DAT_00B0FC04) {
		delete DAT_00B0FC04;
		DAT_00B0FC04 = NULL;
	}
	Uncache_Shape(DAT_00844C08);
	if (DAT_00B0FC08) {
		delete DAT_00B0FC08;
		DAT_00B0FC08 = NULL;
	}
	if (DAT_00B0FBFC) {
		delete DAT_00B0FBFC;
		DAT_00B0FBFC = NULL;
	}
	Uncache_Shape(DAT_00844C04);
	if (DAT_00B0FC00) {
		delete DAT_00B0FC00;
		DAT_00B0FC00 = NULL;
	}
	if (DAT_00B0FBF4) {
		delete DAT_00B0FBF4;
		DAT_00B0FBF4 = NULL;
	}
	Uncache_Shape(DAT_00844C00);
	if (DAT_00B0FBF8) {
		delete DAT_00B0FBF8;
		DAT_00B0FBF8 = NULL;
	}
	if (DAT_00B0FBE4) {
		delete DAT_00B0FBE4;
		DAT_00B0FBE4 = NULL;
	}
	Uncache_Shape(DAT_00844BF0);
	if (DAT_00B0FBE8) {
		delete DAT_00B0FBE8;
		DAT_00B0FBE8 = NULL;
	}
	if (DAT_00B0FBEC) {
		delete DAT_00B0FBEC;
		DAT_00B0FBEC = NULL;
	}
	Uncache_Shape(DAT_00844BF4);
	if (DAT_00B0FBF0) {
		delete DAT_00B0FBF0;
		DAT_00B0FBF0 = NULL;
	}
}

// FUNCTION: GAMEMD 0x00730530
void FUN_00730530(void)
{
	delete DAT_00B0FC30;
	DAT_00B0FC30 = NULL;
	delete DAT_00B0FC34;
	DAT_00B0FC34 = NULL;
	delete DAT_00B0FC38;
	DAT_00B0FC38 = NULL;
	delete DAT_00B0FC3C;
	DAT_00B0FC3C = NULL;
	delete DAT_00B0FC40;
	DAT_00B0FC40 = NULL;
	delete DAT_00B0FC44;
	DAT_00B0FC44 = NULL;
	delete DAT_00B0FC48;
	DAT_00B0FC48 = NULL;
	delete DAT_00B0FC4C;
	DAT_00B0FC4C = NULL;
	delete DAT_00B0FC50;
	DAT_00B0FC50 = NULL;
	delete DAT_00B0FC54;
	DAT_00B0FC54 = NULL;
	delete DAT_00B0FC58;
	DAT_00B0FC58 = NULL;
	delete DAT_00B0FC5C;
	DAT_00B0FC5C = NULL;
	delete DAT_00B0FC60;
	DAT_00B0FC60 = NULL;
	delete DAT_00B0FC64;
	DAT_00B0FC64 = NULL;
	delete DAT_00B0FC68;
	DAT_00B0FC68 = NULL;
	delete DAT_00B0FC6C;
	DAT_00B0FC6C = NULL;
}

// FUNCTION: GAMEMD 0x00730660
void FUN_00730660(void)
{
	FUN_00730100();
	FUN_0072FA10();
}

// FUNCTION: GAMEMD 0x00730670
void FUN_00730670(void)
{
	FUN_00730400();
	if (Scen->PlayerSideIndex == 2) {
		FUN_0072ADE0(DAT_00844BF8, &DAT_00B0FBEC, &DAT_00B0FBF0);
		FUN_0072ADE0(DAT_00844BFC, &DAT_00B0FBF4, &DAT_00B0FBF8);
	} else {
		FUN_0072ADE0(DAT_00844C00, &DAT_00B0FBF4, &DAT_00B0FBF8);
		FUN_0072ADE0(DAT_00844BF4, &DAT_00B0FBEC, &DAT_00B0FBF0);
	}
	FUN_0072ADE0(DAT_00844BF0, &DAT_00B0FBE4, &DAT_00B0FBE8);
	FUN_0072ADE0(DAT_00844C04, &DAT_00B0FBFC, &DAT_00B0FC00);
	FUN_0072ADE0(DAT_00844C08, &DAT_00B0FC04, &DAT_00B0FC08);
}

// FUNCTION: GAMEMD 0x00730720
void __fastcall FUN_00730720(int width, int height)
{
	FUN_00730530();
	FUN_0072FC60(width, height);
}

// FUNCTION: GAMEMD 0x00730740
int FUN_00730740(void)
{
	return Options.ScreenWidth;
}

// FUNCTION: GAMEMD 0x00730750
int FUN_00730750(void)
{
	return Options.ScreenHeight;
}
