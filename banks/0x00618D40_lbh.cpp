// 0x00618D40 OwnerDraw::ListBoxHandler: 52.9% raw, 73.8% normalised, 14562/14666 bytes, frame 0x2D2C (23 Nov, natural form; file checks 253/254 with the five markers below).
// Splice after AutoRadioButtonHandler 0x00616980 in src/ownrdraw.cpp; the five markers at the end go after `template class DNode<HWND, OwnerDraw::WinData>;` (they are PR #406's, used here for checking only).
// New since the 22 Nov bank: WM_SIZE calls On_WM_NCDESTROY(window) (0x006223C0, inlined by the exe: by-value key copy at [esp+0x14], bool test, then the data test at 0x00619129-0x006191B8); case groups reordered (LB_ group, LB_SELITEMRANGE, mouse, OD_, text, scroll group, WM_ group last) so shrinkVector<int> under LB_SELITEMRANGE's setSize and both keyHash calls under the tail's shrink stay out of line.
// Still differing per site: keyHash(Dialogs) at 0x00618DFC (needs Find_Data(window), which costs 4 bytes of frame in this register regime), the CellData copy constructor at 0x0061B302, keyHash(DialogProcs) at 0x0061C2EA.
// The register picture is a regime, not a tie: see 0x00618D40_lbh_diag.cpp.

void __fastcall FUN_00735060(wchar_t * dest, char const * source, int limit);
char * __fastcall FUN_007350C0(wchar_t const * text);

inline WNDPROC Find_Proc(HWND & window)
{
	WNDPROC proc = NULL;
	WNDPROC * found = NULL;
	if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
		proc = *found;
	}
	return proc;
}

inline int Get_Item_Text(HWND window, int index, wchar_t * buffer, int size)
{
	WNDPROC proc = Find_Proc(window);
	buffer[0] = 0;
	OwnerDraw::WinNode * item = (OwnerDraw::WinNode *)CallWindowProc(proc, window, LB_GETITEMDATA, index, 0);
	if (item == (OwnerDraw::WinNode *)LB_ERR || item == NULL) {
		item = (OwnerDraw::WinNode *)CallWindowProc(proc, window, CB_GETITEMDATA, index, 0);
	}
	if (item != (OwnerDraw::WinNode *)LB_ERR && item != NULL) {
		wcsncpy(buffer, item->text, size);
		buffer[size - 1] = 0;
		return(wcslen(buffer));
	}
	return(0);
}

void On_WM_NCDESTROY(HWND window);

