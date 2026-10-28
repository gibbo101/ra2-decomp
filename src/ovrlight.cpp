// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "ovrlight.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "bsurface.h"
#include "dsurface.h"
#include "getcpu.h"
#include "globals.h"
#include "rules.h"
#include "scenario.h"
#include "surface.h"
#include "tactical.h"

// SYNTHETIC: GAMEMD 0x005FEFC0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005FEFF0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005FF010 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005FF030 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005FF050 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005FF070 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005FF090 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005FF0D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005FF100 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005FF130 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005FF160 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005FF170 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005FF190 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005FF1B0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005FF1D0 SYMBOL
// _$E120
DynamicVectorClass<SpotLightClass *> SpotLights;

// SYNTHETIC: GAMEMD 0x005FF210 SYMBOL
// _$E118

int *SpotLightMMXBuffer;

BSurface *SpotLightSurfaces[SpotLightClass::SPOTLIGHT_SURFACE_COUNT + SpotLightClass::SPOTLIGHT_EXTRA_SURFACE_COUNT];

int SpotLightColorMode = -1;

namespace FPSCounter { extern unsigned CurrentFrameRate; }
namespace Detail { unsigned GetMinFrameRate(void); }

extern "C" {
	void __cdecl FUN_007DF3C2(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int dst_width, int dst_height, int *mmx_buffer);
	void __cdecl FUN_007DF500(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int dst_width, int dst_height, int *mmx_buffer);
	void __cdecl FUN_007DF280(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int dst_width, int dst_height, int *mmx_buffer);
	void __cdecl FUN_007DF640(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int dst_width, int dst_height, int *mmx_buffer);
	void __cdecl FUN_007DF191(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int width, int height);
	void __cdecl FUN_007DF0B4(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int width, int height);
	void __cdecl FUN_007DEEFA(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int width, int height);
	void __cdecl FUN_007DEFD7(unsigned char *mul_buffer, unsigned short *color_buffer, int mulbuff_width, int color_buff_width, int width, int height);
}

// FUNCTION: GAMEMD 0x005FF250
SpotLightClass::SpotLightClass(Coord pos, int size) :
	Radius(0),
	Position(pos),
	Size(size),
	DisableFlags(0)
{
	SpotLights.Add(this);
}

// FUNCTION: GAMEMD 0x005FF2D0
SpotLightClass::~SpotLightClass(void)
{
	SpotLights.Delete(this);
}

// FUNCTION: GAMEMD 0x005FF320
void SpotLightClass::AI(void)
{
	Radius += SPOTLIGHT_RADIUS_STEP;
	if (Radius >= SPOTLIGHT_MAX_RADIUS) {
		delete this;
	}
}

// FUNCTION: GAMEMD 0x005FF390
void SpotLightClass::Update_All(void)
{
	for (int i = SpotLights.Count() - 1; i >= 0; i--) {
		SpotLights[i]->AI();
	}
}

// FUNCTION: GAMEMD 0x005FF800
void SpotLightClass::Clear_All(void)
{
	if (SpotLightMMXBuffer != NULL) {
		delete [] SpotLightMMXBuffer;
		SpotLightMMXBuffer = NULL;
	}
	for (int i = 0; i < SPOTLIGHT_SURFACE_COUNT + SPOTLIGHT_EXTRA_SURFACE_COUNT; i++) {
		delete SpotLightSurfaces[i];
		SpotLightSurfaces[i] = NULL;
	}
}

