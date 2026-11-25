// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "ownrdraw.h"
#include "draw.h"
#include "movies.h"
#include "shapeset.h"

class SHPStruct;

extern DynamicVectorClass<char const *> DAT_00ABF390;
MovieHandle *__fastcall OwnerDraw_InitMovieHandle(char const *, Surface *, Rect *);
void __cdecl FUN_004068E0(char const *, ...);

namespace OwnerDraw {
	struct WinData {

		LPARAM userData;
		int scrollBarWidth;
		HWND ownerWindow;
		HWND attachedWindow;
		Surface *cachedSurface;
		Surface *image;
		Surface *altImage;
		int itemHeightSet;
		int paintDisabled;
		int toolTipsEnabled;

		wchar_t *text;
		int hasText;
		int asciiOnly;
		WinNode *nodes;
		int hasFocus;
		WideWstring *editText;
		int caretIndex;
		int scrollStart;
		int textLimit;
		int caretBlink;
		wchar_t *rejectChars;
		int styleFlags;
		MovieHandle *StaticMovieHandle;
		int StaticLoopMovie;
		MovieHandle *StaticMovieAuxHandle;
		BitFont *Font;
		int ControlType;
		int DialogID;
		int StaticDrawMode;
		ConvertClass *StaticShapeDrawer;
		ShapeSet *StaticShape;
		bool StaticOwnsShape;
		int StaticTextRevealCount;
		int StaticTextRevealDelay;
		int StaticTextRevealStep;
		int StaticColorAdjust;
		int StaticSoundIndex;
		int StaticFrameCount;
		int StaticCurrentFrame;
		int StaticLastFrameTick;
		int StaticFrameDelayMs;
		HWND StaticFrameNotifyHwnd;
		bool StaticAnimationRunning;
		int StaticTextFlags;
		int LayoutBand;
		bool StaticFillBackground;
		COLORREF StaticFillColor;
		bool skipDraw;
		bool HasOpenAnimation;
		bool HasFadeAnimation;
		bool Unknown_0BF;
		int TooltipVariant;
		bool ButtonTimerActive;
		bool ButtonAlternateFrame;
		int alpha;
		bool UseItemColorOverrides;
		bool disabled;
		int ComboBoxMaxVisibleDropItems;
		bool Unknown_0D4;
		bool HasTopPanelAnimation;
		bool HasButtonAnimation;
		bool HasMainScreenAnimation;
		bool FlagD8;
		unsigned char skinned;
		unsigned char variant;
		bool Unknown_0DB;
		int ExtraWidth;
		SHPStruct *DialogBackground;
		SHPStruct *DialogBackgroundEx;

		union {
			struct {
				ArrayList<int> *rowColors;
				ArrayList<int> *selStates;
				int topIndex;
				int curSel;
				ArrayList<ColumnData> *columns;
			} ListBox;

			struct {
				int result;
				int dragging;
				int range;
				int position;
				int upPressed;
				int downPressed;
				int keepCapture;
			} ScrollBar;

			struct {
				int result;
				int dragging;
				int range;
				int value;
				int minimum;
				int thumbPos;
				int step;
				int showNumbers;
				int clickSuppress;
			} TrackBar;

			struct {
				int reservedE8[3];
				HWND dropdown;
				int reservedF8[6];
				int itemColors[50];
			} ComboBox;

			struct {
				char *text;
				COLORREF textColor;
			} Static;

			struct {
				int state;
			} Button;

			struct {
				int checkState;
			} CheckBox;

			struct {
				int reserved28;
				int reserved2C;
				int focusPending;
				int focusEnabled;
				int hadTabStop;
			} Edit;

			struct {
				int selection;
				int reserved2C;
				int scrollTop;
			} ComboDrop;

			struct {
				int itemState;
			} DrawItem;

			struct {
				int minimum;
				int maximum;
				int position;
			} ProgressBar;

			unsigned char _size[0x100];
		};

		HFONT font;
		int bkMode;
		COLORREF bkColor;
		COLORREF textColor;
		int field_138;
		int animState;

		WinData(void);
		WinData(WinData const & that);
		~WinData(void)
		{
			Set_Text((wchar_t const *)0);
			while (nodes != NULL) {
				WinNode *node = nodes;
				Remove_Node(node);
			}
			delete editText;
		}
		WinData & operator=(WinData const & that);

		WinNode *Add_Node(int length);
		void Copy_Nodes(WinNode const *source);
		void Set_Text(wchar_t const * str);
		void Set_Text(char const *text);
		void Remove_Node(WinNode *node);
		void Remove_All_Nodes(void)
		{
			while (nodes != NULL) {
				Remove_Node(nodes);
			}
		}
	};

}

#include "tiprequest.h"
#include "_keyboar.h"
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

namespace OwnerDraw {
char const * __fastcall GetTooltipStringLabel(HWND dialog, HWND control);
}
extern wchar_t const DAT_00887734[];

bool Is_Owner_Draw_Message(unsigned int msg);
bool __fastcall FUN_00602AE0(unsigned int msg);

void ODInitMasks(void);

int ODColorToHiColor(COLORREF color);

int WINAPI ODUpdateWindowRect(HWND window, RECT *rect);

BOOL CALLBACK ODAddWindowToList(HWND window, ArrayList<HWND> * list);

// SYNTHETIC: GAMEMD 0x00600340 SYMBOL
// _$E57
// SYNTHETIC: GAMEMD 0x00600370 SYMBOL
// _$E60
// SYNTHETIC: GAMEMD 0x00600390 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x006003B0 SYMBOL
// _$E66
// SYNTHETIC: GAMEMD 0x006003D0 SYMBOL
// _$E69
// SYNTHETIC: GAMEMD 0x006003F0 SYMBOL
// _$E72
// SYNTHETIC: GAMEMD 0x00600410 SYMBOL
// _$E75
// SYNTHETIC: GAMEMD 0x00600450 SYMBOL
// _$E78
// SYNTHETIC: GAMEMD 0x00600480 SYMBOL
// _$E81
// SYNTHETIC: GAMEMD 0x006004B0 SYMBOL
// _$E84
// SYNTHETIC: GAMEMD 0x006004E0 SYMBOL
// _$E87
// SYNTHETIC: GAMEMD 0x006004F0 SYMBOL
// _$E90
// SYNTHETIC: GAMEMD 0x00600510 SYMBOL
// _$E93

// SYNTHETIC: GAMEMD 0x00600530 SYMBOL
// _$E98
// SYNTHETIC: GAMEMD 0x00600550 SYMBOL
// _$E96
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
	ScrollButtonBevelAlpha = 0x7f;
	PrimaryTextColor = 0xffff;
	AltComboTextColor = 0xeeeeee;
	ImeCompositionTextColor = 0x9f9f;
	CaretColor = 0xffffff;
	UnusedMidGrayColor = 0x808080;
	ListSelectionFillColor = 0xff;
	AltSelectionFillColor = 0x626262;
	DefaultBorderColor = 0xff;
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
// _$E103
// SYNTHETIC: GAMEMD 0x00600750 SYMBOL
// _$E101
namespace OwnerDraw { Dictionary<HWND, WNDPROC> DialogProcs(Hash_HWND); }

// SYNTHETIC: GAMEMD 0x00600880 SYMBOL
// _$E108
// SYNTHETIC: GAMEMD 0x00600920 SYMBOL
// _$E106
namespace OwnerDraw { Dictionary<HWND, WNDPROC> SubclassProcs(Hash_HWND); }

// SYNTHETIC: GAMEMD 0x00600A50 SYMBOL
// _$E113
namespace OwnerDraw { Dictionary<HWND, OwnerDraw::WinData> Dialogs(Hash_HWND); }

// SYNTHETIC: GAMEMD 0x00600AF0 SYMBOL
// _$E111

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

inline bool Has_Button_Animation(int type)
{
	return type == 0xbc || type == 0xbd || type == 0x102 || type == 0xc2 || type == 0xc9 || type == 0xbc6;
}

inline bool Has_Main_Screen_Animation(int type)
{
	return type == 0x103 || type == 0xbc7;
}

inline bool Has_Flag_D8(int type)
{
	return type == 0x108 || type == 0xbc6;
}

inline OwnerDraw::WinData *Get_Data(HWND window)
{
	OwnerDraw::WinData *data = NULL;
	if (!OwnerDraw::Dialogs.getPointer(window, &data)) {
		return NULL;
	}
	return data;
}

// FUNCTION: GAMEMD 0x00600CA0
int FUN_00600CA0(HWND dialog, HWND control)
{
	OwnerDraw::WinData *record = Get_Data(dialog);
	int kind = record != 0 ? record->DialogID : 0;
	int id = GetDlgCtrlID(control);
	bool general =
    (kind == 0x10c || kind == 0x94 || kind == 0xa3 || kind == 0x73 || kind == 0xd8 || kind == 0xbbb || kind == 0xf5 ||
	 kind == 0x105 || kind == 0xe2 || kind == 0xd5 || kind == 0x2b5 || kind == 0xb7 || kind == 0x2b4 || kind == 0xbc7 ||
	 kind == 0x101 || kind == 0x129 || kind == 0xbc6 || kind == 0xd7 || kind == 0x108 || kind == 0xbc || kind == 0xbd ||
	 kind == 0xbb || kind == 0xb5 || kind == 0xbba || kind == 0xff || kind == 0x6b || kind == 0x100 || kind == 0x102 ||
	 kind == 0xb8 || kind == 0xd6 || kind == 0x103 || kind == 0x125 || kind == 0xe7 || kind == 0x116 || kind == 0xc2 ||
	 kind == 0xc9 || kind == 0x11d || kind == 0x11c || kind == 0x114 || kind == 700 || kind == 0x113 || kind == 0x109 ||
	 kind == 0x10f || kind == 0x117 || kind == 0xe6 || kind == 0xf3 || kind == 0xf4 || kind == 0x10e ||
	 kind == 0x112) &&
    (id == 0x694);
	if (general) {
		return (0x1e);
	}
	bool dual_rail = (kind == 0xbc || kind == 0xbd || kind == 0x102 || kind == 0xc2 || kind == 0xc9 || kind == 0xbc6) &&
                 (id == 0x6ec || id == 0x5a8);
	if (dual_rail) {
		return (0x1e);
	}
	if ((kind == 0x103) || (kind == 0xbc7)) {
		if (id != 0x6f4) {
			if (id == 0x6f3 || id == 0x69f || id == 0x6f6 || id == 0x6ef || id == 0x6f1 || id == 0x6f0 || id == 0x6f2 ||
			    id == 0x73c || id == 0x73d || id == 0x73e || id == 0x73f || id == 0x740 || id == 0x735 || id == 0x732 ||
			    id == 0x3ea || id == 0x733) {
				return 0x1e;
			}
			bool tall = id == 0x411 || id == 0x412 || id == 0x419 || id == 0x41a || id == 0x6a3 || id == 0x6a4 || id == 0x6a5 ||
            id == 0x6a7 || id == 0x6a6 || id == 0x6a8 || id == 0x73a || id == 0x739 || id == 0x73b || id == 0x738;
			if (tall) {
				return (0x3c);
			}
		} else {
			return 0xf;
		}
	}
	if ((kind == 0x108) || (kind == 0xbc6)) {
		if (id == 0x6d2 || id == 0x3ea || id == 0x733 || id == 0x71b || id == 0x732 || id == 0x69d || id == 0x69f ||
		    id == 0x69e || id == 0x6a0 || id == 0x78b || id == 0x5c8 || id == 0x411 || id == 0x412 || id == 0x413 ||
		    id == 0x414 || id == 0x415 || id == 0x416 || id == 0x417 || id == 0x418) {
			return 0x1e;
		}
		bool tall2 = id == 0x419 || id == 0x41a || id == 0x41b || id == 0x41c || id == 0x41d || id == 0x41e || id == 0x41f ||
             id == 0x420 || id == 0x6a3 || id == 0x6a4 || id == 0x6a9 || id == 0x6ac || id == 0x6af || id == 0x6b2 ||
             id == 0x6b5 || id == 0x6b8 || id == 0x6a5 || id == 0x6a7 || id == 0x6aa || id == 0x6ad || id == 0x6b0 ||
             id == 0x6b3 || id == 0x6b6 || id == 0x6b9 || id == 0x6a6 || id == 0x6a8 || id == 0x6ab || id == 0x6ae ||
             id == 0x6b1 || id == 0x6b4 || id == 0x6b7 || id == 0x6ba || id == 0x70a || id == 0x70b || id == 0x70c ||
             id == 0x70d || id == 0x70e || id == 0x70f || id == 0x710 || id == 0x711;
		if (tall2) {
			return (0x3c);
		}
	}
	if (Is_Owner_Draw_Message(kind) && id == 0x695) {
		return 0xf;
	}
	return 0xf;
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
		type = data->DialogID;
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
		type = data->DialogID;
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
		type = data->DialogID;
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
	return Has_Button_Animation(type) && (id == 0x6ec ||
			id == 0x5a8);
}

inline bool Is_Flag_Control_C(int type, int id)
{
	return Has_Main_Screen_Animation(type) && (id == 0x6f3 ||
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
	return Has_Flag_D8(type) && (id == 0x6d2 ||
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
	return Is_Owner_Draw_Message(type) && id == 0x695 && !Session.FUN_0069BBE0();
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
		type = data->DialogID;
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

inline bool Is_Sidebar_Button(int type, int id)
{
	return (type == 0x94 ||
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
		type == 0xbc7) && id == 0x71c;
}

// FUNCTION: GAMEMD 0x00602DD0
bool Is_Typed_Control(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = data->DialogID;
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);

	if (Is_Sidebar_Button(type, id)) {
		return true;
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

inline OwnerDraw::WinData *Find_Data(HWND window)
{
	OwnerDraw::WinData *data = NULL;
	OwnerDraw::Dialogs.getPointer(window, &data);
	return data;
}


// FUNCTION: GAMEMD 0x00603040
int Control_Sound_Slot(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
	int id = GetDlgCtrlID(control);

	if (Is_Sidebar_Button(type, id)) {
		return 0x25;
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
		type = data->DialogID;
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
		type = data->DialogID;
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

ConvertClass *FUN_0072E280(void);
ConvertClass *FUN_0072E2A0(void);
ConvertClass *FUN_0072DAC0(void);
ConvertClass *FUN_0072DAD0(void);
ConvertClass *FUN_0072D450(void);
ConvertClass *FUN_0072CC80(void);

// FUNCTION: GAMEMD 0x006035F0
ConvertClass *Get_Static_Drawer(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = data->DialogID;
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	ConvertClass *drawer = FUN_0072E280();

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
			drawer = FUN_0072E2A0();
		}
	}
	if (type == 0x94) {
		if (id == 0x6ea) {
			return FUN_0072DAC0();
		}
		if (id == 0x6ec) {
			return FUN_0072DAD0();
		}
	}
	if (type == 0x103) {
		goto typed;
	}
	if (type == 0xbc7) {
typed:
		if (id == 0x72b) {
			return FUN_0072D450();
		}
		return drawer;
	}
	if (type == 0xc4) {
		if (id == 0x7a9) {
			return FUN_0072CC80();
		}
	}
	return drawer;
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
	OwnerDraw::Dialogs.getPointer(dialog, &data);
	GetDlgCtrlID(control);
	return 0;
}

// FUNCTION: GAMEMD 0x00603870
int FUN_00603870(HWND dialog, HWND control, int arg)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = data->DialogID;
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

namespace OwnerDraw {
bool __fastcall TrySetDialogLayoutBand2(HWND dialog);
}

bool __fastcall FUN_006242B0(HWND control);

// FUNCTION: GAMEMD 0x00603AA0
int FUN_00603AA0(HWND dialog, HWND control)
{
	return OwnerDraw::TrySetDialogLayoutBand2(dialog) && FUN_006242B0(control) ? 0xc0 : 0;
}

// FUNCTION: GAMEMD 0x00603D30
void FUN_00603D30(HWND dialog, Surface *surface)
{
	OwnerDraw::WinData *data = NULL;
	HWND key = dialog;

	if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL) {
		data->StaticDrawMode = 2;
		data->image = surface;
		InvalidateRect(dialog, NULL, TRUE);
	}
}

// FUNCTION: GAMEMD 0x00603DB0
LRESULT FUN_00603DB0(HWND window, bool value)
{
	return SendMessage(window, 0x4e5, 0, value & 0xff);
}

// FUNCTION: GAMEMD 0x00603DD0
LRESULT FUN_00603DD0(HWND window, bool value)
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
		data->TooltipVariant = value;
	}
}

// FUNCTION: GAMEMD 0x00603EA0
int FUN_00603EA0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return data->TooltipVariant;
	}
	return -1;
}

// FUNCTION: GAMEMD 0x00603F00
WideWstring FUN_00603F00(HWND window, HWND control, int index)
{
	TooltipRequest request;
	request.Window = control;
	request.Index = index;
	SendMessage(window, 0x4e9, 0, (LPARAM)&request);
	return request.Text;
}

// FUNCTION: GAMEMD 0x00603F60
WideWstring FUN_00603F60(HWND window, HWND control)
{
	TooltipRequest request;
	request.Window = control;
	request.Index = -1;
	SendMessage(window, 0x4e9, 0, (LPARAM)&request);
	return request.Text;
}

// FUNCTION: GAMEMD 0x00603FC0
WideWstring FUN_00603FC0(HWND window, HWND control, WORD low, WORD high)
{
	int index = SendMessage(control, 0x4e8, 0, MAKELONG(low, high));
	WideWstring text = FUN_00603F00(window, control, index);
	return text;
}

// FUNCTION: GAMEMD 0x00604060
WideWstring FUN_00604060(HWND window, HWND control)
{
	char const * label = OwnerDraw::GetTooltipStringLabel(window, control);
	if (label != NULL) {
		return WideWstring(StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\ownrdraw.cpp", 0x7a5));
	}
	return WideWstring(DAT_00887734);
}



inline int Get_Dialog_Type(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return data->DialogID;
	}
	return 0;
}

struct Game { static int ObserverMode; static int PCXInitialized; };

