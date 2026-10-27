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
#include "scenario.h"
#include "session.h"
#include "srfcache.h"
#include "swap.h"
#include "theme.h"
#include "voc.h"
#include "vox.h"
#include "windlg.h"
#include "bitfont.h"
#include "geometry.h"
#include <commctrl.h>
#include <ctime>
#include <mbstring.h>
#include <sys\timeb.h>
#undef LoadString
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x00600320 SYMBOL
// _$E2

int _mouse_counter;

char const * __fastcall GetTooltipStringLabel(int type, int id);
extern wchar_t const DAT_00887734[];

bool Is_Owner_Draw_Message(unsigned int msg);
bool __fastcall FUN_0069BBE0(SessionClass *session);
bool __fastcall FUN_00602AE0(unsigned int msg);

void ODInitMasks(void);

int ODColorToHiColor(COLORREF color);

int WINAPI ODUpdateWindowRect(HWND window, RECT *rect);

BOOL CALLBACK ODAddWindowToList(HWND window, ArrayList<HWND> * list);

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
// SYNTHETIC: GAMEMD 0x00600340 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x00600370 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x00600390 SYMBOL
// _$E62
// SYNTHETIC: GAMEMD 0x006003B0 SYMBOL
// _$E65
// SYNTHETIC: GAMEMD 0x006003D0 SYMBOL
// _$E68
// SYNTHETIC: GAMEMD 0x006003F0 SYMBOL
// _$E71
// SYNTHETIC: GAMEMD 0x00600410 SYMBOL
// _$E74
// SYNTHETIC: GAMEMD 0x00600450 SYMBOL
// _$E77
// SYNTHETIC: GAMEMD 0x00600480 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x006004B0 SYMBOL
// _$E83
// SYNTHETIC: GAMEMD 0x006004E0 SYMBOL
// _$E86
// SYNTHETIC: GAMEMD 0x006004F0 SYMBOL
// _$E89
// SYNTHETIC: GAMEMD 0x00600510 SYMBOL
// _$E92

// SYNTHETIC: GAMEMD 0x00600530 SYMBOL
// _$E97
// SYNTHETIC: GAMEMD 0x00600550 SYMBOL
// _$E95
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

// FUNCTION: GAMEMD 0x00600560
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

// FUNCTION: GAMEMD 0x00600630
OwnerDraw::CellData::CellData(void)
{
	type = CellData::INVALID;
	color = -1;
	pingtime = -1;
	surf = NULL;
}

// FUNCTION: GAMEMD 0x00600660
unsigned int Hash_HWND(HWND &key)
{
	return(*(unsigned int*)&key);
}

// FUNCTION: GAMEMD 0x00600670
unsigned int Hash_CtrlMsg(OwnerDraw::CtrlMsgData &key)
{
	return((unsigned int)((int)key.message * (int)key.window));
}

// FUNCTION: GAMEMD 0x00600680
bool OwnerDraw::CtrlMsgData::operator ==(CtrlMsgData const & that) const
{
	if (that.window == window && that.message == message){
		return(true);
	}
	return(false);
}


// SYNTHETIC: GAMEMD 0x006006B0 SYMBOL
// _$E102
// SYNTHETIC: GAMEMD 0x00600750 SYMBOL
// _$E100
namespace OwnerDraw { Dictionary<HWND, WNDPROC> DialogProcs(Hash_HWND); }

// SYNTHETIC: GAMEMD 0x00600880 SYMBOL
// _$E107
// SYNTHETIC: GAMEMD 0x00600920 SYMBOL
// _$E105
namespace OwnerDraw { Dictionary<HWND, WNDPROC> SubclassProcs(Hash_HWND); }

// SYNTHETIC: GAMEMD 0x00600A50 SYMBOL
// _$E112
namespace OwnerDraw { Dictionary<HWND, OwnerDraw::WinData> Dialogs(Hash_HWND); }

// FUNCTION: GAMEMD 0x00600C70
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

inline bool Is_Quick_Control_B(int id)
{
	return id == 0x411 ||
		id == 0x412 ||
		id == 0x419 ||
		id == 0x41a ||
		id == 0x6a3 ||
		id == 0x6a4 ||
		id == 0x6a5 ||
		id == 0x6a7 ||
		id == 0x6a6 ||
		id == 0x6a8 ||
		id == 0x73a ||
		id == 0x739 ||
		id == 0x73b ||
		id == 0x738;
}

inline bool Is_Quick_Control_D(int id)
{
	return id == 0x419 ||
		id == 0x41a ||
		id == 0x41b ||
		id == 0x41c ||
		id == 0x41d ||
		id == 0x41e ||
		id == 0x41f ||
		id == 0x420 ||
		id == 0x6a3 ||
		id == 0x6a4 ||
		id == 0x6a9 ||
		id == 0x6ac ||
		id == 0x6af ||
		id == 0x6b2 ||
		id == 0x6b5 ||
		id == 0x6b8 ||
		id == 0x6a5 ||
		id == 0x6a7 ||
		id == 0x6aa ||
		id == 0x6ad ||
		id == 0x6b0 ||
		id == 0x6b3 ||
		id == 0x6b6 ||
		id == 0x6b9 ||
		id == 0x6a6 ||
		id == 0x6a8 ||
		id == 0x6ab ||
		id == 0x6ae ||
		id == 0x6b1 ||
		id == 0x6b4 ||
		id == 0x6b7 ||
		id == 0x6ba ||
		id == 0x70a ||
		id == 0x70b ||
		id == 0x70c ||
		id == 0x70d ||
		id == 0x70e ||
		id == 0x70f ||
		id == 0x710 ||
		id == 0x711;
}

// FUNCTION: GAMEMD 0x00601360
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

// FUNCTION: GAMEMD 0x006015E0
int FUN_006015E0(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if ((type == 0x10c ||
			type == 0x94 ||
			type == 0xa3 ||
			type == 0x73 ||
			type == 0xd8 ||
			type == 0xbbb ||
			type == 0xf5 ||
			type == 0x105 ||
			type == 0xe2 ||
			type == 0xd5 ||
			type == 0x2b5 ||
			type == 0xb7 ||
			type == 0x2b4 ||
			type == 0xbc7 ||
			type == 0x101 ||
			type == 0x129 ||
			type == 0xbc6 ||
			type == 0xd7 ||
			type == 0x108 ||
			type == 0xbc ||
			type == 0xbd ||
			type == 0xbb ||
			type == 0xb5 ||
			type == 0xbba ||
			type == 0xff ||
			type == 0x6b ||
			type == 0x100 ||
			type == 0x102 ||
			type == 0xb8 ||
			type == 0xd6 ||
			type == 0x103 ||
			type == 0x125 ||
			type == 0xe7 ||
			type == 0x116 ||
			type == 0xc2 ||
			type == 0xc9 ||
			type == 0x11d ||
			type == 0x11c ||
			type == 0x114 ||
			type == 0x2bc ||
			type == 0x113 ||
			type == 0x109 ||
			type == 0x10f ||
			type == 0x117 ||
			type == 0xe6 ||
			type == 0xf3 ||
			type == 0xf4 ||
			type == 0x10e ||
			type == 0x112) && id == 0x694) {
		return 1;
	}
	if ((type == 0xbc ||
			type == 0xbd ||
			type == 0x102 ||
			type == 0xc2 ||
			type == 0xc9 ||
			type == 0xbc6) && (id == 0x6ec ||
			id == 0x5a8)) {
		return 1;
	}
	if (type == 0x103 || type == 0xbc7) {
		if (id == 0x6f4) {
			return 3;
		}
		if (id == 0x6f3 ||
			id == 0x69f ||
			id == 0x6f6 ||
			id == 0x6ef ||
			id == 0x6f1 ||
			id == 0x6f0 ||
			id == 0x6f2 ||
			id == 0x733 ||
			id == 0x735 ||
			id == 0x732 ||
			id == 0x3ea ||
			id == 0x73c ||
			id == 0x73d ||
			id == 0x73e ||
			id == 0x73f ||
			id == 0x740 ||
			id == 0x736 ||
			id == 0x737 ||
			id == 0x411 ||
			id == 0x412 ||
			id == 0x419 ||
			id == 0x41a ||
			id == 0x6a3 ||
			id == 0x6a4 ||
			id == 0x6a5 ||
			id == 0x6a7 ||
			id == 0x6a6 ||
			id == 0x6a8 ||
			id == 0x73a ||
			id == 0x739 ||
			id == 0x73b ||
			id == 0x738) {
			return 1;
		}
	}
	if (type == 0x108 || type == 0xbc6) {
		if (id == 0x6d2 ||
			id == 0x3ea ||
			id == 0x733 ||
			id == 0x71b ||
			id == 0x732 ||
			id == 0x69d ||
			id == 0x69f ||
			id == 0x69e ||
			id == 0x6a0 ||
			id == 0x78b ||
			id == 0x5c8 ||
			id == 0x411 ||
			id == 0x412 ||
			id == 0x413 ||
			id == 0x414 ||
			id == 0x415 ||
			id == 0x416 ||
			id == 0x417 ||
			id == 0x418 ||
			id == 0x419 ||
			id == 0x41a ||
			id == 0x41b ||
			id == 0x41c ||
			id == 0x41d ||
			id == 0x41e ||
			id == 0x41f ||
			id == 0x420 ||
			id == 0x6a3 ||
			id == 0x6a4 ||
			id == 0x6a9 ||
			id == 0x6ac ||
			id == 0x6af ||
			id == 0x6b2 ||
			id == 0x6b5 ||
			id == 0x6b8 ||
			id == 0x6a5 ||
			id == 0x6a7 ||
			id == 0x6aa ||
			id == 0x6ad ||
			id == 0x6b0 ||
			id == 0x6b3 ||
			id == 0x6b6 ||
			id == 0x6b9 ||
			id == 0x6a6 ||
			id == 0x6a8 ||
			id == 0x6ab ||
			id == 0x6ae ||
			id == 0x6b1 ||
			id == 0x6b4 ||
			id == 0x6b7 ||
			id == 0x6ba ||
			id == 0x70a ||
			id == 0x70b ||
			id == 0x70c ||
			id == 0x70d ||
			id == 0x70e ||
			id == 0x70f ||
			id == 0x710 ||
			id == 0x711) {
			return 1;
		}
	}
	return (Is_Owner_Draw_Message(type) && id == 0x695) ? 3 : 1;
}

// FUNCTION: GAMEMD 0x00601D20
int FUN_00601D20(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if ((type == 0x10c ||
			type == 0x94 ||
			type == 0xa3 ||
			type == 0x73 ||
			type == 0xd8 ||
			type == 0xbbb ||
			type == 0xf5 ||
			type == 0x105 ||
			type == 0xe2 ||
			type == 0xd5 ||
			type == 0x2b5 ||
			type == 0xb7 ||
			type == 0x2b4 ||
			type == 0xbc7 ||
			type == 0x101 ||
			type == 0x129 ||
			type == 0xbc6 ||
			type == 0xd7 ||
			type == 0x108 ||
			type == 0xbc ||
			type == 0xbd ||
			type == 0xbb ||
			type == 0xb5 ||
			type == 0xbba ||
			type == 0xff ||
			type == 0x6b ||
			type == 0x100 ||
			type == 0x102 ||
			type == 0xb8 ||
			type == 0xd6 ||
			type == 0x103 ||
			type == 0x125 ||
			type == 0xe7 ||
			type == 0x116 ||
			type == 0xc2 ||
			type == 0xc9 ||
			type == 0x11d ||
			type == 0x11c ||
			type == 0x114 ||
			type == 0x2bc ||
			type == 0x113 ||
			type == 0x109 ||
			type == 0x10f ||
			type == 0x117 ||
			type == 0xe6 ||
			type == 0xf3 ||
			type == 0xf4 ||
			type == 0x10e ||
			type == 0x112) && id == 0x694) {
		return 8;
	}
	if ((type == 0xbc ||
			type == 0xbd ||
			type == 0x102 ||
			type == 0xc2 ||
			type == 0xc9 ||
			type == 0xbc6) && (id == 0x6ec ||
			id == 0x5a8)) {
		return 8;
	}
	if (type == 0x103 || type == 0xbc7) {
		if (id == 0x6f4) {
			return 0x10;
		}
		if (id == 0x6f3 ||
			id == 0x69f ||
			id == 0x6f6 ||
			id == 0x6ef ||
			id == 0x6f1 ||
			id == 0x6f0 ||
			id == 0x6f2 ||
			id == 0x733 ||
			id == 0x735 ||
			id == 0x732 ||
			id == 0x3ea) {
			return 8;
		}
		if (id == 0x73c ||
			id == 0x73d ||
			id == 0x73e ||
			id == 0x73f ||
			id == 0x740) {
			return 0x10;
		}
		if (Is_Quick_Control_B(id)) {
			return 0x20;
		}
	}
	if (type == 0x108 || type == 0xbc6) {
		if (id == 0x6d2 ||
			id == 0x3ea ||
			id == 0x733 ||
			id == 0x71b ||
			id == 0x732 ||
			id == 0x69d ||
			id == 0x69f ||
			id == 0x69e ||
			id == 0x6a0 ||
			id == 0x78b ||
			id == 0x5c8 ||
			id == 0x411 ||
			id == 0x412 ||
			id == 0x413 ||
			id == 0x414 ||
			id == 0x415 ||
			id == 0x416 ||
			id == 0x417 ||
			id == 0x418) {
			return 0x20;
		}
		if (Is_Quick_Control_D(id)) {
			return 0x40;
		}
	}
	if (Is_Owner_Draw_Message(type) && id == 0x695) {
		return 0x10;
	}
	return 8;
}

// FUNCTION: GAMEMD 0x006023F0
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

