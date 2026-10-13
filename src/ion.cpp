// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "ion.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "aircraft.h"
#include "anim.h"
#include "building.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "convert.h"
#include "data.h"
#include "draw.h"
#include "foot.h"
#include "globals.h"
#include "gscreen.h"
#include "house.h"
#include "swizzle.h"
#include "vector.h"
#include <objidl.h>

extern Cell DAT_00A9F9F8;

struct ScreenEffect {
	virtual ~ScreenEffect(void);
};

extern ScreenEffect * DAT_00A9FA9C;
extern int DAT_00A9FA98;
extern int DAT_00A9FAB0;
void __fastcall FUN_0053AD00(int, int, int, int);

class LightningStorm
{
	public:
		static Cell Coords;
		static DynamicVectorClass<CellClass *> CloudsManifesting;
		static DynamicVectorClass<CellClass *> CloudsPresent;
		static DynamicVectorClass<CellClass *> BoltsPresent;
		static bool Active;
		static int StartTime;
		static int Duration;
		static int Deferment;
		static bool TimeToEnd;
		static HouseClass * Owner;
};

class PsyDom
{
	public:
		static Cell Coords;
		static int Status;
		static AnimClass * Anim;
		static HouseClass * Owner;
};

class NukeFlash
{
	public:
		static int StartTime;
		static int Duration;
		static int Status;
};

Cell LightningStorm::Coords = DAT_00A9F9F8;
Cell PsyDom::Coords = DAT_00A9F9F8;
DynamicVectorClass<CellClass *> LightningStorm::CloudsManifesting;

DynamicVectorClass<CellClass *> LightningStorm::CloudsPresent;
DynamicVectorClass<CellClass *> LightningStorm::BoltsPresent;

// FUNCTION: 0x00539560 $E103
// FUNCTION: 0x00539570 $E106
// FUNCTION: 0x00539580 $E112
// FUNCTION: 0x005395C0 $E109
// FUNCTION: 0x00539620 $E118
// FUNCTION: 0x00539660 $E115
// FUNCTION: 0x005396C0 $E123
// FUNCTION: 0x00539700 $E121

// FUNCTION: 0x00539760
void FUN_00539760(void)
{
	PsyDom::Coords = DAT_00A9F9F8;
	LightningStorm::Coords = DAT_00A9F9F8;
	LightningStorm::Active = false;
	NukeFlash::Status = 0;
	LightningStorm::StartTime = -1;
	LightningStorm::Duration = -1;
	NukeFlash::StartTime = -1;
	NukeFlash::Duration = -1;
	PsyDom::Status = 0;
	PsyDom::Anim = NULL;
	PsyDom::Owner = NULL;
	LightningStorm::Deferment = 0;
	LightningStorm::Owner = NULL;
	LightningStorm::CloudsManifesting.Clear();
	LightningStorm::CloudsPresent.Clear();
	LightningStorm::BoltsPresent.Clear();
	Scen->DesiredAmbientLight = Scen->AmbientLight;
	if (DAT_00A9FA9C != NULL) {
		delete DAT_00A9FA9C;
	}
	DAT_00A9FA98 = 0;
	DAT_00A9FA9C = NULL;
	DAT_00A9FAB0 = 0;
	FUN_0053AD00(-1, -1, -1, 0);
}

