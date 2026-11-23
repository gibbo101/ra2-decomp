// best 48.4% (7216/7265), normalised 78.1%, on 23 Nov src (after 0x00612B70 landed); no overlay; tools/inlinebudget.py reports no differing inlining decisions (80 calls each side).
// Needs in src/ownrdraw.cpp: delete the file-level `static ArrayList<HWND> ActiveWindows;` and `static Dictionary<OwnerDraw::CtrlMsgData, bool> ProcessedMessages(Hash_CtrlMsg);` with their SYNTHETIC markers (0x00612910 _$E121, 0x00612930 _$E126): nothing else uses them, and the exe builds both objects inside this function (operator new at 0x00610E33, _atexit at 0x00610E59 and 0x00610EE5), so those two addresses are the atexit stubs of the function-local statics below and take new markers once this lands. Insert before 0x00612A60. With it TrackBarHandler 0x0061D950 drops to 99.2% (declaration weight) and every other ownrdraw function stays MATCH.
// Still open (unchanged from the 20 Nov bank): register allocation and frame layout; see the 2026-11-20 attempts row.
namespace OwnerDraw {
extern int CachedSurfaceCount;
extern HWND ComboDropActiveDropHwnd;
extern HWND ComboDropActiveParentHwnd;
int __fastcall ServiceIMEMessage(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
int GetIMEResult(void);
void CancelIMEComposition(void);
LRESULT CALLBACK ComboDropWindowHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}
bool __fastcall Is_Web_Browser_Visible(void);

static inline char Remove_Entry(Dictionary<OwnerDraw::CtrlMsgData, bool> & dict, OwnerDraw::CtrlMsgData & key)
{
	bool value;
	return(dict.remove(key, value));
}

void __fastcall FUN_0072AA10(Rect * rect, int dx, int dy);

// FUNCTION: GAMEMD 0x00610CA0
LRESULT CALLBACK OwnerDraw::DefaultHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	static POINT min_update_rect = {0xFFFFFF, 0xFFFFFF};
	static POINT max_update_rect;
	static int num_rect_updates;
	static bool was_in_focus = true;
	static char in_programmatic_reorder;

	if (message == WM_SETCURSOR) {
		return(1);
	}

	if (OwnerDraw::ServiceIMEMessage(window, message, wparam, lparam)) {
		return(OwnerDraw::GetIMEResult());
	}

	WNDPROC specific_proc = NULL;
	OwnerDraw::SubclassProcs.getValue(window, specific_proc);

	WNDPROC original_proc = NULL;
	OwnerDraw::DialogProcs.getValue(window, original_proc);

	if (message == WM_SYSKEYUP && wparam == VK_TAB) {
		SendMessage(MainWindow, message, wparam, lparam);
	}

	LRESULT result = 0;
	BOOL show_tooltip = FALSE;
	BOOL call_proc = TRUE;
	int anim_state = 0;

	static Dictionary<CtrlMsgData, bool> ctrlmessages(Hash_CtrlMsg);

	CtrlMsgData key;
	key.window = window;
	key.message = message;

	RECT display_rect;
	Get_Display_Rect(window, &display_rect);
	RECT window_rect;
	GetWindowRect(window, &window_rect);
	Point2D offset(window_rect.left - display_rect.left, window_rect.top - display_rect.top);

	bool mouse_over = false;

	static ArrayList<HWND> hwndarray;


