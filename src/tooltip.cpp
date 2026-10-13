// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tooltip.h"
#include "data.h"
#include "dbgprint.h"

extern bool DAT_00A8F7D8;

// FUNCTION: 0x00724000
ToolTipManager::ToolTipManager(HWND window) :
	Window(window),
	IsActive(false),
	CurrentToolTip(NULL),
	ToolTipDelay(TOOLTIP_DELAY),
	ToolTipLifetime(TOOLTIP_LIFETIME)
{
	ToolTips.Clear();
	ToolTipIndex.Clear();
}

// FUNCTION: 0x007240B0
ToolTipManager::~ToolTipManager(void)
{
	KillTimer(Window, TOOLTIP_EVENT);

	Reset_Current();

	ToolTipIndex.Clear();

	for (int i = 0; i < ToolTips.Count(); i++) {
		ToolTip * ptr = (ToolTip *)ToolTips[i];
		if (ptr != NULL) {
			delete ptr;
		}
	}

	ToolTips.Clear();
}

// FUNCTION: 0x007241A0
void ToolTipManager::Activate(bool state)
{
	if (IsActive != state) {
		IsActive = state;
		if (!IsActive) {
			KillTimer(Window, TOOLTIP_EVENT);
			Reset_Current();
		}

		DebugString("Tooltips are %s.\n", IsActive == true ? "on" : "off");
	}
}

// FUNCTION: 0x00724200
void ToolTipManager::Message_Handler(MSG * msg)
{
	if (IsActive == true) {

		switch (msg->message) {
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
				KillTimer(Window, TOOLTIP_EVENT);
				Reset_Current();
				break;

			case WM_MOUSEMOVE:
				if (ToolTipDelay != 0 && !DAT_00A8F7D8) {
					KillTimer(Window, TOOLTIP_EVENT);
					SetTimer(Window, TOOLTIP_EVENT, ToolTipDelay, NULL);
					Reset_Current();
				} else {
					GetCursorPos((LPPOINT)&LastMousePos);
					ScreenToClient(Window, (LPPOINT)&LastMousePos);
					CurrentToolTip = Find_From_Pos((Point2D &)LastMousePos);
					if (Process()) {
						SetTimer(Window, TOOLTIP_EVENT, ToolTipLifetime, NULL);
					}
				}
				break;

			case WM_TIMER:
				if (msg->wParam == TOOLTIP_EVENT) {
					KillTimer(Window, TOOLTIP_EVENT);
					if (CurrentToolTip != NULL) {
						Reset_Current();
					} else {
						GetCursorPos((LPPOINT)&LastMousePos);
						ScreenToClient(Window, (LPPOINT)&LastMousePos);
						CurrentToolTip = Find_From_Pos((Point2D &)LastMousePos);
						if (Process()) {
							SetTimer(Window, TOOLTIP_EVENT, ToolTipLifetime, NULL);
						}
					}
				}
				break;
		}
	}
}

// FUNCTION: 0x00724510
int ToolTipManager::Get_Timer_Delay(void)
{
	return(ToolTipDelay);
}

// FUNCTION: 0x00724520
void ToolTipManager::Set_Timer_Delay(int delay)
{
	ToolTipDelay = delay;
}

// FUNCTION: 0x00724530
void ToolTipManager::Save_Timer_Delay(void)
{
	SavedToolTipDelay = ToolTipDelay;
}

// FUNCTION: 0x00724540
void ToolTipManager::Restore_Timer_Delay(void)
{
	ToolTipDelay = SavedToolTipDelay;
}

// FUNCTION: 0x00724550
int ToolTipManager::Get_Lifetime(void)
{
	return(ToolTipLifetime);
}

// FUNCTION: 0x00724560
void ToolTipManager::Set_Lifetime(int lifetime)
{
	ToolTipLifetime = lifetime;
}

// FUNCTION: 0x00724570
int ToolTipManager::Get_Count(void)
{
	return(ToolTips.Count());
}

// FUNCTION: 0x00724BB0
void ToolTipManager::Draw(const ToolTipText *text)
{
	DebugString("ToolTip: %s\n", text->Text);
}
