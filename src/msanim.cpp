// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msanim.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"
#include "movies.h"
#include "msfont.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "shapeset.h"
#include "srfcache.h"
#include "wstring.h"

extern ShapeFlags_Type FadeStages[4];

SHPStruct * __fastcall FUN_004A3970(Wstring const & name, bool * loaded);

// FUNCTION: 0x005CB590
ConvertClass * Create_Drawer(Wstring const & palette_name)
{
	if (palette_name == (char const *)NULL) {
		return(NULL);
	}

	CCFileClass file(palette_name);

	if (!file.Is_Available(0)) {
		return(NULL);
	}

	PaletteClass * palette = (PaletteClass *)Load_Alloc_Data(file);
	if (palette == NULL) {
		return(NULL);
	}

	for (int gindex = 0; gindex < 256; gindex++) {
		CCPalette[gindex] = RGBClass(
			((unsigned char *)*palette)[gindex*3]<<2,
			((unsigned char *)*palette)[gindex*3+1]<<2,
			((unsigned char *)*palette)[gindex*3+2]<<2);
	}

	delete palette;

	return(new ConvertClass(CCPalette, CCPalette, *VisibleSurface));
}

// FUNCTION: 0x005CB6C0
MSAnim::~MSAnim(void)
{
}

// FUNCTION: 0x005CB6D0
MSShapeAnim::MSShapeAnim(Wstring const & name, int x, int y, ConvertClass * drawer, int rate, bool loop, ShapeFlags_Type flags) :
	MSAnim(x, y),
	Drawer(drawer),
	Loop(loop),
	Rate(rate),
	CurFrame(0),
	ShapeFlags(flags),
	AllocLoaded(false)
{
	Shape = FUN_004A3970(name, &AllocLoaded);
	StartFrame = 0;
	StopFrame = Shape->Get_Count() - 1;
}

// FUNCTION: 0x005CB750
MSShapeAnim::~MSShapeAnim(void)
{
	if (Shape != NULL && AllocLoaded) {
		delete Shape;
		Shape = NULL;
		AllocLoaded = false;
	}
}

// FUNCTION: 0x005CB790
void MSShapeAnim::Set_Frame(unsigned frame)
{
	Rect rect = Get_Rect();
	CurFrame = frame;
	if (rect != Get_Rect()) {
		HiddenSurface->Blit_From(rect, *AlternateSurface, rect);
	}
	if (CurFrame >= (unsigned)Shape->Get_Count()) {
		CurFrame = Shape->Get_Count() - 1;
	}
}

// FUNCTION: 0x005CB840
void MSShapeAnim::Set_Start_Frame(unsigned frame)
{
	StartFrame = frame;
	if (StartFrame >= (unsigned)Shape->Get_Count()) {
		StartFrame = Shape->Get_Count() - 1;
	}
}

// FUNCTION: 0x005CB860
void MSShapeAnim::Set_Stop_Frame(unsigned frame)
{
	StopFrame = frame;
	if (StopFrame >= (unsigned)Shape->Get_Count()) {
		StopFrame = Shape->Get_Count() - 1;
	}
}

// FUNCTION: 0x005CB880
bool MSShapeAnim::Advance(Surface * surface, Rect & rect)
{
	if (Active == true && Timer == 0) {

		Timer = Rate;

		Rect draw_rect = Shape->GetFrameBounds(CurFrame);
		draw_rect.X += XPos;
		draw_rect.Y += YPos;
		if (ShapeFlags & SHAPE_CENTER) {
			draw_rect.X -= Shape->Get_Width() / 2;
			draw_rect.Y -= Shape->Get_Height() / 2;
		}

		surface->Blit_From(draw_rect, *AlternateSurface, draw_rect);

		if (++CurFrame > StopFrame) {
			if (!Loop) {
				rect = draw_rect;
				return(true);
			}
			CurFrame = StartFrame;
		}

		Draw_Shape(*surface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), surface->Get_Rect(), ShapeFlags_Type(ShapeFlags | SHAPE_WIN_REL));

		rect = Shape->GetFrameBounds(CurFrame);
		rect.X += XPos;
		rect.Y += YPos;
		if (ShapeFlags & SHAPE_CENTER) {
			rect.X -= Shape->Get_Width() / 2;
			rect.Y -= Shape->Get_Height() / 2;
		}

		rect = Union(rect, draw_rect);
		return(false);
	}

	rect.Set(0, 0, 0, 0);
	return(false);
}