inline bool Is_Flag_Control_A(int type, int id)
{
	return (type == 0x10c ||
		type == 0x94 ||
		type == 0xa3 ||
		type == 0x73 ||
		type == 0xd8 ||
		type == 0xbbb ||
		type == 0xf5 ||
		type == 0x105 ||
		type == 0xe2 ||
		type == 0xd5 ||
		type == 0x2b5 ||
		type == 0xb7 ||
		type == 0x2b4 ||
		type == 0xbc7 ||
		type == 0x101 ||
		type == 0x129 ||
		type == 0xbc6 ||
		type == 0xd7 ||
		type == 0x108 ||
		type == 0xbc ||
		type == 0xbd ||
		type == 0xbb ||
		type == 0xb5 ||
		type == 0xbba ||
		type == 0xff ||
		type == 0x6b ||
		type == 0x100 ||
		type == 0x102 ||
		type == 0xb8 ||
		type == 0xd6 ||
		type == 0x103 ||
		FUN_00602AE0(type)) && id == 0x694;
}

inline bool Is_Flag_Control_B(int type, int id)
{
	return (type == 0xbc ||
			type == 0xbd ||
			type == 0x102 ||
			type == 0xc2 ||
			type == 0xc9 ||
			type == 0xbc6) && (id == 0x6ec ||
			id == 0x5a8);
}

inline bool Is_Flag_Control_C(int type, int id)
{
	return (type == 0x103 || type == 0xbc7) && (id == 0x6f3 ||
			id == 0x6f4 ||
			id == 0x69f ||
			id == 0x6f6 ||
			id == 0x6ef ||
			id == 0x6f1 ||
			id == 0x6f0 ||
			id == 0x6f2 ||
			id == 0x733 ||
			id == 0x735 ||
			id == 0x732 ||
			id == 0x3ea ||
			id == 0x736 ||
			id == 0x737 ||
			id == 0x73c ||
			id == 0x73d ||
			id == 0x73e ||
			id == 0x73f ||
			id == 0x740 ||
			id == 0x411 ||
			id == 0x412 ||
			id == 0x419 ||
			id == 0x41a ||
			id == 0x6a3 ||
			id == 0x6a4 ||
			id == 0x6a5 ||
			id == 0x6a7 ||
			id == 0x6a6 ||
			id == 0x6a8 ||
			id == 0x73a ||
			id == 0x739 ||
			id == 0x73b ||
			id == 0x738);
}

inline bool Is_Flag_Control_D(int type, int id)
{
	return (type == 0x108 || type == 0xbc6) && (id == 0x6d2 ||
			id == 0x3ea ||
			id == 0x69d ||
			id == 0x69f ||
			id == 0x69e ||
			id == 0x6a0 ||
			id == 0x78b ||
			id == 0x5c8 ||
			id == 0x411 ||
			id == 0x412 ||
			id == 0x413 ||
			id == 0x414 ||
			id == 0x415 ||
			id == 0x416 ||
			id == 0x417 ||
			id == 0x418 ||
			id == 0x419 ||
			id == 0x41a ||
			id == 0x41b ||
			id == 0x41c ||
			id == 0x41d ||
			id == 0x41e ||
			id == 0x41f ||
			id == 0x420 ||
			id == 0x6a3 ||
			id == 0x6a4 ||
			id == 0x6a9 ||
			id == 0x6ac ||
			id == 0x6af ||
			id == 0x6b2 ||
			id == 0x6b5 ||
			id == 0x6b8 ||
			id == 0x6a5 ||
			id == 0x6a7 ||
			id == 0x6aa ||
			id == 0x6ad ||
			id == 0x6b0 ||
			id == 0x6b3 ||
			id == 0x6b6 ||
			id == 0x6b9 ||
			id == 0x6a6 ||
			id == 0x6a8 ||
			id == 0x6ab ||
			id == 0x6ae ||
			id == 0x6b1 ||
			id == 0x6b4 ||
			id == 0x6b7 ||
			id == 0x6ba ||
			id == 0x70a ||
			id == 0x70b ||
			id == 0x70c ||
			id == 0x70d ||
			id == 0x70e ||
			id == 0x70f ||
			id == 0x710 ||
			id == 0x711 ||
			id == 0x733 ||
			id == 0x732 ||
			id == 0x71b ||
			id == 0x735);
}

inline bool Is_Flag_Control_E(int type, int id)
{
	return Is_Owner_Draw_Message(type) && id == 0x695 && !FUN_0069BBE0(&Session);
}

// FUNCTION: GAMEMD 0x00602490
bool FUN_00602490(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if (Is_Flag_Control_A(type, id)) {
		return true;
	}
	if (Is_Flag_Control_B(type, id)) {
		return true;
	}
	if (Is_Flag_Control_C(type, id)) {
		return true;
	}
	if (Is_Flag_Control_D(type, id)) {
		return true;
	}
	if (Is_Flag_Control_E(type, id)) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00602AE0
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

// FUNCTION: GAMEMD 0x00602B90
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

// FUNCTION: GAMEMD 0x00602D70
bool Is_Unused_Control(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;

	OwnerDraw::Dialogs.getPointer(dialog, &data);
	GetDlgCtrlID(control);
	return false;
}

// FUNCTION: GAMEMD 0x00602DD0
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

// FUNCTION: GAMEMD 0x00602FE0
int Control_Slot_Count(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;

	OwnerDraw::Dialogs.getPointer(dialog, &data);
	GetDlgCtrlID(control);
	return 12;
}

// FUNCTION: GAMEMD 0x00603040
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

// FUNCTION: GAMEMD 0x00603240
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

// FUNCTION: GAMEMD 0x006033F0
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

int FUN_0072E280(void);
void FUN_0072E2A0(void);
void FUN_0072DAC0(void);
void FUN_0072DAD0(void);
int FUN_0072D450(void);
void FUN_0072CC80(void);

// FUNCTION: GAMEMD 0x006035F0
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

int __fastcall FUN_004A38D0(int a, int b);
extern int DAT_00833674;
extern int DAT_00B0FB48;
extern int DAT_00B0FB10;
extern int DAT_00B0FB30;
extern int DAT_00B0FAD0;

// FUNCTION: GAMEMD 0x00603810
int FUN_00603810(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	return 0;
}

// FUNCTION: GAMEMD 0x00603870
int FUN_00603870(HWND dialog, HWND control, int arg)
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
			return FUN_004A38D0(DAT_00833674, arg);
		}
	}
	if (type == 0x94) {
		if (id == 0x6ea) {
			return DAT_00B0FB48;
		}
		if (id == 0x6ec) {
			return DAT_00B0FB10;
		}
	}
	if (type == 0x103) {
		goto typed;
	}
	if (type == 0xbc7) {
typed:
		if (id == 0x72b) {
			return DAT_00B0FB30;
		}
		goto done;
	}
	if (type == 0xc4) {
		if (id == 0x7a9) {
			return DAT_00B0FAD0;
		}
	}
done:
	return 0;
}

// FUNCTION: GAMEMD 0x00603AA0
int FUN_00603AA0(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	HWND key = dialog;
	int type;

	if (!OwnerDraw::Dialogs.getPointer(key, &data) || data == NULL) {
		return 0;
	}
	type = *(int *)((char *)data + 0x6c);
	if (!(type == 0x10d ||
		type == 0xd9 ||
		type == 0xf0 ||
		type == 0xce ||
		type == 0x120 ||
		type == 0x121 ||
		type == 0x115 ||
		type == 0xd3 ||
		type == 0xcf ||
		type == 0x11f ||
		type == 0xc3 ||
		type == 0x11b ||
		type == 0xe1 ||
		type == 0x11e ||
		type == 0xc4 ||
		type == 0x130 ||
		type == 0xd0 ||
		type == 0xfc ||
		type == 0x126)) {
		return 0;
	}
	*(int *)((char *)data + 0xb0) = 2;

	key = control;
	char name[128];
	GetClassName(control, name, 128);
	if (strcmp(name, "Edit") != 0 && strcmp(name, "NewEdit") != 0) {
		bool owner_drawn;
		if (strcmp(name, "ListBox") == 0) {
			OwnerDraw::WinData *list_data = NULL;
			if (OwnerDraw::Dialogs.getPointer(key, &list_data)) {
				owner_drawn = *(int *)((char *)list_data + 0x68) == 1;
				goto decided;
			}
		}
		if (strcmp(name, "ComboBox") != 0) {
			return 0;
		}
		owner_drawn = (GetWindowLong(key, GWL_STYLE) & 3) != 3;
decided:
		if (!owner_drawn) {
			return 0;
		}
	}
	return 0xc0;
}

// FUNCTION: GAMEMD 0x00603D30
void FUN_00603D30(HWND dialog, int value)
{
	OwnerDraw::WinData *data = NULL;
	HWND key = dialog;

	if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL) {
		*(int *)((char *)data + 0x70) = 2;
		*(int *)((char *)data + 0x14) = value;
		InvalidateRect(dialog, NULL, TRUE);
	}
}

// FUNCTION: GAMEMD 0x00603DB0
LRESULT FUN_00603DB0(HWND window, int value)
{
	return SendMessage(window, 0x4e5, 0, value & 0xff);
}

// FUNCTION: GAMEMD 0x00603DD0
LRESULT FUN_00603DD0(HWND window, int value)
{
	return SendMessage(window, 0x4e6, 0, value & 0xff);
}

// FUNCTION: GAMEMD 0x00603DF0
bool FUN_00603DF0(HWND window)
{
	return SendMessage(window, 0x4e7, 0, 0) ? true : false;
}

// FUNCTION: GAMEMD 0x00603E10
LRESULT FUN_00603E10(HWND window, int low, int high)
{
	return SendMessage(window, 0x4e8, 0, MAKELONG(low, high));
}

// FUNCTION: GAMEMD 0x00603E40
void FUN_00603E40(HWND dialog, int value)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		*(int *)((char *)data + 0xc0) = value;
	}
}

// FUNCTION: GAMEMD 0x00603EA0
int FUN_00603EA0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return *(int *)((char *)data + 0xc0);
	}
	return -1;
}

struct WideTextRequest {
	int first;
	int second;
	WideWstring text;
};

// FUNCTION: GAMEMD 0x00603F00
WideWstring FUN_00603F00(HWND window, int first, int second)
{
	WideTextRequest request;
	request.first = first;
	request.second = second;
	SendMessage(window, 0x4e9, 0, (LPARAM)&request);
	return request.text;
}

// FUNCTION: GAMEMD 0x00603F60
WideWstring FUN_00603F60(HWND window, int first)
{
	WideTextRequest request;
	request.first = first;
	request.second = -1;
	SendMessage(window, 0x4e9, 0, (LPARAM)&request);
	return request.text;
}

// FUNCTION: GAMEMD 0x00603FC0
WideWstring FUN_00603FC0(HWND window, int first, WORD low, WORD high)
{
	int second = SendMessage((HWND)first, 0x4e8, 0, MAKELONG(low, high));
	WideWstring text = FUN_00603F00(window, first, second);
	return text;
}

// FUNCTION: GAMEMD 0x00604060
WideWstring FUN_00604060(int type, int id)
{
	char const * label = GetTooltipStringLabel(type, id);
	if (label != NULL) {
		return WideWstring(StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\ownrdraw.cpp", 0x7a5));
	}
	return WideWstring(DAT_00887734);
}

// FUNCTION: GAMEMD 0x00606740
void FUN_00606740(HWND dialog, bool value)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		*((char *)data + 0xbc) = value;
	}
}

// FUNCTION: GAMEMD 0x006067A0
char FUN_006067A0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return *((char *)data + 0xbc);
	}
	return false;
}

BOOL CALLBACK FUN_00606800(HWND window, LPARAM lparam);

// FUNCTION: GAMEMD 0x006071C0
BOOL FUN_006071C0(HWND window)
{
	return EnumChildWindows(window, FUN_00606800, 1);
}

// FUNCTION: GAMEMD 0x006071D0
BOOL FUN_006071D0(HWND window)
{
	return EnumChildWindows(window, FUN_00606800, 0);
}

struct Game { static HINSTANCE hInstance; static bool __cdecl CallBack(void); static bool __fastcall MainLoop(void); };
void __fastcall DrawCampaignMenuTransition(HWND window, bool reverse);

// FUNCTION: GAMEMD 0x00607FD0
void FUN_00607FD0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	HWND key = dialog;

	if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL) {
		if (*((char *)data + 0xbe)) {
			HWND item = GetDlgItem(dialog, 0x71a);
			if (item != NULL) {
				SendMessage(item, 0x4e2, 0, 0);
			}
			DrawCampaignMenuTransition(dialog, false);
			*((char *)data + 0xbe) = 0;
		}
	}
}

bool __fastcall FUN_0055CBF0(void);
void __fastcall FUN_005D4D50(void);
bool __fastcall FUN_0069BBE0(SessionClass *session);
bool __fastcall FUN_00602490(HWND parent, HWND child);
namespace OwnerDraw {
BOOL CALLBACK ClassifyLayoutBand(HWND window, LPARAM lparam);
BOOL CALLBACK ResetControlDrawModeAndTimerProc(HWND window, LPARAM lparam);
BOOL CALLBACK InitCompactDialogControlsProc(HWND window, LPARAM lparam);
}


inline OwnerDraw::WinData *Get_Data(HWND window)
{
	OwnerDraw::WinData *data = NULL;
	if (!OwnerDraw::Dialogs.getPointer(window, &data)) {
		return NULL;
	}
	return data;
}

// FUNCTION: GAMEMD 0x00608070
bool FUN_00608070(HWND dialog)
{
	if (FUN_0069BBE0(&Session)) {
		return false;
	}
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	bool armed = *((char *)data + 0xbd) && *(int *)((char *)data + 0xb0) == 1;
	bool visible = IsWindowVisible(dialog) != 0;
	if (!armed || !visible) {
		return false;
	}
	VocClass::PlayGlobal(Rule->GUIMoveOutSound, 1.0f, 0x2000);
	bool wasEnabled = IsWindowEnabled(dialog) != 0;
	EnableWindow(dialog, FALSE);
	EnumChildWindows(dialog, FUN_00606800, 1);
	data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	*((char *)data + 0xbe) = 1;
	InvalidateRect(dialog, NULL, FALSE);
	DWORD start = GetTickCount();
	do {
		FUN_005D4D50();
		if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
			Game::CallBack();
		} else if (!FUN_0055CBF0() && Game::MainLoop()) {
			break;
		}
		int done = *((char *)data + 0xbe) == 0;
		if ((int)(GetTickCount() - start) > 5000 || done) {
			break;
		}
	} while (true);
	EnumChildWindows(dialog, FUN_00606800, 0);
	EnableWindow(dialog, wasEnabled);
	return true;
}

