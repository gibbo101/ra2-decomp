// best 99.0% (6895/6895 bytes), normalised 98.9%, 50 changed full-context lines, on 25 Nov src (fable4); no overlay; every other ownrdraw function stays MATCH. Splice as before: insert after the ODDrawArrowBitmap declaration that follows 0x00616980 (drop this copy of it) and remove Find_Original_Proc from above 0x0061E700 (it moves here).
// From the 96.5% bank: case 0x4E8 assigns the packed point to lparam first (lparam = MAKELPARAM(...); SendMessage(dropdown_window, 0x4E8, 0, lparam)), the spelling of the RA2 twin's Combo_Proc in reference/ra2-1001-decomp (credit RobinDigitalPro); that case is now byte-identical (lparam in ECX, the 5-byte and eax, exe's 6895 bytes, no jump-table padding).
// Still open (three residuals, all shared with the RA2 twin game.exe 0x005F4BA0, whose code is identical here): (1) OD_SUBCLASSED loads field_1C after the CB_GETITEMHEIGHT call where the exe holds it in EDI across it, and the memset's lea edi comes after mov ecx; (2) the open-path clamp loads parent.bottom into EAX (exe: display.bottom); (3) the blend loop reloads surface before surfptr (exe the reverse). Measured: base is at an exact tie, removing one real data use (CB_GETCURSEL or 0x4DE) flips result/data; any hold (named int, static inline accessor on BitFont or WinData, wparam reuse) costs that one unit and flips; an inline-accessor hold plus one real extra data use (probe) gives the exe's OD_SUBCLASSED and frame but residuals (2) and (3) and the memset order stay. CSE'd re-reads, reference locals (Robin's int &heights_set), sizeof operands and nesting changes count for nothing; a named-local hold also moves the frame. TibSun's nested-if OD_SUBCLASSED (reference/tibsun/code/ownrdraw.cpp ComboBoxCtrlProc) copies the tail instead of merging; the || form is right.
void ODDrawArrowBitmap(Surface * surface, Rect const & rect, BOOL upward, BOOL pressed, bool dark);

void __fastcall FUN_0072A9E0(Rect * rect, int amount);
void __fastcall FUN_00735060(wchar_t * dest, char const * source, int limit);
char * __fastcall FUN_007350C0(wchar_t const * text);
extern char DAT_00889F64[];

static inline WNDPROC Find_Original_Proc(HWND &window)
{
	WNDPROC proc = NULL;
	OwnerDraw::DialogProcs.getValue(window, proc);
	return proc;
}

