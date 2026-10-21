// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "tooltip.h"

#include "dialog.hh"

class CCToolTip : public ToolTipManager
{
		typedef ToolTipManager BASECLASS;

	public:
		CCToolTip(HWND hWnd) :
			BASECLASS(hWnd),

			Style(TPF_MAP)
		{
		}

		virtual ~CCToolTip() override {}

		virtual bool Update(ToolTipText *text) override;
		virtual void Reset(const ToolTipText *text);
		virtual void Draw_Current(bool sidebar = false) override;
		virtual void Draw(const ToolTipText *text) override;
		virtual const char *ToolTip_Text(int id) override;

	protected:

		bool UseSidebarSurface;

		TextPrintType Style;

};
