// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tooltip.h"
#include "data.h"
#include "dbgprint.h"
#include "point.h"
#include "rect.h"
#undef LoadString
#include <stddef.h>
#include "stringtable.h"
#include "bsearch.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x00723FE0 SYMBOL
// _$E2


extern bool DAT_00A8F7D8;

// FUNCTION: GAMEMD 0x00724000
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

// FUNCTION: GAMEMD 0x007240B0
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

// FUNCTION: GAMEMD 0x007241A0
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

// FUNCTION: GAMEMD 0x00724200
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

// FUNCTION: GAMEMD 0x00724510
int ToolTipManager::Get_Timer_Delay(void)
{
	return(ToolTipDelay);
}

// FUNCTION: GAMEMD 0x00724520
void ToolTipManager::Set_Timer_Delay(int delay)
{
	ToolTipDelay = delay;
}

// FUNCTION: GAMEMD 0x00724530
void ToolTipManager::Save_Timer_Delay(void)
{
	SavedToolTipDelay = ToolTipDelay;
}

// FUNCTION: GAMEMD 0x00724540
void ToolTipManager::Restore_Timer_Delay(void)
{
	ToolTipDelay = SavedToolTipDelay;
}

// FUNCTION: GAMEMD 0x00724550
int ToolTipManager::Get_Lifetime(void)
{
	return(ToolTipLifetime);
}

// FUNCTION: GAMEMD 0x00724560
void ToolTipManager::Set_Lifetime(int lifetime)
{
	ToolTipLifetime = lifetime;
}

// FUNCTION: GAMEMD 0x00724570
int ToolTipManager::Get_Count(void)
{
	return(ToolTips.Count());
}

// FUNCTION: GAMEMD 0x00724580
bool ToolTipManager::Add(ToolTip const * tooltip)
{
	if (!Find(tooltip->ID, NULL)) {
		ToolTip * tt = new ToolTip(*tooltip);
		if (tt != NULL) {
			if (ToolTips.Add(tt)) {
				ToolTipIndex.Add_Index(tt->ID, tt);
				return(true);
			}
			delete tt;
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00724730
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

// FUNCTION: GAMEMD 0x007248C0
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

// FUNCTION: GAMEMD 0x00724A30
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

// FUNCTION: GAMEMD 0x00724AA0
bool ToolTipManager::Update(ToolTipText *text)
{
	return(true);
}

// FUNCTION: GAMEMD 0x00724AB0
void ToolTipManager::Reset(const ToolTipText *text)
{
	CurrentToolTipInfo.Pos.x = 0;
	CurrentToolTipInfo.Pos.y = 0;
	CurrentToolTipInfo.TextWidth = 0;
	CurrentToolTipInfo.TextHeight = 0;
	CurrentToolTip = NULL;
}

// FUNCTION: GAMEMD 0x00724AD0
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

// FUNCTION: GAMEMD 0x00724B80
void ToolTipManager::Draw_Current(bool refresh)
{
	if (refresh == true) {
		Process();
	}

	if (CurrentToolTip != NULL) {
		Draw(&CurrentToolTipInfo);
	}
}

// FUNCTION: GAMEMD 0x00724BB0
void ToolTipManager::Draw(const ToolTipText *text)
{
	DebugString("ToolTip: %s\n", text->Text);
}

// FUNCTION: GAMEMD 0x00724BD0
const char * ToolTipManager::ToolTip_Text(int id)
{
	return(NULL);
}

// FUNCTION: GAMEMD 0x00724BE0
void ToolTipManager::Hide(void)
{
	Reset_Current();
}

// FUNCTION: GAMEMD 0x00724C00
bool ToolTipManager::IsToolTipShowing(void)
{
	return(CurrentToolTip != NULL);
}

// SYNTHETIC: GAMEMD 0x00724C10
// ??_GToolTipManager@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00724C30
// ?Clear@?$DynamicVectorClass@PBVToolTip@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00724C60
// ?ID@?$DynamicVectorClass@PBVToolTip@@@@UAEHPBQBVToolTip@@@Z
// TEMPLATE: GAMEMD 0x00724C80
// ?GetItem@?$VectorClass@PBVToolTip@@@@UBEPBVToolTip@@H@Z
// TEMPLATE: GAMEMD 0x00724C90
// ??8?$VectorClass@PBVToolTip@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00724CD0
// ?Resize@?$VectorClass@PBVToolTip@@@@UAE_NHPBQBVToolTip@@@Z
// TEMPLATE: GAMEMD 0x00724D80
// ?Clear@?$VectorClass@PBVToolTip@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00724DB0
// ?ID@?$VectorClass@PBVToolTip@@@@UAEHABQBVToolTip@@@Z
template class VectorClass<ToolTip const *>;

// TEMPLATE: GAMEMD 0x00724DF0
// ?ID@?$VectorClass@PBVToolTip@@@@UAEHPBQBVToolTip@@@Z
// TEMPLATE: GAMEMD 0x00724E10
// ?Resize@?$DynamicVectorClass@PBVToolTip@@@@UAE_NHPBQBVToolTip@@@Z
// TEMPLATE: GAMEMD 0x00724EC0
// ?ID@?$DynamicVectorClass@PBVToolTip@@@@UAEHABQBVToolTip@@@Z
// SYNTHETIC: GAMEMD 0x00724EF0
// ??_G?$VectorClass@PBVToolTip@@@@UAEPAXI@Z
template class DynamicVectorClass<ToolTip const *>;

// SYNTHETIC: GAMEMD 0x00724F40
// ??_G?$DynamicVectorClass@PBVToolTip@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00724F90
// ??0NodeElement@?$IndexClass@IPBVToolTip@@@@QAE@XZ

// TEMPLATE: GAMEMD 0x00724FA0
// ?Is_Archive_Same@?$IndexClass@IPBVToolTip@@@@ABE_NABI@Z

// TEMPLATE: GAMEMD 0x00724FC0
// ?Search_For_Node@?$IndexClass@IPBVToolTip@@@@ABEPBUNodeElement@1@ABI@Z

// TEMPLATE: GAMEMD 0x00725040
// ?search_compfunc@?$IndexClass@IPBVToolTip@@@@CAHPBX0@Z

// TEMPLATE: GAMEMD 0x00725070
// ?Binary_Search@@YIPAUNodeElement@?$IndexClass@IPBVToolTip@@@@PAU12@HABU12@@Z
template class IndexClass<unsigned, ToolTip const *>;