// FUNCTION: 0x005CBB80
void MSShapeAnim::Redraw(Surface * surface, Rect const * rect)
{
	Rect shape_rect(0, 0, 0, 0);

	if (rect != NULL) {

		shape_rect = Shape->GetFrameBounds(CurFrame);
		shape_rect.X += XPos;
		shape_rect.Y += YPos;
		if (ShapeFlags & SHAPE_CENTER) {
			shape_rect.X -= Shape->Get_Width() / 2;
			shape_rect.Y -= Shape->Get_Height() / 2;
		}

		shape_rect = Intersect(*rect, shape_rect);

		if (!shape_rect.Is_Valid()) {
			return;
		}
	}

	if (Active) {
		Draw_Shape(*surface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), surface->Get_Rect(), ShapeFlags_Type(ShapeFlags | SHAPE_WIN_REL));
	}
}

// FUNCTION: 0x005CBCB0
Rect MSShapeAnim::Get_Rect(void) const
{
	Rect rect = Shape->GetFrameBounds(CurFrame);
	rect.X += XPos;
	rect.Y += YPos;
	if (ShapeFlags & SHAPE_CENTER) {
		rect.X -= Shape->Get_Width() / 2;
		rect.Y -= Shape->Get_Height() / 2;
	}
	return(rect);
}

// FUNCTION: 0x005CBD20
MSFadeAnim::MSFadeAnim(Wstring const & name, int x, int y, ConvertClass * drawer, int rate, ShapeFlags_Type flags, MS_ANIM_LIST * vector) :
	MSShapeAnim(name, x, y, drawer, rate, false, flags),
	FadeStage(0),
	Anims(vector)
{
}

// FUNCTION: 0x005CBDA0
bool MSFadeAnim::Advance(Surface * surface, Rect & rect)
{
	if (Active == true && Timer == 0) {

		Timer = Rate;

		ShapeFlags_Type flags;
		if (FadeStage < 4) {
			flags = ShapeFlags_Type(ShapeFlags | FadeStages[FadeStage]);
		} else {
			flags = ShapeFlags_Type(ShapeFlags | FadeStages[3]);
		}

		Rect draw_rect = Shape->GetFrameBounds(CurFrame);
		draw_rect.X += XPos;
		draw_rect.Y += YPos;
		if (flags & SHAPE_CENTER) {
			draw_rect.X -= Shape->Get_Width() / 2;
			draw_rect.Y -= Shape->Get_Height() / 2;
		}

		surface->Blit_From(draw_rect, *AlternateSurface, draw_rect);

		if (Anims != NULL) {
			int my_id = Anims->ID(this);
			for (int i = 0; i < Anims->Count(); i++) {
				if (i != my_id) {
					(*Anims)[i]->Redraw(surface, &draw_rect);
				}
			}
		}

		if (++CurFrame > StopFrame) {
			if (!Loop) {
				rect.Set(0, 0, 0, 0);
				return(true);
			}
			CurFrame = StartFrame;
		}

		if (++FadeStage < 4) {
			flags = ShapeFlags_Type(ShapeFlags | FadeStages[FadeStage]);
		} else {
			flags = ShapeFlags_Type(ShapeFlags | FadeStages[3]);
		}

		Draw_Shape(*surface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), surface->Get_Rect(), flags);

		rect = Shape->GetFrameBounds(CurFrame);
		rect.X += XPos;
		rect.Y += YPos;
		if (flags & SHAPE_CENTER) {
			rect.X -= Shape->Get_Width() / 2;
			rect.Y -= Shape->Get_Height() / 2;
		}

		rect = Union(rect, draw_rect);
		return(false);
	}

	rect.Set(0, 0, 0, 0);
	return(false);
}

