// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "ownrdraw.h"
#include "_keyboar.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_surface.h"
#include "_xmouse.h"
#include "arraylist.h"
#include "bsurface.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "dict.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "hsv.h"
#include "keyboard.h"
#include "language\language.h"
#include "mainloop.h"
#include "misc.h"
#include "mixfile.h"
#include "msgloop.h"
#include "rgb.h"
#include "rules.h"
#include "session.h"
#include "srfcache.h"
#include "swap.h"
#include "theme.h"
#include "voc.h"
#include "vox.h"
#include "windlg.h"
#include <commctrl.h>
#include <ctime>
#include <sys\timeb.h>

int _mouse_counter;

void ODInitMasks(void);

int ODColorToHiColor(COLORREF color);

int WINAPI ODUpdateWindowRect(HWND window, RECT *rect);

BOOL CALLBACK ODAddWindowToList(HWND window, ArrayList<HWND> * list);

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EF700;
extern const double DAT_007EF708;

// FUNCTION: 0x00600340 $E54
double DAT_00AC1B40 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00600370 $E57
double DAT_00AC48A0 = DAT_00AC1B40 / DAT_007EF700;

// FUNCTION: 0x00600390 $E60
double DAT_00AC4838 = DAT_007E1718 / DAT_00AC48A0;

// FUNCTION: 0x006003B0 $E63
double DAT_00AC4830 = DAT_007E1720 * DAT_007EF708;

// FUNCTION: 0x006003D0 $E66
double DAT_00AC1B88 = DAT_007E1728 * DAT_007EF708;

// FUNCTION: 0x006003F0 $E69
double DAT_00AC1898 = DAT_007E1730 * DAT_007EF708;

// FUNCTION: 0x00600410 $E72
int DAT_00AC48AC = (int)(fastmath::tan(DAT_00AC1898 - DAT_00AC1B88) * DAT_007E1738 * DAT_00AC1B40);

// FUNCTION: 0x00600450 $E75
double DAT_00AC1CA0 = fastmath::atan(DAT_00AC48AC * DAT_007E1740);

// FUNCTION: 0x00600480 $E78
double DAT_00AC4610 = fastmath::atan((DAT_00AC48AC * 2) / DAT_00AC1B40);

// FUNCTION: 0x006004B0 $E81
int DAT_00AC489C = (int)(DAT_00AC48AC * 4 + DAT_007E1738);

