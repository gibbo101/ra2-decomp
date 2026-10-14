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
#include "msglist.h"
#include "rules.h"
#include "scenario.h"
#include "stringtable.h"
#include "voc.h"
#include "vox.h"
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
void __fastcall FUN_0053C280(void);
void __fastcall FUN_0053AF40(void);
void __fastcall FUN_0053B560(void);
void __fastcall FUN_00539EB0(int, int, Cell, HouseClass *);
void __fastcall FUN_0053A300(Coord);
int __fastcall FUN_006D2120(int);
void __fastcall FUN_0065FA70(int, Cell);
extern int DAT_00A9FA84;
extern int DAT_00A9FA90;
extern Coord DAT_00A9FA30;
namespace Unsorted { extern int CurrentFrame; extern int MuteSWLaunches; }
extern MessageListClass DAT_00A8BC60;

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

// FUNCTION: 0x00539EB0
void __fastcall FUN_00539EB0(int duration, int deferment, Cell cell, HouseClass * owner)
{
	if (cell == DAT_00A9F9F8) {
		while (!Map.In_Radar(cell)) {
			short y = Random_Pick(0, Map.MapRect.Height);
			short x = Random_Pick(0, Map.MapRect.Width);
			cell = Cell(x, y);
		}
	}
	LightningStorm::Coords = cell;
	LightningStorm::Owner = owner;
	if (LightningStorm::Active) {
		return;
	}
	if (deferment != 0) {
		if (LightningStorm::Deferment == 0 || LightningStorm::Deferment >= deferment) {
			LightningStorm::Deferment = deferment;
		}
		LightningStorm::Duration = duration;
		return;
	}
	FUN_0065FA70(13, cell);
	LightningStorm::Duration = duration;
	LightningStorm::StartTime = Unsorted::CurrentFrame;
	LightningStorm::Active = true;
	for (int i = 0; i < Houses.Count(); i++) {
		if (LightningStorm::Owner == NULL || !LightningStorm::Owner->Is_Ally(Houses[i])) {
			if (!Houses[i]->IsDefeated && !Unsorted::MuteSWLaunches) {
				Houses[i]->Create_Radar_Outage(duration);
			}
		}
	}
	if (PlayerPtr) {
		PlayerPtr->RecalcRadar = true;
	}
	FUN_0053C280();
	if (Rule->LightningPrintText) {
		int scheme = 3;
		if (PlayerPtr) {
			scheme = PlayerPtr->Scheme;
		}
		VocClass::PlayGlobal(Rule->StormSound, 1.0f, 0x2000);
		DAT_00A8BC60.Add_Message(NULL, 0, StringTable::LoadString("TXT_LIGHTNING_STORM", NULL, "D:\\ra2mdpost\\Ion.cpp", 0x213), scheme, TextPrintType(0x4046), 150, false);
	}
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: 0x0053A090
void FUN_0053A090(void)
{
	if (LightningStorm::Active) {
		LightningStorm::TimeToEnd = true;
	}
}

// FUNCTION: 0x0053A0B0
void FUN_0053A0B0(void)
{
	if (LightningStorm::Active) {
		LightningStorm::Active = false;
		LightningStorm::Owner = NULL;
		LightningStorm::Coords = DAT_00A9F9F8;
		FUN_0053C280();
	}
}

// FUNCTION: 0x0053A0E0
bool FUN_0053A0E0(void)
{
	return LightningStorm::Active || LightningStorm::Deferment > 0;
}

// FUNCTION: 0x0053A100
bool FUN_0053A100(void)
{
	return LightningStorm::Active;
}

// FUNCTION: 0x0053A110
bool FUN_0053A110(void)
{
	return NukeFlash::Status == 1;
}

// FUNCTION: 0x0053A120
bool FUN_0053A120(void)
{
	return NukeFlash::Status == 2;
}

// FUNCTION: 0x0053A130
bool FUN_0053A130(void)
{
	return false;
}

// FUNCTION: 0x0053A140
void __fastcall FUN_0053A140(Cell cell)
{
	CellClass * cellptr = &Map[cell];
	int half = ((short const *)Rule->WeatherConBolts[0]->Get_Image_Data())[2] / 2;
	int zz = FUN_006D2120(half);
	int bheight = (cellptr->IsUnderBridge ? DAT_00A9FA84 : 0);
	Coord coord(cell, zz + bheight + cellptr->Height * DAT_00A9FA90);
	if (coord != DAT_00A9FA30) {
		AnimTypeClass const * type = Rule->WeatherConClouds.Pick(Scen->RandomNumber);
		AnimClass * anim = new AnimClass(type, coord, 0, 1, ShapeFlags_Type(0x600), 0, false);
		LightningStorm::CloudsManifesting.Add((CellClass *)anim);
		LightningStorm::CloudsPresent.Add((CellClass *)anim);
	}
}

// FUNCTION: 0x0053A6C0
void FUN_0053A6C0(void)
{
	int i;
	if (NukeFlash::Status == 1) {
		if (NukeFlash::Duration != -1 && NukeFlash::StartTime + NukeFlash::Duration < Unsorted::CurrentFrame) {
			NukeFlash::Status = 2;
			NukeFlash::Duration = 15;
			NukeFlash::StartTime = Unsorted::CurrentFrame;
			FUN_0053C280();
			Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
		}
	} else if (NukeFlash::Status == 2) {
		if (NukeFlash::Duration != -1 && NukeFlash::StartTime + NukeFlash::Duration < Unsorted::CurrentFrame) {
			NukeFlash::Status = 0;
		}
	}

	FUN_0053AF40();
	FUN_0053B560();

	if (LightningStorm::BoltsPresent.Count() > 0) {
		for (i = LightningStorm::BoltsPresent.Count() - 1; i >= 0; i--) {
			AnimClass * anim = (AnimClass *)LightningStorm::BoltsPresent[i];
			if (anim->Animation.Fetch_Stage() >= ((short const *)anim->Class_Of()->Get_Image_Data())[3] / 2) {
				LightningStorm::BoltsPresent.Delete_Index(i);
			}
		}
	}

	for (i = LightningStorm::CloudsManifesting.Count() - 1; i >= 0; i--) {
		AnimClass * anim = (AnimClass *)LightningStorm::CloudsManifesting[i];
		if (anim->Animation.Fetch_Stage() > ((short const *)anim->Class_Of()->Get_Image_Data())[3] / 2) {
			FUN_0053A300(anim->Center_Coord());
			LightningStorm::CloudsManifesting.Delete_Index(i);
		}
	}

	if (LightningStorm::CloudsPresent.Count() > 0) {
		for (i = LightningStorm::CloudsPresent.Count() - 1; i >= 0; i--) {
			AnimClass * anim = (AnimClass *)LightningStorm::CloudsPresent[i];
			if (anim->Animation.Fetch_Stage() >= ((short const *)anim->Class_Of()->Get_Image_Data())[3] - 1) {
				LightningStorm::CloudsPresent.Delete_Index(i);
			}
		}
	} else if (LightningStorm::TimeToEnd) {
		FUN_0053A0B0();
		LightningStorm::TimeToEnd = false;
	}

	if (LightningStorm::Active && !LightningStorm::TimeToEnd) {
		if (LightningStorm::Duration != -1 && LightningStorm::StartTime + LightningStorm::Duration < Unsorted::CurrentFrame) {
			LightningStorm::TimeToEnd = true;
			return;
		}
		if (Unsorted::CurrentFrame % Rule->LightningHitDelay == 0) {
			FUN_0053A140(LightningStorm::Coords);
		}
		if (Unsorted::CurrentFrame % Rule->LightningScatterDelay == 0) {
			int spread = Rule->LightningCellSpread >> 1;
			int lo = -spread;
			int tries;
			Cell target;
			bool blocked;
			for (tries = 3; tries > 0; tries--) {
				target = LightningStorm::Coords;
				target.X += Scen->RandomNumber(lo, spread);
				target.Y += Scen->RandomNumber(lo, spread);
				blocked = false;
				for (i = 0; i < LightningStorm::CloudsPresent.Count(); i++) {
					Cell cc = ((AnimClass *)LightningStorm::CloudsPresent[i])->Center_Coord().As_Cell();
					int dx = target.X - cc.X;
					if (dx < 0) dx = -dx;
					int dy = target.Y - cc.Y;
					if (dy < 0) dy = -dy;
					if (dx + dy < Rule->LightningSeparation) {
						blocked = true;
						break;
					}
				}
				if (Map.In_Radar(target) && !blocked) {
					break;
				}
			}
			if (tries > 0) {
				FUN_0053A140(target);
			}
		}
	} else if (LightningStorm::Deferment > 0) {
		LightningStorm::Deferment--;
		if (LightningStorm::Deferment == 0) {
			FUN_00539EB0(LightningStorm::Duration, 0, LightningStorm::Coords, LightningStorm::Owner);
		} else if (LightningStorm::Deferment % 225 == 0 && Rule->LightningPrintText) {
			int scheme = 3;
			if (PlayerPtr) {
				scheme = PlayerPtr->Scheme;
			}
			VoxClass::Play("EVA_LightningStormCreated", -1, -1);
			DAT_00A8BC60.Add_Message(NULL, 0, StringTable::LoadString("TXT_LIGHTNING_STORM_APPROACHING", NULL, "D:\\ra2mdpost\\Ion.cpp", 0x3ce), scheme, TextPrintType(0x46), 150, false);
		}
	}
}

// FUNCTION: 0x0053AB50
void FUN_0053AB50(void)
{
	FUN_0053C280();
	PlayerPtr->RecalcRadar = true;
}
