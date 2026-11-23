// 0x0079E520 buddy dialog proc 48.0% (3283/2939 bytes) on main 08cfecb, no overlay; splice before 0x0079F0A0. inlinebudget: 25 differing decisions in two clusters, both 'ours inlines more': case 0x497's FUN_0079D4E0 (ours inlines Dictionary<Wstring,WolEntryState>::clear and calls FUN_0079D2F0; slack 8..16 units) and case 0x704's FUN_0079CE70 (ours inlines FUN_0077F320 and the addTail/C3A0 internals). Each flips with 1 more inline site after it. Diagnostic probes (not source): one site after case 0x497's SetTimer fixes the whole D4E0 cluster but leaves timer-0's and 0x704's CE70 copies inlining get/remove/add; one site at the end of case 0x704 fixes D4E0 but starves timer-0's FUN_0079C3A0. Moving case 0x704 first changes layout (same dispatch group), so it is not free.
extern int DAT_0084A1CC;
extern int DAT_0084A1D0;

// FUNCTION: GAMEMD 0x0079E520
BOOL CALLBACK FUN_0079E520(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_DESTROY:
			KillTimer(window, 0);
			KillTimer(window, 0x704);
			FUN_005EE1B0();
			return FALSE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case 0x497:
			{
				FUN_005EE350(window);
				HWND item = GetDlgItem(window, 0x689);
				SendMessage(item, 0x4A6, DAT_00B765F0, DAT_0084A1C4);
				SendMessage(item, 0x4A6, 0, DAT_0084A1C8);
				SendMessage(item, 0x4A6, 0, DAT_0084A1CC);
				SendMessage(item, 0x4A6, 0, DAT_0084A1D0);
				SendMessage(item, 0x4A6, DAT_00B73E3C, DAT_0084A1D4);
				SendMessage(item, 0x4A6, 0, DAT_0084A1D8);
				item = GetDlgItem(window, 0x704);
				SendMessage(item, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("wouref.pcx", NULL));
				SendMessage(item, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("wodref.pcx", NULL));
				FUN_0079D4E0();
				SetTimer(window, 0, 1000, NULL);
			}
			return FALSE;

		case WM_TIMER:
			switch (wparam) {
				case 0:
					if ((unsigned int)(time(NULL) - DAT_00B778C8) <= 30) {
						break;
					}
					FUN_0079CE70();
					return FALSE;

				case 0x704:
					KillTimer(window, wparam);
					EnableWindow(GetDlgItem(window, wparam), TRUE);
					return FALSE;
			}
			break;

		case WM_COMMAND:
			if (control->F2()) {
				break;
			}
			switch (LOWORD(wparam)) {
				case 0x5C2:
					FUN_0079BE70();
					return FALSE;

				case 0x540:
					if (HIWORD(wparam) == EN_CHANGE) {
						Wstring text;
						FUN_00623E40((HWND)lparam, text);
						EnableWindow(GetDlgItem(window, 0x702), text.length() != 0);
						return FALSE;
					}
					break;

				case 0x62B:
					control->F1(window, 0x62B);
					return FALSE;

				case 0x686:
					control->F1(window, 0x686);
					return FALSE;

				case 0x62D:
					control->F1(window, 0x62D);
					return FALSE;

				case 0x689:
					switch (HIWORD(wparam)) {
						case LBN_SELCHANGE:
							{
								HWND list = (HWND)lparam;
								int sel = SendMessage(list, LB_GETCURSEL, 0, 0);
								if (sel == -1) {
									DAT_00B76E24.clear();
								} else {
									FUN_00623F00(list, sel, &DAT_00B76E24);
								}
								EnableWindow(GetDlgItem(window, 0x703), DAT_00B76E24.length() > 0);
								EnableWindow(GetDlgItem(window, 0x62D), FUN_0079BC80());
								bool joinable;
								if (DAT_00B76E24.length() == 0) {
									joinable = false;
								} else {
									WolEntryState state = WOL_ENTRY_STATE_NONE;
									DAT_00B76990.getValue(DAT_00B76E24, state);
									joinable = state < 0 || state > 2;
								}
								EnableWindow(GetDlgItem(window, 0x5C2), joinable);
								FUN_0079C040(window);
							}
							return FALSE;

						case LBN_DBLCLK:
							if (FUN_0079BC80()) {
								SendMessage(GetDlgItem(window, 0x62D), BM_CLICK, 0, 0);
								return FALSE;
							}
							if (DAT_00B76E24.length() == 0) {
								break;
							}
							{
								WolEntryState state = WOL_ENTRY_STATE_NONE;
								DAT_00B76990.getValue(DAT_00B76E24, state);
								if (state >= 0 && state <= 2) {
									break;
								}
							}
							SendMessage(GetDlgItem(window, 0x5C2), BM_CLICK, 0, 0);
							return FALSE;
					}
					break;

				case 0x75F:
				case 0x760:
					{
						int checked = 0;
						HWND item = GetDlgItem(window, 0x760);
						if (item) {
							checked = SendMessage(item, BM_GETCHECK, 0, 0);
						}
						if (LOWORD(wparam) == 0x75F) {
							checked = checked == 0;
						}
						Game::ObserverMode = checked;
						FUN_005EE350(NULL);
					}
					return FALSE;

				case 0x702:
					{
						FUN_007F7624_Class dialog;
						Run_Dialog_Control(0x115, FUN_0079DFE0, (OwnerDraw::DialogControl *)&dialog);
						FUN_0079AF10(window);
					}
					return FALSE;

				case 0x703:
					{
						HWND list = GetDlgItem(window, 0x689);
						int sel = SendMessage(list, LB_GETCURSEL, 0, 0);
						if (sel == -1) {
							break;
						}
						Wstring name;
						FUN_00623F00(list, sel, &name);
						FUN_0079CBA0(&name);
						SendMessage(list, LB_DELETESTRING, sel, 0);
						DAT_00B76E24.clear();
						FUN_0079AF10(window);
					}
					return FALSE;

				case 0x704:
					EnableWindow((HWND)lparam, FALSE);
					FUN_0079CE70();
					SetTimer(window, 0x704, 3000, NULL);
					break;
			}
			break;
	}
	return FALSE;
}
