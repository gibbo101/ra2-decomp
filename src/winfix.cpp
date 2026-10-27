// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "winfix.h"
#include "ini.h"
#include "misc.h"
#include "ownrdraw.h"
#include "trim.h"
#include "xmouse.h"
#include <commctrl.h>

// SYNTHETIC: GAMEMD 0x00776210 SYMBOL
// _$E3

char *TreeView_Text;

HWND TreeView_LastHandle;

static bool s_dragging;

HTREEITEM TreeView_DraggedItem;

HIMAGELIST TreeView_ImageList;

struct item_data
{
	HWND hwnd1;
	HWND hwnd2;
	HWND hwnd3;
	HTREEITEM item;
	POINT point1;
	POINT point2;
	POINT point3;
};

item_data TreeView_get_item_data(DWORD xy);

bool TreeView_On_Mouse_Move(LPARAM lParam);

char const *last_view_ini_section_name;

#define AUD_UNCOMP_CHUNK_SIZE 2048
#define AUD_FILL_SIZE ((unsigned)AUD_UNCOMP_CHUNK_SIZE * sizeof(unsigned short))

// FUNCTION: GAMEMD 0x00776230
bool TreeView_Begin_Drag(HWND window, HWND hwndTV, LPNMTREEVIEW lpnmtv)
{
	if (lpnmtv->itemNew.lParam != 0) {
		if (SendMessage(window, TVDRAG_BEGIN, (WPARAM)hwndTV, (LPARAM)lpnmtv->itemNew.hItem) == 0) {
			return(false);
		}
		TreeView_DraggedItem = lpnmtv->itemNew.hItem;

		TreeView_SelectItem(hwndTV, 0);

		TreeView_ImageList = TreeView_CreateDragImage(hwndTV, lpnmtv->itemNew.hItem);

		RECT rcItem;
		TreeView_GetItemRect(hwndTV, lpnmtv->itemNew.hItem, &rcItem, false);

		int indent = TreeView_GetIndent(hwndTV);

		int hotspotx = lpnmtv->ptDrag.x - indent - rcItem.left;
		int hotspoty = lpnmtv->ptDrag.y - rcItem.top;

		SetCapture(window);

		ImageList_BeginDrag(TreeView_ImageList, 0, hotspotx, hotspoty);
		ImageList_DragEnter(window, 0, 0);

		SetTimer(window, 1, 1, NULL);

		s_dragging = true;
		TreeView_LastHandle = hwndTV;
	}
	return(s_dragging);
}