// FUNCTION: GAMEMD 0x00617250
LRESULT CALLBACK OwnerDraw::ComboBoxHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	static wchar_t _buffer[256];
	LRESULT result = 0;
	int wide = 1;
	bool pass_on = true;
	WNDPROC proc = Find_Original_Proc(window);

	RECT display_rect;
	Get_Display_Rect(window, &display_rect);
	RECT client_rect;
	GetClientRect(window, &client_rect);
	RECT window_rect;
	GetWindowRect(window, &window_rect);

	OwnerDraw::WinData *data = Find_Data(window);
	if (data == NULL) {
		return CallWindowProc(proc, window, message, wparam, lparam);
	}

	switch (message) {
	case WM_DESTROY:
		SendMessage(window, CB_SHOWDROPDOWN, FALSE, 0);
		break;

	case CB_GETITEMDATA:
	case CB_SETITEMDATA:
	case 0x199:
	case 0x19A: {
		result = CallWindowProc(proc, window, CB_GETITEMDATA, wparam, 0);
		pass_on = false;
		if (result != CB_ERR) {
			if (result == 0) {
				result = CB_ERR;
			} else {
				OwnerDraw::WinNode *item = (OwnerDraw::WinNode *)result;
				if (message == CB_GETITEMDATA || message == 0x199) {
					result = item->id;
				} else {
					item->id = lparam;
				}
			}
		}
		break;
	}

	case CB_GETCURSEL:
		result = data->ComboBox.reservedF8[0];
		pass_on = false;
		break;

	case CB_SETCURSEL: {
		data->ComboBox.reservedF8[0] = wparam;
		if (wparam != CB_ERR) {
			OwnerDraw::WinNode *item = (OwnerDraw::WinNode *)CallWindowProc(proc, window, CB_GETITEMDATA, wparam, 0);
			if (item != (OwnerDraw::WinNode *)CB_ERR && item != NULL) {
				if (item->data) {
					SendMessage(window, 0x4B2, 0, (LPARAM)item->text);
				} else {
					char buffer[128];
					OwnerDraw::WideToCharString(buffer, item->text, -1);
					SendMessage(window, 0x4B4, 0, (LPARAM)buffer);
				}
			}
		} else {
			SendMessage(window, 0x4B4, 0, (LPARAM)DAT_00889F64);
		}
		if ((GetWindowLong(window, GWL_STYLE) & 3) == CBS_DROPDOWN) {
			pass_on = false;
		}
		InvalidateRect(window, NULL, FALSE);
		break;
	}

	case CB_SHOWDROPDOWN: {
		result = 1;
		pass_on = false;
		if (wparam == 0) {
			if (data->ComboBox.dropdown != NULL) {
				ReleaseCapture();
				SendMessage(GetParent(window), OD_SETTOP, (WPARAM)data->ComboBox.dropdown, 0);

				HWND dropdown_window = data->ComboBox.dropdown;
				DestroyWindow(dropdown_window);

				OwnerDraw::WinData *dropdown_data = Find_Data(dropdown_window);
				if (dropdown_data != NULL && dropdown_data->cachedSurface != NULL) {
					delete dropdown_data->cachedSurface;
					dropdown_data->cachedSurface = NULL;
					OwnerDraw::CachedSurfaceCount--;
				}

				Remove_Entry(OwnerDraw::Dialogs, dropdown_window);
				data->ComboBox.dropdown = NULL;
			}
			break;
		}

		if (data->ComboBox.dropdown != NULL) {
			break;
		}

		RECT parent_rect;
		Get_Display_Rect(GetParent(window), &parent_rect);

		RECT drop_rect;
		SendMessage(window, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)&drop_rect);
		if (drop_rect.bottom > parent_rect.bottom) {
			drop_rect.bottom = parent_rect.bottom;
		}

		int item_height = SendMessage(window, CB_GETITEMHEIGHT, 0, 0);
		int height = drop_rect.bottom - drop_rect.top;
		int visible = (height - 1) / item_height;
		int count = SendMessage(window, CB_GETCOUNT, 0, 0);
		if (count < 1) {
			count = 1;
		}
		if (data->ComboBoxMaxVisibleDropItems > 0) {
			if (count >= data->ComboBoxMaxVisibleDropItems) {
				count = data->ComboBoxMaxVisibleDropItems;
			}
			height = count * item_height + 1;
		} else if (visible > count) {
			height = count * item_height + 1;
		}

		if (display_rect.bottom + height > parent_rect.bottom) {
			height = parent_rect.bottom - display_rect.bottom;
		}

		RECT parent_display;
		RECT combo_display;
		HWND parent = GetParent(window);
		Get_Display_Rect(parent, &parent_display);
		Get_Display_Rect(window, &combo_display);

		int x = combo_display.left - parent_display.left;
		int y = combo_display.top - parent_display.top;
		HWND dropdown_window = CreateWindowEx(
			0,
			"ComboDropWin",
			NULL,
			WS_CHILD,
			x,
			client_rect.bottom + y + 1,
			client_rect.right,
			height - height % item_height,
			GetParent(window),
			NULL,
			ProgramInstance,
			window);

		OwnerDraw::WinData *dropdown_data = NULL;
		if (!OwnerDraw::Dialogs.getPointer(dropdown_window, &dropdown_data)) {
			OwnerDraw::WinData tmp;
			OwnerDraw::Dialogs.add(dropdown_window, tmp);
		}

		FUN_0053E3C0(dropdown_window);
		FUN_0053E3C0(dropdown_window);
		SendMessage(dropdown_window, 0x7E8, 0, 0);
		SendMessage(GetParent(window), OD_SETTOP, (WPARAM)dropdown_window, 1);
		SetCapture(dropdown_window);
		ShowWindow(dropdown_window, SW_SHOWNORMAL);

		data->ComboBox.dropdown = dropdown_window;
		break;
	}

	case WM_COMMAND:
		if (HIWORD(wparam) == EN_CHANGE) {
			if ((GetWindowLong(window, GWL_STYLE) & 3) != 3) {
				SendMessage(GetParent(window), message, MAKEWPARAM(GetDlgCtrlID(window), CBN_EDITCHANGE), (LPARAM)window);
				pass_on = false;
			}
		}
		break;

	case WM_DELETEITEM: {
		DELETEITEMSTRUCT *item = (DELETEITEMSTRUCT *)lparam;
		data->Remove_Node((OwnerDraw::WinNode *)item->itemData);
		if (data->ComboBox.reservedF8[0] == (int)item->itemID) {
			data->ComboBox.reservedF8[0] = -1;
		}
		break;
	}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		VocClass::PlayGlobal(Rule->GUIComboOpenSound, 1.0f, 0x2000);
		if ((unsigned short)LOWORD(lparam) > client_rect.right - 20) {
			BOOL show = (SendMessage(window, CB_GETDROPPEDSTATE, 0, 0) != 1);
			PostMessage(window, CB_SHOWDROPDOWN, show, 0);
		}
		result = 0;
		pass_on = false;
		break;

	case 0x4DA:
	case 0x4DB: {
		HWND parent = GetParent(window);
		SendMessage(parent, message, wparam, lparam);
		break;
	}

	case 0x4B8:
	case 0x4B9:
	case 0x4BA: {
		wchar_t converted[0x1400];
		FUN_00735060(converted, (char const *)lparam, 0x1400);
		lparam = (LPARAM)converted;
	}

	case 0x4BE:
	case 0x4BF:
	case 0x4C0: {
		int count = CallWindowProc(proc, window, CB_GETCOUNT, 0, 0);
		int found_it = 0;
		pass_on = false;
		if (count == CB_ERR) {
			break;
		}
		for (int i = wparam; i < count && !found_it; i++) {
			result = CallWindowProc(proc, window, CB_GETITEMDATA, i, 0);
			if (result != CB_ERR) {
				OwnerDraw::WinNode *item = (OwnerDraw::WinNode *)result;
				if (message == 0x4B9 || message == 0x4BF) {
					found_it = (_wcsicmp((wchar_t const *)lparam, item->text) == 0);
				} else {
					found_it = (_wcsnicmp((wchar_t const *)lparam, item->text, wcslen((wchar_t const *)lparam)) == 0);
				}
				result = i;
			}
		}
		if (!found_it) {
			result = CB_ERR;
		} else if (message == 0x4BA || message == 0x4C0) {
			result = CallWindowProc((WNDPROC)OwnerDraw::ComboBoxHandler, window, CB_SETCURSEL, result, 0);
		}
		break;
	}

	case 0x4BB:
	case 0x4BC:
		wide = 0;

	case 0x4C1:
	case 0x4C2: {
		if (message == 0x4BC || message == 0x4C2) {
			if (!wide) {
				result = CallWindowProc(proc, window, CB_ADDSTRING, 0, lparam);
			} else {
				result = CallWindowProc(proc, window, CB_ADDSTRING, 0, (LPARAM)FUN_007350C0((wchar_t const *)lparam));
			}
		} else {
			if (!wide) {
				result = CallWindowProc(proc, window, CB_INSERTSTRING, wparam, lparam);
			} else {
				result = CallWindowProc(proc, window, CB_INSERTSTRING, wparam, (LPARAM)FUN_007350C0((wchar_t const *)lparam));
			}
		}
		pass_on = false;
		if (result == CB_ERR || result == CB_ERRSPACE) {
			break;
		}
		int index = result;
		int length;
		if (wide) {
			length = wcslen((wchar_t const *)lparam);
		} else {
			length = strlen((char const *)lparam);
		}
		OwnerDraw::WinNode *node = data->Add_Node(length);
		if (node != NULL) {
			node->data = wide;
			if (wide) {
				wcscpy(node->text, (wchar_t const *)lparam);
			} else {
				FUN_00735060(node->text, (char const *)lparam, -1);
			}
			result = CallWindowProc(proc, window, CB_SETITEMDATA, index, (LPARAM)node);
			if (result != CB_ERR && result != CB_ERRSPACE) {
				result = index;
				break;
			}
		} else {
			result = CB_ERRSPACE;
		}
		CallWindowProc(proc, window, CB_DELETESTRING, index, 0);
		if (node != NULL) {
			data->Remove_Node(node);
		}
		break;
	}

	case WM_SETFOCUS:
	case WM_SETTEXT:
	case WM_GETTEXT:
	case WM_GETTEXTLENGTH:
	case CB_LIMITTEXT:
	case 0x4B2:
	case 0x4B3:
	case 0x4B4:
	case 0x4B5: {
		if ((GetWindowLong(window, GWL_STYLE) & 3) == 3) {
			break;
		}
		HWND child = GetWindow(window, GW_CHILD);
		while (child) {
			char class_name[16];
			GetClassName(child, class_name, sizeof(class_name));
			if (stricmp(class_name, "listbox") == 0) {
				if (message == WM_SETFOCUS) {
					SetFocus(child);
					result = 0;
				} else if (message == CB_LIMITTEXT) {
					result = SendMessage(child, EM_LIMITTEXT, wparam, lparam);
				} else {
					result = SendMessage(child, message, wparam, lparam);
				}
			}
			child = GetWindow(child, GW_HWNDNEXT);
		}
		pass_on = false;
		break;
	}

	case 0x4BD:
		wide = 0;

	case CB_GETLBTEXTLEN:
	case 0x4C3:
	case 0x4D0: {
		if (lparam != 0 && message != CB_GETLBTEXTLEN && message != 0x4D0) {
			if (wide) {
				*(wchar_t *)lparam = 0;
			} else {
				*(char *)lparam = 0;
			}
		}
		pass_on = false;
		result = wparam;
		if (result != CB_ERR) {
			result = CallWindowProc(proc, window, CB_GETITEMDATA, wparam, 0);
			if (result != CB_ERR && result != 0) {
				OwnerDraw::WinNode *item = (OwnerDraw::WinNode *)result;
				if (message == 0x4D0) {
					result = item->data;
				} else {
					result = wcslen(item->text);
					if (message == 0x4C3 || message == 0x4BD) {
						if (wide) {
							wcscpy((wchar_t *)lparam, item->text);
						} else {
							OwnerDraw::WideToCharString((char *)lparam, item->text, -1);
						}
					}
				}
			}
		}
		if (result < 0) {
			result = 0;
		}
		break;
	}

	case WM_ERASEBKGND:
		result = 0;
		pass_on = false;
		break;

	case OD_SETCOLOR:
		if ((unsigned int)wparam <= 50) {
			data->ComboBox.itemColors[wparam] = lparam;
		}
		break;

	case 0x4DD:
		data->UseItemColorOverrides = (lparam == 1);
		break;

	case 0x4F1:
		data->disabled = (lparam == 1);
		break;

	case 0x4DE:
		data->ComboBoxMaxVisibleDropItems = lparam;
		break;

	case 0x4E8: {
		HWND dropdown_window = data->ComboBox.dropdown;
		LRESULT drop_result = CB_ERR;
		if (dropdown_window != NULL) {
			RECT combo_window;
			RECT drop_window;
			GetWindowRect(window, &combo_window);
			GetWindowRect(dropdown_window, &drop_window);
			lparam = MAKELPARAM(combo_window.left - drop_window.left + lparam, HIWORD(lparam) - drop_window.top + combo_window.top);
			drop_result = SendMessage(dropdown_window, 0x4E8, 0, lparam);
		}
		return(drop_result);
	}

	case OD_SUBCLASSED:
		if (!data->itemHeightSet || SendMessage(window, CB_GETITEMHEIGHT, 0, 0) != data->Font->field_1C + 6) {
			SendMessage(window, CB_SETITEMHEIGHT, (WPARAM)-1, data->Font->field_1C + 2);
			SendMessage(window, CB_SETITEMHEIGHT, 0, data->Font->field_1C + 6);
			data->itemHeightSet = 1;
		}
		data->ComboBox.reservedF8[0] = -1;
		memset(data->ComboBox.itemColors, 0xFF, sizeof(data->ComboBox.itemColors));
		pass_on = false;
		break;

	case WM_PAINT: {
		BitFont *font = data->Font;
		LRESULT dropped = SendMessage(window, CB_GETDROPPEDSTATE, 0, 0);
		RECT wrect;
		GetWindowRect(window, &wrect);
		RECT dropped_rect;
		SendMessage(window, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)&dropped_rect);

		dropped_rect.top += (display_rect.bottom - display_rect.top + 1);
		dropped_rect.bottom = dropped_rect.bottom + display_rect.top - display_rect.bottom - 1;

		Rect rect(display_rect.left, display_rect.top, display_rect.right - display_rect.left, 24);

		int width = display_rect.right - display_rect.left;
		int height = display_rect.bottom - display_rect.top;
		Rect dst_rect(0, 0, width, height);

		HWND parent = GetParent(window);
		OwnerDraw::WinData *parent_data = NULL;
		if (parent) {
			OwnerDraw::Dialogs.getPointer(parent, &parent_data);
		}

		RECT parent_rect;
		Get_Display_Rect(parent, &parent_rect);

		Rect src_rect = dst_rect;
		if (parent_data != NULL && parent_data->cachedSurface != NULL) {
			src_rect.X = display_rect.left + src_rect.X - parent_rect.left;
			src_rect.Y += display_rect.top - parent_rect.top;
		}

		if (data->cachedSurface == NULL) {
			Surface *surface = new BSurface(width, height, 2);
			data->cachedSurface = surface;
			OwnerDraw::CachedSurfaceCount++;

			if (parent_data != NULL && parent_data->cachedSurface != NULL) {
				surface->Blit_From(dst_rect, *parent_data->cachedSurface, src_rect, false, true);
			}

			int total = width * height;
			unsigned short *surfptr = (unsigned short *)surface->Lock();
			if (total > 0) {
				for (int i = 0; i < total; ++i) {
					surfptr[i] = OD_Blend_Color(surfptr[i], 0, data->alpha);
				}
			}
			if (surfptr != NULL) {
				surface->Unlock();
			}
		}

		OwnerDraw::CopyDimmedBackground(rect, window, data->alpha);
		ODFillRectTrans(&rect, AlternateSurface, 0, data->alpha);

		LONG style = GetWindowLong(window, GWL_STYLE);
		int color;
		if (data->disabled) {
			color = ODColorToHiColor(OwnerDraw::AltBorderColor);
			if (style & WS_DISABLED) {
				color = ODColorToHiColor(OwnerDraw::AltDisabledBorderColor);
			}
		} else {
			color = ODColorToHiColor(OwnerDraw::DefaultBorderColor);
			if (style & WS_DISABLED) {
				color = ODColorToHiColor(OwnerDraw::DisabledBorderColor);
			}
		}
		OwnerDraw::DrawBeveledBorder(*AlternateSurface, rect, 2, color);

		Rect text_area = rect;
		text_area.Width = rect.Width - 20;

		Rect arrow_rect = rect;
		arrow_rect.X = display_rect.right - 19;
		arrow_rect.Y += 1;
		ODDrawArrowBitmap(AlternateSurface, arrow_rect, dropped, dropped, data->disabled == 1);

		if (style & WS_DISABLED) {
			ODFillRectTrans(&rect, AlternateSurface, 0, data->alpha);
		}

		if ((style & 3) == 3) {
			int sel = CallWindowProc(proc, window, CB_GETCURSEL, 0, 0);
			HWND owner = window;
			WNDPROC item_proc = Find_Original_Proc(owner);
			_buffer[0] = 0;
			int item = CallWindowProc(item_proc, owner, 0x199, sel, 0);
			if (item == -1 || item == 0) {
				item = CallWindowProc(item_proc, owner, CB_GETITEMDATA, sel, 0);
			}
			if (item != -1 && item != 0) {
				Copy_Wide_Text(_buffer, ((OwnerDraw::WinNode *)item)->text, 256);
			}

			int text_color = OwnerDraw::PrimaryTextColor;
			if (data->disabled) {
				if (style & WS_DISABLED) {
					text_color = OwnerDraw::AltDisabledTextColor;
				} else {
					text_color = OwnerDraw::AltComboTextColor;
				}
			} else if (style & WS_DISABLED) {
				text_color = OwnerDraw::DisabledTextColor;
			}

			if (data->UseItemColorOverrides && sel >= 0 && sel < 50 && data->ComboBox.itemColors[sel] >= 0) {
				Rect fill = text_area;
				FUN_0072A9E0(&fill, 2);
				text_color = data->ComboBox.itemColors[sel];
				AlternateSurface->Fill_Rect(fill, ODColorToHiColor(text_color));
			}

			int max_width = client_rect.right - 20;
			int len = wcslen(_buffer);
			int text_width = font->FUN_00433ED0(_buffer, 0);
			while (len != 0 && text_width > max_width) {
				_buffer[--len] = 0;
				wcscat(_buffer, L"...");
				text_width = font->FUN_00433ED0(_buffer, 0);
			}

			RECT text_rect = display_rect;
			text_rect.left += 2;
			OwnerDraw::DrawWideText(AlternateSurface, _buffer, (LTRBStruct *)&text_rect, font, text_color, 4, 0xc, 0, 0, 0);
			ValidateRect(window, NULL);
			result = 0;
			pass_on = false;
			break;
		}

		ValidateRect(window, NULL);
		pass_on = false;
		break;
	}
	}

	if (pass_on) {
		WNDPROC original = Find_Original_Proc(window);
		result = CallWindowProc(original, window, message, wparam, lparam);
	}
	return(result);
}
