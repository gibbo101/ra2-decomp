// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "sounddlg.h"
#include "globals.h"
#include "init.h"
#include "goptions.h"
#include "ownrdraw.h"
#include "theme.h"
#include "winfix.h"
#include <math.h>
#include <wchar.h>

// SYNTHETIC: GAMEMD 0x006B6210 SYMBOL
// _$E2


void __cdecl FUN_004068E0(char const * format, ...);
extern bool DAT_00B0B800;
extern int FUN_00407000(void);

namespace UI {
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	bool Updated(void);
}

// FUNCTION: GAMEMD 0x006B6230
void SoundControlsClass::Dialog(void)
{
	int rc = -1;
	FUN_004068E0("SoundControls: GameSpeed = %d, ScrollRate = %d, Detail = %d\n", Options.GameSpeed, Options.ScrollRate, Options.DetailLevel);
	DAT_00B0B800 = false;

	HWND dialog;
	if (!GameActive) {
		dialog = UI::BeginDialog(0xd6, Sound_Option_Dialog_Func, 0);
	} else {
		dialog = UI::BeginDialog(0xb8, Sound_Option_Dialog_Func, 0);
	}

	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		UI::FocusOnWindow(dialog);
		while (rc == -1) {
			if (UI::Updated() == true) {
				rc = 2;
			}
			if (!GameActive) {
				Title_Screen_Restore(false);
			}
		}
		UI::EndDialog(dialog);
	}

	FUN_004068E0("SoundControls: GameSpeed = %d, ScrollRate = %d, Detail = %d\n", Options.GameSpeed, Options.ScrollRate, Options.DetailLevel);
}

// FUNCTION: GAMEMD 0x006B6300
BOOL CALLBACK SoundControlsClass::Sound_Option_Dialog_Func(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc == 0) {
		switch (message) {
			case 0x497: {
					DAT_00B0B800 = false;
					int enabled = FUN_00407000();

					HWND track = GetDlgItem(window, 0x52F);
					if (track) {
						SendMessage(track, OD_TRACKSILENT, 0, 0);
						Slider_SetRange(track, 0, VOLUME_LEVELS);
						Slider_SetPos(track, (int)(Options.ScoreVolume * (double)VOLUME_LEVELS + 0.5));
						EnableWindow(track, enabled);
					}

					track = GetDlgItem(window, 0x532);
					if (track) {
						SendMessage(track, OD_TRACKSILENT, 0, 0);
						Slider_SetRange(track, 0, VOLUME_LEVELS);
						Slider_SetPos(track, (int)(Options.SoundVolume * (double)VOLUME_LEVELS + 0.5));
						EnableWindow(track, enabled);
					}

					track = GetDlgItem(window, 0x536);
					if (track) {
						SendMessage(track, OD_TRACKSILENT, 0, 0);
						Slider_SetRange(track, 0, VOLUME_LEVELS);
						Slider_SetPos(track, (int)(Options.VoiceVolume * (double)VOLUME_LEVELS + 0.5));
						EnableWindow(track, enabled);
					}

					if (GameActive) {
						HWND button = GetDlgItem(window, 0x533);
						if (button) {
							Button_SetCheck(button, Options.IsScoreShuffle ? BST_CHECKED : BST_UNCHECKED);
							EnableWindow(button, enabled);
						}

						button = GetDlgItem(window, 0x534);
						if (button) {
							Button_SetCheck(button, Options.IsScoreRepeat ? BST_CHECKED : BST_UNCHECKED);
							EnableWindow(button, enabled);
						}

						HWND list = GetDlgItem(window, 0x530);
						if (list) {
							int active_theme = 0;
							int visible_num = 1;

							ListBox_ResetContent(list);

							for (int index = THEME_FIRST; index < Theme.Max_Themes(); index++) {
								if (Theme.Is_Allowed((ThemeType)index)) {
									wchar_t buffer[100];
									int length = Theme.Track_Length((ThemeType)index);
									wchar_t const * fullname = Theme.Full_Name((ThemeType)index);

									swprintf(buffer, L"%02d - %s [%d:%02d]", visible_num, fullname, length / 60, length % 60);
									visible_num++;

									int row = SendMessage(list, 0x4CD, 0, (LPARAM)buffer);
									if (row != LB_ERR) {
										ListBox_SetItemData(list, row, index);
										if (Theme.What_Is_Playing() == index) {
											active_theme = row;
										}
									}
								}
							}

							ListBox_SetCurSel(list, active_theme);
							ListBox_SetTopIndex(list, active_theme);
							EnableWindow(list, enabled);
						}
					}

					DAT_00B0B800 = true;
				}
				break;

			case WM_COMMAND:
				switch (LOWORD(wparam)) {
					case 0x533:
						Options.Set_Shuffle(Button_GetCheck((HWND)lparam) == BST_CHECKED);
						if (Button_GetCheck((HWND)lparam) == BST_CHECKED) {
							SendDlgItemMessage(window, 0x534, BM_SETCHECK, BST_UNCHECKED, 0);
							Options.Set_Repeat(false);
						}
						break;

					case 0x534:
						Options.Set_Repeat(Button_GetCheck((HWND)lparam) == BST_CHECKED);
						if (Button_GetCheck((HWND)lparam) == BST_CHECKED) {
							SendDlgItemMessage(window, 0x533, BM_SETCHECK, BST_UNCHECKED, 0);
							Options.Set_Shuffle(false);
						}
						break;

					case 0x535:
						if (HIWORD(wparam) == 0) {
							Theme.Stop(true);
							Theme.Queue_Song(THEME_QUIET);
							Options.InGameMusic = false;
						}
						break;

					case 0x686:
						if (HIWORD(wparam) == 0) {
							HWND button = GetDlgItem(window, 0x52F);
							if (button) {
								Options.Set_Score_Volume(Slider_GetPos(button) * 0.1, false);
								if (Options.ScoreVolume == 0.0f) {
									Theme.Queue_Song(Theme.What_Is_Playing());
									Theme.Stop();
								}
							}
							button = GetDlgItem(window, 0x532);
							if (button) {
								Options.Set_Sound_Volume(Slider_GetPos(button) * 0.1, false);
							}
							button = GetDlgItem(window, 0x536);
							if (button) {
								Options.Set_Voice_Volume(Slider_GetPos(button) * 0.1, false);
							}
							int * res = (int *)GetWindowLong(window, DWL_USER);
							*res = IDOK;
						}
						break;

					case 0x531:
						if (HIWORD(wparam) == 0) {
							HWND list = GetDlgItem(window, 0x530);
							if (list) {
								int row = ListBox_GetCurSel(list);
								if (row != LB_ERR) {
									ThemeType theme = (ThemeType)ListBox_GetItemData(list, row);
									Options.InGameMusic = true;
									if (Theme.What_Is_Playing() != theme) {
										Theme.Stop(true);
										Theme.Queue_Song(theme);
									}
								}
							}
						}
						break;
				}
				break;

			case WM_HSCROLL:
				if (DAT_00B0B800) {
					HWND track = (HWND)lparam;
					if (track == GetDlgItem(window, 0x52F)) {
						Options.Set_Score_Volume(Slider_GetPos(track) * 0.1, true);
					} else if (track == GetDlgItem(window, 0x532)) {
						Options.Set_Sound_Volume(Slider_GetPos(track) * 0.1, true);
					} else if (track == GetDlgItem(window, 0x536)) {
						Options.Set_Voice_Volume(Slider_GetPos(track) * 0.1, true);
					}
				}
				break;
		}
		return(FALSE);
	}

	return(rc);
}