struct ODShortPair { short a; short b; ODShortPair(void) { a = 0; b = 0; } };
struct ODIntTriple { int a; int b; int c; ODIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x006004E0 $E84
ODShortPair DAT_00AC1B38;

// FUNCTION: 0x006004F0 $E87
ODIntTriple DAT_00AC1DF8;


struct ODCenterTriple { int a; int b; int c; ODCenterTriple(void) { c = 0; a = 128; b = 128; } };

// FUNCTION: 0x00600510 $E90
ODCenterTriple DAT_00AC4888;

// FUNCTION: 0x00600530 $E95
// FUNCTION: 0x00600550 $E93
SurfaceCacheClass SurfaceCache;

namespace OwnerDraw {
	extern int ControlInsetPx;
	extern int ListSelectionFillColor;
	extern int DefaultBorderColor;
	extern int ScrollButtonBevelAlpha;
	extern int PrimaryTextColor;
	extern int AltComboTextColor;
	extern int ImeCompositionTextColor;
	extern int CaretColor;
	extern int UnusedMidGrayColor;
	extern int AltSelectionFillColor;
	extern int AltBorderColor;
	extern int BevelLightColor;
	extern int BevelShadowColor;
	extern int DisabledTextColor;
	extern int AltDisabledTextColor;
	extern int DisabledBorderColor;
	extern int AltDisabledBorderColor;
	extern int DisabledOverlayAlpha;
	extern int SelectedTabTextColor;
	extern int TooltipBackgroundColor;
	extern int UnusedDarkAccentColor;
}

// FUNCTION: 0x00600560
void OwnerDraw::Initialize(void)
{
	ControlInsetPx = 1;
	ListSelectionFillColor = 0xff;
	DefaultBorderColor = 0xff;
	ScrollButtonBevelAlpha = 0x7f;
	PrimaryTextColor = 0xffff;
	AltComboTextColor = 0xeeeeee;
	ImeCompositionTextColor = 0x9f9f;
	CaretColor = 0xffffff;
	UnusedMidGrayColor = 0x808080;
	AltSelectionFillColor = 0x626262;
	AltBorderColor = 0x929292;
	BevelLightColor = 0xc5bea7;
	BevelShadowColor = 0x807a68;
	DisabledTextColor = 0x9f;
	AltDisabledTextColor = 0x8f8f8f;
	DisabledBorderColor = 0x9f;
	AltDisabledBorderColor = 0x646464;
	DisabledOverlayAlpha = 0x60;
	SelectedTabTextColor = 0xaaaa;
	TooltipBackgroundColor = 0x221b0b;
	UnusedDarkAccentColor = 0x443716;
}

// FUNCTION: 0x00600630
OwnerDraw::CellData::CellData(void)
{
	type = CellData::INVALID;
	color = -1;
	pingtime = -1;
	surf = NULL;
}

// FUNCTION: 0x00600660
unsigned int Hash_HWND(HWND &key)
{
	return(*(unsigned int*)&key);
}

// FUNCTION: 0x00600670
unsigned int Hash_CtrlMsg(OwnerDraw::CtrlMsgData &key)
{
	return((unsigned int)((int)key.message * (int)key.window));
}

// FUNCTION: 0x00600680
bool OwnerDraw::CtrlMsgData::operator ==(CtrlMsgData const & that) const
{
	if (that.window == window && that.message == message){
		return(true);
	}
	return(false);
}


// FUNCTION: 0x006006B0 $E100
// FUNCTION: 0x00600750 $E98
namespace OwnerDraw { Dictionary<HWND, WNDPROC> DialogProcs(Hash_HWND); }

// FUNCTION: 0x00600880 $E105
// FUNCTION: 0x00600920 $E103
namespace OwnerDraw { Dictionary<HWND, WNDPROC> SubclassProcs(Hash_HWND); }

// FUNCTION: 0x00600A50 $E110
namespace OwnerDraw { Dictionary<HWND, OwnerDraw::WinData> Dialogs(Hash_HWND); }

// FUNCTION: 0x00600C70
bool Focus_Next_Tab_Item(HWND window, bool previous)
{
	HWND parent = GetParent(window);
	HWND next = GetNextDlgTabItem(parent, window, previous);
	if (next != window) {
		SetFocus(next);
		return true;
	}
	return false;
}

// FUNCTION: 0x00601360
bool Is_Owner_Draw_Message(unsigned int msg)
{
	if (msg == 0x10c ||
		msg == 0x94 ||
		msg == 0xb6 ||
		msg == 0xa3 ||
		msg == 0x73 ||
		msg == 0xd8 ||
		msg == 0xbbb ||
		msg == 0xf5 ||
		msg == 0x105 ||
		msg == 0xe2 ||
		msg == 0xd5 ||
		msg == 0x2b5 ||
		msg == 0xb7 ||
		msg == 0x2b4 ||
		msg == 0x101 ||
		msg == 0x129 ||
		msg == 0xbc7 ||
		msg == 0xbc6 ||
		msg == 0xd7 ||
		msg == 0x108 ||
		msg == 0xbc ||
		msg == 0xbd ||
		msg == 0xbb ||
		msg == 0xb5 ||
		msg == 0xbba ||
		msg == 0xff ||
		msg == 0xea ||
		msg == 0x6b ||
		msg == 0x100 ||
		msg == 0x102 ||
		msg == 0xb8 ||
		msg == 0xd6 ||
		msg == 0x103 ||
		msg == 0x11f ||
		msg == 0x125 ||
		msg == 0x115 ||
		msg == 0x122 ||
		msg == 0x112 ||
		msg == 0xe7 ||
		msg == 0x116 ||
		msg == 0xc2 ||
		msg == 0xc9 ||
		msg == 0x11d ||
		msg == 0x11c ||
		msg == 0x113 ||
		msg == 0xfe ||
		msg == 0x109 ||
		msg == 0x10f ||
		msg == 0x117 ||
		msg == 0x114 ||
		msg == 0xc3 ||
		msg == 0x124 ||
		msg == 0xe6 ||
		msg == 0xf3 ||
		msg == 0xf4 ||
		msg == 0x2bc ||
		msg == 0x11b ||
		msg == 0x10e) {
		return true;
	}
	return false;
}

// FUNCTION: 0x006023F0
int Typed_Char_Sound(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if ((type == 0x103 || type == 0xbc7) && id == 0x6f4) {
		return Rule->MessageCharTyped;
	}
	return -1;
}

// FUNCTION: 0x00602AE0
int Is_Owner_Draw_Subset_Message(unsigned int msg)
{
	if (msg == 0x125 ||
		msg == 0xe7 ||
		msg == 0x116 ||
		msg == 0xc2 ||
		msg == 0xc9 ||
		msg == 0x11d ||
		msg == 0x11c ||
		msg == 0x114 ||
		msg == 0x2bc ||
		msg == 0x113 ||
		msg == 0x109 ||
		msg == 0x10f ||
		msg == 0x117 ||
		msg == 0xe6 ||
		msg == 0xf3 ||
		msg == 0xf4 ||
		msg == 0x10e ||
		msg == 0x112) {
		return 1;
	}
	return 0;
}

// FUNCTION: 0x00602B90
bool Is_Special_Control(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);

