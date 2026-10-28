// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "laser.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "ccrand.h"
#include "coord.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "inline.h"
#include "mouse.h"
#include "rgb.h"
#include "scenario.h"
#include "tactical.h"
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>

// SYNTHETIC: GAMEMD 0x0054FBB0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0054FBE0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0054FC00 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0054FC20 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0054FC40 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0054FC60 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0054FC80 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0054FCC0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0054FCF0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0054FD20 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0054FD50 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0054FD60 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0054FD80 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0054FDA0 SYMBOL
// _$E42

DynamicVectorClass<LaserDrawClass *> LaserDrawClass::LaserDraws;

// SYNTHETIC: GAMEMD 0x0054FDC0 SYMBOL
// _$E117
// SYNTHETIC: GAMEMD 0x0054FE00 SYMBOL
// _$E114

// FUNCTION: GAMEMD 0x0054FE60
LaserDrawClass::LaserDrawClass(Coord start, Coord end, int zadjust, bool unknown, RGBClass inner_color, RGBClass outer_color, RGBClass outer_spread, int duration, bool blinks, bool fades, float start_intensity, float end_intensity) :
	StageClass(),
	Thickness(1),
	IsHouseColor(false),
	IsSupported(false),
	Start(start),
	End(end),
	ZAdjust(zadjust),
	UnusedBool1(unknown),
	InnerColor(),
	OuterColor(),
	OuterSpread(),
	Duration(duration),
	Blinks(blinks),
	BlinkState(false),
	Fades(fades),
	StartIntensity(start_intensity),
	EndIntensity(end_intensity)
{
	Set_Stage(0);
	Set_Rate(1);

	InnerColor = inner_color;
	OuterColor = outer_color;
	OuterSpread = outer_spread;

	LaserDraws.Add(this);
}

// FUNCTION: GAMEMD 0x0054FFB0
LaserDrawClass::~LaserDrawClass(void)
{
	LaserDraws.Delete(this);
}

// FUNCTION: GAMEMD 0x00550000
void LaserDrawClass::All_Clear(void)
{
	while (LaserDraws.Count()) {
		delete LaserDraws[0];
	}
}

// FUNCTION: GAMEMD 0x00550080
void LaserDrawClass::AI(void)
{
	Graphic_Logic();
	if (Blinks) {
		BlinkState = BlinkState == false;
	}

	if (Fetch_Stage() >= Duration) {
		delete this;
	}
}

// FUNCTION: GAMEMD 0x00550150
void LaserDrawClass::Update_All(void)
{
	for (int i = LaserDraws.Count() - 1; i >= 0; i--) {
		LaserDraws[i]->AI();
	}
}

// FUNCTION: GAMEMD 0x00550240
void LaserDrawClass::Draw_All(void)
{
	for (int i = LaserDraws.Count() - 1; i >= 0; i--) {
		LaserDraws[i]->Draw_It();
	}
}