// FUNCTION: GAMEMD 0x005FF850
void SpotLightClass::Draw_It(void)
{
	static char _index_table[SPOTLIGHT_MAX_RADIUS + 10] = {
		5, 10, 15, 20, 25, 30, 35, 40,
		45, 50, 55, 60, 61, 62, 63, 63,
		63, 62, 61, 60, 59, 58, 57, 56,
		55, 54, 53, 52, 51, 50, 49, 48,
		47, 46, 45, 44, 43, 42, 41, 40,
		39, 38, 37, 36, 35, 34, 33, 32,
		31, 30, 29, 28, 27, 26, 25, 24,
		23, 22, 21, 20, 19, 18, 17, 16,
		15, 14, 13, 12, 11, 10, 9, 8,
		7, 6, 5, 4, 3, 2, 1, 0,
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73,
	};

	unsigned int flags = DisableFlags & 0xF;
	if ((flags & ~1) == 0) {
		if (FPSCounter::CurrentFrameRate < Detail::GetMinFrameRate()) {
			return;
		}
	}

	Point2D drawpoint;
	if (TacticalMap->Coord_To_Pixel(Position, drawpoint)) {
		if (!Scen->Special.IsFogOfWar || !Map.Is_Fogged(Position)) {
			Surface *dsurf = DSurface::Temp;
			Rect drect(drawpoint.X - 128, drawpoint.Y - 64, 255, 127);
			Rect dcliprect = DSurface::ViewBounds;
			int index = _index_table[Radius];
			if (index < SpotLightClass::SPOTLIGHT_SURFACE_COUNT) {
				index = index * Size / SpotLightClass::SPOTLIGHT_SURFACE_COUNT;
			}
			Surface * ssurf = SpotLightSurfaces[index];
			int stride = dsurf->Stride();
			bool overlapped = false;
			Rect srect(0, 0, 255, 127);
			Rect scliprect(0, 0, 255, 127);
			void * dbuffer;
			void * sbuffer;
			if (XSurface::Prep_For_Blit(*dsurf, dcliprect, drect, *ssurf, scliprect, srect, overlapped, dbuffer, sbuffer)) {

				unsigned short *dptr_row = (unsigned short *)dbuffer;
				unsigned char *sptr_row = (unsigned char *)sbuffer;
				unsigned short *dptr = dptr_row;
				unsigned char *sptr = sptr_row;
				unsigned char spixel;
				RGBClass rgb;

				int colormode = SpotLightColorMode;
				if (flags == 0 && (unsigned)colormode <= COLORMODE_655) {
					switch (colormode) {

						case COLORMODE_555:
							if (SpotLightMMXBuffer) {
								FUN_007DF3C2(sptr, dptr, 256, stride, srect.Width, srect.Height, SpotLightMMXBuffer);
							} else {
								FUN_007DF191(sptr, dptr, 256, stride, srect.Width, srect.Height);
							}
							break;

						case COLORMODE_556:
							if (SpotLightMMXBuffer) {
								FUN_007DF500(sptr, dptr, 256, stride, srect.Width, srect.Height, SpotLightMMXBuffer);
							} else {
								FUN_007DF0B4(sptr, dptr, 256, stride, srect.Width, srect.Height);
							}
							break;

						case COLORMODE_565:
							if (SpotLightMMXBuffer) {
								FUN_007DF280(sptr, dptr, 256, stride, srect.Width, srect.Height, SpotLightMMXBuffer);
							} else {
								FUN_007DEEFA(sptr, dptr, 256, stride, srect.Width, srect.Height);
							}
							break;

						case COLORMODE_655:
							if (SpotLightMMXBuffer) {
								FUN_007DF640(sptr, dptr, 256, stride, srect.Width, srect.Height, SpotLightMMXBuffer);
							} else {
								FUN_007DEFD7(sptr, dptr, 256, stride, srect.Width, srect.Height);
							}
							break;

						default:
							goto fallback;
					}
				} else {
					fallback:
					if (DisableFlags & 1) {
						for (int y = 0; y < srect.Height; y++) {
							for (int x = 0; x < srect.Width; x++) {
								spixel = *sptr++;
								if (spixel != 0) {
									int scale = 256 - spixel;
									rgb = DSurface::Deconstruct_Hicolor_Pixel(*dptr);
									*dptr = DSurface::Build_Hicolor_Pixel((rgb.Get_Red() * scale) >> 8, (rgb.Get_Green() * scale) >> 8, (rgb.Get_Blue() * scale) >> 8);
								}
								dptr++;
							}
							dptr_row = (unsigned short *)((unsigned char *)dptr_row + stride);
							dptr = dptr_row;
							sptr_row += 256;
							sptr = sptr_row;
						}
					} else if (DisableFlags & 0xE) {
						for (int x = 0; x < srect.Height; x++) {
							for (int y = 0; y < srect.Width; y++) {
								spixel = *sptr++;
								if (spixel != 0) {
									rgb = DSurface::Deconstruct_Hicolor_Pixel(*dptr);
									int scale = spixel + 256;
									int rr = rgb.Get_Red();
									int gg = rgb.Get_Green();
									int bb = rgb.Get_Blue();
									if (!(DisableFlags & 2)) {
										rr = (rr * scale) >> 8;
										if (rr > 255) rr = 255;
									}
									if (!(DisableFlags & 4)) {
										gg = (gg * scale) >> 8;
										if (gg > 255) gg = 255;
									}
									if (!(DisableFlags & 8)) {
										bb = (bb * scale) >> 8;
										if (bb > 255) bb = 255;
									}
									*dptr = DSurface::Build_Hicolor_Pixel(rr, gg, bb);
								}
								dptr++;
							}
							dptr_row = (unsigned short *)((unsigned char *)dptr_row + stride);
							dptr = dptr_row;
							sptr_row += 256;
							sptr = sptr_row;
						}
					} else {
						for (int x = 0; x < srect.Height; x++) {
							for (int y = 0; y < srect.Width; y++) {
								spixel = *sptr++;
								if (spixel != 0) {
									int scale = spixel + 256;
									rgb = DSurface::Deconstruct_Hicolor_Pixel(*dptr);
									int rr = (rgb.Get_Red() * scale) >> 8;
									int gg = (rgb.Get_Green() * scale) >> 8;
									int bb = (rgb.Get_Blue() * scale) >> 8;
									*dptr = DSurface::Build_Hicolor_Pixel(MIN(255, rr), MIN(255, gg), MIN(255, bb));
								}
								dptr++;
							}
							dptr_row = (unsigned short *)((unsigned char *)dptr_row + stride);
							dptr = dptr_row;
							sptr_row += 256;
							sptr = sptr_row;
						}
					}
				}
				dsurf->Unlock();
				ssurf->Unlock();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005FFFA0
void SpotLightClass::Draw_All(void)
{
	for (int i = SpotLights.Count() - 1; i >= 0; i--) {
		SpotLights[i]->Draw_It();
	}
}

// TEMPLATE: GAMEMD 0x005FFFC0
// ?Clear@?$DynamicVectorClass@PAVSpotLightClass@@@@UAEXXZ
template class DynamicVectorClass<SpotLightClass *>;

// TEMPLATE: GAMEMD 0x005FFFF0
// ?ID@?$DynamicVectorClass@PAVSpotLightClass@@@@UAEHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600010
// ?GetItem@?$VectorClass@PAVSpotLightClass@@@@UBEPAVSpotLightClass@@H@Z

// TEMPLATE: GAMEMD 0x00600020
// ??8?$VectorClass@PAVSpotLightClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x00600060
// ?Resize@?$VectorClass@PAVSpotLightClass@@@@UAE_NHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600110
// ?Clear@?$VectorClass@PAVSpotLightClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00600140
// ?ID@?$VectorClass@PAVSpotLightClass@@@@UAEHABQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600180
// ?ID@?$VectorClass@PAVSpotLightClass@@@@UAEHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x006001A0
// ?Resize@?$DynamicVectorClass@PAVSpotLightClass@@@@UAE_NHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600250
// ?ID@?$DynamicVectorClass@PAVSpotLightClass@@@@UAEHABQAVSpotLightClass@@@Z

// SYNTHETIC: GAMEMD 0x00600280
// ??_G?$VectorClass@PAVSpotLightClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006002D0
// ??_G?$DynamicVectorClass@PAVSpotLightClass@@@@UAEPAXI@Z