	if (type == 0xbc || type == 0xbd || type == 0xc2 || type == 0xc9 || type == 0xbc6) {
		if (id == 0x6d2 ||
		id == 0x6d3 ||
		id == 0x6d4 ||
		id == 0x6d5 ||
		id == 0x6d6 ||
		id == 0x6d7 ||
		id == 0x6d8 ||
		id == 0x6d9) {
			return true;
		}
	}
	if (type == 0x102 || type == 0xbc || type == 0xbd || type == 0xc2 || type == 0xc9) {
		if (id == 0x6da ||
		id == 0x6db ||
		id == 0x6dc ||
		id == 0x6dd ||
		id == 0x6de ||
		id == 0x6df ||
		id == 0x6e0 ||
		id == 0x6e1) {
			return true;
		}
	}
	if (type == 0xc2 || type == 0xc9) {
		if (id == 0x754 ||
		id == 0x755 ||
		id == 0x756 ||
		id == 0x757 ||
		id == 0x758 ||
		id == 0x759 ||
		id == 0x75a ||
		id == 0x75b) {
			return true;
		}
	}
	if (type == 0xbc6) {
		if (id == 0x77f || id == 0x6db) {
			return true;
		}
	}
	return false;
}

// FUNCTION: 0x00602D70
bool Is_Unused_Control(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;

	OwnerDraw::Dialogs.getPointer(dialog, &data);
	GetDlgCtrlID(control);
	return false;
}