// FUNCTION: GAMEMD 0x00550260
void LaserDrawClass::Draw_It(void)
{
	if (Duration > 0) {
		if (IsHouseColor) {
			FUN_005509F0();
			return;
		}

		static Point2D _glow_offsets[16] = {
			Point2D(0, -1),
			Point2D(1, 0),
			Point2D(0, -1),
			Point2D(0, 1),
			Point2D(1, 0),
			Point2D(0, 1),
			Point2D(-1, 0),
			Point2D(1, 0),
			Point2D(-1, 0),
			Point2D(0, 1),
			Point2D(0, -1),
			Point2D(1, 0),
			Point2D(0, -1),
			Point2D(-1, 0),
			Point2D(-1, 0),
			Point2D(1, 0)
		};

		if (!BlinkState) {
			DirType dir = Direction(Start, End);
			FacingType facing = dir.As_Facing();

			Point2D start_pixel;
			TacticalMap->Coord_To_Pixel(Start, start_pixel);
			Point2D end_pixel;
			TacticalMap->Coord_To_Pixel(End, end_pixel);

			int start_z = ZAdjust - TacticalMap->Z_Lepton_To_Pixel(Start.Z) - 2;
			int end_z = -TacticalMap->Z_Lepton_To_Pixel(End.Z) - 2;

			RGBClass outer_color;
			int outer_hicolor = 0;
			bool has_outer_glow = true;

			if (OuterColor.Get_Red() == 0 && OuterColor.Get_Green() == 0 && OuterColor.Get_Blue() == 0) {
				has_outer_glow = false;
			} else {
				int red = Sim_Random_Pick(-OuterSpread.Get_Red(), OuterSpread.Get_Red());
				int green = Sim_Random_Pick(-OuterSpread.Get_Green(), OuterSpread.Get_Green());
				int blue = Sim_Random_Pick(-OuterSpread.Get_Blue(), OuterSpread.Get_Blue());

				red += OuterColor.Get_Red();
				red = MAX(0, red);
				red = MIN(255, red);

				green += OuterColor.Get_Green();
				green = MAX(0, green);
				green = MIN(255, green);

				blue += OuterColor.Get_Blue();
				blue = MAX(0, blue);
				blue = MIN(255, blue);

				outer_hicolor = DSurface::Build_Hicolor_Pixel(red, green, blue);
				outer_color.Set_Red(red);
				outer_color.Set_Green(green);
				outer_color.Set_Blue(blue);
			}

			float current_intensity = 1.0;

			bool has_r = InnerColor.Get_Red() > 0;
			bool has_g = InnerColor.Get_Green() > 0;
			bool has_b = InnerColor.Get_Blue() > 0;

			if (Fades) {
				current_intensity = (StartIntensity - EndIntensity) * (Duration - Fetch_Stage()) / Duration + EndIntensity;
			}

			if (Options.DetailLevel != 0) {
				if (Blinks) {
					DSurface::Temp->Draw_Depth_Antialiased_Line(DSurface::ViewBounds, start_pixel, end_pixel, InnerColor, start_z, end_z, false, true, true, true, current_intensity);
					if (has_outer_glow) {
						DSurface::Temp->Draw_Depth_Antialiased_Line(DSurface::ViewBounds, _glow_offsets[2 * facing] + start_pixel, _glow_offsets[2 * facing] + end_pixel, outer_color, start_z, end_z, false, true, true, true, 1.0f);
						DSurface::Temp->Draw_Depth_Antialiased_Line(DSurface::ViewBounds, start_pixel + _glow_offsets[2 * facing + 1], _glow_offsets[2 * facing + 1] + end_pixel, outer_color, start_z, end_z, false, true, true, true, 1.0f);
					}
				} else if (has_outer_glow) {
					DSurface::Temp->DrawRGBMultiplyingLine(DSurface::ViewBounds, _glow_offsets[2 * facing] + start_pixel, _glow_offsets[2 * facing] + end_pixel, outer_color, current_intensity, start_z, end_z);
					DSurface::Temp->DrawRGBMultiplyingLine(DSurface::ViewBounds, _glow_offsets[2 * facing + 1] + start_pixel, _glow_offsets[2 * facing + 1] + end_pixel, outer_color, current_intensity, start_z, end_z);
					DSurface::Temp->DrawRGBMultiplyingLine(DSurface::ViewBounds, start_pixel, end_pixel, InnerColor, current_intensity, start_z, end_z);
				} else {
					DSurface::Temp->Draw_Depth_Antialiased_Line(DSurface::ViewBounds, start_pixel, end_pixel, InnerColor, start_z, end_z, false, has_r, has_g, has_b, current_intensity);
				}
			} else {
				DSurface::Temp->Draw_Depth_Shaded_Line(DSurface::ViewBounds, start_pixel, end_pixel,
					DSurface::Build_Hicolor_Pixel(InnerColor.Get_Red(), InnerColor.Get_Green(), InnerColor.Get_Blue()),
					start_z, end_z, false);

				if (has_outer_glow) {
					DSurface::Temp->Draw_Depth_Shaded_Line(DSurface::ViewBounds, _glow_offsets[2 * facing] + start_pixel, _glow_offsets[2 * facing] + end_pixel, outer_hicolor, start_z, end_z);
					DSurface::Temp->Draw_Depth_Shaded_Line(DSurface::ViewBounds, start_pixel + _glow_offsets[2 * facing + 1], _glow_offsets[2 * facing + 1] + end_pixel, outer_hicolor, start_z, end_z);
				}
			}
		}
	}
}

