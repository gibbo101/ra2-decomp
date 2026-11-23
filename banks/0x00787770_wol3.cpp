// 0x00787770 Join_WOL_Lobby 55.9% (2705/2568 bytes) on main 08cfecb, no overlay; splice after Login_WOL (0x00787010), before #include "_mixfile.h". inlinebudget: 8 differing decisions, all 'ours inlines more': ArrayList<Wstring> copy-ctor clear in Play copies 2/3 (slack 40..48 units, or 6 more inline sites after them), FUN_0079D2F0 under FUN_0079D4E0 (slack 0..8 units, or 2 more sites), plus copy 1's get. Diagnostic only (not source): a WDT predicate inline at the 2nd+3rd test sites fixes D2F0 and copy 1 but leaves copies 2/3 at copy 1's pattern (58.0%); no out-of-line copy of that predicate exists in the exe (18 inline sites scanned). The idx==7 block after the ret does not move with block form or with a label after the return.
extern char g_WDTSoundsInited;
extern char DAT_0084A210;
void Init_WDT_Sounds(void);
void Play_WDT_Sound(ArrayList<Wstring> list);
void FUN_0079D4E0(void);
bool FUN_0052F3F0(void);
void __fastcall Set_Wait_Dialog_Text(wchar_t const * text);
void __fastcall FUN_00792EC0(unsigned int mask);
void FUN_0072CD60(void);
void FUN_0072CDB0(void);
BOOL CALLBACK FUN_0078AC10(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00787770
int __fastcall Join_WOL_Lobby(HWND win, bool flag)
{
	int retval = 0;

	if (DAT_00B77DC1 && Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3 && Session.Winner != -1 && Session.Winner < 8) {
		Wstring name = FUN_007B7F90(Session.Score[Session.Winner].Name);
		if (strnicmp(name.get(), Session.Handle, sizeof(Session.Handle)) == 0) {
			if (Session.House == 0) {
				Play_WDT_Sound(g_WDT_GDIWinEmphasisSounds);
			} else if (Session.House == 1) {
				Play_WDT_Sound(g_WDT_NodWinEmphasisSounds);
			}
		} else if (Session.House == 0) {
			Play_WDT_Sound(g_WDT_GDILoseEmphasisSounds);
		}
	}

	DAT_00B77DC1 = 0;
	g_GameStarted = 0;
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		g_pChat->RequestSetTeam(Session.House);
		if (!g_WDTSoundsInited) {
			Init_WDT_Sounds();
		}
	}
	FUN_0052F3F0();

	if (g_ConnectionLost) {
		int result = Login_WOL();
		if (result != 1) {
			return result;
		}
	}

	FUN_00798340();

	if (win == 0) {
		FUN_0052FEC0(NULL, true);
		FUN_0072CD60();
		win = UI::BeginDialog(0x113, FUN_0078AC10, 0);
		if (g_MessageOfTheDay.length() > 0) {
			char * motd = new char[g_MessageOfTheDay.length() + 1];
			if (motd != NULL) {
				strcpy(motd, g_MessageOfTheDay.get());
				char * token = strtok(motd, "\n");
				while (token != NULL) {
					WideWstring line = FUN_007B7ED0(token);
					PMessagePrintf(DAT_007EEF28, line.get());
					token = strtok(NULL, "\n");
				}
				PMessagePrintf(DAT_007EEF28, L"\n");
				g_MessageOfTheDay.clear();
				delete [] motd;
			}
		}
		Show_Wait_Window(0x10, StringTable::LoadString("GUI:LoginComplete", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2894), 0);
	}

	g_SquadInfoRequests++;
	g_RequestingOwnSquadInfo = 1;
	g_pChat->RequestSquadInfo(0);

	FUN_0079D4E0();

	DAT_0084A1BC = 0x29;
	Set_Wait_Dialog_Text(StringTable::LoadString("TXT_REQ_CHANLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x28A8));
	if (!FUN_0077F520(true)) {
		goto cleanup;
	}
	Set_Wait_Dialog_Text(StringTable::LoadString("TXT_JOINLOB", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x28AF));
	{
		int lobby = Join_Lobby(DAT_0084A210 && !flag);
		DAT_0084A210 = 1;
		if (lobby == -1) {
			goto cleanup;
		}
	}

	DWORD idx;
	while ((idx = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT) {
		WOL_Wait_Callback();
	}
	ResetEvent(g_WaitEventHandles[idx]);
	if (idx == 0 || idx == 5) {
		goto logout;
	}

	g_JoinLobbyNow = 0;
	PostMessageA(win, 0x593, 0, 0);
	FUN_00792EC0(0xFFFFFFFF);
	SetFocus(MainWindow);
	ShowWindow(win, SW_SHOWNORMAL);
	SetFocus(win);
	SetFocus(GetDlgItem(win, 0x540));
	DAT_00B77DBC = 0;
	DAT_00B77DB8 = time(0);

	while (true) {
		while ((idx = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 20)) == WAIT_TIMEOUT) {
			WOL_Wait_Callback();
			if (g_JoinLobbyNow) {
				EnableWindow(GetDlgItem(win, 0x53D), FALSE);
				EnableWindow(GetDlgItem(win, 0x6EE), FALSE);
				EnableWindow(GetDlgItem(win, 0x62D), FALSE);
				EnableWindow(GetDlgItem(win, 0x62B), FALSE);
				EnableWindow(GetDlgItem(win, 0x689), FALSE);
				EnableWindow(GetDlgItem(win, 0x688), FALSE);
				EnableWindow(GetDlgItem(win, 0x7A3), FALSE);
				EnableWindow(GetDlgItem(win, 0x7A6), FALSE);
				EnableWindow(GetDlgItem(win, 0x61F), FALSE);
				EnableWindow(GetDlgItem(win, 0x5C2), FALSE);
				EnableWindow(GetDlgItem(win, 0x686), FALSE);
				if (Join_Lobby(false) == -1) {
					EnableWindow(GetDlgItem(win, 0x686), TRUE);
					SetEvent(g_WaitEventHandles[5]);
				} else {
					PostMessageA(win, 0x593, 0, 0);
				}
				g_JoinLobbyNow = 0;
			}
		}
		ResetEvent(g_WaitEventHandles[idx]);
		if (idx != 0) {
			if (idx != 6) {
				if (idx != 15) {
					if (idx != 5) {
						if (idx != 7) {
							continue;
						}
						retval = 1;
						g_pChat->RequestChannelLeave();
						g_GameStartRequested = 0;
						g_LastErrorMessage[0] = 0;
						goto cleanup;
					}
					goto cleanup;
				}
				retval = 2;
			}
			g_LastErrorMessage[0] = 0;
			goto logout;
		}
		goto cleanup;
	}

logout:
	FUN_007757E0(win, 0);
	WOL_Wait_Callback();
	win = 0;
	if (g_pChat != NULL) {
		g_pChat->RequestChannelLeave();
	}

cleanup:
	FUN_00792EC0(0xFFFFFFFF);
	ODMessageBox(g_LastErrorMessage, 0, FUN_0077D710, 0);
	if (win != 0) {
		FUN_007757E0(win, 0);
	}
	while (FUN_007757E0(NULL, 0)) {
	}
	FUN_0072CDB0();
	if (retval == 1) {
		DAT_00B77DC1 = 1;
	}
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		g_WDTTeams.clear();
	}
	return retval;
}
