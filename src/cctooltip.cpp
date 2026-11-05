// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cctooltip.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_xmouse.h"
#include "dialog.h"
#include "dsurface.h"
#include "goptions.h"
#include "scheme.h"
#include "geometry.h"
#include "wwfont.h"
#include "color.hh"
#include <math.h>
#include "bitfont.h"

// SYNTHETIC: GAMEMD 0x00478990 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004789B0 SYMBOL
// _$E64
// SYNTHETIC: GAMEMD 0x004789E0 SYMBOL
// _$E67
// SYNTHETIC: GAMEMD 0x00478A00 SYMBOL
// _$E70
// SYNTHETIC: GAMEMD 0x00478A20 SYMBOL
// _$E73
// SYNTHETIC: GAMEMD 0x00478A40 SYMBOL
// _$E76
// SYNTHETIC: GAMEMD 0x00478A60 SYMBOL
// _$E79
// SYNTHETIC: GAMEMD 0x00478A80 SYMBOL
// _$E82
// SYNTHETIC: GAMEMD 0x00478AC0 SYMBOL
// _$E85
// SYNTHETIC: GAMEMD 0x00478AF0 SYMBOL
// _$E88
// SYNTHETIC: GAMEMD 0x00478B20 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x00478B50 SYMBOL
// _$E94
// SYNTHETIC: GAMEMD 0x00478B60 SYMBOL
// _$E97
// SYNTHETIC: GAMEMD 0x00478B80 SYMBOL
// _$E100

// FUNCTION: GAMEMD 0x00478BA0
bool CCToolTip::Update(ToolTipText * text)
{
	if (Get_Mouse_State() >= 0) {
		BitFont *font = BitFont::Instance;
		Rect *trect;
		POINT &position = text->Pos;
		if (Options.IsSidebarOnRight == true) {
			if (position.x <= TacticalRect.X + TacticalRect.Width) {
				trect = &TacticalRect;
			} else {
				trect = &SidebarRect;
				Map.SidebarClass::IsToRedraw = true;
			}
		} else {
			if (position.x <= SidebarRect.X + SidebarRect.Width) {
				trect = &SidebarRect;
				Map.SidebarClass::IsToRedraw = true;
			} else {
				trect = &TacticalRect;
			}
		}

		if (trect) {
			int width;
			int height;
			font->GetTextDimension(text->Text, &width, &height, trect->Width);
			width += 4;
			height += 3;
			text->TextWidth = max(width, text->TextWidth);
			text->TextHeight = max(height, text->TextHeight);
			if (text->TextWidth >= trect->Width) {
				font->GetTextDimension(text->Text, &width, &height, trect->Width - 4);
				width += 4;
				height += 3;
				text->TextWidth = max(width, text->TextWidth);
				text->TextHeight = max(height, text->TextHeight);
			}
			if (CurrentToolTip && CurrentToolTip->Flag) {
				Rect owner = CurrentToolTip->Region;
				if (owner.X < trect->X + trect->Width / 2) {
					position.x = owner.X + owner.Width - 5;
				} else {
					position.x = owner.X - width + 5;
				}
				position.x = MAX(trect->X, position.x);
				if (owner.Y < trect->Y + trect->Height / 2) {
					position.y = owner.Y + owner.Height - 5;
				} else {
					position.y = owner.Y - height + 5;
				}
				position.y = MAX(trect->Y, position.y);
			} else {
				int x = position.x + text->TextWidth - trect->Width - trect->X;
				if (x > 0) {
					position.x -= x;
				}

				position.y += 16;
				if (position.y + text->TextHeight - trect->Height - trect->Y > 0) {
					position.y = position.y - text->TextHeight - 16;
				}
			}
		}
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00478DB0
void CCToolTip::Reset(const ToolTipText * text)
{
	bool redraw = false;
	if (Options.IsSidebarOnRight == true) {
		if (text->Pos.x >= TacticalRect.X + TacticalRect.Width) {
			redraw = true;
		}
	} else {
		if (text->Pos.x <= SidebarRect.X + SidebarRect.Width) {
			redraw = true;
		}
	}

	if (redraw) {
		Map.SidebarClass::IsToRedraw = true;
		Map.SidebarClass::IsForceCompleteRedraw = true;
	}
	Map.Flag_To_Redraw();
	BASECLASS::Reset(text);
}

// FUNCTION: GAMEMD 0x00478E10
void CCToolTip::Draw_Current(bool sidebar)
{
	UseSidebarSurface = sidebar;
	BASECLASS::Draw_Current(sidebar == 0);
}

// FUNCTION: GAMEMD 0x00479050
const char * CCToolTip::ToolTip_Text(int id)
{
	if (Get_Mouse_State() < 0) {
		return(NULL);
	}
	return(Map.Help_Text(id));
}

namespace Drawing { extern RGBClass TooltipColor; }

// FUNCTION: GAMEMD 0x00478E30
void CCToolTip::Draw(ToolTipText const * text)
{
	Rect drawrect;
	Point2D point(text->Pos.x, text->Pos.y);
	Surface * surface = NULL;
	if (Options.IsSidebarOnRight == true) {
		int offset = TacticalRect.X + TacticalRect.Width;
		if (point.X + text->TextWidth <= offset) {
			surface = CompositeSurface;
		} else if (UseSidebarSurface == true && point.X >= offset) {
			surface = SidebarSurface;
			point.X -= offset;
			Map.IsToBlitSidebar = true;
		}
	} else {
		int offset = SidebarRect.X + SidebarRect.Width;
		if (point.X >= offset) {
			surface = CompositeSurface;
			point.X -= offset;
		} else if (UseSidebarSurface == true && point.X + text->TextWidth < offset) {
			surface = SidebarSurface;
			Map.IsToBlitSidebar = true;
		}
	}
	if (surface) {
		LTRBStruct bounds;
		bounds.Right = bounds.Left = point.X;
		bounds.Bottom = bounds.Top = point.Y;
		BitFont * font = BitFont::Instance;
		int textwidth;
		int textheight;
		font->GetTextDimension(text->Text, &textwidth, &textheight, surface->Get_Width());
		bounds.Right = bounds.Left + textwidth;
		bounds.Bottom = bounds.Top + textheight;
		int width = bounds.Right - bounds.Left + 8;
		int height = bounds.Bottom - bounds.Top + 4;
		if (bounds.Left + width > surface->Get_Width()) bounds.Left = surface->Get_Width() - width;
		bounds.Right = bounds.Left + width;
		bounds.Bottom = bounds.Top + height;
		int color = DSurface::Build_Hicolor_Pixel(Drawing::TooltipColor);
		drawrect.Set(bounds.Left, bounds.Top, width, height);
		surface->Fill_Rect(drawrect, TBLACK);
		surface->Draw_Rect(drawrect, color);
		font->SetClipMode(true);
		font->SetBounds(&bounds);
		font->SetColor(color);
		BitText::Instance->DrawText(font, surface, text->Text, bounds.Left + 4, bounds.Top + 2,
			bounds.Right - bounds.Left, bounds.Bottom - bounds.Top, 0, 0, 0);
	}
}
