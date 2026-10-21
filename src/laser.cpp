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

// TEMPLATE: GAMEMD 0x005512D0
// ?Clear@?$DynamicVectorClass@PAVLaserDrawClass@@@@UAEXXZ
template class DynamicVectorClass<LaserDrawClass *>;

// TEMPLATE: GAMEMD 0x00551300
// ?ID@?$VectorClass@PAVLaserDrawClass@@@@UAEHPBQAVLaserDrawClass@@@Z
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
// ?ID@?$DynamicVectorClass@PAVLaserDrawClass@@@@UAEHPBQAVLaserDrawClass@@@Z
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

// SYNTHETIC: GAMEMD 0x005517A0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005517D0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005517F0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00551810 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x00551830 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00551850 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00551870 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005518B0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005518E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00551910 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00551940 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00551950 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00551970 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00551990 SYMBOL
// _$E42