// FUNCTION: GAMEMD 0x00608260
bool RunOpenAnimationIfNeeded(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (FUN_0069BBE0(&Session)) {
		return false;
	}
	bool armed;
	{
		HWND key = dialog;
		if (!OwnerDraw::Dialogs.getPointer(key, &data) || data == NULL) {
			return false;
		}
		armed = *((char *)data + 0xbd) && *(int *)((char *)data + 0xb0) == 1;
	}
	bool visible = IsWindowVisible(dialog) != 0;
	if (!armed || !visible) {
		return false;
	}
	VocClass::PlayGlobal(Rule->GUIMoveInSound, 1.0f, 0x2000);
	bool wasEnabled = IsWindowEnabled(dialog) != 0;
	EnableWindow(dialog, FALSE);
	EnumChildWindows(dialog, FUN_00606800, 1);
	DrawCampaignMenuTransition(dialog, true);
	EnumChildWindows(dialog, FUN_00606800, 0);
	EnableWindow(dialog, wasEnabled);
	InvalidateRect(dialog, NULL, FALSE);
	return true;
}

// FUNCTION: GAMEMD 0x00608380
void FUN_00608380(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		*((char *)data + 0xbd) = 1;
	}
}

// FUNCTION: GAMEMD 0x006083E0
void FUN_006083E0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		*((char *)data + 0xbd) = 0;
	}
}

// FUNCTION: GAMEMD 0x00608440
void FUN_00608440(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		*((char *)data + 0xd4) = 1;
	}
}

// FUNCTION: GAMEMD 0x006084A0
void FUN_006084A0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		*((char *)data + 0xd4) = 0;
	}
}

namespace Drawing { extern int RenderWidth; }
extern int DAT_007F5BE0;
extern int DAT_007F5BE4;
extern int DAT_007F5BE8;

inline int Get_Layout_Dialog_Type(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	HWND key = dialog;

	OwnerDraw::Dialogs.getPointer(key, &data);
	if (data != NULL) {
		return *(int *)((char *)data + 0x6c);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00608500
bool __fastcall FUN_00608500(HWND parent, HWND child, Rect *out)
{
	OwnerDraw::WinData *data = NULL;
	Rect *rect = NULL;
	int type;

	type = Get_Layout_Dialog_Type(parent);
	int id = GetDlgCtrlID(child);
	if (type == 0x94) {
		if (Drawing::RenderWidth >= DAT_007F5BE4) {
			if (id == 0x6ea) {
				rect = new Rect(0x8d, 0x3a, 0x15c, 0x88);
			} else if (id == 0x6ec) {
				rect = new Rect(0x8d, 0x16c, 0x15c, 0x88);
			} else if (id == 0x50f) {
				rect = new Rect(0xb1, 0x109, 0x110, 0x16);
			} else if (id == 0x71e) {
				rect = new Rect(0xb1, 0x123, 0x8d, 0x14);
			} else if (id == 0x670) {
				rect = new Rect(0x135, 0x123, 0x8d, 0x14);
			} else if (id == 0x7a7) {
				rect = new Rect(0xa, 0xc6, 0x262, 0x14);
			} else if (id == 0x7a8) {
				rect = new Rect(0xa, 0x1f9, 0x262, 0x14);
			}
		}
	} else if (type == 0x103) {
		if (id == 0x72b) {
			int side = Scen->PlayerSideIndex;
			if (Drawing::RenderWidth >= DAT_007F5BE4) {
				if (side == 0) {
					rect = new Rect(0x68, 0x8e, 0x1a8, 0xdc);
				} else if (side == 1) {
					rect = new Rect(0x67, 0x82, 0x1a8, 0xe6);
				} else {
					rect = new Rect(0x67, 0x82, 0x1a8, 0xe6);
				}
			} else {
				if (side == 0) {
					rect = new Rect(0x18, 0x8e, 0x1a8, 0xdc);
				} else if (side == 1) {
					rect = new Rect(0x17, 0x82, 0x1a8, 0xe6);
				} else {
					rect = new Rect(0x17, 0x82, 0x1a8, 0xe6);
				}
			}
		}
	} else if (type == 0xbc7) {
		if (id == 0x72b) {
			int side = Scen->PlayerSideIndex;
			if (Drawing::RenderWidth >= DAT_007F5BE4) {
				if (side == 0) {
					rect = new Rect(0x68, 0x8e, 0x1a8, 0xdc);
				} else if (side == 1) {
					rect = new Rect(0x67, 0x82, 0x1a8, 0xe6);
				} else {
					rect = new Rect(0x67, 0x82, 0x1a8, 0xe6);
				}
			} else {
				if (side == 0) {
					rect = new Rect(0x18, 0x8e, 0x1a8, 0xdc);
				} else if (side == 1) {
					rect = new Rect(0x17, 0x82, 0x1a8, 0xe6);
				} else {
					rect = new Rect(0x17, 0x82, 0x1a8, 0xe6);
				}
			}
		}
	} else if (type == 0xbc) {
		if (id == 0x72f) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x34, 0x4e, 0x18c, 0x8f);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x15);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x16);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x1ae, 0xbe, 0x4b, 0x14);
			}
		}
	} else if (type == 0xbd) {
		if (id == 0x72f) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x34, 0x4e, 0x18c, 0x8f);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x15);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x16);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x1ae, 0xbe, 0x4b, 0x14);
			}
		}
	} else if (type == 0xc2) {
		if (id == 0x72f) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x34, 0x4e, 0x18c, 0x8f);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x16);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x15);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x1ae, 0xbe, 0x4b, 0x14);
			}
		}
	} else if (type == 0xc9) {
		if (id == 0x72f) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x34, 0x4e, 0x18c, 0x8f);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x15);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x16);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == DAT_007F5BE8) {
				rect = new Rect(0x1ae, 0xbe, 0x4b, 0x14);
			}
		}
	}
	if (rect != NULL) {
		*out = *rect;
		delete rect;
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00608CD0
bool __fastcall FUN_00608CD0(HWND parent, HWND child)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(parent, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(child);
	if ((type == 0x10c ||
			type == 0x94 ||
			type == 0xa3 ||
			type == 0x73 ||
			type == 0xd8 ||
			type == 0xbbb ||
			type == 0xf5 ||
			type == 0x105 ||
			type == 0xe2 ||
			type == 0xd5 ||
			type == 0x2b5 ||
			type == 0xb7 ||
			type == 0x2b4 ||
			type == 0xbc7 ||
			type == 0x101 ||
			type == 0x129 ||
			type == 0xbc6 ||
			type == 0xd7 ||
			type == 0x108 ||
			type == 0xbc ||
			type == 0xbd ||
			type == 0xbb ||
			type == 0xb5 ||
			type == 0xbba ||
			type == 0xff ||
			type == 0x6b ||
			type == 0x100 ||
			type == 0x102 ||
			type == 0xb8 ||
			type == 0xd6 ||
			type == 0x103 ||
			type == 0x125 ||
			type == 0xe7 ||
			type == 0x116 ||
			type == 0xc2 ||
			type == 0xc9 ||
			type == 0x11d ||
			type == 0x11c ||
			type == 0x114 ||
			type == 0x2bc ||
			type == 0x113 ||
			type == 0x109 ||
			type == 0x10f ||
			type == 0x117 ||
			type == 0xe6 ||
			type == 0xf3 ||
			type == 0xf4 ||
			type == 0x10e ||
			type == 0x112) && id == 0x694) {
		return true;
	}
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
		return true;
	}
	if ((type == 0xbc ||
			type == 0xbd ||
			type == 0x102 ||
			type == 0xc2 ||
			type == 0xc9 ||
			type == 0xbc6 ||
			type == 0x105 ||
			type == 0x6b ||
			type == 0x113) && id == 0x468) {
		return true;
	}
	if (type == 0xe2) {
		return id == 0x686 ||
			id == 0x578 ||
			id == 0x55c ||
			id == 0x683 ||
			id == 0x55f ||
			id == 0x684;
	}
	if (type == 0x101) {
		return id == 0x68e ||
			id == 0x68d ||
			id == 0x68f;
	}
	if (type == 0x129) {
		return id == 0x745;
	}
	if (type == 0x100) {
		return id == 0x689 ||
			id == 0x688 ||
			id == 0x579;
	}
	if (type == 0x94) {
		return id == 0x40e;
	}
	if (type == 0xb7) {
		return id == 0x40f;
	}
	if (type == 0x2b4) {
		return id == 0x6c7;
	}
	if (type == 0x2b5) {
		return id == 0x6c8;
	}
	if (type == 0x105) {
		return id == 0x6c4 ||
			id == 0x6c2 ||
			id == 0x6c3 ||
			id == 0x6c5;
	}
	if (type == 0x102) {
		return id == 0x6ec ||
			id == 0x5aa ||
			id == 0x5a8 ||
			id == 0x617;
	}
	if (type == 0xb5) {
		return id == 0x522 ||
			id == 0x521 ||
			id == 0x51e ||
			id == 0x51f ||
			id == 0x520;
	}
	if (type == 0xbba) {
		return id == 0x522 ||
			id == 0x521;
	}
	if (type == 0xbbb) {
		return id == 0x52c ||
			id == 0x52d;
	}
	if (type == 0xd5) {
		return id == 0x5ce ||
			id == 0x5cd;
	}
	if (type == 0xfb) {
		return id == 0x6 ||
			id == 0x1;
	}
	if (type == 0xd8) {
		return id == 0x6cb;
	}
	if (type == 0xd7) {
		return id == 0x1;
	}
	if (type == 0xbc || type == 0xc2) {
		return id == 0x6ec ||
			id == 0x588 ||
			id == 0x5aa ||
			id == 0x5a8 ||
			id == 0x5c2;
	}
	if (type == 0xbd || type == 0xc9) {
		return id == 0x59f ||
			id == 0x6ec ||
			id == 0x5a8 ||
			id == 0x5c2;
	}
	if (type == 0xbb) {
		return id == 0x6cd ||
			id == 0x6cc ||
			id == 0x6ce;
	}
	if (type == 0xb6) {
		return id == 0x6c9 ||
			id == 0x712 ||
			id == 0x524;
	}
	if (type == 0x6b) {
		return id == 0x583 ||
			id == 0x6c5;
	}
	if (type == 0xbc6) {
		return id == 0x6d1 ||
			id == 0x6ec ||
			id == 0x5a8;
	}
	if (type == 0x10f || type == 0x11d) {
		return id == 0x539 ||
			id == 0x6ec ||
			id == 0x6eb;
	}
	if (type == 0x109) {
		return id == 0x1 ||
			id == 0x625 ||
			id == 0x53b;
	}
	if (type == 0x10e || type == 0x11c) {
		return id == 0x6e0 ||
			id == 0x6e1 ||
			id == 0x6e2 ||
			id == 0x6e4 ||
			id == 0x771 ||
			id == 0x55f;
	}
	if (type == 0x114) {
		return id == 0x687 ||
			id == 0x6ec ||
			id == 0x6eb ||
			id == 0x688 ||
			id == 0x7a3;
	}
	if (type == 0x113) {
		return id == 0x61f ||
			id == 0x688 ||
			id == 0x7a3 ||
			id == 0x7a6 ||
			id == 0x689 ||
			id == 0x62d ||
			id == 0x62b ||
			id == 0x53d ||
			id == 0x5c2;
	}
	if (type == 0x2bc) {
		return id == 0x1 ||
			id == 0x63a;
	}
	if (type == 0x116) {
		return id == 0x62b ||
			id == 0x62d ||
			id == 0x5c2 ||
			id == 0x702 ||
			id == 0x703;
	}
	if (type == 0xff) {
		return id == 0x521 ||
			id == 0x522;
	}
	if (type == 0x122) {
		return id == 0x1;
	}
	if (type == 0x112) {
		return id == 0x1;
	}
	if (type == 0xe7) {
		return id == 0x1 ||
			id == 0x576;
	}
	if (type == 0xfe) {
		return id == 0x1;
	}
	if (type == 0xe6 || type == 0xf3 || type == 0xf4) {
		return id == 0x1;
	}
	return false;
}