// FUNCTION: GAMEMD 0x00776320
bool TreeView_End_Drag(LPARAM lParam)
{
	if (s_dragging) {
		bool ok = TreeView_On_Mouse_Move(lParam);
		ImageList_EndDrag();

		ReleaseCapture();

		ImageList_Destroy(TreeView_ImageList);

		TreeView_ImageList = NULL;
		s_dragging = false;

		item_data data;
		data = TreeView_get_item_data(lParam);

		if (ok) {
			SendMessage(data.hwnd2, TVDRAG_DROP, (WPARAM)&data, lParam);
		}

		KillTimer(data.hwnd2, 1);

		TreeView_SelectDropTarget(TreeView_LastHandle, 0);

		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00776490
item_data TreeView_get_item_data(DWORD xy)
{
	item_data data;

	POINT point;
	point.x = LOWORD(xy);
	point.y = HIWORD(xy);

	HWND hwnd1 = ChildWindowFromPoint((HWND)GetWindowLong(TreeView_LastHandle, GWL_HWNDPARENT), point);
	HWND hwnd2 = (HWND)GetWindowLong(hwnd1, GWL_HWNDPARENT);
	if ((LONG)hwnd1 == GetWindowLong(TreeView_LastHandle, GWL_HWNDPARENT)) {
		hwnd2 = hwnd1;
	}

	RECT rect;
	POINT screen;
	screen.x = point.x;
	screen.y = point.y;

	ClientToScreen(hwnd2, &screen);
	data.point1 = screen;

	GetWindowRect(hwnd1, &rect);
	screen.x -= rect.left;
	screen.y -= rect.top;

	data.hwnd1 = hwnd1;
	data.hwnd2 = hwnd2;
	data.hwnd3 = TreeView_LastHandle;
	data.item = TreeView_DraggedItem;
	data.point2 = point;
	data.point3 = screen;
	return(data);
}

// FUNCTION: GAMEMD 0x00776570
void TreeView_handle_item_drag(int timer_id)
{
	if (timer_id == 1 && s_dragging) {
		POINT cursor;
		GetCursorPos(&cursor);

		RECT rect;
		GetWindowRect(TreeView_LastHandle, &rect);

		LONG i1 = rect.top - cursor.y;
		LONG i2 = cursor.y - rect.bottom;

		i2 = MAX(rect.top - cursor.y, cursor.y - rect.bottom);

		int i3 = MAX(i2, 0);

		if (i3 > 0) {
			int time = 500 - 40 * i3;
			time = MAX(time, 5);
			SetTimer((HWND)GetWindowLong(TreeView_LastHandle, GWL_HWNDPARENT), 1, time, NULL);
			HTREEITEM item;
			HTREEITEM visible;

			item = TreeView_GetFirstVisible(TreeView_LastHandle);

			if (i1 > 0) {
				visible = TreeView_GetPrevVisible(TreeView_LastHandle, item);
			} else {
				visible = TreeView_GetNextVisible(TreeView_LastHandle, item);
			}

			ImageList_DragShowNolock(FALSE);
			TreeView_SelectSetFirstVisible(TreeView_LastHandle, visible);
			ImageList_DragShowNolock(TRUE);
		}
	}
}

// FUNCTION: GAMEMD 0x00776670
bool TreeView_On_Mouse_Move(LPARAM lParam)
{
	bool ok;
	if (s_dragging) {
		ImageList_DragShowNolock(FALSE);

		item_data data;
		data = TreeView_get_item_data(lParam);

		if (SendMessage(data.hwnd2, TVDRAG_OVER, (WPARAM)&data, lParam) != 0) {
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			ok = true;
		} else {
			SetCursor(LoadCursor(NULL, IDC_NO));
			ok = false;
		}

		ImageList_DragShowNolock(TRUE);

		RECT rect;
		GetWindowRect(data.hwnd2, &rect);

		ImageList_DragMove(data.point1.x - rect.left, data.point1.y - rect.top);

		return(ok);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00776750
bool TreeView_set_item_image(HWND window, NMTREEVIEW *nmtv, int collapse_image, int expand_image)
{
	bool ret;
	TVITEMA item;

	ret = false;
	item.hItem = nmtv->itemNew.hItem;
	item.mask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;

	switch (nmtv->action) {
		case TVE_EXPAND:
			item.iImage = expand_image;
			item.iSelectedImage = expand_image;
			ret = true;
			break;

		case TVE_COLLAPSE:
			item.iImage = collapse_image;
			item.iSelectedImage = collapse_image;
			ret = false;
			break;
	}

	TreeView_SetItem(window, &item);
	return(ret);
}

// FUNCTION: GAMEMD 0x007767B0
void TreeView_set_edit_text(HWND window, char *text, WPARAM limit)
{
	HWND ctrl = TreeView_GetEditControl(window);
	if (ctrl != NULL) {
		SendMessage(ctrl, EM_SETLIMITTEXT, limit, 0);
		TreeView_Text = text;
		TreeView_LastHandle = window;
	}
}

// FUNCTION: GAMEMD 0x007767F0
int TreeView_set_item_text(NMTVDISPINFO *info)
{
	char * text = info->item.pszText;
	if (text != NULL) {
		strtrim(text);
		strcpy(TreeView_Text, text);
		TVITEMA item;
		item.hItem = info->item.hItem;
		item.mask = TVIF_TEXT;
		item.pszText = TreeView_Text;
		TreeView_SetItem(TreeView_LastHandle, &item);
		return(strlen(TreeView_Text));
	}
	return(0);
}

// FUNCTION: GAMEMD 0x00776880
BOOL On_WM_HELP(LPARAM help_info)
{
	HELPINFO *info = (HELPINFO *)help_info;
	HWND window = (HWND)info->hItemHandle;
	return(WinHelp(window, "SUN.HLP", HELP_CONTEXTPOPUP, GetWindowContextHelpId(window)));
}

// FUNCTION: GAMEMD 0x007768A0
BOOL On_WM_CONTEXTMENU(WPARAM window)
{
	DWORD identifiers[4];

	identifiers[3] = identifiers[2] = identifiers[1] = identifiers[0] = 0;

	identifiers[0] = GetWindowLong((HWND)window, GWL_ID);
	identifiers[1] = GetWindowContextHelpId((HWND)window);
	return(WinHelp((HWND)window, "SUN.HLP", HELP_CONTEXTMENU, (ULONG)identifiers));
}

// FUNCTION: GAMEMD 0x007768F0
BOOL CALLBACK read_view_from_ini(HWND window, INIClass const &ini)
{
	char buffer[64];
	char const *section;

	GetClassName(window, buffer, sizeof(buffer));

	if (strcmp(buffer, WC_TREEVIEW) == 0) {
			int i = 0;
			section = last_view_ini_section_name;
			HTREEITEM item = TreeView_GetRoot(window);
			while (item != NULL) {
				i++;
				wsprintf(buffer, "TV%d", i);

				if (ini.Get_Bool(section, buffer, false)) {
					TreeView_Expand(window, item, TVE_EXPAND);
				} else {
					TreeView_Expand(window, item, TVE_COLLAPSE);
				}

				item = TreeView_GetNextSibling(window, item);
			}
	}

	GetClassName(window, buffer, sizeof(buffer));

	if (strcmp(buffer, WC_LISTVIEW) == 0) {
		section = last_view_ini_section_name;
		if (window != NULL) {
			for (int i = 0; i < 10; i++) {
				wsprintf(buffer, "LV%d", i);
				unsigned int width = ListView_GetColumnWidth(window, i);
				width = ini.Get_Int(section, buffer, width);
				if (width < 1000) {
					ListView_SetColumnWidth(window, i, width);
				}
			}
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00776A70
void read_views_from_ini(char const *section, HWND window, INIClass const &ini)
{
	LONG x;
	LONG y;
	int screen_x;
	int screen_y;

	if (window != NULL)
	{
		RECT rect;
		GetWindowRect(window, &rect);

		x = ini.Get_Int(section, "X", rect.left);
		y = ini.Get_Int(section, "Y", rect.top);

		screen_x = GetSystemMetrics(SM_CXSCREEN);
		if (x + rect.right - rect.left > screen_x) {
			x = rect.left + screen_x - rect.right;
		}
		screen_y = GetSystemMetrics(SM_CYSCREEN);
		if (y + rect.bottom - rect.top > screen_y) {
			y = rect.top + screen_y - rect.bottom;
		}

		if (x < 0) {
			x = 0;
		}
		if (y < 0) {
			y = 0;
		}

		SetWindowPos(window, 0, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		last_view_ini_section_name = section;
		EnumChildWindows(window, (WNDENUMPROC)read_view_from_ini, (LPARAM)&ini);
	}
}

// FUNCTION: GAMEMD 0x00776B40
BOOL CALLBACK write_view_to_ini(HWND window, INIClass &ini)
{
	char buf[32];
	char buffer[64];
	char const *section;

	GetClassName(window, buffer, sizeof(buffer));

	if (strcmp(buffer, WC_TREEVIEW) == 0) {
			int i = 0;

			section = last_view_ini_section_name;
			HTREEITEM item = TreeView_GetRoot(window);

			while (item != NULL) {
				i++;
				wsprintf(buf, "TV%d", i);

				TVITEM *tmp = (TVITEM *)buffer;
				tmp->mask = TVIF_HANDLE|TVIF_STATE;
				tmp->stateMask = TVIS_EXPANDED;
				tmp->hItem = item;

				if (TreeView_GetItem(window, tmp)) {
					bool expanded = false;
					if (tmp->state & TVIS_EXPANDED) {
						expanded = true;
					}
					ini.Put_Bool(section, buf, expanded);
				}
				item = TreeView_GetNextSibling(window, item);
			}
	}

	GetClassName(window, buffer, sizeof(buffer));

	if (strcmp(buffer, WC_LISTVIEW) == 0) {
		section = last_view_ini_section_name;
		if (window != NULL) {
			for (int i = 0; i < 10; i++) {
				wsprintf(buf, "LV%d", i);
				unsigned int width = ListView_GetColumnWidth(window, i);
				if (width < 1000) {
					ini.Put_Int(section, buf, width);
				}
			}
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00776CD0
void write_views_to_ini(char const *section, HWND window, INIClass &ini)
{
	if (window != NULL) {
		RECT rcl;
		GetWindowRect(window, &rcl);
		ini.Put_Int(section, "X", rcl.left);
		ini.Put_Int(section, "Y", rcl.top);
		last_view_ini_section_name = section;
		EnumChildWindows(window, (WNDENUMPROC)write_view_to_ini, (LPARAM)&ini);
	}
}

// FUNCTION: GAMEMD 0x00776D40
const char *Make_Identifier(char *str, int num)
{
	static char _buffer[32];

	if ( str )
	{
		wsprintf(_buffer, "%s%d", str, num);
	}
	else
	{
		wsprintf(_buffer, "%d", num);
	}
	return(_buffer);
}

// FUNCTION: GAMEMD 0x00776D80
bool On_WM_MOVING(HWND window, WPARAM wparam, LPARAM lparam)
{
	RECT *rcl = (RECT *)lparam;

	bool res = false;

	if (rcl->left < 0) {
		rcl->right -= rcl->left;
		rcl->left = 0;
		res = true;
	}

	if (rcl->top < 0) {
		rcl->bottom -= rcl->top;
		rcl->top = 0;
		res = true;
	}

	if (rcl->right > GetSystemMetrics(SM_CXFULLSCREEN))
	{
		rcl->left += GetSystemMetrics(SM_CXFULLSCREEN) - rcl->right;
		rcl->right = GetSystemMetrics(SM_CXFULLSCREEN);
		res = true;
	}
	if (rcl->bottom > GetSystemMetrics(SM_CYFULLSCREEN))
	{
		rcl->top += GetSystemMetrics(SM_CYFULLSCREEN) - rcl->bottom;
		rcl->bottom = GetSystemMetrics(SM_CYFULLSCREEN);
		res = true;
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00776E10
int ComboBox_Find_Item_Data(HWND hwndCtl, WPARAM indexStart, LPARAM data)
{
	int count = SendMessage(hwndCtl, CB_GETCOUNT, 0, 0);

	for (int index = indexStart; index < count; index++) {
		LRESULT res = SendMessage(hwndCtl, CB_GETITEMDATA, index, 0);
		if (res == data) {
			return(index);
		}
	}

	return(-1);
}

// FUNCTION: GAMEMD 0x00776E70
void Capture_Or_Release_Mouse(HWND window, short release)
{
	if (MouseCursor != NULL) {
		if (release == 0) {
			MouseCursor->Capture_Mouse();
		} else {
			MouseCursor->Release_Mouse();
			UINT flags = RDW_INVALIDATE|RDW_INTERNALPAINT|RDW_ERASE|RDW_ALLCHILDREN|RDW_UPDATENOW|RDW_FRAME;
			RedrawWindow(window, NULL, NULL, flags);
		}
	}
}

// FUNCTION: GAMEMD 0x00776EA0
void noop(void)
{
}

// FUNCTION: GAMEMD 0x00776EB0
WindowsVersionInfo::WindowsVersionInfo(void) :
	MajorVersionNumber(0),
	MinorVersionNumber(0),
	BuildNumber(0),
	IsWin9x(false),
	IsWinNT(false)
{
	OSVERSIONINFO version_info;

	version_info.dwOSVersionInfoSize = sizeof(version_info);

	GetVersionEx(&version_info);

	MajorVersionNumber = (int)version_info.dwMajorVersion;
	MinorVersionNumber = (int)version_info.dwMinorVersion;
	BuildNumber = (int)version_info.dwBuildNumber;

	if (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		IsWin9x = true;
	}

	if (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		IsWinNT = true;
	}
	strncpy(AdditionalInfo, version_info.szCSDVersion, sizeof(AdditionalInfo) - 1);
	AdditionalInfo[sizeof(AdditionalInfo) - 1] = '\x0';
}

// FUNCTION: GAMEMD 0x00776F40
char *WindowsVersionInfo::Version_String(void)
{
	static char _ver95[]   	= {"Windows 95 "};
	static char _ver98[]   	= {"Windows 98 "};
	static char _verNT4[]	= {"Windows NT 4 "};
	static char _verNT5[]	= {"Windows NT 5 "};
	static char _unknown[]	= {"Unknown "};

	static char version[256];

	if (Is_Win95()) {
		strcpy (version, _ver95);
	}

	if (Is_Win98()) {
		strcpy (version, _ver98);
	}

	if (Is_WinNT5()) {
		strcpy (version, _verNT5);
	}

	if (Is_WinNT()) {
		strcpy (version, _verNT4);
	}

	strcat (version, AdditionalInfo);

	return(version);
}

// FUNCTION: GAMEMD 0x00777060
void Center_Window_Within_Window(HWND window)
{
	HWND parent = (HWND)GetWindowLong(window, GWL_HWNDPARENT);
	if (parent != NULL) {
		Center_Window_Within_Window(window, parent);
	}
}

// FUNCTION: GAMEMD 0x00777080
void Center_Window_Within_Window(HWND window, HWND parent)
{
	RECT rcl;
	GetClientRect(parent, &rcl);

	if (parent == MainWindow) {
		rcl.right = VideoModeWidth;
		rcl.bottom = VideoModeHeight;
	}

	ClientToScreen(parent, (LPPOINT)&rcl);
	ClientToScreen(parent, (LPPOINT)&rcl.right);
	rcl.right -= rcl.left;
	rcl.bottom -= rcl.top;

	RECT rect;
	GetClientRect(window, &rect);
	ClientToScreen(window, (LPPOINT)&rect);
	ClientToScreen(window, (LPPOINT)&rect.right);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	int x = (rcl.right - rect.right + 1) / 2;
	int y = (rcl.bottom - rect.bottom + 1) / 2;

	if (x < 0) {
		x = 0;
	}
	if (y < 0) {
		y = 0;
	}

	SetWindowPos(window, 0, x, y, -1, -1, SWP_NOSIZE|SWP_NOZORDER);
}

// FUNCTION: GAMEMD 0x00777160
void Prefetch_Audio_Buffer(char *src, int size)
{
	int n1 = (unsigned short)src % AUD_FILL_SIZE;
	int n2 = size - n1;
	char *ptr = src + n1;

	if (n2 >= 1) {
		n2 -= n2 % AUD_FILL_SIZE;

		if (n2 >= 1) {
			int n3 = (16 * AUD_FILL_SIZE);
			int n4 = 0;

			while (n3 < n2) {
				int n5 = ptr[n3 - (16 * AUD_FILL_SIZE)];
				int n6 = ptr[n3] + n4;
				n4 = n5 + n6;
				n3 += AUD_FILL_SIZE;
			}
		}
	}
}
