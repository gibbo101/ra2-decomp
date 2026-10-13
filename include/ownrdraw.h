// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "arraylist.h"
#include "keyboard.h"
#include "surface.h"
#include "win.h"
#include "winfix.h"
#include "widewstring.h"
#include "wstring.h"

namespace OwnerDraw {

	struct CellData {
		CellData(void);
		CellData(CellData const & that) : type(that.type), string(that.string), hint(that.hint), color(that.color), surf(that.surf), pingtime(that.pingtime)  {}
		CellData const & operator=(CellData & that)
		{
			type = that.type;
			string = that.string;
			hint = that.hint;
			color = that.color;
			surf = that.surf;
			pingtime = that.pingtime;
			return(*this);
		}

		enum DataType {
			INVALID,
			TEXT,
			SURFACE,
			PING,
			PRIMARY,
		};

		DataType type;

		WideWstring string;

		WideWstring hint;

		int color;

		Surface * surf;

		int pingtime;
	};

	struct ColumnData {
		ColumnData(void) : xPos(0), width(0), cells() {}
		ColumnData(ColumnData & that) : xPos(that.xPos), width(that.width), cells(that.cells) {}

		int xPos;

		int width;

		ArrayList<CellData> cells;
	};

	struct Tooltip {

		Rect bounds;

		Surface * background;

		char text[128];

		int isActive;

		int isHidden;

		HWND window;

		Tooltip(void);
	};

	struct CtrlMsgData {

		UINT message;
		HWND window;

		bool operator ==(CtrlMsgData const & that) const;
	};

	struct FontMetrics {
		int charWidths[256];
		int glyphWidth;
		int glyphHeight;
		int topMargin;
		int leftMargin;
	};

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
				int selIndex;
				int reserved2C;
				int reserved30;
				HWND dropdown;
				int reserved38[6];
				COLORREF itemColors[50];
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
	};

	void Initialize(void);
	void Register_Control_Classes(void);
	bool Subclass_Dialog(HWND window, LPARAM lParam);

	void Draw_Item(LPDRAWITEMSTRUCT drawit);
	void Draw_Dialog_Back(HWND window);

	int Capture_Mouse(void);
	int Release_Mouse(void);

	HWND Begin_Dialog(int id, DLGPROC proc);
	void Display_Dialog(HWND window);
	int Move_Dialog(HWND window, int x, int y);
	void End_Dialog(HWND window);

	int Default_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	bool Dialog_Message_Handler(void);

	bool Start_Tooltip(Rect const & rect, char const * text, HWND window);
	bool Show_Tooltip(bool save_background);
	bool Hide_Tooltip(void);
	bool End_Tooltip(void);

	HWND Custom_Message_Box(const char * btn1txt, const char * btn2txt = NULL, bool * cancelled = NULL);
	void Set_Custom_Message_Box_Text(HWND window, LPCSTR text);
};

#define OD_TEXT_ALIGN_MIN 1
#define OD_TEXT_ALIGN_CENTER 2
#define OD_TEXT_ALIGN_MAX 3

int OD_Draw_Text_Remap(Surface & surface, const char * string, Rect const & rect, char const * name, COLORREF color, int flags, int char_spacing);
int OD_Draw_Text(COLORREF color, HFONT font, Rect const & rect, const char * text, int len, int x_alignment, int y_alignment, Surface * surface);
void OD_Draw_Rect(Surface & surf, Rect const & rect, int offset, int color);

void On_WM_NCDESTROY(HWND window);

int Build_Hotkey_String(KeyNumType key, char * buffer);

extern COLORREF ODColorText;
extern COLORREF ODColorTextDim;
extern COLORREF ODColorDisabled;
extern COLORREF ODColorFrame;
extern COLORREF ODListBoxColor;
extern COLORREF ODColorUnused1;

#define OD_DRAW_CHAR_FLAG_HORIZONTAL_CENTER 1
#define OD_DRAW_CHAR_ALIGN_FLAG_RIGHT 2
#define OD_DRAW_CHAR_FLAG_VERTICAL_CENTER 4

extern unsigned short ODRComponentMask;
extern unsigned short ODGComponentMask;
extern unsigned short ODBComponentMask;

inline unsigned short OD_Blend_Color(unsigned short pixel, unsigned short color, unsigned char alpha)
{
	static unsigned blend_color_alpha;
	static unsigned blend_pixel_alpha;

	blend_color_alpha = alpha;
	blend_pixel_alpha = 255 - alpha;

	unsigned short r = ((((pixel & ODRComponentMask) * blend_pixel_alpha) + ((color & ODRComponentMask) * blend_color_alpha)) >> 8) & ODRComponentMask;
	unsigned short g = ((((pixel & ODGComponentMask) * blend_pixel_alpha) + ((color & ODGComponentMask) * blend_color_alpha)) >> 8) & ODGComponentMask;
	unsigned short b = (((pixel & ODBComponentMask) * blend_pixel_alpha) + ((color & ODBComponentMask) * blend_color_alpha)) >> 8;
	return((unsigned short)(r | g | b));
}

#define OD_SUBCLASSED				(WM_USER + 151)

#define OD_SETCOLOR					(WM_USER + 152)

#define OD_TOOLTIPS					(WM_USER + 154)

#define OD_GETTIPTEXT				(WM_USER + 155)

#define OD_SETIMAGE					(WM_USER + 156)

#define OD_DISABLEPAINT				(WM_USER + 157)

#define OD_RESTOREDC				(WM_USER + 158)

#define OD_SAVEDC					(WM_USER + 159)

#define OD_HASATTACHED				(WM_USER + 160)

#define OD_REFRESHNOPAINT			(WM_USER + 165)

#define OD_ADDCOLUMN				(WM_USER + 166)

#define OD_REMOVECOLUMN				(WM_USER + 167)

#define OD_SETCELL					(WM_USER + 168)

#define OD_SETTOP					(WM_USER + 169)

#define OD_SETALTIMAGE				(WM_USER + 170)

#define OD_SETTRACKSTEP				(WM_USER + 171)

#define OD_TRACKNUMBERS				(WM_USER + 172)

#define OD_GETCELLTIP				(WM_USER + 173)

#define OD_TRACKSILENT				(WM_USER + 174)

#define OD_ACTIVATE					(WM_USER + 175)

#define OD_REFOCUS					(WM_USER + 176)

#define OD_SETKEEPCAPTURE			(WM_USER + 177)

#define OD_DROPSUBCLASSED			(WM_USER + 1000)