// FUNCTION: 0x00539890
HRESULT __fastcall FUN_00539890(IStream * stream)
{
	int count;
	CellClass * item;
	HRESULT hr = stream->Write(&LightningStorm::Active, sizeof(LightningStorm::Active), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&LightningStorm::StartTime, sizeof(LightningStorm::StartTime), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&LightningStorm::Duration, sizeof(LightningStorm::Duration), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&LightningStorm::Deferment, sizeof(LightningStorm::Deferment), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&LightningStorm::TimeToEnd, sizeof(LightningStorm::TimeToEnd), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&NukeFlash::StartTime, sizeof(NukeFlash::StartTime), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&NukeFlash::Duration, sizeof(NukeFlash::Duration), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&NukeFlash::Status, sizeof(NukeFlash::Status), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&PsyDom::Status, sizeof(PsyDom::Status), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&PsyDom::Anim, sizeof(PsyDom::Anim), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&LightningStorm::Coords, sizeof(LightningStorm::Coords), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&PsyDom::Coords, sizeof(PsyDom::Coords), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&LightningStorm::Owner, sizeof(LightningStorm::Owner), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Write(&PsyDom::Owner, sizeof(PsyDom::Owner), NULL);
	if (FAILED(hr)) return(hr);

	count = LightningStorm::CloudsManifesting.Count();
	hr = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(hr)) return(hr);
	for (int i1 = 0; i1 < count; i1++) {
		item = LightningStorm::CloudsManifesting[i1];
		hr = stream->Write(&item, sizeof(item), NULL);
		if (FAILED(hr)) return(hr);
	}

	count = LightningStorm::CloudsPresent.Count();
	hr = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(hr)) return(hr);
	for (int i2 = 0; i2 < count; i2++) {
		item = LightningStorm::CloudsPresent[i2];
		hr = stream->Write(&item, sizeof(item), NULL);
		if (FAILED(hr)) return(hr);
	}

	count = LightningStorm::BoltsPresent.Count();
	hr = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(hr)) return(hr);
	for (int i3 = 0; i3 < count; i3++) {
		item = LightningStorm::BoltsPresent[i3];
		hr = stream->Write(&item, sizeof(item), NULL);
		if (FAILED(hr)) return(hr);
	}
	return(hr);
}

// FUNCTION: 0x00539AE0
HRESULT __fastcall FUN_00539AE0(IStream * stream)
{
	int count;
	CellClass * item;
	HRESULT hr = stream->Read(&LightningStorm::Active, sizeof(LightningStorm::Active), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&LightningStorm::StartTime, sizeof(LightningStorm::StartTime), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&LightningStorm::Duration, sizeof(LightningStorm::Duration), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&LightningStorm::Deferment, sizeof(LightningStorm::Deferment), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&LightningStorm::TimeToEnd, sizeof(LightningStorm::TimeToEnd), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&NukeFlash::StartTime, sizeof(NukeFlash::StartTime), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&NukeFlash::Duration, sizeof(NukeFlash::Duration), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&NukeFlash::Status, sizeof(NukeFlash::Status), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&PsyDom::Status, sizeof(PsyDom::Status), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&PsyDom::Anim, sizeof(PsyDom::Anim), NULL);
	if (FAILED(hr)) return(hr);

	Swizzle_Pointer(&PsyDom::Anim);

	hr = stream->Read(&LightningStorm::Coords, sizeof(LightningStorm::Coords), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&PsyDom::Coords, sizeof(PsyDom::Coords), NULL);
	if (FAILED(hr)) return(hr);

	hr = stream->Read(&LightningStorm::Owner, sizeof(LightningStorm::Owner), NULL);
	if (FAILED(hr)) return(hr);

	Swizzle_Pointer(&LightningStorm::Owner);

	hr = stream->Read(&PsyDom::Owner, sizeof(PsyDom::Owner), NULL);
	if (FAILED(hr)) return(hr);

	Swizzle_Pointer(&PsyDom::Owner);

	hr = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(hr)) return(hr);
	for (int i1 = 0; i1 < count; i1++) {
		hr = stream->Read(&item, sizeof(item), NULL);
		if (FAILED(hr)) return(hr);
		LightningStorm::CloudsManifesting.Add(item);
	}
	for (int j1 = 0; j1 < count; j1++) {
		Swizzle_Pointer(&LightningStorm::CloudsManifesting[j1]);
	}

	hr = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(hr)) return(hr);
	for (int i2 = 0; i2 < count; i2++) {
		hr = stream->Read(&item, sizeof(item), NULL);
		if (FAILED(hr)) return(hr);
		LightningStorm::CloudsPresent.Add(item);
	}
	for (int j2 = 0; j2 < count; j2++) {
		Swizzle_Pointer(&LightningStorm::CloudsPresent[j2]);
	}

	hr = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(hr)) return(hr);
	for (int i3 = 0; i3 < count; i3++) {
		hr = stream->Read(&item, sizeof(item), NULL);
		if (FAILED(hr)) return(hr);
		LightningStorm::BoltsPresent.Add(item);
	}
	for (int j3 = 0; j3 < count; j3++) {
		Swizzle_Pointer(&LightningStorm::BoltsPresent[j3]);
	}

	DAT_00A9FA9C = NULL;
	return(hr);
}

// FUNCTION: 0x00539EA0
void __fastcall FUN_00539EA0(bool active)
{
	LightningStorm::Active = active;
}