// FUNCTION: GAMEMD 0x006096A0
bool FUN_006096A0(HWND parent, HWND child)
{
	OwnerDraw::WinData *data = NULL;

	if ((GetWindowLong(child, GWL_STYLE) & 0xb) == 0xb) {
		HWND key = child;
		if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL && *(int *)((char *)data + 0x68) == 0) {
			return FUN_00608CD0(parent, child);
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609730
bool FUN_00609730(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if (type == 0xe2) {
		return id == 0x3ee;
	}
	if (type == 0x73 || type == 0x10c || type == 0x103) {
		return id == 0x423;
	}
	if (type == 0x108 || type == 0xbc7) {
		return id == 0x6d1;
	}
	if (type == 0x94 || type == 0xb6 || type == 0xa3 || type == 0xd8 || type == 0xbbb ||
		type == 0xf5 || type == 0xd5 || type == 0x2b5 || type == 0xb7 || type == 0x2b4 ||
		type == 0x101 || type == 0x129 || type == 0xb5 || type == 0xbba || type == 0x100 ||
		type == 0xb8 || type == 0xd6 || type == 0x125 || type == 0x116 || type == 0x11d ||
		type == 0x11c || type == 0x113 || type == 0x109 || type == 0x10f || type == 0x114 ||
		type == 0x10e) {
		return id == 0x686;
	}
	if (type == 0x102 || type == 0x105 || type == 0x6b || type == 0xbb || type == 0x117 || type == 0xbc6) {
		return id == 0x5c0;
	}
	if (type == 0xbc || type == 0xbd || type == 0xea || type == 0xd7 || type == 0xc2 ||
		type == 0xc9 || type == 0x2bc || type == 0xe7 || type == 0xe6 || type == 0xf3 ||
		type == 0xf4 || type == 0x122 || type == 0x112 || type == 0xfe || type == 0xfc) {
		return id == 2;
	}
	if (type == 0xd4) {
		return id == 1;
	}
	if (type == 0xfb) {
		return id == 7;
	}
	if (type == 0xff) {
		return id == 0x675;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609AA0
bool FUN_00609AA0(HWND parent, HWND child)
{
	OwnerDraw::WinData *data = NULL;

	if ((GetWindowLong(child, GWL_STYLE) & 0xb) == 0xb) {
		HWND key = child;
		if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL && *(int *)((char *)data + 0x68) == 0) {
			return FUN_00609730(parent, child);
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609B30
bool FUN_00609B30(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if (Is_Owner_Draw_Message(type) && id == 0x695) {
		return true;
	}
	return false;
}


inline bool Is_Plain_Control(HWND window)
{
	OwnerDraw::WinData *data = NULL;

	if ((GetWindowLong(window, GWL_STYLE) & 0xb) == 0xb) {
		HWND key = window;
		if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL && *(int *)((char *)data + 0x68) == 0) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609BC0
bool FUN_00609BC0(HWND parent, HWND child)
{
	if (FUN_0069BBE0(&Session)) {
		return false;
	}
	if (Is_Plain_Control(child) && FUN_00608CD0(parent, child)) {
		return true;
	}
	if (Is_Plain_Control(child) && FUN_00609730(parent, child)) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609CF0
bool FUN_00609CF0(HWND parent, HWND child)
{
	if (!FUN_0069BBE0(&Session)) {
		return false;
	}
	if (Is_Plain_Control(child) && FUN_00608CD0(parent, child)) {
		return true;
	}
	if (Is_Plain_Control(child) && FUN_00609730(parent, child)) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609E20
bool FUN_00609E20(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if (type == 0x10d) {
		return id == 0x5c1 || id == 0x5c0;
	}
	if (type == 0xd9) {
		return id == 0x5c1 || id == 0x6ca;
	}
	if (type == 0xa3) {
		return id == 0x4cf || id == 0x4d0;
	}
	if (type == 0x105) {
		return id == 0x620 || id == 0x621;
	}
	if (type == 0xf0) {
		return id == 0x5c0;
	}
	if (type == 0xce) {
		return id == 0x5ae;
	}
	if (type == 0x120) {
		return id == 0x5ae || id == 2;
	}
	if (type == 0x121) {
		return id == 0x5ae || id == 0x5af || id == 2;
	}
	if (type == 0xea) {
		return id == 0x640 || id == 0x642 || id == 0x641 || id == 0x643 || id == 0x644 || id == 0x645 || id == 0x646 || id == 0x647;
	}
	if (type == 0xb8) {
		return id == 0x531 || id == 0x535;
	}
	if (type == 0x11f) {
		return id == 0x5c0;
	}
	if (type == 0x115) {
		return id == 0x702 || id == 0x686;
	}
	if (type == 0xd3) {
		return id == 2;
	}
	if (type == 0xcf) {
		return id == 0x5c2 || id == 2;
	}
	if (type == 0xc3) {
		return id == 1 || id == 2;
	}
	if (type == 0x11b) {
		return id == 0x5c2 || id == 0x686;
	}
	if (type == 0xe1) {
		return id == 1 || id == 2;
	}
	if (type == 0x11e) {
		return id == 0x5c2 || id == 0x686;
	}
	if (type == 0xc4) {
		return id == 0x58d || id == 0x58e;
	}
	if (type == 0xd0) {
		return id == 1;
	}
	if (type == 0xfc) {
		return id == 1 || id == 2;
	}
	if (type == 0x126) {
		return id == 6 || id == 7;
	}
	return false;
}

// FUNCTION: GAMEMD 0x0060A100
bool FUN_0060A100(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = *(int *)((char *)data + 0x6c);
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	if (type == 0xe2) {
		return id == 0x71d;
	}
	return false;
}

int DAT_00AC1CAC;
int DAT_00AC4894;

// FUNCTION: GAMEMD 0x0060A180
BOOL CALLBACK FUN_0060A180(HWND window, LPARAM lparam)
{
	HWND parent = GetParent(window);
	OwnerDraw::WinData *data = NULL;

	if ((GetWindowLong(window, GWL_STYLE) & 0xb) == 0xb) {
		HWND key = window;
		if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL && *(int *)((char *)data + 0x68) == 0) {
			if (FUN_00608CD0(parent, window) && IsWindowVisible(window)) {
				DAT_00AC1CAC++;
			}
		}
	}
	return TRUE;
}

// FUNCTION: GAMEMD 0x0060A230
int __fastcall FUN_0060A230(HWND dialog)
{
	DAT_00AC1CAC = 0;
	EnumChildWindows(dialog, FUN_0060A180, 0);
	return DAT_00AC1CAC;
}

// FUNCTION: GAMEMD 0x0060A250
BOOL CALLBACK FUN_0060A250(HWND window, LPARAM lparam)
{
	HWND parent = GetParent(window);
	OwnerDraw::WinData *data = NULL;

	if ((GetWindowLong(window, GWL_STYLE) & 0xb) == 0xb) {
		HWND key = window;
		if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL && *(int *)((char *)data + 0x68) == 0) {
			if (FUN_00609730(parent, window) && IsWindowVisible(window)) {
				DAT_00AC4894++;
			}
		}
	}
	return TRUE;
}

// FUNCTION: GAMEMD 0x0060A300
int __fastcall FUN_0060A300(HWND dialog)
{
	DAT_00AC4894 = 0;
	EnumChildWindows(dialog, FUN_0060A250, 0);
	return DAT_00AC4894 >= 1;
}

// FUNCTION: GAMEMD 0x0060A5A0
void __fastcall FUN_0060A5A0(HWND dialog)
{
	EnumChildWindows(dialog, OwnerDraw::ClassifyLayoutBand, 0);
}

// FUNCTION: GAMEMD 0x0060AA50
void __fastcall FUN_0060AA50(HWND dialog)
{
	EnumChildWindows(dialog, OwnerDraw::ResetControlDrawModeAndTimerProc, 0);
}


namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060AA60
// OwnerDraw::SendTransitionCompleteToCustomTextChildProc
BOOL CALLBACK SendTransitionCompleteToCustomTextChildProc(HWND window, LPARAM lparam)
{
	HWND parent = GetParent(window);
	if (FUN_00602490(parent, window)) {
		SendMessage(window, 0x4ee, 0, 0);
	}
	return TRUE;
}
}

// FUNCTION: GAMEMD 0x0060AAA0
void __fastcall FUN_0060AAA0(HWND dialog)
{
	EnumChildWindows(dialog, OwnerDraw::SendTransitionCompleteToCustomTextChildProc, 0);
}

// FUNCTION: GAMEMD 0x0060AF40
void __fastcall FUN_0060AF40(HWND dialog)
{
	EnumChildWindows(dialog, OwnerDraw::InitCompactDialogControlsProc, 0);
}

extern int DAT_007F5BE4;
extern int DAT_007F5BF0;

inline int Centre_X(int span, bool compact)
{
	int offset = 0;
	if (!compact) {
		int half = (span - DAT_007F5BE4) / 2;
		offset = (((span - DAT_007F5BE4) / 2) < 0) ? 0 : half;
	}
	return offset;
}

inline int Centre_Y(int span, bool compact)
{
	int offset = 0;
	if (!compact) {
		int half = (span - DAT_007F5BF0) / 2;
		offset = (((span - DAT_007F5BF0) / 2) < 0) ? 0 : half;
	}
	return offset;
}

// FUNCTION: GAMEMD 0x0060AF50
void __fastcall FUN_0060AF50(HWND window, RECT *rect)
{
	HWND parent = GetParent(window);
	bool compact = FUN_0069BBE0(&Session);
	RECT frame;
	GetWindowRect(parent, &frame);
	int dx = Centre_X(frame.right - frame.left, compact);
	int tmp0 = Centre_Y(frame.bottom - frame.top, compact);
	MoveWindow(window, rect->left + dx, rect->top + tmp0, rect->right, rect->bottom, FALSE);
	InvalidateRect(window, NULL, FALSE);
}

extern Rect *DAT_00B0FC28;
extern Rect *DAT_00B0FC50;
extern int DAT_007F5BF8;

// FUNCTION: GAMEMD 0x0060B420
void __fastcall FUN_0060B420(HWND window)
{
	HWND parent = GetParent(window);
	bool compact = FUN_0069BBE0(&Session);
	RECT client;
	RECT own;
	GetClientRect(parent, &client);
	GetWindowRect(window, &own);
	int span = client.right - client.left;
	int dx = 0;
	if (!compact) {
		int hh = (span - DAT_007F5BE4) / 2;
		dx = (((span - DAT_007F5BE4) / 2) < 0) ? 0 : hh;
	}
	int w = own.right - own.left;
	int h = own.bottom - own.top;
	OwnerDraw::WinData *data = Get_Data(window);
	int half;
	if (data != NULL && *(int *)((char *)data + 0xdc) != 0) {
		half = *(int *)((char *)data + 0xdc);
	} else {
		half = (DAT_007F5BF8 - w) / 2;
	}
	int x = client.right - half - w - dx - client.left;
	MoveWindow(window, x, compact ? DAT_00B0FC50->Y - h : DAT_00B0FC28->Y - h, w, h, FALSE);
}

// FUNCTION: GAMEMD 0x0060B550
void __fastcall FUN_0060B550(HWND window)
{
	HWND parent = GetParent(window);
	bool compact = FUN_0069BBE0(&Session);
	RECT client;
	RECT own;
	GetClientRect(parent, &client);
	GetWindowRect(window, &own);
	int span = client.right - client.left;
	int dx = 0;
	if (!compact) {
		int hh = (span - DAT_007F5BE4) / 2;
		dx = (((span - DAT_007F5BE4) / 2) < 0) ? 0 : hh;
	}
	int spanY = client.bottom - client.top;
	int dy = 0;
	if (!compact) {
		int hv = (spanY - DAT_007F5BF0) / 2;
		dy = (((spanY - DAT_007F5BF0) / 2) < 0) ? 0 : hv;
	}
	int h = own.bottom - own.top;
	int w = own.right - own.left;
	MoveWindow(window, dx + client.left + 10, client.bottom - h - dy - 1, w, h, FALSE);
}

// FUNCTION: GAMEMD 0x0060B610
void __fastcall FUN_0060B610(HWND window)
{
	HWND parent = GetParent(window);
	bool compact = FUN_0069BBE0(&Session);
	RECT client;
	RECT own;
	GetClientRect(parent, &client);
	GetWindowRect(window, &own);
	int span = client.right - client.left;
	int dx = 0;
	if (!compact) {
		int hh = (span - DAT_007F5BE4) / 2;
		dx = (((span - DAT_007F5BE4) / 2) < 0) ? 0 : hh;
	}
	int w = own.right - own.left;
	int h = own.bottom - own.top;
	OwnerDraw::WinData *data = Get_Data(window);
	int half;
	if (data != NULL && *(int *)((char *)data + 0xdc) != 0) {
		half = *(int *)((char *)data + 0xdc);
	} else {
		half = (DAT_007F5BF8 - w) / 2;
	}
	int x = client.right - half - w - dx - client.left;
	MoveWindow(window, x, compact ? DAT_00B0FC50->Height + DAT_00B0FC50->Y - h : DAT_00B0FC28->Height + DAT_00B0FC28->Y - h, w, h, FALSE);
}

namespace OwnerDraw {
extern HWND CurrentDialogHwnd;
}
BOOL CALLBACK FUN_0060C0C0(HWND window, LPARAM lparam);

// FUNCTION: GAMEMD 0x0060B740
void __fastcall FUN_0060B740(HWND window)
{
	HWND parent = GetParent(window);
	RECT frame;
	RECT own;
	GetWindowRect(parent, &frame);
	GetWindowRect(window, &own);
	MoveWindow(window, own.left - frame.left, own.top - frame.top, own.right - own.left, own.bottom - own.top, FALSE);
}

// FUNCTION: GAMEMD 0x0060B7A0
void __fastcall FUN_0060B7A0(HWND window)
{
	HWND parent = GetParent(window);
	if (FUN_0069BBE0(&Session)) {
		RECT own;
		RECT frame;
		GetWindowRect(window, &own);
		GetWindowRect(parent, &frame);
		int spanX = frame.right - frame.left;
		int dx = 0;
		if (spanX > DAT_007F5BE4) {
			int half = (spanX - DAT_007F5BE4) / 2;
			dx = (half < 0) ? 0 : half;
		} else if (spanX < DAT_007F5BE4) {
			dx = (spanX - DAT_007F5BE4) / 2;
		}
		int spanY = frame.bottom - frame.top;
		int dy = 0;
		if (spanY > DAT_007F5BF0) {
			int half = (spanY - DAT_007F5BF0) / 2;
			dy = (half < 0) ? 0 : half;
		} else if (spanY < DAT_007F5BF0) {
			dy = (spanY - DAT_007F5BF0) / 2;
		}
		int x = own.left + dx;
		int y = own.top + dy;
		int w = own.right - own.left;
		int h = own.bottom - own.top;
		if (x < 0) {
			x = 0;
		}
		if (y < 0) {
			y = 0;
		}
		MoveWindow(window, x, y, w, h, FALSE);
	}
}

// FUNCTION: GAMEMD 0x0060B880
bool __fastcall FUN_0060B880(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	int type = (data != NULL) ? *(int *)((char *)data + 0x6c) : 0;
	GetDlgCtrlID(control);
	if (type == 0xb7 || type == 0x2b4 || type == 0xbbb || type == 0xf5 || type == 0x2b5 || type == 0xb8 ||
		type == 0xa3 || type == 0xb6 || type == 0x10c || type == 0x73 || type == 0xff || type == 0xea) {
		return true;
	}
	return false;
}

void __fastcall FUN_0060B000(HWND window, LPARAM lparam);
void __fastcall FUN_0060B1D0(HWND window, LPARAM lparam);

namespace Drawing {
extern int RenderWidth;
extern int RenderHeight;
extern unsigned char RedShiftRight;
extern unsigned char GreenShiftRight;
extern unsigned char BlueShiftRight;
extern int RedShiftLeft;
extern int GreenShiftLeft;
extern int BlueShiftLeft;
}

void __fastcall FUN_0060B950(HWND window);

namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060C4A0
// OwnerDraw::UpdateControlPosition
void __fastcall UpdateControlPosition(HWND dialog, LPARAM lparam)
{
	MoveWindow(dialog, 0, 0, Drawing::RenderWidth, Drawing::RenderHeight, FALSE);
	OwnerDraw::CurrentDialogHwnd = dialog;
	EnumChildWindows(dialog, FUN_0060C0C0, lparam);
}
}

// FUNCTION: GAMEMD 0x0060C4E0
int __fastcall FUN_0060C4E0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return *(int *)((char *)data + 0x74);
	}
	return 0;
}

namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060C7D0
// OwnerDraw::TrySetDialogLayoutBand2
bool __fastcall TrySetDialogLayoutBand2(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = *(int *)((char *)data + 0x6c);
	bool match = type == 0x10d || type == 0xd9 || type == 0xf0 || type == 0xce || type == 0x120 || type == 0x121 ||
		type == 0x115 || type == 0xd3 || type == 0xcf || type == 0x11f || type == 0xc3 || type == 0x11b ||
		type == 0xe1 || type == 0x11e || type == 0xc4 || type == 0x130 || type == 0xd0 || type == 0xfc ||
		type == 0x126;
	if (match) {
		*(int *)((char *)data + 0xb0) = 2;
	}
	return match;
}
}

// FUNCTION: GAMEMD 0x0060C8D0
unsigned char __fastcall FUN_0060C8D0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return *((unsigned char *)data + 0xd6);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060C930
unsigned char __fastcall FUN_0060C930(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = *(int *)((char *)data + 0x6c);
	int v;
	if (type == 0xbc || type == 0xbd || type == 0x102 || type == 0xc2 || type == 0xc9 || type == 0xbc6) {
		v = 1;
	} else {
		v = 0;
	}
	*((unsigned char *)data + 214) = v;
	return v;
}

// FUNCTION: GAMEMD 0x0060C9D0
void __fastcall FUN_0060C9D0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		*((unsigned char *)data + 0xd6) = 1;
	}
}

