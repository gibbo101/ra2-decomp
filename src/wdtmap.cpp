// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "ini.h"
#include "wdtnet.h"

#include <cstdio>
#include <math.h>
#include "geometry.h"
#include "rect.h"
#include "msanim.h"
#include "surface.h"
#include "convert.h"
#include "ccfile.h"
#include "_surface.h"
#include "gscreen.h"
#include "wdtnet.h"

using namespace WorldDominationTour;

extern char DAT_00889F64[];

extern bool DAT_008487BC;
extern bool DAT_008487BD;
void __fastcall FUN_0072E450(Surface * surface, Rect * rect, int flag);

class MSFrameAnim : public MSAnim
{
	public:
		MSFrameAnim(void);
		virtual ~MSFrameAnim(void) override;

		virtual bool Advance(Surface * surface, Rect & rect) override;
		virtual void Redraw(Surface * surface, Rect const * rect=NULL) override;
		virtual Rect Get_Rect(void) const override;
};

// SYNTHETIC: GAMEMD 0x00767C80 SYMBOL
// _$E3
// SYNTHETIC: GAMEMD 0x00767CA0 SYMBOL
// _$E6
// SYNTHETIC: GAMEMD 0x00767CD0 SYMBOL
// _$E9
// SYNTHETIC: GAMEMD 0x00767CF0 SYMBOL
// _$E12
// SYNTHETIC: GAMEMD 0x00767D10 SYMBOL
// _$E15
// SYNTHETIC: GAMEMD 0x00767D30 SYMBOL
// _$E18
// SYNTHETIC: GAMEMD 0x00767D50 SYMBOL
// _$E21
// SYNTHETIC: GAMEMD 0x00767D70 SYMBOL
// _$E24
// SYNTHETIC: GAMEMD 0x00767DB0 SYMBOL
// _$E27
// SYNTHETIC: GAMEMD 0x00767DE0 SYMBOL
// _$E30
// SYNTHETIC: GAMEMD 0x00767E10 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x00767E40 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x00767E50 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x00767E70 SYMBOL
// _$E42

// FUNCTION: GAMEMD 0x00767E90
MSFrameAnim::MSFrameAnim(void) :
	MSAnim(0, 0, false)
{
}

// FUNCTION: GAMEMD 0x00767EC0
MSFrameAnim::~MSFrameAnim(void)
{
}

// FUNCTION: GAMEMD 0x00767ED0
bool MSFrameAnim::Advance(Surface * surface, Rect & rect)
{
	if (DAT_008487BC) {
		FUN_0072E450(surface, &rect, 1);
	}
	return false;
}

// FUNCTION: GAMEMD 0x00767EF0
void MSFrameAnim::Redraw(Surface * surface, Rect const * rect)
{
	if (DAT_008487BD) {
		int w = surface->Get_Width();
		int h = surface->Get_Height();
		if (rect) {
			Rect r = *rect;
			FUN_0072E450(surface, &r, 1);
		} else {
			Rect r(0, 0, w, h);
			FUN_0072E450(surface, &r, 1);
		}
	}
}

// FUNCTION: GAMEMD 0x00767F80
Rect MSFrameAnim::Get_Rect(void) const
{
	return Rect(0, 0, 640, 480);
}

// FUNCTION: GAMEMD 0x00767FB0
// ?FUN_00767FB0@WorldDominationTour@@YI?AV?$rc_ptr@VMap@WorldDominationTour@@@@HHH@Z
rc_ptr<Map> __fastcall WorldDominationTour::FUN_00767FB0(int map_id, int vq_anim, int faction)
{
	return rc_ptr<Map>(new Map(map_id, vq_anim, faction));
}

// FUNCTION: GAMEMD 0x00768000
// WorldDominationTour::Map::Map
Map::Map(int id, int vq_anim, int faction) :
	ID(id),
	VQAnim(vq_anim),
	Faction(faction),
	OverlayDrawer(NULL),
	IntroAnim(NULL),
	BackgroundAnim(NULL),
	HelpBarAnim(NULL),
	MaskSurface(NULL),
	Name(DAT_00889F64),
	Origin(0, 0),
	CancelAnim(NULL),
	CancelActive(false),
	CancelFrame(0),
	CancelHoverFrame(0),
	CancelHovered(false),
	CancelDrawer(NULL)
{
}

// FUNCTION: GAMEMD 0x007680C0
// WorldDominationTour::Map::~Map
Map::~Map(void)
{
	delete MaskSurface;
	delete OverlayDrawer;
	for (int i = 0; i < Palettes.Count(); i++) {
		delete Palettes[i];
	}
	AlternateSurface->Fill(0);
	HiddenSurface->Fill(0);
	VisibleSurface->Blit_From(*HiddenSurface);
	delete CancelDrawer;
}