	if (hwndarray.length() != 0) {
		HWND topwindow = NULL;
		hwndarray.getTail(topwindow);

		BOOL allow = FALSE;
		BOOL forward = FALSE;
		if (GetParent(window) == NULL) {
			allow = TRUE;
		}
		if (GetWindowLong(window, GWL_ID) <= 0) {
			allow = TRUE;
		}

		HWND parent = window;
		while (parent != NULL) {
			if (parent == topwindow) {
				allow = TRUE;
				break;
			}
			parent = GetParent(parent);
		}

		if (message < WM_MOUSEMOVE || message > WM_MBUTTONDBLCLK) {
			forward = TRUE;
		}
		if (message >= WM_NCMOUSEMOVE && message <= WM_NCMBUTTONDBLCLK) {
			forward = FALSE;
		}
		if (message >= WM_KEYFIRST && message <= WM_KEYLAST) {
			forward = FALSE;
		}
		if (message == WM_SYSKEYUP || message == WM_SYSKEYDOWN || message == WM_SYSCOMMAND || message == WM_SYSCHAR) {
			forward = TRUE;
		}
		if (message == OD_GETTIPTEXT || message == WM_TIMER || message == OD_GETCELLTIP) {
			forward = FALSE;
		}
		if (!allow && !forward) {
			return(0);
		}
	}

	if (ctrlmessages.contains(key)) {
		if (message != WM_COMMAND && message != WM_SYSKEYDOWN && message != WM_SYSKEYUP
			&& message != WM_SYSCOMMAND && message != WM_SYSCHAR) {
			return(0);
		}
	}

	Remove_Entry(ctrlmessages, key);
	bool processing = true;
	ctrlmessages.add(key, processing);

	RECT client_rect;
	GetClientRect(window, &client_rect);
	RECT disp_rect;
	Get_Display_Rect(window, &disp_rect);

	bool in_focus = GameInFocus;

	bool is_paint = false;
	if (message == WM_PAINT) {
		is_paint = true;
		if (!in_focus) {
			ValidateRect(window, NULL);
			Remove_Entry(ctrlmessages, key);
			return(0);
		}
	}

	if (in_focus == true && !was_in_focus) {
		InvalidateRect(MainWindow, NULL, FALSE);
		in_focus = GameInFocus;
	}
	was_in_focus = in_focus;

	RECT update_rect;
	if (is_paint) {
		++num_rect_updates;
		GetUpdateRect(window, &update_rect, FALSE);
		update_rect.left += disp_rect.left;
		update_rect.right += disp_rect.left;
		update_rect.top += disp_rect.top;
		update_rect.bottom += disp_rect.top;
	}

	WinData *data = Find_Data(window);

