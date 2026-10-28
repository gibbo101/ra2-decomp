// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x0065AF50 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0065AF80 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0065AFA0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0065AFC0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0065AFE0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0065B000 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0065B020 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0065B060 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0065B090 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0065B0C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0065B0F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0065B100 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0065B120 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0065B140 SYMBOL
// _$E49

namespace Unsorted { extern int CurrentFrame; }

DynamicVectorClass<RadSiteClass *> RadSites;

// SYNTHETIC: GAMEMD 0x0065B160 SYMBOL
// _$E101
// SYNTHETIC: GAMEMD 0x0065B1A0 SYMBOL
// _$E99

// FUNCTION: GAMEMD 0x0065B1E0
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

// FUNCTION: GAMEMD 0x0065B2A0
RadSiteClass::RadSiteClass(NoInitClass const & x) :
	BASECLASS(x),
	RadLevelTimer(),
	RadLightTimer()
{
}

// FUNCTION: GAMEMD 0x0065B2F0
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

// FUNCTION: GAMEMD 0x0065B3A0
int RadSiteClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0065B3B0
void RadSiteClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: GAMEMD 0x0065B3C0
RTTIType RadSiteClass::Fetch_RTTI(void) const
{
	return(RTTI_RADSITE);
}

// FUNCTION: GAMEMD 0x0065B3D0
HRESULT STDMETHODCALLTYPE RadSiteClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) RadSiteClass(NoInitClass());
		Swizzle_Pointer(&LightSource);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0065B450
HRESULT STDMETHODCALLTYPE RadSiteClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x0065B470
HRESULT STDMETHODCALLTYPE RadSiteClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}
	*retval = CLSID_RadSiteClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0065B4B0
int RadSiteClass::GetSpread(void)
{
	return(Spread);
}

// FUNCTION: GAMEMD 0x0065B4C0
void RadSiteClass::SetBaseCell(Cell * cell)
{
	BaseCell = *cell;
}

// FUNCTION: GAMEMD 0x0065B4D0
void RadSiteClass::SetSpread(int spread)
{
	Spread = spread;
	SpreadInLeptons = spread * 256 + 128;
}

// FUNCTION: GAMEMD 0x0065B4F0
void RadSiteClass::SetRadLevel(int level)
{
	RadLevel = level;
	RadDuration = Rule->RadDurationMultiple * level;
	RadTimeLeft = RadDuration;
}

// FUNCTION: GAMEMD 0x0065B510
int RadSiteClass::GetRadLevel(void)
{
	if (RadDuration <= 0) {
		return(0);
	}
	return(RadTimeLeft * RadLevel / RadDuration);
}

// FUNCTION: GAMEMD 0x0065B530
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

// FUNCTION: GAMEMD 0x0065B580
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

// FUNCTION: GAMEMD 0x0065B800
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

// FUNCTION: GAMEMD 0x0065B8F0
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

// FUNCTION: GAMEMD 0x0065B9C0
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

// FUNCTION: GAMEMD 0x0065BB50
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

// FUNCTION: GAMEMD 0x0065BD00
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

// FUNCTION: GAMEMD 0x0065BE90
void RadSiteClass::DecreaseLight(void)
{
	LightSource->ChangeLevels(LightSource->Intensity - IntensityDecrement, Tint.Red * RadTimeLeft / RadDuration, Tint.Green * RadTimeLeft / RadDuration, Tint.Blue * RadTimeLeft / RadDuration, false);
}

// SYNTHETIC: GAMEMD 0x0065BED0
// ??_GRadSiteClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0065BEF0
// ?Clear@?$DynamicVectorClass@PAVRadSiteClass@@@@UAEXXZ
template class DynamicVectorClass<RadSiteClass *>;

// TEMPLATE: GAMEMD 0x0065BF20
// ?ID@?$DynamicVectorClass@PAVRadSiteClass@@@@UAEHPBQAVRadSiteClass@@@Z
// TEMPLATE: GAMEMD 0x0065BF40
// ?GetItem@?$VectorClass@PAVRadSiteClass@@@@UBEPAVRadSiteClass@@H@Z
// TEMPLATE: GAMEMD 0x0065BF50
// ??8?$VectorClass@PAVRadSiteClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0065BF90
// ?Resize@?$VectorClass@PAVRadSiteClass@@@@UAE_NHPBQAVRadSiteClass@@@Z
// TEMPLATE: GAMEMD 0x0065C040
// ?Clear@?$VectorClass@PAVRadSiteClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0065C070
// ?ID@?$VectorClass@PAVRadSiteClass@@@@UAEHABQAVRadSiteClass@@@Z
// TEMPLATE: GAMEMD 0x0065C0B0
// ?ID@?$VectorClass@PAVRadSiteClass@@@@UAEHPBQAVRadSiteClass@@@Z
// TEMPLATE: GAMEMD 0x0065C0D0
// ?Resize@?$DynamicVectorClass@PAVRadSiteClass@@@@UAE_NHPBQAVRadSiteClass@@@Z
// TEMPLATE: GAMEMD 0x0065C180
// ?ID@?$DynamicVectorClass@PAVRadSiteClass@@@@UAEHABQAVRadSiteClass@@@Z
// SYNTHETIC: GAMEMD 0x0065C1B0
// ??_G?$VectorClass@PAVRadSiteClass@@@@UAEPAXI@Z
template class VectorClass<RadSiteClass *>;