// FUNCTION: GAMEMD 0x006040B0
char const * __fastcall OwnerDraw::GetTooltipStringLabel(HWND window, HWND control)
{
	int type = Get_Dialog_Type(window);
	int id = GetDlgCtrlID(control);
	if (id == 0) return NULL;
	if (id == -1) return NULL;
	if (type == 0xe2) {
		if (id == 0x683) {
			return "STT:MainButtonSinglePlayer";
		}
		if (id == 0x684) {
			return "STT:MainButtonWWOnline";
		}
		if (id == 0x578) {
			return "STT:MainButtonNetwork";
		}
		if (id == 0x686) {
			return "STT:MainButtonMovies";
		}
		if (id == 0x55c) {
			return "STT:MainButtonOptions";
		}
		if (id == 0x55f) {
			return "STT:MainButtonYuriWebSite";
		}
		if (id == 0x3ee) {
			return "STT:MainButtonExitGamemd";
		}
	}
	else if (type == 0x100) {
		if (id == 0x688) {
			return "STT:SingleButtonNewCampaign";
		}
		if (id == 0x689) {
			return "STT:SingleButtonLoadSavedGame";
		}
		if (id == 0x579) {
			return "STT:SingleButtonSkirmish";
		}
		if (id == 0x686) {
			return "STT:SingleButtonBack";
		}
	}
	else if (type == 0x94) {
		if (id == 0x6ea) {
			return "STT:AlliedCampaignIcon";
		}
		if (id == 0x6eb) {
			return "STT:CampaignAnimTutorial";
		}
		if (id == 0x6ec) {
			return "STT:SovietCampaignIcon";
		}
		if (id == 0x50f) {
			return "STT:CampaignSliderDifficulty";
		}
		if (id == 0x40e) {
			return "STT:CampaignButtonLoad";
		}
		if (id == 0x455) {
			return "STT:CampaignList";
		}
		if (id == 0x686) {
			return "STT:CampaignButtonBack";
		}
	}
	else if (type == 0xb7) {
		if (id == 0x525) {
			return "STT:LoadList";
		}
		if (id == 0x40f) {
			return "STT:LoadButtonLoad";
		}
		if (id == 0x686) {
			return "STT:LoadButtonBack";
		}
	}
	else if (type == 0x102) {
		if (id == 0x6a0) {
			return "STT:SkirmishEditPlayer";
		}
		if (id == 0x50b || id == 0x50e || id == 0x516 || id == 0x51a || id == 0x51b || id == 0x51c || id == 0x51d) {
			return "STT:SkirmishComboAIPlayer";
		}
		if (id == 0x6da || id == 0x6db || id == 0x6dc || id == 0x6dd || id == 0x6de || id == 0x6df || id == 0x6e0 || id == 0x6e1) {
			return "STT:SkirmishPictureFlag";
		}
		if (id == 0x6a1 || id == 0x510 || id == 0x513 || id == 0x51e || id == 0x514 || id == 0x51f || id == 0x520 || id == 0x521) {
			return "STT:SkirmishComboCountry";
		}
		if (id == 0x6a2 || id == 0x522 || id == 0x523 || id == 0x524 || id == 0x525 || id == 0x526 || id == 0x527 || id == 0x528) {
			return "STT:SkirmishComboColor";
		}
		if (id == 0x6a3 || id == 0x6a4 || id == 0x6a5 || id == 0x6a6 || id == 0x6a7 || id == 0x6a8 || id == 0x6aa || id == 0x6ab) {
			return "STT:HostComboStart";
		}
		if (id == 0x76d || id == 0x76e || id == 0x76f || id == 0x770 || id == 0x771 || id == 0x772 || id == 0x773 || id == 0x774) {
			return "STT:HostComboTeam";
		}
		if (id == 0x529) {
			return "STT:SkirmishSliderSpeed";
		}
		if (id == 0x511) {
			return "STT:SkirmishSliderCredits";
		}
		if (id == 0x50c) {
			return "STT:SkirmishSliderUnit";
		}
		if (id == 0x5aa) {
			return "STT:SkirmishButtonChooseMap";
		}
		if (id == 0x693) {
			return "STT:SkirmishCBoxRedeploys";
		}
		if (id == 0x54e) {
			return "STT:SkirmishCBoxShortGame";
		}
		if (id == 0x69a) {
			return "STT:SkirmishCBoxSWAllowed";
		}
		if (id == 0x69d) {
			return "STT:SkirmishCBoxBuildOffAlly";
		}
		if (id == 0x696) {
			return "STT:SkirmishCBoxCrates";
		}
		if (id == 0x468) {
			return "STT:SkirmishMapThumbnail";
		}
		if (id == 0x6ec) {
			return "STT:SkirmishLabelGameType";
		}
		if (id == 0x5a8) {
			return "STT:SkirmishLabelScenario";
		}
		if (id == 0x617) {
			return "STT:SkirmishButtonStartGame";
		}
		if (id == 0x5c0) {
			return "STT:SkirmishButtonBack";
		}
	}
	else if (type == 0xbb) {
		if (id == 0x541) {
			return "STT:LobbyEditYourName";
		}
		if (id == 0x53e) {
			return "STT:LobbyListUsers";
		}
		if (id == 0x549) {
			return "STT:LobbyListGames";
		}
		if (id == 0x53f) {
			return "STT:LobbyEditOutput";
		}
		if (id == 0x540) {
			return "STT:LobbyEditInput";
		}
		if (id == 0x6cc) {
			return "STT:LobbyButtonNew";
		}
		if (id == 0x6cd) {
			if (!Game::ObserverMode) {
				return "STT:LobbyButtonJoin";
			}
			return "STT:LobbyButtonObserve";
		}
		if (id == 0x760) {
			return "STT:ObserverToggle";
		}
		if (id == 0x6ce) {
			return "STT:LobbyButtonObserve";
		}
		if (id == 0x5c0) {
			return "STT:LobbyButtonBack";
		}
	}
	else if (type == 0xbc || type == 0xc2) {
		if (id == 0x6d2 || id == 0x6d3 || id == 0x6d4 || id == 0x6d5 || id == 0x6d6 || id == 0x6d7 || id == 0x6d8 || id == 0x6d9) {
			return "STT:HostPictureAcceptance";
		}
		if (id == 0x411 || id == 0x412 || id == 0x413 || id == 0x414 || id == 0x415 || id == 0x416 || id == 0x417 || id == 0x418) {
			return "STT:HostComboPlayer";
		}
		if (id == 0x6da || id == 0x6db || id == 0x6dc || id == 0x6dd || id == 0x6de || id == 0x6df || id == 0x6e0 || id == 0x6e1) {
			return "STT:HostPictureFlag";
		}
		if (id == 0x6a1 || id == 0x510 || id == 0x513 || id == 0x51e || id == 0x514 || id == 0x51f || id == 0x520 || id == 0x521) {
			return "STT:HostComboCountry";
		}
		if (id == 0x6a2 || id == 0x522 || id == 0x523 || id == 0x524 || id == 0x525 || id == 0x526 || id == 0x527 || id == 0x528) {
			return "STT:HostComboColor";
		}
		if (id == 0x6a3 || id == 0x6a4 || id == 0x6a5 || id == 0x6a6 || id == 0x6a7 || id == 0x6a8 || id == 0x6aa || id == 0x6ab) {
			return "STT:HostComboStart";
		}
		if (id == 0x76d || id == 0x76e || id == 0x76f || id == 0x770 || id == 0x771 || id == 0x772 || id == 0x773 || id == 0x774) {
			return "STT:HostComboTeam";
		}
		if (id == 0x529) {
			return "STT:HostSliderSpeed";
		}
		if (id == 0x402) {
			return "STT:HostSliderCredits";
		}
		if (id == 0x54f) {
			return "STT:HostSliderUnit";
		}
		if (id == 0x3e8) {
			return "STT:HostCBoxRedeploys";
		}
		if (id == 0x54e) {
			return "STT:HostCBoxShortGame";
		}
		if (id == 0x69a) {
			return "STT:HostCBoxSWAllowed";
		}
		if (id == 0x69d) {
			return "STT:HostCBoxBuildOffAlly";
		}
		if (id == 0x54d) {
			return "STT:HostCBoxCrates";
		}
		if (id == 0x53f) {
			return "STT:HostEditOutput";
		}
		if (id == 0x540) {
			return "STT:HostEditInput";
		}
		if (id == 0x468) {
			return "STT:HostMapThumbnail";
		}
		if (id == 0x5a8) {
			return "STT:HostLabelScenario";
		}
		if (id == 0x588) {
			return "STT:HostButtonGo";
		}
		if (id == 0x5aa) {
			return "STT:HostButtonChooseMap";
		}
		if (id == 2) {
			if (type == 0xc2) {
				return "STT:WOLNewGameCancel";
			}
			return "STT:HostButtonBack";
		}
		if (id == 0x5c2) {
			return "STT:WOLPageButton";
		}
		if (id == 0x3ed) {
			return "STT:EmoteButton";
		}
	}
	else if (type == 0xbd || type == 0xc9) {
		if (id == 0x6d2 || id == 0x6d3 || id == 0x6d4 || id == 0x6d5 || id == 0x6d6 || id == 0x6d7 || id == 0x6d8 || id == 0x6d9) {
			return "STT:GuestPictureAcceptance";
		}
		if (id == 0x411 || id == 0x412 || id == 0x413 || id == 0x414 || id == 0x415 || id == 0x416 || id == 0x417 || id == 0x418) {
			return "STT:GuestComboPlayer";
		}
		if (id == 0x6da || id == 0x6db || id == 0x6dc || id == 0x6dd || id == 0x6de || id == 0x6df || id == 0x6e0 || id == 0x6e1) {
			return "STT:GuestPictureFlag";
		}
		if (id == 0x6a1 || id == 0x510 || id == 0x513 || id == 0x51e || id == 0x514 || id == 0x51f || id == 0x520 || id == 0x521) {
			return "STT:GuestComboCountry";
		}
		if (id == 0x6a2 || id == 0x522 || id == 0x523 || id == 0x524 || id == 0x525 || id == 0x526 || id == 0x527 || id == 0x528) {
			return "STT:GuestComboColor";
		}
		if (id == 0x6a3 || id == 0x6a4 || id == 0x6a5 || id == 0x6a6 || id == 0x6a7 || id == 0x6a8 || id == 0x6aa || id == 0x6ab) {
			return "STT:GuestComboStart";
		}
		if (id == 0x76d || id == 0x76e || id == 0x76f || id == 0x770 || id == 0x771 || id == 0x772 || id == 0x773 || id == 0x774) {
			return "STT:GuestComboTeam";
		}
		if (id == 0x529) {
			return "STT:GuestSliderSpeed";
		}
		if (id == 0x402) {
			return "STT:GuestSliderCredits";
		}
		if (id == 0x54f) {
			return "STT:GuestSliderUnit";
		}
		if (id == 0x3e8) {
			return "STT:GuestCBoxRedeploys";
		}
		if (id == 0x54e) {
			return "STT:GuestCBoxShortGame";
		}
		if (id == 0x69a) {
			return "STT:GuestCBoxSWAllowed";
		}
		if (id == 0x69d) {
			return "STT:GuestCBoxBuildOffAlly";
		}
		if (id == 0x54d) {
			return "STT:GuestCBoxCrates";
		}
		if (id == 0x53f) {
			return "STT:GuestEditOutput";
		}
		if (id == 0x540) {
			return "STT:GuestEditInput";
		}
		if (id == 0x468) {
			return "STT:GuestMapThumbnail";
		}
		if (id == 0x5a8) {
			return "STT:GuestLabelScenario";
		}
		if (id == 0x59f) {
			return "STT:GuestButtonAccept";
		}
		if (id == 2) {
			if (type == 0xc9) {
				return "STT:WOLNewGameCancel";
			}
			return "STT:GuestButtonBack";
		}
		if (id == 0x5c2) {
			return "STT:WOLPageButton";
		}
		if (id == 0x3ed) {
			return "STT:EmoteButton";
		}
	}
	else if (type == 0x101) {
		if (id == 0x68d) {
			return "STT:OptionsButtonSneak";
		}
		if (id == 0x68e) {
			return "STT:OptionsButtonMovies";
		}
		if (id == 0x68f) {
			return "STT:OptionsButtonCredits";
		}
		if (id == 0x686) {
			return "STT:OptionsButtonBack";
		}
	}
	else if (type == 0x129) {
		if (id == 0x745) {
			return "GUI:PlayMovie";
		}
		if (id == 0x686) {
			return "STT:MsnDltButtonBack";
		}
		if (id == 0x744) {
			return "GUI:SelectMovie";
		}
	}
	else if (type == 0xd5) {
		if (id == 0x5ce) {
			return "STT:MainOptButtonKeyboard";
		}
		if (id == 0x5cd) {
			return "STT:MainOptButtonNetwork";
		}
		if (id == 0x686) {
			return "STT:MainOptButtonBack";
		}
		if (id == 0x674) {
			return "STT:MainOptCBoxStretch";
		}
		if (id == 0x52b) {
			return "STT:MainOptSliderVisual";
		}
		if (id == 0x6ed) {
			return "STT:MainOptComboModes";
		}
		if (id == 0x529) {
			return "STT:MainOptSliderSpeed";
		}
		if (id == 0x50f) {
			return "STT:MainOptSliderDifficulty";
		}
		if (id == 0x601) {
			return "STT:MainOptCBoxTargetLines";
		}
		if (id == 0x604) {
			return "STT:MainOptCBoxHidden";
		}
		if (id == 0x602) {
			return "STT:MainOptCBoxTooltips";
		}
		if (id == 0x52a) {
			return "STT:MainOptSliderScroll";
		}
		if (id == 0x51a) {
			return "STT:MainOptCBoxScrollCoasting";
		}
		if (id == 0x52f) {
			return "STT:MainOptSliderMusic";
		}
		if (id == 0x532) {
			return "STT:MainOptSliderSound";
		}
		if (id == 0x536) {
			return "STT:MainOptSliderVoice";
		}
	}
	else if (type == 0xd7) {
		if (id == 0x5d4) {
			return "STT:MultiOptComboCard";
		}
		if (id == 0x5d1) {
			return "STT:MultiOptEditSocketNum";
		}
		if (id == 0x5d3) {
			return "STT:MultiOptEditDestAddr";
		}
		if (id == 0x762) {
			return "STT:MultiOptSendDelay";
		}
		if (id == 0x5d7) {
			return "STT:MultiOptPortNumber";
		}
		if (id == 1) {
			return "STT:MultiOptButtonOk";
		}
		if (id == 2) {
			return "STT:MultiOptButtonCancel";
		}
	}
	else if (type == 0xa3) {
		if (id == 0x4c7) {
			return "STT:KeyboardComboCategory";
		}
		if (id == 0x6e9) {
			return "STT:KeyboardGroupDescription";
		}
		if (id == 0x4c8) {
			return "STT:KeyboardListCommands";
		}
		if (id == 0x63c) {
			return "STT:KeyboardLabelShortcut";
		}
		if (id == 0x63d) {
			return "STT:KeyboardLabelError";
		}
		if (id == 0x4cd) {
			return "STT:KeyboardEditEntry";
		}
		if (id == 0x4cf) {
			return "STT:KeyboardButtonAssign";
		}
		if (id == 0x63b) {
			return "STT:KeyboardLabelAssigned";
		}
		if (id == 0x4d0) {
			return "STT:KeyboardButtonResetAll";
		}
		if (id == 0x686) {
			return "STT:KeyboardButtonBack";
		}
	}
	else if (type == 0x10c) {
		if (id == 0x6e7) {
			return "STT:BriefingLabelContent";
		}
		if (id == 0x423) {
			return "STT:BriefingButtonReturn";
		}
	}
	else if (type == 0xb6) {
		if (id == 0x6c9) {
			int action = FUN_00603EA0((HWND)control);
			if (action == 3) return "STT:ConfirmExitButtonLeaveEx";
			if (action == 2) return "STT:ConfirmExitButtonLeave";
			return "STT:Empty";
		}
		if (id == 0x712) {
			int action = FUN_00603EA0((HWND)control);
			if (action == 4) return "STT:ConfirmExitButtonObserve";
			if (action == 5) return "STT:ConfirmExitButtonRestart";
			return "STT:Empty";
		}
		if (id == 0x686) {
			return "STT:ConfirmExitButtonResume";
		}
	}
	else if (type == 0xd9) {
		if (id == 0x5c1) {
			return "STT:ConfirmModeButtonOk";
		}
		if (id == 0x6ca) {
			return "STT:ConfirmModeButtonCancel";
		}
	}
	else if (type == 0x73) {
		if (id == 0x411 || id == 0x412 || id == 0x413 || id == 0x414 || id == 0x415 || id == 0x416 || id == 0x417 || id == 0x418) {
			return "STT:DiplomacyLabelPlayer";
		}
		if (id == 0x6cf || id == 0x6d0 || id == 0x6d1 || id == 0x6d2 || id == 0x6d3 || id == 0x6d4 || id == 0x6d5) {
			return "STT:DiplomacyCBoxAllies";
		}
		if (id == 0x6d6 || id == 0x6d7 || id == 0x6d8 || id == 0x6d9 || id == 0x6e2 || id == 0x6e3 || id == 0x6e4) {
			return "STT:DiplomacyCBoxListeners";
		}
		if (id == 0x419 || id == 0x41a || id == 0x41b || id == 0x41c || id == 0x41d || id == 0x41e || id == 0x41f || id == 0x420) {
			return "STT:DiplomacyCBoxKills";
		}
		if (id == 0x423) {
			return "STT:DiplomacyButtonReturn";
		}
		if (id == 0x730) {
			return "STT:TauntsOn";
		}
	}
	else if (type == 0xbbb) {
		if (id == 0x529) {
			return "STT:IGGameOptSliderSpeed";
		}
		if (id == 0x52a) {
			return "STT:IGGameOptSliderScroll";
		}
		if (id == 0x52b) {
			return "STT:IGGameOptSliderVisual";
		}
		if (id == 0x602) {
			return "STT:IGGameOptCBoxTooltips";
		}
		if (id == 0x601) {
			return "STT:IGGameOptCBoxTargetLines";
		}
		if (id == 0x604) {
			return "STT:IGGameOptCBoxHidden";
		}
		if (id == 0x51a) {
			return "STT:IGGameOptCBoxScrollCoast";
		}
		if (id == 0x52d) {
			return "STT:IGGameOptButtonSound";
		}
		if (id == 0x52c) {
			return "STT:IGGameOptButtonKeyboard";
		}
		if (id == 0x686) {
			return "STT:IGGameOptButtonBack";
		}
	}
	else if (type == 0x105) {
		if (id == 0x405) {
			return "STT:GenerateCBoxEnvironment";
		}
		if (id == 0x3ea) {
			return "STT:GenerateCBoxTime";
		}
		if (id == 0x407) {
			return "STT:GenerateCBoxTheater";
		}
		if (id == 0x406) {
			return "STT:GenerateCBoxMapSize";
		}
		if (id == 0x408) {
			return "STT:GenerateCBoxResources";
		}
		if (id == 0x3eb) {
			return "STT:GenerateSliderNumPlayers";
		}
		if (id == 0x621) {
			return "STT:GenerateButtonSurprise";
		}
		if (id == 0x620) {
			return "STT:GenerateButtonPreview";
		}
		if (id == 0x6c2) {
			return "STT:GenerateButtonLoadMap";
		}
		if (id == 0x6c3) {
			return "STT:GenerateButtonSaveMap";
		}
		if (id == 0x6c4) {
			return "STT:GenerateButtonDeleteMap";
		}
		if (id == 0x6c5) {
			return "STT:GenerateButtonUseMap";
		}
		if (id == 0x5c0) {
			return "STT:GenerateButtonCancel";
		}
	}
	else if (type == 0x2b5) {
		if (id == 0x528) {
			return "STT:MsnDltListMission";
		}
		if (id == 0x6c8) {
			return "STT:MsnDltButtonDelete";
		}
		if (id == 0x686) {
			return "STT:MsnDltButtonBack";
		}
	}
	else if (type == 0x2b4) {
		if (id == 0x527) {
			return "STT:MsnSaveListMission";
		}
		if (id == 0x526) {
			return "STT:MsnSaveEditName";
		}
		if (id == 0x6c7) {
			return "STT:MsnSaveButtonSave";
		}
		if (id == 0x686) {
			return "STT:MsnSaveButtonBack";
		}
	}
	else if (type == 0x108) {
		if (id == 0x6d2) {
			return "STT:MPScoreLabelGame";
		}
		if (id == 0x3ea) {
			return "STT:MPScoreLabelTime";
		}
		if (id == 0x411 || id == 0x412 || id == 0x413 || id == 0x414 || id == 0x415 || id == 0x416 || id == 0x417 || id == 0x418) {
			return "STT:MPScoreLabelPlayer";
		}
		if (id == 0x419 || id == 0x41a || id == 0x41b || id == 0x41c || id == 0x41d || id == 0x41e || id == 0x41f || id == 0x420) {
			return "STT:MPScoreLabelKills";
		}
		if (id == 0x6a3 || id == 0x6a4 || id == 0x6a9 || id == 0x6ac || id == 0x6af || id == 0x6b2 || id == 0x6b5 || id == 0x6b8) {
			return "STT:MPScoreLabelLosses";
		}
		if (id == 0x6a5 || id == 0x6a7 || id == 0x6aa || id == 0x6ad || id == 0x6b0 || id == 0x6b3 || id == 0x6b6 || id == 0x6b9) {
			return "STT:MPScoreLabelBuilt";
		}
		if (id == 0x6a6 || id == 0x6a8 || id == 0x6ab || id == 0x6ae || id == 0x6b1 || id == 0x6b4 || id == 0x6b7 || id == 0x6ba) {
			return "STT:MPScoreLabelScore";
		}
		if (id == 0x6d1) {
			return "STT:MPScoreButtonContinue";
		}
	}
	else if (type == 0xb5) {
		if (id == 0x521) {
			return "STT:IGSingleOptButtonControls";
		}
		if (id == 0x51e) {
			return "STT:IGSingleOptButtonLoad";
		}
		if (id == 0x51f) {
			return "STT:IGSingleOptButtonSave";
		}
		if (id == 0x520) {
			return "STT:IGSingleOptButtonDelete";
		}
		if (id == 0x522) {
			return "STT:IGSingleOptButtonAbort";
		}
		if (id == 0x686) {
			return "STT:IGSingleOptButtonBack";
		}
	}
	else if (type == 0xbba) {
		if (id == 0x521) {
			return "STT:IGMultiOptButtonControls";
		}
		if (id == 0x522) {
			return "STT:IGMultiOptButtonAbort";
		}
		if (id == 0x686) {
			return "STT:IGMultiOptButtonResume";
		}
	}
	else if (type == 0xea) {
		if (id == 0x640 || id == 0x642 || id == 0x641 || id == 0x643 || id == 0x644 || id == 0x645 || id == 0x646 || id == 0x647) {
			return "STT:ReconnectButtonPlayer";
		}
		if (id == 0x648 || id == 0x649 || id == 0x64a || id == 0x64b || id == 0x64c || id == 0x64d || id == 0x64e || id == 0x64f) {
			return "STT:ReconnectProgressPlayer";
		}
		if (id == 0x650) {
			return "STT:ReconnectLabelTime";
		}
		if (id == 0x651) {
			return "STT:ReconnectEditInfo";
		}
		if (id == 2) {
			return "STT:ReconnectButtonLeaveGame";
		}
	}
	else if (type == 0x6b) {
		if (id == 0x6eb) {
			return "STT:ScenarioListGameType";
		}
		if (id == 0x553) {
			return "STT:ScenarioListMaps";
		}
		if (id == 0x468) {
			return "STT:ScenarioMapThumbnail";
		}
		if (id == 0x6c5) {
			return "STT:ScenarioButtonUseMap";
		}
		if (id == 0x583) {
			return "STT:ScenarioButtonRandom";
		}
		if (id == 0x5c0) {
			return "STT:ScenarioButtonCancel";
		}
	}
	else if (type == 0xb8) {
		if (id == 0x52f) {
			return "STT:SoundOptSliderMusic";
		}
		if (id == 0x532) {
			return "STT:SoundOptSliderSound";
		}
		if (id == 0x536) {
			return "STT:SoundOptSliderVoice";
		}
		if (id == 0x530) {
			return "STT:SoundOptListScore";
		}
		if (id == 0x533) {
			return "STT:SoundOptCBoxShuffle";
		}
		if (id == 0x534) {
			return "STT:SoundOptCBoxRepeat";
		}
		if (id == 0x531) {
			return "STT:SoundOptButtonPlay";
		}
		if (id == 0x535) {
			return "STT:SoundOptButtonStop";
		}
		if (id == 0x686) {
			return "STT:SoundOptButtonBack";
		}
	}
	else if (type == 0x103) {
		if (id == 0x6f1) {
			return "STT:StandScoreParTime";
		}
		if (id == 0x6f2) {
			return "STT:StandScoreYourTime";
		}
		if (id == 0x6f3) {
			return "STT:StandScoreTitle";
		}
		if (id == 0x6f4) {
			return "STT:StandScoreMessage";
		}
		if (id == 0x6f6) {
			return "STT:StandScoreKills";
		}
		if (id == 0x423) {
			return "STT:StandScoreButtonReturn";
		}
	}
	else if (type == 0xff) {
		if (id == 0x3ef) {
			return "STT:OptionInternetLatency";
		}
		if (id == 0x529) {
			return "STT:OptionInternetSpeedSlider";
		}
		if (id == 0x521) {
			return "STT:OptionInternetGameControls";
		}
		if (id == 0x522) {
			return "STT:OptionInternetAbortMission";
		}
		if (id == 0x675) {
			return "STT:IGMultiOptButtonResume";
		}
	}
	else if (type == 0x122) {
		if (id == 0x6f9) {
			return "STT:WOLAddBuddyQueryDoNotAsk";
		}
		if (id == 1) {
			return "STT:WOLAddBuddyQueryYes";
		}
		if (id == 2) {
			return "STT:WOLAddBuddyQueryNo";
		}
	}
	else if (type == 0x112) {
		if (id == 0x6f9) {
			return "STT:WOLAutoLoginDoNotAsk";
		}
		if (id == 1) {
			return "STT:WOLAutoLoginYes";
		}
		if (id == 2) {
			return "STT:WOLAutoLoginNo";
		}
	}
	else if (type == 0x116) {
		if (id == 0x689) {
			return "STT:WOLBuddyBuddyList";
		}
		if (id == 0x6f0) {
			return "STT:WOLBuddyWins";
		}
		if (id == 0x6f1) {
			return "STT:WOLBuddyLosses";
		}
		if (id == 0x701) {
			return "STT:WOLBuddyDisconnects";
		}
		if (id == 0x6f4) {
			return "STT:WOLBuddyRank";
		}
		if (id == 0x6f5) {
			return "STT:WOLBuddyPoints";
		}
		if (id == 0x62b) {
			return "STT:WOLBuddyCreateGame";
		}
		if (id == 0x62d) {
			return "STT:WOLBuddyJoin";
		}
		if (id == 0x760) {
			return "STT:ObserverToggle";
		}
		if (id == 0x5c2) {
			return "STT:WOLBuddyPage";
		}
		if (id == 0x702) {
			return "STT:WOLBuddyAdd";
		}
		if (id == 0x703) {
			return "STT:WOLBuddyRemove";
		}
		if (id == 0x704) {
			return "STT:WOLBuddyRefresh";
		}
		if (id == 0x686) {
			return "STT:WOLBuddyBack";
		}
	}
	else if (type == 0x113) {
		if (id == 0x55d) {
			return "STT:WOLLobbyRefreshChannels";
		}
		if (id == 0x53f) {
			return "STT:LobbyEditOutput";
		}
		if (id == 0x540) {
			return "STT:LobbyEditInput";
		}
		if (id == 0x3ed) {
			return "STT:EmoteButton";
		}
		if (id == 0x53d) {
			return "STT:WOLLobbyChannelList";
		}
		if (id == 0x62b) {
			return "STT:LobbyButtonNew";
		}
		if (id == 0x62d) {
			if (!Game::ObserverMode) {
				return "STT:LobbyButtonJoin";
			}
			return "STT:LobbyButtonObserve";
		}
		if (id == 0x760) {
			return "STT:ObserverToggle";
		}
		if (id == 0x689) {
			return "STT:WOLBuddy";
		}
		if (id == 0x688) {
			return "STT:WOLClan";
		}
		if (id == 0x7a3) {
			return "STT:WOLTournament";
		}
		if (id == 0x7a6) {
			return "STT:WOLNews";
		}
		if (id == 0x61f) {
			return "STT:WOLLobbyOptions";
		}
		if (id == 0x686) {
			return "STT:WOLLobbyBack";
		}
		if (id == 0x769) {
			return "STT:WOLGamesortGametype";
		}
		if (id == 0x768) {
			return "STT:WOLGamesortPassword";
		}
		if (id == 0x767) {
			return "STT:WOLGamesortReslock";
		}
		if (id == 0x765) {
			return "STT:WOLGamesortAlpha";
		}
		if (id == 0x766) {
			return "STT:WOLGamesortPing";
		}
		if (id == 0x76a) {
			return "STT:WOLPlayersortAlpha";
		}
		if (id == 0x76b) {
			return "STT:WOLPlayersortRank";
		}
		if (id == 0x76c) {
			return "STT:WOLPlayersortClan";
		}
		if (id == 0x5c2) {
			return "STT:WOLPageButton";
		}
	}
	else if (type == 0xfe) {
		if (id == 0x3f0) {
			return "STT:WOLMyInformationLocale";
		}
		if (id == 1) {
			return "STT:WOLLocaleOK";
		}
		if (id == 2) {
			return "STT:WOLLocaleCancel";
		}
	}
	else if (type == 0x10f) {
		if (id == 0x628) {
			return "STT:WOLLoginNickname";
		}
		if (id == 0x72c) {
			return "STT:WOLLoginPassword";
		}
		if (id == 0x72d) {
			return "STT:WOLLoginRemember";
		}
		if (id == 0x539) {
			return "STT:WOLLoginLogin";
		}
		if (id == 0x6ea) {
			return "STT:WOLNewAccount";
		}
		if (id == 0x6eb) {
			return "STT:WOLLoginForget";
		}
		if (id == 0x686) {
			return "STT:WOLLoginBack";
		}
		if (id == 0x6ec) {
			return "STT:WOLManageAccount";
		}
	}
	else if (type == 0x11d) {
		if (id == 0x628) {
			return "STT:IGRLogin";
		}
		if (id == 0x72c) {
			return "STT:WOLLoginPassword";
		}
		if (id == 0x539) {
			return "STT:WOLLoginLogin";
		}
		if (id == 0x6ea) {
			return "STT:WOLNewAccount";
		}
		if (id == 0x686) {
			return "STT:WOLLoginBack";
		}
		if (id == 0x6ec) {
			return "STT:WOLManageAccount";
		}
		if (id == 0x629) {
			return "STT:WOLMyInformationServer";
		}
		if (id == 0x62a) {
			return "STT:WOLMyInformationLocale";
		}
		if (id == 0x6a1) {
			return "STT:WOLMyInformationCountry";
		}
		if (id == 0x6a2) {
			return "STT:WOLMyInformationColor";
		}
	}
	else if (type == 0x117) {
		if (id == 0x53f) {
			return "STT:WOLMatchOutput";
		}
		if (id == 0x5c0) {
			return (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) ? "WOL:WDTMatchBack" : "STT:WOLMatchCancel";
		}
	}
	else if (type == 0x114) {
		if (id == 0x628) {
			return "STT:WOLMyInformationName";
		}
		if (id == 0x629) {
			return "STT:WOLMyInformationServer";
		}
		if (id == 0x62a) {
			return "STT:WOLMyInformationLocale";
		}
		if (id == 0x6a1) {
			return "STT:WOLMyInformationCountry";
		}
		if (id == 0x6a2) {
			return "STT:WOLMyInformationColor";
		}
		if (id == 0x6ff) {
			return "STT:WOLMyInformationAutoLogin";
		}
		if (id == 0x6f0) {
			return "STT:WOLMyInformationWins";
		}
		if (id == 0x6f1) {
			return "STT:WOLMyInformationLosses";
		}
		if (id == 0x701) {
			return "STT:WOLMyInformationDisconnects";
		}
		if (id == 0x6f4) {
			return "STT:WOLMyInformationRank";
		}
		if (id == 0x6f5) {
			return "STT:WOLMyInformationPoints";
		}
		if (id == 0x687) {
			return "STT:WOLBuddy";
		}
		if (id == 0x688) {
			return "STT:WOLClan";
		}
		if (id == 0x7a3) {
			return "STT:WOLTournament";
		}
		if (id == 0x686) {
			return "STT:WOLMyInformationBack";
		}
		if (id == 0x6ea) {
			return "STT:WOLNewAccount";
		}
		if (id == 0x6eb) {
			return "STT:WOLLoginForget";
		}
		if (id == 0x6ec) {
			return "STT:WOLManageAccount";
		}
		if (id == 0x773) {
			return "STT:MatchSameRes";
		}
	}
	else if (type == 0xc3) {
		if (id == 0x548) {
			return "STT:WOLNewGameMaxPlayers";
		}
		if (id == 0x72c) {
			return "STT:WOLNewGamePassword";
		}
		if (id == 0x629) {
			return "STT:WOLNewGameTournament";
		}
		if (id == 0x66c) {
			return "STT:WOLNewGameClanTournament";
		}
		if (id == 1) {
			return "STT:WOLNewGameOK";
		}
		if (id == 2) {
			return "STT:WOLNewGameCancel";
		}
		if (id == 0x71e) {
			return "STT:WOLLimitScreenResolution";
		}
	}
	else if (type == 0x2bc) {
		if (id == 0x5f5) {
			return "STT:WOLOptionsPage";
		}
		if (id == 0x5f6) {
			return "STT:WOLOptionsLanguageFilter";
		}
		if (id == 0x5f7) {
			return "STT:WOLOptionsFind";
		}
		if (id == 0x5f8) {
			return "STT:WOLOptionsMusic";
		}
		if (id == 0x5f9) {
			return "STT:WOLOptionsShowAllInLobby";
		}
		if (id == 1) {
			return "STT:WOLOptionsOk";
		}
		if (id == 0x63a) {
			return "STT:WOLOptionsSoundOptions";
		}
		if (id == 2) {
			return "STT:WOLOptionsCancel";
		}
		if (id == 0x6ed) {
			return "STT:MainOptComboModes";
		}
		if (id == 0x5fa) {
			return "STT:WOLOptionFilterAsian";
		}
		if (id == 0x5fb) {
			return "STT:WOLOptionFilterNonAsian";
		}
		if (id == 0x775) {
			return "STT:WOLOptionFilterClan";
		}
		if (id == 0x774) {
			return "STT:WOLOptionFilterBuddies";
		}
	}
	else if (type == 0x10e) {
		if (id == 0x6f0) {
			return "STT:WOLMyInformationWins";
		}
		if (id == 0x6f1) {
			return "STT:WOLMyInformationLosses";
		}
		if (id == 0x701) {
			return "STT:WOLMyInformationDisconnects";
		}
		if (id == 0x6f4) {
			return "STT:WOLMyInformationRank";
		}
		if (id == 0x6f5) {
			return "STT:WOLMyInformationPoints";
		}
		if (id == 0x6e0) {
			return "STT:WOLWelcomeQuickMatch";
		}
		if (id == 0x6e1) {
			return "STT:WOLWelcomeCustomMatch";
		}
		if (id == 0x6e2) {
			return "STT:WOLBuddy";
		}
		if (id == 0x6e3) {
			return "STT:WOLWelcomeWDT";
		}
		if (id == 0x6e4) {
			return "STT:WOLWelcomeMyInformation";
		}
		if (id == 0x686) {
			return "STT:WOLWelcomeBack";
		}
		if (id == 0x771) {
			return "STT:QuickCoop";
		}
		if (id == 0x797) {
			return "STT:WelcomeUpdate";
		}
		if (id == 0x55f) {
			return "STT:MainButtonYuriWebSite";
		}
	}
	else if (type == 0x11c) {
		if (id == 0x6e0) {
			return "STT:WOLWelcomeQuickMatch";
		}
		if (id == 0x6e1) {
			return "STT:WOLWelcomeCustomMatch";
		}
		if (id == 0x6e2) {
			return "STT:WOLBuddy";
		}
		if (id == 0x6e3) {
			return "STT:WOLWelcomeWDT";
		}
		if (id == 0x686) {
			return "STT:WOLWelcomeBack";
		}
		if (id == 0x771) {
			return "STT:QuickCoop";
		}
		if (id == 0x55f) {
			return "STT:MainButtonYuriWebSite";
		}
	}
	else if (type == 0xe7) {
		if (id == 0x62e) {
			return "STT:WOLBeginNickMonth";
		}
		if (id == 0x62f) {
			return "STT:WOLBeginNickDay";
		}
		if (id == 0x630) {
			return "STT:WOLBeginNickYear";
		}
		if (id == 0x626) {
			return "STT:WOLBeginNickEmail";
		}
		if (id == 0x631 || id == 0x70a) {
			return "STT:WOLBeginNickNewsletter";
		}
		if (id == 1) {
			return "STT:WOLBeginNickContinue";
		}
		if (id == 0x576) {
			return "STT:WOLBeginNickViewPrivacy";
		}
		if (id == 2) {
			return "STT:WOLBeginNickCancel";
		}
	}
	else if (type == 0xe6 || type == 0xf3 || type == 0xf4) {
		if (id == 1) {
			return "STT:WOLNewNickOk";
		}
		if (id == 2) {
			return "STT:WOLNewNickCancel";
		}
		if (id == 0x628) {
			return "STT:WOLNewNickNickname";
		}
		if (id == 0x72c) {
			return "STT:WOLNewNickPassword";
		}
		if (id == 0x627) {
			return "STT:WOLNewNickVerify";
		}
		if (id == 0x5e5) {
			return "STT:WOLNewNickParentEMail";
		}
		if (id == 0x66b || id == 0x70b) {
			return "STT:WOLNewNickSellInfo";
		}
	}
	else if (type == 0x125) {
		if (id == 0x686) {
			return "WOL:WDTFactionBack";
		}
		return "WDT:FactionSelectToolTip";
	}
	else if (type == 0xbc6) {
		if (id == 0x781 || id == 0x782) {
			return "STT:MPScoreLabelPlayer";
		}
		if (id == 0x783 || id == 0x784) {
			return "STT:MPScoreLabelKills";
		}
		if (id == 0x785 || id == 0x786) {
			return "STT:MPScoreLabelLosses";
		}
		if (id == 0x787 || id == 0x788) {
			return "STT:MPScoreLabelBuilt";
		}
		if (id == 0x789 || id == 0x78a) {
			return "STT:MPScoreLabelScore";
		}
		if (id == 0x733) {
			return "STT:MPScoreLabelCampaignName";
		}
		if (id == 0x71b) {
			return "STT:MPScoreLabelMapName";
		}
		if (id == 0x732) {
			return "STT:MPScoreLabelDifficulty";
		}
		if (id == 0x6d1) {
			return "STT:MPCampScoreContinue";
		}
		if (id == 0x5c0) {
			return "STT:MPCampScoreQuit";
		}
		if (id == 0x3ea) {
			return "STT:MPScoreLabelTime";
		}
		if (id == 0x411 || id == 0x412) {
			return "STT:MPScoreLabelPlayer";
		}
		if (id == 0x6da || id == 0x6db) {
			return "STT:HostPictureFlag";
		}
		if (id == 0x6a1 || id == 0x510) {
			return "STT:HostComboCountry";
		}
		if (id == 0x6a2 || id == 0x522) {
			return "STT:HostComboColor";
		}
		if (id == 0x540) {
			return "STT:HostEditInput";
		}
		if (id == 0x53f) {
			return "STT:HostEditOutput";
		}
	}
	else if (type == 0xbc7) {
		if (id == 0x733) {
			return "STT:MPScoreLabelCampaignName";
		}
		if (id == 0x735) {
			return "STT:MPCampEndCampaignNumber";
		}
		if (id == 0x732) {
			return "STT:MPScoreLabelDifficulty";
		}
		if (id == 0x3ea) {
			return "STT:MPCampEndTime";
		}
		if (id == 0x411 || id == 0x412 || id == 0x73a || id == 0x739) {
			return "STT:MPScoreLabelPlayer";
		}
		if (id == 0x419 || id == 0x41a) {
			return "STT:MPScoreLabelKills";
		}
		if (id == 0x6a3 || id == 0x6a4) {
			return "STT:MPScoreLabelLosses";
		}
		if (id == 0x6a5 || id == 0x6a7) {
			return "STT:MPScoreLabelBuilt";
		}
		if (id == 0x6a6 || id == 0x6a8 || id == 0x73b || id == 0x738) {
			return "STT:MPScoreLabelScore";
		}
		if (id == 0x6d1) {
			return "STT:MPScoreButtonContinue";
		}
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x00606740
void FUN_00606740(HWND dialog, bool value)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		data->skipDraw = value;
	}
}

// FUNCTION: GAMEMD 0x006067A0
char FUN_006067A0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return data->skipDraw;
	}
	return false;
}

BOOL CALLBACK FUN_00606800(HWND window, LPARAM lparam);

bool __fastcall FUN_00608CD0(HWND parent, HWND child);
bool FUN_00609730(HWND dialog, HWND control);


inline bool Is_Plain_Control(HWND window)
{
	if ((GetWindowLong(window, GWL_STYLE) & BS_OWNERDRAW) == BS_OWNERDRAW) {
		HWND key = window;
		OwnerDraw::WinData *data = NULL;
		OwnerDraw::Dialogs.getPointer(key, &data);
		if (data != NULL && data->ControlType == 0) {
			return true;
		}
	}
	return false;
}


// FUNCTION: GAMEMD 0x00606800
BOOL CALLBACK FUN_00606800(HWND window, LPARAM lparam)
{
	HWND parent = GetParent(window);
	int type = Get_Dialog_Type(parent);
	int id = GetDlgCtrlID(window);
	bool value = lparam != 0;

	if (((type == 0x10c || type == 0x94 || type == 0xa3 || type == 0x73 || type == 0xd8 || type == 0xbbb || type == 0xf5 || type == 0x105 || type == 0xe2 || type == 0xd5 || type == 0x2b5 || type == 0xb7 || type == 0x2b4 || type == 0xbc7 || type == 0x101 || type == 0x129 || type == 0xbc6 || type == 0xd7 || type == 0x108 || type == 0xbc || type == 0xbd || type == 0xbb || type == 0xb5 || type == 0xbba || type == 0xff || type == 0x6b || type == 0x100 || type == 0x102 || type == 0xb8 || type == 0xd6 || type == 0x103 || FUN_00602AE0(type)) && id == 0x694) ||
		(Is_Owner_Draw_Message(type) && id == 0x695) ||
		((type == 0x94 || type == 0xd8 || type == 0xf5 || type == 0xe2 || type == 0xd5 || type == 0x101 || type == 0x129 || type == 0xd7 || type == 0xbb || type == 0x100 || type == 0xd6 || type == 0x125 || type == 0x122 || type == 0x112 || type == 0xe7 || type == 0x116 || type == 0x11d || type == 0x11c || type == 0xfe || type == 0x10f || type == 0x117 || type == 0x114 || type == 0xe6 || type == 0xf3 || type == 0xf4 || type == 0x2bc || type == 0x10e || type == 0x108 || type == 0xbc6 || type == 0xbc7) && id == 0x71c) ||
		((type == 0xbc || type == 0xbd || type == 0x102 || type == 0xc2 || type == 0xc9 || type == 0xbc6 || type == 0x105 || type == 0x6b || type == 0x113) && id == 0x468) ||
		Is_Flag_Control_B(type, id) ||
		((type == 0x103 || type == 0xbc7) && (id == 0x6f3 || id == 0x6f4 || id == 0x72b || id == 0x6ef || id == 0x6f1 || id == 0x6f0 || id == 0x6f2 || id == 0x69f || id == 0x6f6 || id == 0x733 || id == 0x735 || id == 0x732 || id == 0x3ea || id == 0x736 || id == 0x737 || id == 0x73c || id == 0x73d || id == 0x73e || id == 0x73f || id == 0x740 || Is_Quick_Control_B(id))) ||
		((type == 0x108 || type == 0xbc6) && (id == 0x6d2 || id == 0x3ea || id == 0x69d || id == 0x69f || id == 0x69e || id == 0x6a0 || id == 0x78b || id == 0x5c8 || id == 0x411 || id == 0x412 || id == 0x413 || id == 0x414 || id == 0x415 || id == 0x416 || id == 0x417 || id == 0x418 || Is_Quick_Control_D(id) || id == 0x733 || id == 0x732 || id == 0x71b || id == 0x735)) ||
		(Is_Plain_Control(window) && FUN_00608CD0(parent, window)) ||
		(Is_Plain_Control(window) && FUN_00609730(parent, window))) {
		FUN_00606740(window, value);
	}
	return TRUE;
}

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

class SHPStruct;
namespace OwnerDraw {
	extern SHPStruct *SmallButtonAnimShape;
	ConvertClass *GetSmallButtonAnimConvert(void);
	void __fastcall DrawCampaignMenuTransition(HWND dialog, bool reverse);
}
namespace Drawing { extern int RenderWidth; }
extern SHPStruct *DAT_00B0FAF8;
extern SHPStruct *DAT_00B0F9DC;
extern SHPStruct *DAT_00B0FAC0;
extern SHPStruct *DAT_00B0FB00;
extern SHPStruct *DAT_00B0FB34;
extern int DAT_00B0FA20;
extern Rect *DAT_00B0FC10;
extern Rect *DAT_00B0FC14;
extern Rect *DAT_00B0FC18;
extern Rect *DAT_00B0FC1C;
extern Rect *DAT_00B0FC20;
Point2D __fastcall FUN_0072A9C0(Rect const *rect);
void FUN_00406F70(void);
int FUN_0060A230(HWND window);
int FUN_0060A300(HWND window);
extern Rect *DAT_00B0FC28;
extern int DAT_007F5BE4;
// A piece's slide phase at a step: -1 before it starts, -2 once its six frames are done.
static inline int Slide_Phase(int delta)
{
	if (delta < 0) return -1;
	if (delta >= 6) return -2;
	return delta;
}

// Slides the frame pieces in sequence and then notifies the dialog.
// FUNCTION: GAMEMD 0x006071E0
void __fastcall OwnerDraw::DrawCampaignMenuTransition(HWND dialog, bool reverse)
{
	OwnerDraw::WinData *record = Get_Data(dialog);
	bool animate_main_screen = record ? record->HasMainScreenAnimation : false;
	record = Get_Data(dialog);
	bool animate_buttons = record ? record->HasButtonAnimation : false;
	record = Get_Data(dialog);
	bool animate_top_panel = record ? record->HasTopPanelAnimation : false;

	ConvertClass *drawer = FUN_0072E280();
	ConvertClass *button_convert = OwnerDraw::GetSmallButtonAnimConvert();
	ShapeSet const *top_background_shape = (ShapeSet const *)DAT_00B0FAF8;
	ShapeSet const *button_shape = (ShapeSet const *)OwnerDraw::SmallButtonAnimShape;
	ShapeSet const *button_panel_shape = (ShapeSet const *)DAT_00B0F9DC;
	ShapeSet const *top_panel_shape = (ShapeSet const *)DAT_00B0FAC0;
	ConvertClass *main_screen_convert = FUN_0072D450();
	ShapeSet const *main_screen_shape = (ShapeSet const *)DAT_00B0FB00;
	int count = DAT_00B0FA20;
	Rect window = AlternateSurface->Get_Rect();
	Rect top_background = *DAT_00B0FC20;
	Rect button_panel = *DAT_00B0FC14;
	Rect top_panel = *DAT_00B0FC18;
	Rect button_row = *DAT_00B0FC10;
	Rect main_screen = *DAT_00B0FC1C;

	RECT client;
	Get_Display_Rect(dialog, &client);
	RECT screen;
	GetWindowRect(dialog, &screen);
	int dx = screen.left - client.left;
	int dy = screen.top - client.top;

	int x = button_row.X;
	int y = button_row.Y;
	int width = button_row.Width;
	int height = button_row.Height * count;
	if (animate_top_panel) {
		x = top_panel.X;
		width = top_panel.Width;
		y = top_panel.Y;
		height += button_row.Y;
	}
	Rect from(x, y, width, height);
	Rect to = from;
	to.X += dx;
	to.Y += dy;
	Rect main_source = main_screen;
	Rect main_destination = main_source;
	main_destination.X += dx;
	main_destination.Y += dy;

	int dir = reverse ? -1 : 1;
	int button_frame = 5;
	if (reverse) button_frame = 10;
	int empty_row_frame = 11;
	if (reverse) empty_row_frame = 16;
	int button_panel_frame = 1;
	if (reverse) button_panel_frame = 6;
	int top_panel_frame = 0;
	if (reverse) top_panel_frame = 5;
	int main_screen_frame = 0;
	if (reverse) main_screen_frame = 5;

	int buttons = FUN_0060A230(dialog);
	int last = count - 1;
	int top = button_row.Y;
	if (animate_buttons) top += button_row.Height;
	bool any = FUN_0060A300(dialog);
	if (!any) last++;

	// One start step per piece: the button rows, then the button panel, top panel and main screen.
	const int pieces = count + 3;
	const int button_panel_index = pieces - 3;
	const int top_panel_index = pieces - 2;
	int *order = new int[pieces];
	const int main_screen_index = pieces - 1;
	int value = 1;
	for (int i = 0; i < top_panel_index; i++) {
		order[i] = value++;
	}
	order[main_screen_index] = 0;
	order[top_panel_index] = value + 1;
	order[button_panel_index] = 0;
	int steps;
	{
		int last_start = 0;
		for (int j = 0; j < pieces; ++j) {
			if (last_start <= order[j]) last_start = order[j];
		}
		steps = last_start + 6;
	}

	for (int step = 0; step < steps; step++) {
		if (animate_buttons) {
			Point2D point = FUN_0072A9C0(&button_panel);
			int phase = Slide_Phase(step - order[button_panel_index]);
			if (phase == -1) {
				Draw_Shape(*AlternateSurface, *drawer, button_panel_shape, reverse ? 6 : 0, point, window, SHAPE_WIN_REL);
			} else if (phase == -2) {
				if (!reverse) {
					Draw_Shape(*AlternateSurface, *drawer, button_panel_shape, 6, point, window, SHAPE_WIN_REL);
				}
			}
		}
		if (animate_top_panel) {
			Point2D point = FUN_0072A9C0(&top_panel);
			int phase = Slide_Phase(step - order[top_panel_index]);
			if (phase == -1) {
				Draw_Shape(*AlternateSurface, *drawer, top_background_shape, !reverse, point, window, SHAPE_WIN_REL);
			} else if (phase == -2) {
				Draw_Shape(*AlternateSurface, *drawer, top_background_shape, reverse ? 1 : 0, point, window, SHAPE_WIN_REL);
			} else {
				point = FUN_0072A9C0(&top_background);
				Draw_Shape(*AlternateSurface, *drawer, top_background_shape, 1, point, window, SHAPE_WIN_REL);
				point = FUN_0072A9C0(&top_panel);
				Draw_Shape(*AlternateSurface, *drawer, top_panel_shape, phase * dir + top_panel_frame, point, window, SHAPE_WIN_REL);
			}
		}
		if (animate_buttons) {
			Point2D point = FUN_0072A9C0(&button_panel);
			int phase = Slide_Phase(step - order[button_panel_index]);
			if (phase == -2) {
				if (reverse) {
					Draw_Shape(*AlternateSurface, *drawer, button_panel_shape, 0, point, window, SHAPE_WIN_REL);
				}
			} else {
				Draw_Shape(*AlternateSurface, *drawer, button_panel_shape, phase * dir + button_panel_frame, point, window, SHAPE_WIN_REL);
			}
		}
		int k;
		for (k = 0; k < buttons; k++) {
			Point2D point(button_row.X, k * button_row.Height + top);
			int phase = Slide_Phase(step - order[k]);
			if (phase == -1) {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, reverse ? 10 : 1, point, window, SHAPE_WIN_REL);
			} else if (phase == -2) {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, reverse ? 1 : 10, point, window, SHAPE_WIN_REL);
			} else {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, phase * dir + button_frame, point, window, SHAPE_WIN_REL);
			}
		}
		for (k = buttons; k < last; k++) {
			Point2D point(button_row.X, k * button_row.Height + top);
			int phase = Slide_Phase(step - order[k]);
			if (phase == -1) {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, reverse ? 10 : 0, point, window, SHAPE_WIN_REL);
			} else if (phase == -2) {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, reverse ? 0 : 10, point, window, SHAPE_WIN_REL);
			} else {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, phase * dir + empty_row_frame, point, window, SHAPE_WIN_REL);
			}
		}
		if (any) {
			Point2D point(button_row.X, DAT_00B0FC28->Y - button_row.Height);
			int phase = Slide_Phase(step - order[count - 1]);
			if (phase == -1) {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, reverse ? 10 : 1, point, window, SHAPE_WIN_REL);
			} else if (phase == -2) {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, reverse ? 1 : 10, point, window, SHAPE_WIN_REL);
			} else {
				Draw_Shape(*AlternateSurface, *button_convert, button_shape, phase * dir + button_frame, point, window, SHAPE_WIN_REL);
			}
		}
		if (animate_main_screen) {
			Point2D point = FUN_0072A9C0(&main_screen);
			int phase = Slide_Phase(step - order[main_screen_index]);
			if (phase == -1) {
				if (!reverse) {
					if (Drawing::RenderWidth >= DAT_007F5BE4) point.X += 0x50;
				}
				Draw_Shape(*AlternateSurface, *main_screen_convert, (reverse ? (ShapeSet const *)DAT_00B0FB34 : main_screen_shape), 0, point, window, SHAPE_WIN_REL);
			} else if (phase == -2) {
				if (reverse) {
					if (Drawing::RenderWidth >= DAT_007F5BE4) point.X += 0x50;
				}
				Draw_Shape(*AlternateSurface, *main_screen_convert, (reverse ? main_screen_shape : (ShapeSet const *)DAT_00B0FB34), 0, point, window, SHAPE_WIN_REL);
			} else {
				if (Drawing::RenderWidth >= DAT_007F5BE4) point.X += 0x50;
				Draw_Shape(*AlternateSurface, *main_screen_convert, main_screen_shape, phase * dir + main_screen_frame, point, window, SHAPE_WIN_REL);
			}
		}
		VisibleSurface->Lock();
		AlternateSurface->Lock();
		VisibleSurface->Blit_From(to, *AlternateSurface, from, false, true);
		if (animate_main_screen) {
			VisibleSurface->Blit_From(main_destination, *AlternateSurface, main_source, false, true);
		}
		AlternateSurface->Unlock();
		VisibleSurface->Unlock();
		FUN_00406F70();
		Sleep(30);
	}
	delete [] order;
	if (reverse) {
		VocClass::PlayGlobal(Rule->ShellButtonSlideSound, 1.0f, 0x2000);
		while (MouseCursor->Get_Mouse_State()) {
			MouseCursor->Show_Mouse();
		}
		SendMessageA(dialog, 0x4ec, 0, 0);
	} else {
		SendMessageA(dialog, 0x4ed, 0, 0);
	}
}