// FUNCTION: GAMEMD 0x0060CA30
void __fastcall FUN_0060CA30(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		*((unsigned char *)data + 0xd6) = 0;
	}
}

// FUNCTION: GAMEMD 0x0060CA90
unsigned char __fastcall FUN_0060CA90(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return *((unsigned char *)data + 0xd5);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CAF0
unsigned char __fastcall FUN_0060CAF0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = *(int *)((char *)data + 0x6c);
	int v;
	if (type == 0xbc || type == 0xbd || type == 0x102 || type == 0xc2 || type == 0xc9 || type == 0xbc6 || type == 0x105 || type == 0x6b || type == 0x113) {
		v = 1;
	} else {
		v = 0;
	}
	*((unsigned char *)data + 213) = v;
	return v;
}

// FUNCTION: GAMEMD 0x0060CBA0
void __fastcall FUN_0060CBA0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		*((unsigned char *)data + 0xd5) = 1;
	}
}

// FUNCTION: GAMEMD 0x0060CC00
void __fastcall FUN_0060CC00(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		*((unsigned char *)data + 0xd5) = 0;
	}
}

// FUNCTION: GAMEMD 0x0060CC60
unsigned char __fastcall FUN_0060CC60(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return *((unsigned char *)data + 0xd7);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CCC0
unsigned char __fastcall FUN_0060CCC0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = *(int *)((char *)data + 0x6c);
	int v;
	if (type == 0x103 || type == 0xbc7) {
		v = 1;
	} else {
		v = 0;
	}
	*((unsigned char *)data + 215) = v;
	return v;
}

// FUNCTION: GAMEMD 0x0060CD50
unsigned char __fastcall FUN_0060CD50(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return *((unsigned char *)data + 0xd8);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CDB0
unsigned char __fastcall FUN_0060CDB0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = *(int *)((char *)data + 0x6c);
	int v;
	if (type == 0x108 || type == 0xbc6) {
		v = 1;
	} else {
		v = 0;
	}
	*((unsigned char *)data + 216) = v;
	return v;
}

// FUNCTION: GAMEMD 0x0060CE40
int __fastcall FUN_0060CE40(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return *(int *)((char *)data + 0xe0);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CEA0
int __fastcall FUN_0060CEA0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return *(int *)((char *)data + 0xe4);
	}
	return 0;
}

int __cdecl FUN_0072B9D0(void);
int __cdecl FUN_0072BBB0(void);
int __cdecl FUN_0072BD90(void);
int __cdecl FUN_0072BF70(void);
int __cdecl FUN_0072C150(void);
int __cdecl FUN_0072C330(void);
int __cdecl FUN_0072C510(void);
int __cdecl FUN_0072C6F0(void);
int __cdecl FUN_0072C8D0(void);
int __cdecl FUN_0072CAB0(void);
int __cdecl FUN_0072CE50(void);
int __cdecl FUN_0072D030(void);
int __cdecl FUN_0072D210(void);
int __cdecl FUN_0072D820(void);
int __cdecl FUN_0072DAE0(void);
extern int DAT_00B0F9E8;
extern int DAT_00B0FA04;
extern int DAT_00B0FA18;
extern int DAT_00B0FA30;
extern int DAT_00B0FA4C;
extern int DAT_00B0FA60;
extern int DAT_00B0FA64;
extern int DAT_00B0FA6C;
extern int DAT_00B0FA98;
extern int DAT_00B0FA9C;
extern int DAT_00B0FAAC;
extern int DAT_00B0FAB8;
extern int DAT_00B0FB0C;
extern int DAT_00B0FB1C;
extern int DAT_00B0FB2C;
extern int DAT_00B0FB34;
extern int DAT_00B0FB50;
extern int DAT_00B0FB54;

// FUNCTION: GAMEMD 0x0060CF00
void __fastcall FUN_0060CF00(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return;
	}
	char *p = (char *)data;
	int type = *(int *)(p + 0x6c);
	if (type == 0x94) {
		*(int *)(p + 0x74) = FUN_0072DAE0();
		*(int *)(p + 0xe0) = DAT_00B0FA6C;
		*(int *)(p + 0xe4) = DAT_00B0FA6C;
	} else if (type == 0x103) {
		*(int *)(p + 0x74) = FUN_0072D450();
		*(int *)(p + 0xe0) = DAT_00B0FB34;
		*(int *)(p + 0xe4) = DAT_00B0FB34;
	} else if (type == 0x108) {
		*(int *)(p + 0x74) = FUN_0072D820();
		*(int *)(p + 0xe0) = DAT_00B0FB1C;
		*(int *)(p + 0xe4) = DAT_00B0FB1C;
	} else if (type == 0xbc6) {
		*(int *)(p + 0x74) = FUN_0072D450();
		*(int *)(p + 0xe0) = DAT_00B0FB34;
		*(int *)(p + 0xe4) = DAT_00B0FB34;
	} else if (type == 0xbc7) {
		*(int *)(p + 0x74) = FUN_0072D450();
		*(int *)(p + 0xe0) = DAT_00B0FB34;
		*(int *)(p + 0xe4) = DAT_00B0FB34;
	} else if (type == 0x6b) {
		*(int *)(p + 0x74) = FUN_0072D210();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FAB8;
	} else if (type == 0x102 || type == 0xbc || type == 0xbd || type == 0xc2 || type == 0xc9) {
		*(int *)(p + 0x74) = FUN_0072D030();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA18;
	} else if (type == 0x113) {
		*(int *)(p + 0x74) = FUN_0072CE50();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FAAC;
	} else if (type == 0x114) {
		*(int *)(p + 0x74) = FUN_0072CAB0();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA60;
	} else if (type == 0x10e || type == 0x11c) {
		*(int *)(p + 0x74) = FUN_0072C8D0();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0F9E8;
	} else if (type == 0x10f || type == 0x11d) {
		*(int *)(p + 0x74) = FUN_0072C6F0();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FB0C;
	} else if (type == 0xe6 || type == 0xf3 || type == 0xf4) {
		*(int *)(p + 0x74) = FUN_0072C510();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA64;
	} else if (type == 0xe7) {
		*(int *)(p + 0x74) = FUN_0072C330();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA30;
	} else if (type == 0x116) {
		*(int *)(p + 0x74) = FUN_0072C150();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA4C;
	} else if (type == 0x117) {
		*(int *)(p + 0x74) = FUN_0072BF70();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA9C;
	} else if (type == 0x112) {
		*(int *)(p + 0x74) = FUN_0072BD90();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA98;
	} else if (type == 0x2bc) {
		*(int *)(p + 0x74) = FUN_0072BBB0();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FB2C;
	} else if (type == 0xd6) {
		*(int *)(p + 0x74) = FUN_0072B9D0();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FB54;
	} else {
		*(int *)(p + 0x74) = FUN_0072E280();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA04;
	}
}

// FUNCTION: GAMEMD 0x0060D2C0
void __fastcall FUN_0060D2C0(HWND dialog, int id)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		*(int *)((char *)data + 0x6c) = id;
	}
}

// FUNCTION: GAMEMD 0x0060D320
int __fastcall FUN_0060D320(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return *(int *)((char *)data + 0x6c);
	}
	return 0;
}

void __fastcall FUN_0052B9B0(HWND dialog);
namespace UI {
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall EndDialog(HWND window);
}
class InputManagerClass {
	public:
		void DoSomething(void);
		static InputManagerClass * Instance;
};

// FUNCTION: GAMEMD 0x0060D380
int __fastcall FUN_0060D380(int resource, DLGPROC proc, bool focus)
{
	int result;
	HWND dialog = UI::BeginDialog(resource, proc, 0);
	result = 0;
	if (dialog != NULL) {
		SetWindowLong(dialog, 8, (LONG)&result);
		ShowWindow(dialog, 1);
		SetForegroundWindow(dialog);
		InputManagerClass::Instance->DoSomething();
		if (focus) {
			FUN_0052B9B0(dialog);
		}
		while (result == 0) {
			FUN_005D4D50();
			if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
				Game::CallBack();
			} else if (!FUN_0055CBF0() && Game::MainLoop()) {
				break;
			}
		}
		UI::EndDialog(dialog);
	}
	return result;
}

// FUNCTION: GAMEMD 0x0060D430
void FUN_0060D430(void)
{
}

// FUNCTION: GAMEMD 0x0060D440
void FUN_0060D440(void)
{
}