// FUNCTION: 0x00602DD0
bool Is_Typed_Control(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);

	if (type == 0x94 ||
		type == 0xd8 ||
		type == 0xf5 ||
		type == 0xe2 ||
		type == 0xd5 ||
		type == 0x101 ||
		type == 0x129 ||
		type == 0xd7 ||
		type == 0xbb ||
		type == 0x100 ||
		type == 0xd6 ||
		type == 0x125 ||
		type == 0x122 ||
		type == 0x112 ||
		type == 0xe7 ||
		type == 0x116 ||
		type == 0x11d ||
		type == 0x11c ||
		type == 0xfe ||
		type == 0x10f ||
		type == 0x117 ||
		type == 0x114 ||
		type == 0xe6 ||
		type == 0xf3 ||
		type == 0xf4 ||
		type == 0x2bc ||
		type == 0x10e ||
		type == 0x108 ||
		type == 0xbc6 ||
		type == 0xbc7) {
		if (id == 0x71c) {
			return true;
		}
	}
	if (type == 0x94) {
		if (id == 0x6ea || id == 0x6ec || id == 0x6eb) {
			return true;
		}
	} else if (type == 0x103) {
		if (id == 0x72b) {
			return true;
		}
	} else if (type == 0xbc7) {
		if (id == 0x72b) {
			return true;
		}
	} else if (type == 0xc4) {
		if (id == 0x7a9) {
			return true;
		}
	}
	return false;
}

// FUNCTION: 0x00602FE0
int Control_Slot_Count(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;

	OwnerDraw::Dialogs.getPointer(dialog, &data);
	GetDlgCtrlID(control);
	return 12;
}

// FUNCTION: 0x00603040
int Control_Sound_Slot(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);

	if (type == 0x94 ||
		type == 0xd8 ||
		type == 0xf5 ||
		type == 0xe2 ||
		type == 0xd5 ||
		type == 0x101 ||
		type == 0x129 ||
		type == 0xd7 ||
		type == 0xbb ||
		type == 0x100 ||
		type == 0xd6 ||
		type == 0x125 ||
		type == 0x122 ||
		type == 0x112 ||
		type == 0xe7 ||
		type == 0x116 ||
		type == 0x11d ||
		type == 0x11c ||
		type == 0xfe ||
		type == 0x10f ||
		type == 0x117 ||
		type == 0x114 ||
		type == 0xe6 ||
		type == 0xf3 ||
		type == 0xf4 ||
		type == 0x2bc ||
		type == 0x10e ||
		type == 0x108 ||
		type == 0xbc6 ||
		type == 0xbc7) {
		if (id == 0x71c) {
			return 0x25;
		}
	}
	if (type == 0xbc || type == 0xbd || type == 0xc2 || type == 0xc9 || type == 0x102 || type == 0xbc6) {
		if (id == 0x6ec || id == 0x5a8) {
			return 0xe;
		}
	}
	return 0;
}

// FUNCTION: 0x00603240
int Is_Typed_Control_Int(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);

	if (type == 0x94 ||
		type == 0xd8 ||
		type == 0xf5 ||
		type == 0xe2 ||
		type == 0xd5 ||
		type == 0x101 ||
		type == 0x129 ||
		type == 0xd7 ||
		type == 0xbb ||
		type == 0x100 ||
		type == 0xd6 ||
		type == 0x125 ||
		type == 0x122 ||
		type == 0x112 ||
		type == 0xe7 ||
		type == 0x116 ||
		type == 0x11d ||
		type == 0x11c ||
		type == 0xfe ||
		type == 0x10f ||
		type == 0x117 ||
		type == 0x114 ||
		type == 0xe6 ||
		type == 0xf3 ||
		type == 0xf4 ||
		type == 0x2bc ||
		type == 0x10e ||
		type == 0x108 ||
		type == 0xbc6 ||
		type == 0xbc7) {
		if (id == 0x71c) {
			return 1;
		}
	}
	return 0;
}