// FUNCTION: 0x005CC110
void MSFadeAnim::Redraw(Surface * surface, Rect const * rect)
{
	Rect shape_rect(0, 0, 0, 0);

	if (rect != NULL) {

		shape_rect = Shape->GetFrameBounds(CurFrame);
		shape_rect.X += XPos;
		shape_rect.Y += YPos;
		if (ShapeFlags & SHAPE_CENTER) {
			shape_rect.X -= Shape->Get_Width() / 2;
			shape_rect.Y -= Shape->Get_Height() / 2;
		}

		shape_rect = Intersect(*rect, shape_rect);

		if (!shape_rect.Is_Valid()) {
			return;
		}
	}

	ShapeFlags_Type flags;
	if (FadeStage < 4) {
		flags = ShapeFlags_Type(ShapeFlags | FadeStages[FadeStage]);
	} else {
		flags = ShapeFlags_Type(ShapeFlags | FadeStages[3]);
	}

	Draw_Shape(*surface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), surface->Get_Rect(), flags);
}

// FUNCTION: 0x005CC250
MSOverlayAnim::MSOverlayAnim(Wstring const & name, int x, int y, ConvertClass * drawer, int rate, MS_ANIM_LIST * vector, bool persistent, unsigned frame, bool fade) :
	MSFadeAnim(name, x, y, drawer, rate, SHAPE_NORMAL, vector),
	Persistent(persistent)
{
	CurFrame = frame;
	if (!fade) {
		FadeStage = 4;
	}
}

// FUNCTION: 0x005CC2F0
MSOverlayAnim::~MSOverlayAnim(void)
{
}

// FUNCTION: 0x005CC330
bool MSOverlayAnim::Advance(Surface * surface, Rect & rect)
{
	if (Active == true && Timer == 0) {

		Timer = Rate;

		if (FadeStage < 4) {

			rect = Shape->GetFrameBounds(CurFrame);
			rect.X += XPos;
			rect.Y += YPos;
			if (ShapeFlags & SHAPE_CENTER) {
				rect.X -= Shape->Get_Width() / 2;
				rect.Y -= Shape->Get_Height() / 2;
			}

			surface->Blit_From(rect, *AlternateSurface, rect);

			Draw_Shape(*surface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), surface->Get_Rect(), ShapeFlags_Type(ShapeFlags | FadeStages[FadeStage < 3 ? FadeStage : 3]));

			if (Anims != NULL) {
				int my_id = Anims->ID(this);
				for (int i = 0; i < Anims->Count(); i++) {
					if (i != my_id) {
						(*Anims)[i]->Redraw(surface, &rect);
					}
				}
			}

			if (++FadeStage >= 4) {
				Draw_Shape(*AlternateSurface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), surface->Get_Rect(), ShapeFlags_Type(ShapeFlags | SHAPE_WIN_REL));
			}
			return(!Persistent);
		}

		return(false);
	}

	rect.Set(0, 0, 0, 0);
	return(false);
}

// FUNCTION: 0x005CC550
void MSOverlayAnim::Redraw(Surface * surface, Rect const * rect)
{
	Rect shape_rect(0, 0, 0, 0);

	if (Active) {

		shape_rect = Shape->GetFrameBounds(CurFrame);
		shape_rect.X += XPos;
		shape_rect.Y += YPos;
		if (ShapeFlags & SHAPE_CENTER) {
			shape_rect.X -= Shape->Get_Width() / 2;
			shape_rect.Y -= Shape->Get_Height() / 2;
		}

		if (rect != NULL) {
			shape_rect = Intersect(*rect, shape_rect);
			if (!shape_rect.Is_Valid()) {
				return;
			}
		}

		int fading_stage = FadeStage < 3 ? FadeStage : 3;
		Draw_Shape(*surface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), surface->Get_Rect(), ShapeFlags_Type(ShapeFlags | FadeStages[fading_stage]));
	}
}