// SYNTHETIC: GAMEMD 0x0065C200
// ??_G?$DynamicVectorClass@PAVRadSiteClass@@@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x0065C250
RAMFileClass::RAMFileClass(void * buffer, int len) :
	Buffer((char *)buffer),
	MaxLength(len),
	Length(len),
	Offset(0),
	Access(READ),
	IsOpen(false),
	IsAllocated(false)
{
	if (buffer == NULL && len > 0) {
		Buffer = new char[len];
		IsAllocated = true;
	}
}

// FUNCTION: GAMEMD 0x0065C2A0
RAMFileClass::~RAMFileClass(void)
{
	Close();
	if (IsAllocated) {
		delete [] Buffer;
		Buffer = NULL;
		IsAllocated = false;
	}
}

// FUNCTION: GAMEMD 0x0065C2E0
int RAMFileClass::Create(void)
{
	if (!Is_Open()) {
		Length = 0;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0065C300
int RAMFileClass::Delete(void)
{
	if (!Is_Open()) {
		Length = 0;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0065C320
bool RAMFileClass::Is_Available(int )
{
	return(true);
}

// FUNCTION: GAMEMD 0x0065C330
bool RAMFileClass::Is_Open(void) const
{
	return(IsOpen);
}

// FUNCTION: GAMEMD 0x0065C340
int RAMFileClass::Open(char const *, int access)
{
	return(Open(access));
}

// FUNCTION: GAMEMD 0x0065C350
int RAMFileClass::Open(int access)
{
	if (Buffer == NULL || Is_Open()) {
		return(false);
	}

	Offset = 0;
	Access = access;
	IsOpen = true;

	switch (access) {
		default:
		case READ:
			break;

		case WRITE:
			Length = 0;
			break;

		case READ|WRITE:
			break;
	}

	return(Is_Open());
}

// FUNCTION: GAMEMD 0x0065C3A0
int RAMFileClass::Read(void * buffer, int size)
{
	if (Buffer == NULL || buffer == NULL || size == 0) {
		return(0);
	}

	bool hasopened = false;
	if (!Is_Open()) {
		Open(READ);
		hasopened = true;
	} else {
		if ((Access & READ) == 0) {
			return(0);
		}
	}

	int tocopy = (size < (Length-Offset)) ? size : (Length-Offset);
	memmove(buffer, &Buffer[Offset], tocopy);
	Offset += tocopy;

	if (hasopened) {
		Close();
	}

	return(tocopy);
}

// FUNCTION: GAMEMD 0x0065C420
int RAMFileClass::Seek(int pos, int dir)
{
	if (Buffer == NULL || !Is_Open()) {
		return(Offset);
	}

	int maxoffset = Length;
	if ((Access & WRITE) != 0) {
		maxoffset = MaxLength;
	}

	switch (dir) {
		case SEEK_CUR:
			Offset += pos;
			break;

		case SEEK_SET:
			Offset = 0 + pos;
			break;

		case SEEK_END:
			Offset = maxoffset + pos;
			break;
	}

	if (Offset < 0) Offset = 0;
	if (Offset > maxoffset) Offset = maxoffset;

	if (Offset > Length) {
		Length = Offset;
	}

	return(Offset);
}

// FUNCTION: GAMEMD 0x0065C4A0
int RAMFileClass::Size(void)
{
	return(Length);
}

// FUNCTION: GAMEMD 0x0065C4B0
int RAMFileClass::Write(void const * buffer, int size)
{
	if (Buffer == NULL || buffer == NULL || size == 0) {
		return(0);
	}

	bool hasopened = false;
	if (!Is_Open()) {
		Open(WRITE);
		hasopened = true;
	} else {
		if ((Access & WRITE) == 0) {
			return(0);
		}
	}

	int tocopy = (size < (MaxLength-Offset)) ? size : (MaxLength-Offset);
	memmove(&Buffer[Offset], buffer, tocopy);
	Offset += tocopy;
	if (Offset > Length) {
		Length = Offset;
	}

	if (hasopened) {
		Close();
	}

	return(tocopy);
}

// FUNCTION: GAMEMD 0x0065C540
// RAMFileClass::Close

// FUNCTION: GAMEMD 0x0065C550
// RAMFileClass::File_Name

// FUNCTION: GAMEMD 0x0065C560
// RAMFileClass::Set_Name

// FUNCTION: GAMEMD 0x0065C570
// RAMFileClass::Get_Date_Time

// FUNCTION: GAMEMD 0x0065C580
// RAMFileClass::Set_Date_Time

// FUNCTION: GAMEMD 0x0065C590
// RAMFileClass::Error

// SYNTHETIC: GAMEMD 0x0065C5A0
// ??_GRAMFileClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x0065C5F0
// FileClass::Get_Date_Time

// FUNCTION: GAMEMD 0x0065C600
// FileClass::Set_Date_Time

// SYNTHETIC: GAMEMD 0x0065C610
// ??_GFileClass@@UAEPAXI@Z
