// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "keyboard.h"
#include "link.h"

#include "scheme.hh"

class ControlClass;

#ifndef DEFAULT_GADGET_SCHEME
#define DEFAULT_GADGET_SCHEME "Grey"
#endif

class GadgetClass : public LinkClass
{
		typedef LinkClass BASECLASS;

	public:
		enum FlagEnum {
			LEFTPRESS    = 0x0001,
			LEFTHELD     = 0x0002,
			LEFTRELEASE  = 0x0004,
			LEFTUP       = 0x0008,
			RIGHTPRESS   = 0x0010,
			RIGHTHELD    = 0x0020,
			RIGHTRELEASE = 0x0040,
			RIGHTUP      = 0x0080,
			KEYBOARD     = 0x0100
		};

		GadgetClass(int x, int y, int w, int h, unsigned flags, bool sticky=false);
		GadgetClass(NoInitClass const & x) : BASECLASS(x) {};
		GadgetClass(void) {};
		GadgetClass(GadgetClass const & gadget);
		virtual ~GadgetClass(void) override;

		virtual KeyNumType Input(void);
		virtual void Draw_All(bool forced=true);
		virtual void Delete_List(void);
		virtual ControlClass * Extract_Gadget(unsigned id);
		virtual void Flag_List_To_Redraw(void) {LastList = 0;};
		virtual LinkClass * Remove(void) override;
		virtual LinkClass * Get_Next(void) const override;
		virtual LinkClass * Get_Prev(void) const override;

		virtual void Disable(void);
		virtual void Enable(void);
		virtual unsigned Get_ID(void) const {return(0);};
		virtual void Flag_To_Redraw(void);
		virtual void Peer_To_Peer(unsigned , KeyNumType & , ControlClass & ) {};
		virtual void Set_Focus(void);
		virtual void Clear_Focus(void);
		virtual bool Has_Focus(void);
		virtual int  Is_List_To_Redraw(void);
		virtual bool Is_To_Redraw(void) {return(IsToRepaint);}
		virtual void Set_Position(int x, int y);
		virtual void Set_Size(int w, int h);

		void Set_Flags(unsigned flags) { Flags = flags; }

		bool Is_Enabled(void) const;
		GadgetClass * Extract_Gadget_At(int x, int y);

		virtual int Draw_Me(int forced=false);
		virtual void OnMouseEnter(void);
		virtual void OnMouseLeave(void);

		static void Set_Color_Scheme(const char *scheme) { CurrentColorScheme = scheme; }

		static int Get_Color_Scheme(void);

		int X;
		int Y;
		int Width;
		int Height;

	protected:

		virtual void Sticky_Process(unsigned flags);

		virtual int Action(unsigned flags, KeyNumType & key, int modifier);

		static GadgetClass * LastList;

		static GadgetClass * Focused;

		bool IsToRepaint;

	public:

		bool IsSticky;

	protected:

		static GadgetClass * StuckOn;

		bool IsDisabled;

		unsigned Flags;

	public:

		static const char *CurrentColorScheme;

	private:
		virtual int Clicked_On(KeyNumType & key, unsigned flags, int x, int y, int modifier);
};
