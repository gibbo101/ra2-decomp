// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rect.h"
#include "stimer.h"
#include "timer.h"
#include "vector.h"
#include "wstring.h"
#include "widewstring.h"

#include "draw.hh"

class ConvertClass;
class Wstring;
class MSFont;
class MSAnim;
class Surface;
class BSurface;
class ShapeSet;
class SurfaceCacheClass;
struct VQHandle;

class SHPStruct
{
	public:
		Rect GetFrameBounds(int frame) const;
		int Get_Width(void) const {return(Width);}
		int Get_Height(void) const {return(Height);}
		int Get_Count(void) const {return(Count);}

	private:
		short Flags;
		short Width;
		short Height;
		short Count;
};

typedef DynamicVectorClass<MSAnim *> MS_ANIM_LIST;
typedef VectorCursor<MSAnim *, MS_ANIM_LIST> MS_ANIM_ITER;

class MSAnim
{
	public:
		MSAnim(int x=0, int y=0, bool active=true) : XPos(x), YPos(y), Active(active) {}
		virtual ~MSAnim(void);

		virtual void Set_Active(bool active) { Active=active; }

		virtual void Pause(void) { Timer.Stop(); }
		virtual void Resume(void) { Timer.Start(); }

		virtual bool Advance(Surface * surface, Rect & rect) = 0;

		virtual void Redraw(Surface * surface, Rect const * rect=NULL) = 0;

		virtual Rect Get_Rect(void) const = 0;

		virtual bool Has_Finished(void) const { return(false); }

		virtual void Restore(Rect const & rect) { }

	protected:

		int XPos;
		int YPos;

		bool Active;

		CDTimerClass<SystemTimerClass> Timer;
};

class MSShapeAnim : public MSAnim
{
	public:
		MSShapeAnim(Wstring const & name, int x, int y, ConvertClass * drawer, int rate, bool loop=true, ShapeFlags_Type flags=SHAPE_NORMAL);
		virtual ~MSShapeAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;

		void Set_Frame(unsigned frame);
		void Set_Start_Frame(unsigned frame);
		void Set_Stop_Frame(unsigned frame);

	protected:

		SHPStruct * Shape;

		ConvertClass * Drawer;

		int Rate;

		unsigned CurFrame;

		unsigned StartFrame;

		unsigned StopFrame;

		bool Loop;

		ShapeFlags_Type ShapeFlags;

		bool AllocLoaded;
};

class MSFadeAnim : public MSShapeAnim
{
	public:
		MSFadeAnim(Wstring const & name, int x, int y, ConvertClass * drawer, int rate, ShapeFlags_Type flags, MS_ANIM_LIST * vector);
		virtual ~MSFadeAnim(void) override {}

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;

	protected:

		unsigned FadeStage;

		MS_ANIM_LIST * Anims;
};

class MSOverlayAnim : public MSFadeAnim
{
	public:
		MSOverlayAnim(Wstring const & name, int x, int y, ConvertClass * drawer, int rate, MS_ANIM_LIST * vector, bool persistent=false, unsigned frame=0, bool fade=true);
		virtual ~MSOverlayAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual bool Has_Finished(void) const override;
		virtual void Restore(Rect const & rect) override;

		void Restart(void);
		void Set_Frame(int frame);

	protected:

		bool Persistent;
};

class MSVQAnim : public MSAnim
{
	public:
		MSVQAnim(Wstring const & name, Surface * surface, MS_ANIM_LIST * vector, bool persistent=false);
		MSVQAnim(Wstring const & name, Surface * surface, MS_ANIM_LIST * vector, Rect const & rect, bool persistent=false);
		virtual ~MSVQAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;
		virtual bool Has_Finished(void) const override;
		virtual void Restore(Rect const & rect) override;

	protected:

		VQHandle * Movie;

		Surface * TargetSurface;

		MS_ANIM_LIST * Anims;

		Surface * Background;

		bool Persistent;

		bool Done;
};

class MSPrintAnim : public MSAnim
{
	public:
		MSPrintAnim(WideWstring const & text, int x, int y, MSFont * font, Rect const & rect=RECT_NONE, int start_delay=0, int print_delay=4, bool fade_effect=true, bool center=false);
		virtual ~MSPrintAnim(void) override {}

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;
		virtual bool Has_Finished(void) const override;

		void Set_Transient(bool transient);
		static int Word_Wrap(WideWstring & string, MSFont * font, int line_width);
		static int Paginate(WideWstring & string, MSFont * font, int page_height);

	protected:
		void Set_Dimensions(Rect const & rect);
		int Get_Line_Width(WideWstring const & str) const;
		int Get_Printed_Char_Count(void) const { return(PrintedCharCount); }

	protected:

		WideWstring Text;

		unsigned LineStart;

		unsigned PrintedCharCount;

		unsigned CompletionCount;

		int PrintDelay;

		bool FadeEffect;

		MSFont * Font;

		Rect Area;

		bool Transient;

		bool Centered;

		int Offset;
};

class MSWordAnim : public MSPrintAnim
{
	public:
		MSWordAnim(WideWstring const & text, int x, int y, MSFont * font, Rect const & rect=RECT_NONE, int start_delay=0, int print_delay=4, bool fade_effect=true, bool center=false) :
			MSPrintAnim(text, x, y, font, rect, start_delay, print_delay, fade_effect, center), LineYPos(YPos)
		{
			LineRect.Set(0, 0, 0, 0);
		}

		virtual ~MSWordAnim(void) override {}

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;

	protected:

		int LineYPos;

		Rect LineRect;
};

class MSBitPrintAnim : public MSAnim
{
	public:
		MSBitPrintAnim(WideWstring const & text, Rect const & rect, bool flag);
		virtual ~MSBitPrintAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;
		virtual bool Has_Finished(void) const override;

		void Set_Flag(bool flag);

	protected:

		WideWstring Text;

		Rect Area;

		bool FlagA;

		bool FlagB;
};

class MSTextLayout
{
	public:
		MSTextLayout(char const * text, int width);
		~MSTextLayout(void);

		void Set_Position(int x, int y);
		bool Is_Complete(void) const;
		bool Is_Dirty(void) const;

	private:
		char Data[0x34];
};

class MSButtonAnim : public MSAnim
{
	public:
		MSButtonAnim(Wstring const & text, int width, MS_ANIM_LIST * vector, Rect const & rect, bool persistent);
		virtual ~MSButtonAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;
		virtual bool Has_Finished(void) const override;
		virtual void Restore(Rect const & rect) override;

	protected:

		MSTextLayout * Text;

		int Width;

		Rect Area;

		MS_ANIM_LIST * Anims;

		bool Persistent;
};

class MSPCXAnim : public MSAnim
{
	public:
		MSPCXAnim(Wstring const & name, MS_ANIM_LIST * vector, bool transient=true);
		MSPCXAnim(Wstring const & name, MS_ANIM_LIST * vector, Point2D const & position, bool transient=true);
		virtual ~MSPCXAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;
		virtual bool Has_Finished(void) const override;
		virtual void Restore(Rect const & rect) override;

	public:

		Surface * TargetSurface;

		MS_ANIM_LIST * Anims;

		Surface * Image;

		bool Transient;

		bool Drawn;

		Rect Area;
};

ConvertClass * Create_Drawer(Wstring const & palette_name);