// FUNCTION: 0x005CC690
void MSOverlayAnim::Restart(void)
{
	FadeStage = 0;
}

// FUNCTION: 0x005CC6A0
void MSOverlayAnim::Set_Frame(int frame)
{
	FadeStage = 0;
	CurFrame = frame;
}

// FUNCTION: 0x005CC6C0
void MSOverlayAnim::Restore(Rect const & rect)
{
	int fading_stage = FadeStage < 3 ? FadeStage : 3;

	if (Active) {
		Draw_Shape(*AlternateSurface, *Drawer, (ShapeSet *)Shape, CurFrame, Point2D(XPos, YPos), AlternateSurface->Get_Rect(), ShapeFlags_Type(ShapeFlags | FadeStages[fading_stage]));
	}
}

// FUNCTION: 0x005CC740
bool MSOverlayAnim::Has_Finished(void) const
{
	return(!(Active && FadeStage < 4));
}

// FUNCTION: 0x005CC760
MSButtonAnim::MSButtonAnim(Wstring const & text, int width, MS_ANIM_LIST * vector, Rect const & rect, bool persistent) :
	MSAnim(rect.X, rect.Y),
	Text(NULL),
	Width(width),
	Area(rect),
	Anims(vector),
	Persistent(persistent)
{
	if (text.length() > 0) {
		Text = new MSTextLayout(text.text(), width);
		Text->Set_Position(rect.X, rect.Y);
	}
}

// FUNCTION: 0x005CC820
MSButtonAnim::~MSButtonAnim(void)
{
	if (Text != NULL) {
		delete Text;
	}
}

// FUNCTION: 0x005CC850
Rect MSButtonAnim::Get_Rect(void) const
{
	return(Area);
}


// FUNCTION: 0x005CC880
bool MSButtonAnim::Has_Finished(void) const
{
	return(Text == NULL || Text->Is_Complete());
}

// FUNCTION: 0x005CC8A0
bool MSButtonAnim::Advance(Surface * surface, Rect & rect)
{
	if (surface == NULL) {
		return(false);
	}
	if (Text == NULL) {
		return(true);
	}

	if (!Text->Is_Complete() && Text->Is_Dirty()) {
		Redraw(surface, NULL);
		rect = Area;
		int my_id = Anims->ID(this);
		for (int i = 0; i < Anims->Count(); i++) {
			if (i != my_id) {
				(*Anims)[i]->Redraw(surface, &Area);
			}
		}
	}

	return(Text->Is_Complete() && !Persistent);
}

// FUNCTION: 0x005CC970
void MSButtonAnim::Redraw(Surface * surface, Rect const * rect)
{
	if (rect != NULL) {
		Intersect(*rect, Area);
	}
	surface->Blit_From(Area, *AlternateSurface, Area);
}

// FUNCTION: 0x005CD0E0
void MSVQAnim::Redraw(Surface * surface, const Rect * rect)
{
	if (Movie != NULL && !Done) {
		if (rect == NULL || Intersect(*rect, Movie->StretchRect).Is_Valid()) {
			surface->Blit_From(Movie->InitialRect, *AlternateSurface, Movie->StretchRect);
		}
	}
}

// FUNCTION: 0x005CD290
Rect MSVQAnim::Get_Rect(void) const
{
	static Rect _rect_none(0,0,0,0);
	return(Movie != NULL ? Movie->StretchRect : _rect_none);
}

// FUNCTION: 0x005CE2B0
Rect MSPrintAnim::Get_Rect(void) const
{
	return(Area);
}

// FUNCTION: 0x005CE2E0
void MSPrintAnim::Set_Transient(bool transient)
{
	if (transient != Transient) {
		Transient = transient;
	}
}

// FUNCTION: 0x005CEA80
Rect MSPCXAnim::Get_Rect(void) const
{
	return(Area);
}
