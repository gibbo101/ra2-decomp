// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tooltip.h"
#include "data.h"
#include "dbgprint.h"
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

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

// FUNCTION: 0x00724730
void ToolTipManager::Remove(unsigned id)
{
	if (ToolTipIndex.Is_Present(id)) {
		ToolTip * tooltip = (ToolTip *)ToolTipIndex[id];

		if (CurrentToolTip == tooltip) {
			Reset_Current();
		}

		ToolTipIndex.Remove_Index(tooltip->ID);
		ToolTips.Delete(tooltip);

		if (tooltip != NULL) {
			delete tooltip;
		}
	}
}

// FUNCTION: 0x007248C0
bool ToolTipManager::Find(unsigned id, ToolTip * tooltip)
{
	if (!ToolTipIndex.Is_Present(id)) {
		return(false);
	}

	if (tooltip != NULL) {
		*tooltip = *ToolTipIndex[id];
	}

	return(true);
}

// FUNCTION: 0x00724A30
ToolTip const * ToolTipManager::Find_From_Pos(Point2D &xy)
{
	for (int i = 0; i < ToolTips.Count(); i++) {
		Rect const & region = ToolTips[i]->Region;
		if (xy.X >= region.X &&
			xy.X <= region.X + region.Width &&
			xy.Y >= region.Y &&
			xy.Y <= region.Y + region.Height) {

			return(ToolTips[i]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x00724AA0
bool ToolTipManager::Update(ToolTipText *text)
{
	return(true);
}

// FUNCTION: 0x00724AB0
void ToolTipManager::Reset(const ToolTipText *text)
{
	CurrentToolTipInfo.Pos.x = 0;
	CurrentToolTipInfo.Pos.y = 0;
	CurrentToolTipInfo.TextWidth = 0;
	CurrentToolTipInfo.TextHeight = 0;
	CurrentToolTip = NULL;
}

// FUNCTION: 0x00724AD0
bool ToolTipManager::Process(void)
{
	ToolTip const * cur = CurrentToolTip;
	if (cur != NULL) {
		const wchar_t *str = cur->Text == 0 ? (const wchar_t *)ToolTip_Text(cur->ID) : StringTable::LoadString((char const *)cur->Text, NULL, "D:\\ra2mdpost\\ToolTip.cpp", 0x233);

		if (str != NULL && wcslen(str) > 0) {

			wcsncpy(CurrentToolTipInfo.Text, str, 256);

			CurrentToolTipInfo.Pos.x = LastMousePos.x;
			CurrentToolTipInfo.Pos.y = LastMousePos.y;

			ToolTipText & txt = CurrentToolTipInfo;

			if (!Update(&CurrentToolTipInfo)) {
				txt.Pos.x = 0;
				txt.Pos.y = 0;
				txt.TextWidth = 0;
				txt.TextHeight = 0;
				CurrentToolTip = NULL;
			}

		} else {
			CurrentToolTipInfo.Pos.x = 0;
			CurrentToolTipInfo.Pos.y = 0;
			CurrentToolTipInfo.TextWidth = 0;
			CurrentToolTipInfo.TextHeight = 0;
			CurrentToolTip = NULL;
		}
	}

	return(CurrentToolTip != NULL);
}

// FUNCTION: 0x00724B80
void ToolTipManager::Draw_Current(bool refresh)
{
	if (refresh == true) {
		Process();
	}

	if (CurrentToolTip != NULL) {
		Draw(&CurrentToolTipInfo);
	}
}

// FUNCTION: 0x00724BB0
void ToolTipManager::Draw(const ToolTipText *text)
{
	DebugString("ToolTip: %s\n", text->Text);
}

// FUNCTION: 0x00724BD0
const char * ToolTipManager::ToolTip_Text(int id)
{
	return(NULL);
}

// FUNCTION: 0x00724BE0
void ToolTipManager::Hide(void)
{
	Reset_Current();
}

// FUNCTION: 0x00724C00
bool ToolTipManager::IsToolTipShowing(void)
{
	return(CurrentToolTip != NULL);
}

// FUNCTION: 0x00724C10 ToolTipManager::~ToolTipManager

// FUNCTION: 0x00724C30 ?$DynamicVectorClass@PBVToolTip@@::Clear