// FUNCTION: GAMEMD 0x00607FD0
void FUN_00607FD0(HWND dialog)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	if (data != NULL && data->HasFadeAnimation) {
		HWND item = GetDlgItem(dialog, 0x71a);
		if (item != NULL) {
			SendMessage(item, 0x4e2, 0, 0);
		}
		OwnerDraw::DrawCampaignMenuTransition(dialog, false);
		data->HasFadeAnimation = 0;
	}
}

bool __fastcall FUN_0055CBF0(void);
void __fastcall FUN_005D4D50(void);
bool __fastcall FUN_00602490(HWND parent, HWND child);
namespace OwnerDraw {
BOOL CALLBACK ClassifyLayoutBand(HWND window, LPARAM lparam);
BOOL CALLBACK ResetControlDrawModeAndTimerProc(HWND window, LPARAM lparam);
BOOL CALLBACK InitCompactDialogControlsProc(HWND window, LPARAM lparam);
}

// FUNCTION: GAMEMD 0x00608070
bool FUN_00608070(HWND dialog)
{
	if (Session.FUN_0069BBE0()) {
		return false;
	}
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	bool armed = data->HasOpenAnimation && data->LayoutBand == 1;
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
	data->HasFadeAnimation = 1;
	InvalidateRect(dialog, NULL, FALSE);
	DWORD start = GetTickCount();
	do {
		FUN_005D4D50();
		if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
			Call_Back();
		} else if (!FUN_0055CBF0() && Main_Loop()) {
			break;
		}
		int done = data->HasFadeAnimation == 0;
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

	if (Session.FUN_0069BBE0()) {
		return false;
	}
	bool armed;
	{
		HWND key = dialog;
		if (!OwnerDraw::Dialogs.getPointer(key, &data) || data == NULL) {
			return false;
		}
		armed = data->HasOpenAnimation && data->LayoutBand == 1;
	}
	bool visible = IsWindowVisible(dialog) != 0;
	if (!armed || !visible) {
		return false;
	}
	VocClass::PlayGlobal(Rule->GUIMoveInSound, 1.0f, 0x2000);
	bool wasEnabled = IsWindowEnabled(dialog) != 0;
	EnableWindow(dialog, FALSE);
	EnumChildWindows(dialog, FUN_00606800, 1);
	OwnerDraw::DrawCampaignMenuTransition(dialog, true);
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
		data->HasOpenAnimation = 1;
	}
}

// FUNCTION: GAMEMD 0x006083E0
void FUN_006083E0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		data->HasOpenAnimation = 0;
	}
}

// FUNCTION: GAMEMD 0x00608440
void FUN_00608440(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		data->Unknown_0D4 = 1;
	}
}

// FUNCTION: GAMEMD 0x006084A0
void FUN_006084A0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		data->Unknown_0D4 = 0;
	}
}

namespace Drawing { extern int RenderWidth; }
extern int DAT_007F5BE0;
extern int DAT_007F5BE4;
extern int WideScreenThreshold;

inline int Get_Layout_Dialog_Type(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	HWND key = dialog;

	OwnerDraw::Dialogs.getPointer(key, &data);
	if (data != NULL) {
		return data->DialogID;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00608500
bool __fastcall FUN_00608500(HWND parent, HWND child, Rect *out)
{
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
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x15);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x16);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x1ae, 0xbe, 0x4b, 0x14);
			}
		}
	} else if (type == 0xbd) {
		if (id == 0x72f) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x34, 0x4e, 0x18c, 0x8f);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x15);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x16);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x1ae, 0xbe, 0x4b, 0x14);
			}
		}
	} else if (type == 0xc2) {
		if (id == 0x72f) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x34, 0x4e, 0x18c, 0x8f);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x16);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x15);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x1ae, 0xbe, 0x4b, 0x14);
			}
		}
	} else if (type == 0xc9) {
		if (id == 0x72f) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x34, 0x4e, 0x18c, 0x8f);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x3c, 0x62, 0x21c, 0x8f);
			}
		} else if (id == 0x732) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x6e, 0xd7, 0x10a, 0x15);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xb4, 0xd7, 0x10a, 0x16);
			}
		} else if (id == 0x78e) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x50, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0xaa, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78d) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0xe4, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
				rect = new Rect(0x127, 0xbe, 0x4b, 0x14);
			}
		} else if (id == 0x78c) {
			if (Drawing::RenderWidth == DAT_007F5BE0) {
				rect = new Rect(0x15a, 0xbe, 0x4b, 0x14);
			} else if (Drawing::RenderWidth == DAT_007F5BE4 || Drawing::RenderWidth == WideScreenThreshold) {
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
		type = data->DialogID;
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
	if ((GetWindowLong(child, GWL_STYLE) & BS_OWNERDRAW) == BS_OWNERDRAW) {
		HWND key = child;
		OwnerDraw::WinData *data = NULL;
		OwnerDraw::Dialogs.getPointer(key, &data);
		if (data != NULL && data->ControlType == 0) {
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
		type = data->DialogID;
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
	if (Is_Plain_Control(child)) {
		return FUN_00609730(parent, child);
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609B30
bool FUN_00609B30(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
	int id = GetDlgCtrlID(control);
	if (Is_Owner_Draw_Message(type) && id == 0x695) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00609BC0
bool FUN_00609BC0(HWND parent, HWND child)
{
	if (Session.FUN_0069BBE0()) {
		return false;
	}
	if (FUN_006096A0(parent, child)) {
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
	if (!Session.FUN_0069BBE0()) {
		return false;
	}
	if (FUN_006096A0(parent, child)) {
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
		type = data->DialogID;
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
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
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
		if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL && data->ControlType == 0) {
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
		if (OwnerDraw::Dialogs.getPointer(key, &data) && data != NULL && data->ControlType == 0) {
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

namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060A330
// OwnerDraw::ClassifyLayoutBand
BOOL CALLBACK ClassifyLayoutBand(HWND window, LPARAM lparam)
{
	WinData *data = Find_Data(window);
	if (data != NULL) {
		HWND parent = GetParent(window);
		if (FUN_00609BC0(parent, window)) {
			data->LayoutBand = 1;
			return TRUE;
		}
		if (FUN_00609CF0(parent, window)) {
			data->LayoutBand = 2;
			return TRUE;
		}
		if (FUN_00609E20(parent, window)) {
			data->LayoutBand = 3;
		}
	}
	return TRUE;
}
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

namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060AAB0
// OwnerDraw::InitCompactDialogControlsProc
BOOL CALLBACK InitCompactDialogControlsProc(HWND window, LPARAM lparam)
{
	HWND parent = GetParent(window);
	WinData *data = Find_Data(window);
	if (data != NULL) {
		data->ExtraWidth = Control_Sound_Slot(parent, window);
		data->alpha = FUN_00603AA0(parent, window);
	}
	return TRUE;
}
}

// FUNCTION: GAMEMD 0x0060AF40
void __fastcall FUN_0060AF40(HWND dialog)
{
	EnumChildWindows(dialog, OwnerDraw::InitCompactDialogControlsProc, 0);
}

extern int DAT_007F5BE4;
extern int DAT_007F5BF0;

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
void __fastcall FUN_0060AF50(HWND window, Rect *rect)
{
	HWND parent = GetParent(window);
	bool compact = Session.FUN_0069BBE0();
	RECT frame;
	GetWindowRect(parent, &frame);
	int span = frame.right - frame.left;
	int dx = 0;
	if (!compact) {
		int hh = (span - DAT_007F5BE4) / 2;
		dx = (((span - DAT_007F5BE4) / 2) < 0) ? 0 : hh;
	}
	int spanY = frame.bottom - frame.top;
	int dy = 0;
	if (!compact) {
		int hv = (spanY - DAT_007F5BF0) / 2;
		dy = (((spanY - DAT_007F5BF0) / 2) < 0) ? 0 : hv;
	}
	MoveWindow(window, rect->X + dx, rect->Y + dy, rect->Width, rect->Height, FALSE);
	InvalidateRect(window, NULL, FALSE);
}

extern Rect *DAT_00B0FC28;
extern Rect *DAT_00B0FC50;
extern int SidebarWidth;

#include "msanim.h"
namespace OwnerDraw {
extern SHPStruct *SmallButtonAnimShape;
extern SHPStruct *SideButtonShape;
}
extern Rect *DAT_00B0FC24;
extern Rect *DAT_00B0FC4C;

extern Rect *DAT_00B0FC48;

// FUNCTION: GAMEMD 0x0060B000
void __fastcall FUN_0060B000(HWND window, Point2D const &size)
{
	HWND parent = GetParent(window);
	bool compact = Session.FUN_0069BBE0();
	RECT parent_rect;
	RECT rect;
	GetWindowRect(parent, &parent_rect);
	GetWindowRect(window, &rect);
	Rect area(parent_rect.left, parent_rect.top, parent_rect.right - parent_rect.left, parent_rect.bottom - parent_rect.top);
	int offset = 0;
	if (!compact) {
		offset = (area.Width - DAT_007F5BE4) / 2;
		offset = offset < 0 ? 0 : offset;
	}
	int frameHeight = parent_rect.bottom - parent_rect.top;
	int shift = 0;
	if (!compact) {
		shift = Centre_Y(frameHeight, compact);
		shift -= Centre_Y(size.Y, compact);
	}
	int x;
	int y;
	int width;
	int height;
	if (compact) {
		width = OwnerDraw::SideButtonShape->Get_Width();
		height = OwnerDraw::SideButtonShape->Get_Height();
		Rect *strip = DAT_00B0FC48;
		x = parent_rect.right - offset - parent_rect.left - 147;
		HWND dialog = GetParent(window);
		RECT parent2;
		RECT rect2;
		GetWindowRect(dialog, &parent2);
		GetWindowRect(window, &rect2);
		int top = rect2.top - parent2.top;
		y = ((rect2.bottom - parent2.top - top) / 2 - 198 + top) / 44 * height + strip->Y;
	} else {
		width = OwnerDraw::SmallButtonAnimShape->Get_Width();
		height = OwnerDraw::SmallButtonAnimShape->Get_Height();
		Rect *strip = DAT_00B0FC24;
		x = parent_rect.right - offset - parent_rect.left - 156;
		int top = strip->Y;
		int pos = rect.top - top - parent_rect.top + shift;
		int row = pos / strip->Height;
		int prod = strip->Height * row;
		int up = abs(strip->Height - pos + prod);
		int down = abs(pos - prod);
		y = (down < up ? row : row + 1) * strip->Height + top;
	}
	MoveWindow(window, x, y, width, height, FALSE);
}

static inline int Sidebar_Margin(HWND window, int width)
{
	OwnerDraw::WinData *data = Get_Data(window);
	if (data && data->ExtraWidth) {
		return data->ExtraWidth;
	}
	return (SidebarWidth - width) / 2;
}

// FUNCTION: GAMEMD 0x0060B1D0
void __fastcall FUN_0060B1D0(HWND window, Point2D const &size)
{
	HWND parent = GetParent(window);
	bool compact = Session.FUN_0069BBE0();
	RECT parent_rect;
	RECT rect;
	GetWindowRect(parent, &parent_rect);
	GetWindowRect(window, &rect);
	Rect area(parent_rect.left, parent_rect.top, parent_rect.right - parent_rect.left, parent_rect.bottom - parent_rect.top);
	int offset = 0;
	if (!compact) {
		int excess = (area.Width - DAT_007F5BE4) / 2;
		offset = excess < 0 ? 0 : excess;
	}
	int frameHeight = parent_rect.bottom - parent_rect.top;
	int shift = 0;
	if (!compact) {
		shift = MAX(0, (frameHeight - DAT_007F5BF0) / 2);
		shift -= MAX(0, (size.Y - DAT_007F5BF0) / 2);
	}
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	MoveWindow(window, parent_rect.right - Sidebar_Margin(window, width) - width - offset - parent_rect.left, rect.top - parent_rect.top + shift, width, height, FALSE);
}

// FUNCTION: GAMEMD 0x0060B350
void __fastcall FUN_0060B350(HWND window)
{
	HWND parent = GetParent(window);
	bool compact = Session.FUN_0069BBE0();
	RECT frame;
	RECT own;
	GetWindowRect(parent, &frame);
	GetWindowRect(window, &own);
	int span = frame.right - frame.left;
	int dx = 0;
	if (!compact) {
		int half = (span - DAT_007F5BE4) / 2;
		dx = (((span - DAT_007F5BE4) / 2) < 0) ? 0 : half;
	}
	int w;
	int h;
	int x;
	int y;
	if (compact) {
		goto compact_button;
	}
	w = OwnerDraw::SmallButtonAnimShape->Get_Width();
	h = OwnerDraw::SmallButtonAnimShape->Get_Height();
	x = frame.right - dx - frame.left - 156;
	{
		int top = DAT_00B0FC24->Y;
		int height = DAT_00B0FC24->Height;
		y = top + ((DAT_00B0FC28->Y - top) / height - 1) * height;
	}
move_button:
	MoveWindow(window, x, y, w, h, FALSE);
	return;
compact_button:
	w = OwnerDraw::SideButtonShape->Get_Width();
	h = OwnerDraw::SideButtonShape->Get_Height();
	x = frame.right - dx - frame.left - 147;
	y = DAT_00B0FC4C->Y - h;
	goto move_button;
}

// FUNCTION: GAMEMD 0x0060B420
void __fastcall FUN_0060B420(HWND window)
{
	HWND parent = GetParent(window);
	bool compact = Session.FUN_0069BBE0();
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
	int half = Sidebar_Margin(window, w);
	int x = client.right - half - w - dx - client.left;
	MoveWindow(window, x, compact ? DAT_00B0FC50->Y - h : DAT_00B0FC28->Y - h, w, h, FALSE);
}

// FUNCTION: GAMEMD 0x0060B550
void __fastcall FUN_0060B550(HWND window)
{
	HWND parent = GetParent(window);
	bool compact = Session.FUN_0069BBE0();
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
	bool compact = Session.FUN_0069BBE0();
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
	int half = Sidebar_Margin(window, w);
	int x = client.right - half - w - dx - client.left;
	MoveWindow(window, x, compact ? DAT_00B0FC50->Height + DAT_00B0FC50->Y - h : DAT_00B0FC28->Height + DAT_00B0FC28->Y - h, w, h, FALSE);
}

namespace OwnerDraw {
extern HWND CurrentDialogHwnd;
}

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
	if (Session.FUN_0069BBE0()) {
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
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
	GetDlgCtrlID(control);
	if (type == 0xb7 || type == 0x2b4 || type == 0xbbb || type == 0xf5 || type == 0x2b5 || type == 0xb8 ||
		type == 0xa3 || type == 0xb6 || type == 0x10c || type == 0x73 || type == 0xff || type == 0xea) {
		return true;
	}
	return false;
}

namespace Drawing {
extern int RenderWidth;
extern int RenderHeight;
}

void __fastcall FUN_0060B950(HWND window);

// FUNCTION: GAMEMD 0x0060C0C0
BOOL CALLBACK FUN_0060C0C0(HWND window, LPARAM lparam)
{
	HWND parent = GetParent(window);
	if (OwnerDraw::CurrentDialogHwnd == parent) {
		Rect rect;
		rect.X = 0;
		rect.Y = 0;
		rect.Width = 0;
		rect.Height = 0;
		if (FUN_00608500(parent, window, &rect)) {
			FUN_0060AF50(window, &rect);
		} else if (FUN_006096A0(parent, window)) {
			FUN_0060B000(window, *(Point2D const *)lparam);
		} else if (FUN_00608CD0(parent, window)) {
			FUN_0060B1D0(window, *(Point2D const *)lparam);
		} else if (FUN_00609AA0(parent, window)) {
			FUN_0060B350(window);
		} else if (FUN_00609730(parent, window)) {
			FUN_0060B420(window);
		} else if (FUN_00609B30(parent, window)) {
			FUN_0060B550(window);
		} else if (FUN_0060A100(parent, window)) {
			FUN_0060B610(window);
		} else if (FUN_0060B880(parent, window)) {
			FUN_0060B7A0(window);
		} else {
			FUN_0060B740(window);
		}
		FUN_0060B950(window);
	}
	return TRUE;
}

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
ConvertClass *__fastcall FUN_0060C4E0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return data->StaticShapeDrawer;
	}
	return 0;
}

namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060C540
// OwnerDraw::TrySetDialogLayoutBand1
bool __fastcall TrySetDialogLayoutBand1(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = data->DialogID;
	bool match = type == 0x10c || type == 0x94 || type == 0xb6 || type == 0xa3 || type == 0x73 ||
		type == 0xd8 || type == 0xbbb || type == 0xf5 || type == 0x105 || type == 0xd5 ||
		type == 0xe2 || type == 0x2b5 || type == 0xb7 || type == 0x2b4 || type == 0x101 ||
		type == 0x129 || type == 0xbc6 || type == 0xbc7 || type == 0x10b || type == 0xd7 ||
		type == 0x108 || type == 0xbc || type == 0xbd || type == 0xbb || type == 0xb5 ||
		type == 0xbba || type == 0xff || type == 0xea || type == 0x6b || type == 0x100 ||
		type == 0x102 || type == 0xb8 || type == 0xd6 || type == 0x103 || type == 0xd4 ||
		type == 0x125 || type == 0x122 || type == 0x112 || type == 0xe7 || type == 0x116 ||
		type == 0xc2 || type == 0xc9 || type == 0x113 || type == 0xfe || type == 0x10f ||
		type == 0x11d || type == 0x11c || type == 0x117 || type == 0x114 || type == 0xe6 ||
		type == 0xf3 || type == 0xf4 || type == 0x2bc || type == 0x10e || type == 0xfb;
	if (match) {
		data->LayoutBand = 1;
		data->HasOpenAnimation = true;
	}
	return match;
}
}

namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060C7D0
// OwnerDraw::TrySetDialogLayoutBand2
bool __fastcall TrySetDialogLayoutBand2(HWND dialog)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = data->DialogID;
	bool match = type == 0x10d || type == 0xd9 || type == 0xf0 || type == 0xce || type == 0x120 || type == 0x121 ||
		type == 0x115 || type == 0xd3 || type == 0xcf || type == 0x11f || type == 0xc3 || type == 0x11b ||
		type == 0xe1 || type == 0x11e || type == 0xc4 || type == 0x130 || type == 0xd0 || type == 0xfc ||
		type == 0x126;
	if (match) {
		data->LayoutBand = 2;
	}
	return match;
}
}

// FUNCTION: GAMEMD 0x0060C8D0
unsigned char __fastcall FUN_0060C8D0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return data->HasButtonAnimation;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060C930
bool __fastcall FUN_0060C930(HWND dialog)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	if (data == NULL) {
		return false;
	}
	data->HasButtonAnimation = Has_Button_Animation(data->DialogID);
	return data->HasButtonAnimation;
}

// FUNCTION: GAMEMD 0x0060C9D0
void __fastcall FUN_0060C9D0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		data->HasButtonAnimation = 1;
	}
}

// FUNCTION: GAMEMD 0x0060CA30
void __fastcall FUN_0060CA30(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		data->HasButtonAnimation = 0;
	}
}

// FUNCTION: GAMEMD 0x0060CA90
unsigned char __fastcall FUN_0060CA90(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return data->HasTopPanelAnimation;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CAF0
bool __fastcall FUN_0060CAF0(HWND dialog)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	if (data == NULL) {
		return false;
	}
	int type = data->DialogID;
	data->HasTopPanelAnimation = (type == 0xbc || type == 0xbd || type == 0x102 || type == 0xc2 || type == 0xc9 || type == 0xbc6 || type == 0x105 || type == 0x6b || type == 0x113);
	return data->HasTopPanelAnimation;
}

// FUNCTION: GAMEMD 0x0060CBA0
void __fastcall FUN_0060CBA0(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		data->HasTopPanelAnimation = 1;
	}
}

// FUNCTION: GAMEMD 0x0060CC00
void __fastcall FUN_0060CC00(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		data->HasTopPanelAnimation = 0;
	}
}

// FUNCTION: GAMEMD 0x0060CC60
unsigned char __fastcall FUN_0060CC60(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return data->HasMainScreenAnimation;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CCC0
bool __fastcall FUN_0060CCC0(HWND dialog)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	if (data == NULL) {
		return false;
	}
	data->HasMainScreenAnimation = Has_Main_Screen_Animation(data->DialogID);
	return data->HasMainScreenAnimation;
}

// FUNCTION: GAMEMD 0x0060CD50
unsigned char __fastcall FUN_0060CD50(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return data->FlagD8;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CDB0
bool __fastcall FUN_0060CDB0(HWND dialog)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	if (data == NULL) {
		return false;
	}
	data->FlagD8 = Has_Flag_D8(data->DialogID);
	return data->FlagD8;
}

// FUNCTION: GAMEMD 0x0060CE40
SHPStruct *__fastcall FUN_0060CE40(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return data->DialogBackground;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0060CEA0
SHPStruct *__fastcall FUN_0060CEA0(HWND dialog)
{
	OwnerDraw::WinData *data = NULL;
	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		return data->DialogBackgroundEx;
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
extern SHPStruct *DAT_00B0FB34;
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
		*(int *)(p + 0x74) = (int)FUN_0072D450();
		*(int *)(p + 0xe0) = (int)DAT_00B0FB34;
		*(int *)(p + 0xe4) = (int)DAT_00B0FB34;
	} else if (type == 0x108) {
		*(int *)(p + 0x74) = FUN_0072D820();
		*(int *)(p + 0xe0) = DAT_00B0FB1C;
		*(int *)(p + 0xe4) = DAT_00B0FB1C;
	} else if (type == 0xbc6) {
		*(int *)(p + 0x74) = (int)FUN_0072D450();
		*(int *)(p + 0xe0) = (int)DAT_00B0FB34;
		*(int *)(p + 0xe4) = (int)DAT_00B0FB34;
	} else if (type == 0xbc7) {
		*(int *)(p + 0x74) = (int)FUN_0072D450();
		*(int *)(p + 0xe0) = (int)DAT_00B0FB34;
		*(int *)(p + 0xe4) = (int)DAT_00B0FB34;
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
		*(int *)(p + 0x74) = (int)FUN_0072E280();
		*(int *)(p + 0xe0) = DAT_00B0FB50;
		*(int *)(p + 0xe4) = DAT_00B0FA04;
	}
}

// FUNCTION: GAMEMD 0x0060D2C0
void __fastcall FUN_0060D2C0(HWND dialog, int id)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	if (data != NULL) {
		data->DialogID = id;
	}
}

// FUNCTION: GAMEMD 0x0060D320
int __fastcall FUN_0060D320(HWND dialog)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		return data->DialogID;
	}
	return 0;
}

void __fastcall FUN_0052B9B0(HWND dialog);
namespace UI {
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall EndDialog(HWND window);
}

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
		Keyboard->Clear();
		if (focus) {
			FUN_0052B9B0(dialog);
		}
		while (result == 0) {
			FUN_005D4D50();
			if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
				Call_Back();
			} else if (!FUN_0055CBF0() && Main_Loop()) {
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
namespace OwnerDraw { LRESULT CALLBACK ComboDropWindowHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam); }
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
	wc.lpfnWndProc = OwnerDraw::ComboDropWindowHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ProgramInstance;
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
	wc.hInstance = ProgramInstance;
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
			HWND list = CreateWindowEx(0, "ListBox", NULL, flags, x, y, rect.right - rect.left, rect.bottom - rect.top, parent, NULL, ProgramInstance, NULL);
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

namespace OwnerDraw {
BOOL CALLBACK SetUserData2(HWND window, LPARAM lparam);
BOOL CALLBACK InitializeCtrl(HWND window, LPARAM lparam);

// FUNCTION: GAMEMD 0x0060F4B0
// OwnerDraw::PrepareDialogChildControls
bool __fastcall PrepareDialogChildControls(HWND dialog, int mode)
{
	UI::RegisterComboDropAndNewEditClasses();
	CurrentDialogHwnd = dialog;
	EnumChildWindows(dialog, SetUserData2, 1);
	SetUserData2(dialog, 1);
	EnumChildWindows(dialog, FUN_0060F320, 1);
	EnumChildWindows(dialog, InitializeCtrl, mode);
	InitializeCtrl(dialog, mode);
	EnumChildWindows(dialog, SetUserData2, 0);
	SetUserData2(dialog, 0);
	return true;
}
}


