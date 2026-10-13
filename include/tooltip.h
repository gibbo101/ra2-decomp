// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "index.h"
#include "noinit.h"
#include "rect.h"
#include "vector.h"
#include "win.h"

class ToolTip
{
public:
	ToolTip(void) : ID(0), Region(0, 0, 0, 0), Text(0), Flag(false) {}
	~ToolTip(void){}
	ToolTip(ToolTip const &that) : ID(that.ID), Region(that.Region), Text(that.Text), Flag(that.Flag) {}
	ToolTip(NoInitClass const &) {}

	unsigned ID;

	Rect Region;

	int Text;

	bool Flag;
};

struct ToolTipText
{
	POINT Pos;
	int TextWidth;
	int TextHeight;
	wchar_t Text[256];
};

class ToolTipManager
{
	public:
		ToolTipManager(HWND window);
		ToolTipManager(NoInitClass const &) {}
		virtual ~ToolTipManager(void);

		void Activate(bool state);

		void Message_Handler(MSG *msg);

		int Get_Timer_Delay(void);
		void Set_Timer_Delay(int delay);

		void Save_Timer_Delay(void);
		void Restore_Timer_Delay(void);

		int Get_Lifetime(void);
		void Set_Lifetime(int lifetime);

		int Get_Count(void);

		bool Add(ToolTip const * tooltip);
		void Remove(unsigned id);

		bool Find(unsigned id, ToolTip * tooltip);

		ToolTip const * Find_From_Pos(Point2D &xy)
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

		virtual bool Update(ToolTipText *text);
		virtual void Reset(ToolTipText const *text);

		bool Process(void);

		virtual void Draw_Current(bool refresh = false);
		virtual void Draw(const ToolTipText *text);
		virtual const char *ToolTip_Text(int id);

		void Reset_Current(void)
		{
			if (CurrentToolTip != NULL) {
				Reset(&CurrentToolTipInfo);
			}
		}

		enum {
			TOOLTIP_EVENT = 'TTIP',
			TOOLTIP_DELAY = 1000,
			TOOLTIP_LIFETIME = 10000,
		};

	private:

		ToolTip const * CurrentToolTip;

		HWND Window;

		bool IsActive;

		POINT LastMousePos;

		ToolTipText CurrentToolTipInfo;

		int ToolTipDelay;

		int SavedToolTipDelay;

		int ToolTipLifetime;

		DynamicVectorClass<ToolTip const *> ToolTips;

		IndexClass<unsigned, ToolTip const *> ToolTipIndex;
};
