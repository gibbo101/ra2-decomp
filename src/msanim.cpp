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

struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
};

SHPStruct * __fastcall FUN_004A3970(Wstring const & name, bool * loaded);
void __fastcall FUN_00406670(char const * name, int volume);

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

// FUNCTION: 0x005CCA10
void MSButtonAnim::Restore(Rect const & rect)
{
}

// FUNCTION: 0x005CCA20
MSVQAnim::MSVQAnim(Wstring const & name, Surface * surface, MS_ANIM_LIST * vector, Rect const & rect, bool persistent) :
	MSAnim(0, 0, false),
	Anims(vector),
	Movie(NULL),
	TargetSurface(surface),
	Background(NULL),
	Persistent(persistent),
	Done(false)
{
	if (name != (char const *)NULL && surface != NULL) {
		Movie = Movie_Create(name.text(), surface, Rect(0, 0, 0, 0), Rect(0, 0, 0, 0), int(Options.SoundVolume * 255.0), false);
		if (Movie != NULL) {
			Movie->InitialRect = Rect((surface->Get_Width() - Drawing::RenderWidth) / 2, (surface->Get_Height() - Drawing::RenderHeight) / 2, Drawing::RenderWidth, Drawing::RenderHeight);
			Movie->StretchRect = rect;
		}
		Wstring token;
		name.getToken(0, ".", token);
		CCFileClass file(token + ".PCX");
		if (file.Is_Available(0)) {
			Background = Read_PCX_File(file);
		}
	}
}

// FUNCTION: 0x005CCC30
MSVQAnim::MSVQAnim(Wstring const & name, Surface * surface, MS_ANIM_LIST * vector, bool persistent) :
	MSAnim(0, 0, false),
	Anims(vector),
	Movie(NULL),
	TargetSurface(surface),
	Background(NULL),
	Persistent(persistent),
	Done(false)
{
	if (name != (char const *)NULL && surface != NULL) {
		Movie = Movie_Create(name.text(), surface, Rect(0, 0, 0, 0), Rect(0, 0, 0, 0), int(Options.SoundVolume * 255.0), false);
		if (Movie != NULL) {
			Movie->InitialRect = Rect((surface->Get_Width() - 640) / 2, (surface->Get_Height() - 400) / 2, 640, 400);
			Movie->StretchRect = Rect((HiddenSurface->Get_Width() - 640) / 2, (HiddenSurface->Get_Height() - 400) / 2, 640, 400);
		}
		Wstring token;
		name.getToken(0, ".", token);
		CCFileClass file(token + ".PCX");
		if (file.Is_Available(0)) {
			Background = Read_PCX_File(file);
		}
	}
}

// FUNCTION: 0x005CCE40
MSVQAnim::~MSVQAnim(void)
{
	if (Background != NULL) {
		delete Background;
	}

	if (Movie != NULL) {
		Movie_Destroy(Movie);
		delete Movie;
	}
}

