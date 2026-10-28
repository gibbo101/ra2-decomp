// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lightcon.h"
#include "dsurface.h"
#include "globals.h"
#include "ion.h"
#include "scenario.h"
#include "geometry.h"
#include "sun.h"
#include "palette.h"

#include <math.h>


extern bool DAT_00829C20[256];
extern DSurfaceColorMode DAT_00829D20;

extern "C" {
	void __cdecl FUN_007DE53A(void * palette, void * translator, int red, int green, int blue, int intensity, bool * mask);
	void __cdecl FUN_007DE87A(void * palette, void * translator, int red, int green, int blue, int intensity, bool * mask);
	void __cdecl FUN_007DE200(void * palette, void * translator, int red, int green, int blue, int intensity, bool * mask);
	void __cdecl FUN_007DEBBA(void * palette, void * translator, int red, int green, int blue, int intensity, bool * mask);
}

struct NukeFlash {
	static bool IsFadingIn(void);
};

struct ChronoScreenEffect {
	static bool Active(void);
};

struct PsyDom {
	static bool Active(void);
};

// SYNTHETIC: GAMEMD 0x00555B90 SYMBOL
// _$E3
// SYNTHETIC: GAMEMD 0x00555BB0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00555BE0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00555C00 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00555C20 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x00555C40 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x00555C60 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x00555C80 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x00555CC0 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x00555CF0 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x00555D20 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x00555D50 SYMBOL
// _$E62
// SYNTHETIC: GAMEMD 0x00555D60 SYMBOL
// _$E65
// SYNTHETIC: GAMEMD 0x00555D80 SYMBOL
// _$E68

// FUNCTION: GAMEMD 0x00555DA0
LightConvertClass::LightConvertClass(PaletteClass const & artpalette, PaletteClass const & screenpalette, Surface const & typicalsurface, int red_tint, int green_tint, int blue_tint, bool blitters_created, bool * tint_mask, int intensity_levels) :
	BASECLASS(artpalette, screenpalette, typicalsurface, intensity_levels, true),
	ArtPalette(&artpalette),
	ScreenPalette(&screenpalette),
	TintMask(tint_mask),
	ReferenceCount(0),
	NormalRedTint(red_tint),
	NormalGreenTint(green_tint),
	NormalBlueTint(blue_tint),
	IonRedTint(red_tint),
	IonGreenTint(green_tint),
	IonBlueTint(blue_tint),
	UseIonLighting(false)
{
	if (IntensityLevels >= 1) {
		if (tint_mask == NULL) {
			TintMask = DAT_00829C20;
		}

		if (DAT_00829D20 == COLORMODE_INVALID) {
			DAT_00829D20 = (DSurfaceColorMode)DSurface::Get_Primary_Color_Mode();
		}

		if (IonStormClass::Is_Ion_Storm_Active()) {
			if (red_tint == -1) {
				NormalRedTint = NORMAL_LIGHT;
				NormalGreenTint = NORMAL_LIGHT;
				NormalBlueTint = NORMAL_LIGHT;
			} else {
				NormalRedTint = red_tint;
				NormalGreenTint = green_tint;
				NormalBlueTint = blue_tint;
			}
			Apply_Tint(NORMAL_LIGHT * Scen->IonLighting.RedTint / 100, NORMAL_LIGHT * Scen->IonLighting.GreenTint / 100, NORMAL_LIGHT * Scen->IonLighting.BlueTint / 100, true);
		} else if (NukeFlash::IsFadingIn() || ChronoScreenEffect::Active()) {
			if (red_tint == -1) {
				NormalRedTint = NORMAL_LIGHT;
				NormalGreenTint = NORMAL_LIGHT;
				NormalBlueTint = NORMAL_LIGHT;
			} else {
				NormalRedTint = red_tint;
				NormalGreenTint = green_tint;
				NormalBlueTint = blue_tint;
			}
			Apply_Tint(NORMAL_LIGHT * Scen->NukeLighting.RedTint / 100, NORMAL_LIGHT * Scen->NukeLighting.GreenTint / 100, NORMAL_LIGHT * Scen->NukeLighting.BlueTint / 100, true);
		} else if (PsyDom::Active()) {
			if (red_tint == -1) {
				NormalRedTint = NORMAL_LIGHT;
				NormalGreenTint = NORMAL_LIGHT;
				NormalBlueTint = NORMAL_LIGHT;
			} else {
				NormalRedTint = red_tint;
				NormalGreenTint = green_tint;
				NormalBlueTint = blue_tint;
			}
			Apply_Tint(NORMAL_LIGHT * Scen->DominatorLighting.RedTint / 100, NORMAL_LIGHT * Scen->DominatorLighting.GreenTint / 100, NORMAL_LIGHT * Scen->DominatorLighting.BlueTint / 100, true);
		} else {
			Apply_Tint(red_tint, green_tint, blue_tint, false);
		}

		if (!blitters_created) {
			Create_Blitters();
		}
	}
}