extern int DAT_00AC48CC;
LRESULT CALLBACK FUN_0060D540(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK FUN_0060D520(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
namespace UI { void RegisterComboDropAndNewEditClasses(void); }

// FUNCTION: GAMEMD 0x0060D450
// UI::RegisterComboDropAndNewEditClasses
void UI::RegisterComboDropAndNewEditClasses(void)
{
	if (DAT_00AC48CC == 1) {
		return;
	}
	DAT_00AC48CC = 1;
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = FUN_0060D540;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = Game::hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = "ComboDropWin";
	wc.lpszClassName = "ComboDropWin";
	RegisterClass(&wc);
	memset(&wc, 0, sizeof(wc));
	wc.style = 0x83;
	wc.lpfnWndProc = FUN_0060D520;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = Game::hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "NewEdit";
	RegisterClass(&wc);
}

// FUNCTION: GAMEMD 0x0060D520
LRESULT CALLBACK FUN_0060D520(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(window, message, wparam, lparam);
}

struct NewEditLink {
	const char *className;
	LONG style;
};

// FUNCTION: GAMEMD 0x0060F320
BOOL CALLBACK FUN_0060F320(HWND edit, LPARAM lparam)
{
	LONG style = GetWindowLong(edit, GWL_STYLE);
	SetWindowLong(edit, GWL_USERDATA, 0);
	if ((style & WS_VISIBLE) && !(style & WS_DISABLED)) {
		char name[128];
		GetClassName(edit, name, sizeof(name));
		if (strcmp(name, "Edit") == 0) {
			HWND parent = GetParent(edit);
			RECT rect;
			RECT parent_rect;
			Get_Display_Rect(edit, &rect);
			Get_Display_Rect(parent, &parent_rect);
			int x = rect.left - parent_rect.left;
			int y = rect.top - parent_rect.top;
			DWORD flags = 0x40000110;
			if (style & WS_TABSTOP) {
				flags = 0x40010110;
			}
			HWND list = CreateWindowEx(0, "ListBox", NULL, flags, x, y, rect.right - rect.left, rect.bottom - rect.top, parent, NULL, Game::hInstance, NULL);
			NewEditLink *link = new NewEditLink;
			link->className = "NewEdit";
			link->style = GetWindowLong(edit, GWL_STYLE);
			SetWindowLong(list, GWL_USERDATA, (LONG)link);
			EnableWindow(edit, FALSE);
			ShowWindow(edit, SW_HIDE);
			EnableWindow(list, TRUE);
			ShowWindow(list, SW_SHOWNORMAL);
			LONG id = GetWindowLong(edit, GWL_ID);
			SetWindowLong(edit, GWL_ID, GetWindowLong(list, GWL_ID));
			SetWindowLong(list, GWL_ID, id);
		}
	}
	return TRUE;
}

extern DSurface * DAT_00887308;
extern int DAT_00AC48D8;
void __fastcall FUN_006219D0(Rect const & rect, Surface & surface, int xpos, int ypos);
namespace OwnerDraw {
int __fastcall PrintTextFixedLength(unsigned color, BitFont * font, Rect const & rect, wchar_t const * text, int length, int x_alignment, int y_alignment, Surface * surface, int unknown);
void DrawBeveledBorder(Surface & surface, Rect const & rect, int offset, int color);
}

void __fastcall FUN_0053E420(HWND window);
BOOL CALLBACK FUN_00610570(HWND window, LPARAM lparam);

static inline char Remove_Entry(Dictionary<HWND, WNDPROC> & dict, HWND & window)
{
	WNDPROC proc;
	return(dict.remove(window, proc));
}

static inline char Remove_Entry(Dictionary<HWND, OwnerDraw::WinData> & dict, HWND & window)
{
	OwnerDraw::WinData data;
	return(dict.remove(window, data));
}

static inline BOOL RemoveFromDicts(HWND window)
{
	Remove_Entry(OwnerDraw::DialogProcs, window);
	Remove_Entry(OwnerDraw::Dialogs, window);
	Remove_Entry(OwnerDraw::SubclassProcs, window);

	void *userdata = (void *)GetWindowLong(window, GWL_USERDATA);
	if (userdata != NULL) {
		delete userdata;
	}
	SetWindowLong(window, GWL_USERDATA, 0);
	FUN_0053E420(window);
	return TRUE;
}

// FUNCTION: GAMEMD 0x00610360
BOOL __fastcall FUN_00610360(HWND window)
{
	EnumChildWindows(window, FUN_00610570, 0);
	RemoveFromDicts(window);
	return TRUE;
}

union ODColorBits {
	struct {
		unsigned int red : 8;
		unsigned int green : 8;
		unsigned int blue : 8;
		unsigned int a : 8;
	};
	int v;
};

// SYNTHETIC: GAMEMD 0x006107A0 SYMBOL
// _$E117
OwnerDraw::Tooltip ODTooltip;

// FUNCTION: GAMEMD 0x006107E0
OwnerDraw::Tooltip::Tooltip(void)
{
	bounds.Set(0, 0, 0, 0);
	background = NULL;
	text[0] = L'\0';
	isActive = false;
	isHidden = false;
	window = (HWND)NULL;
}

// FUNCTION: GAMEMD 0x00610810
bool OwnerDraw::Start_Tooltip(Rect const & rect, wchar_t const * text, HWND window)
{
	OwnerDraw::End_Tooltip();

	DAT_00AC48D8 = time(NULL);

	swprintf(ODTooltip.text, StringTable::LoadString("GUI:ToolTip", NULL, "D:\\ra2mdpost\\ownrdraw.cpp", 0x1d21));

	if (text != NULL) {
		wcscpy(ODTooltip.text, text);
	}

	ODTooltip.bounds = rect;
	ODTooltip.isActive = true;
	ODTooltip.window = window;

	return OwnerDraw::DrawTooltip(true);
}

// FUNCTION: GAMEMD 0x00610950
// OwnerDraw::DrawTooltip
bool OwnerDraw::DrawTooltip(bool save_background)
{
	if (ODTooltip.isActive == false) {
		return false;
	}

	if (save_background) {
		if (ODTooltip.background != NULL) {
			delete ODTooltip.background;
		}
		ODTooltip.background = NULL;

		Surface * backgd = new BSurface(ODTooltip.bounds.Width, ODTooltip.bounds.Height, 2);
		ODTooltip.background = backgd;

		Rect drect(0, 0, ODTooltip.bounds.Width, ODTooltip.bounds.Height);
		backgd->Blit_From(drect, *DAT_00887308, ODTooltip.bounds, false, true);
	}

	ODTooltip.isHidden = false;

	Rect rect = ODTooltip.bounds;

	int color;
	if (OwnerDraw::TooltipBackgroundColor == -1) {
		color = -1;
	} else {
		ODColorBits *c = (ODColorBits *)&OwnerDraw::TooltipBackgroundColor;
		color = DSurface::Build_Hicolor_Pixel(c->red, c->green, c->blue);
	}

	DAT_00887308->Lock();
	DAT_00887308->Fill_Rect(rect, color);
	DAT_00887308->Unlock();

	FUN_006219D0(rect, *DAT_00887308, 8, 4);

	OwnerDraw::PrintTextFixedLength(OwnerDraw::PrimaryTextColor, 0, rect, ODTooltip.text, wcslen(ODTooltip.text), 1, 1, DAT_00887308, 0);
	rect.X += 1;
	rect.Y += 1;
	rect.Width -= 2;
	rect.Height -= 2;
	OwnerDraw::DrawBeveledBorder(*DAT_00887308, rect, 1, -1);

	return true;
}

// FUNCTION: GAMEMD 0x00610B50
// OwnerDraw::RestoreTooltipBackground
bool OwnerDraw::RestoreTooltipBackground(void)
{
	if (ODTooltip.isActive == false) {
		return false;
	}

	if (ODTooltip.isHidden == (int)true) {
		return false;
	}

	if (ODTooltip.background == NULL) {
		return false;
	}

	Rect drect = ODTooltip.bounds;
	Rect srect(0, 0, ODTooltip.bounds.Width, ODTooltip.bounds.Height);
	DAT_00887308->Blit_From(drect, *ODTooltip.background, srect, false, true);
	ODTooltip.isHidden = true;
	return true;
}

// FUNCTION: GAMEMD 0x00610BF0
bool OwnerDraw::End_Tooltip(void)
{
	if (ODTooltip.isActive == false) {
		return false;
	}

	OwnerDraw::RestoreTooltipBackground();

	if (ODTooltip.background != NULL) {
		delete ODTooltip.background;
	}
	ODTooltip.background = NULL;

	ODTooltip.isActive = false;
	ODTooltip.isHidden = false;
	return true;
}

// SYNTHETIC: GAMEMD 0x00612910 SYMBOL
// _$E120
static ArrayList<HWND> ActiveWindows;

// SYNTHETIC: GAMEMD 0x00612930 SYMBOL
// _$E125
static Dictionary<OwnerDraw::CtrlMsgData, bool> ProcessedMessages(Hash_CtrlMsg);

// FUNCTION: GAMEMD 0x00612A60
// OwnerDraw::SysListViewHandler
LRESULT CALLBACK OwnerDraw::SysListViewHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	WNDPROC original = NULL;
	WNDPROC *found = NULL;
	OwnerDraw::WinData *data = NULL;
	RECT display;
	RECT client;

	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		original = *found;
	}
	Get_Display_Rect(window, &display);
	GetClientRect(window, &client);
	OwnerDraw::Dialogs.getPointer(window, &data);

	if (message == WM_CTLCOLOREDIT) {
		SetTextColor((HDC)wparam, OwnerDraw::PrimaryTextColor);
		SetBkMode((HDC)wparam, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}
	return CallWindowProc(original, window, message, wparam, lparam);
}

inline OwnerDraw::WinData *Find_Data(HWND window)
{
	OwnerDraw::WinData *data = NULL;
	OwnerDraw::Dialogs.getPointer(window, &data);
	return data;
}

extern DSurface * DAT_00887310;
namespace OwnerDraw {
extern int CachedSurfaceCount;
void __fastcall BlendGradientRect(Rect const & rect, Surface & surface, int color, int amount);
void __fastcall CopyDimmedBackground(Rect const & rect, HWND hWnd, int alpha);
}

void __fastcall FUN_00621B80(Rect * rect, Surface * surface, unsigned short color, int alpha);
namespace OwnerDraw {
int __fastcall DrawWideText(Surface * surface, wchar_t const * text, LTRBStruct * rect, BitFont * font, unsigned color, int style, int p5, int p6, int p7, int p8);
}

inline Rect Rect_From_RECT(RECT const & r)
{
	return Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}

// FUNCTION: GAMEMD 0x006163A0
// OwnerDraw::CheckBoxButtonHandler
LRESULT CALLBACK OwnerDraw::CheckBoxButtonHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	OwnerDraw::WinData *data = Find_Data(window);

	if (data != NULL) {
	switch (message) {
	case BM_GETCHECK:
		return data->CheckBox.checkState;

	case BM_SETCHECK:
		data->CheckBox.checkState = wparam;
		InvalidateRect(window, NULL, FALSE);
		return 0;

	case WM_PAINT:
		if (data->paintDisabled == 0) {
			RECT client;
			GetClientRect(window, &client);
			RECT disprect;
			LTRBStruct trect;
			Get_Display_Rect(window, (RECT *)&trect);
			Get_Display_Rect(window, &disprect);
			Rect drect = Rect_From_RECT(disprect);
			drect.Width = 18;
			drect.Height = 18;
			int style = GetWindowLong(window, GWL_STYLE);
			int state = data->CheckBox.checkState;
			unsigned char skinned = data->CheckBox.skinned;
			unsigned char variant = data->CheckBox.variant;
			bool checked = state == 1;
			char name[64];
			if (skinned) {
				if (checked) {
					if (variant) {
						strcpy(name, "cce_i.pcx");
					} else {
						strcpy(name, "cce_il.pcx");
					}
				} else {
					if (variant) {
						strcpy(name, "cce_ir.pcx");
					} else {
						strcpy(name, "cue_i.pcx");
					}
				}
			} else {
				char letter = 'u';
				if (checked) {
					letter = 'c';
				}
				sprintf(name, "c%ce_i.pcx", letter);
			}
			Surface *image = SurfaceCache.GetSurface(name, NULL);
			int image_height = image->Get_Height();
			Rect srect(0, 0, image->Get_Width(), image_height);
			DAT_00887310->Blit_From(drect, *image, srect, false, true);
			if (style & WS_DISABLED) {
				FUN_00621B80(&drect, DAT_00887310, 0, OwnerDraw::DisabledOverlayAlpha);
			}
			trect.Left += 26;
			if (data->Wide.text) {
				int color = OwnerDraw::PrimaryTextColor;
				if (style & WS_DISABLED) {
					color = OwnerDraw::DisabledTextColor;
				}
				OwnerDraw::DrawWideText(DAT_00887310, data->Wide.text, (LTRBStruct *)&trect, data->Wide.font, color, 4, 0xc, 0, 0, 0);
			}
			ValidateRect(window, NULL);
			return 0;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK: {
		POINT pt;
		pt.x = lparam & 0xffff;
		pt.y = (unsigned int)lparam >> 16;
		if (pt.x < 18 && pt.y < 18) {
			int checked = data->CheckBox.checkState != 1;
			data->CheckBox.checkState = checked;
			InvalidateRect(window, NULL, FALSE);
			VocClass::PlayGlobal(Rule->GUICheckboxSound, 1.0f, 0x2000);
			HWND parent = GetParent(window);
			SendMessage(parent, WM_COMMAND, MAKEWPARAM(GetWindowLong(window, GWL_ID), checked), (LPARAM)window);
			return(0);
		} else {
			return(0);
		}
	}

	case OD_SUBCLASSED: {
		WNDPROC proc = NULL;
		WNDPROC *found = NULL;
		if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
			proc = *found;
		}
		data->CheckBox.checkState = CallWindowProc(proc, window, BM_GETCHECK, 0, 0);
		break;
	}

	case WM_USER + 229: {
		unsigned char value = lparam != 0;
		unsigned char old = data->CheckBox.variant;
		data->CheckBox.skinned = value;
		if (old != value) {
			InvalidateRect(window, NULL, FALSE);
		}
		break;
	}

	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		InvalidateRect(window, NULL, FALSE);
		break;

	case WM_USER + 230: {
		unsigned char value = lparam != 0;
		unsigned char old = data->CheckBox.variant;
		data->CheckBox.variant = value;
		if (old != value) {
			InvalidateRect(window, NULL, FALSE);
		}
		break;
	}

	case WM_USER + 231:
		return data->CheckBox.variant;
	}

	WNDPROC proc = NULL;
	WNDPROC *found = NULL;
	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		proc = *found;
	}
	return CallWindowProc(proc, window, message, wparam, lparam);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0061D6D0
// OwnerDraw::ProgressHandler
LRESULT CALLBACK OwnerDraw::ProgressHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	RECT rect;
	Get_Display_Rect(window, &rect);

	OwnerDraw::WinData *data = Find_Data(window);
	if (data != NULL) {
		switch (message) {
		case OD_SUBCLASSED:
			data->ProgressBar.maximum = 100;
			break;

		case 0x401:
			data->ProgressBar.minimum = LOWORD(lparam);
			data->ProgressBar.maximum = HIWORD(lparam);
			break;

		case 0x402: {
			int pos = (int)wparam;
			if (pos < data->ProgressBar.minimum) {
				pos = data->ProgressBar.minimum;
			}
			if (pos > data->ProgressBar.maximum) {
				pos = data->ProgressBar.maximum;
			}
			data->ProgressBar.position = pos;
			InvalidateRect(window, NULL, FALSE);
			break;
		}

		case WM_PAINT: {
			Rect sourcerect(0, 0, rect.right - rect.left + 1, rect.bottom - rect.top + 1);
			Rect destrect(rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1);

			if (data->cachedSurface == NULL) {
				Surface *surf = new BSurface(rect.right - rect.left + 1, rect.bottom - rect.top + 1, 2);
				data->cachedSurface = surf;
				OwnerDraw::CachedSurfaceCount++;
				surf->Blit_From(sourcerect, *DAT_00887310, destrect);
			}

			DAT_00887310->Blit_From(destrect, *data->cachedSurface, sourcerect);
			int pos = (data->ProgressBar.position << 16) / (data->ProgressBar.maximum - data->ProgressBar.minimum);
			int color = ODColorToHiColor(0x000000FF);
			OwnerDraw::BlendGradientRect(destrect, *DAT_00887310, color, pos);
			ValidateRect(window, NULL);
			break;
		}
		}
	}
	return 0;
}

