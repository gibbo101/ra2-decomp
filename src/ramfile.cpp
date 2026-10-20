// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "ramfile.h"
#include "radsite.h"
#include "cell.h"
#include "crc.h"
#include "map.h"
#include "_rules.h"
#include "rules.h"
#include "scenario.h"
#include "swizzle.h"
#include "tracker.h"
#include <cstring>

namespace Unsorted { extern int CurrentFrame; }

DynamicVectorClass<RadSiteClass *> RadSites;

// FUNCTION: 0x0065B160 $E56
// FUNCTION: 0x0065B1A0 $E54

// FUNCTION: 0x0065B1E0
RadSiteClass::RadSiteClass(void) :
	BASECLASS(),
	LightSource(NULL),
	RadLevelTimer(),
	RadLightTimer(),
	BaseCell(0, 0),
	Spread(0),
	SpreadInLeptons(0),
	RadLevel(0),
	LevelSteps(0),
	Intensity(0),
	IntensitySteps(0),
	IntensityDecrement(0),
	RadDuration(0),
	RadTimeLeft(0)
{
	RadSites.Add(this);
}

// FUNCTION: 0x0065B2A0
RadSiteClass::RadSiteClass(NoInitClass const & x) :
	BASECLASS(x),
	RadLevelTimer(),
	RadLightTimer()
{
}

// FUNCTION: 0x0065B2F0
RadSiteClass::~RadSiteClass(void)
{
	CellClass & cell = Map[BaseCell];
	cell.FUN_00487C70(NULL);
	Detach_This_From_All(this, true);
	if (LightSource != NULL) {
		delete LightSource;
		LightSource = NULL;
	}
	RadSites.Delete(this);
}

// FUNCTION: 0x0065B3A0
int RadSiteClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x0065B3B0
void RadSiteClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: 0x0065B3C0
RTTIType RadSiteClass::Fetch_RTTI(void) const
{
	return(RTTI_RADSITE);
}

// FUNCTION: 0x0065B3D0
HRESULT STDMETHODCALLTYPE RadSiteClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) RadSiteClass(NoInitClass());
		Swizzle_Pointer(&LightSource);
	}
	return(result);
}

// FUNCTION: 0x0065B450
HRESULT STDMETHODCALLTYPE RadSiteClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0065B470
HRESULT STDMETHODCALLTYPE RadSiteClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}
	*retval = CLSID_RadSiteClass;
	return(S_OK);
}

// FUNCTION: 0x0065B4B0
int RadSiteClass::GetSpread(void)
{
	return(Spread);
}

// FUNCTION: 0x0065B4C0
void RadSiteClass::SetBaseCell(Cell * cell)
{
	BaseCell = *cell;
}

// FUNCTION: 0x0065B4D0
void RadSiteClass::SetSpread(int spread)
{
	Spread = spread;
	SpreadInLeptons = spread * 256 + 128;
}

// FUNCTION: 0x0065B4F0
void RadSiteClass::SetRadLevel(int level)
{
	RadLevel = level;
	RadDuration = Rule->RadDurationMultiple * level;
	RadTimeLeft = RadDuration;
}

// FUNCTION: 0x0065B510
int RadSiteClass::GetRadLevel(void)
{
	if (RadDuration <= 0) {
		return(0);
	}
	return(RadTimeLeft * RadLevel / RadDuration);
}

// FUNCTION: 0x0065B530
void RadSiteClass::Add(int level)
{
	double a = RadLevel;
	double b = RadTimeLeft;
	double c = RadDuration;
	int current = (int)(a * b / c);
	Deactivate();
	RadLevel = current + level;
	RadDuration = Rule->RadDurationMultiple * RadLevel;
	RadTimeLeft = RadDuration;
	Activate();
}

// FUNCTION: 0x0065B580
void RadSiteClass::Activate(void)
{
	int leveldelay = Rule->RadLevelDelay;
	int lightdelay = Rule->RadLightDelay;
	RadLevelTimer = leveldelay;
	RadLightTimer = lightdelay;

	int green = Rule->RadColor.Get_Green();
	int blue = Rule->RadColor.Get_Blue();
	int red = Rule->RadColor.Get_Red();
	if (Scen->Theater == THEATER_SNOW) {
		red = blue = -128;
	}
	int r = red * 1000 / 255;
	int g = green * 1000 / 255;
	int b = blue * 1000 / 255;
	int visibility = SpreadInLeptons;
	double light = RadLevel;
	light *= Rule->RadLightFactor;
	light = (2000.0 < light) ? 2000.0 : light;
	int intensity = (int)light;
	double tr = r * Rule->RadTintFactor;
	tr = (2000.0 < tr) ? 2000.0 : tr;
	int tintr = (int)tr;
	double tg = g * Rule->RadTintFactor;
	tg = (2000.0 < tg) ? 2000.0 : tg;
	int tintg = (int)tg;
	double tb = b * Rule->RadTintFactor;
	tb = (2000.0 < tb) ? 2000.0 : tb;
	int tintb = (int)tb;

	Tint.Red = tintr;
	Tint.Green = tintg;
	Tint.Blue = tintb;
	Intensity = intensity;
	LevelSteps = RadDuration / leveldelay;
	IntensitySteps = RadDuration / lightdelay;
	IntensityDecrement = intensity / IntensitySteps;

	Coord coord = Map[BaseCell].Center_Coord();
	if (LightSource == NULL) {
		LightSource = new LightSourceClass(coord, visibility, intensity, tintr, tintg, tintb);
		LightSource->DetailLevel = 0;
		LightSource->Enable(false);
	} else {
		LightSource->ChangeLevels(intensity, tintr, tintg, tintb, false);
	}
	Radiate();
}