// FUNCTION: GAMEMD 0x00556090
void LightConvertClass::Apply_Tint(int red_tint, int green_tint, int blue_tint, bool ion_light)
{
	if (IntensityLevels >= 1) {
		if (TintMask == NULL) {
			TintMask = DAT_00829C20;
		}

		if (red_tint != -1) {
			red_tint = MIN(red_tint, 2000);
			red_tint = MAX(red_tint, 0);
			green_tint = MIN(green_tint, 2000);
			green_tint = MAX(green_tint, 0);
			blue_tint = MIN(blue_tint, 2000);
			blue_tint = MAX(blue_tint, 0);
		}

		if (ion_light) {
			if (red_tint == -1) {
				red_tint = IonRedTint;
				blue_tint = IonBlueTint;
				green_tint = IonGreenTint;
			} else {
				IonRedTint = red_tint;
				IonGreenTint = green_tint;
				IonBlueTint = blue_tint;
			}
			UseIonLighting = true;
		} else {
			if (red_tint == -1) {
				red_tint = NormalRedTint;
				green_tint = NormalGreenTint;
				blue_tint = NormalBlueTint;
			} else {
				NormalRedTint = red_tint;
				NormalGreenTint = green_tint;
				NormalBlueTint = blue_tint;
			}
			UseIonLighting = false;
		}

		int red_step = (int)((double)(NORMAL_LIGHT * red_tint) * (65536.0 / (double)(NORMAL_LIGHT * NORMAL_LIGHT)));
		int green_step = (int)((double)(NORMAL_LIGHT * green_tint) * (65536.0 / (double)(NORMAL_LIGHT * NORMAL_LIGHT)));
		int blue_step = (int)((double)(NORMAL_LIGHT * blue_tint) * (65536.0 / (double)(NORMAL_LIGHT * NORMAL_LIGHT)));

		unsigned short * translator = (unsigned short *)IntensityTranslator;

		int level_count = IntensityLevels - 1;

		int max_level = MAX(30 * IntensityLevels / 200 - 1, 0);
		max_level = MIN(max_level, level_count >> 1);

		int level = 0;

		if (level_count >= 0) {

			for (; level <= IntensityLevels - 1; level++) {

				int interp_intensity;
				int interp_red;
				int interp_green;
				int interp_blue;

				if (IntensityLevels > 1) {
					interp_red = (2 * level * red_step) / level_count;
					interp_green = (2 * level * green_step) / level_count;
					interp_blue = (2 * level * blue_step) / level_count;
					if (level > max_level) {
						interp_intensity = 0x10000;
					} else {
						interp_intensity = (level * 0x10000) / max_level;
					}
				} else {
					interp_red = red_step;
					interp_green = green_step;
					interp_blue = blue_step;
					interp_intensity = 0x10000;
				}

				switch (DAT_00829D20) {
					case COLORMODE_555:
						FUN_007DE53A((void *)ArtPalette, translator, interp_red, interp_green, interp_blue, interp_intensity, TintMask);
						translator += PaletteClass::COLOR_COUNT;
						break;

					case COLORMODE_556:
						FUN_007DE87A((void *)ArtPalette, translator, interp_red, interp_green, interp_blue, interp_intensity, TintMask);
						translator += PaletteClass::COLOR_COUNT;
						break;

					case COLORMODE_565:
						FUN_007DE200((void *)ArtPalette, translator, interp_red, interp_green, interp_blue, interp_intensity, TintMask);
						translator += PaletteClass::COLOR_COUNT;
						break;

					case COLORMODE_655:
						FUN_007DEBBA((void *)ArtPalette, translator, interp_red, interp_green, interp_blue, interp_intensity, TintMask);
						translator += PaletteClass::COLOR_COUNT;
						break;

					default:
						*translator = 0;
						translator++;
						for (int i = 1; i < PaletteClass::COLOR_COUNT; i++) {
							if (TintMask[i]) {
								red_tint   = (interp_red   * (unsigned char)(*ArtPalette)[i].Get_Red()) >> 16;
								green_tint = (interp_green * (unsigned char)(*ArtPalette)[i].Get_Green()) >> 16;
								blue_tint  = (interp_blue   * (unsigned char)(*ArtPalette)[i].Get_Blue()) >> 16;
							} else {
								red_tint   = (interp_intensity * (unsigned char)(*ArtPalette)[i].Get_Red()) >> 16;
								green_tint = (interp_intensity * (unsigned char)(*ArtPalette)[i].Get_Green()) >> 16;
								blue_tint  = (interp_intensity * (unsigned char)(*ArtPalette)[i].Get_Blue()) >> 16;
							}
							red_tint = MIN(red_tint, 255);
							green_tint = MIN(green_tint, 255);
							blue_tint = MIN(blue_tint, 255);
							*translator = DSurface::Build_Hicolor_Pixel(red_tint, green_tint, blue_tint);
							translator++;
						}
						break;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00556510
// FUNCTION: GAMEMD 0x00556520
LightConvertClass::~LightConvertClass(void)
{
}