	if (message == OD_HASATTACHED) {
		if (data != NULL && data->attachedWindow != NULL) {
			result = 1;
			goto cleanup;
		} else {
			result = 0;
			goto cleanup;
		}
	} else if (message == OD_SAVEDC) {
		if (data != NULL) {
			HDC hdc = (HDC)lparam;
			HGDIOBJ old = SelectObject(hdc, GetStockObject(SYSTEM_FONT));
			data->font = (HFONT)old;
			SelectObject(hdc, old);
			data->bkMode = GetBkMode(hdc);
			data->bkColor = GetBkColor(hdc);
			data->textColor = GetTextColor(hdc);
			result = 1;
			goto cleanup;
		}
	} else if (message == OD_RESTOREDC) {
		if (data != NULL) {
			HDC hdc = (HDC)lparam;
			SelectObject(hdc, data->font);
			SetBkMode(hdc, data->bkMode);
			SetBkColor(hdc, data->bkColor);
			SetTextColor(hdc, data->textColor);
			result = 1;
			goto cleanup;
		}
	} else if (message == OD_DISABLEPAINT) {
		if (data != NULL) {
			HWND linked = (HWND)data->attachedWindow;
			result = data->paintDisabled;
			data->paintDisabled = lparam;
			if (linked != NULL) {
				WinData *linkeddata = NULL;
				OwnerDraw::Dialogs.getPointer((HWND &)data->attachedWindow, &linkeddata);
				if (linkeddata != NULL) {
					linkeddata->paintDisabled = lparam;
				}
			}
			goto call_custom_proc;
		}
	} else if (message == OD_SETTOP) {
		HWND found = NULL;
		hwndarray.getTail(found);
		result = (LRESULT)found;

		HWND target = window;
		if (wparam) {
			target = (HWND)wparam;
		}

		int scan = 0;
		for (int index = 0; index < hwndarray.length(); index++) {
			hwndarray.get(found, scan);
			if (found == target) {
				hwndarray.remove(scan);
			} else {
				scan++;
			}
		}

		if (lparam) {
			hwndarray.addTail(target);
			in_programmatic_reorder = 1;
			SetWindowPos(target, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			in_programmatic_reorder = 0;
		}
		goto cleanup;
	}

	if (hwndarray.length() != 0) {
		if (message == WM_WINDOWPOSCHANGING) {
			if (in_programmatic_reorder != 1) {
				HWND found = NULL;
				for (int index = 0; index < hwndarray.length(); index++) {
					hwndarray.get(found, index);
					if (found == window) {
						WINDOWPOS *wp = (WINDOWPOS *)lparam;
						if (index == hwndarray.length() - 1 && wp->hwndInsertAfter != NULL && (wp->flags & SWP_NOZORDER) == 0) {
							wp->hwndInsertAfter = NULL;
							InvalidateRect(window, NULL, FALSE);
							result = 0;
						} else {
							wp->flags |= SWP_NOOWNERZORDER | SWP_NOZORDER;
							InvalidateRect(window, NULL, FALSE);
							result = 0;
						}
						goto cleanup;
					}
				}
			}
			goto call_custom_proc;
		}

		if (message == WM_DESTROY) {
			HWND found = NULL;
			int scan = 0;
			for (int index = 0; index < hwndarray.length(); index++) {
				hwndarray.get(found, scan);
				if (found == window) {
					hwndarray.remove(scan);
				} else {
					scan++;
				}
			}
		}
	}

	if (window == ODTooltip.window
		&& (message == WM_NCDESTROY || message == WM_SHOWWINDOW || message == WM_KILLFOCUS)) {
		OwnerDraw::End_Tooltip();
	}

	if (message == WM_ERASEBKGND) {
		result = 1;
		goto cleanup;
	} else if (message == WM_SETFOCUS) {
		if (specific_proc == OwnerDraw::OwnerDrawButtonHandler || specific_proc == OwnerDraw::ListBoxHandler) {
			SetFocus((HWND)wparam);
		}
		if (data == NULL) {
			goto messages_done;
		}
		if (!data->hasFocus) {
			OwnerDraw::CancelIMEComposition();
			data->hasFocus = TRUE;
		}
	} else if (message == WM_KILLFOCUS) {
		data->hasFocus = FALSE;
	} else if (message == WM_SHOWWINDOW) {
		if (wparam == 0) {
			if (data == NULL) {
				goto messages_done;
			}
			data->animState = 0;
		}
	}

	if (data != NULL) {
		if (message == OD_SETIMAGE) {
			result = (LRESULT)data->image;
			data->image = (Surface *)lparam;
			goto cleanup;
		} else if (message == OD_SETALTIMAGE) {
			result = (LRESULT)data->altImage;
			data->altImage = (Surface *)lparam;
			goto cleanup;
		} else if (message == OD_TOOLTIPS) {
			result = data->toolTipsEnabled;
			data->toolTipsEnabled = lparam;
			goto cleanup;
		}

		if (data->toolTipsEnabled) {
			if (message == WM_TIMER || (message >= WM_MOUSEMOVE && message <= WM_MOUSELAST)) {
				Rect corner;
				GetWindowRect(window, (LPRECT)&corner);
				if (WindowFromPoint(*(POINT *)&corner) == window) {
					mouse_over = true;
				}
			}
		}
	}

	if (message == WM_SETTEXT) {
		call_proc = FALSE;
	} else if (message == WM_GETTEXT) {
		call_proc = FALSE;
		result = 0;
	} else if (message == LB_GETTEXT) {
		call_proc = FALSE;
		result = 0;
	} else if (message == CB_ADDSTRING || message == CB_FINDSTRING || message == CB_FINDSTRINGEXACT
		|| message == CB_SELECTSTRING || message == CB_INSERTSTRING || message == CB_GETLBTEXT
		|| message == LB_ADDSTRING || message == LB_FINDSTRING || message == LB_FINDSTRINGEXACT
		|| message == LB_SELECTSTRING || message == LB_INSERTSTRING) {
		call_proc = FALSE;
		result = -1;
	}

	if (data != NULL && message == 0x4ce) {
		result = (data->hasText == 0);
		call_proc = FALSE;
	} else if (data != NULL && message == 0x4b5) {
		char *buffer = (char *)lparam;
		if (buffer != NULL) {
			*buffer = '\0';
			if (data->text != NULL) {
				OwnerDraw::WideToCharString(buffer, data->text, wparam);
				if (wparam > 0) {
					buffer[wparam - 1] = '\0';
				}
			}
		}
	} else if (data != NULL && message == 0x4b3) {
		wchar_t *buffer = (wchar_t *)lparam;
		if (buffer != NULL) {
			*buffer = L'\0';
			if (data->text != NULL) {
				int size = wparam;
				wcsncpy(buffer, data->text, size);
				if (size > 0) {
					buffer[size - 1] = L'\0';
				}
			}
		}
	} else if (data != NULL && message == 0x4eb) {
		data->rejectChars = (wchar_t *)lparam;
	} else if (data != NULL && message == 0x4b4) {
		data->Set_Text((char const *)lparam);
		data->hasText = TRUE;
	} else if (data != NULL && message == 0x4d1) {
		result = data->asciiOnly;
		data->asciiOnly = wparam;
		call_proc = FALSE;
	} else if (data != NULL && message == 0x4b2) {
		wchar_t const *old_text = data->text;
		wchar_t const *new_text = (wchar_t const *)lparam;
		bool either = (old_text != NULL || new_text != NULL);
		bool added = (old_text == NULL && new_text != NULL);
		bool removed = (old_text != NULL && new_text == NULL);
		bool changed = added || removed || (either && wcscmp(old_text, new_text) != 0);
		data->Set_Text(new_text);
		data->hasText = FALSE;
		if (changed && data->StaticDrawMode == 1 && data->StaticAnimationRunning) {
			KillTimer(window, 0);
			data->StaticAnimationRunning = false;
			SendMessage(window, 0x4ee, 0, 0);
		}
	} else if (message == WM_MOUSEMOVE) {
		HWND parent = GetParent(window);
		HWND tip = GetDlgItem(parent, 0x695);
		if (tip != NULL) {
			WideWstring text;
			text = FUN_00603FC0(parent, window, LOWORD(lparam), HIWORD(lparam));
			if (wcscmp(text.get(), DAT_00887734) == 0) {
				text = FUN_00603F60(parent, window);
				if (wcscmp(text.get(), DAT_00887734) == 0) {
					text = FUN_00604060(parent, window);
				}
			}
			SendMessage(tip, 0x4b2, 0, (LPARAM)text.text());
		}
	}

messages_done:
	if (is_paint && data != NULL) {
		HWND owner;
		WinData *ownerdata;
		int state;
		RECT intersect;
		Rect sibling_rect;

		if (data->paintDisabled) {
			ValidateRect(window, NULL);
			result = CallWindowProc(specific_proc, window, OD_REFRESHNOPAINT, wparam, lparam);
		} else {

			{
				RECT tooltip_rect;
				tooltip_rect.left = ODTooltip.bounds.X;
				tooltip_rect.right = ODTooltip.bounds.Width + ODTooltip.bounds.X + 1;
				tooltip_rect.top = ODTooltip.bounds.Y;
				tooltip_rect.bottom = ODTooltip.bounds.Height + ODTooltip.bounds.Y + 1;
				if (num_rect_updates == 1) {
					if (IntersectRect(&intersect, &disp_rect, &tooltip_rect)) {
						if (ODTooltip.isActive && ODTooltip.isHidden != 1 && ODTooltip.background != NULL) {
							Rect drect = ODTooltip.bounds;
							Rect srect(0, 0, ODTooltip.bounds.Width, ODTooltip.bounds.Height);
							VisibleSurface->Blit_From(drect, *ODTooltip.background, srect, false, true);
							ODTooltip.isHidden = true;
							show_tooltip = true;
						}
					}
				}
			}

			if (min_update_rect.x >= disp_rect.left) {
				min_update_rect.x = disp_rect.left;
			}
			if (min_update_rect.y >= disp_rect.top) {
				min_update_rect.y = disp_rect.top;
			}
			if (max_update_rect.x <= disp_rect.right) {
				max_update_rect.x = disp_rect.right;
			}
			if (max_update_rect.y <= disp_rect.bottom) {
				max_update_rect.y = disp_rect.bottom;
			}

			owner = window;
			while (owner != NULL) {
				if (GetWindowLong(owner, DWL_DLGPROC) != 0) {
					break;
				}
				owner = GetParent(owner);
			}

			ownerdata = NULL;
			if (owner != NULL) {
				OwnerDraw::Dialogs.getPointer(owner, &ownerdata);
			}

			if (owner != window) {
				if (ownerdata == NULL) {
					state = anim_state;
					goto skip_paint;
				}
				state = ownerdata->animState;
			} else {
				if (ownerdata->animState < 1) {
					state = 1;
					anim_state = 1;
					goto call_proc_paint;
				}
				state = ownerdata->animState;
			}

			anim_state = state;
			if (state < 1) {
			skip_paint:
				ValidateRect(window, NULL);
				result = 1;
			} else {

			call_proc_paint:
				result = CallWindowProc(specific_proc, window, message, wparam, lparam);
				if (ownerdata != NULL) {
					ownerdata->animState = state;
				}

				{
					ArrayList<HWND> children;
					EnumChildWindows(window, (WNDENUMPROC)ODAddWindowToList, (LPARAM)&children);

					HWND combo_owner = NULL;
					HWND child = NULL;
					for (int index = 0; index < children.length(); index++) {
						children.get(child, index);

						WNDPROC childproc = NULL;
						OwnerDraw::DialogProcs.getValue(child, childproc);

						if (childproc != (WNDPROC)OwnerDraw::ComboDropWindowHandler) {
							InvalidateRect(child, NULL, FALSE);
							UpdateWindow(child);
						} else {
							combo_owner = child;
						}
					}

					if (combo_owner != NULL) {
						InvalidateRect(combo_owner, NULL, FALSE);
						UpdateWindow(combo_owner);
					} else if (OwnerDraw::ComboDropActiveDropHwnd != NULL) {
						if (OwnerDraw::ComboDropActiveParentHwnd == owner) {
							Rect drop_rect;
							Get_Display_Rect(OwnerDraw::ComboDropActiveDropHwnd, (LPRECT)&drop_rect);
							RECT intersect;
							if (IntersectRect(&intersect, &disp_rect, (const RECT *)&drop_rect)) {
								InvalidateRect(OwnerDraw::ComboDropActiveDropHwnd, NULL, FALSE);
								UpdateWindow(OwnerDraw::ComboDropActiveDropHwnd);
							}
						}
					}
				}

				state = anim_state;
			}

		finalize_paint:
			if (state > 0) {
				HWND sibling = owner;
				while (window == owner || num_rect_updates == 1) {
					if (sibling == NULL) {
						break;
					}
					sibling = GetWindow(sibling, GW_HWNDPREV);
					if (sibling == NULL) {
						break;
					}
					if (GetWindowLong(sibling, DWL_DLGPROC)) {
						Get_Display_Rect(sibling, (LPRECT)&sibling_rect);
						if (IntersectRect(&intersect, &disp_rect, (const RECT *)&sibling_rect)) {
							InvalidateRect(sibling, NULL, FALSE);
							UpdateWindow(sibling);
							break;
						}
					}
				}
			}
		}
	} else {
		if (call_proc) {
		call_custom_proc:
			if (specific_proc != NULL) {
				result = CallWindowProc(specific_proc, window, message, wparam, lparam);
			}
		}
	}

	if (message == WM_NCDESTROY) {
		HWND destroyed = window;
		WinData * destroydata;
		if (OwnerDraw::Dialogs.getPointer(destroyed, &destroydata)) {
			if (destroydata != NULL && destroydata->cachedSurface != NULL) {
				delete destroydata->cachedSurface;
				destroydata->cachedSurface = NULL;
				OwnerDraw::CachedSurfaceCount--;
			}
		}
		ODRemoveFromDict(window, 0);
	}

cleanup:
	Remove_Entry(ctrlmessages, key);

	if (data != NULL && is_paint) {
		if (GetWindowLong(window, DWL_DLGPROC)) {
			if (num_rect_updates > 1) {
				data->animState = 2;
			}
		}

		if (--num_rect_updates == 0) {
			if (!data->paintDisabled && anim_state >= 1 && !Is_Web_Browser_Visible()) {

				Rect rect2;
				Rect screen_rect;
				rect2.X = min_update_rect.x;
				rect2.Y = min_update_rect.y;
				rect2.Width = max_update_rect.x - min_update_rect.x;
				rect2.Height = max_update_rect.y - min_update_rect.y;
				screen_rect.X = min_update_rect.x;
				screen_rect.Y = min_update_rect.y;
				screen_rect.Width = max_update_rect.x - min_update_rect.x;
				screen_rect.Height = max_update_rect.y - min_update_rect.y;

				if (data->animState == 1) {
					data->animState = 2;
					if (GetWindowLong(window, DWL_DLGPROC) && RunOpenAnimationIfNeeded(window)) {
						data->animState = 3;
					}

					Rect whole_rect(min_update_rect.x, min_update_rect.y, max_update_rect.x - min_update_rect.x, max_update_rect.y - min_update_rect.y);
					screen_rect = whole_rect;
					screen_rect += offset;
					VisibleSurface->Lock();
					AlternateSurface->Lock();
					VisibleSurface->Blit_From(screen_rect, *AlternateSurface, whole_rect, false, true);
					AlternateSurface->Unlock();
					VisibleSurface->Unlock();

					ArrayList<HWND> children;
					EnumChildWindows(window, (WNDENUMPROC)ODAddWindowToList, (LPARAM)&children);
					HWND child = NULL;
					for (int index = 0; index < children.length(); index++) {
						children.get(child, index);
						SendMessage(child, OD_ACTIVATE, 0, 0);
					}

				} else {
					screen_rect = rect2;
					screen_rect += offset;

					char class_name[128];
					GetClassName(window, class_name, sizeof(class_name));
					if (strcmp(class_name, "ComboBox") == 0) {
						FUN_0072AA10(&rect2, -1, -1);
						FUN_0072AA10(&screen_rect, -1, -1);
					}
					VisibleSurface->Lock();
					AlternateSurface->Lock();
					VisibleSurface->Blit_From(screen_rect, *AlternateSurface, rect2, false, true);
					AlternateSurface->Unlock();
					VisibleSurface->Unlock();
				}
			}

			min_update_rect.x = 0xFFFFFF;
			min_update_rect.y = 0xFFFFFF;
			max_update_rect.x = 0;
			max_update_rect.y = 0;
		}
	}

	if (show_tooltip) {
		OwnerDraw::DrawTooltip(true);
	}

	if (message == WM_INITDIALOG) {
		result = 0;
	}
	return(result);
}

// SYNTHETIC: GAMEMD 0x00612910 SYMBOL
// _$E122

// SYNTHETIC: GAMEMD 0x00612930 SYMBOL
// _$E121