// FUNCTION: 0x006033F0
int Control_Volume(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if ((type == 0x94 ||
		type == 0xd8 ||
		type == 0xf5 ||
		type == 0xe2 ||
		type == 0xd5 ||
		type == 0x101 ||
		type == 0x129 ||
		type == 0xd7 ||
		type == 0xbb ||
		type == 0x100 ||
		type == 0xd6 ||
		type == 0x125 ||
		type == 0x122 ||
		type == 0x112 ||
		type == 0xe7 ||
		type == 0x116 ||
		type == 0x11d ||
		type == 0x11c ||
		type == 0xfe ||
		type == 0x10f ||
		type == 0x117 ||
		type == 0x114 ||
		type == 0xe6 ||
		type == 0xf3 ||
		type == 0xf4 ||
		type == 0x2bc ||
		type == 0x10e ||
		type == 0x108 ||
		type == 0xbc6 ||
		type == 0xbc7) && id == 0x71c) {
		return 100;
	}
	if (type == 0x94 && id == 0x6ea) {
		return 100;
	}
	if (type == 0x94 && id == 0x6ec) {
		return 100;
	}
	if (type == 0x94 && id == 0x6eb) {
		return 100;
	}
	if (type == 0x103 && id == 0x72b) {
		return 100;
	}
	if (type == 0xbc7 && id == 0x72b) {
		return 100;
	}
	if (type == 0xc4 && id == 0x7a9) {
		return 50;
	}
	return 0;
}

void FUN_0072E280(void);
void FUN_0072E2A0(void);
void FUN_0072DAC0(void);
void FUN_0072DAD0(void);
void FUN_0072D450(void);
void FUN_0072CC80(void);

// FUNCTION: 0x006035F0
void Play_Control_Sound(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	FUN_0072E280();

	if (type == 0x94 ||
		type == 0xd8 ||
		type == 0xf5 ||
		type == 0xe2 ||
		type == 0xd5 ||
		type == 0x101 ||
		type == 0x129 ||
		type == 0xd7 ||
		type == 0xbb ||
		type == 0x100 ||
		type == 0xd6 ||
		type == 0x125 ||
		type == 0x122 ||
		type == 0x112 ||
		type == 0xe7 ||
		type == 0x116 ||
		type == 0x11d ||
		type == 0x11c ||
		type == 0xfe ||
		type == 0x10f ||
		type == 0x117 ||
		type == 0x114 ||
		type == 0xe6 ||
		type == 0xf3 ||
		type == 0xf4 ||
		type == 0x2bc ||
		type == 0x10e ||
		type == 0x108 ||
		type == 0xbc6 ||
		type == 0xbc7) {
		if (id == 0x71c) {
			FUN_0072E2A0();
		}
	}
	if (type == 0x94) {
		if (id == 0x6ea) {
			FUN_0072DAC0();
			return;
		}
		if (id == 0x6ec) {
			FUN_0072DAD0();
			return;
		}
	}
	if (type == 0x103) {
		goto typed;
	}
	if (type == 0xbc7) {
typed:
		if (id == 0x72b) {
			FUN_0072D450();
		}
		return;
	}
	if (type == 0xc4) {
		if (id == 0x7a9) {
			FUN_0072CC80();
		}
	}
}

// FUNCTION: 0x0061F190
void ODInitMasks(void)
{
	ODRComponentMask = 255;
	ODRComponentMask = ODRComponentMask >> DSurface::Get_Red_Left();
	ODRComponentMask <<= DSurface::Get_Red_Right();

	ODGComponentMask = 255;
	ODGComponentMask = ODGComponentMask >> DSurface::Get_Green_Left();
	ODGComponentMask <<= DSurface::Get_Green_Right();

	ODBComponentMask = 255;
	ODBComponentMask = ODBComponentMask >> DSurface::Get_Blue_Left();
	ODBComponentMask <<= DSurface::Get_Blue_Right();
}

// FUNCTION: 0x00620810
int ODColorToHiColor(COLORREF color)
{
	if (color == 0xFFFFFFFF) {
		return(0xFFFFFFFF);
	}
	union {
		struct {
			unsigned int red : 8;
			unsigned int green : 8;
			unsigned int blue : 8;
			unsigned int a : 8;
		};
		int v;
	} c;

	c.v = color;

	return(DSurface::Build_Hicolor_Pixel(c.red, c.green, c.blue));
}