namespace UI {
void __fastcall GetKeyboardKeyString(unsigned short key, wchar_t *buffer);
}
int __fastcall FUN_00620F60(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags);
inline int Draw_Hotkey_Text0(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return FUN_00620F60(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text1(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text0(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text2(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text1(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text3(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text2(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text4(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text3(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text5(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text4(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text6(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text5(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text7(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text6(surface, text, rect, font, color, flags); }
namespace OwnerDraw {
void __fastcall DrawBeveledBorder(Surface & surface, Rect const & rect, int offset, int color);
}

// FUNCTION: GAMEMD 0x0061ECA0
// OwnerDraw::HotkeyHandler
LRESULT CALLBACK OwnerDraw::HotkeyHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	case WM_PAINT: {
		RECT rect;
		Get_Display_Rect(window, &rect);
		Rect alt_rect(rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1);
		Rect win_rect(0, 0, alt_rect.Width, alt_rect.Height);
		OwnerDraw::WinData *data = Find_Data(window);
		if (data != NULL) {
			if (data->cachedSurface == NULL) {
				Surface *surf = new BSurface(alt_rect.Width, alt_rect.Height, 2);
				data->cachedSurface = surf;
				OwnerDraw::CachedSurfaceCount++;
				surf->Blit_From(win_rect, *DAT_00887310, alt_rect);
			}
			wchar_t text[64];
			int key = SendMessage(window, 0x402, 0, 0);
			UI::GetKeyboardKeyString(key, text);
			if (data->cachedSurface != NULL) {
				DAT_00887310->Blit_From(alt_rect, *data->cachedSurface, win_rect);
			}
			OwnerDraw::DrawBeveledBorder(*DAT_00887310, alt_rect, 2, -1);
			if (wcslen(text) > 0) {
				rect.left += 4;
				rect.top += 4;
				rect.right -= 4;
				rect.bottom -= 4;
				Draw_Hotkey_Text7(DAT_00887310, text, &rect, data->Wide.font, OwnerDraw::PrimaryTextColor, DT_SINGLELINE | DT_VCENTER);
			}
			ValidateRect(window, NULL);
		}
		return 0;
	}

	case WM_ERASEBKGND:
		return 1;

	case WM_NCPAINT:
		return 0;
	}

	WNDPROC proc = NULL;
	WNDPROC *found = NULL;
	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		proc = *found;
	}
	return CallWindowProc(proc, window, message, wparam, lparam);
}

// FUNCTION: GAMEMD 0x0061EF70
// UI::GetKeyboardKeyString
void __fastcall UI::GetKeyboardKeyString(unsigned short key, wchar_t *buffer)
{
	char key_name[32];
	char text[500];
	unsigned char modifier = key >> 8;

	text[0] = 0;
	buffer[0] = 0;
	if ((modifier & 8) == 0) {
		UINT lparam;

		if (modifier & 4) {
			lparam = (MapVirtualKey(VK_MENU, 0) << 16) | 0x2000001;
			GetKeyNameText(lparam, key_name, sizeof(key_name));
			strcat(text, key_name);
			strcat(text, "+");
		}
		if (modifier & 2) {
			lparam = (MapVirtualKey(VK_CONTROL, 0) << 16) | 0x2000001;
			GetKeyNameText(lparam, key_name, sizeof(key_name));
			strcat(text, key_name);
			strcat(text, "+");
		}
		if (modifier & 1) {
			lparam = (MapVirtualKey(VK_SHIFT, 0) << 16) | 0x2000001;
			GetKeyNameText(lparam, key_name, sizeof(key_name));
			strcat(text, key_name);
			strcat(text, "+");
		}
		lparam = (MapVirtualKey(key & 0xff, 0) << 16) | 0x2000001;
		GetKeyNameText(lparam, key_name, sizeof(key_name));
		strcat(text, key_name);
		swprintf(buffer, L"%hs", text);
	}
}

ODComponentMasks ODMasks;
unsigned short ODBComponentMask;

// FUNCTION: GAMEMD 0x0061F190
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

// FUNCTION: GAMEMD 0x00620720
void ODDrawArrowBitmap(Surface * surface, Rect const & rect, BOOL upward, BOOL pressed, bool dark)
{
	if (surface != NULL) {
		char fname[32];
		Rect destrect;
		Rect srcrect;

		char state = 'r';
		if (pressed) {
			state = 'p';
		}

		if (upward) {
			sprintf(fname, "guparrow%c.pcx", state);
		} else {
			sprintf(fname, "gdnarrow%c.pcx", state);
		}

		Surface * arrow = SurfaceCache.GetSurface(&fname[dark == false], NULL);
		if (arrow != NULL) {
			destrect = rect;
			destrect.Width = arrow->Get_Width();
			destrect.Height = arrow->Get_Height();

			srcrect.Y = 0;
			srcrect.X = 0;
			srcrect.Width = arrow->Get_Width();
			srcrect.Height = arrow->Get_Height();

			surface->Blit_From(destrect, *arrow, srcrect);
		}
	}
}

// FUNCTION: GAMEMD 0x00620870
COLORREF ODPixelToPaletteColor(unsigned short pixel)
{
	RGBClass color = DSurface::Deconstruct_Hicolor_Pixel(pixel);
	return PALETTERGB(color.Get_Red(), color.Get_Green(), color.Get_Blue());
}

// FUNCTION: GAMEMD 0x00620810
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

// FUNCTION: GAMEMD 0x00620CD0
void ODDrawButtonRect(Surface & surf, Rect const & rect, int offset)
{
	Rect work = rect;

	int color1 = DSurface::Build_Hicolor_Pixel(39, 248, 116);
	int color2 = DSurface::Build_Hicolor_Pixel(19, 123, 57);
	int color3 = DSurface::Build_Hicolor_Pixel(30, 186, 87);
	int color4 = DSurface::Build_Hicolor_Pixel(24, 153, 71);

	work.X += -1 - offset;
	work.Y += -1 - offset;
	work.Width += 2 * offset + 2;
	work.Height += 2 * offset + 2;

	for (int i = 0; i < offset; i++) {

		Point2D end1;
		Point2D start1;
		Point2D end4;
		Point2D start4;

		end1.X = work.Width - i + work.X - 2;
		start1.X = i + work.X;
		end1.Y = i + work.Y;
		start1.Y = i + work.Y;
		surf.Draw_Line(start1, end1, color1);

		surf.Draw_Line(Point2D(i + work.X, work.Y - i + work.Height - 1), Point2D(work.Width - i + work.X - 1, work.Y - i + work.Height - 1), color2);

		surf.Draw_Line(Point2D(work.X + i, work.Y + i + 1), Point2D(work.X + i, work.Y - i + work.Height - 1), color3);

		end4.Y = work.Y - i + work.Height - 2;
		end4.X = work.X - i + work.Width - 1;
		start4.X = end4.X;
		start4.Y = i + work.Y;
		surf.Draw_Line(start4, end4, color4);
	}
}

inline unsigned Color_To_Pixel(unsigned color)
{
	return (GetGValue(color) >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft)
		| (GetBValue(color) >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft)
		| (GetRValue(color) >> Drawing::RedShiftRight << Drawing::RedShiftLeft);
}

// FUNCTION: GAMEMD 0x00620F60
int __fastcall FUN_00620F60(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags)
{
	unsigned pixel = Color_To_Pixel(color);
	int x = rect->left;
	int y = rect->top;
	int width = rect->right - x;
	int height = rect->bottom - y;
	if (font == NULL) {
		font = BitFont::Instance;
	}
	font->SetClipMode(true);
	font->SetBounds((LTRBStruct *)rect);
	font->SetColor(pixel);
	BitText::Instance->DrawText(font, surface, text, x, y, width, height, false, 0, 0);
	return 0;
}

namespace OwnerDraw {
int __fastcall DrawWideText(Surface * surface, wchar_t const * text, LTRBStruct * rect, BitFont * font, unsigned color, int style, int p5, int p6, int p7, int p8);
}

// FUNCTION: GAMEMD 0x00621040
// OwnerDraw::DrawWideText
int __fastcall OwnerDraw::DrawWideText(Surface * surface, wchar_t const * text, LTRBStruct * rect, BitFont * font, unsigned color, int style, int p5, int p6, int p7, int p8)
{
	unsigned pixel = Color_To_Pixel(color);
	int x = rect->Left;
	int y = rect->Top;
	int width = rect->Right - rect->Left;
	int height = rect->Bottom - rect->Top;
	if (style & 4) {
		int tw;
		int th;
		font->GetTextDimension(text, &tw, &th, width);
		y += (height - th) / 2;
	}
	font->SetClipMode(true);
	font->SetBounds(rect);
	font->SetColor(pixel);
	BitText::Instance->DrawText(font, surface, text, x, y, width, height, style, p7, p8);
	return 0;
}

// FUNCTION: GAMEMD 0x00621150
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

// FUNCTION: GAMEMD 0x006213A0
void OwnerDraw::Draw_Item(LPDRAWITEMSTRUCT drawit)
{
	if (DAT_00887308 != NULL && DAT_00887310 != NULL) {
		RECT rect1;
		Get_Display_Rect(drawit->hwndItem, &rect1);
		RECT rect2;
		GetClientRect(drawit->hwndItem, &rect2);

		if (GetWindowLong(drawit->hwndItem, GWL_STYLE) & WS_BORDER) {
			int x = GetSystemMetrics(SM_CXBORDER);
			int y = GetSystemMetrics(SM_CYBORDER);
			rect1.left += x;
			rect1.right -= x;
			rect1.top += y;
			rect1.bottom -= y;
		}

		if (drawit->CtlType == ODT_BUTTON) {
			WinData * data = NULL;
			HWND window = drawit->hwndItem;
			HWND key = window;
			if (Dialogs.getPointer(key, &data) && data != NULL) {
				data->DrawItem.itemState = drawit->itemState;
				InvalidateRect(window, &drawit->rcItem, FALSE);
				UpdateWindow(window);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006214B0
void __fastcall OwnerDraw::CopyDimmedBackground(Rect const & rect, HWND hWnd, int alpha)
{
	OwnerDraw::WinData * winData = NULL;
	OwnerDraw::WinData * parentWinData = NULL;

	OwnerDraw::Dialogs.getPointer(hWnd, &winData);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	RECT dispChild;
	Get_Display_Rect(hWnd, &dispChild);

	Surface * surface = winData->cachedSurface;
	if (!surface) {
		surface = new BSurface(rcClient.right + 1, rcClient.bottom + 1, 2);
		winData->cachedSurface = surface;
		OwnerDraw::CachedSurfaceCount++;

		Rect dstFull(0, 0, rcClient.right + 1, rcClient.bottom + 1);

		HWND parent = GetParent(hWnd);
		OwnerDraw::Dialogs.getPointer(parent, &parentWinData);

		Surface * parentSurf = parentWinData->cachedSurface;
		if (parentSurf) {
			RECT dispParent;
			Get_Display_Rect(parent, &dispParent);

			Rect srcRel;
			srcRel.X = dispChild.left - dispParent.left;
			srcRel.Y = dispChild.top - dispParent.top;
			srcRel.Width = dispChild.right - dispChild.left + 1;
			srcRel.Height = dispChild.bottom - dispChild.top + 1;

			surface->Blit_From(dstFull, *parentSurf, srcRel, false, true);
		} else {
			Surface * srcSurf = DAT_00887308;

			Rect srcAbs;
			srcAbs.X = dispChild.left;
			srcAbs.Y = dispChild.top;
			srcAbs.Width = dispChild.right - dispChild.left + 1;
			srcAbs.Height = dispChild.bottom - dispChild.top + 1;
			if (srcSurf) {
				surface->Blit_From(dstFull, *srcSurf, srcAbs, false, true);
			}
		}

		unsigned short * surfptr = (unsigned short *)surface->Lock();
		if (surfptr) {
			for (int i = 0; i < surface->Get_Width() * surface->Get_Height(); ++i) {
				surfptr[i] = OD_Blend_Color(surfptr[i], 0, alpha);
			}
			surface->Unlock();
		}
	}

	Rect src;
	src.X = rect.X;
	src.Y = rect.Y;
	src.Width = rect.Width;
	src.Height = rect.Height;
	src.X -= dispChild.left;
	src.Y -= dispChild.top;

	DAT_00887310->Blit_From(rect, *surface, src, false, true);
}

// FUNCTION: GAMEMD 0x006217E0
void __fastcall OwnerDraw::BlendGradientRect(Rect const & rect, Surface & surface, int color, int amount)
{
	int fade = 1;
	int run = (rect.Width * amount) >> 16;

	if (run < 0) return;
	if (run == 0) run = 1;

	unsigned short * surfptr = (unsigned short *)surface.Lock();
	if (!surfptr) return;

	for (int row = 0; row < rect.Height; row++) {
		int idx = rect.X + (rect.Y + row) * (surface.Stride() / 2);

		int q_div4 = rect.Height / 4;
		if (row == (q_div4 * 3)) {
			fade = 1;
		}
		if (row == q_div4) {
			fade = 0;
		}

		if (run > 0) {
			unsigned short * pixptr = &surfptr[idx];
			for (int i = 0; i < run; i++) {
				if (fade) {
					int alpha = (255 * (i + 1)) / rect.Width;
					*pixptr = OD_Blend_Color(*pixptr, color, alpha);
				} else {
					*pixptr = color;
				}
				pixptr++;
			}
		}
	}

	surface.Unlock();
}

// FUNCTION: GAMEMD 0x00621D30
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

// FUNCTION: GAMEMD 0x006223C0
void On_WM_NCDESTROY(HWND window)
{
	OwnerDraw::WinData * data;
	if (OwnerDraw::Dialogs.getPointer(window, &data)) {
		if (data != NULL && data->cachedSurface != NULL) {
			delete data->cachedSurface;
			data->cachedSurface = NULL;
			OwnerDraw::CachedSurfaceCount--;
		}
	}
}

// FUNCTION: GAMEMD 0x00622440
int WINAPI ODUpdateWindowRect(HWND window, RECT *rect)
{
	InvalidateRect(window, rect, FALSE);
	UpdateWindow(window);
	return(1);
}

// FUNCTION: GAMEMD 0x00622470
BOOL CALLBACK ODAddWindowToList(HWND window, ArrayList<HWND> * list)
{
	if (list != NULL) {
		list->add(window, list->length());
	}
	return(TRUE);
}

// FUNCTION: GAMEMD 0x006225E0
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

// FUNCTION: GAMEMD 0x00622610
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

struct DialogStackEntry {
	HWND handle;
	int id;
};
extern DialogStackEntry DAT_00B72D28[];
extern int DAT_00B72F50;
extern HWND DAT_00B72F44;
extern int DAT_00B72F48;
extern HWND DAT_00B73550;
extern HINSTANCE DAT_00B732F0;
void __fastcall Add_Modeless_Dialog(HWND dialog);

// FUNCTION: GAMEMD 0x00622650
HWND __fastcall UI::BeginDialog(int resource, DLGPROC proc, int param)
{
	unsigned short id = resource;
	LPCDLGTEMPLATE templ = (LPCDLGTEMPLATE)Fetch_Resource((LPCSTR)id, RT_DIALOG);
	if (templ == NULL) {
		return NULL;
	}

	int index = DAT_00B72F50;
	DAT_00B72D28[index].handle = NULL;
	DAT_00B72D28[index].id = 0;
	DAT_00B72F50++;

	struct {
		unsigned short id;
		int param;
	} init;
	init.id = (unsigned short)resource;
	init.param = param;

	HWND dialog = CreateDialogIndirectParam(DAT_00B732F0, templ, DAT_00B73550, proc, (LPARAM)&init);
	if (dialog == NULL) {
		DAT_00B72F50--;
		return NULL;
	}

	DAT_00B72D28[index].handle = dialog;
	DAT_00B72D28[index].id = id;

	Add_Modeless_Dialog(dialog);

	DAT_00B72F44 = dialog;
	DAT_00B72F48 = id;
	return dialog;
}

// FUNCTION: GAMEMD 0x00622720
void __fastcall UI::EndDialog(HWND window)
{
	InputManagerClass::Instance->DoSomething();
	FUN_00608070(window);
	DestroyWindow(window);

	for (int index = 0; index < DAT_00B72F50; index++) {
		if (DAT_00B72D28[index].handle == window) {
			memmove(&DAT_00B72D28[index], &DAT_00B72D28[index + 1], sizeof(DialogStackEntry) * (DAT_00B72F50 - (index + 1)));

			DAT_00B72D28[DAT_00B72F50 - 1].handle = NULL;
			DAT_00B72D28[DAT_00B72F50 - 1].id = 0;

			--DAT_00B72F50;

			if (DAT_00B72F50 > 0) {
				DAT_00B72F44 = DAT_00B72D28[DAT_00B72F50 - 1].handle;
				DAT_00B72F48 = DAT_00B72D28[DAT_00B72F50 - 1].id;

				SetForegroundWindow(DAT_00B72F44);
				SetFocus(DAT_00B72F44);
			} else {
				DAT_00B72F44 = NULL;
				DAT_00B72F48 = 0;

				SetForegroundWindow(DAT_00B73550);
				SetFocus(DAT_00B73550);
			}
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x00622800
void OwnerDraw::Display_Dialog(HWND window)
{
	ShowWindow(window, SW_SHOWNORMAL);
	SetForegroundWindow(window);
	Keyboard->Clear();
}

namespace OwnerDraw {
BOOL CALLBACK SetNeedsControlImage(HWND window, LPARAM lparam);
BOOL CALLBACK RegisterChildControlProc(HWND window, LPARAM lparam);
}
BOOL CALLBACK FUN_0060F320(HWND edit, LPARAM lparam);
bool __fastcall FUN_0060C540(HWND dialog);
void __fastcall FUN_0060CF00(HWND dialog);
void __fastcall Resize_Dialogs(HWND window);
void __fastcall Center_Window_Within_Window(HWND window);
extern int DAT_007F5BE0;

namespace UI { bool Updated(void); }
void __fastcall FUN_005D4D50(void);
bool __fastcall FUN_0055CBF0(void);

// FUNCTION: GAMEMD 0x00623120
// UI::Updated
bool UI::Updated(void)
{
	FUN_005D4D50();
	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
		Game::CallBack();
		return false;
	}
	if (!FUN_0055CBF0() && Game::MainLoop()) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00623170
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

// FUNCTION: GAMEMD 0x00623310
void OwnerDraw::Set_Custom_Message_Box_Text(HWND window, wchar_t const * text)
{
	HWND label = GetDlgItem(window, 0x5B0);
	SendMessage(label, WM_USER + 178, 0, (LPARAM)text);
	UpdateWindow(window);
}

// FUNCTION: GAMEMD 0x00623340
OwnerDraw::WinData::WinData(void)
{
	memset(this, 0, sizeof(*this));
	*(int *)((char *)this + 0x68) = 0xb;
	*(BitFont **)((char *)this + 0x64) = BitFont::Instance;
	Wide.editText = new WideWstring();
	*(int *)((char *)this + 0x40) = 0;
	*(int *)((char *)this + 0x5c) = 0;
	*(int *)((char *)this + 0x90) = -1;
}

// FUNCTION: GAMEMD 0x006233A0
// OwnerDraw::WinData::~WinData

// FUNCTION: GAMEMD 0x00623420
OwnerDraw::WinNode * OwnerDraw::WinData::Add_Node(int length)
{
	WinNode * node = (WinNode *)operator new(length * 2 + 0x12);
	if (node) {
		node->text = (wchar_t *)(node + 1);
		node->next = Wide.nodes;
		node->id = 0;
		Wide.nodes = node;
	}
	return node;
}

// FUNCTION: GAMEMD 0x00623460
void OwnerDraw::WinData::Remove_Node(WinNode * node)
{
	WinNode * prev = NULL;
	WinNode * cur = Wide.nodes;
	while (cur) {
		if (node == cur) {
			break;
		}
		prev = cur;
		cur = cur->next;
	}
	if (cur) {
		if (prev) {
			prev->next = cur->next;
		} else {
			Wide.nodes = cur->next;
		}
		delete cur;
	}
}

// FUNCTION: GAMEMD 0x006234B0
void OwnerDraw::WinData::Copy_Nodes(WinNode const * source)
{
	while (Wide.nodes) {
		Remove_Node(Wide.nodes);
	}
	while (source) {
		WinNode * node = Add_Node(wcslen(source->text));
		node->data = source->data;
		node->id = source->id;
		wcscpy(node->text, source->text);
		source = source->next;
	}
}

// FUNCTION: GAMEMD 0x00623560
void OwnerDraw::WinData::Set_Text(wchar_t const * str)
{
	if (Wide.text) {
		delete Wide.text;
		Wide.text = NULL;
	}
	if (str && *str) {
		Wide.text = new wchar_t[wcslen(str) + 1];
		wcscpy(Wide.text, str);
	}
}

// FUNCTION: GAMEMD 0x006235B0
void OwnerDraw::WinData::Set_Text(char const * str)
{
	if (Wide.text) {
		delete Wide.text;
		Wide.text = NULL;
	}
	if (str && *str) {
		Wide.text = new wchar_t[strlen(str) + 1];
		swprintf(Wide.text, L"%hs", str);
	}
}

// FUNCTION: GAMEMD 0x00623610
OwnerDraw::WinData & OwnerDraw::WinData::operator=(WinData const & that)
{
	WinNode * nodes = Wide.nodes;
	wchar_t * text = Wide.text;
	WideWstring * edit = Wide.editText;
	if (this == &that) {
		return *this;
	}
	memcpy(this, &that, sizeof(*this));
	Wide.text = text;
	Wide.editText = edit;
	Wide.nodes = nodes;
	Set_Text(that.Wide.text);
	Copy_Nodes(that.Wide.nodes);
	return *this;
}

// FUNCTION: GAMEMD 0x00623730
OwnerDraw::WinData::WinData(WinData const & that)
{
	WinNode * nodes = Wide.nodes;
	wchar_t * text = Wide.text;
	WideWstring * edit = Wide.editText;
	memcpy(this, &that, sizeof(*this));
	Wide.editText = edit;
	Wide.text = text;
	Wide.nodes = nodes;
	Set_Text(that.Wide.text);
	Copy_Nodes(that.Wide.nodes);
}

// FUNCTION: GAMEMD 0x00623840
int __fastcall FUN_00623840(HWND list)
{
	int count = SendMessage(list, 0x146, 0, 0);
	while (count--) {
		SendMessage(list, 0x144, count, 0);
	}
	return 0;
}

int CALLBACK FUN_00623DA0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
namespace UI { int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam); }

// FUNCTION: GAMEMD 0x00623CD0
int __fastcall FUN_00623CD0(wchar_t const * text)
{
	int result = -1;
	HWND dialog = UI::BeginDialog(0x10d, (DLGPROC)FUN_00623DA0, 0);
	if (dialog != NULL) {
		HWND item = GetDlgItem(dialog, 0x5b0);
		SendMessage(item, 0x4b2, 0, (LPARAM)text);
		SetWindowLong(dialog, 8, (LONG)&result);
		ShowWindow(dialog, 1);
		SetForegroundWindow(dialog);
		InputManagerClass::Instance->DoSomething();
		while (result < 0) {
			FUN_005D4D50();
			if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH && !Session.NetOpen && !Session.Suspended) {
				if (!FUN_0055CBF0() && Game::MainLoop()) {
					break;
				}
			}
		}
		UI::EndDialog(dialog);
	}
	return result == 0x5c1;
}

// FUNCTION: GAMEMD 0x00623DA0
int CALLBACK FUN_00623DA0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int handled = UI::StandardWndProc(window, message, wparam, lparam);
	if (handled) {
		return handled;
	}
	int *result = (int *)GetWindowLong(window, 8);
	if (message == WM_COMMAND) {
		int id = wparam & 0xffff;
		if (id >= 0x5c0 && id <= 0x5c1) {
			*result = id;
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00623E00
int __fastcall FUN_00623E00(HWND list)
{
	int count = SendMessage(list, 0x18b, 0, 0);
	while (count--) {
		SendMessage(list, 0x182, count, 0);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00623E40
int __fastcall FUN_00623E40(HWND edit, Wstring * out)
{
	int length = SendMessage(edit, 0xe, 0, 0);
	if (length <= 0) {
		out->clear();
		return 0;
	}
	char * buffer = new char[length + 2];
	int result = SendMessage(edit, 0x4b5, length + 1, (LPARAM)buffer);
	*out = buffer;
	delete[] buffer;
	return result;
}

// FUNCTION: GAMEMD 0x00623EA0
int __fastcall FUN_00623EA0(HWND edit, WideWstring * out)
{
	int length = SendMessage(edit, 0xe, 0, 0);
	if (length <= 0) {
		out->clear();
		return 0;
	}
	wchar_t * buffer = new wchar_t[length + 2];
	int result = SendMessage(edit, 0x4b3, length + 1, (LPARAM)buffer);
	*out = buffer;
	delete[] buffer;
	return result;
}

// FUNCTION: GAMEMD 0x00623F00
int __fastcall FUN_00623F00(HWND list, int index, Wstring * out)
{
	int length = SendMessage(list, 0x18a, index, 0);
	if (length <= 0) {
		out->clear();
	}
	char * buffer = new char[length + 2];
	int result = SendMessage(list, 0x4b7, index, (LPARAM)buffer);
	*out = buffer;
	delete[] buffer;
	return result;
}

// FUNCTION: GAMEMD 0x00623F60
int __fastcall FUN_00623F60(HWND list, int index, WideWstring * out)
{
	int length = SendMessage(list, 0x18a, index, 0);
	if (length <= 0) {
		out->clear();
	}
	wchar_t * buffer = new wchar_t[length + 2];
	int result = SendMessage(list, 0x4b6, index, (LPARAM)buffer);
	*out = buffer;
	delete[] buffer;
	return result;
}

// FUNCTION: GAMEMD 0x00623FC0
int __fastcall FUN_00623FC0(HWND combo, int index, Wstring * out)
{
	int length = SendMessage(combo, 0x149, index, 0);
	if (length <= 0) {
		out->clear();
	}
	char * buffer = new char[length + 2];
	int result = SendMessage(combo, 0x4bd, index, (LPARAM)buffer);
	*out = buffer;
	delete[] buffer;
	return result;
}

// FUNCTION: GAMEMD 0x00624020
int __fastcall FUN_00624020(HWND combo, int index, WideWstring * out)
{
	int length = SendMessage(combo, 0x149, index, 0);
	if (length <= 0) {
		out->clear();
	}
	wchar_t * buffer = new wchar_t[length + 2];
	int result = SendMessage(combo, 0x4c3, index, (LPARAM)buffer);
	*out = buffer;
	delete[] buffer;
	return result;
}

// FUNCTION: GAMEMD 0x006240A0
void CALLBACK FUN_006240A0(HWND dialog, int unused)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		*((char *)data + 0xd4) = 1;
	}
}

// FUNCTION: GAMEMD 0x00624110
OwnerDraw::SimpleDialogControl::SimpleDialogControl(void)
	: Field4(0), Field8(false)
{
}

// FUNCTION: GAMEMD 0x00624130
OwnerDraw::DialogControl::~DialogControl(void)
{
}

// FUNCTION: GAMEMD 0x00624140
void OwnerDraw::SimpleDialogControl::F1(HWND window, int value)
{
	Field4 = value;
	OwnerDraw::WinData *data = Get_Data(window);
	if (data != NULL) {
		*((char *)data + 0xd4) = 1;
	}
	Field8 = true;
}

// FUNCTION: GAMEMD 0x006241C0
bool OwnerDraw::SimpleDialogControl::F2(void)
{
	return Field8;
}

// FUNCTION: GAMEMD 0x006241D0
int OwnerDraw::SimpleDialogControl::F3(void)
{
	return Field4;
}

// FUNCTION: GAMEMD 0x006241E0
bool OwnerDraw::SimpleDialogControl::F4(void)
{
	return false;
}

void __fastcall Title_Screen_Restore(bool force);

// FUNCTION: GAMEMD 0x006241F0
int __fastcall Run_Dialog_Control(int resource, DLGPROC proc, OwnerDraw::DialogControl *control)
{
	HWND dialog = UI::BeginDialog(resource, proc, (int)control);
	if (dialog == NULL) {
		control->F1(NULL, 0);
		return 0;
	}
	ShowWindow(dialog, SW_SHOWNORMAL);
	SetForegroundWindow(dialog);
	InputManagerClass::Instance->DoSomething();
	while (!control->F2() && !control->F4()) {
		FUN_005D4D50();
		if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
			Game::CallBack();
		} else if (!FUN_0055CBF0() && Game::MainLoop()) {
			break;
		}
		Title_Screen_Restore(false);
	}
	UI::EndDialog(dialog);
	return control->F3();
}

// FUNCTION: GAMEMD 0x006242B0
bool __fastcall FUN_006242B0(HWND control)
{
	HWND key = control;
	char name[128];
	GetClassName(control, name, 128);
	if (strcmp(name, "Edit") == 0) {
		return true;
	}
	if (strcmp(name, "NewEdit") == 0) {
		return true;
	}
	if (strcmp(name, "ListBox") == 0) {
		OwnerDraw::WinData *data = NULL;
		if (OwnerDraw::Dialogs.getPointer(key, &data)) {
			return *(int *)((char *)data + 0x68) == 1;
		}
	}
	if (strcmp(name, "ComboBox") == 0) {
		return (GetWindowLong(key, GWL_STYLE) & 3) != 3;
	}
	return false;
}

// SYNTHETIC: GAMEMD 0x00624480
// ??_GWideWstring@@QAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006244A0
// ??_GDialogControl@OwnerDraw@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006244C0
// ??_GSimpleDialogControl@OwnerDraw@@UAEPAXI@Z