// SYNTHETIC: GAMEMD 0x005509E0 SYMBOL
// _$E120

// TEMPLATE: GAMEMD 0x005512D0
// ?Clear@?$DynamicVectorClass@PAVLaserDrawClass@@@@UAEXXZ
template class DynamicVectorClass<LaserDrawClass *>;

// TEMPLATE: GAMEMD 0x00551300
// ?ID@?$DynamicVectorClass@PAVLaserDrawClass@@@@UAEHPBQAVLaserDrawClass@@@Z
// TEMPLATE: GAMEMD 0x00551320
// ?GetItem@?$VectorClass@PAVLaserDrawClass@@@@UBEPAVLaserDrawClass@@H@Z
// TEMPLATE: GAMEMD 0x00551330
// ??8?$VectorClass@PAVLaserDrawClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00551370
// ?Resize@?$VectorClass@PAVLaserDrawClass@@@@UAE_NHPBQAVLaserDrawClass@@@Z

// TEMPLATE: GAMEMD 0x00551420
// ?Clear@?$VectorClass@PAVLaserDrawClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00551450
// ?ID@?$VectorClass@PAVLaserDrawClass@@@@UAEHABQAVLaserDrawClass@@@Z
// TEMPLATE: GAMEMD 0x00551490
// ?ID@?$VectorClass@PAVLaserDrawClass@@@@UAEHPBQAVLaserDrawClass@@@Z
// TEMPLATE: GAMEMD 0x005514B0
// ?Resize@?$DynamicVectorClass@PAVLaserDrawClass@@@@UAE_NHPBQAVLaserDrawClass@@@Z
// TEMPLATE: GAMEMD 0x00551560
// ?ID@?$DynamicVectorClass@PAVLaserDrawClass@@@@UAEHABQAVLaserDrawClass@@@Z
// SYNTHETIC: GAMEMD 0x00551590
// ??_G?$VectorClass@PAVLaserDrawClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005515E0
// ??_G?$DynamicVectorClass@PAVLaserDrawClass@@@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00551630
bool ViewHTML(char const * name)
{
	char windir[MAX_PATH];
	char tempfile[MAX_PATH];
	char exename[MAX_PATH];
	char command[MAX_PATH];
	STARTUPINFO startup;
	PROCESS_INFORMATION process;
	DWORD written;

	if (name && strlen(name)) {
		GetWindowsDirectory(windir, MAX_PATH);
		GetTempFileName(windir, "WWS", 0, tempfile);
		strcpy(strrchr(tempfile, '.'), ".html");

		HANDLE file = CreateFile(tempfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file != INVALID_HANDLE_VALUE) {
			WriteFile(file, "<title>ViewHTML</title>", strlen("<title>ViewHTML</title>"), &written, NULL);
			CloseHandle(file);

			int result = (int)FindExecutable(tempfile, NULL, exename);
			DeleteFile(tempfile);
			if (result > 32) {
				sprintf(command, "[open] %s", name);
				memset(&startup, 0, sizeof(startup));
				startup.cb = sizeof(startup);
				return CreateProcess(exename, command, NULL, NULL, FALSE, 0, NULL, NULL, &startup, &process) == TRUE;
			}
		}
	}
	return false;
}