void __fastcall FUN_0053E3C0(HWND window);
void ODCacheImages(void);
namespace OwnerDraw {
LRESULT CALLBACK ScrollBarHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK ListBoxHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK ComboBoxHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK TrackBarHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK StaticHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK SysTabHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK OwnerDrawButtonHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK AutoRadioButtonHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK DefaultHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x0060F760
// OwnerDraw::SetUserData2
BOOL CALLBACK SetUserData2(HWND window, LPARAM lparam)
{
	WinData *data = NULL;
	if (!Dialogs.getPointer(window, &data)) {
		WinData temp;
		Dialogs.add(window, temp);
		Dialogs.getPointer(window, &data);
	}
	data->paintDisabled = lparam;
	return TRUE;
}


// FUNCTION: GAMEMD 0x0060F9A0
// OwnerDraw::InitializeCtrl
BOOL CALLBACK InitializeCtrl(HWND window, LPARAM lparam)
{
	char class_name[128];
	GetClassName(window, class_name, sizeof(class_name));
	LONG style = GetWindowLong(window, GWL_STYLE);
	RECT rect1;
	Get_Display_Rect(window, &rect1);
	RECT rect2;
	GetClientRect(window, &rect2);
	FUN_0053E3C0(window);
	Initialize();
	if (!Game::PCXInitialized) {
		ODInitMasks();
		ODCacheImages();
		Game::PCXInitialized = 1;
	}
	WNDPROC customProc = NULL;
	int controlType = 11;
	NewEditLink *link = (NewEditLink *)GetWindowLong(window, GWL_USERDATA);
	if (link) {
		strcpy(class_name, link->className);
	}
	if (strcmp(class_name, "ScrollBar") == 0) {
		customProc = ScrollBarHandler;
		controlType = 8;
	} else if (strcmp(class_name, "ListBox") == 0) {
		customProc = ListBoxHandler;
		controlType = 4;
	} else if (strcmp(class_name, "ComboBox") == 0) {
		customProc = ComboBoxHandler;
		controlType = 3;
	} else if (strcmp(class_name, TRACKBAR_CLASS) == 0) {
		customProc = TrackBarHandler;
		controlType = 7;
	} else if (strcmp(class_name, PROGRESS_CLASS) == 0) {
		customProc = ProgressHandler;
		controlType = 6;
	} else if (strcmp(class_name, "NewEdit") == 0) {
		customProc = NewEditHandler;
		controlType = 1;
	} else if (strcmp(class_name, "Edit") == 0) {
		customProc = EditHandler;
		controlType = 1;
	} else if (strcmp(class_name, "Static") == 0) {
		customProc = StaticHandler;
		controlType = 2;
	} else if (strcmp(class_name, "SysTabControl32") == 0) {
		customProc = SysTabHandler;
		controlType = 10;
	} else if (strcmp(class_name, "Button") == 0) {
		if ((style & BS_GROUPBOX) == BS_GROUPBOX) {
			customProc = GroupBoxButtonHandler;
			controlType = 0;
		} else if ((style & BS_OWNERDRAW) == BS_OWNERDRAW) {
			customProc = OwnerDrawButtonHandler;
			controlType = 0;
		} else if ((style & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX) {
			customProc = CheckBoxButtonHandler;
			controlType = 0;
		} else {
			if ((style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON) {
				customProc = AutoRadioButtonHandler;
			}
			controlType = 0;
		}
	} else if (strcmp(class_name, HOTKEY_CLASS) == 0) {
		customProc = HotkeyHandler;
		controlType = 9;
	} else {
		customProc = SysListViewHandler;
	}
	WNDPROC originalProc = (WNDPROC)SetWindowLong(window, GWL_WNDPROC, (LONG)DefaultHandler);
	if (!SubclassProcs.contains(window)) {
		SubclassProcs.add(window, customProc);
	}
	if (!DialogProcs.contains(window)) {
		DialogProcs.add(window, originalProc);
	}
	WinData *data = NULL;
	if (!Dialogs.getPointer(window, &data)) {
		WinData temp;
		Dialogs.add(window, temp);
		Dialogs.getPointer(window, &data);
	}
	data->userData = lparam;
	data->ControlType = controlType;
	if (link) {
		data->styleFlags = link->style;
	}
	WNDPROC proc = NULL;
	DialogProcs.getValue(window, proc);
	char text[2048];
	text[0] = 0;
	CallWindowProc(proc, window, WM_GETTEXT, sizeof(text), (LPARAM)text);
	if (text[0]) {
		data->Set_Text(StringTable::LoadString(text, NULL, "D:\\ra2mdpost\\ownrdraw.cpp", 0x1ce0));
	} else {
		data->Set_Text((wchar_t const *)NULL);
	}
	data->hasText = 0;
	SendMessage(window, OD_SUBCLASSED, 0, 0);
	return TRUE;
}
}

extern int DAT_00AC48D8;
void __fastcall ODDrawBevelDarken(Rect const & rect, Surface & surface, int xpos, int ypos);
namespace OwnerDraw {
int __fastcall PrintTextFixedLength(unsigned color, BitFont * font, Rect const & rect, wchar_t const * text, int length, int x_alignment, int y_alignment, Surface * surface, int unknown);
void DrawBeveledBorder(Surface & surface, Rect const & rect, int offset, int color);
}

void __fastcall FUN_0053E420(HWND window);
BOOL CALLBACK ODRemoveFromDict(HWND window, LPARAM lparam);

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

// FUNCTION: GAMEMD 0x00610360
BOOL __fastcall ODCleanupDicts(HWND window)
{
	EnumChildWindows(window, ODRemoveFromDict, 0);
	ODRemoveFromDict(window, 0);
	return TRUE;
}

// FUNCTION: GAMEMD 0x00610570
BOOL CALLBACK ODRemoveFromDict(HWND window, LPARAM lparam)
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
// _$E118
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
		backgd->Blit_From(drect, *VisibleSurface, ODTooltip.bounds, false, true);
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

	VisibleSurface->Lock();
	VisibleSurface->Fill_Rect(rect, color);
	VisibleSurface->Unlock();

	ODDrawBevelDarken(rect, *VisibleSurface, 8, 4);

	OwnerDraw::PrintTextFixedLength(OwnerDraw::PrimaryTextColor, 0, rect, ODTooltip.text, wcslen(ODTooltip.text), 1, 1, VisibleSurface, 0);
	rect.X += 1;
	rect.Y += 1;
	rect.Width -= 2;
	rect.Height -= 2;
	OwnerDraw::DrawBeveledBorder(*VisibleSurface, rect, 1, -1);

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
	VisibleSurface->Blit_From(drect, *ODTooltip.background, srect, false, true);
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
// _$E121
static ArrayList<HWND> ActiveWindows;

// SYNTHETIC: GAMEMD 0x00612930 SYMBOL
// _$E126
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

namespace OwnerDraw {
extern int CachedSurfaceCount;
void __fastcall BlendGradientRect(Rect const & rect, Surface & surface, int color, int amount);
void __fastcall CopyDimmedBackground(Rect const & rect, HWND hWnd, int alpha);
}

void __fastcall ODFillRectTrans(Rect * rect, Surface * surface, unsigned short color, int alpha);
namespace OwnerDraw {
int __fastcall DrawWideText(Surface * surface, wchar_t const * text, LTRBStruct * rect, BitFont * font, unsigned color, int style, int p5, int p6, int p7, int p8);
}

inline Rect Rect_From_RECT(RECT const & r)
{
	return Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}

int __fastcall FUN_006149B0(HWND window, wchar_t *out, int size, int *cursor);
void __fastcall FUN_00623880(Surface * surface, Rect * rect, wchar_t * text, int cursor, BitFont * font, unsigned color, int * scroll, int focused, int mask, int draw, int unknown);
namespace OwnerDraw {
void __fastcall DrawBeveledBorder(Surface & surface, Rect const & rect, int offset, int color);
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

// FUNCTION: GAMEMD 0x006137D0
// OwnerDraw::SysTabHandler
LRESULT CALLBACK OwnerDraw::SysTabHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	static char string1[64];
	static wchar_t string2[256];

	LRESULT result = 0;
	WNDPROC proc = NULL;
	WinData * data = NULL;
	Dialogs.getPointer(window, &data);
	DialogProcs.getValue(window, proc);

	if (message == WM_ERASEBKGND) {
		return(1);
	}

	if (message == WM_NCPAINT) {
		return(0);
	}

	if (message == OD_SUBCLASSED) {
		RECT rect;
		Get_Display_Rect(window, &rect);

		Surface * surf = SurfaceCache.GetSurface("tab_tlu.pcx");
		SendMessage(window, TCM_SETITEMSIZE, 0, MAKELPARAM(89, surf->Get_Height() - 1));

	}

	if (message == WM_PAINT) {
		RECT winrect;
		Get_Display_Rect(window, &winrect);

		TabCtrl_GetItemCount(window);

		RECT tabrect;
		TabCtrl_GetItemRect(window, 0, &tabrect);
		TabCtrl_GetItemRect(window, 0, &tabrect);

		winrect.top += (tabrect.bottom - tabrect.top) + 3;
		Rect dimrect;
		dimrect.X = winrect.left;
		dimrect.Y = winrect.top;
		dimrect.Width = winrect.right - winrect.left;
		dimrect.Height = winrect.bottom - winrect.top;

		CopyDimmedBackground(dimrect, window, 0);
		ValidateRect(window, NULL);

		dimrect.X = winrect.left;
		dimrect.Y = winrect.top;
		dimrect.Width = winrect.right - winrect.left;
		dimrect.Height = winrect.bottom - winrect.top;

		Surface * image = SurfaceCache.GetSurface("tab_fml.pcx");
		if (image) {
			image->Get_Height();
			int width = image->Get_Width();
			SurfaceCache.Draw(Rect(Point2D(winrect.left, winrect.top), width, winrect.bottom - winrect.top), *AlternateSurface, *image, 0, 0);
		}

		image = SurfaceCache.GetSurface("tab_fmr.pcx");
		if (image) {
			image->Get_Height();
			int width = image->Get_Width();
			SurfaceCache.Draw(Rect(winrect.right - width, winrect.top, width, winrect.bottom - winrect.top), *AlternateSurface, *image, 0, 0);
		}

		image = SurfaceCache.GetSurface("tab_ftm.pcx");
		if (image) {
			int height = image->Get_Height();
			image->Get_Width();
			SurfaceCache.Draw(Rect(Point2D(winrect.left, winrect.top), winrect.right - winrect.left, height), *AlternateSurface, *image, 0, 0);
		}

		image = SurfaceCache.GetSurface("tab_fbm.pcx");
		if (image) {
			int height = image->Get_Height();
			image->Get_Width();
			SurfaceCache.Draw(Rect(winrect.left, winrect.bottom - height, winrect.right - winrect.left, height), *AlternateSurface, *image, 0, 0);
		}

		image = SurfaceCache.GetSurface("tab_ftl.pcx");
		if (image) {
			int height = image->Get_Height();
			int width = image->Get_Width();
			AlternateSurface->Blit_From(Rect(Point2D(winrect.left, winrect.top), width, height), *image, Rect(0, 0, width, height));
		}

		image = SurfaceCache.GetSurface("tab_ftr.pcx");
		if (image) {
			int height = image->Get_Height();
			int width = image->Get_Width();
			AlternateSurface->Blit_From(Rect(winrect.right - width, winrect.top, width, height), *image, Rect(0, 0, width, height));
		}

		image = SurfaceCache.GetSurface("tab_fbl.pcx");
		if (image) {
			int height = image->Get_Height();
			int width = image->Get_Width();
			AlternateSurface->Blit_From(Rect(winrect.left, winrect.bottom - height, width, height), *image, Rect(0, 0, width, height));
		}

		image = SurfaceCache.GetSurface("tab_fbr.pcx");
		if (image) {
			int height = image->Get_Height();
			int width = image->Get_Width();
			AlternateSurface->Blit_From(Rect(Point2D(winrect.right - width, winrect.bottom - height), width, height), *image, Rect(0, 0, width, height));
		}

		int current = TabCtrl_GetCurSel(window);
		Get_Display_Rect(window, &winrect);
		int itab = current + 1;

		RECT itemrect;
		while (true) {
			int tab = itab;

			if (TabCtrl_GetItemRect(window, tab, &itemrect)) {
				TC_ITEM item;
				memset(&item, 0, sizeof(item));
				item.pszText = string1;
				item.cchTextMax = sizeof(string1);
				strcpy(string1, "Title");
				item.mask = TCIF_TEXT;
				TabCtrl_GetItem(window, tab, &item);

				char state = 'd';
				if (tab == current) {
					state = 'u';
				}

				dimrect.X = winrect.left + (itemrect.left - MIN(itemrect.left, 6));
				dimrect.Y = itemrect.top + winrect.top;
				dimrect.Width = itemrect.right + (MIN(itemrect.left, 6) - itemrect.left);
				dimrect.Height = itemrect.bottom - itemrect.top;

				char fname[80];
				Surface * tab_lu = SurfaceCache.GetSurface("tab_tlu.pcx");
				sprintf(fname, "tab_tm%c.pcx", state);
				image = SurfaceCache.GetSurface(fname);
				if (image) {
					int height = image->Get_Height();
					image->Get_Width();
					int width = dimrect.Width - 2 * tab_lu->Get_Width();
					int draw_y = dimrect.Y;
					int draw_x = dimrect.X + tab_lu->Get_Width();
					SurfaceCache.Draw(Rect(draw_x, draw_y, width, height), *AlternateSurface, *image, 0, 0);
				}

				sprintf(fname, "tab_tl%c.pcx", state);
				image = SurfaceCache.GetSurface(fname);
				if (image) {
					int height = image->Get_Height();
					int width = image->Get_Width();
					ODColorBits transparent;
					transparent.v = RGB(255, 0, 255);
					SurfaceCache.DrawTrans(Rect(Point2D(dimrect.X, dimrect.Y), width, height), *AlternateSurface, *image, DSurface::Build_Hicolor_Pixel(transparent.red, transparent.green, transparent.blue));
				}

				sprintf(fname, "tab_tr%c.pcx", state);
				image = SurfaceCache.GetSurface(fname);
				if (image) {
					int height = image->Get_Height();
					int width = image->Get_Width();
					Rect trrect;
					trrect.X = dimrect.X + dimrect.Width - width;
					trrect.Y = dimrect.Y;
					trrect.Width = width;
					trrect.Height = height;
					ODColorBits transparent;
					transparent.v = RGB(255, 0, 255);
					SurfaceCache.DrawTrans(trrect, *AlternateSurface, *image, DSurface::Build_Hicolor_Pixel(transparent.red, transparent.green, transparent.blue));
				}

				if (item.pszText) {
					strcpy(string1, item.pszText);
				}

				LTRBStruct text_rect;
				text_rect.Left = dimrect.X;
				text_rect.Right = dimrect.X + dimrect.Width;
				text_rect.Top = dimrect.Y + 6;
				text_rect.Bottom = dimrect.Y + dimrect.Height;

				COLORREF color = SelectedTabTextColor;
				if (itab == current) {
					color = PrimaryTextColor;
				}
				swprintf(string2, L"%hs", string1);
				DrawWideText(AlternateSurface, string2, &text_rect, data->Font, color, 5, 0xc, 0, 0, 0);

				ValidateRect(window, &itemrect);
				if (itab == current) {
					break;
				}

				itab++;
			} else {
				itab = 0;
			}
		}

		ValidateRect(window, NULL);
	} else if (proc) {
		result = CallWindowProc(proc, window, message, wparam, lparam);
	}
	return(result);
}

namespace OwnerDraw {
extern SHPStruct *CloseButtonShape;
ConvertClass *GetSideButtonConvert(void);
}
ConvertClass *FUN_0072B050(void);
extern RGBClass DAT_00B0F9FC;
extern RGBClass DAT_00B0FB14;
extern RGBClass DAT_00B0FB19;
extern RGBClass DAT_00B0FA94;

// FUNCTION: GAMEMD 0x00612B70
LRESULT CALLBACK OwnerDraw::OwnerDrawButtonHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	WNDPROC original = NULL;
	WNDPROC *found = NULL;
	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		original = *found;
	}

	RECT drect;
	memset(&drect, 0, sizeof(drect));
	Get_Display_Rect(window, &drect);

	Rect origin;
	origin.X = drect.left;
	origin.Y = drect.top;
	origin.Width = drect.right - drect.left;
	origin.Height = drect.bottom - drect.top;

	RECT crect;
	memset(&crect, 0, sizeof(crect));
	GetClientRect(window, &crect);

	OwnerDraw::WinData *data = Find_Data(window);
	if (data == NULL) {
		return CallWindowProc(original, window, message, wparam, lparam);
	}

	int state = data->Button.state;
	LONG style = GetWindowLong(window, GWL_STYLE);

	switch (message) {
	case WM_TIMER:
		data->ButtonAlternateFrame = !data->ButtonAlternateFrame;
		InvalidateRect(window, NULL, TRUE);
		break;

	case WM_USER + 220: {
		OwnerDraw::Dialogs.getPointer(window, &data);
		bool on = lparam == 1;
		if (on && !data->ButtonTimerActive) {
			data->ButtonTimerActive = true;
			SetTimer(window, 0, 1000, NULL);
		} else if (!on && data->ButtonTimerActive) {
			data->ButtonTimerActive = false;
			data->ButtonAlternateFrame = false;
			KillTimer(window, 0);
			InvalidateRect(window, NULL, TRUE);
		}
		break;
	}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if (data->skipDraw) {
			return 0;
		}
		VocClass::PlayGlobal(Rule->GUIMainButtonSound, 1.0f, 0x2000);
		break;

	case WM_ACTIVATE:
	case WM_KILLFOCUS:
	case WM_MOUSEACTIVATE:
		return 0;

	case WM_PAINT: {
		if (data->skipDraw) {
			ValidateRect(window, NULL);
			return 0;
		}

		Rect rect = origin;
		unsigned color = OwnerDraw::PrimaryTextColor;
		Surface *image;

		if (data->cachedSurface == NULL) {
			Surface *surface = new BSurface(crect.right + 1, crect.bottom + 1, 2);
			data->cachedSurface = surface;
			OwnerDraw::CachedSurfaceCount++;

			Rect dest(0, 0, crect.right + 1, crect.bottom + 1);
			Rect src(drect.left, drect.top, crect.right + 1, crect.bottom + 1);

			surface->Blit_From(dest, *AlternateSurface, src);
		}

		if (data->LayoutBand != 0) {
			ConvertClass *convert = NULL;
			ShapeSet const *shape = NULL;
			int frame = 0;

			if (data->LayoutBand == 1) {
				convert = OwnerDraw::GetSmallButtonAnimConvert();
				shape = (ShapeSet const *)OwnerDraw::SmallButtonAnimShape;
				int f = 2;
				if (state & 1) {
					f = 4;
				} else if (data->ButtonAlternateFrame) {
					f = 3;
				}
				frame = f;
			} else if (data->LayoutBand == 2) {
				convert = OwnerDraw::GetSideButtonConvert();
				shape = (ShapeSet const *)OwnerDraw::SideButtonShape;
				int f = 0;
				if (state & 1) {
					f = 1;
				} else if (data->ButtonAlternateFrame) {
					f = 2;
				}
				frame = f;
			} else if (data->LayoutBand == 3) {
				convert = FUN_0072B050();
				shape = (ShapeSet const *)OwnerDraw::CloseButtonShape;
				int f = 0;
				if (state & 1) {
					f = 1;
				} else if (data->ButtonAlternateFrame) {
					f = 2;
				}
				frame = f;
			}

			if (style & WS_DISABLED) {
				int pixel;
				if (Session.FUN_0069BBE0()) {
					int side = Scen->PlayerSideIndex;
					if (side == 0) {
						pixel = DSurface::Build_Hicolor_Pixel(DAT_00B0F9FC);
					} else if (side == 1) {
						pixel = DSurface::Build_Hicolor_Pixel(DAT_00B0FB14);
					} else {
						pixel = DSurface::Build_Hicolor_Pixel(DAT_00B0FB19);
					}
				} else {
					pixel = DSurface::Build_Hicolor_Pixel(DAT_00B0FA94);
				}
				RGBClass rgb = DSurface::Deconstruct_Hicolor_Pixel(pixel);
				color = PALETTERGB(rgb.Get_Red(), rgb.Get_Green(), rgb.Get_Blue());
			}

			Point2D point = FUN_0072A9C0(&rect);
			Rect window_rect = AlternateSurface->Get_Rect();
			if (convert && shape) {
				Draw_Shape(*AlternateSurface, *convert, shape, frame, point, window_rect, SHAPE_WIN_REL);
			}

		} else if (data->image != NULL) {
			image = data->image;
			if ((state & 1) && data->altImage != NULL) {
				image = data->altImage;
			}

			Rect src = rect;
			src.X = 0;
			src.Y = 0;
			AlternateSurface->Blit_From(rect, *image, src);

		} else {
			static char _prev_state = 'u';

			char updown = 'u';
			if (state & 1) {
				updown = 'd';
			}
			if (style & WS_DISABLED) {
				updown = 'u';
			} else if (updown == 'd' && _prev_state == 'u') {
				VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000);
			}
			_prev_state = updown;

			int widths[2] = {7, 7};
			int margins[2] = {10, 10};
			int heights[2] = {24, 30};

			int index = 0;
			for (unsigned int i = 0; i < 2; i++) {
				if (heights[i] > rect.Height && i != 0) {
					break;
				}
				index = i;
			}

			int height = heights[index];
			int w = widths[index];
			int margin = margins[index];

			if (data->cachedSurface != NULL) {
				Rect cachesrc(0, 0, crect.right + 1, crect.bottom + 1);
				Rect cachedest(drect.left, drect.top, crect.right + 1, crect.bottom + 1);
				AlternateSurface->Blit_From(cachedest, *data->cachedSurface, cachesrc);
				InvalidateRect(window, NULL, FALSE);
			}

			origin.Y += (rect.Height - height) / 2;
			if (state & 1) {
				origin.Y += 2;
			}

			Rect destrect;
			Rect sourcerect;
			char buffer[40];

			sprintf(buffer, "b%c%c_li%d.pcx", updown, 'e', height);
			image = SurfaceCache.GetSurface(buffer);
			origin.Height = image->Get_Height();
			destrect = origin;
			destrect.Width = w;
			sourcerect.Y = 0;
			sourcerect.X = 0;
			destrect.Height = height;
			sourcerect.Width = w;
			sourcerect.Height = height;
			AlternateSurface->Blit_From(destrect, *image, sourcerect);

			sprintf(buffer, "b%c%c_mi%d.pcx", updown, 'e', height);
			image = SurfaceCache.GetSurface(buffer);
			Rect midrect = origin;
			midrect.X += w;
			midrect.Width -= margin;
			midrect.Height = image->Get_Height();
			SurfaceCache.Draw(midrect, *AlternateSurface, *image, 0, 0);

			sprintf(buffer, "b%c%c_ri%d.pcx", updown, 'e', height);
			image = SurfaceCache.GetSurface(buffer);
			destrect = origin;
			destrect.X += origin.Width - margin;
			destrect.Width = margin;
			destrect.Height = image->Get_Height();
			sourcerect.Height = destrect.Height;
			sourcerect.Y = 0;
			sourcerect.X = 0;
			sourcerect.Width = destrect.Width;
			AlternateSurface->Blit_From(destrect, *image, sourcerect);
		}

		if (data->image == NULL && data->text) {
			RECT client;
			GetClientRect(window, &client);

			Rect text_rect(
				origin.X,
				origin.Y + 1,
				origin.Width + origin.X - 2,
				origin.Height + origin.Y);
			if (state & 1) {
				text_rect.X += 2;
				text_rect.Y += 4;
			}
			OwnerDraw::DrawWideText(AlternateSurface, data->text, (LTRBStruct *)&text_rect, data->Font, color, 5, 0xc, 0, 0, 0);
		}

		if (data->LayoutBand == 0 && (style & WS_DISABLED)) {
			ODFillRectTrans(&rect, AlternateSurface, 0, 128);
		}
		ValidateRect(window, NULL);
		return 0;
	}
	}

	return CallWindowProc(original, window, message, wparam, lparam);
}

static inline int Copy_Wide_Text(wchar_t *dest, wchar_t const *source, unsigned int capacity)
{
	wcsncpy(dest, source, capacity);
	dest[capacity - 1] = 0;
	return wcslen(dest);
}

// FUNCTION: GAMEMD 0x00614190
// OwnerDraw::EditHandler
LRESULT CALLBACK OwnerDraw::EditHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	WNDPROC proc = NULL;
	WNDPROC *found = NULL;
	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		proc = *found;
	}

	OwnerDraw::WinData *data = Find_Data(window);
	if (data == NULL) {
		return CallWindowProc(proc, window, message, wparam, lparam);
	}

	if (GetFocus() == window && !data->Edit.focusEnabled) {
		data->Edit.focusPending = 1;
		SetFocus(MainWindow);
	}

	LONG style = GetWindowLong(window, GWL_STYLE);
	char class_name[32];
	char narrow[0x800];
	wchar_t wide[0x800];

	if ((message == WM_KEYUP || message == WM_KEYDOWN) && wparam == VK_TAB) {
		return(0);
	}

	if (message == OD_SUBCLASSED) {
		RECT window_rect;
		GetWindowRect(window, &window_rect);

		RECT client_rect;
		GetClientRect(window, &client_rect);

		RECT parent_rect;
		GetWindowRect(GetParent(window), &parent_rect);

		MoveWindow(window, window_rect.left - parent_rect.left + 1, window_rect.top - parent_rect.top + 1, client_rect.right - 2, client_rect.bottom - 2, FALSE);

		if (GetFocus() == window) {
			data->Edit.focusPending = 1;
			SetFocus(MainWindow);
		}

		if (style & WS_TABSTOP) {
			data->Edit.hadTabStop = 1;
			style &= ~WS_TABSTOP;
			SetWindowLong(window, GWL_STYLE, style);
		}
		return(0);
	}

	else if (message == WM_SETFOCUS) {
		SendMessage(window, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
		if (!data->Edit.focusEnabled) {
			PostMessage(window, OD_REFOCUS, 0, 0);
		}
		goto invalidate_and_default;
	}

	else if (message == WM_USER + 181 || message == WM_USER + 179 || message == WM_GETTEXTLENGTH) {
		wide[0] = 0;
		FUN_006149B0(window, wide, 0x800, NULL);
		if (message == WM_GETTEXTLENGTH) {
			return(wcslen(wide));
		}
		if (message == WM_USER + 179) {
			return Copy_Wide_Text((wchar_t *)lparam, wide, wparam);
		}
		OwnerDraw::WideToCharString((char *)lparam, wide, wparam);
		((char *)lparam)[wparam - 1] = 0;
		return(strlen((char *)lparam));
	}

	else if (message == WM_USER + 180 || message == WM_USER + 178) {
		narrow[0] = 0;
		if (data->text != NULL) {
			WideCharToMultiByte(CP_ACP, 0, data->text, -1, narrow, sizeof(narrow), NULL, NULL);
		}
		return(CallWindowProc(proc, window, WM_SETTEXT, 0, (LPARAM)narrow));
	}

	else if (message == WM_CHAR) {
		if (wparam == VK_RETURN) {
			if (style & ES_MULTILINE) {
				WPARAM len = CallWindowProc(proc, window, WM_GETTEXTLENGTH, 0, 0) + 3;
				char * text = new char[len];
				CallWindowProc(proc, window, WM_GETTEXT, len, (LPARAM)text);
				strcat(text, "\r\n");
				CallWindowProc(proc, window, WM_SETTEXT, 0, (LPARAM)text);
				delete[] text;

				WPARAM command = (unsigned short)GetWindowLong(window, GWL_ID) | 0x5010000;
				SendMessage(GetParent(window), WM_COMMAND, command, (LPARAM)window);
				return(0);
			}
		} else if (wparam == VK_TAB) {
			HWND next = window;
			HWND parent = GetParent(next);
			HWND tab_item = GetNextDlgTabItem(parent, next, FALSE);
			if (tab_item != NULL) {
				next = tab_item;
			}
			SetFocus(next);
			return(0);
		}
		goto call_default;
	}

	else if (message == OD_ACTIVATE) {

		int focus_state = data->Edit.focusPending;
		data->Edit.focusEnabled = 1;
		if (focus_state) {
			SetFocus(window);
			data->Edit.focusPending = 0;
		}
		if (data->Edit.hadTabStop) {
			style |= WS_TABSTOP;
			SetWindowLong(window, GWL_STYLE, style);
		}
		return(0);
	}

	else {
		if (message == WM_PAINT || message == WM_ERASEBKGND) {
			RECT display_rect;
			Get_Display_Rect(window, &display_rect);

			GetClassName(GetParent(window), class_name, sizeof(class_name)/2);
			bool is_combo = strcmp(class_name, "ComboBox") == 0;

			RECT update_rect;
			if (message == WM_PAINT && GetUpdateRect(window, &update_rect, FALSE)) {
				update_rect.left += display_rect.left;
				update_rect.right += display_rect.left;
				update_rect.top += display_rect.top;
				update_rect.bottom += display_rect.top;
			}

			Rect draw_rect;
			draw_rect.X = display_rect.left;
			draw_rect.Y = display_rect.top;
			draw_rect.Width = display_rect.right - display_rect.left + 1;
			draw_rect.Height = display_rect.bottom - display_rect.top + 1;

			OwnerDraw::CopyDimmedBackground(draw_rect, window, data->alpha);
			if (!is_combo) {
				OwnerDraw::DrawBeveledBorder(*AlternateSurface, draw_rect, 2, -1);
			}

			static wchar_t _buffer[0x1400];
			int cursor = 0;
			FUN_006149B0(window, _buffer, 0x1400, &cursor);

			Rect text_rect;
			text_rect.X = display_rect.left;
			text_rect.Y = display_rect.top;
			text_rect.Width = display_rect.right - display_rect.left + 1;
			text_rect.Height = display_rect.bottom - display_rect.top + 1;

			LONG text_style = GetWindowLong(window, GWL_STYLE);
			FUN_00623880(AlternateSurface, &text_rect, _buffer, cursor, data->Font, OwnerDraw::PrimaryTextColor, &data->scrollStart, data->hasFocus, (text_style & ES_PASSWORD) != 0, 0, 0);

			ValidateRect(window, NULL);
		}

		if (message == WM_CONTEXTMENU) {
			return(1);
		}

		if (message == WM_MOUSEMOVE) {
			return(1);
		}

		if (message == WM_KEYDOWN || message == WM_KEYUP || message == WM_SYSKEYDOWN || message == WM_SYSKEYUP || message == WM_SYSCHAR || message == WM_SYSDEADCHAR || message == WM_KILLFOCUS || message == WM_LBUTTONDOWN) {
		invalidate_and_default:
			GetClassName(GetParent(window), class_name, sizeof(class_name));
			if (strcmp(class_name, "ComboBox") == 0) {
				InvalidateRect(GetParent(window), NULL, FALSE);
			}
			InvalidateRect(window, NULL, FALSE);
		}

	call_default:
		return(CallWindowProc(proc, window, message, wparam, lparam));
	}
	return(0);
}

// FUNCTION: GAMEMD 0x006149B0
int __fastcall FUN_006149B0(HWND window, wchar_t *out, int size, int *cursor)
{
	WNDPROC proc = NULL;
	WNDPROC *found = NULL;
	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		proc = *found;
	}
	char buffer[0x400];
	DWORD sel_start;
	DWORD sel_end;
	unsigned int sel = 0;
	buffer[0] = 0;
	CallWindowProc(proc, window, WM_GETTEXT, sizeof(buffer), (LPARAM)buffer);
	if (cursor) {
		sel = (unsigned int)CallWindowProc(proc, window, EM_GETSEL, (WPARAM)&sel_start, (LPARAM)&sel_end) >> 16;
	}
	out[0] = 0;
	MultiByteToWideChar(0, 0, buffer, -1, out, size);
	if (cursor) {
		*cursor = _mbsnccnt((unsigned char *)buffer, sel);
	}
	if (size > 0) {
		out[size - 1] = 0;
	}
	int length = wcslen(out);
	wchar_t *clean = new wchar_t[length + 3];
	memset(clean, 0, (length + 3) * 2);
	int kept = 0;
	int i = 0;
	for (; i < length; ++i) {
		if (out[i] != L'\r' && out[i] != L'\n') {
			clean[kept++] = out[i];
		}
	}
	if (i != kept) {
		wcscat(clean, L"\r\n");
	}
	wcscpy(out, clean);
	delete clean;
	return wcslen(out);
}

extern int DAT_00AC48C8;
namespace OwnerDraw { wchar_t ConvertIMECharToWide(WPARAM, LPARAM); }

// FUNCTION: GAMEMD 0x00614B30
LRESULT CALLBACK OwnerDraw::NewEditHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	WNDPROC proc = NULL;
	int changed = 0;
	int handled = 0;
	if (message == WM_GETDLGCODE) {
		return DLGC_WANTALLKEYS;
	}
	DialogProcs.getValue(window, proc);
	WinData *data = NULL;
	Dialogs.getPointer(window, &data);
	HWND parent = GetParent(window);

	if ((message == WM_KEYUP || message == WM_KEYDOWN) && wparam == VK_TAB) {
		if (message == WM_KEYDOWN) {
			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			SendMessage(parent, WM_USER + 219, shift, (LPARAM)window);
		}
		return 0;
	}
	if (message == OD_SUBCLASSED) {
		RECT window_rect;
		GetWindowRect(window, &window_rect);
		RECT client_rect;
		GetClientRect(window, &client_rect);
		RECT parent_rect;
		GetWindowRect(parent, &parent_rect);
		SetWindowPos(window, NULL, window_rect.left - parent_rect.left + 1,
		window_rect.top - parent_rect.top + 1, client_rect.right - 2,
		client_rect.bottom - 2, SWP_SHOWWINDOW);
		return 0;
	} else if (message == EM_LIMITTEXT) {
		data->textLimit = wparam;
		if (wparam) {
			data->editText->truncate(wparam);
		}
	} else if (message == WM_USER + 181 || message == WM_USER + 179 || message == WM_GETTEXTLENGTH) {
		if (message == WM_GETTEXTLENGTH) {
			return data->editText->length();
		}
		WideWstring *query_text = data->editText;
		if (message == WM_USER + 179) {
			wcsncpy((wchar_t *)lparam, query_text->get(), wparam);
			((wchar_t *)lparam)[wparam - 1] = 0;
			return wcslen((wchar_t *)lparam);
		}
		WideToCharString((char *)lparam, query_text->get(), wparam);
		((char *)lparam)[wparam - 1] = 0;
		return strlen((char *)lparam);
	} else {
		if (message == WM_USER + 180 || message == WM_USER + 178) {
			data->editText->clear();
			data->caretIndex = 0;
			data->scrollStart = 0;
			if (data->text != NULL) {
				data->editText->set(data->text);
			}
			if (data->textLimit) {
				data->editText->truncate((unsigned int)data->textLimit);
			}
			data->caretIndex = data->editText->length();
		}
		if (message == WM_KEYDOWN) {
			if (wparam == VK_RETURN) {
				SendMessage(parent, WM_USER + 218, 0, (LPARAM)window);
				if (data->styleFlags & ES_MULTILINE) {
					*data->editText += L"\r\n";
					WPARAM command = (unsigned short)GetWindowLong(window, GWL_ID) | 0x5010000;
					SendMessage(parent, WM_COMMAND, command, (LPARAM)window);
				}
				return 0;
			}
		} else if (message == WM_SETFOCUS) {
			data->caretBlink = 0;
			SetTimer(window, 0, 1000, NULL);
		} else if (message == WM_KILLFOCUS) {
			KillTimer(window, 0);
		} else if (message == WM_TIMER) {
			data->caretBlink ^= 1;
			InvalidateRect(window, NULL, FALSE);
		} else {
			if (message == WM_PAINT || message == WM_ERASEBKGND) {
				RECT display_rect;
				Get_Display_Rect(window, &display_rect);
				char paint_class[80];
				GetClassName(parent, paint_class, sizeof(paint_class));
				bool is_combo = strcmp(paint_class, "ComboBox") == 0;
				RECT update_rect;
				if (message == WM_PAINT && GetUpdateRect(window, &update_rect, FALSE)) {
					update_rect.left += display_rect.left;
					update_rect.right += display_rect.left;
					update_rect.top += display_rect.top;
					update_rect.bottom += display_rect.top;
				}
				Rect draw_rect;
				draw_rect.X = display_rect.left;
				draw_rect.Y = display_rect.top;
				draw_rect.Width = display_rect.right - display_rect.left + 1;
				draw_rect.Height = display_rect.bottom - display_rect.top + 1;
				CopyDimmedBackground(draw_rect, window, data->alpha);
				if (!is_combo) {
					DrawBeveledBorder(*AlternateSurface, draw_rect, 2, -1);
				}
				display_rect.left += 2;
				Rect text_rect;
				text_rect.X = display_rect.left;
				text_rect.Y = display_rect.top;
				text_rect.Width = display_rect.right - display_rect.left + 1;
				text_rect.Height = display_rect.bottom - display_rect.top + 1;
				DAT_00AC48C8 = data->caretBlink;
				FUN_00623880(AlternateSurface, &text_rect, data->editText->get(),
				data->caretIndex, data->Font, PrimaryTextColor,
				&data->scrollStart, data->hasFocus,
				(data->styleFlags & ES_PASSWORD) != 0, 0, 0);
				ValidateRect(window, NULL);
			}
			if (message == WM_CONTEXTMENU) {
				return 1;
			}
			if (message == WM_MOUSEMOVE) {
				return 1;
			}
		}
		if (message == WM_KEYDOWN || message == WM_SETFOCUS || message == WM_KILLFOCUS ||
		message == WM_KEYUP || message == WM_SYSKEYDOWN || message == WM_SYSKEYUP ||
		message == WM_SYSCHAR || message == WM_SYSDEADCHAR || message == WM_LBUTTONDOWN) {
			char class_name[32];
			GetClassName(parent, class_name, sizeof(class_name));
			if (strcmp(class_name, "ComboBox") == 0) {
				InvalidateRect(parent, NULL, FALSE);
			}
			InvalidateRect(window, NULL, FALSE);
		}
		wchar_t ch = 0;
		if (message == WM_CHAR) {
			if (wparam > 31) {
				ch = (wchar_t)wparam;
			}
		} else if (message == WM_KEYDOWN) {
			switch (wparam) {
			case VK_DELETE:
				if (data->caretIndex < (int)data->editText->length()) {
					data->editText->remove(data->caretIndex, 1);
					changed = 1;
				}
				handled = 1;
				break;
			case VK_BACK:
				if (data->caretIndex > 0) {
					data->editText->remove(--data->caretIndex, 1);
					changed = 1;
				}
				handled = 1;
				break;
			case VK_LEFT:
				if (data->caretIndex > 0) {
					--data->caretIndex;
					return 0;
				}
				handled = 1;
				break;
			case VK_RIGHT:
				if (data->caretIndex < (int)data->editText->length()) {
					++data->caretIndex;
					return 0;
				}
				handled = 1;
				break;
			case VK_END:
				data->caretIndex = data->editText->length();
				return 0;
			case VK_HOME:
				data->caretIndex = 0;
				return 0;
			default:
				break;
			}
		} else if (message == WM_IME_CHAR) {
			ch = ConvertIMECharToWide(wparam, lparam);
			handled = 1;
		} else if (message == WM_USER + 217) {
			ch = (wchar_t)wparam;
			handled = 1;
		}
		if (ch) {
			if ((!data->rejectChars || !wcschr(data->rejectChars, ch)) &&
			(!data->textLimit || (int)data->editText->length() < data->textLimit) &&
			ch > 31 && (!data->asciiOnly || ch < 256)) {
				data->editText->insert(ch, data->caretIndex++);
				changed = 1;
			}
			handled = 1;
		}
		if (changed) {
			int id = (unsigned short)GetWindowLong(window, GWL_ID);
			SendMessage(parent, WM_COMMAND, id | (EN_CHANGE << 16), (LPARAM)window);
			SendMessage(parent, WM_COMMAND, id | (EN_UPDATE << 16), (LPARAM)window);
		}
		if (handled) {
			return 0;
		}
	}
	return CallWindowProc(proc, window, message, wparam, lparam);
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
			unsigned char skinned = data->skinned;
			unsigned char variant = data->variant;
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
			AlternateSurface->Blit_From(drect, *image, srect, false, true);
			if (style & WS_DISABLED) {
				ODFillRectTrans(&drect, AlternateSurface, 0, OwnerDraw::DisabledOverlayAlpha);
			}
			trect.Left += 26;
			if (data->text) {
				int color = OwnerDraw::PrimaryTextColor;
				if (style & WS_DISABLED) {
					color = OwnerDraw::DisabledTextColor;
				}
				OwnerDraw::DrawWideText(AlternateSurface, data->text, (LTRBStruct *)&trect, data->Font, color, 4, 0xc, 0, 0, 0);
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
		unsigned char old = data->variant;
		data->skinned = value;
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
		unsigned char old = data->variant;
		data->variant = value;
		if (old != value) {
			InvalidateRect(window, NULL, FALSE);
		}
		break;
	}

	case WM_USER + 231:
		return data->variant;
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

namespace OwnerDraw {
LRESULT CALLBACK AutoRadioButtonHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}

