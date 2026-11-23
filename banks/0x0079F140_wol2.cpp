// 0x0079F140 buddy-list loop: 75.9% (2711/2763 bytes), no overlay, against the n24-wol2 worktree src (SimpleWonlineDialogControl unified; F2/F3 are SimpleDialogControl 0x006241C0/0x006241D0); insert before 0x0079FC10. Differs: FUN_00798340 called inside the inlined FUN_0077F710 (shortfall 2..4 units), second FUN_0079F0A0 result kept as a flag in al in the exe.
BOOL CALLBACK FUN_0079E520(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
bool FUN_00789B60(void);
void Title_Screen_Restore(bool force);
void FUN_0072C060(void);
void FUN_0072C0B0(void);
namespace UI { void __fastcall EndDialog(HWND window); }


// FUNCTION: GAMEMD 0x0079F140
int FUN_0079F140(void)
{
	DWORD index;

	DAT_00B77DC1 = 0;
	g_GameStarted = 0;
	FUN_0052F3F0();
	if (!FUN_0077F520(true)) {
		Session.unknown_10 = 0;
		return 0;
	}
	Leave_Channel(0x40, 1);
	DAT_00B76608.clear();

	while (true) {
		FUN_0072C060();
		SimpleWonlineDialogControl control;
		FUN_00798340();
		HWND dialog = UI::BeginDialog(0x116, FUN_0079E520, (int)&control);
		OwnerDraw::Display_Dialog(dialog);
		while (!control.F2()) {
			while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT && !control.F2()) {
				WOL_Wait_Callback();
			}
			ResetEvent(g_WaitEventHandles[index]);
			if (index == 0) {
				FUN_00792EC0(0xFFFFFFFF);
				UI::EndDialog(dialog);
				Session.unknown_10 = 0;
				ODMessageBox(StringTable::LoadString("TXT_YOURE_DISCON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6C7B), 0, FUN_0077D710, 0);
				FUN_0072C0B0();
				return 0;
			}
			if (index == 5) {
				FUN_00792EC0(0xFFFFFFFF);
				UI::EndDialog(dialog);
				Session.unknown_10 = 0;
				ODMessageBox(StringTable::LoadString("TXT_YOURE_DISCON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6C84), 0, FUN_0077D710, 0);
				FUN_0072C0B0();
				return 0;
			}
			Title_Screen_Restore(false);
		}
		UI::EndDialog(dialog);
		FUN_0072C0B0();

		switch (control.F3()) {
			case 0x62B:
				FUN_00798340();
				if (!FUN_00789B60()) {
					FUN_004068E0("Buddy List Host Create Game failed\n");
				} else {
					g_JoinLobbyNow = 0;
					while (true) {
						index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 20);
						if (index == WAIT_TIMEOUT) {
							WOL_Wait_Callback();
							if (!g_JoinLobbyNow) {
								continue;
							}
						}
						ResetEvent(g_WaitEventHandles[index]);
						if (index == 0) {
							FUN_004068E0("EV_ABORT from Buddy List Host\n");
							g_JoinLobbyNow = 1;
						} else if (index == 6) {
							FUN_004068E0("EV_EXIT from Buddy List Host\n");
							g_JoinLobbyNow = 1;
						} else if (index == 15) {
							FUN_004068E0("Skirmish from Buddy List\n");
							g_GameStartRequested = false;
							g_LastErrorMessage[0] = 0;
							DAT_00B77DC1 = 0;
							return 2;
						} else if (index == 7) {
							FUN_004068E0("Start Game from Buddy List Host\n");
							g_GameStartRequested = false;
							g_pChat->RequestChannelLeave();
							g_LastErrorMessage[0] = 0;
							DAT_00B77DC1 = 1;
							g_JoinLobbyNow = 0;
							return 1;
						}
						if (g_JoinLobbyNow) {
							break;
						}
					}
					FUN_004068E0("Stop Buddy Host\n");
					g_JoinLobbyNow = 0;
				}
				FUN_00792EC0(0xFFFFFFFF);
				FUN_00798340();
				if (g_LastErrorMessage[0]) {
					ODMessageBox(g_LastErrorMessage, 0, FUN_0077D710, 0);
				}
				while (FUN_007757E0(NULL, 0)) {
				}
				DAT_00B77DC1 = 0;
				break;

			case 0x62D: {
				Channel chan;
				memset(&chan, 0, sizeof(chan));
				if (DAT_00B76E24.length() > 0) {
					if (!DAT_00B76608.getValue(DAT_00B76E24, chan)) {
						break;
					}
					WolEntryState state = WOL_ENTRY_STATE_NONE;
					if (!DAT_00B76990.getValue(DAT_00B76E24, state)) {
						break;
					}
					switch (state) {
						case 5: {
							Wstring name((char *)chan.name);
							Channel other;
							bool found = FUN_0079F0A0(&name, &g_GameChannelList, &other);
							if (!found) {
								if (!FUN_0077F710(0, true)) {
									FUN_004068E0("Buddy List Guest Get Channel List failed\n");
									SetEvent(g_WaitEventHandles[5]);
								} else {
									found = FUN_0079F0A0(&name, &g_GameChannelList, &other);
								}
							}
							if (!found) {
								FUN_004068E0("Buddy List Guest Find Channel by name failed\n");
							} else if (!FUN_00789960(&other, Game::ObserverMode != 0)) {
								FUN_004068E0("Buddy List Guest Join_Game failed\n");
							} else {
								g_JoinLobbyNow = 0;
								int timeout = 500;
								while (true) {
									while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 20)) == WAIT_TIMEOUT && !g_JoinLobbyNow) {
										WOL_Wait_Callback();
									}
									ResetEvent(g_WaitEventHandles[index]);
									if (index == 0) {
										FUN_004068E0("EV_ABORT from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									else if (index == 6) {
										FUN_004068E0("EV_EXIT from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									else if (index == 5) {
										FUN_004068E0("EV_ERROR from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									else if (index == 10) {
										FUN_004068E0("EV_CHAN_JOIN_FAIL from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									if (index == 7) {
										FUN_004068E0("EV_STARTGAME from Buddy List Guest\n");
										g_GameStartRequested = false;
										g_pChat->RequestChannelLeave();
										g_LastErrorMessage[0] = 0;
										DAT_00B77DC1 = 1;
										g_JoinLobbyNow = 0;
										return 1;
									}
									if (timeout-- < 0) {
										FUN_004068E0("Timeout from Buddy List Guest\n");
										break;
									}
									if (g_JoinLobbyNow) {
										break;
									}
								}
								g_JoinLobbyNow = 0;
								FUN_004068E0("Exit from BuddyList Guest\n");
							}
							break;
						}

						case 4: {
							Channel other;
							memset(&other, 0, sizeof(other));
							Session.unknown_10 = 4;
							for (int j = 0; j < g_UserChannels.length(); j++) {
								g_UserChannels.get(other, j);
								if (stricmp((char *)other.name, (char *)chan.name) == 0) {
									Set_Lobby_Number(chan);
									DAT_0084A210 = 0;
									return 0;
								}
							}
							return 0;
						}
					}
				}
				break;
			}

			case 0x5C0:
			case 0x686:
				if (Session.unknown_10 == 4) {
					DAT_0084A210 = 0;
				}
				return 0;
		}
	}
}