// FUNCTION: 0x005CCE90
bool MSVQAnim::Advance(Surface * surface, Rect & rect)
{
	if (Movie != NULL) {
		bool is_done = false;
		if (!Done) {
			bool advanced = Movie_Advance_Frame(Movie, is_done);
			if (advanced == true) {
				Redraw(surface);
				rect = Movie->StretchRect;

				int my_id = Anims->ID(this);
				for (int i = 0; i < Anims->Count(); i++) {
					if (i != my_id) {
						(*Anims)[i]->Redraw(surface, &Movie->StretchRect);
					}
				}
			}

			if (is_done == true) {
				if (Background != NULL) {
					TargetSurface->Blit_From(Movie->InitialRect, *Background, Background->Get_Rect());
					Redraw(surface);
					rect = Movie->StretchRect;

					int my_id = Anims->ID(this);
					for (int i = 0; i < Anims->Count(); i++) {
						if (i != my_id) {
							(*Anims)[i]->Redraw(surface, &Movie->StretchRect);
						}
					}
				}
			}
		}

		if (is_done) {
			Done = is_done;
		}
	} else {
		if (!Done) {
			if (Background != NULL) {
				Rect backdrop_rect = Background->Get_Rect();
				TargetSurface->Blit_From(backdrop_rect, *Background, backdrop_rect);
				Redraw(surface);
				rect = backdrop_rect;

				int my_id = Anims->ID(this);
				for (int i = 0; i < Anims->Count(); i++) {
					if (i != my_id) {
						(*Anims)[i]->Redraw(surface, &backdrop_rect);
					}
				}
			}
		}
		Done = true;
	}

	return(Done && !Persistent);
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

// FUNCTION: 0x005CD240
void MSVQAnim::Restore(const Rect & rect)
{
	if (Done && Movie != NULL && Background != NULL) {
		TargetSurface->Blit_From(Movie->InitialRect, *Background, Background->Get_Rect());
	}
}

// FUNCTION: 0x005CD290
Rect MSVQAnim::Get_Rect(void) const
{
	static Rect _rect_none(0,0,0,0);
	return(Movie != NULL ? Movie->StretchRect : _rect_none);
}

// FUNCTION: 0x005CD310 $E18

// FUNCTION: 0x005CD320
bool MSVQAnim::Has_Finished(void) const
{
	return(Done);
}

// FUNCTION: 0x005CD330
MSBitPrintAnim::MSBitPrintAnim(WideWstring const & text, Rect const & rect, bool flag) :
	MSAnim(rect.X, rect.Y, true),
	Text(text),
	Area(rect),
	FlagA(false),
	FlagB(flag)
{
}

// FUNCTION: 0x005CD3A0
MSBitPrintAnim::~MSBitPrintAnim(void)
{
}

// FUNCTION: 0x005CD3C0
void MSBitPrintAnim::Set_Flag(bool flag)
{
	FlagA = flag;
}

// FUNCTION: 0x005CD3D0
Rect MSBitPrintAnim::Get_Rect(void) const
{
	return(Area);
}

// FUNCTION: 0x005CD400
bool MSBitPrintAnim::Has_Finished(void) const
{
	return(FlagA);
}

// FUNCTION: 0x005CD570
MSPrintAnim::MSPrintAnim(WideWstring const & text, int x, int y, MSFont * font, Rect const & rect, int start_delay, int print_delay, bool fade_effect, bool center) :
	MSAnim(x, y, true),
	Text(text),
	LineStart(0),
	PrintedCharCount(start_delay),
	CompletionCount(0),
	PrintDelay(print_delay),
	FadeEffect(fade_effect),
	Font(font),
	Transient(false),
	Centered(center),
	Offset(0)
{
	Timer = start_delay;
	Set_Dimensions(rect);
	if (center) {
		int font_width = Font->Get_Font_Width();
		int width = 0;
		for (int i = 0; Text.getChar(i) != 0 && Text.getChar(i) != '\n'; i++) {
			width += Font->Get_Character_Width(Text.getChar(i));
		}
		Offset = (Area.Width - width - font_width) / 2;
	}
}

// FUNCTION: 0x005CD690
void MSPrintAnim::Set_Dimensions(Rect const & rect)
{
	if (rect.Is_Valid()) {
		Font->Get_String_Rect(Text.get(), Area);

		XPos += (rect.Width - Area.Width) / 2;
		YPos += (rect.Height - (rect.Height / Font->Get_Font_Height() * Font->Get_Font_Height())) / 2;

		Area.X += XPos;
		Area.Y += YPos;

		Area = Intersect(rect, Area);
		XPos += Font->Get_Font_Width() / 2;
	} else {
		Font->Get_String_Rect(Text.get(), Area);
		Area.X = XPos;
		Area.Y = YPos;
	}
}

// FUNCTION: 0x005CD850
int MSPrintAnim::Word_Wrap(WideWstring & string, MSFont * font, int line_width)
{
	int max_width = 0;

	if (string == (wchar_t const *)NULL || font == NULL || line_width <= font->Get_Font_Width()) {
		return(0);
	}

	line_width -= font->Get_Font_Width();
	unsigned index = 0;

	while (string.getChar(index) != 0) {
		int current_width = 0;
		unsigned start = index;

		while (current_width < line_width && string.getChar(index) != 0 && string.getChar(index) != '\n' && string.getChar(index) != '@') {
			current_width += font->Get_Character_Width(string.getChar(index++));
		}

		if (current_width >= line_width) {
			while (string.getChar(index) != 0 && string.getChar(index) != ' ' && string.getChar(index) != '\n' && index != start) {
				index--;
				current_width -= font->Get_Character_Width(string.getChar(index));
			}
		}

		if (index == start) {
			while (current_width < line_width && string.getChar(index) != 0 && string.getChar(index) != ' ' && string.getChar(index) != '\n' && string.getChar(index) != '@' && string.getChar(index) != '-') {
				current_width += font->Get_Character_Width(string.getChar(index++));
			}

			if (current_width < line_width) {
				WideWstring head(string);
				head.truncate(++index);
				WideWstring tail(string);
				tail.remove(0, index);
				string = head + WideWstring(L"\n") + tail;
			} else {
				current_width -= font->Get_Character_Width(string.getChar(index));
				WideWstring head(string);
				head.truncate(index);
				WideWstring tail(string);
				tail.remove(0, index);
				string = head + WideWstring(L"\n") + tail;
			}
		}

		if (current_width > max_width) {
			max_width = current_width;
		}

		if (string.getChar(index) != 0) {
			string.set('\n', index++);
		}
	}

	return(max_width);
}

// FUNCTION: 0x005CDB90
int MSPrintAnim::Paginate(WideWstring & string, MSFont * font, int page_height)
{
	if (string == (wchar_t const *)NULL || font == NULL || page_height < font->Get_Font_Height()) {
		return(0);
	}

	unsigned index = 0;
	int overflow_count = 0;
	int current_height = 0;

	while (string.getChar(index) != 0) {
		while (string.getChar(index) != 0 && string.getChar(index) != '\n') {
			index++;
		}

		if (string.getChar(index) != '\n') {
			current_height += font->Get_Font_Height();
			if (current_height >= page_height) {
				current_height = 0;
				overflow_count++;
				string.set('\f', index);
			}
			index++;
		}
	}

	return(overflow_count);
}

// FUNCTION: 0x005CDC50
bool MSPrintAnim::Advance(Surface * surface, Rect & rect)
{
	rect.Set(0, 0, 0, 0);

	if (Timer != 0) return(false);

	if (Active != true) return(false);

	Timer = PrintDelay;

	if (PrintedCharCount > Text.length() && CompletionCount > 2) {
		return(!Transient);
	}

	if (FadeEffect == true) {
		surface->Blit_From(Area, *AlternateSurface, Area);
	}

	int x = XPos;
	bool should_center = Centered;
	int y = YPos;

	for (unsigned char_index = LineStart; char_index <= PrintedCharCount; char_index++) {
		wchar_t ch = Text.get(char_index);
		if (ch == 0) {
			CompletionCount++;
			break;
		}

		if (should_center) {
			WideWstring rest(Text);
			rest.remove(0, char_index);
			int font_width = Font->Get_Font_Width();
			int i = 0;
			int string_width = 0;
			for (; rest.getChar(i) != 0 && rest.getChar(i) != '\n'; i++) {
				string_width += Font->Get_Character_Width(rest.getChar(i));
			}
			Offset = (Area.Width - string_width - font_width) / 2;
			x += Offset;
			should_center = false;
		}

		if (ch == '\n') {
			x = XPos;
			should_center = Centered;
			y += Font->Get_Font_Height();

			if (y + Font->Get_Font_Height() - YPos > Area.Height) {
				unsigned next_line_start = LineStart;
				while (Text.getChar(next_line_start) != 0 && Text.getChar(next_line_start) != '\n') {
					next_line_start++;
				}
				if (Text.getChar(next_line_start) == '\n') {
					next_line_start++;
				}
				LineStart = next_line_start;
				break;
			}
		} else {
			Font->Draw_Character(surface, ch, x, y, (FadeEffect == false || PrintedCharCount - char_index >= 2) ? 2 : (PrintedCharCount - char_index), true);
			x += Font->Get_Character_Width(ch);
		}
	}

	rect = Area;
	PrintedCharCount++;

	return(false);
}

// FUNCTION: 0x005CDEE0
void MSPrintAnim::Redraw(Surface * surface, Rect const * rect)
{
	if (rect == NULL || Intersect(*rect, Area).Is_Valid()) {
		int x = XPos;
		bool should_center = Centered;
		int y = YPos;

		unsigned printed = Get_Printed_Char_Count();
		unsigned end = MIN(printed, Text.length());

		for (unsigned char_index = LineStart; char_index < end; char_index++) {
			if (Text.getChar(char_index) == '\n') {
				x = XPos;
				should_center = Centered;
				y += Font->Get_Font_Height();
				if (y + Font->Get_Font_Height() >= surface->Get_Height()) {
					return;
				}
			} else {
				if (should_center) {
					WideWstring rest(Text);
					rest.remove(0, char_index);
					int i = 0;
					int font_width = Font->Get_Font_Width();
					int string_width = 0;
					for (; rest.getChar(i) != 0 && rest.getChar(i) != '\n'; i++) {
						string_width += Font->Get_Character_Width(rest.getChar(i));
					}
					Offset = (Area.Width - string_width - font_width) / 2;
					x += Offset;
					should_center = false;
				}
				Font->Draw_Character(surface, Text.getChar(char_index), x, y, 2, true);
				x += Font->Get_Character_Width(Text.getChar(char_index));
			}
		}
	}
}

// FUNCTION: 0x005CE0C0
bool MSWordAnim::Advance(Surface * surface, Rect & rect)
{
	if (Timer != 0 || Active != true) {
		rect.Set(0, 0, 0, 0);
		return(false);
	}

	Timer = PrintDelay;

	if (LineStart >= Text.length()) {
		FUN_00406670("bleep1", int(Options.SoundVolume * 64.0f));
		return(true);
	}

	if (FadeEffect == true && LineRect.Is_Valid()) {
		surface->Blit_From(LineRect, *AlternateSurface, LineRect);
	}

	int x = XPos;

	if (Centered) {
		x += Offset;
	}

	unsigned line_end_index = LineStart;
	wchar_t character = Text.getChar(line_end_index++);
	while (character != '\n' && character != 0) {
		Font->Draw_Character(surface, character, x, LineYPos, PrintedCharCount, false);
		x += Font->Get_Character_Width(character);
		character = Text.getChar(line_end_index++);
	}

	rect.Set(Area.X, LineYPos, Area.Width, Font->Get_Font_Height());

	if (PrintedCharCount == 2) {
		LineRect.Set(0, 0, 0, 0);
		LineStart = line_end_index;
		if (character == '\n') {
			FUN_00406670("bleep1", int(Options.SoundVolume * 64.0f));
			LineYPos += Font->Get_Font_Height();
		}
	} else {
		LineRect = rect;
	}

	if (FadeEffect == true) {
		PrintedCharCount++;
		if (PrintedCharCount > 2) {
			PrintedCharCount = 0;
		}
		return(false);
	}

	return(false);
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

// FUNCTION: 0x005CE300
bool MSPrintAnim::Has_Finished(void) const
{
	return(Transient && PrintedCharCount > Text.length() && CompletionCount > 2);
}

// FUNCTION: 0x005CE330
int MSPrintAnim::Get_Line_Width(WideWstring const & str) const
{
	int i = 0;
	int width = 0;
	for (; str.getChar(i) != 0 && str.getChar(i) != '\n'; i++) {
		width += Font->Get_Character_Width(str.getChar(i));
	}
	return(width);
}

// FUNCTION: 0x005CE3A0
void MSWordAnim::Redraw(Surface * surface, Rect const * rect)
{
	if (rect == NULL || Intersect(*rect, Area).Is_Valid()) {
		int x = XPos;
		int y = YPos;

		unsigned end = LineStart;

		for (unsigned char_index = 0; char_index < end; char_index++) {
			if (Text.getChar(char_index) == '\n') {
				x = XPos;
				y += Font->Get_Font_Height();
				if (y + Font->Get_Font_Height() >= surface->Get_Height()) {
					return;
				}
			} else {
				Font->Draw_Character(surface, Text.getChar(char_index), x, y, 2, true);
				x += Font->Get_Character_Width(Text.getChar(char_index));
			}
		}
	}
}

// FUNCTION: 0x005CE4A0
MSPCXAnim::MSPCXAnim(Wstring const & name, MS_ANIM_LIST * vector, bool transient) :
	MSAnim(0, 0, false),
	TargetSurface(AlternateSurface),
	Anims(vector),
	Image(NULL),
	Transient(transient),
	Drawn(false),
	Area(0, 0, 0, 0)
{
	Active = true;

	if (name != (char const *)NULL) {
		Wstring token;
		name.getToken(0, ".", token);
		CCFileClass file(token + ".PCX");
		if (file.Is_Available(0)) {
			Image = Read_PCX_File(file);
			if (Image != NULL) {
				Area = Image->Get_Rect();
				Rect surface_rect = TargetSurface->Get_Rect();
				Area.X += (surface_rect.Width - Area.Width) / 2;
				Area.Y += (surface_rect.Height - Area.Height) / 2;
			}
		}
	}
}

// FUNCTION: 0x005CE640
MSPCXAnim::MSPCXAnim(Wstring const & name, MS_ANIM_LIST * vector, Point2D const & position, bool transient) :
	MSAnim(0, 0, false),
	TargetSurface(AlternateSurface),
	Anims(vector),
	Image(NULL),
	Transient(transient),
	Drawn(false),
	Area(0, 0, 0, 0)
{
	Active = true;

	if (name != (char const *)NULL) {
		Wstring token;
		name.getToken(0, ".", token);
		CCFileClass file(token + ".PCX");
		if (file.Is_Available(0)) {
			Image = Read_PCX_File(file);
			if (Image != NULL) {
				Area = Image->Get_Rect();
				Area.X = position.X;
				Area.Y = position.Y;
				TargetSurface->Get_Rect();
			}
		}
	}
}

// FUNCTION: 0x005CE7B0
MSPCXAnim::~MSPCXAnim(void)
{
	if (Image) {
		delete Image;
	}
}

// FUNCTION: 0x005CE7D0
bool MSPCXAnim::Advance(Surface * surface, Rect & rect)
{
	if (!Drawn) {
		if (Active) {
			if (Image != NULL) {
				TargetSurface->Blit_From(Area, *Image, Image->Get_Rect());
				Redraw(surface);

				Rect rect2 = Area;
				rect = rect2;

				int my_id = Anims->ID(this);
				for (int i = 0; i < Anims->Count(); i++) {
					if (i != my_id) {
						(*Anims)[i]->Redraw(surface, &rect2);
					}
				}
			}
			Drawn = true;
		}
	}

	return(Drawn && !Transient);
}

// FUNCTION: 0x005CE8C0
void MSPCXAnim::Redraw(Surface * surface, Rect const * rect)
{
	if (Image != NULL && !Drawn && Active) {
		if (rect == NULL || Intersect(*rect, Area).Is_Valid()) {
			surface->Blit_From(Area, *Image, Image->Get_Rect());
		}
	}
}

// FUNCTION: 0x005CEA40
void MSPCXAnim::Restore(Rect const & rect)
{
	if (Image != NULL && Active) {
		TargetSurface->Blit_From(Area, *Image, Image->Get_Rect());
	}
}

// FUNCTION: 0x005CEA80
Rect MSPCXAnim::Get_Rect(void) const
{
	return(Area);
}

// FUNCTION: 0x005CEAB0
bool MSPCXAnim::Has_Finished(void) const
{
	return(Drawn);
}

// FUNCTION: 0x005CEAC0 MSAnim::Set_Active

// FUNCTION: 0x005CEAD0 MSAnim::Pause

// FUNCTION: 0x005CEB20 MSAnim::Resume

// FUNCTION: 0x005CEB40 MSAnim::Has_Finished