// FUNCTION: GAMEMD 0x00616980
// OwnerDraw::AutoRadioButtonHandler
LRESULT CALLBACK OwnerDraw::AutoRadioButtonHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	OwnerDraw::WinData *data = Find_Data(window);

	RECT drect;
	memset(&drect, 0, sizeof(drect));
	Get_Display_Rect(window, &drect);

	Rect origin;
	origin.X = drect.left;
	origin.Y = drect.top;
	origin.Width = drect.right - drect.left;
	origin.Height = drect.bottom - drect.top;

	RECT crect;
	memset(&crect, 0, sizeof(crect));
	GetClientRect(window, &crect);

	int state = data->CheckBox.checkState;
	LONG style = GetWindowLong(window, GWL_STYLE);

	switch (message) {
	case WM_PAINT: {
		Rect rect = origin;
		unsigned color = OwnerDraw::PrimaryTextColor;
		Surface *image;

		if (data->cachedSurface == NULL) {
			Surface *surface = new BSurface(crect.right + 1, crect.bottom + 1, 2);
			data->cachedSurface = surface;
			OwnerDraw::CachedSurfaceCount++;

			Rect dest(0, 0, crect.right + 1, crect.bottom + 1);
			Rect src(drect.left, drect.top, crect.right + 1, crect.bottom + 1);

			surface->Blit_From(dest, *AlternateSurface, src);
		}

		if (data->image != NULL) {
			image = data->image;
			if ((state & 1) && data->altImage != NULL) {
				image = data->altImage;
			}

			Rect src = rect;
			src.X = 0;
			src.Y = 0;
			AlternateSurface->Blit_From(rect, *image, src);

		} else {
			char updown = 'u';
			if (state & 1) {
				updown = 'd';
			}
			if (style & WS_DISABLED) {
				updown = 'u';
			}

			int widths[2] = {7, 7};
			int margins[2] = {10, 10};
			int heights[2] = {24, 30};

			int index = 0;
			for (unsigned int i = 0; i < 2; i++) {
				if (heights[i] > rect.Height && i != 0) {
					break;
				}
				index = i;
			}

			int height = heights[index];
			int w = widths[index];
			int margin = margins[index];

			if (data->cachedSurface != NULL) {
				Rect cachesrc(0, 0, crect.right + 1, crect.bottom + 1);
				Rect cachedest(drect.left, drect.top, crect.right + 1, crect.bottom + 1);
				AlternateSurface->Blit_From(cachedest, *data->cachedSurface, cachesrc);
				InvalidateRect(window, NULL, FALSE);
			}

			origin.Y += (rect.Height - height) / 2;
			if (state & 1) {
				origin.Y += 2;
			}

			Rect destrect;
			Rect sourcerect;
			char buffer[40];

			sprintf(buffer, "b%c%c_li%d.pcx", updown, 'e', height);
			image = SurfaceCache.GetSurface(buffer);
			origin.Height = image->Get_Height();
			destrect = origin;
			destrect.Width = w;
			sourcerect.Width = w;
			sourcerect.Y = 0;
			sourcerect.X = 0;
			destrect.Height = height;
			sourcerect.Height = height;
			AlternateSurface->Blit_From(destrect, *image, sourcerect);

			sprintf(buffer, "b%c%c_mi%d.pcx", updown, 'e', height);
			image = SurfaceCache.GetSurface(buffer);
			Rect midrect = origin;
			midrect.X += w;
			midrect.Width -= margin;
			midrect.Height = image->Get_Height();
			SurfaceCache.Draw(midrect, *AlternateSurface, *image, 0, 0);

			sprintf(buffer, "b%c%c_ri%d.pcx", updown, 'e', height);
			image = SurfaceCache.GetSurface(buffer);
			destrect = origin;
			destrect.X += origin.Width - margin;
			destrect.Width = margin;
			destrect.Height = image->Get_Height();
			sourcerect.Height = destrect.Height;
			sourcerect.Y = 0;
			sourcerect.X = 0;
			sourcerect.Width = destrect.Width;
			AlternateSurface->Blit_From(destrect, *image, sourcerect);
		}

		if (data->image == NULL && data->text) {
			RECT client;
			GetClientRect(window, &client);

			Rect text_rect(
				origin.X,
				origin.Y + 1,
				origin.Width + origin.X - 2,
				origin.Height + origin.Y - 2);
			if (state & 1) {
				text_rect.X += 2;
				text_rect.Y += 4;
			}
			OwnerDraw::DrawWideText(AlternateSurface, data->text, (LTRBStruct *)&text_rect, data->Font, color, 5, 0xc, 0, 0, 0);
		}

		if (style & WS_DISABLED) {
			ODFillRectTrans(&rect, AlternateSurface, 0, 128);
		}
		ValidateRect(window, NULL);
		return 0;
	}

	case BM_GETCHECK:
		return data->CheckBox.checkState;

	case BM_SETCHECK:
		data->CheckBox.checkState = wparam;
		InvalidateRect(window, NULL, TRUE);
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if (data->CheckBox.checkState == 0) {
			data->CheckBox.checkState = 1;
			InvalidateRect(window, NULL, TRUE);
			VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000);
			break;
		}
		return 0;

	case WM_LBUTTONUP: {
		LockWindowUpdate(GetParent(window));
		WNDPROC proc = NULL;
		WNDPROC *found = NULL;
		if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
			proc = *found;
		}
		LRESULT result = CallWindowProc(proc, window, WM_LBUTTONUP, wparam, lparam);
		LockWindowUpdate(NULL);
		return result;
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
	}

	WNDPROC proc = NULL;
	WNDPROC *found = NULL;
	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		proc = *found;
	}
	return CallWindowProc(proc, window, message, wparam, lparam);
}

namespace OwnerDraw {
LRESULT CALLBACK ScrollBarHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
void ODDrawEdgeGlows(Surface & surface, Rect const & rect, BOOL raised, int count, int left_alpha, int top_alpha, int right_alpha, int bottom_alpha);
void ODDrawArrowBitmap(Surface * surface, Rect const & rect, BOOL upward, BOOL pressed, bool dark);

// FUNCTION: GAMEMD 0x0061C690
// OwnerDraw::ScrollBarHandler
LRESULT CALLBACK OwnerDraw::ScrollBarHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	RECT client_rect;
	RECT display_rect;
	GetClientRect(window, &client_rect);
	Get_Display_Rect(window, &display_rect);

	client_rect.bottom -= 2 * OwnerDraw::ControlInsetPx;
	client_rect.right -= 2 * OwnerDraw::ControlInsetPx;

	display_rect.left += OwnerDraw::ControlInsetPx;
	display_rect.right -= OwnerDraw::ControlInsetPx;
	display_rect.top += OwnerDraw::ControlInsetPx;
	display_rect.bottom -= OwnerDraw::ControlInsetPx;

	OwnerDraw::WinData *data = Find_Data(window);
	if (data == NULL) {
		return 0;
	}

	bool keep_parent_capture = false;
	if (data->ScrollBar.keepCapture) {
		keep_parent_capture = true;
	}
	int message_result = data->ScrollBar.result;
	int dragging = data->ScrollBar.dragging;
	int range = data->ScrollBar.range;
	int position = data->ScrollBar.position;
	int up_pressed = data->ScrollBar.upPressed;
	int down_pressed = data->ScrollBar.downPressed;

	if (!range) {
		range = 100;
	}

	int grip_top = 0;
	int grip_bottom = 0;
	int scroll_code = 0;

	int width = client_rect.right - client_rect.left;
	int travel = client_rect.bottom - client_rect.top - 44;
	int grip_height = (int)((double)travel - log((double)(range + 1)) * (double)travel * 0.2);
	if (grip_height <= 14) {
		grip_height = 14;
	}

	travel -= grip_height;
	if (travel <= 1) {
		travel = 1;
	}

	if (message < WM_USER || message == OD_REFRESHNOPAINT) {
		if (!dragging) {
			grip_top = position * travel / range + client_rect.top + 22;
			grip_bottom = grip_top + grip_height;
		} else {
			POINT cursor;
			GetCursorPos(&cursor);
			ScreenToClient(window, &cursor);

			grip_top = cursor.y - grip_height / 2;
			if (grip_top < 22) {
				grip_top = 22;
			}

			if (client_rect.bottom - grip_height - 22 < grip_top) {
				grip_top = client_rect.bottom - grip_height - 22;
			}

			grip_bottom = grip_top + grip_height;
			scroll_code = SB_THUMBTRACK;
			position = range * (grip_top - 22) / travel;
		}
	}

	switch (message) {
		case SBM_GETPOS:
			return position;

		case SBM_SETPOS:
			if ((int)wparam <= range && (int)wparam > 0) {
				position = (int)wparam;
			}
			break;

		case SBM_SETRANGE:
			range = (int)lparam;
			if (position > range) {
				position = range;
			}
			break;

		case SBM_SETSCROLLINFO: {
			SCROLLINFO * info = (SCROLLINFO *)lparam;
			range = info->nMax;
			position = info->nPos;
			break;
		}

		case WM_NCHITTEST:
		case WM_GETDLGCODE: {
			WNDPROC proc = NULL;
			WNDPROC *found = NULL;
			if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
				proc = *found;
			}
			return CallWindowProc(proc, window, message, wparam, lparam);
		}

		case WM_ERASEBKGND:
			return 0;

		case WM_PAINT: {
			if (data->skipDraw) {
				ValidateRect(window, NULL);
				break;
			}
			if (data->paintDisabled) {
				return 0;
			}

			Rect src_rect(0, 0, client_rect.right, client_rect.bottom);
			Rect full_rect(display_rect.left, display_rect.top, client_rect.right, client_rect.bottom);

			HWND parent = GetParent(window);
			OwnerDraw::WinData * parent_data = NULL;
			if (parent != NULL) {
				OwnerDraw::Dialogs.getPointer(parent, &parent_data);
			}

			RECT parent_display;
			Get_Display_Rect(parent, &parent_display);

			Rect source_rect = src_rect;
			if (parent_data != NULL && parent_data->cachedSurface != NULL) {
				source_rect.X = display_rect.left + source_rect.X - parent_display.left;
				source_rect.Y += display_rect.top - parent_display.top;
			}

			if (data->cachedSurface != NULL) {
				if (data->cachedSurface->Get_Width() != client_rect.right || data->cachedSurface->Get_Height() != client_rect.bottom) {
					delete data->cachedSurface;
					data->cachedSurface = NULL;
				}
			}

			if (data->cachedSurface == NULL) {
				Surface * background = new BSurface(client_rect.right, client_rect.bottom, 2);
				data->cachedSurface = background;
				OwnerDraw::CachedSurfaceCount++;

				if (parent_data != NULL && parent_data->cachedSurface != NULL) {
					background->Blit_From(src_rect, *parent_data->cachedSurface, source_rect);
				}

				int pixel_count = client_rect.right * client_rect.bottom;
				unsigned short * pixels = (unsigned short *)background->Lock();
				for (int i = 0; i < pixel_count; i++) {
					pixels[i] = OD_Blend_Color(pixels[i], 0xFFFF, data->alpha);
				}
				if (pixels != NULL) {
					background->Unlock();
				}
			}

			if (parent_data != NULL && parent_data->cachedSurface != NULL) {
				AlternateSurface->Blit_From(full_rect, *parent_data->cachedSurface, source_rect);
			}

			int color = ODColorToHiColor(OwnerDraw::DefaultBorderColor);
			if (data->disabled) {
				color = ODColorToHiColor(OwnerDraw::AltBorderColor);
			}
			OwnerDraw::DrawBeveledBorder(*AlternateSurface, full_rect, 2, color);

			Rect grip_rect(display_rect.left, grip_top + display_rect.top, client_rect.right, grip_bottom - grip_top);
			full_rect = grip_rect;
			src_rect = grip_rect;
			src_rect.X = 0;
			src_rect.Y = grip_top;

			Surface * grip_center;
			if (data->disabled) {
				grip_center = SurfaceCache.GetSurface("gsbgripm.pcx", NULL);
			} else {
				grip_center = SurfaceCache.GetSurface("sbgripm.pcx", NULL);
			}
			if (grip_center != NULL) {
				grip_rect.Width = grip_center->Get_Width();
			}
			SurfaceCache.Draw(grip_rect, *AlternateSurface, *grip_center, 0, 0);

			Rect grip_src(0, 0, grip_center->Get_Height(), grip_center->Get_Width());

			Surface * grip_top_surf;
			if (data->disabled) {
				grip_top_surf = SurfaceCache.GetSurface("gsbgript.pcx", NULL);
			} else {
				grip_top_surf = SurfaceCache.GetSurface("sbgript.pcx", NULL);
			}
			if (grip_top_surf != NULL) {
				full_rect.Height = grip_top_surf->Get_Height();
			}
			AlternateSurface->Blit_From(full_rect, *grip_top_surf, grip_src);

			Surface * grip_bottom_surf;
			if (data->disabled) {
				grip_bottom_surf = SurfaceCache.GetSurface("gsbgripb.pcx", NULL);
			} else {
				grip_bottom_surf = SurfaceCache.GetSurface("sbgripb.pcx", NULL);
			}
			if (grip_bottom_surf != NULL) {
				full_rect.Y = display_rect.top + grip_bottom - grip_bottom_surf->Get_Height();
			}
			AlternateSurface->Blit_From(full_rect, *grip_bottom_surf, grip_src);

			Rect up_rect(display_rect.left, display_rect.top, client_rect.right, 22);
			full_rect = up_rect;
			src_rect = up_rect;
			src_rect.X = 0;
			src_rect.Y = 0;
			AlternateSurface->Blit_From(full_rect, *data->cachedSurface, src_rect);
			ODDrawEdgeGlows(*AlternateSurface, up_rect, up_pressed == 0, 2, OwnerDraw::ScrollButtonBevelAlpha, OwnerDraw::ScrollButtonBevelAlpha, OwnerDraw::ScrollButtonBevelAlpha, OwnerDraw::ScrollButtonBevelAlpha);
			ODDrawArrowBitmap(AlternateSurface, up_rect, 1, up_pressed, data->disabled);

			Rect down_rect(display_rect.left, display_rect.bottom - 22, client_rect.right, 22);
			full_rect = down_rect;
			src_rect = down_rect;
			src_rect.X = 0;
			src_rect.Y = client_rect.bottom - 22;
			AlternateSurface->Blit_From(full_rect, *data->cachedSurface, src_rect);
			ODDrawEdgeGlows(*AlternateSurface, down_rect, down_pressed == 0, 2, OwnerDraw::ScrollButtonBevelAlpha, OwnerDraw::ScrollButtonBevelAlpha, OwnerDraw::ScrollButtonBevelAlpha, OwnerDraw::ScrollButtonBevelAlpha);
			ODDrawArrowBitmap(AlternateSurface, down_rect, 0, down_pressed, data->disabled);

			ValidateRect(window, NULL);
			break;
		}

		case WM_TIMER: {
			POINT cursor;
			GetCursorPos(&cursor);
			ScreenToClient(window, &cursor);

			up_pressed = 0;
			down_pressed = 0;

			if (message_result && cursor.x > client_rect.right - width) {
				if (cursor.y < 22) {
					if (!dragging) {
						up_pressed = 1;
					}
					if (position != 0) {
						scroll_code = SB_LINEUP;
						--position;
					}
				} else if (cursor.y > client_rect.bottom - 22) {
					if (!dragging) {
						down_pressed = 1;
					}
					if (position + 1 <= range) {
						scroll_code = SB_LINEDOWN;
						++position;
					}
				}
			}

			SetTimer(window, 0, 25, NULL);
			break;
		}

		case WM_MOUSEMOVE:
			if (dragging) {
				RECT rect = client_rect;
				rect.left = client_rect.right - width;
				InvalidateRect(window, &rect, FALSE);
			}

			if (wparam & MK_LBUTTON) {
				break;
			}

		case WM_LBUTTONUP:
			message_result = 0;
			dragging = 0;
			if (up_pressed || down_pressed) {
				InvalidateRect(window, NULL, FALSE);
			}
			up_pressed = 0;
			down_pressed = 0;
			KillTimer(window, 0);
			ReleaseCapture();
			if (keep_parent_capture) {
				SetCapture(data->ownerWindow);
			}
			scroll_code = SB_ENDSCROLL;
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK: {
			if (message == WM_LBUTTONDOWN) {
				message_result = 1;
				SetCapture(window);
				SetTimer(window, 0, 500, NULL);
			} else {
				message_result = 0;
				dragging = 0;
				KillTimer(window, 0);
				ReleaseCapture();
				if (keep_parent_capture) {
					SetCapture(data->ownerWindow);
				}
			}

			POINT click;
			click.x = LOWORD(lparam);
			click.y = HIWORD(lparam);
			int repeat = 1;
			if (message == WM_LBUTTONDBLCLK) {
				repeat = 2;
			}

			down_pressed = 0;
			up_pressed = 0;

			while (repeat > 0) {
				if (click.x > (client_rect.right - width)) {
					if (click.y < 22 && position) {
						up_pressed = 1;
						scroll_code = SB_LINEUP;
						--position;
					} else {
						bool skip_thumb_logic = false;
						if (click.y > client_rect.bottom - 22) {
							if (position + 1 <= range) {
								down_pressed = 1;
								scroll_code = SB_LINEDOWN;
								++position;
								skip_thumb_logic = true;
							}
						}

						if (!skip_thumb_logic) {
							if (click.y < grip_top || click.y >= grip_bottom) {
								grip_top = click.y - grip_height / 2;
								if (grip_top < 22) {
									grip_top = 22;
								}

								if (client_rect.bottom - grip_height - 22 < grip_top) {
									grip_top = client_rect.bottom - grip_height - 22;
								}

								grip_bottom = grip_top + grip_height;
								scroll_code = SB_THUMBTRACK;
								position = range * (grip_top - 22) / travel;
							} else if (message == WM_LBUTTONDOWN) {
								dragging = 1;
							}
						}
					}
				}
				--repeat;
			}
			break;
		}

		case WM_USER + 210:
			data->ScrollBar.keepCapture = (lparam != 0);
			break;

		case WM_USER + 241:
			data->disabled = (lparam == 1);
			break;
	}

	int send_notify = 0;
	if (position != data->ScrollBar.position || range != data->ScrollBar.range) {
		if (data->ownerWindow != NULL) {
			send_notify = 1;
		}
	}

	data->ScrollBar.result = message_result;
	data->ScrollBar.dragging = dragging;
	data->ScrollBar.range = range;
	data->ScrollBar.position = position;
	data->ScrollBar.upPressed = up_pressed;
	data->ScrollBar.downPressed = down_pressed;

	if (send_notify) {
		SendMessage(data->ownerWindow, WM_VSCROLL, MAKEWPARAM(scroll_code, (unsigned short)position), (LPARAM)window);
		InvalidateRect(window, NULL, FALSE);
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
				surf->Blit_From(sourcerect, *AlternateSurface, destrect);
			}

			AlternateSurface->Blit_From(destrect, *data->cachedSurface, sourcerect);
			int pos = (data->ProgressBar.position << 16) / (data->ProgressBar.maximum - data->ProgressBar.minimum);
			int color = 0x000000FF;
			color = ODColorToHiColor(color);
			OwnerDraw::BlendGradientRect(destrect, *AlternateSurface, color, pos);
			ValidateRect(window, NULL);
			break;
		}
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0061D950
// OwnerDraw::TrackBarHandler
LRESULT CALLBACK OwnerDraw::TrackBarHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	bool play_click = true;

	RECT client_rect;
	RECT display_rect;
	GetClientRect(window, &client_rect);
	Get_Display_Rect(window, &display_rect);
	int number_width = 50;

	OwnerDraw::WinData *data = Find_Data(window);
	if (data == NULL) {
		return 0;
	}

	int message_result = data->TrackBar.result;
	int dragging = data->TrackBar.dragging;
	int range = data->TrackBar.range;
	int value = data->TrackBar.value;
	int minimum = data->TrackBar.minimum;
	int maximum;
	int thumb_pos = data->TrackBar.thumbPos;
	int step = data->TrackBar.step;
	int show_numbers = data->TrackBar.showNumbers;

	if (!show_numbers) {
		number_width = 0;
	}

	int slider_width = client_rect.right - client_rect.left - number_width - 13;
	if (slider_width <= 1) {
		slider_width = 1;
	}

	if (!range) {
		WNDPROC proc = NULL;
		WNDPROC *found = NULL;
		if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
			proc = *found;
		}

		int range_max = CallWindowProc(proc, window, TBM_GETRANGEMAX, 0, 0);
		minimum = CallWindowProc(proc, window, TBM_GETRANGEMIN, 0, 0);
		range = range_max - minimum;
		value = CallWindowProc(proc, window, TBM_GETPOS, 0, 0) - minimum;
		thumb_pos = value * slider_width / range;
		if (!range) {
			range = 100;
		}

		data->TrackBar.thumbPos = thumb_pos;
		data->TrackBar.range = range;
		data->TrackBar.value = value;
		data->TrackBar.minimum = minimum;
		data->TrackBar.step = step;
		data->TrackBar.showNumbers = show_numbers;
	}

	if (!step) {
		number_width = 50;
		step = 1;
		show_numbers = 1;
	}

	LONG grip_left;
	LONG grip_right;
	if (!dragging) {
		grip_left = client_rect.left + thumb_pos + 1;
		grip_right = grip_left + 12;
	} else {
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(window, &point);

		int xpos = point.x - 6;
		if (xpos < 1) {
			xpos = 1;
		}
		if (client_rect.right - number_width - 12 < xpos) {
			xpos = client_rect.right - number_width - 12;
		}

		int idx = ((range + 1) * (xpos - 1)) / slider_width;
		if (idx >= range) {
			idx = range;
		}

		value = step * ((minimum + idx) / step) - minimum;
		thumb_pos = value * slider_width / range;
		grip_left = thumb_pos + 1;
		grip_right = grip_left + 12;
	}

	switch (message) {
		case WM_NCHITTEST:
		case WM_GETDLGCODE: {
			WNDPROC proc = NULL;
			WNDPROC *found = NULL;
			if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
				proc = *found;
			}
			return CallWindowProc(proc, window, message, wparam, lparam);
		}

		case WM_ENABLE:
			InvalidateRect(window, NULL, FALSE);
			break;

		case WM_PAINT: {
			RECT paint_rect;
			Get_Display_Rect(window, &paint_rect);
			Rect disp_rect(paint_rect.left, paint_rect.top, paint_rect.right - paint_rect.left, paint_rect.bottom - paint_rect.top);
			LONG style = GetWindowLong(window, GWL_STYLE);

			if (data->cachedSurface == NULL) {
				RECT disp_copy;
				RECT client_copy;
				Get_Display_Rect(window, &disp_copy);
				GetClientRect(window, &client_copy);

				Surface *surf = new BSurface(client_copy.right + 1, client_copy.bottom + 1, 2);
				data->cachedSurface = surf;
				OwnerDraw::CachedSurfaceCount++;

				Rect src_rect(0, 0, client_copy.right + 1, client_copy.bottom + 1);
				Rect dst_rect(disp_copy.left, disp_copy.top, client_copy.right + 1, client_copy.bottom + 1);
				surf->Blit_From(src_rect, *AlternateSurface, dst_rect);
				ODFillRectTrans(&src_rect, surf, 0, data->alpha);
			}

			if (data->cachedSurface != NULL) {
				RECT disp_copy;
				RECT client_copy;
				Get_Display_Rect(window, &disp_copy);
				GetClientRect(window, &client_copy);

				Rect src_rect(0, 0, client_copy.right + 1, client_copy.bottom + 1);
				Rect dst_rect(disp_copy.left, disp_copy.top, client_copy.right + 1, client_copy.bottom + 1);
				AlternateSurface->Blit_From(dst_rect, *data->cachedSurface, src_rect);
			}

			if (show_numbers) {
				{
					Surface *center = SurfaceCache.GetSurface("trofm.pcx", NULL);
					Rect center_rect(disp_rect.X + disp_rect.Width - number_width + 1, disp_rect.Y - 1, number_width, center->Get_Height());
					SurfaceCache.Draw(center_rect, *AlternateSurface, *center, 0, 0);
				}
				{
					Surface *left = SurfaceCache.GetSurface("trofl.pcx", NULL);
					Rect left_dst(disp_rect.X + disp_rect.Width - number_width + 1, disp_rect.Y - 1, left->Get_Width(), left->Get_Height());
					Rect left_src = left_dst;
					left_src.Y = 0;
					left_src.X = 0;
					AlternateSurface->Blit_From(left_dst, *left, left_src);
				}
				{
					Surface *right = SurfaceCache.GetSurface("trofr.pcx", NULL);
					Rect right_dst(disp_rect.X + disp_rect.Width - right->Get_Width() + 1, disp_rect.Y - 1, right->Get_Width(), right->Get_Height());
					Rect right_src = right_dst;
					right_src.Y = 0;
					right_src.X = 0;

					AlternateSurface->Blit_From(right_dst, *right, right_src);
				}
			}

			Rect grip_rect(paint_rect.left + grip_left, paint_rect.top, grip_right - grip_left, paint_rect.bottom - paint_rect.top);
			Surface *grip = SurfaceCache.GetSurface("trakgrip.pcx", NULL);
			Rect grip_src;
			grip_src.X = 0;
			grip_src.Y = 0;
			grip_src.Width = grip->Get_Width();
			grip_src.Height = grip->Get_Height();
			AlternateSurface->Blit_From(grip_rect, *grip, grip_src);

			int disabled = style & WS_DISABLED;
			if (disabled) {
				ODFillRectTrans(&grip_rect, AlternateSurface, 0, OwnerDraw::DisabledOverlayAlpha);
			}

			int frame_color = ODColorToHiColor(OwnerDraw::DefaultBorderColor);
			if (disabled) {
				frame_color = ODColorToHiColor(OwnerDraw::DisabledBorderColor);
			}

			Rect frame_rect = disp_rect;
			frame_rect.Width = disp_rect.Width - number_width;
			OwnerDraw::DrawBeveledBorder(*AlternateSurface, frame_rect, 2, frame_color);

			Rect number_rect = disp_rect;
			int inset = (show_numbers != 0) + 1;
			number_rect.X = inset - number_width + disp_rect.Width + disp_rect.X;
			number_rect.Width = number_width - inset;
			OwnerDraw::DrawBeveledBorder(*AlternateSurface, number_rect, 2, frame_color);

			if (show_numbers) {
				wchar_t buffer[32];


				swprintf(buffer, L"%d", step * ((minimum + value) / step));

				int text_color = OwnerDraw::PrimaryTextColor;
				if (disabled) {
					text_color = OwnerDraw::DisabledTextColor;
				}

				RECT text_rect = paint_rect;
				text_rect.left = paint_rect.right - 49;
				OwnerDraw::DrawWideText(AlternateSurface, buffer, (LTRBStruct *)&text_rect, data->Font, text_color, 5, 0xc, 0, 0, 0);
			}

			ValidateRect(window, NULL);
			break;
		}

		case WM_ERASEBKGND:
			return 0;

		case WM_MOUSEMOVE:
			if (dragging) {
				RECT rect = client_rect;
				InvalidateRect(window, &rect, FALSE);
			}
			if (wparam & MK_LBUTTON) {
				break;
			}

		case WM_LBUTTONUP:
			message_result = 0;
			dragging = 0;
			ReleaseCapture();
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK: {
			if (message == WM_LBUTTONDOWN) {
				message_result = 1;
				SetCapture(window);
			} else {
				message_result = 0;
				ReleaseCapture();
			}

			int xpos = LOWORD(lparam);
			int ypos = HIWORD(lparam);
			if (ypos > client_rect.bottom - 18) {
				if (xpos >= grip_left && xpos < grip_right) {
					if (message == WM_LBUTTONDOWN) {
						dragging = 1;
					}
				} else {
					int x = xpos - 6;
					if (x < 1) {
						x = 1;
					}
					if (client_rect.right - number_width - 12 < x) {
						x = client_rect.right - number_width - 12;
					}


					int idx = ((range + 1) * (x - 1)) / slider_width;
					if (idx >= range) {
						idx = range;
					}

					value = step * ((minimum + idx) / step) - minimum;
					thumb_pos = value * slider_width / range;
				}
			}
			break;
		}

		case TBM_GETPOS:
			return step * ((minimum + value) / step);

		case TBM_SETPOS:
			if (lparam - minimum <= range && lparam - minimum >= 0) {
				value = lparam - minimum;
			}
			play_click = false;
			thumb_pos = value * slider_width / range;
			break;

		case TBM_SETRANGE:
			minimum = LOWORD(lparam);
			maximum = HIWORD(lparam);
			range = maximum - minimum;
			if (value > range) {
				value = range;
			}
			if (value < minimum) {
				value = minimum;
			}
			play_click = false;
			thumb_pos = value * slider_width / range;
			break;

		case OD_SETTRACKSTEP:
			step = lparam;
			break;

		case OD_TRACKNUMBERS:
			show_numbers = lparam;
			break;

		case OD_TRACKSILENT:
			data->TrackBar.clickSuppress = (wparam == 0);
			break;
	}

	int changed = 0;
	if (value != data->TrackBar.value || range != data->TrackBar.range || minimum != data->TrackBar.minimum) {
		changed = 1;
	}

	data->TrackBar.result = message_result;
	data->TrackBar.dragging = dragging;
	data->TrackBar.thumbPos = thumb_pos;
	data->TrackBar.range = range;
	data->TrackBar.value = value;
	data->TrackBar.minimum = minimum;
	data->TrackBar.step = step;
	data->TrackBar.showNumbers = show_numbers;

	if (changed) {
		InvalidateRect(window, NULL, FALSE);

		HWND parent = GetParent(window);
		SendMessage(parent, WM_HSCROLL, MAKEWPARAM(TB_THUMBTRACK, (unsigned short)(minimum + value)), (LPARAM)window);

		if (play_click == true && !data->TrackBar.clickSuppress) {
			VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000);
		}
	}

	return 0;
}