// FUNCTION: 0x00621150
float ODCalcTextRemapFactor(int hue)
{
	float val = 1.0f;

	int arr[3];
	arr[0] = 43;
	arr[1] = 128;
	arr[2] = 213;

	for (int i = 0; i < 3; i++) {
		int value = arr[i];

		if (hue > value - 16 && hue <= value) {
			val = float(value - hue);
			val *= (1.0f / 16);
			val *= (60.0f / 100);
			val += (40.0f / 100);
		} else if (hue > value && hue <= value + 16) {
			val = float(hue - value);
			val *= (1.0f / 16);
			val *= (60.0f / 100);
			val += (40.0f / 100);
		}
	}
	return(val);
}

// FUNCTION: 0x00621D30
void ODDrawBitsStrand(Rect const & rect, Surface & surface)
{
	unsigned char paldata[768];
	Surface *img = SurfaceCache.GetSurface("bits_i.pcx", paldata);
	Surface *mask = SurfaceCache.GetSurface("bits_a.pcx", NULL);
	int index = 0;
	SurfaceCacheConvertPalette(paldata);
	Rect work = rect;
	int rnd;

	for (int x = 0; x < rect.Width; x += 10) {
		rnd = rand() % 8;
		work.Set(rect.X+x, rect.Y+1, 10, 12);
		SurfaceCache.DrawMasked(work, surface, *img, *mask, paldata, 0, 10 * rnd, 0);

		rnd = rand() % 8;
		work.Set(rect.X+x, rect.Y+rect.Height-16, 10, 12);
		SurfaceCache.DrawMasked(work, surface, *img, *mask, paldata, 0, 10 * rnd, 0);

		index++;

		if ((index % 6) == 0) {
			x += 10;
		}
	}
}

// FUNCTION: 0x00622440
int WINAPI ODUpdateWindowRect(HWND window, RECT *rect)
{
	InvalidateRect(window, rect, FALSE);
	UpdateWindow(window);
	return(1);
}

// FUNCTION: 0x00622470
BOOL CALLBACK ODAddWindowToList(HWND window, ArrayList<HWND> * list)
{
	if (list != NULL) {
		list->add(window, list->length());
	}
	return(TRUE);
}

// FUNCTION: 0x006225E0
int OwnerDraw::Capture_Mouse(void)
{
	if (MouseCursor != NULL) {
		if (MouseCursor->Is_Captured() == true) {
			MouseCursor->Release_Mouse();
		}
	}
	_mouse_counter++;
	return(_mouse_counter);
}

// FUNCTION: 0x00622610
int OwnerDraw::Release_Mouse(void)
{
	if (_mouse_counter > 0) {
		_mouse_counter--;
	}
	if (_mouse_counter == 0) {
		if (MouseCursor != NULL) {
			if (!MouseCursor->Is_Captured()) {
				MouseCursor->Capture_Mouse();
			}
		}
	}
	return(_mouse_counter);
}

// FUNCTION: 0x00622800
void OwnerDraw::Display_Dialog(HWND window)
{
	ShowWindow(window, SW_SHOWNORMAL);
	SetForegroundWindow(window);
	Keyboard->Clear();
}

// FUNCTION: 0x00623170
int OwnerDraw::Move_Dialog(HWND window, int x, int y)
{
	int xpos;
	int ypos;

	RECT rect1;
	rect1.left = 0;
	rect1.top = 0;
	rect1.right = VideoModeWidth;
	rect1.bottom = VideoModeHeight;

	ClientToScreen(MainWindow, (LPPOINT)&rect1);
	ClientToScreen(MainWindow, (LPPOINT)&rect1.right);

	RECT rect2;
	GetWindowRect(window, &rect2);

	rect2.right -= rect2.left;
	rect2.bottom -= rect2.top;

	if (x == -1) {
		xpos = rect2.left - rect1.left;
	} else {
		xpos = x;
	}
	rect2.left = xpos;

	if (y == -1) {
		ypos = rect2.top - rect1.top;
	} else {
		ypos = y;
	}
	rect2.top = ypos;

	return(MoveWindow(window, rect2.left, rect2.top, rect2.right, rect2.bottom, FALSE));
}