// FUNCTION: GAMEMD 0x00768F50
// WorldDominationTour::Map::FUN_00768F50
MSShapeAnim * Map::FUN_00768F50(INIClass const & ini, Wstring const & section, Wstring const & entry, ConvertClass * fallback)
{
	Wstring buffer;
	bool found;
	{
		Wstring none((char const *)NULL);
		found = ini.Get_String(section, entry, none, buffer) > 0;
	}
	if (found) {
		Wstring name;
		Wstring xs;
		Wstring ys;
		Wstring rate_text;
		Wstring palette_name;
		int pos = buffer.getToken(0, ",", name);
		pos = buffer.getToken(pos, ",", xs);
		pos = buffer.getToken(pos, ",", ys);
		pos = buffer.getToken(pos, ",", rate_text);
		buffer.getToken(pos, ",", palette_name);
		if (ys != (char const *)NULL) {
			int x = xs.toInt() + Origin.X;
			int y = ys.toInt() + Origin.Y;
			int rate = 5;
			if (rate_text != (char const *)NULL && rate_text.length() > 0) {
				rate = rate_text.toInt();
			}
			ConvertClass * drawer = NULL;
			if (palette_name != (char const *)NULL && palette_name.length() > 0) {
				drawer = FUN_007691E0(palette_name);
			}
			if (drawer == NULL) {
				drawer = fallback;
			}
			bool available = CCFileClass(name).Is_Available(0);
			if (available && drawer) {
				return new MSShapeAnim(name, x, y, drawer, rate, true, SHAPE_NORMAL);
			}
		}
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x007691E0
// WorldDominationTour::Map::FUN_007691E0
ConvertClass * Map::FUN_007691E0(Wstring const & name)
{
	ConvertClass * drawer = NULL;
	for (int i = 0; i < Palettes.Count(); i++) {
		if (Palettes[i]->Name == name) {
			return Palettes[i]->Drawer;
		}
	}
	drawer = Create_Drawer(name);
	if (drawer) {
		Palettes.Add(new AnimationPalette(name, drawer));
	}
	return drawer;
}

// FUNCTION: GAMEMD 0x007692B0
// WorldDominationTour::Map::AnimationPalette::AnimationPalette
Map::AnimationPalette::AnimationPalette(Wstring const & name, ConvertClass * drawer) :
	Name(name),
	Drawer(drawer)
{
}

// FUNCTION: GAMEMD 0x007692D0
// WorldDominationTour::Map::AnimationPalette::~AnimationPalette
Map::AnimationPalette::~AnimationPalette(void)
{
	delete Drawer;
}

// FUNCTION: GAMEMD 0x007692F0
// WorldDominationTour::Map::FUN_007692F0
void Map::FUN_007692F0(Selection * selection, YRVoices * voices)
{
	if (IntroAnim) {
		voices->FUN_007714B0(0, true, 0);
		selection->Wait_For_Anim(IntroAnim);
	}
	if (BackgroundAnim) {
		BackgroundAnim->Set_Active(true);
	}
}

// FUNCTION: GAMEMD 0x00769330
// WorldDominationTour::Map::FUN_00769330
int Map::FUN_00769330(Point2D const & point)
{
	Point2D p = point - Origin;
	if (p.X >= 0 && p.X < MaskSurface->Get_Width() && p.Y >= 0 && p.Y < MaskSurface->Get_Height()) {
		return MaskSurface->Get_Pixel(p);
	}
	return -1;
}

// FUNCTION: GAMEMD 0x007693A0
// WorldDominationTour::Map::FUN_007693A0
void Map::FUN_007693A0(Selection * selection, bool hovered)
{
	if (CancelHovered != hovered) {
		CancelHovered = hovered;
		CancelAnim->Set_Frame((CancelHovered && CancelActive) ? CancelHoverFrame : CancelFrame);
		selection->Restore_Anims(CancelRect);
		selection->Restore_And_Advance();
	}
}

// FUNCTION: GAMEMD 0x007693F0
// WorldDominationTour::Map::FUN_007693F0
void Map::FUN_007693F0(Selection * selection, bool active)
{
	if (CancelActive != active) {
		CancelActive = active;
		CancelAnim->Set_Frame((CancelHovered && CancelActive) ? CancelHoverFrame : CancelFrame);
		selection->Restore_Anims(CancelRect);
		selection->Restore_And_Advance();
	}
}

// FUNCTION: GAMEMD 0x00769440
// WorldDominationTour::Map::FUN_00769440
bool Map::FUN_00769440(Point2D const & point)
{
	return (point.X >= CancelRect.X && point.X < CancelRect.X + CancelRect.Width && point.Y >= CancelRect.Y && point.Y < CancelRect.Y + CancelRect.Height);
}

// FUNCTION: GAMEMD 0x00769480
// WorldDominationTour::Centroid::Centroid
Centroid::Centroid(void) :
	Position(0, 0),
	Count(0)
{
}

// FUNCTION: GAMEMD 0x00769490
// WorldDominationTour::Centroid::operator+=
Centroid const & Centroid::operator+=(Point2D const & pt)
{
	Position.X += pt.X;
	Position.Y += pt.Y;
	Count++;
	return *this;
}

// FUNCTION: GAMEMD 0x007694C0
// WorldDominationTour::Centroid::Center_Point
Point2D Centroid::Center_Point(void) const
{
	return (Count > 0) ? Point2D(Position.X / Count, Position.Y / Count) : Point2D(0, 0);
}

// FUNCTION: GAMEMD 0x00769510
// WorldDominationTour::Centroid::operator==
bool Centroid::operator==(Centroid const & that) const
{
	return Position == that.Position && Count == that.Count;
}

// FUNCTION: GAMEMD 0x00769540
unsigned int __fastcall FUN_00769540(unsigned int value)
{
	if (value > 0x1d) {
		return 0xff;
	}
	return value;
}

#include "cdfile.h"
#include "pcx.h"
#include "wstring.h"

Wstring operator+(Wstring const & a, Wstring const & b);

struct Drawing {
	static int RenderWidth;
};

typedef VectorClass<Rect> RECT_LIST;
typedef VectorCursor<Rect, RECT_LIST> RECT_ITER;

inline RECT_ITER Rect_Cursor(RECT_LIST & list)
{
	return RECT_ITER(list);
}

inline void Rewind(RECT_ITER & it, RECT_LIST & list)
{
	it = Rect_Cursor(list);
}

// FUNCTION: GAMEMD 0x00769550
void __fastcall FUN_00769550(Wstring const & map_name, Wstring const & pcx_file_name)
{
	CDFileClass ifile;
	ifile.Open("WDTMapMD.ini", 1);
	INIClass ini;
	ini.Load(ifile);
	ifile.Close();
	CDFileClass pfile(pcx_file_name);
	Surface * surf = Read_PCX_File(pfile);

	int height = surf->Get_Height();
	int width = surf->Get_Width();

	RECT_LIST rects(30);
	CENTROID_LIST centroids(30);

	RECT_ITER riter = rects;
	for (; riter; ++riter) {
		Rect & rect = *riter;
		rect.X = 0;
		rect.Y = 0;
		rect.Width = 0;
		rect.Height = 0;
	}

	Point2D point;
	for (point.Y = 0; point.Y < height; point.Y++) {
		for (point.X = 0; point.X < width; point.X++) {
			unsigned n = FUN_00769540(surf->Get_Pixel(point));
			if (n >= 0xFF) {
				continue;
			}

			if ((unsigned)centroids.Length() < n + 1) {
				centroids.Resize(n + 1);
			}
			centroids[n] += point;

			if ((unsigned)rects.Length() < n + 1) {
				int length = rects.Length();
				rects.Resize(n + 1);
				RECT_ITER iter(rects, length);
				for (; iter; ++iter) {
					Rect & blank = *iter;
					blank.X = 0;
					blank.Y = 0;
					blank.Width = 0;
					blank.Height = 0;
				}
			}
			Rect & rect = rects[n];
			if (rect.Width == 0) {
				rect.Height = 0;
			}
			if (!rect.Is_Point_Within(point)) {
				rect = Union(rect, Rect(point.X, point.Y, 1, 1));
			}
		}
	}

	Rewind(riter, rects);

	int ci = 0;
	while (riter) {
		Wstring section = operator+(map_name, Wstring_FromInt(ci, 2));
		Wstring width_text = Wstring_FromInt(Drawing::RenderWidth);
		Point2D target = centroids[ci].Center_Point();
		Point2D center = target;
		ini.Put_Point(section, Wstring("Target_").operator+(width_text), center);
		++riter;
		ci++;
	}

	ifile.Open("WDTMapMD.ini", 2);
	ini.Save(ifile);
	ifile.Open("..\\code\\wdtmapMD.ini", 2);
	ini.Save(ifile);
}

// SYNTHETIC: GAMEMD 0x00769BD0
// ??_GMSFrameAnim@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00769C00
// ??_GMap@WorldDominationTour@@UAEPAXI@Z

template class VectorClass<Centroid>;
template class VectorClass<Rect>;
template class VectorCursor<Rect, VectorClass<Rect> >;
template class TRect<int>;

// TEMPLATE: GAMEMD 0x00769C20
// ??4?$VectorCursor@V?$TRect@H@@V?$VectorClass@V?$TRect@H@@@@@@QAEAAV0@ABV0@@Z

// TEMPLATE: GAMEMD 0x00769C40
// ??0?$TRect@H@@QAE@XZ

// TEMPLATE: GAMEMD 0x00769C50
// ?Clear@?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00769C80
// ?ID@?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEHPBV?$rc_ptr@VTerritory@WorldDominationTour@@@@@Z

// TEMPLATE: GAMEMD 0x00769CA0
// ?GetItem@?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UBE?AV?$rc_ptr@VTerritory@WorldDominationTour@@@@H@Z

// TEMPLATE: GAMEMD 0x00769CD0
// ?Clear@?$DynamicVectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00769D00
// ?ID@?$DynamicVectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEHPBQAUAnimationPalette@Map@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x00769D20
// ?GetItem@?$VectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UBEPAUAnimationPalette@Map@WorldDominationTour@@H@Z

// TEMPLATE: GAMEMD 0x00769D30
// ?GetItem@?$VectorClass@VCentroid@WorldDominationTour@@@@UBE?AVCentroid@WorldDominationTour@@H@Z

// TEMPLATE: GAMEMD 0x00769D60
// ??B?$VectorCursor@V?$TRect@H@@V?$VectorClass@V?$TRect@H@@@@@@UBE_NXZ

// FUNCTION: GAMEMD 0x00769D70
// Rect_Cursor

// SYNTHETIC: GAMEMD 0x00769D90
// ??_G?$rc_ptr@VMap@WorldDominationTour@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00769DC0
// ??_G?$VectorCursor@V?$TRect@H@@V?$VectorClass@V?$TRect@H@@@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00769DE0
// ??8?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x00769E30
// ?Resize@?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAE_NHPBV?$rc_ptr@VTerritory@WorldDominationTour@@@@@Z

// TEMPLATE: GAMEMD 0x00769F50
// ?Clear@?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00769F80
// ?ID@?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEHABV?$rc_ptr@VTerritory@WorldDominationTour@@@@@Z

// TEMPLATE: GAMEMD 0x00769FC0
// ?ID@?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEHPBV?$rc_ptr@VTerritory@WorldDominationTour@@@@@Z

// TEMPLATE: GAMEMD 0x00769FE0
// ?Resize@?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAE_NHPBV?$rc_ptr@VTerritory@WorldDominationTour@@@@@Z

// TEMPLATE: GAMEMD 0x0076A100
// ?ID@?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEHABV?$rc_ptr@VTerritory@WorldDominationTour@@@@@Z

// TEMPLATE: GAMEMD 0x0076A130
// ??8?$VectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0076A170
// ?Resize@?$VectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAE_NHPBQAUAnimationPalette@Map@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A220
// ?Clear@?$VectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0076A250
// ?ID@?$VectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEHABQAUAnimationPalette@Map@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A290
// ?ID@?$VectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEHPBQAUAnimationPalette@Map@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A2B0
// ?Resize@?$DynamicVectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAE_NHPBQAUAnimationPalette@Map@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A360
// ?ID@?$DynamicVectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEHABQAUAnimationPalette@Map@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A390
// ??8?$VectorClass@VCentroid@WorldDominationTour@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0076A3F0
// ?Resize@?$VectorClass@VCentroid@WorldDominationTour@@@@UAE_NHPBVCentroid@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A4F0
// ?Clear@?$VectorClass@VCentroid@WorldDominationTour@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0076A520
// ?ID@?$VectorClass@VCentroid@WorldDominationTour@@@@UAEHABVCentroid@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A570
// ?ID@?$VectorClass@VCentroid@WorldDominationTour@@@@UAEHPBVCentroid@WorldDominationTour@@@Z

// TEMPLATE: GAMEMD 0x0076A5A0
// ??E?$VectorCursor@V?$TRect@H@@V?$VectorClass@V?$TRect@H@@@@@@UAEAAV0@XZ

// TEMPLATE: GAMEMD 0x0076A5B0
// ??F?$VectorCursor@V?$TRect@H@@V?$VectorClass@V?$TRect@H@@@@@@UAEAAV0@XZ

// SYNTHETIC: GAMEMD 0x0076A5C0
// ??_G?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0076A610
// ??_E?$rc_ptr@VTerritory@WorldDominationTour@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0076A680
// ??_G?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0076A6D0
// ??_G?$VectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0076A720
// ??_G?$DynamicVectorClass@PAUAnimationPalette@Map@WorldDominationTour@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0076A770
// ??_G?$VectorClass@VCentroid@WorldDominationTour@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0076A7C0
// ??0?$rc_ptr@VTerritory@WorldDominationTour@@@@QAE@XZ

// TEMPLATE: GAMEMD 0x0076A7E0
// ??0?$VectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@QAE@HPBV?$rc_ptr@VTerritory@WorldDominationTour@@@@@Z