// FUNCTION: 0x0065B800
void RadSiteClass::AI(void)
{
	RadTimeLeft--;
	if (RadLevelTimer.Value() == 0) {
		DecreaseRadiation();
		RadLevelTimer = Rule->RadLevelDelay;
	}
	if (RadLightTimer.Value() == 0) {
		DecreaseLight();
		RadLightTimer = Rule->RadLightDelay;
	}
	if (RadTimeLeft <= 0) {
		delete this;
	}
}

// FUNCTION: 0x0065B8F0
double RadSiteClass::GetRadLevelAt(Cell const & cell)
{
	CellClass & here = Map[BaseCell];
	CellClass & there = Map[cell];
	Coord a = here.Center_Coord();
	Coord b = there.Center_Coord();
	int distance = a.Distance_To(b);
	int range = SpreadInLeptons;
	double result = 0.0;
	if (distance <= range) {
		result = (double)(range - distance) / range * RadLevel;
	}
	return(result);
}

// FUNCTION: 0x0065B9C0
void RadSiteClass::Radiate(void)
{
	int x0 = BaseCell.X - Spread;
	int y0 = BaseCell.Y - Spread;
	int xend = x0 + Spread * 2 + 1;
	int yend = y0 + Spread * 2 + 1;
	for (int y = y0; y < yend; y++) {
		for (int x = x0; x < xend; x++) {
			Cell cell(x, y);
			CellClass & here = Map[BaseCell];
			CellClass & there = Map[cell];
			Coord a = here.Center_Coord();
			Coord b = there.Center_Coord();
			int distance = a.Distance_To(b);
			int range = SpreadInLeptons;
			double amount = 0.0;
			if (distance <= range) {
				amount = (double)(range - distance) / range * RadLevel;
			}
			CellClass & target = Map[cell];
			double level = amount;
			target.RadLevel_Increase(level);
		}
	}
}

// FUNCTION: 0x0065BB50
void RadSiteClass::Deactivate(void)
{
	int steps = RadTimeLeft / Rule->RadLevelDelay + 1;
	int x0 = BaseCell.X - Spread;
	int y0 = BaseCell.Y - Spread;
	int xend = x0 + Spread * 2 + 1;
	int yend = y0 + Spread * 2 + 1;
	for (int y = y0; y < yend; y++) {
		for (int x = x0; x < xend; x++) {
			Cell cell(x, y);
			CellClass & here = Map[BaseCell];
			CellClass & there = Map[cell];
			Coord a = here.Center_Coord();
			Coord b = there.Center_Coord();
			int distance = a.Distance_To(b);
			int range = SpreadInLeptons;
			double amount = 0.0;
			if (distance <= range) {
				amount = (double)(range - distance) / range * RadLevel;
			}
			double level = amount;
			level = level / LevelSteps * steps;
			CellClass & target = Map[cell];
			target.RadLevel_Decrease(level);
		}
	}
}

// FUNCTION: 0x0065BD00
void RadSiteClass::DecreaseRadiation(void)
{
	int x0 = BaseCell.X - Spread;
	int y0 = BaseCell.Y - Spread;
	int xend = x0 + Spread * 2 + 1;
	int yend = y0 + Spread * 2 + 1;
	for (int y = y0; y < yend; y++) {
		for (int x = x0; x < xend; x++) {
			Cell cell(x, y);
			CellClass & here = Map[BaseCell];
			CellClass & there = Map[cell];
			Coord a = here.Center_Coord();
			Coord b = there.Center_Coord();
			int distance = a.Distance_To(b);
			int range = SpreadInLeptons;
			double amount = 0.0;
			if (distance <= range) {
				amount = (double)(range - distance) / range * RadLevel;
			}
			double level = amount;
			level /= LevelSteps;
			CellClass & target = Map[cell];
			target.RadLevel_Decrease(level);
		}
	}
}

// FUNCTION: 0x0065BE90
void RadSiteClass::DecreaseLight(void)
{
	LightSource->ChangeLevels(LightSource->Intensity - IntensityDecrement, Tint.Red * RadTimeLeft / RadDuration, Tint.Green * RadTimeLeft / RadDuration, Tint.Blue * RadTimeLeft / RadDuration, false);
}

// FUNCTION: 0x0065BED0 RadSiteClass::~RadSiteClass

// FUNCTION: 0x0065BEF0 ?$DynamicVectorClass@PAVRadSiteClass@@::Clear
template class DynamicVectorClass<RadSiteClass *>;

// FUNCTION: 0x0065BF20 ?$DynamicVectorClass@PAVRadSiteClass@@::ID
// FUNCTION: 0x0065BF40 ?$VectorClass@PAVRadSiteClass@@::GetItem
// FUNCTION: 0x0065BF50 ?$VectorClass@PAVRadSiteClass@@::operator==
// FUNCTION: 0x0065BF90 ?$VectorClass@PAVRadSiteClass@@::Resize
// FUNCTION: 0x0065C040 ?$VectorClass@PAVRadSiteClass@@::Clear
// FUNCTION: 0x0065C070 ?$VectorClass@PAVRadSiteClass@@::ID
// FUNCTION: 0x0065C0B0 ?$VectorClass@PAVRadSiteClass@@::ID
// FUNCTION: 0x0065C0D0 ?$DynamicVectorClass@PAVRadSiteClass@@::Resize
// FUNCTION: 0x0065C180 ?$DynamicVectorClass@PAVRadSiteClass@@::ID
// FUNCTION: 0x0065C1B0 ?$VectorClass@PAVRadSiteClass@@::~?$VectorClass@PAVRadSiteClass@@
template class VectorClass<RadSiteClass *>;

// FUNCTION: 0x0065C340
int RAMFileClass::Open(char const *, int access)
{
	return(Open(access));
}