// FUNCTION: GAMEMD 0x00618D40
// OwnerDraw::ListBoxHandler
LRESULT CALLBACK OwnerDraw::ListBoxHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int wide = 1;
	WNDPROC proc = Find_Proc(window);

	HWND parent = GetParent(window);
	int needs_scrollbar = -1;
	int max_position = 0;
	char call_default = 1;
	LRESULT result = 0;

	WinData * data = NULL;
	Dialogs.getPointer(window, &data);
	if (data == NULL) {
		return(0);
	}

	int scrollbar_width = 2 * ControlInsetPx + 18;

	RECT client_rect;
	GetClientRect(window, &client_rect);
	RECT display_rect;
	Get_Display_Rect(window, &display_rect);
	client_rect.bottom -= 2 * ControlInsetPx;
	client_rect.right -= 2 * ControlInsetPx;
	display_rect.left += ControlInsetPx;
	display_rect.right -= ControlInsetPx;
	display_rect.top += ControlInsetPx;
	display_rect.bottom -= ControlInsetPx;
	Rect content_rect(display_rect.left, display_rect.top, client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);

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

	switch (message) {
		case CB_GETITEMDATA:
		case CB_SETITEMDATA:
		case LB_GETITEMDATA:
		case LB_SETITEMDATA: {
			call_default = 0;
			result = CallWindowProc(proc, window, LB_GETITEMDATA, wparam, 0);
			if (result != LB_ERR) {
				if (result == 0) {
					result = LB_ERR;
				} else {
					WinNode * item = (WinNode *)result;
					if (message == LB_GETITEMDATA || message == CB_GETITEMDATA) {
						result = item->id;
					} else {
						item->id = lparam;
					}
				}
			}
			break;
		}

		case LB_SETTOPINDEX: {
			int index = (int)wparam;
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

		case LB_SETSEL: {
			int index = (int)lparam;
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
			int last = MAX(index, SendMessage(window, LB_GETCOUNT, 0, 0) - 1);
			if (last >= sel_states->length()) {
				int unselected = 0;
				sel_states->setSize(last + 1, unselected);
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
				int * value = NULL;
				if (sel_states->getPointer(&value, i)) {
					selected = *value;
				}
				if (selected) {
					total++;
				}
			}
			return(total);
		}

		case LB_GETSELITEMS: {
			ArrayList<int> * sel_states = data->ListBox.selStates;
			int total = 0;
			if (sel_states != NULL) {
				int selected = 0;
				if (sel_states->length() > 0) {
					int * out = (int *)lparam;
					int max = (int)wparam;
					for (int i = 0; i < sel_states->length(); i++) {
						int * value = NULL;
						if (sel_states->getPointer(&value, i)) {
							selected = *value;
						}
						if (selected) {
							*out = i;
							total++;
							out++;
						}
						if (total >= max) {
							break;
						}
					}
				}
			}
			return(total);
		}

		case LB_GETSEL: {
			ArrayList<int> * sel_states = data->ListBox.selStates;
			if (sel_states == NULL) {
				return(0);
			}
			if ((int)wparam >= sel_states->length()) {
				return(0);
			}
			int * selected = NULL;
			if (!sel_states->getPointer(&selected, (int)wparam)) {
				return(0);
			}
			return(*selected);
		}

		case LB_SETCURSEL: {
			int index = (int)wparam;
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
			int index = (int)wparam;
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
			out->bottom = relative * item_height + item_height;
			out->left = client_rect.left;
			out->right = client_rect.right - client_rect.left;
			return(0);
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

		case WM_LBUTTONDBLCLK:
			PostMessage(parent, WM_COMMAND, (GetWindowLong(window, GWL_ID) & 0xFFFF) | 0x20000, (LPARAM)window);
			return(0);

		case WM_LBUTTONDOWN: {
			int top = SendMessage(window, LB_GETTOPINDEX, 0, 0);
			int item_height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
			int index = top + (int)HIWORD(lparam) / item_height;
			if (index >= SendMessage(window, LB_GETCOUNT, 0, 0)) {
				return(0);
			}
			LONG style = GetWindowLong(window, GWL_STYLE);
			SetFocus(window);
			int paint_disabled = SendMessage(window, OD_DISABLEPAINT, 0, 1);
			if ((style & LBS_MULTIPLESEL) != 0) {
				int select = (SendMessage(window, LB_GETSEL, index, 0) == 0);
				VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000);
				SendMessage(window, LB_SETSEL, select, index);
			} else if ((style & LBS_NOSEL) == 0) {
				VocClass::PlayGlobal(Rule->GenericClick, 1.0f, 0x2000);
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
			int index = (int)wparam;
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
			SendMessage(window, LB_SETITEMHEIGHT, (WPARAM)-1, (unsigned short)(data->Font->field_1C + 2));
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
					column = NULL;
					columns->getPointer(&column, best_column);
					if (column != NULL && row < column->cells.length()) {
						CellData * cell = NULL;
						column->cells.getPointer(&cell, row);
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
				return(-1);
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

		case WM_USER + 196:
		case WM_USER + 197:
		case WM_USER + 198: {
			wchar_t converted[0x1400];
			FUN_00735060(converted, (char const *)lparam, 0x1400);
			lparam = (LPARAM)converted;
		}
		case WM_USER + 201:
		case WM_USER + 202:
		case WM_USER + 203: {
			call_default = 0;
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			int found_it = 0;
			if (count == LB_ERR) {
				break;
			}
			for (int i = wparam; i < count && !found_it; i++) {
				result = CallWindowProc(proc, window, LB_GETITEMDATA, i, 0);
				if (result != LB_ERR) {
					WinNode * item = (WinNode *)result;
					if (message == WM_USER + 197 || message == WM_USER + 202) {
						found_it = (_wcsicmp((wchar_t const *)lparam, item->text) == 0);
					} else {
						found_it = (_wcsnicmp((wchar_t const *)lparam, item->text, wcslen((wchar_t const *)lparam)) == 0);
					}
					result = i;
				}
			}
			if (!found_it) {
				result = LB_ERR;
			} else if (message == WM_USER + 198) {
				result = SendMessage(window, LB_SETCURSEL, result, 0);
			}
			break;
		}

		case WM_USER + 199:
		case WM_USER + 200:
			wide = 0;
		case WM_USER + 204:
		case WM_USER + 205: {
			call_default = 0;
			if (message == WM_USER + 200 || message == WM_USER + 205) {
				if (!wide) {
					result = CallWindowProc(proc, window, LB_ADDSTRING, 0, lparam);
				} else {
					result = CallWindowProc(proc, window, LB_ADDSTRING, 0, (LPARAM)FUN_007350C0((wchar_t const *)lparam));
				}
			} else {
				if (!wide) {
					result = CallWindowProc(proc, window, LB_INSERTSTRING, wparam, lparam);
				} else {
					result = CallWindowProc(proc, window, LB_INSERTSTRING, wparam, (LPARAM)FUN_007350C0((wchar_t const *)lparam));
				}
			}
			if (result == LB_ERR || result == LB_ERRSPACE) {
				break;
			}
			int index = result;
			int length;
			if (wide) {
				length = wcslen((wchar_t const *)lparam);
			} else {
				length = strlen((char const *)lparam);
			}
			WinNode * node = data->Add_Node(length);
			if (node != NULL) {
				node->data = wide;
				if (wide) {
					wcscpy(node->text, (wchar_t const *)lparam);
				} else {
					FUN_00735060(node->text, (char const *)lparam, -1);
				}
				result = CallWindowProc(proc, window, LB_SETITEMDATA, index, (LPARAM)node);
				if (result != LB_ERR && result != LB_ERRSPACE) {
					result = index;
					int position = index;
					if (message == WM_USER + 200) {
						position = -1;
					}
					if (position != -1) {
						ArrayList<int> * row_colors = data->ListBox.rowColors;
						if (row_colors != NULL && position < row_colors->length()) {
							int color = -1;
							row_colors->add(color, position);
						}
						ArrayList<int> * sel_states = data->ListBox.selStates;
						if (sel_states != NULL && position < sel_states->length()) {
							int selected = 0;
							sel_states->add(selected, position);
						}
					}
					if (position < 0) {
						position = SendMessage(window, LB_GETCOUNT, 0, 0);
					}

					ArrayList<ColumnData> * columns = data->ListBox.columns;
					CellData cell;
					if (columns != NULL) {
						for (int col = 0; col < columns->length(); col++) {
							cell.type = (col == 0) ? CellData::PRIMARY : CellData::INVALID;
							ColumnData * column = NULL;
							columns->getPointer(&column, col);
							column->cells.add(cell, position);
						}
					}
					break;
				}
			} else {
				result = LB_ERRSPACE;
			}
			CallWindowProc(proc, window, LB_DELETESTRING, index, 0);
			if (node != NULL) {
				data->Remove_Node(node);
			}
			break;
		}

		case WM_USER + 181:
		case WM_USER + 183:
			wide = 0;
		case LB_GETTEXTLEN:
		case WM_USER + 179:
		case WM_USER + 182:
		case WM_USER + 207: {
			call_default = 0;
			if (lparam != 0 && message != LB_GETTEXTLEN && message != WM_USER + 207) {
				if (wide) {
					*(wchar_t *)lparam = 0;
				} else {
					*(char *)lparam = 0;
				}
			}
			result = CallWindowProc(proc, window, LB_GETITEMDATA, wparam, 0);
			if (result != LB_ERR && result != 0) {
				WinNode * item = (WinNode *)result;
				if (message == WM_USER + 207) {
					result = item->data;
				} else {
					result = wcslen(item->text);
					if (message != LB_GETTEXTLEN) {
						if (wide) {
							wcscpy((wchar_t *)lparam, item->text);
						} else {
							WideToCharString((char *)lparam, item->text, -1);
						}
					}
				}
			}
			if (result < 0) {
				result = 0;
			}
			break;
		}

		case WM_USER + 232: {
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);
			if (x < 0 || y < 0 || x >= client_rect.right || y >= client_rect.bottom) {
				return(-1);
			}
			int item_height = SendMessage(window, LB_GETITEMHEIGHT, 0, 0);
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			int index = y / item_height + data->ListBox.topIndex;
			if (index >= 0 && index <= count - 1) {
				return(index);
			}
			return(-1);
		}

		case WM_USER + 239:
			return((LRESULT)data->attachedWindow);

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
				for (int col = 0; col < columns->length(); col++) {
					ColumnData * column = NULL;
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
			data->ListBox.rowColors = NULL;
			data->ListBox.selStates = NULL;
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

		case WM_DELETEITEM:
			data->Remove_Node((WinNode *)((DELETEITEMSTRUCT *)lparam)->itemData);
			result = 1;
			break;

		case WM_SIZE: {
			if ((int)data->attachedWindow > 1) {
				RECT parent_display;
				Get_Display_Rect(GetParent(window), &parent_display);
				RECT win_display;
				Get_Display_Rect(window, &win_display);
				MoveWindow(data->attachedWindow, win_display.right - parent_display.left, win_display.top - parent_display.top, scrollbar_width, win_display.bottom - win_display.top, TRUE);
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
			call_default = 0;
			BitFont * font = data->Font;
			CopyDimmedBackground(content_rect, window, data->alpha);
			DrawBeveledBorder(*AlternateSurface, content_rect, 2, -1);

			int fill_color = ODColorToHiColor(ListSelectionFillColor);

			RECT update_rect;
			if (!GetUpdateRect(window, &update_rect, FALSE)) {
				break;
			}

			wchar_t string[512];
			int count = SendMessage(window, LB_GETCOUNT, 0, 0);
			LONG style = GetWindowLong(window, GWL_STYLE);
			int index = SendMessage(window, LB_GETTOPINDEX, 0, 0);
			while (index < count) {
				RECT item_rect;
				if (SendMessage(window, LB_GETITEMRECT, index, (LPARAM)&item_rect) != -1) {
					if (client_rect.top + item_rect.bottom > client_rect.bottom) {
						break;
					}
					Get_Item_Text(window, index, string, 512);

					ArrayList<ColumnData> * columns = data->ListBox.columns;
					if (columns != NULL) {
						if (SendMessage(window, LB_GETSEL, index, 0) > 0) {
							Rect fill;
							fill.X = item_rect.left + display_rect.left;
							fill.Y = item_rect.top + display_rect.top;
							fill.Width = item_rect.right - item_rect.left;
							fill.Height = item_rect.bottom - item_rect.top;
							AlternateSurface->Fill_Rect(fill, fill_color);
						}

						HDC dc = GetDC(window);
						ColumnData * column = NULL;
						for (int col = 0; col < columns->length(); col++) {
							columns->getPointer(&column, col);
							CellData * cell = NULL;
							column->cells.getPointer(&cell, index);
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
									Get_Item_Text(window, index, string, 512);
								}
								int text_color = cell->color;
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
								if (max_width == 0) {
									max_width = 0xFFFF;
								}
								int overflow = display_rect.right - max_width - item_rect.left - display_rect.left - column->xPos;
								if (overflow < 0) {
									max_width += overflow;
								}
								SendMessage(window, OD_RESTOREDC, 0, (LPARAM)dc);
								int length = wcslen(string);
								int width;
								int height;
								font->GetTextDimension(string, &width, &height, 0);
								while (length && width > max_width) {
									string[length - 1] = 0;
									length--;
									wcscat(string, L"...");
									font->GetTextDimension(string, &width, &height, 0);
								}
								PrintTextFixedLength(text_color, font, text_rect, string, wcslen(string), 0, 0, NULL, 0);
							} else if (cell->type == CellData::SURFACE) {
								Surface * surface = cell->surf;
								if (surface != NULL) {
									int height = surface->Get_Height();
									int width = surface->Get_Width();
									Rect surface_rect;
									surface_rect.Width = width;
									surface_rect.X = display_rect.left + item_rect.left + column->xPos;
									surface_rect.Height = height;
									surface_rect.Y = display_rect.top + item_rect.top + (item_rect.bottom - item_rect.top - height) / 2;
									SurfaceCache.DrawTrans(surface_rect, *AlternateSurface, *surface, DSurface::Build_Hicolor_Pixel(255, 0, 255));
								}
							} else if (cell->type == CellData::PING) {
								int ping = cell->pingtime;
								Rect ping_rect;
								ping_rect.X = display_rect.left + item_rect.left + column->xPos;
								ping_rect.Y = display_rect.top + item_rect.top;
								ping_rect.Width = 32;
								ping_rect.Height = 12;
								int color;
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
						ArrayList<int> * row_colors = data->ListBox.rowColors;
						int text_color;
						int * color_ptr = NULL;
						if (row_colors == NULL || !row_colors->getPointer(&color_ptr, index) || *color_ptr == -1) {
							text_color = PrimaryTextColor;
						} else {
							text_color = *color_ptr;
						}
						if (style & WS_DISABLED) {
							text_color = DisabledTextColor;
						}

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
				index++;
			}
			ValidateRect(window, &update_rect);
			if (data->attachedWindow != NULL) {
				InvalidateRect(data->attachedWindow, NULL, FALSE);
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

			RECT validate_rect;
			validate_rect.left = x;
			validate_rect.top = y;
			validate_rect.right = x + client_rect.right + 1;
			validate_rect.bottom = client_rect.bottom + y + 1;
			ValidateRect(parent, &validate_rect);
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
		result = CallWindowProc(proc, window, message, wparam, lparam);
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

// TEMPLATE: GAMEMD 0x006248A0
// ?shrinkVector@?$ArrayList@UCellData@OwnerDraw@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x006254D0
// ?shrinkVector@?$ArrayList@UColumnData@OwnerDraw@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x00625A20
// ?growVector@?$ArrayList@UColumnData@OwnerDraw@@@@AAEDXZ