namespace UI {
void __fastcall GetKeyboardKeyString(unsigned short key, wchar_t *buffer);
}
int __fastcall ODDrawTextBG(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags);
inline int Draw_Hotkey_Text0(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return ODDrawTextBG(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text1(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text0(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text2(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text1(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text3(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text2(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text4(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text3(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text5(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text4(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text6(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text5(surface, text, rect, font, color, flags); }
inline int Draw_Hotkey_Text7(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags) { return Draw_Hotkey_Text6(surface, text, rect, font, color, flags); }

static inline WNDPROC Find_Original_Proc(HWND &window)
{
	WNDPROC proc = NULL;
	OwnerDraw::DialogProcs.getValue(window, proc);
	return proc;
}

// FUNCTION: GAMEMD 0x0061E700
LRESULT CALLBACK OwnerDraw::GroupBoxButtonHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int i;

	switch (message) {
	case WM_PAINT: {
		int spacing = 0;
		WinData *data = Find_Data(window);

		if (data != NULL) {
			wchar_t const *text = data->text ? data->text : L"";

			RECT area;
			Get_Display_Rect(window, &area);

			int width = area.right - area.left;
			int middle_y = data->Font->field_1C / 2 + area.top;
			int text_width;
			int text_height;

			data->Font->GetTextDimension(text, &text_width, &text_height, width);

			Rect caption(area.left + 10, area.top, width, area.bottom - area.top);

			PrintTextFixedLength(PrimaryTextColor, NULL, caption, text, wcslen(text), 0, 0, AlternateSurface, 0);

			int shades[2] = {BevelLightColor, BevelShadowColor};
			int colors[2];
			colors[1] = ODColorToHiColor(shades[1]);
			colors[0] = ODColorToHiColor(shades[0]);
			i = 0;

			int middle_color = ((((shades[0] & 0xFF) + (shades[1] & 0xFF)) / 2) & 0xFF) +
			                   ((((shades[0] & 0xFF0000) + (shades[1] & 0xFF0000)) / 2) & 0xFF0000) +
			                   ((((shades[0] & 0xFF00) + (shades[1] & 0xFF00)) / 2) & 0xFF00);

			for (; i < 2; i++) {
				int color = colors[i];

				if (wcslen(text) == 0) {
					AlternateSurface->Draw_Line(Point2D(area.left + i, middle_y + i),
					                            Point2D(area.right - i - 1, middle_y + i), color);
				} else {
					AlternateSurface->Draw_Line(Point2D(area.left + i, middle_y + i),
					                            Point2D(area.left + 8, middle_y + i), color);

					if (text_width)
						spacing = 12;

					AlternateSurface->Draw_Line(Point2D(area.left + text_width + spacing, middle_y + i),
					                            Point2D(area.right - i - 1, middle_y + i), color);
				}

				AlternateSurface->Draw_Line(Point2D(area.left + i, middle_y + i + 1),
				                            Point2D(area.left + i, area.bottom - i - 1), color);

				color = colors[1 - i];

				AlternateSurface->Draw_Line(Point2D(area.left + i, area.bottom - i - 1),
				                            Point2D(area.right - i - 1, area.bottom - i - 1), color);

				AlternateSurface->Draw_Line(Point2D(area.right - i - 1, middle_y + i),
				                            Point2D(area.right - i - 1, area.bottom - i - 2), color);

				color = ODColorToHiColor(middle_color);

				AlternateSurface->Put_Pixel(Point2D(area.right - i - 1, middle_y + i), color);

				AlternateSurface->Put_Pixel(Point2D(area.left + i, area.bottom - i - 1), color);
			}

			ValidateRect(window, NULL);
		}
	}
	case WM_NCPAINT:
		return 0;

	case WM_ERASEBKGND:
		return 1;
	}

	WNDPROC proc = Find_Original_Proc(window);
	return CallWindowProc(proc, window, message, wparam, lparam);
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
				surf->Blit_From(win_rect, *AlternateSurface, alt_rect);
			}
			wchar_t text[64];
			int key = SendMessage(window, 0x402, 0, 0);
			UI::GetKeyboardKeyString(key, text);
			if (data->cachedSurface != NULL) {
				AlternateSurface->Blit_From(alt_rect, *data->cachedSurface, win_rect);
			}
			OwnerDraw::DrawBeveledBorder(*AlternateSurface, alt_rect, 2, -1);
			if (wcslen(text) > 0) {
				rect.left += 4;
				rect.top += 4;
				rect.right -= 4;
				rect.bottom -= 4;
				Draw_Hotkey_Text7(AlternateSurface, text, &rect, data->Font, OwnerDraw::PrimaryTextColor, DT_SINGLELINE | DT_VCENTER);
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

unsigned short ODRComponentMask = 0;
unsigned short ODGComponentMask = 0;
unsigned short ODBComponentMask = 0;

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

extern char const * DAT_00833654;

// FUNCTION: GAMEMD 0x0061F210
void ODCacheImages(void)
{
	SurfaceCache.CachePCX(DAT_00833654);
	SurfaceCache.CachePCX("gdii.pcx");
	SurfaceCache.CachePCX("nodi.pcx");
	SurfaceCache.CachePCX("rani.pcx");
	SurfaceCache.CachePCX("usai.pcx");
	SurfaceCache.CachePCX("japi.pcx");
	SurfaceCache.CachePCX("frai.pcx");
	SurfaceCache.CachePCX("geri.pcx");
	SurfaceCache.CachePCX("gbri.pcx");
	SurfaceCache.CachePCX("djbi.pcx");
	SurfaceCache.CachePCX("arbi.pcx");
	SurfaceCache.CachePCX("lati.pcx");
	SurfaceCache.CachePCX("rusi.pcx");
	SurfaceCache.CachePCX("obsi.pcx");
	SurfaceCache.CachePCX("yrii.pcx");
	SurfaceCache.CachePCX("arrow_uu.pcx");
	SurfaceCache.CachePCX("arrow_ud.pcx");
	SurfaceCache.CachePCX("arrow_du.pcx");
	SurfaceCache.CachePCX("arrow_dd.pcx");
	SurfaceCache.CachePCX("leftbar.pcx");
	SurfaceCache.CachePCX("rightbar.pcx");
	SurfaceCache.CachePCX("trakgrip.pcx");
	SurfaceCache.CachePCX("sbgript.pcx");
	SurfaceCache.CachePCX("sbgripm.pcx");
	SurfaceCache.CachePCX("sbgripb.pcx");
	SurfaceCache.CachePCX("gsbgript.pcx");
	SurfaceCache.CachePCX("gsbgripm.pcx");
	SurfaceCache.CachePCX("gsbgripb.pcx");
	SurfaceCache.CachePCX("bar_ll.pcx");
	SurfaceCache.CachePCX("bar_lr.pcx");
	SurfaceCache.CachePCX("bar_ul.pcx");
	SurfaceCache.CachePCX("bar_ur.pcx");
	SurfaceCache.CachePCX("dlgsysi.pcx", 1);
	SurfaceCache.CachePalettedPCX("dlgsysa.pcx");
	SurfaceCache.CachePCX("wouban.pcx");
	SurfaceCache.CachePCX("wodban.pcx");
	SurfaceCache.CachePCX("wouleave.pcx");
	SurfaceCache.CachePCX("wodleave.pcx");
	SurfaceCache.CachePCX("wousqlch.pcx");
	SurfaceCache.CachePCX("wodsqlch.pcx");
	SurfaceCache.CachePCX("woudcon.pcx");
	SurfaceCache.CachePCX("woddcon.pcx");
	SurfaceCache.CachePCX("woukick.pcx");
	SurfaceCache.CachePCX("wodkick.pcx");
	SurfaceCache.CachePCX("wouhelp.pcx");
	SurfaceCache.CachePCX("wodhelp.pcx");
	SurfaceCache.CachePCX("woufind.pcx");
	SurfaceCache.CachePCX("wodfind.pcx");
	SurfaceCache.CachePCX("wouopt.pcx");
	SurfaceCache.CachePCX("wodopt.pcx");
	SurfaceCache.CachePCX("woutrny.pcx");
	SurfaceCache.CachePCX("wodtrny.pcx");
	SurfaceCache.CachePCX("wouclan.pcx");
	SurfaceCache.CachePCX("wodclan.pcx");
	SurfaceCache.CachePCX("woufgame.pcx");
	SurfaceCache.CachePCX("wodfgame.pcx");
	SurfaceCache.CachePCX("wouact.pcx");
	SurfaceCache.CachePCX("wodact.pcx");
	SurfaceCache.CachePCX("wouref.pcx");
	SurfaceCache.CachePCX("wodref.pcx");
	SurfaceCache.CachePCX("tab_tlu.pcx");
	SurfaceCache.CachePCX("tab_tmu.pcx");
	SurfaceCache.CachePCX("tab_tru.pcx");
	SurfaceCache.CachePCX("tab_tld.pcx");
	SurfaceCache.CachePCX("tab_tmd.pcx");
	SurfaceCache.CachePCX("tab_trd.pcx");
	SurfaceCache.CachePCX("tab_ftl.pcx");
	SurfaceCache.CachePCX("tab_ftr.pcx");
	SurfaceCache.CachePCX("tab_ftm.pcx");
	SurfaceCache.CachePCX("tab_fbr.pcx");
	SurfaceCache.CachePCX("tab_fbl.pcx");
	SurfaceCache.CachePCX("tab_fbm.pcx");
	SurfaceCache.CachePCX("tab_fmr.pcx");
	SurfaceCache.CachePCX("tab_fml.pcx");
	SurfaceCache.CachePCX("woloper.pcx");
	SurfaceCache.CachePCX("wolsqlch.pcx");
	SurfaceCache.CachePCX("woltrny.pcx");
	SurfaceCache.CachePCX("woluser.pcx");
	SurfaceCache.CachePCX("wolvoice.pcx");
	SurfaceCache.CachePCX("wolpriv.pcx");
	SurfaceCache.CachePCX("wolacpt.pcx");
	SurfaceCache.CachePCX("wolhost.pcx");
	SurfaceCache.CachePCX("wolclan.pcx");
	SurfaceCache.CachePCX("wolob.pcx");
	SurfaceCache.CachePCX("wolnob.pcx");
	SurfaceCache.CachePCX("wolrslkob.pcx");
	SurfaceCache.CachePCX("wolrslknob.pcx");
	SurfaceCache.CachePCX("gt18.pcx");
	SurfaceCache.CachePCX("private.pcx");
	SurfaceCache.CachePCX("corporal.pcx");
	SurfaceCache.CachePCX("sergeant.pcx");
	SurfaceCache.CachePCX("lieutena.pcx");
	SurfaceCache.CachePCX("major.pcx");
	SurfaceCache.CachePCX("colonel.pcx");
	SurfaceCache.CachePCX("stargen.pcx");
	SurfaceCache.CachePCX("briggenr.pcx");
	SurfaceCache.CachePCX("general.pcx");
	SurfaceCache.CachePCX("comchief.pcx");
	SurfaceCache.CachePCX("cooperat.pcx");
	SurfaceCache.CachePCX("number0.pcx");
	SurfaceCache.CachePCX("number1.pcx");
	SurfaceCache.CachePCX("number2.pcx");
	SurfaceCache.CachePCX("number3.pcx");
	SurfaceCache.CachePCX("number4.pcx");
	SurfaceCache.CachePCX("number5.pcx");
	SurfaceCache.CachePCX("number6.pcx");
	SurfaceCache.CachePCX("number7.pcx");
	SurfaceCache.CachePCX("number8.pcx");
	SurfaceCache.CachePCX("number9.pcx");
	SurfaceCache.CachePCX("dnarrowp.pcx");
	SurfaceCache.CachePCX("uparrowp.pcx");
	SurfaceCache.CachePCX("dnarrowr.pcx");
	SurfaceCache.CachePCX("uparrowr.pcx");
	SurfaceCache.CachePCX("gdnarrowp.pcx");
	SurfaceCache.CachePCX("guparrowp.pcx");
	SurfaceCache.CachePCX("gdnarrowr.pcx");
	SurfaceCache.CachePCX("guparrowr.pcx");
	SurfaceCache.CachePCX("trofl.pcx");
	SurfaceCache.CachePCX("trofm.pcx");
	SurfaceCache.CachePCX("trofr.pcx");
	SurfaceCache.CachePCX("sb_psh_u.pcx");
	SurfaceCache.CachePCX("sb_psh_d.pcx");
	SurfaceCache.CachePCX("sb_rel_u.pcx");
	SurfaceCache.CachePCX("sb_rel_d.pcx");
	SurfaceCache.CachePCX("bst_chkd.pcx");
	SurfaceCache.CachePCX("bst_uchk.pcx");
	SurfaceCache.CachePCX("bst_chkg.pcx");
	SurfaceCache.CachePCX("bst_uckg.pcx");
	SurfaceCache.CachePCX("ccd_i.pcx");
	SurfaceCache.CachePCX("cce_i.pcx");
	SurfaceCache.CachePCX("cud_i.pcx");
	SurfaceCache.CachePCX("cue_i.pcx");
	SurfaceCache.CachePCX("cce_ir.pcx");
	SurfaceCache.CachePCX("cce_il.pcx");
	SurfaceCache.CachePCX("bue_li30.pcx");
	SurfaceCache.CachePCX("bue_mi30.pcx");
	SurfaceCache.CachePCX("bue_ri30.pcx");
	SurfaceCache.CachePCX("bde_li30.pcx");
	SurfaceCache.CachePCX("bde_mi30.pcx");
	SurfaceCache.CachePCX("bde_ri30.pcx");
	SurfaceCache.CachePCX("bud_li30.pcx");
	SurfaceCache.CachePCX("bud_mi30.pcx");
	SurfaceCache.CachePCX("bud_ri30.pcx");
	SurfaceCache.CachePCX("bue_li24.pcx");
	SurfaceCache.CachePCX("bue_mi24.pcx");
	SurfaceCache.CachePCX("bue_ri24.pcx");
	SurfaceCache.CachePCX("bde_li24.pcx");
	SurfaceCache.CachePCX("bde_mi24.pcx");
	SurfaceCache.CachePCX("bde_ri24.pcx");
	SurfaceCache.CachePCX("bud_li24.pcx");
	SurfaceCache.CachePCX("bud_mi24.pcx");
	SurfaceCache.CachePCX("bud_ri24.pcx");
	SurfaceCache.CachePCX("wolreslk.pcx");
	SurfaceCache.CachePCX("woluclan.pcx");
	SurfaceCache.CachePCX("woldclan.pcx");
	SurfaceCache.CachePCX("wolupriv.pcx");
	SurfaceCache.CachePCX("woldpriv.pcx");
	SurfaceCache.CachePCX("wolurslkob.pcx");
	SurfaceCache.CachePCX("woldrslkob.pcx");
	SurfaceCache.CachePCX("pingg.pcx");
	SurfaceCache.CachePCX("pingy.pcx");
	SurfaceCache.CachePCX("pingr.pcx");
	SurfaceCache.CachePCX("mpascrnlbar01.pcx");
	SurfaceCache.CachePCX("mpascrnlbar02.pcx");
	SurfaceCache.CachePCX("mpascrnlbar03.pcx");
	SurfaceCache.CachePCX("mpascrnlbar04.pcx");
	SurfaceCache.CachePCX("mpascrnlbar05.pcx");
	SurfaceCache.CachePCX("mpascrnlbar06.pcx");
	SurfaceCache.CachePCX("mpascrnlbar07.pcx");
	SurfaceCache.CachePCX("mpascrnlbar08.pcx");
	SurfaceCache.CachePCX("mpascrnlbar09.pcx");
	SurfaceCache.CachePCX("mpascrnlbar10.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar01.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar02.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar03.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar04.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar05.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar06.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar07.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar08.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar09.pcx");
	SurfaceCache.CachePCX("mpsscrnlbar10.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar01.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar02.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar03.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar04.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar05.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar06.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar07.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar08.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar09.pcx");
	SurfaceCache.CachePCX("mpyscrnlbar10.pcx");
}

bool ODDrawEdgeGlow(Surface & surf, Point2D const & start, Point2D const & end, int color, int steps);

// FUNCTION: GAMEMD 0x00620050
bool ODDrawEdgeGlow(Surface & surf, Point2D const & start, Point2D const & end, int color, int steps)
{
	Point2D startpoint = start;
	Point2D endpoint = end;

	if (startpoint.X > endpoint.X) {
		swap(startpoint, endpoint);
	}

	int bpp = surf.Bytes_Per_Pixel();
	void * buffer = surf.Lock(startpoint);
	if (buffer != NULL) {
		if (startpoint.Y == endpoint.Y) {
			if (bpp == 1) {
				memset(buffer, color, endpoint.X - startpoint.X + 1);
			} else {
				for (int i = 0; i <= endpoint.X - startpoint.X; i++) {
					*((unsigned short *)buffer + i) = OD_Blend_Color(*((unsigned short *)buffer + i), color, steps);
				}
			}
		} else if (startpoint.X == endpoint.X) {
			int pitch = startpoint.Y > endpoint.Y ? -surf.Stride() : surf.Stride();

			int dy = abs(endpoint.Y - startpoint.Y);
			for (int i = 0; i <= dy; i++) {
				if (bpp == 1) {
					*(unsigned char *)buffer = color;
				} else {
					*(unsigned short *)buffer = OD_Blend_Color(*(unsigned short *)buffer, color, steps);
				}
				buffer = (unsigned char *)buffer + pitch;
			}
		} else {
			int dx = endpoint.X - startpoint.X;
			int dy = endpoint.Y - startpoint.Y;
			int pitch = surf.Stride();
			if (dy < 0) {
				pitch = -pitch;
			}

			dy = abs(dy);
			int dx2 = 2 * dx;
			int dy2 = 2 * dy;

			if (dx > dy) {
				int delta = dy2 - dx;

				for (int i = 0; i < dx; i++) {
					if (bpp == 1) {
						*((unsigned char *)buffer + i) = color;
					} else {
						*((unsigned short *)buffer + i) = OD_Blend_Color(*((unsigned short *)buffer + i), color, steps);
					}

					if (delta > 0) {
						buffer = (unsigned char *)buffer + pitch;
						delta -= dx2;
					}

					delta += dy2;
				}
			} else {
				int delta = dx2 - dy;
				int k = 0;

				for (int i = 0; i < dy; i++) {
					if (bpp == 1) {
						*((unsigned char *)buffer + k) = color;
					} else {
						*((unsigned short *)buffer + k) = OD_Blend_Color(*((unsigned short *)buffer + k), color, steps);
					}

					if (delta > 0) {
						k++;
						delta -= dy2;
					}

					delta += dx2;
					buffer = (unsigned char *)buffer + pitch;
				}
			}
		}

		surf.Unlock();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006205D0
void ODDrawEdgeGlows(Surface & surface, Rect const & rect, BOOL raised, int count, int left_alpha, int top_alpha, int right_alpha, int bottom_alpha)
{
	int color2 = 0xFFFF;
	int color1 = 0;
	if (!raised) {
		color1 = 0xFFFF;
		color2 = 0;
	}
	for (int i = 0; i < count; i++) {
		Point2D end1;
		Point2D start1;
		Point2D end2;
		Point2D start2;
		Point2D end3;
		Point2D start3;
		Point2D end4;
		Point2D start4;
		end1.X = rect.Width - i + rect.X - 2;
		end1.Y = i + rect.Y;
		start1.Y = i + rect.Y;
		start1.X = i + rect.X;
		ODDrawEdgeGlow(surface, start1, end1, color2, top_alpha);
		end2.X = rect.Width - i + rect.X - 1;
		end2.Y = rect.Y - i + rect.Height - 1;
		start2.Y = end2.Y;
		start2.X = i + rect.X;
		ODDrawEdgeGlow(surface, start2, end2, color1, bottom_alpha);
		end3.X = rect.X + i;
		end3.Y = rect.Y - i + rect.Height - 1;
		start3.X = rect.X + i;
		start3.Y = rect.Y + i + 1;
		ODDrawEdgeGlow(surface, start3, end3, color2, left_alpha);
		end4.X = rect.X - i + rect.Width - 1;
		end4.Y = rect.Y - i + rect.Height - 2;
		start4.X = rect.X - i + rect.Width - 1;
		start4.Y = i + rect.Y;
		ODDrawEdgeGlow(surface, start4, end4, color1, right_alpha);
	}
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
	struct ColorBytes {
		unsigned int red : 8;
		unsigned int green : 8;
		unsigned int blue : 8;
		unsigned int a : 8;
	};

	return(DSurface::Build_Hicolor_Pixel(((ColorBytes *)&color)->red, ((ColorBytes *)&color)->green, ((ColorBytes *)&color)->blue));
}

namespace OwnerDraw
{
// FUNCTION: GAMEMD 0x006208F0
// OwnerDraw::DrawBeveledBorder
void DrawBeveledBorder(Surface & surf, Rect const & rect, int offset, int color)
{
	if (color == -1) {
		color = ODColorToHiColor(DefaultBorderColor);
	}
	int shades[2] = { BevelLightColor, BevelShadowColor };
	int pixels[2] = { ODColorToHiColor(shades[0]), ODColorToHiColor(shades[1]) };
	Rect work = rect;
	work.X -= offset;
	work.Y -= offset;
	work.Width += 2 * offset;
	work.Height += 2 * offset;

	for (int i = 0; i < offset; i++) {
		Point2D end1;
		Point2D start1;
		Point2D end2;
		Point2D start2;
		Point2D end3;
		Point2D start3;
		Point2D end4;

		if (offset == 2) color = pixels[i];

		end1.X = work.Width - i + work.X - 2;
		end1.Y = i + work.Y;
		start1.X = i + work.X;
		start1.Y = i + work.Y;
		surf.Draw_Line(start1, end1, color);

		end3.X = work.X + i;
		end3.Y = work.Y - i + work.Height - 1;
		start3.X = end3.X;
		start3.Y = work.Y + i + 1;
		surf.Draw_Line(start3, end3, color);

		if (offset == 2) color = pixels[offset - 1 - i];

		end2.X = work.Width - i + work.X - 1;
		start2.X = i + work.X;
		end2.Y = work.Y - i + work.Height - 1;
		start2.Y = end2.Y;
		surf.Draw_Line(start2, end2, color);

		end4.X = work.X - i + work.Width - 1;
		end4.Y = work.Y - i + work.Height - 2;
		surf.Draw_Line(Point2D(end4.X, i + work.Y), end4, color);

		if (offset == 2) {
			int middle =
				((((shades[0] & 0xFF00) + (shades[1] & 0xFF00)) / 2) & 0xFF00)
				+ ((((shades[0] & 0xFF0000) + (shades[1] & 0xFF0000)) / 2) & 0xFF0000)
				+ ((((shades[0] & 0xFF) + (shades[1] & 0xFF)) / 2) & 0xFF);

			color = ODColorToHiColor(middle);
			surf.Put_Pixel(Point2D(work.Width - i + work.X - 1, work.Y + i), color);
			surf.Put_Pixel(Point2D(work.X + i, work.Y - i + work.Height - 1), color);
		}
	}
}
}

namespace Drawing {
extern unsigned char RedShiftRight;
extern unsigned char GreenShiftRight;
extern unsigned char BlueShiftRight;
extern int RedShiftLeft;
extern int GreenShiftLeft;
extern int BlueShiftLeft;
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
int __fastcall ODDrawTextBG(Surface *surface, wchar_t const *text, RECT *rect, BitFont *font, unsigned color, int flags)
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

extern bool GameInFocus;

// FUNCTION: GAMEMD 0x006211D0
// OwnerDraw::PrintTextFixedLength
int __fastcall OwnerDraw::PrintTextFixedLength(unsigned color, BitFont * font, Rect const & rect, wchar_t const * text, int length, int x_alignment, int y_alignment, Surface * surface, int unknown)
{
	if (!GameInFocus) {
		return 0;
	}
	if (surface == NULL) {
		surface = AlternateSurface;
	}
	int pixel = DSurface::Build_Hicolor_Pixel(GetRValue(color), GetGValue(color), GetBValue(color));
	int x = rect.X;
	int y = rect.Y;
	int width = rect.Width;
	int height = rect.Height;
	if (font == NULL) {
		font = BitFont::Instance;
	}
	int tw;
	int th;
	font->GetTextDimension(text, &tw, &th, width);
	if (x_alignment == 1) {
		x += (width - tw + 1) / 2;
	} else if (x_alignment == 2) {
		x += -((tw + 1) / 2);
	} else if (x_alignment == 3) {
		x += -1 - tw;
	} else if (x_alignment == 4) {
		x += width - tw - 1;
	}
	if (y_alignment == 1) {
		y += (height - th + 1) / 2;
	} else if (y_alignment == 2) {
		y += -((th + 1) / 2);
	} else if (y_alignment == 3) {
		y += -1 - th;
	}
	LTRBStruct bounds;
	bounds.Left = rect.X;
	bounds.Top = rect.Y;
	bounds.Right = rect.X + width;
	bounds.Bottom = rect.Y + height;
	font->SetClipMode(true);
	font->SetBounds(&bounds);
	font->SetColor(pixel);
	return BitText::Instance->Print(font, surface, text, x, y, length, unknown) - x;
}

// FUNCTION: GAMEMD 0x006213A0
void OwnerDraw::Draw_Item(LPDRAWITEMSTRUCT drawit)
{
	if (VisibleSurface != NULL && AlternateSurface != NULL) {
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
			Surface * srcSurf = VisibleSurface;

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

	AlternateSurface->Blit_From(rect, *surface, src, false, true);
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

// FUNCTION: GAMEMD 0x006219D0
void __fastcall ODDrawBevelDarken(Rect const &rect, Surface &surface, int xpos, int ypos)
{
	unsigned short *surfptr = (unsigned short *)surface.Lock();

	if (surfptr != NULL) {
		int stride = surface.Stride() / 2;
		int y;

		for (y = rect.Y; y < rect.Y + rect.Height; ++y) {
			int index = y * stride + rect.X;

			for (int x = rect.X; x < rect.X + xpos; ++x) {
				unsigned short px = surfptr[index];

				surfptr[index] = ((px >> 1) & ((ODRComponentMask >> 1) & (unsigned int)ODRComponentMask)) |
				                 ((px >> 1) & ((ODGComponentMask >> 1) & ODGComponentMask)) |
				                 ((px >> 1) & (ODBComponentMask >> 1));

				index++;
			}
		}

		for (y = rect.Y; y < rect.Y + ypos; ++y) {
			int index = y * stride + rect.X + xpos;

			for (int x = rect.X + xpos; x < rect.Width + rect.X; ++x) {
				unsigned short px = surfptr[index];

				surfptr[index] = ((px >> 1) & ((ODRComponentMask >> 1) & (unsigned int)ODRComponentMask)) |
				                 ((px >> 1) & ((ODGComponentMask >> 1) & ODGComponentMask)) |
				                 ((px >> 1) & (ODBComponentMask >> 1));

				index++;
			}
		}

		surface.Unlock();
	}
}

// FUNCTION: GAMEMD 0x00621B80
void __fastcall ODFillRectTrans(Rect * rect, Surface * surface, unsigned short color, int alpha)
{
	unsigned short * pixels = (unsigned short *)surface->Lock();
	if (pixels) {
		int stride = surface->Stride() / 2;
		for (int y = rect->Y; y < rect->Y + rect->Height; ++y) {
			int index = y * stride + rect->X;
			for (int x = rect->X; x < rect->X + rect->Width; ++x) {
				pixels[index] = OD_Blend_Color(pixels[index], color, alpha);
				++index;
			}
		}
		surface->Unlock();
	}
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

class ConvertClass;
namespace OwnerDraw { void Paint(HWND window); }
extern ShapeSet const * DAT_00B0FC80;
extern ShapeSet const * DAT_00B0FC84;
extern ShapeSet const * DAT_00B0FC88;
extern ShapeSet const * DAT_00B0FC8C;
void __fastcall FUN_0072F540(Surface * surface, Rect * rect, int flag);
bool FUN_0072E260(void);
void __fastcall FUN_0072E450(Surface * surface, Rect * rect, bool flag);
void __fastcall FUN_0072E730(Surface * surface, Rect const * area, ConvertClass * convert, SHPStruct * shape800, SHPStruct * shape);
void __fastcall FUN_0072E8C0(Surface * surface, Rect * rect);
void __fastcall FUN_0072E860(Surface * surface, Rect * rect);
void __fastcall FUN_0072E920(Surface * surface, Rect * rect);
ConvertClass * FUN_0072AFF0(void);
ConvertClass * FUN_0072B010(void);
ConvertClass * FUN_0072B030(void);

// FUNCTION: GAMEMD 0x00621E90
void __fastcall OwnerDraw::Paint(HWND window)
{
	WinData * entry = Find_Data(window);
	if (entry == NULL) {
		return;
	}
	RECT rcClient;
	GetClientRect(window, &rcClient);

	RECT rcDisp;
	Get_Display_Rect(window, &rcDisp);

	Rect rFull;
	rFull.Set(rcDisp.left, rcDisp.top, rcDisp.right - rcDisp.left, rcDisp.bottom - rcDisp.top);
	if (rFull.Width <= rcClient.right) {
		rFull.Width = rcClient.right;
	}
	if (rFull.Height <= rcClient.bottom) {
		rFull.Height = rcClient.bottom;
	}

	if (entry->paintDisabled == 0) {
		Surface * surf = entry->cachedSurface;
		if (!surf) {
			surf = new BSurface(rcClient.right, rcClient.bottom, 2);
			entry->cachedSurface = surf;
			OwnerDraw::CachedSurfaceCount++;
		}

		int mode = entry->LayoutBand;
		if (mode == 1) {
			if (Session.FUN_0069BBE0()) {
				FUN_0072F540(surf, &rFull, 0);
			} else if (FUN_0072E260()) {
				FUN_0072E450(surf, &rFull, !entry->Unknown_0D4);

				ConvertClass *a = FUN_0060C4E0(window);
				SHPStruct *b = FUN_0060CE40(window);
				SHPStruct *c = FUN_0060CEA0(window);
				FUN_0072E730(surf, &rFull, a, b, c);

				if (entry->HasTopPanelAnimation) {
					FUN_0072E8C0(surf, &rFull);
				}
				if (entry->HasButtonAnimation) {
					FUN_0072E860(surf, &rFull);
				}
				if (FUN_0060CC60(window)) {
					FUN_0072E920(surf, &rFull);
				}
			}
		} else if (mode == 2) {
			Rect dst = rFull;
			dst.X = 0;
			dst.Y = 0;
			surf->Blit_From(dst, *AlternateSurface, rFull, false, true);

			ShapeSet const * shape;
			ConvertClass * palette;
			if (Session.FUN_0069BBE0()) {
				int theme = Scen->PlayerSideIndex;
				if (theme == 0) {
					shape = DAT_00B0FC84;
					palette = FUN_0072AFF0();
				} else if (theme == 1) {
					shape = DAT_00B0FC88;
					palette = FUN_0072AFF0();
				} else {
					shape = DAT_00B0FC8C;
					palette = FUN_0072B010();
				}
			} else {
				shape = DAT_00B0FC80;
				palette = FUN_0072B030();
			}
			Point2D position(0, 0);
			if (palette && shape) {
				Draw_Shape(*surf, *palette, shape, 0, position, dst, SHAPE_WIN_REL);
			}
		} else {
			Surface * back = SurfaceCache.GetSurface(DAT_00833654, NULL);

			Rect dst = rFull;
			Rect src = rFull;
			dst.X -= rcDisp.left;
			dst.Y -= rcDisp.top;

			int width = back->Get_Width();
			int height = back->Get_Height();
			if (Drawing::RenderWidth > width) {
				src.X += (Drawing::RenderWidth - width) / -2;
			}
			if (Drawing::RenderHeight > height) {
				src.Y += (Drawing::RenderHeight - height) / -2;
			}
			surf->Blit_From(dst, *back, src, false, true);
		}

		if (surf) {
			Rect srcOnXs = rFull;
			srcOnXs.X -= rcDisp.left;
			srcOnXs.Y -= rcDisp.top;
			AlternateSurface->Blit_From(rFull, *surf, srcOnXs, false, true);
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

	HWND dialog = CreateDialogIndirectParam(ProgramInstance, templ, MainWindow, proc, (LPARAM)&init);
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
	Keyboard->Clear();
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

				SetForegroundWindow(MainWindow);
				SetFocus(MainWindow);
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
BOOL CALLBACK SetUserData2(HWND window, LPARAM lparam);
BOOL CALLBACK InitializeCtrl(HWND window, LPARAM lparam);
}
BOOL CALLBACK FUN_0060F320(HWND edit, LPARAM lparam);
void __fastcall FUN_0060CF00(HWND dialog);
void __fastcall Resize_Dialogs(HWND window);
void __fastcall Center_Window_Within_Window(HWND window);
extern int DAT_007F5BE0;

extern int DAT_007F5BEC;
namespace UI { void __fastcall RegisterWindow(HWND window, int type); }

// FUNCTION: GAMEMD 0x00622820
// UI::RegisterWindow
void __fastcall UI::RegisterWindow(HWND window, int type)
{
	OwnerDraw::PrepareDialogChildControls(window, 0);
	Resize_Dialogs(window);
	FUN_0060D2C0(window, type);
	if (!Session.FUN_0069BBE0()) {
		FUN_0060CF00(window);
	}
	FUN_0060CAF0(window);
	FUN_0060C930(window);
	FUN_0060CCC0(window);
	FUN_0060CDB0(window);
	FUN_0060AF40(window);
	if (OwnerDraw::TrySetDialogLayoutBand1(window)) {
		int layout[2] = { DAT_007F5BE0, DAT_007F5BEC };
		OwnerDraw::UpdateControlPosition(window, (LPARAM)layout);
	} else {
		OwnerDraw::TrySetDialogLayoutBand2(window);
	}
	FUN_0060A5A0(window);
	FUN_0060AA50(window);
	Center_Window_Within_Window(window);
}


namespace UI { int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam); }

// FUNCTION: GAMEMD 0x00622B50
int __fastcall UI::StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_DESTROY:
			Remove_Modeless_Dialog(window);
			--_dialog_count;
			SetFocus(MainWindow);
			return 0;

		case WM_DRAWITEM:
			OwnerDraw::Draw_Item((LPDRAWITEMSTRUCT)lparam);
			return 1;

		case WM_PAINT: {
			OwnerDraw::WinData *data = Get_Data(window);
			if (data != NULL) {
				if (data->skipDraw) {
					ValidateRect(window, NULL);
					return 1;
				}
				OwnerDraw::Paint(window);
				FUN_00607FD0(window);
				ValidateRect(window, NULL);
			}
			return 0;
		}

		case WM_ERASEBKGND:
			return 1;

		case WM_INITDIALOG:
			++_dialog_count;
			if (lparam != 0) {
				UI::RegisterWindow(window, *(unsigned short *)lparam);
			} else {
				UI::RegisterWindow(window, 0);
			}
			SetFocus(window);
			return 0;

		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			return (int)GetStockObject(BLACK_BRUSH);

		case WM_NCHITTEST: {
			HWND tip = GetDlgItem(window, 0x695);
			if (tip != NULL) {
				int x = LOWORD(lparam);
				int y = HIWORD(lparam);
				RECT rect;
				GetWindowRect(window, &rect);
				POINT pt;
				pt.x = x - rect.left;
				pt.y = y - rect.top;
				WideWstring text;
				HWND child = ChildWindowFromPointEx(window, pt, CWP_SKIPINVISIBLE);
				if (child != NULL) {
					text = FUN_00603FC0(window, child, x, y);
					if (wcscmp(text.get(), DAT_00887734) == 0) {
						text = FUN_00603F60(window, child);
						if (wcscmp(text.get(), DAT_00887734) == 0) {
							text = FUN_00604060(window, child);
						}
					}
				}
				SendMessage(tip, 0x4b2, 0, (LPARAM)text.text());
			}
			return 0;
		}

		case 0x497:
			SendMessage(window, 0x4a9, (WPARAM)window, 1);
			return 0;

		case 0x4ec:
			FUN_0060AAA0(window);
			return 0;
	}
	return 0;
}

namespace UI { bool Updated(void); }

// FUNCTION: GAMEMD 0x00623120
// UI::Updated
bool UI::Updated(void)
{
	FUN_005D4D50();
	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
		Call_Back();
		return false;
	}
	if (!FUN_0055CBF0() && Main_Loop()) {
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

LRESULT CALLBACK Custom_Message_Box_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
namespace UI { int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam); }

// FUNCTION: GAMEMD 0x00623230
HWND OwnerDraw::Custom_Message_Box(wchar_t const * btn1txt, wchar_t const * btn2txt, bool * cancelled)
{
	HWND dlg = UI::BeginDialog(0xF0, (DLGPROC)Custom_Message_Box_Proc, 0);
	if (dlg != NULL) {
		SetWindowLong(dlg, 8, (LONG)cancelled);
		HWND label = GetDlgItem(dlg, 0x5B0);
		SendMessage(label, WM_USER + 178, 0, (LPARAM)btn1txt);
		if (btn2txt) {
			HWND handle = GetDlgItem(dlg, 0x5C0);
			SendMessage(handle, WM_USER + 178, 0, (LPARAM)btn2txt);
			EnableWindow(handle, TRUE);
			ShowWindow(handle, SW_SHOW);
		}
	}
	return dlg;
}

// FUNCTION: GAMEMD 0x006232B0
LRESULT CALLBACK Custom_Message_Box_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT res = UI::StandardWndProc(window, message, wparam, lparam);
	if (res == 0) {
		if (message == WM_COMMAND && wparam == 0x5C0) {
			bool * cancelled = (bool *)GetWindowLong(window, 8);
			if (cancelled) {
				Keyboard->Put(KN_ESC);
				*cancelled = true;
			}
		}
		return 0;
	}
	return res;
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
	ControlType = 0xb;
	Font = BitFont::Instance;
	editText = new WideWstring();
	caretIndex = 0;
	StaticLoopMovie = 0;
	StaticSoundIndex = -1;
}

// FUNCTION: GAMEMD 0x006233A0
// OwnerDraw::WinData::~WinData

// FUNCTION: GAMEMD 0x00623420
OwnerDraw::WinNode * OwnerDraw::WinData::Add_Node(int length)
{
	WinNode * node = (WinNode *)operator new(length * 2 + 0x12);
	if (node != NULL) {
		node->text = (wchar_t *)(node + 1);
		node->next = nodes;
		node->id = 0;
		nodes = node;
	}
	return node;
}

// FUNCTION: GAMEMD 0x00623460
void OwnerDraw::WinData::Remove_Node(WinNode * node)
{
	WinNode * prev = NULL;
	WinNode * cur = nodes;
	while (cur != NULL) {
		if (node == cur) {
			break;
		}
		prev = cur;
		cur = cur->next;
	}
	if (cur != NULL) {
		if (prev != NULL) {
			prev->next = cur->next;
		} else {
			nodes = cur->next;
		}
		delete cur;
	}
}

// FUNCTION: GAMEMD 0x006234B0
void OwnerDraw::WinData::Copy_Nodes(WinNode const * source)
{
	while (nodes != NULL) {
		Remove_Node(nodes);
	}
	while (source != NULL) {
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
	if (text != NULL) {
		operator delete(text);
		text = NULL;
	}
	if (str != NULL && *str != 0) {
		text = (wchar_t *)operator new(wcslen(str) * 2 + 2);
		wcscpy(text, str);
	}
}

// FUNCTION: GAMEMD 0x006235B0
void OwnerDraw::WinData::Set_Text(char const * str)
{
	if (text != NULL) {
		operator delete(text);
		text = NULL;
	}
	if (str != NULL && *str != 0) {
		text = new wchar_t[strlen(str) + 1];
		swprintf(text, L"%hs", str);
	}
}

// FUNCTION: GAMEMD 0x00623610
OwnerDraw::WinData & OwnerDraw::WinData::operator=(WinData const & that)
{
	WinNode * old_nodes = nodes;
	wchar_t * old_text = text;
	WideWstring * old_edit = editText;
	if (this == &that) {
		return *this;
	}
	memcpy(this, &that, sizeof(*this));
	text = old_text;
	editText = old_edit;
	nodes = old_nodes;
	Set_Text(that.text);
	Copy_Nodes(that.nodes);
	return *this;
}

// FUNCTION: GAMEMD 0x00623730
OwnerDraw::WinData::WinData(WinData const & that)
{
	WinNode * old_nodes = nodes;
	wchar_t * old_text = text;
	WideWstring * old_edit = editText;
	memcpy(this, &that, sizeof(*this));
	editText = old_edit;
	text = old_text;
	nodes = old_nodes;
	Set_Text(that.text);
	Copy_Nodes(that.nodes);
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

namespace OwnerDraw {
extern wchar_t IMECompositionString[];
extern int IMECompositionStringLength;
extern int IMECompositionCursorPos;
extern int IMEComposing;
void UpdateIMECompositionString(void);
}
extern int DAT_00AC48C8;

template<class T>
inline T Edit_Text_Maximum(T a, T b)
{
	return a < b ? b : a;
}

// FUNCTION: GAMEMD 0x00623880
void __fastcall FUN_00623880(
	Surface *surface, Rect *rect, wchar_t *text, int caret,
	BitFont *font, unsigned int color, int *scroll, int ime,
	int password, int frame, int unknown9)
{
	wchar_t buffer[0x800];
	int caret_x = -1;
	int composition_length = 0;
	int composing = 0;
	if (text == 0) {
		text = L"";
	}
	int text_length = wcslen(text);
	if (caret < 0 || caret > text_length) {
		caret = 0;
	}
	if (ime) {
		OwnerDraw::UpdateIMECompositionString();
		composition_length = OwnerDraw::IMECompositionStringLength;
		composing = OwnerDraw::IMEComposing;
	}
	buffer[0] = 0;
	wcsncpy(buffer, text, 0x800);
	if (composition_length) {
		wcsncpy(&buffer[caret], OwnerDraw::IMECompositionString,
				0x800 - caret);
		wcsncpy(&buffer[composition_length + caret], &text[caret],
				0x800 - composition_length - caret);
	}
	buffer[0x7FF] = 0;
	int length = wcslen(buffer);
	if (password && length > 0) {
		wchar_t *p = buffer;
		for (int i = length; i != 0; i--) {
			*p++ = '*';
		}
	}
	int cursor;
	if (composing || composition_length) {
		cursor = OwnerDraw::IMECompositionCursorPos + caret;
	} else {
		cursor = caret;
	}
	if (cursor < *scroll + 5) {
		if (cursor >= 5) {
			*scroll = cursor - 5;
		} else {
			*scroll = 0;
		}
	}
	while (*scroll < length) {
		if (font->FUN_00433F50(&buffer[*scroll],
							  rect->Width - 5, 0, 0) >= cursor - *scroll) {
			break;
		}
		(*scroll)++;
	}
	int before = caret - *scroll;
	int lead = Edit_Text_Maximum(before, 0);
	int composed = composition_length;
	if (before < 0) {
		before += composition_length;
		composed = Edit_Text_Maximum(before, 0);
	}
	int after = length - composition_length - caret;
	if (before < 0) {
		after += before;
	}
	if (after < 0) {
		after = 0;
	}
	text = &buffer[*scroll];
	if (frame) {
		Rect area(rect->X - 1, rect->Y - 1,
				  font->FUN_00433ED0(text, 0) + 5, rect->Height + 2);
		surface->Fill_Rect(area, 0);
	}
	if (lead) {
		int width = OwnerDraw::PrintTextFixedLength(
			color, font, *rect, text, lead, 0, 0, surface, unknown9);
		rect->X += width;
		rect->Width -= width;
		if (rect->Width < 0) {
			rect->Width = 0;
		}
		text += lead;
	}
	if (composed) {
		cursor -= *scroll + lead;
		int rest;
		if (cursor < 0 || cursor > composed) {
			rest = -1;
		} else {
			rest = composed - cursor;
			composed -= rest;
		}
		if (composed > 0) {
			int width = OwnerDraw::PrintTextFixedLength(
				OwnerDraw::ImeCompositionTextColor, font, *rect,
				text, composed, 0, 0, surface, unknown9);
			rect->X += width;
			rect->Width -= width;
			if (rect->Width < 0) {
				rect->Width = 0;
			}
			text += composed;
		}
		if (rest >= 0) {
			caret_x = rect->X;
			if (rest) {
				int width = OwnerDraw::PrintTextFixedLength(
					OwnerDraw::ImeCompositionTextColor, font, *rect,
					text, rest, 0, 0, surface, unknown9);
				rect->X += width;
				rect->Width -= width;
				if (rect->Width < 0) {
					rect->Width = 0;
				}
				text += rest;
			}
		}
	} else {
		caret_x = rect->X;
	}
	if (after) {
		int width = OwnerDraw::PrintTextFixedLength(
			color, font, *rect, text, after, 0, 0, surface, unknown9);
		rect->X += width;
		rect->Width -= width;
		if (rect->Width < 0) {
			rect->Width = 0;
		}
	}
	if (ime && caret_x > 0 && !DAT_00AC48C8) {
		ODDrawEdgeGlow(
			*surface, Point2D(caret_x, rect->Y),
			Point2D(caret_x, rect->Y + rect->Height - 2),
			OwnerDraw::CaretColor, 0xFF);
		ODDrawEdgeGlow(
			*surface, Point2D(caret_x + 1, rect->Y),
			Point2D(caret_x + 1, rect->Y + rect->Height - 2),
			OwnerDraw::CaretColor, 0xFF);
	}
	DAT_00AC48C8 = 0;
}

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
		Keyboard->Clear();
		while (result < 0) {
			FUN_005D4D50();
			if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH && !Session.NetOpen && !Session.Suspended) {
				if (!FUN_0055CBF0() && Main_Loop()) {
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

// FUNCTION: GAMEMD 0x00624080
OwnerDraw::DialogControl::DialogControl(void)
{
}

// FUNCTION: GAMEMD 0x00624090
OwnerDraw::DialogControl::~DialogControl(void)
{
}

// FUNCTION: GAMEMD 0x006240A0
void CALLBACK FUN_006240A0(HWND dialog, int unused)
{
	OwnerDraw::WinData *data = Get_Data(dialog);
	if (data != NULL) {
		data->Unknown_0D4 = 1;
	}
}

// FUNCTION: GAMEMD 0x00624110
OwnerDraw::SimpleDialogControl::SimpleDialogControl(void)
	: Field4(0), Field8(false)
{
}

// FUNCTION: GAMEMD 0x00624130
OwnerDraw::SimpleDialogControl::~SimpleDialogControl(void)
{
}

// FUNCTION: GAMEMD 0x00624140
void OwnerDraw::SimpleDialogControl::F1(HWND window, int value)
{
	Field4 = value;
	OwnerDraw::WinData *data = Get_Data(window);
	if (data != NULL) {
		data->Unknown_0D4 = 1;
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
	Keyboard->Clear();
	while (!control->F2() && !control->F4()) {
		FUN_005D4D50();
		if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Session.NetOpen || Session.Suspended) {
			Call_Back();
		} else if (!FUN_0055CBF0() && Main_Loop()) {
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
			return data->ControlType == 1;
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

// TEMPLATE: GAMEMD 0x00624530
// ?add@?$Dictionary@PAUHWND__@@UWinData@OwnerDraw@@@@QAEDAAPAUHWND__@@AAUWinData@OwnerDraw@@@Z
template class Dictionary<HWND, OwnerDraw::WinData>;
template class Dictionary<HWND, WNDPROC>;

// TEMPLATE: GAMEMD 0x00624760
// ?getPointer@?$Dictionary@PAUHWND__@@UWinData@OwnerDraw@@@@QAEDAAPAUHWND__@@PAPAUWinData@OwnerDraw@@@Z

// FUNCTION: GAMEMD 0x006247D0
// ??0CellData@OwnerDraw@@QAE@ABU01@@Z

// TEMPLATE: GAMEMD 0x00624810
// ?growVector@?$ArrayList@UCellData@OwnerDraw@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x00624930
// ?getPointer@?$Dictionary@PAUHWND__@@P6GJPAU1@IIJ@Z@@QAEDAAPAUHWND__@@PAPAP6GJPAU2@IIJ@Z@Z

// TEMPLATE: GAMEMD 0x006249A0
// ?remove@?$Dictionary@PAUHWND__@@P6GJPAU1@IIJ@Z@@QAEDAAPAUHWND__@@AAP6GJPAU2@IIJ@Z@Z

// TEMPLATE: GAMEMD 0x00624BE0
// ?expand@?$Dictionary@PAUHWND__@@P6GJPAU1@IIJ@Z@@AAEXXZ

// TEMPLATE: GAMEMD 0x00624C80
// ?keyHash@?$Dictionary@PAUHWND__@@P6GJPAU1@IIJ@Z@@AAEIAAPAUHWND__@@@Z

// TEMPLATE: GAMEMD 0x00624CA0
// ?remove@?$Dictionary@PAUHWND__@@UWinData@OwnerDraw@@@@QAEDAAPAUHWND__@@AAUWinData@OwnerDraw@@@Z

// TEMPLATE: GAMEMD 0x00624FC0
// ?expand@?$Dictionary@PAUHWND__@@UWinData@OwnerDraw@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x00625070
// ?keyHash@?$Dictionary@PAUHWND__@@UWinData@OwnerDraw@@@@AAEIAAPAUHWND__@@@Z

// TEMPLATE: GAMEMD 0x00625090
// ?remove@?$Dictionary@UCtrlMsgData@OwnerDraw@@_N@@QAEDAAUCtrlMsgData@OwnerDraw@@AA_N@Z

// TEMPLATE: GAMEMD 0x006252F0
// ?expand@?$Dictionary@UCtrlMsgData@OwnerDraw@@_N@@AAEXXZ

// TEMPLATE: GAMEMD 0x00625390
// ?keyHash@?$Dictionary@UCtrlMsgData@OwnerDraw@@_N@@AAEIAAUCtrlMsgData@OwnerDraw@@@Z
template class Dictionary<OwnerDraw::CtrlMsgData, bool>;

// TEMPLATE: GAMEMD 0x006253B0
// ?growVector@?$ArrayList@H@@AAEDXZ

// TEMPLATE: GAMEMD 0x00625440
// ?shrinkVector@?$ArrayList@H@@AAEDXZ
template class ArrayList<int>;

// FUNCTION: GAMEMD 0x006255F0
OwnerDraw::ColumnData::ColumnData(ColumnData & that)
	: xPos(that.xPos), width(that.width), cells(that.cells)
{
}

// TEMPLATE: GAMEMD 0x00625780
// ?shrink@?$Dictionary@PAUHWND__@@P6GJPAU1@IIJ@Z@@AAEXXZ

// TEMPLATE: GAMEMD 0x00625830
// ?shrink@?$Dictionary@PAUHWND__@@UWinData@OwnerDraw@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x006258E0
// ?shrink@?$Dictionary@UCtrlMsgData@OwnerDraw@@_N@@AAEXXZ

// TEMPLATE: GAMEMD 0x00625990
// ?growVector@?$ArrayList@PAUHWND__@@@@AAEDXZ
template class ArrayList<HWND>;

// SYNTHETIC: GAMEMD 0x00625560
// ??_G?$DNode@PAUHWND__@@UWinData@OwnerDraw@@@@QAEPAXI@Z
template class DNode<HWND, OwnerDraw::WinData>;

extern HWND DAT_00AC48D0;
namespace OwnerDraw {
	extern HWND ComboDropActiveParentHwnd;
	extern HWND ComboDropActiveDropHwnd;
}
void __fastcall FUN_0072A9E0(Rect *rect, int amount);
// FUNCTION: GAMEMD 0x0060D540
LRESULT CALLBACK OwnerDraw::ComboDropWindowHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	static HBRUSH solid_brush = CreateSolidBrush(0x306030);
	RECT rect;
	Get_Display_Rect(window, &rect);
	RECT client;
	GetClientRect(window, &client);
	Rect fill;
	RECT parent_rect;
	memset(&parent_rect, 0, sizeof(parent_rect));
	int scrollbar_width = 2 * OwnerDraw::ControlInsetPx + 18;
	int need_scrollbar = -1;
	int max_top = 0;
	HWND parent = 0;
	if (DAT_00AC48D0) {
		parent = GetParent(DAT_00AC48D0);
	}
	OwnerDraw::WinData *parent_data = 0;
	if (parent) {
		OwnerDraw::Dialogs.getPointer(parent, &parent_data);
		Get_Display_Rect(parent, &parent_rect);
	}
	OwnerDraw::ComboDropActiveParentHwnd = parent;
	OwnerDraw::ComboDropActiveDropHwnd = window;
	OwnerDraw::WinData *data = 0;
	OwnerDraw::Dialogs.getPointer(window, &data);
	if (data == 0) {
		DebugString("ComboBox dropdown windata = NULL\n");
	}
	OwnerDraw::WinData *master = 0;
	if (DAT_00AC48D0) {
		OwnerDraw::Dialogs.getPointer(DAT_00AC48D0, &master);
	}
	if (message != 0x146 && message != 0x154 && message != 0x115) {
		int item_count = SendMessageA(DAT_00AC48D0, 0x146, 0, 0);
		int item_height = SendMessageA(DAT_00AC48D0, 0x154, 0, 0);
		if (item_height <= 1) {
			item_height = 1;
		}
		int content_height = item_count * item_height;
		need_scrollbar = content_height > client.bottom - client.top;
		int visible_items = (client.bottom - client.top) / item_height;
		max_top = item_count - visible_items;
		if (data) {
			if ((unsigned int)data->attachedWindow > 1) {
				SCROLLINFO info;
				info.fMask = 5;
				info.nMin = 0;
				info.nMax = max_top;
				info.nPos = data->ComboDrop.scrollTop;
				info.cbSize = 0x1c;
				SendMessageA(data->attachedWindow, 0xe9, 0, (LPARAM)&info);
			}
			if (data->attachedWindow) {
				BringWindowToTop(data->attachedWindow);
			}
		}
	}
	switch (message) {
	case 0x14:
		return 0;
	case 1: {
		SetCapture(window);
		DAT_00AC48D0 = *(HWND *)lparam;
		char buffer[100];
		sprintf(buffer, "Dropd=%p  Master=%p  Dlg=%p", window, DAT_00AC48D0, GetParent(DAT_00AC48D0));
		DebugString(buffer);
		SetTimer(window, 0, 20, 0);
		return 0;
	}
	case 0x82:
		OwnerDraw::ComboDropActiveDropHwnd = 0;
		OwnerDraw::ComboDropActiveParentHwnd = 0;
		ReleaseCapture();
		KillTimer(window, 0);
		break;
	case 0x115: {
		int top = SendMessageA(data->attachedWindow, 0xe1, 0, 0);
		if (top != SendMessageA(window, 0x15b, 0, 0)) {
			SendMessageA(window, 0x15c, top, 0);
		}
		break;
	}
	case 0x113: {
		HWND capture = GetCapture();
		if (capture == window) {
			ReleaseCapture();
			Sleep(0);
			SetCapture(capture);
		}
		break;
	}
	case 0x15b:
		if (data) {
			return data->ComboDrop.scrollTop;
		}
		break;
	case 0x200: {
		int x = lparam & 0xffff;
		int y = (unsigned int)lparam >> 16;
		int index = SendMessageA(window, 0x4e8, 0, (y << 16) | x);
		if (index != -1) {
			if (data->ComboDrop.selection != index) {
				InvalidateRect(window, 0, 0);
			}
			data->ComboDrop.selection = index;
			HWND tip = GetDlgItem(parent, 0x695);
			if (tip) {
				WideWstring text;
				text = FUN_00603F00(parent, DAT_00AC48D0, index);
				if (wcscmp((wchar_t const *)text.get(), DAT_00887734)) {
					SendMessageA(tip, 0x4b2, 0, (LPARAM)text.text());
				} else {
					text = FUN_00603F60(parent, DAT_00AC48D0);
					if (wcscmp((wchar_t const *)text.get(), DAT_00887734)) {
						SendMessageA(tip, 0x4b2, 0, (LPARAM)text.text());
					}
				}
			}
		}
		return 0;
	}
	case 0x15c: {
		int top = wparam;
		int item_count = SendMessageA(DAT_00AC48D0, 0x146, 0, 0);
		int item_height = SendMessageA(DAT_00AC48D0, 0x154, 0, 0);
		if (!item_count || !item_height) {
			break;
		}
		int visible = (client.bottom - client.top) / item_height;
		if (top < 0) {
			top = 0;
		}
		int limit = item_count - visible;
		if (limit > 0) {
			if (top > limit) {
				top = limit;
			}
		} else {
			top = 0;
		}
		if (top != data->ComboDrop.scrollTop) {
			data->ComboDrop.scrollTop = top;
			InvalidateRect(window, 0, 0);
		}
		return 0;
	}
	case 0x201:
	case 0x203: {
		int x = lparam & 0xffff;
		int y = (unsigned int)lparam >> 16;
		if (x > client.right && x < client.right + data->scrollBarWidth && y > 0 && y < client.bottom) {
			SendMessageA(data->attachedWindow, message, wparam, lparam);
			break;
		}
		VocClass::PlayGlobal(Rule->GUIComboCloseSound, 1.0f, 0x2000, 0);
		if (x >= 0 && y >= 0 && x <= client.right && y <= client.bottom) {
			int item_height = SendMessageA(DAT_00AC48D0, 0x154, 0, 0);
			int item_count = SendMessageA(DAT_00AC48D0, 0x146, 0, 0);
			int index = y / item_height;
			if (data) {
				index += data->ComboDrop.scrollTop;
			}
			int clamped = index < 0 ? 0 : index;
			int max_index = item_count - 1;
			if (max_index < clamped) {
				clamped = max_index;
			}
			SendMessageA(DAT_00AC48D0, 0x14e, clamped, 0);
			int id = GetWindowLongA(DAT_00AC48D0, -12);
			if (need_scrollbar) {
				DestroyWindow(data->attachedWindow);
			}
			ReleaseCapture();
			SendMessageA(DAT_00AC48D0, 0x14f, 0, 0);
			PostMessageA(parent, 0x111, (id & 0xffff) | 0x10000, (LPARAM)DAT_00AC48D0);
			return 0;
		}
		if (need_scrollbar) {
			DestroyWindow(data->attachedWindow);
		}
		ReleaseCapture();
		SendMessageA(DAT_00AC48D0, 0x14f, 0, 0);
		return 0;
	}
	case 0x4e8: {
		int x = lparam & 0xffff;
		int y = (unsigned int)lparam >> 16;
		int result = -1;
		if (x >= 0 && y >= 0 && x < client.right && y < client.bottom) {
			int item_height = SendMessageA(DAT_00AC48D0, 0x154, 0, 0);
			int item_count = SendMessageA(DAT_00AC48D0, 0x146, 0, 0);
			int index = y / item_height + data->ComboDrop.scrollTop;
			int clamped = index < 0 ? 0 : index;
			int max_index = item_count - 1;
			if (max_index < clamped) {
				clamped = max_index;
			}
			result = clamped;
		}
		return result;
	}
	case 0x4ea:
		return (LRESULT)DAT_00AC48D0;
	case 0x7e8:
		data->ComboDrop.selection = SendMessageA(DAT_00AC48D0, 0x147, 0, 0);
		return 0;
	case 0xf: {
		BitFont *font = parent_data->Font;
		int item_count = SendMessageA(DAT_00AC48D0, 0x146, 0, 0);
		int item_height = SendMessageA(DAT_00AC48D0, 0x154, 0, 0);
		int client_height = client.bottom - client.top;
		Rect dest_rect(rect.left, rect.top, rect.right - rect.left, client_height);
		int selected = data->ComboDrop.selection;
		Rect src(dest_rect.Top_Left() - Point2D(parent_rect.left, parent_rect.top), client.right, client.bottom);
		if (data->cachedSurface == 0) {
			Rect dst(0, 0, client.right, client.bottom);
			BSurface *surface = new BSurface(client.right, client.bottom, 2);
			data->cachedSurface = surface;
			OwnerDraw::CachedSurfaceCount++;
			if (parent_data && parent_data->cachedSurface) {
				surface->Blit_From(dst, *parent_data->cachedSurface, src, false, true);
			}
			int total = client.right * client.bottom;
			unsigned short *pixels = (unsigned short *)surface->Lock();
			for (int i = 0; i < total; i++) {
				unsigned short pixel = pixels[i];
				unsigned char alpha = data->alpha;
				pixels[i] = OD_Blend_Color(pixel, 0, alpha);
			}
			surface->Unlock();
		}
		{
			Rect from(0, 0, dest_rect.Width, dest_rect.Height);
			AlternateSurface->Blit_From(dest_rect, *data->cachedSurface, from, false, true);
		}
		Rect frame(dest_rect);
		int color = ODColorToHiColor(OwnerDraw::DefaultBorderColor);
		if (master && master->disabled) {
			color = ODColorToHiColor(OwnerDraw::AltBorderColor);
		}
		OwnerDraw::DrawBeveledBorder(*AlternateSurface, frame, 2, color);
		if (need_scrollbar && data && data->attachedWindow) {
			Rect outer(dest_rect);
			outer.Width += scrollbar_width;
			OwnerDraw::DrawBeveledBorder(*AlternateSurface, outer, 2, color);
		}
		int first = data->ComboDrop.scrollTop;
		Rect row;
		for (int index = first; index < item_count; index++) {
			row.Set(rect.left, rect.top, rect.right - rect.left, item_height);
			row.Y += (index - first) * item_height;
			wchar_t text[128];
			if ((index - first) * item_height + item_height > client_height) {
				break;
			} else {
				HWND owner = DAT_00AC48D0;
				WNDPROC proc = Find_Original_Proc(owner);
				text[0] = 0;
				int item = CallWindowProcA(proc, owner, 0x199, index, 0);
				if (item == -1 || item == 0) {
					item = CallWindowProcA(proc, owner, 0x150, index, 0);
				}
				if (item != -1 && item != 0) {
					Copy_Wide_Text(text, ((OwnerDraw::WinNode *)item)->text, 0x80);
				}
			}
			if (index == selected) {
				fill.Set(row.X, row.Y, row.Width, row.Height);
				int fill_color;
				if (master ? master->disabled : false) {
					fill_color = ODColorToHiColor(OwnerDraw::AltSelectionFillColor);
				} else {
					fill_color = ODColorToHiColor(OwnerDraw::ListSelectionFillColor);
				}
				AlternateSurface->Fill_Rect(fill, fill_color);
			}
			RECT text_rect;
			text_rect.top = row.Y;
			text_rect.left = row.X + 3;
			text_rect.right = row.X + row.Width;
			text_rect.bottom = row.Y + row.Height;
			unsigned long text_color = OwnerDraw::PrimaryTextColor;
			if (master && master->disabled) {
				text_color = OwnerDraw::AltComboTextColor;
			}
			if (index < 50 && master && master->ComboBox.itemColors[index] >= 0) {
				text_color = master->ComboBox.itemColors[index];
				if (master->UseItemColorOverrides) {
					Rect fill2;
					fill2.Set(row.X, row.Y, row.Width, row.Height);
					FUN_0072A9E0(&fill2, 2);
					AlternateSurface->Fill_Rect(fill2, ODColorToHiColor(text_color));
				}
			}
			int max_width = client.right - 20;
			int length = wcslen(text);
			int text_width = font->FUN_00433ED0(text, 0);
			while (length && text_width > max_width) {
				length--;
				text[length] = 0;
				wcscat(text, L"...");
				text_width = font->FUN_00433ED0(text, 0);
			}
			OwnerDraw::DrawWideText(AlternateSurface, text, (LTRBStruct *)&text_rect, font, text_color, 4, 12, 0, 0, 0);
		}
		AlternateSurface->Lock();
		VisibleSurface->Lock();
		Rect screen(dest_rect);
		RECT window_rect;
		GetWindowRect(window, &window_rect);
		screen.X += window_rect.left - rect.left;
		screen.Y += window_rect.top - rect.top;
		VisibleSurface->Blit_From(screen, *AlternateSurface, dest_rect, false, true);
		VisibleSurface->Unlock();
		AlternateSurface->Unlock();
		ValidateRect(window, 0);
		return 0;
	}
	case 2:
		FUN_0053E420(window);
		break;
	}
	if (need_scrollbar == 1) {
		if (data && !data->attachedWindow) {
			data->attachedWindow = (HWND)1;
			GetWindowLongA(window, -12);
			parent = GetParent(window);
			RECT parent_display;
			Get_Display_Rect(parent, &parent_display);
			RECT drop_display;
			Get_Display_Rect(window, &drop_display);
			int left = drop_display.left - parent_display.left;
			int top = drop_display.top - parent_display.top;
			bool alternate = false;
			int x = left - scrollbar_width + client.right;
			int y = top + client.top;
			int height = drop_display.bottom - drop_display.top;
			if (master) {
				alternate = master->disabled;
			}
			HWND scroll = CreateWindowExA(0, "Scrollbar", 0, 0x50010001, x, y, scrollbar_width, height, parent, 0,
				ProgramInstance, 0);
			data->attachedWindow = scroll;
			data->scrollBarWidth = scrollbar_width;
			OwnerDraw::InitializeCtrl(scroll, 0);
			OwnerDraw::WinData *scroll_data = 0;
			OwnerDraw::Dialogs.getPointer(data->attachedWindow, &scroll_data);
			scroll_data->ownerWindow = window;
			scroll_data->disabled = data->disabled;
			SCROLLINFO info;
			info.nMax = max_top;
			info.fMask = 5;
			info.nMin = 0;
			info.nPos = data->ComboDrop.scrollTop;
			info.cbSize = 0x1c;
			SendMessageA(data->attachedWindow, 0xe9, 0, (LPARAM)&info);
			SendMessageA(data->attachedWindow, 0x4f1, 0, alternate);
			SetWindowPos(window, 0, 0, 0, drop_display.right - drop_display.left - scrollbar_width, drop_display.bottom - drop_display.top, 2);
			ShowWindow(data->attachedWindow, 5);
			BringWindowToTop(data->attachedWindow);
			InvalidateRect(data->attachedWindow, 0, 0);
			UpdateWindow(data->attachedWindow);
			SendMessageA(data->attachedWindow, 0x4a9, (WPARAM)data->attachedWindow, 1);
			SendMessageA(data->attachedWindow, 0x4d2, 0, 1);
			RECT validate;
			validate.left = left;
			validate.top = top;
			validate.right = left + client.right + 1;
			validate.bottom = top + client.bottom + 1;
			ValidateRect(parent, &validate);
		}
	} else if (need_scrollbar == 0 && data && data->attachedWindow && !data->paintDisabled) {
		DestroyWindow(data->attachedWindow);
		ODRemoveFromDict(data->attachedWindow, 0);
		data->attachedWindow = 0;
		data->scrollBarWidth = 0;
		SetWindowPos(window, 0, 0, 0, client.right - client.left + scrollbar_width, client.bottom - client.top, 2);
		parent = GetParent(window);
		RECT validate_parent;
		Get_Display_Rect(parent, &validate_parent);
		RECT validate;
		validate.left = rect.left - validate_parent.left;
		validate.top = rect.top - validate_parent.top;
		validate.right = validate.left + client.right + scrollbar_width;
		validate.bottom = validate.top + client.bottom + 1;
		ValidateRect(parent, &validate);
	}
	return DefWindowProcA(window, message, wparam, lparam);
}

static inline void Stop_Static_Movie(OwnerDraw::WinData *data, HWND window)
{
	if (data->StaticMovieHandle != NULL) {
		delete data->StaticMovieHandle;
		data->StaticMovieHandle = NULL;
	}
	KillTimer(window, 101);
	if (data->StaticMovieAuxHandle != NULL) {
		delete data->StaticMovieAuxHandle;
		data->StaticMovieAuxHandle = NULL;
	}
}

static inline void Play_Static_Movie(OwnerDraw::WinData *data, HWND window, char const *movieName)
{
	Stop_Static_Movie(data, window);
	RECT drect;
	Get_Display_Rect(window, &drect);
	data->StaticMovieHandle = OwnerDraw_InitMovieHandle(movieName, AlternateSurface, NULL);
	if (data->StaticMovieHandle != NULL) {
		data->StaticMovieHandle->SetPosition(drect.left, drect.top);
		MovieHandle *movie = data->StaticMovieHandle;
		MoveWindow(window, drect.left, drect.top, movie->Width, movie->Height, FALSE);
		SetTimer(window, 101, 34, NULL);
	} else {
		KillTimer(window, 101);
		if (data->StaticMovieAuxHandle != NULL) {
			delete data->StaticMovieAuxHandle;
			data->StaticMovieAuxHandle = NULL;
		}
	}
}

// FUNCTION: GAMEMD 0x006153E0
// OwnerDraw::StaticHandler
LRESULT CALLBACK OwnerDraw::StaticHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	WinData *data = Find_Data(window);
	RECT text_rect;
	if (data == NULL) {
		return 0;
	}
	switch (message) {
	case WM_DESTROY: {
		if (data->cachedSurface != NULL) {
			delete data->cachedSurface;
			data->cachedSurface = NULL;
		}
		if (data->StaticOwnsShape) {
			delete data->StaticShape;
			data->StaticShape = NULL;
		}
		if (data->StaticDrawMode == 1) {
			if (data->StaticAnimationRunning) {
				KillTimer(window, 0);
			}
		} else if (data->StaticDrawMode == 4) {
			KillTimer(window, 0);
		}
		HWND const movieWindow = window;
		if (data->StaticMovieHandle != NULL) {
			delete data->StaticMovieHandle;
			data->StaticMovieHandle = NULL;
		}
		KillTimer(movieWindow, 101);
		if (data->StaticMovieAuxHandle != NULL) {
			delete data->StaticMovieAuxHandle;
			data->StaticMovieAuxHandle = NULL;
		}
	}
	default:
		goto call_original;
	case WM_PAINT: {
		if (data->skipDraw) {
			ValidateRect(window, NULL);
			break;
		}
		if (data->StaticMovieHandle != NULL) {
			ValidateRect(window, NULL);
			break;
		}
		if (data->cachedSurface == NULL) {
			RECT drect;
			Get_Display_Rect(window, &drect);
			RECT crect;
			GetClientRect(window, &crect);
			BSurface *surf = new BSurface(crect.right + 1, crect.bottom + 1, 2);
			data->cachedSurface = surf;
			++CachedSurfaceCount;
			Rect dst_rect;
			dst_rect.X = 0;
			dst_rect.Y = 0;
			dst_rect.Width = crect.right + 1;
			dst_rect.Height = crect.bottom + 1;
			Rect src_rect;
			src_rect.X = drect.left;
			src_rect.Y = drect.top;
			src_rect.Width = crect.right + 1;
			src_rect.Height = crect.bottom + 1;
			surf->Blit_From(dst_rect, *AlternateSurface, src_rect);
		}
		Get_Display_Rect(window, &text_rect);
		if (data->StaticFillBackground) {
			Rect rect(text_rect.left, text_rect.top, text_rect.right - text_rect.left, text_rect.bottom - text_rect.top);
			AlternateSurface->Fill_Rect(rect, DSurface::Build_Hicolor_Pixel(GetRValue(data->StaticFillColor), (unsigned char)(data->StaticFillColor >> 8), GetBValue(data->StaticFillColor)));
		}
		bool text = data->StaticDrawMode == 0;
		bool typewriter = data->StaticDrawMode == 1;
		if (text || typewriter) {
			if (data->text != NULL && (text || (typewriter && data->StaticAnimationRunning))) {
				LONG style = GetWindowLong(window, GWL_STYLE);
				int draw_flags = 16;
				if (style & SS_CENTER) {
					draw_flags = 17;
				} else if (style & SS_RIGHT) {
					draw_flags = 18;
				}
				COLORREF color = data->Static.textColor;
				if (style & WS_DISABLED) {
					color = DisabledTextColor;
				}
				DrawWideText(AlternateSurface, data->text, (LTRBStruct *)&text_rect, data->Font, color, draw_flags, data->StaticTextFlags, 0, data->StaticTextRevealCount, data->StaticColorAdjust);
				if (data->StaticSoundIndex != -1) {
					VocClass::PlayGlobal(data->StaticSoundIndex, 1.0f, 0x2000, NULL);
				}
				if (typewriter) {
					int length = wcslen(data->text) + data->StaticColorAdjust + 1;
					if (data->StaticTextRevealCount < length) {
						data->StaticTextRevealCount += data->StaticTextRevealStep;
						if (data->StaticTextRevealCount >= length) {
							KillTimer(window, 0);
						}
					}
				}
			}
		} else {
			bool pcx = data->StaticDrawMode == 2;
			bool shape = data->StaticDrawMode == 3;
			bool animated = data->StaticDrawMode == 4;
			if (pcx || shape || animated) {
				int now = GetTickCount();
				if (now - data->StaticLastFrameTick > data->StaticFrameDelayMs) {
					Rect dst_rect(text_rect.left, text_rect.top, text_rect.right - text_rect.left, text_rect.bottom - text_rect.top);
					RECT crect;
					GetClientRect(window, &crect);
					Rect src_rect(crect.left, crect.top, crect.right - crect.left, crect.bottom - crect.top);
					AlternateSurface->Blit_From(dst_rect, *data->cachedSurface, src_rect);
					if (pcx) {
						Surface *image = data->image;
						if (image != NULL) {
							int width = image->Get_Width();
							int height = image->Get_Height();
							Rect rect = dst_rect;
							if (rect.Width > width) {
								rect.X += (rect.Width - width) / 2;
								rect.Width = width;
							}
							if (rect.Height > height) {
								rect.Y += (rect.Height - height) / 2;
								rect.Height = height;
							}
							SurfaceCache.DrawTrans(rect, *AlternateSurface, *image, DSurface::Build_Hicolor_Pixel(255, 0, 255));
						}
					} else if (shape || animated) {
						ShapeSet *shapes = data->StaticShape;
						if (shapes != NULL) {
							ConvertClass *drawer = data->StaticShapeDrawer;
							int height = shapes->Get_Height();
							int width = shapes->Get_Width();
							Rect rect = dst_rect;
							if (rect.Width > width) {
								rect.X += (rect.Width - width) / 2;
								rect.Width = width;
							}
							if (rect.Height > height) {
								rect.Y += (rect.Height - height) / 2;
								rect.Height = height;
							}
							Point2D point(rect.X, rect.Y);
							Rect bounds = AlternateSurface->Get_Rect();
							int frame = data->StaticCurrentFrame;
							Draw_Shape(*AlternateSurface, *drawer, shapes, frame, point, bounds, SHAPE_WIN_REL);
							if (animated && data->StaticAnimationRunning) {
								if (++frame >= data->StaticFrameCount) {
									frame = 0;
								}
								data->StaticCurrentFrame = frame;
								if (data->StaticFrameNotifyHwnd != NULL) {
									SendMessage(data->StaticFrameNotifyHwnd, WM_USER + 216, frame, (LPARAM)window);
								}
							}
						}
					}
				}
			}
		}
		ValidateRect(window, NULL);
		break;
	}

	case WM_USER + 211:
		if (data->StaticShape != NULL && data->StaticDrawMode == 4 && !data->StaticAnimationRunning) {
			data->StaticAnimationRunning = true;
			HWND parent = GetParent(window);
			int interval = Control_Volume(parent, window);
			SetTimer(window, 0, interval, NULL);
		}
		break;

	case OD_SETCOLOR:
		if ((COLORREF)lparam != data->Static.textColor) {
			InvalidateRect(window, NULL, FALSE);
		}
		if (lparam == -1) {
			data->Static.textColor = PrimaryTextColor;
		} else {
			data->Static.textColor = lparam;
		}
		break;

	case WM_USER + 177:
		data->StaticFillBackground = true;
		data->StaticFillColor = lparam;
		break;

	case WM_USER + 178:
	case WM_USER + 180: {
		Surface *cachedSurf = data->cachedSurface;
		if (cachedSurf != NULL) {
			Get_Display_Rect(window, &text_rect);
			RECT crect;
			GetClientRect(window, &crect);

			Rect src_rect(0, 0, crect.right + 1, crect.bottom + 1);
			Rect dst_rect(text_rect.left, text_rect.top, src_rect.Width, src_rect.Height);

			AlternateSurface->Blit_From(dst_rect, *cachedSurf, src_rect);
			InvalidateRect(window, NULL, FALSE);
		}
		return 1;
	}

	case WM_USER + 212:
		if (data->StaticDrawMode == 4 && data->StaticAnimationRunning) {
			KillTimer(window, 0);
			data->StaticAnimationRunning = false;
		}
		break;

	case WM_USER + 238:
		if (data->StaticDrawMode == 1 && !data->StaticAnimationRunning) {
			data->StaticAnimationRunning = true;
			data->StaticTextRevealCount = 1;
			SetTimer(window, 0, data->StaticTextRevealDelay, NULL);
			InvalidateRect(window, NULL, FALSE);
		}
		break;

	case WM_USER + 213:
		if (data->StaticDrawMode == 4) {
			data->StaticCurrentFrame = lparam;
			InvalidateRect(window, NULL, TRUE);
		}
		break;

	case WM_USER + 214: {
		int frame = -1;
		if (data->StaticDrawMode == 4) {
			frame = data->StaticCurrentFrame;
		}
		return frame;
	}

	case WM_USER + 215:
		if (data->StaticDrawMode == 4) {
			data->StaticFrameNotifyHwnd = (HWND)lparam;
		}
		break;

	case WM_USER + 223:
		if ((int)wparam >= 0 && (int)wparam < DAT_00ABF390.Count()) {
			char const *movieName = DAT_00ABF390[(int)wparam];
			Play_Static_Movie(data, window, movieName);
		}
		break;

	case WM_USER + 228: {
		Play_Static_Movie(data, window, (char const *)lparam);
		break;
	}

	case WM_USER + 226: {
		Stop_Static_Movie(data, window);
		break;
	}

	case WM_USER + 240:
		if (data->StaticMovieHandle != NULL) {
			data->StaticMovieHandle->Blit();
		}
		break;

	case WM_USER + 224:
		if (data->StaticMovieHandle != NULL) {
			data->StaticMovieHandle->Pause(true);
		}
		break;

	case WM_USER + 225:
		if (data->StaticMovieHandle != NULL) {
			data->StaticMovieHandle->Pause(false);
		}
		break;

	case WM_USER + 227:
		data->StaticLoopMovie = wparam;
		break;

	case WM_TIMER: {
		if (wparam == 101) {
			if (data->StaticMovieHandle != NULL) {
				if (data->StaticMovieHandle->AdvanceFrame()) {
					InvalidateRect(window, NULL, FALSE);
				}
				if (data->StaticMovieHandle->FramesLeft()) {
					if (data->StaticLoopMovie) {
						data->StaticMovieHandle->SeekToFrame(1);
						FUN_004068E0("Looping movie\n");
					} else {
						Stop_Static_Movie(data, window);
					}
				}
			}
			break;
		}
		if (data->StaticDrawMode == 1) {
			InvalidateRect(window, NULL, TRUE);
			break;
		}
		bool animated = data->StaticDrawMode == 4;
		bool still = data->StaticDrawMode == 2 || data->StaticDrawMode == 3;
		if (still || animated) {
			int now = GetTickCount();
			if (now - data->StaticLastFrameTick > data->StaticFrameDelayMs) {
				if (still || (animated && data->StaticAnimationRunning)) {
					InvalidateRect(window, NULL, TRUE);
					if (still) {
						KillTimer(window, 0);
						break;
					}
				}
				if (data->StaticFrameCount == 0) {
					KillTimer(window, 0);
					if (data->StaticShape != NULL) {
						data->StaticFrameCount = data->StaticShape->Get_Count();
						SetTimer(window, 0, data->StaticFrameCount, NULL);
					}
				}
			}
		}
		break;
	}

	case WM_MOVE:
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		delete data->cachedSurface;
		data->cachedSurface = NULL;
		InvalidateRect(window, NULL, FALSE);
		break;

	case OD_SUBCLASSED: {
		WNDPROC proc = NULL;
		DialogProcs.getValue(window, proc);
		HWND parent = GetParent(window);
		data->StaticDrawMode = 0;
		data->StaticTextFlags = Control_Slot_Count(parent, window);
		data->Static.textColor = PrimaryTextColor;
		break;
	}


	}
	return 0;

call_original:
	WNDPROC proc = NULL;
	WNDPROC *found = NULL;
	if (DialogProcs.getPointer(window, &found) && found) {
		proc = *found;
	}
	return CallWindowProc(proc, window, message, wparam, lparam);
}



void __fastcall FUN_00735060(wchar_t *dest, char const *source, int count);
char * __fastcall FUN_007350C0(wchar_t const *source);

// TEMPLATE: GAMEMD 0x006248A0
// ?shrinkVector@?$ArrayList@UCellData@OwnerDraw@@@@AAEDXZ


LRESULT CALLBACK OwnerDraw::ListBoxHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	BOOL wide_messages = TRUE;
	WNDPROC original_proc = NULL;
	DialogProcs.getValue(window, original_proc);
	HWND parent = GetParent(window);
	int needs_scrollbar = -1;
	int max_position = 0;
	char call_default = 1;
	LRESULT result = 0;

	WinData *data = Find_Data(window);
	if (data == NULL) return 0;
	wchar_t converted[0x1400];

	int scrollbar_width = 2 * ControlInsetPx + 18;

	RECT client_rect;
	GetClientRect(window, &client_rect);
	RECT display_rect;
	Get_Display_Rect(window, &display_rect);
	display_rect.right -= ControlInsetPx;
	display_rect.left += ControlInsetPx;

	Rect content_rect;
	content_rect.X = display_rect.left;
	client_rect.right -= 2 * ControlInsetPx;
	content_rect.Width = client_rect.right - client_rect.left;
	display_rect.top += ControlInsetPx;
	content_rect.Y = display_rect.top;
	client_rect.bottom -= 2 * ControlInsetPx;
	display_rect.bottom -= ControlInsetPx;
	content_rect.Height = client_rect.bottom - client_rect.top;


	if (message != LB_GETCOUNT && message != LB_GETITEMHEIGHT && message != WM_VSCROLL) {
		int count = SendMessage(window, LB_GETCOUNT, 0, 0);
		int item_height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
		if (item_height <= 1) {
			item_height = 1;
		}
		needs_scrollbar = (count * item_height > client_rect.bottom - client_rect.top);
		max_position = count - (client_rect.bottom - client_rect.top) / item_height;
		if ((unsigned int)data->attachedWindow > 1) {
			SCROLLINFO info;
			info.fMask = SIF_RANGE | SIF_POS;
			info.nMin = 0;
			info.nMax = max_position;
			info.nPos = data->ListBox.topIndex;
			info.cbSize = sizeof(SCROLLINFO);
			SendMessage(data->attachedWindow, SBM_SETSCROLLINFO, 0, (LPARAM)&info);
		}
	}


	int index;
	switch (message) {
		case LB_SETSEL: {
			index = (int)lparam;
			if (index < -1) {
				return(-1);
			}
			ArrayList<int> * sel_states = data->ListBox.selStates;
			if (sel_states == NULL) {
				sel_states = new ArrayList<int>;
				data->ListBox.selStates = sel_states;
			}
			if (index >= SendMessage(window, LB_GETCOUNT, 0, 0) - 1) {
				index = SendMessage(window, LB_GETCOUNT, 0, 0) - 1;
			}
			int max_index = MAX(index, SendMessage(window, LB_GETCOUNT, 0, 0) - 1);
			if (max_index >= sel_states->length()) {
				int unselected = 0;
				sel_states->setSize(max_index + 1, unselected);
			}
			if (index == -1) {
				for (int i = 0; i < sel_states->length(); i++) {
					int selected = (int)wparam;
					sel_states->replace(selected, i);
				}
			} else {
				int selected = (int)wparam;
				sel_states->replace(selected, index);
			}
			parent = GetParent(window);
			SendMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x10000, (LPARAM)window);
			InvalidateRect(window, NULL, FALSE);
			return 0;
		}


		case LB_GETITEMDATA:
		case LB_SETITEMDATA:
		case CB_GETITEMDATA:
		case CB_SETITEMDATA: {
			call_default = 0;
			result = CallWindowProc(original_proc, window, LB_GETITEMDATA, wparam, 0);
			if (result == -1) break;
			WinNode *node = (WinNode *)result;
			if (node == NULL) {
				result = -1;
				break;
			}
			if (message == LB_GETITEMDATA || message == CB_GETITEMDATA) {
				result = node->id;
			} else {
				node->id = lparam;
			}
			break;
		}


		case LB_SETTOPINDEX: {
			index = (int)wparam;
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			int item_height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
			if (!count || !item_height) {
				break;
			}
			int visible = (client_rect.bottom - client_rect.top) / item_height;
			if (index < 0) {
				index = 0;
			}
			if (count - visible > 0) {
				if (index > count - visible) {
					index = count - visible;
				}
			} else {
				index = 0;
			}
			if (index != data->ListBox.topIndex) {
				data->ListBox.topIndex = index;
				InvalidateRect(window, NULL, FALSE);
			}
			return(0);
		}


		case LB_GETTOPINDEX:
			return(data->ListBox.topIndex);


		case LB_GETSELCOUNT: {
			ArrayList<int> * sel_states = data->ListBox.selStates;
			if (sel_states == NULL) {
				return(0);
			}
			int count = sel_states->length();
			int selected = 0;
			int total = 0;
			for (int i = 0; i < count; i++) {
				sel_states->get(selected, i);
				if (selected) {
					total++;
				}
			}
			return(total);
		}


		case LB_GETSELITEMS: {
			ArrayList<int> * sel_states = data->ListBox.selStates;
			int selected = 0;
			int total = 0;
			if (sel_states == NULL) {
				return(0);
			}
			int * out = (int *)lparam;
			int max = (int)wparam;
			for (int i = 0; i < sel_states->length(); i++) {
				sel_states->get(selected, i);
				if (selected) {
					*out = i;
					total++;
					out++;
				}
				if (total >= max) {
					break;
				}
			}
			return(total);
		}


		case LB_GETSEL: {
			ArrayList<int> * sel_states = data->ListBox.selStates;
			int selected = 0;
			if (sel_states == NULL) {
				return(0);
			}
			if ((int)wparam >= sel_states->length()) {
				return(0);
			}
			sel_states->get(selected, (int)wparam);
			return(selected);
		}


		case LB_SETCURSEL: {
			index = (int)wparam;
			if ((int)wparam >= -1 && index < SendMessage(window, LB_GETCOUNT, 0, 0)) {
				if (data->ListBox.curSel != -1) {
					SendMessage(window, LB_SETSEL, 0, data->ListBox.curSel);
				}
				data->ListBox.curSel = index;
				if (index != -1) {
					SendMessage(window, LB_SETSEL, TRUE, index);
				}
			}
			parent = GetParent(window);
			SendMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x10000, (LPARAM)window);
			InvalidateRect(window, NULL, FALSE);
			return(0);
		}


		case LB_GETCURSEL:
			return(data->ListBox.curSel);


		case LB_GETITEMRECT: {
			index = (int)wparam;
			if (index < data->ListBox.topIndex) {
				return(-1);
			}
			if (index >= SendMessage(window, LB_GETCOUNT, 0, 0)) {
				return(-1);
			}
			int item_height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
			int relative = index - data->ListBox.topIndex;
			if (relative > (client_rect.bottom - client_rect.top) / item_height) {
				return(-1);
			}
			RECT * out = (RECT *)lparam;
			out->top = relative * item_height;
			out->bottom = out->top + item_height;
			out->left = client_rect.left;
			out->right = client_rect.right - client_rect.left;
			return(0);
		}


		case WM_LBUTTONDBLCLK:
			PostMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x20000, (LPARAM)window);
			return(0);


		case WM_LBUTTONDOWN: {
			int top = SendMessage(window, LB_GETTOPINDEX, 0, 0);
			int item_height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
			index = top + (int)HIWORD(lparam) / item_height;
			if (index >= SendMessage(window, LB_GETCOUNT, 0, 0)) {
				return 0;
			}
			LONG style = GetWindowLong(window, GWL_STYLE);
			SetFocus(window);
			int paint_disabled = SendMessage(window, OD_DISABLEPAINT, 0, 1);
			if ((style & LBS_MULTIPLESEL) != 0) {
				int select = (SendMessage(window, LB_GETSEL, index, 0) == 0);
				VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000, 0);
				SendMessage(window, LB_SETSEL, select, index);
			} else if ((style & LBS_NOSEL) == 0) {
				VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000, 0);
				SendMessage(window, LB_SETCURSEL, index, 0);
			}
			SendMessage(window, OD_DISABLEPAINT, 0, paint_disabled);
			InvalidateRect(window, NULL, FALSE);
			parent = GetParent(window);
			SendMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x10000, (LPARAM)window);
			return(0);
		}


		case WM_RBUTTONDOWN:
			if (GetWindowLong(window, GWL_STYLE) & LBS_MULTIPLESEL) {
			SendMessage(window, LB_SETSEL, 0, -1);
			SendMessage(window, LB_SETCURSEL, (WPARAM)-1, 0);
			parent = GetParent(window);
			SendMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x10000, (LPARAM)window);
			InvalidateRect(window, NULL, FALSE);
				return(0);
			}
			break;


		case OD_SETCOLOR: {
			ArrayList<int> * row_colors = data->ListBox.rowColors;
			if (row_colors == NULL) {
				row_colors = new ArrayList<int>;
				data->ListBox.rowColors = row_colors;
			}
			index = (int)wparam;
			if (index >= row_colors->length()) {
				int blank = -1;
				row_colors->setSize(index + 1, blank);
			}
			int color = (int)lparam;
			row_colors->replace(color, index);
			InvalidateRect(window, NULL, FALSE);
			break;
		}


		case OD_SUBCLASSED:
			data->ListBox.curSel = -1;
			SendMessage(window, LB_SETITEMHEIGHT, (WPARAM)-1, ((data->Font->field_1C + 2) & 0xFFFF));
			break;


		case OD_ADDCOLUMN: {
			ArrayList<ColumnData> * columns = data->ListBox.columns;
			if (columns == NULL) {
				columns = new ArrayList<ColumnData>;
				data->ListBox.columns = columns;
			}
			ColumnData * column = NULL;
			int col = 0;
			while (col < columns->length()) {
				columns->getPointer(&column, col);
				if (column->xPos == lparam) {
					return(lparam);
				}
				col++;
			}
			ColumnData new_column;
			new_column.xPos = lparam;
			new_column.width = wparam;
			columns->addTail(new_column);
			return(lparam);
		}


		case OD_GETCELLTIP: {
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			int item_height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
			ArrayList<ColumnData> * columns = data->ListBox.columns;
			if (columns != NULL) {
				int row = data->ListBox.topIndex + (int)HIWORD(wparam) / item_height;
				int best_column = -1;
				int best_x = 0;
				ColumnData * column = NULL;
				for (int col = 0; col < columns->length(); col++) {
					columns->getPointer(&column, col);
					if (column->xPos <= (int)LOWORD(wparam)) {
						if (column->xPos > best_x) {
							best_column = col;
							best_x = column->xPos;
						}
					}
				}
				if (best_column != -1 && row >= 0 && row < count) {
					columns->getPointer(&column, best_column);
					if (column != NULL) {
						CellData * cell;
						if (!column->cells.getPointer(&cell, row)) return 0;
						if (cell != NULL && lparam != 0) {
							wcscpy((wchar_t *)lparam, cell->hint.get());
							return(wcslen(cell->hint.get()) == 0);
						}
					}
				}
			}
			return(0);
		}


		case OD_REMOVECOLUMN: {
			ArrayList<ColumnData> * columns = data->ListBox.columns;
			if (columns == NULL) {
				return -1;
			}
			ColumnData * column = NULL;
			for (int col = 0; col < columns->length(); col++) {
					columns->getPointer(&column, col);
					if (column->xPos == lparam) {
						columns->remove(col);
						return(lparam);
					}
			}
			return(-1);
		}


		case OD_SETCELL: {
			ArrayList<ColumnData> * columns = data->ListBox.columns;
			if (columns == NULL) {
				return(-1);
			}
			int column_id = LOWORD(wparam);
			int row = HIWORD(wparam);
			ColumnData * column = NULL;
			int found = -1;
			for (int col = 0; col < columns->length(); col++) {
				columns->getPointer(&column, col);
				if (column->xPos == column_id) {
					found = col;
					break;
				}
			}
			if (found == -1) {
				return(-1);
			}
			if (row < 0 || row >= SendMessage(window, LB_GETCOUNT, 0, 0)) {
				return(-1);
			}
			CellData filler;
			if (found == 0) {
				filler.type = CellData::PRIMARY;
			}
			if (row >= column->cells.length()) {
				column->cells.setSize(row + 1, filler);
			}
			column->cells.replace(*(CellData *)lparam, row);
			return(column_id);
		}


		case 0x4C4:
		case 0x4C5:
		case 0x4C6:
			::FUN_00735060(converted, (char *)lparam, 0x1400);
			lparam = (LPARAM)converted;
		case 0x4C9:
		case 0x4CA:
		case 0x4CB: {
			call_default = 0;
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			BOOL found = FALSE;
			if (count == -1) break;
			for (index = (int)wparam; index < count; ++index) {
				if (found) break;
				result = CallWindowProc(original_proc, window, LB_GETITEMDATA, index, 0);
				if (result != -1) {
					WinNode *node = (WinNode *)result;
					if (message == 0x4C5 || message == 0x4CA) {
						found = (_wcsicmp((wchar_t *)lparam, node->text) == 0);
					} else {
						found = (_wcsnicmp((wchar_t *)lparam, node->text, wcslen((wchar_t *)lparam)) == 0);
					}
					result = index;
				}
			}
			if (!found) {
				result = -1;
			} else if (message == 0x4C6) {
				result = SendMessage(window, LB_SETCURSEL, result, 0);
			}
			break;
		}


		case 0x4C7:
		case 0x4C8:
			wide_messages = FALSE;
		case 0x4CC:
		case 0x4CD: {
			call_default = 0;
			if (message == 0x4C8 || message == 0x4CD) {
				if (wide_messages) {
					result = CallWindowProc(original_proc, window, LB_ADDSTRING, 0, (LPARAM)::FUN_007350C0((wchar_t *)lparam));
				} else {
					result = CallWindowProc(original_proc, window, LB_ADDSTRING, 0, lparam);
				}
			} else {
				if (wide_messages) {
					result = CallWindowProc(original_proc, window, LB_INSERTSTRING, wparam, (LPARAM)::FUN_007350C0((wchar_t *)lparam));
				} else {
					result = CallWindowProc(original_proc, window, LB_INSERTSTRING, wparam, lparam);
				}
			}
			if (result == -1 || result == -2) break;
			int position = result;
			int length = wide_messages ? wcslen((wchar_t *)lparam) : strlen((char *)lparam);
			WinNode *node = data->Add_Node(length);
			if (node != NULL) {
				node->data = wide_messages;
				if (wide_messages) {
					wcscpy(node->text, (wchar_t *)lparam);
				} else {
					::FUN_00735060(node->text, (char *)lparam, -1);
				}
				result = CallWindowProc(original_proc, window, LB_SETITEMDATA, position, (LPARAM)node);
				if (result != -1 && result != -2) {
					result = position;
					if (message != 0x4C8 && position != -1) {
						ArrayList<int> *row_colors = data->ListBox.rowColors;
						if (row_colors != NULL && position < row_colors->length()) {
							int color = -1;
							row_colors->add(color, position);
							}
						ArrayList<int> *sel_states = data->ListBox.selStates;
						if (sel_states != NULL && position < sel_states->length()) {
							int selected = 0;
							sel_states->add(selected, position);
						}
					}
					if (message == 0x4C8 || position < 0) {
						position = SendMessage(window, LB_GETCOUNT, 0, 0);
					}
					ArrayList<ColumnData> *columns = data->ListBox.columns;
					CellData cell;
					if (columns != NULL) {
						ColumnData *column = NULL;
						for (int col = 0; col < columns->length(); ++col) {
							columns->getPointer(&column, col);
							cell.type = (col == 0) ? CellData::PRIMARY : CellData::INVALID;
							column->cells.add(cell, position);
						}
					}
					break;
				}
			} else {
				result = -2;
			}
			CallWindowProc(original_proc, window, LB_DELETESTRING, position, 0);
			if (node != NULL) data->Remove_Node(node);
			break;
		}


		case 0x4B5:
		case 0x4B7:
			wide_messages = FALSE;
		case 0x4B3:
		case 0x4B6:
		case 0x4CF:
		case LB_GETTEXTLEN: {
			call_default = 0;
			if (lparam && message != LB_GETTEXTLEN && message != 0x4CF) {
				if (wide_messages) *(wchar_t *)lparam = 0;
				else *(char *)lparam = 0;
			}
			result = CallWindowProc(original_proc, window, LB_GETITEMDATA, wparam, 0);
			if (result != -1) {
				WinNode *node = (WinNode *)result;
				if (node == NULL) break;
				if (message == 0x4CF) {
					result = node->data;
				} else {
					result = wcslen(node->text);
					if (message != LB_GETTEXTLEN) {
						if (wide_messages) wcscpy((wchar_t *)lparam, node->text);
						else WideToCharString((char *)lparam, node->text, -1);
					}
				}
			}
			if (result < 0) result = 0;
			break;
		}


		case 0x4E8: {
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);
			if (x < 0 || y < 0 || x >= client_rect.right || y >= client_rect.bottom) return -1;
			int height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			index = y / height + data->ListBox.topIndex;
			if (index < 0 || index > count - 1) return -1;
			return index;
		}


		case 0x4EF:
			return (LRESULT)data->attachedWindow;


		case WM_DELETEITEM:
			data->Remove_Node((WinNode *)((DELETEITEMSTRUCT *)lparam)->itemData);
			result = 1;
			break;


		case WM_SIZE: {
			if ((int)data->attachedWindow > 1) {
				RECT parent_display;
				Get_Display_Rect(GetParent(window), &parent_display);
				Rect win_display;
				Get_Display_Rect(window, (LPRECT)&win_display);
				MoveWindow(data->attachedWindow, win_display.Width - parent_display.left, win_display.Y - parent_display.top, scrollbar_width, win_display.Height - win_display.Y, TRUE);
			}
			Surface * surface = data->cachedSurface;
			if (surface != NULL) {
				if ((unsigned short)lparam != surface->Get_Width() || HIWORD(lparam) != surface->Get_Height()) {
					On_WM_NCDESTROY(window);
				}
			}
			break;
		}


		case WM_SETFONT: {
			HDC dc = GetDC(window);
			TEXTMETRIC tm;
			GetTextMetrics(dc, &tm);
			ReleaseDC(window, dc);
			SendMessage(window, LB_SETITEMHEIGHT, (WPARAM)-1, (unsigned short)(LOWORD(tm.tmHeight) + 2));
			data->font = (HFONT)wparam;
			return(0);
		}


		case WM_ERASEBKGND:
			return(0);


		case WM_PAINT: {
			wchar_t string[512];
			call_default = 0;
			BitFont *font = data->Font;
			CopyDimmedBackground(content_rect, window, data->alpha);
			DrawBeveledBorder(*AlternateSurface, content_rect, 2, -1);
			int fill_color = ODColorToHiColor(ListSelectionFillColor);
			RECT update_rect;
			if (!GetUpdateRect(window, &update_rect, FALSE)) {
				break;
			}
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			LONG style = GetWindowLong(window, GWL_STYLE);
			index = SendMessage(window, LB_GETTOPINDEX, 0, 0);
			while (index < count) {
				RECT item_rect;
				if (SendMessage(window, LB_GETITEMRECT, index, (LPARAM)&item_rect) != -1) {
					if (client_rect.top + item_rect.bottom > client_rect.bottom) {
						break;
					}
					string[0] = 0;
					LRESULT item = CallWindowProc(original_proc, window, LB_GETITEMDATA, index, 0);
					if (item != LB_ERR && item != 0) {
						Copy_Wide_Text(string, ((WinNode *)item)->text, 512);
					}
					ArrayList<ColumnData> *columns = data->ListBox.columns;
					if (columns != NULL) {
						if (SendMessage(window, LB_GETSEL, index, 0) > 0) {
							Rect fill;
							fill.X = item_rect.left + display_rect.left;
							fill.Y = item_rect.top + display_rect.top;
							fill.Width = item_rect.right - item_rect.left;
							fill.Height = item_rect.bottom - item_rect.top;
							AlternateSurface->Fill_Rect(fill, fill_color);
						}
						ColumnData *column = NULL;
						HDC dc = GetDC(window);
						for (int col = 0; col < columns->length(); ++col) {
							columns->getPointer(&column, col);
							CellData *cell;
							if (!column->cells.getPointer(&cell, index)) {
								continue;
							}
							if (cell == NULL || cell->type == CellData::INVALID) {
								continue;
							}
							if (cell->type == CellData::TEXT || cell->type == CellData::PRIMARY) {
								Rect text_rect;
								text_rect.X = display_rect.left + item_rect.left + column->xPos;
								text_rect.Y = display_rect.top + item_rect.top;
								text_rect.Width = item_rect.right - item_rect.left;
								text_rect.Height = item_rect.bottom - item_rect.top;
								if (cell->type == CellData::TEXT) {
									wcscpy(string, cell->string.get());
								} else if (cell->type == CellData::PRIMARY) {
									string[0] = 0;
									LRESULT item = CallWindowProc(original_proc, window, LB_GETITEMDATA, index, 0);
									if (item != LB_ERR && item != 0) {
										Copy_Wide_Text(string, ((WinNode *)item)->text, 512);
									}
								}
								COLORREF text_color = cell->color;
								if (text_color == -1) {
									text_color = PrimaryTextColor;
								}
								if (style & WS_DISABLED) {
									text_color = DisabledTextColor;
								}
								if (column->width != 0) {
									text_rect.Width = column->width;
								}
								int max_width = text_rect.Width;
								if (max_width == 0) max_width = 0xFFFF;
								int overflow = display_rect.right - max_width - item_rect.left - display_rect.left - column->xPos;
								if (overflow < 0) {
									max_width += overflow;
								}
								SendMessage(window, OD_RESTOREDC, 0, (LPARAM)dc);
								int length = wcslen(string);
								Point2D text_dimensions;
								font->GetTextDimension(string, &text_dimensions.X, &text_dimensions.Y, 0);
								while (length && text_dimensions.X > max_width) {
									string[--length] = 0;
									wcscat(string, L"...");
									font->GetTextDimension(string, &text_dimensions.X, &text_dimensions.Y, 0);
								}
								PrintTextFixedLength(text_color, font, text_rect, string, wcslen(string), 0, 0, NULL, 0);
							} else if (cell->type == CellData::SURFACE) {
								if (cell->surf != NULL) {
									int height = cell->surf->Get_Height();
									int width = cell->surf->Get_Width();
									Rect surface_rect;
									surface_rect.Width = width;
									surface_rect.X = display_rect.left + item_rect.left + column->xPos;
									surface_rect.Height = height;
									surface_rect.Y = display_rect.top + item_rect.top + (item_rect.bottom - item_rect.top - height) / 2;
									SurfaceCache.DrawTrans(surface_rect, *AlternateSurface, *cell->surf, DSurface::Build_Hicolor_Pixel(255, 0, 255));
								}
							} else if (cell->type == CellData::PING) {
								int ping = cell->pingtime;
								Rect ping_rect;
								ping_rect.X = display_rect.left + item_rect.left + column->xPos;
								ping_rect.Y = display_rect.top + item_rect.top;
								ping_rect.Width = 32;
								ping_rect.Height = 12;
								unsigned color;
								if (ping < 0) {
									color = DSurface::Build_Hicolor_Pixel(0, 0, 192);
									ping = 1000;
								} else if (ping < 300) {
									color = DSurface::Build_Hicolor_Pixel(0, 192, 0);
								} else if (ping < 500) {
									color = DSurface::Build_Hicolor_Pixel(192, 192, 0);
								} else {
									color = DSurface::Build_Hicolor_Pixel(192, 0, 0);
								}
								BlendGradientRect(ping_rect, *AlternateSurface, color, (ping << 16) / 1000);
							}
						}
						ReleaseDC(window, dc);
					} else {
						ArrayList<int> *row_colors = data->ListBox.rowColors;
						int text_color;
						if (row_colors == NULL || !row_colors->get(text_color, index) || text_color == -1) {
							text_color = PrimaryTextColor;
						}
						if (style & WS_DISABLED) text_color = DisabledTextColor;
						if (SendMessage(window, LB_GETSEL, index, 0) > 0) {
							Rect fill;
							fill.X = item_rect.left + display_rect.left;
							fill.Y = item_rect.top + display_rect.top;
							fill.Width = item_rect.right - item_rect.left;
							fill.Height = item_rect.bottom - item_rect.top;
							AlternateSurface->Fill_Rect(fill, fill_color);
						}
						Rect text_rect;
						text_rect.X = item_rect.left + display_rect.left + 2;
						text_rect.Y = display_rect.top + item_rect.top;
						text_rect.Width = item_rect.right - item_rect.left;
						text_rect.Height = item_rect.bottom - item_rect.top;
						PrintTextFixedLength(text_color, font, text_rect, string, wcslen(string), 0, 0, NULL, 0);
					}
				}
				++index;
			}
			ValidateRect(window, &update_rect);
			if (data->attachedWindow) InvalidateRect(data->attachedWindow, NULL, FALSE);
			break;
		}

		case LB_SELITEMRANGE: {
			int last = HIWORD(lparam);
			int first = LOWORD(lparam);
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			if (first < 0 || last < 0 || last < first) {
				return(-1);
			}
			if (last >= count) {
				last = count - 1;
			}
			ArrayList<int> * sel_states = data->ListBox.selStates;
			if (sel_states == NULL) {
				sel_states = new ArrayList<int>;
				data->ListBox.selStates = sel_states;
			}
			if (last >= sel_states->length()) {
				int unselected = 0;
				sel_states->setSize(last + 1, unselected);
			}
			for (int i = first; i <= last; i++) {
				int selected = (int)wparam;
				sel_states->replace(selected, i);
			}
			parent = GetParent(window);
			SendMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x10000, (LPARAM)window);
			return(0);
		}


		case WM_VSCROLL: {
			call_default = 0;
			LRESULT position = SendMessage(data->attachedWindow, SBM_GETPOS, 0, 0);
			if (position != SendMessage(window, LB_GETTOPINDEX, 0, 0)) {
				SendMessage(window, LB_SETTOPINDEX, position, 0);
			}
			break;
		}


		case LB_DELETESTRING: {
			ArrayList<int> * row_colors = data->ListBox.rowColors;
			if (row_colors != NULL && row_colors->length() > (int)wparam) {
				row_colors->remove((int)wparam);
			}
			ArrayList<int> * sel_states = data->ListBox.selStates;
			if (sel_states != NULL && sel_states->length() > (int)wparam) {
				sel_states->remove((int)wparam);
			}
			ArrayList<ColumnData> * columns = data->ListBox.columns;
			if (columns != NULL) {
				ColumnData * column = NULL;
				for (int col = 0; col < columns->length(); col++) {
					columns->getPointer(&column, col);
					column->cells.remove((int)wparam);
				}
			}
			break;
		}


		case WM_NCDESTROY:
		case LB_RESETCONTENT: {
			ArrayList<int> * row_colors = data->ListBox.rowColors;
			if (row_colors != NULL) {
				delete row_colors;
			}
			ArrayList<int> * sel_states = data->ListBox.selStates;
			if (sel_states != NULL) {
				delete sel_states;
			}
			data->ListBox.rowColors = 0;
			data->ListBox.selStates = 0;
			data->ListBox.topIndex = 0;
			data->ListBox.curSel = -1;

			ArrayList<ColumnData> * columns = data->ListBox.columns;
			if (columns != NULL) {
				ColumnData * column = NULL;
				for (int col = 0; col < columns->length(); col++) {
					columns->getPointer(&column, col);
					column->cells.clear();
				}
			}

			if (message == WM_NCDESTROY) {
				if (columns != NULL) {
					delete columns;
				}
				data->ListBox.columns = NULL;
			} else {
				parent = GetParent(window);
				SendMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x10000, (LPARAM)window);
			}
			break;
		}



	}


	if (needs_scrollbar == 1) {
		if (data->attachedWindow == NULL) {
			data->attachedWindow = (HWND)1;
			GetWindowLong(window, GWL_ID);
			parent = GetParent(window);
			RECT parent_display;
			Get_Display_Rect(parent, &parent_display);
			RECT win_display;
			Get_Display_Rect(window, &win_display);
			int x = win_display.left - parent_display.left;
			int y = win_display.top - parent_display.top;
			data->attachedWindow = CreateWindowEx(0, "Scrollbar", NULL, 0x50010001,
				win_display.left - parent_display.left - scrollbar_width + client_rect.right + 1,
				client_rect.top + win_display.top - parent_display.top,
				scrollbar_width, win_display.bottom - win_display.top,
				parent, NULL, ProgramInstance, NULL);
			data->scrollBarWidth = scrollbar_width;
			InitializeCtrl(data->attachedWindow, 0);

			WinData * sb_data = NULL;
			Dialogs.getPointer(data->attachedWindow, &sb_data);
			sb_data->ownerWindow = window;
			sb_data->disabled = false;

			SCROLLINFO info;
			info.nMax = max_position;
			info.fMask = SIF_RANGE | SIF_POS;
			info.nMin = 0;
			info.nPos = data->ListBox.topIndex;
			info.cbSize = sizeof(SCROLLINFO);
			SendMessage(data->attachedWindow, SBM_SETSCROLLINFO, 0, (LPARAM)&info);

			SetWindowPos(window, NULL, 0, 0, win_display.right - win_display.left - scrollbar_width, win_display.bottom - win_display.top, SWP_NOMOVE);
			ShowWindow(data->attachedWindow, SW_SHOW);
			BringWindowToTop(data->attachedWindow);
			InvalidateRect(data->attachedWindow, NULL, FALSE);
			UpdateWindow(data->attachedWindow);

			Rect validate_rect;
			validate_rect.X = x;
			validate_rect.Y = y;
			validate_rect.Width = x + client_rect.right + 1;
			validate_rect.Height = client_rect.bottom + y + 1;
			ValidateRect(parent, (const RECT *)&validate_rect);
		}
	} else if (needs_scrollbar == 0 && data->attachedWindow != NULL && !data->paintDisabled) {
		DestroyWindow(data->attachedWindow);
		HWND scrollbar = data->attachedWindow;
		ODRemoveFromDict(scrollbar, 0);
		data->attachedWindow = NULL;
		data->scrollBarWidth = 0;
		SetWindowPos(window, NULL, 0, 0, client_rect.right + ControlInsetPx - client_rect.left + scrollbar_width + 1, client_rect.bottom + 2 * ControlInsetPx - client_rect.top, SWP_NOMOVE);
		parent = GetParent(window);
		RECT parent_display;
		Get_Display_Rect(parent, &parent_display);
		RECT validate_rect;
		validate_rect.left = display_rect.left - parent_display.left - 1;
		validate_rect.top = display_rect.top - parent_display.top - 1;
		validate_rect.right = scrollbar_width + client_rect.right + display_rect.left - parent_display.left;
		validate_rect.bottom = display_rect.top - parent_display.top + client_rect.bottom + 1;
		ValidateRect(parent, &validate_rect);
	}

	if (call_default) {
		result = CallWindowProc(original_proc, window, message, wparam, lparam);
	}

	if (message == WM_NCDESTROY) {
		ArrayList<int> * row_colors = data->ListBox.rowColors;
		if (row_colors != NULL) {
			delete row_colors;
		}
		ArrayList<int> * sel_states = data->ListBox.selStates;
		if (sel_states != NULL) {
			delete sel_states;
		}
	}

	return(result);
}



// SYNTHETIC: GAMEMD 0x00624460
// ??1CellData@OwnerDraw@@QAE@XZ

// TEMPLATE: GAMEMD 0x006244E0
// ?clear@?$ArrayList@UCellData@OwnerDraw@@@@QAEXXZ

// TEMPLATE: GAMEMD 0x006254D0
// ?shrinkVector@?$ArrayList@UColumnData@OwnerDraw@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x00625A20
// ?growVector@?$ArrayList@UColumnData@OwnerDraw@@@@AAEDXZ
