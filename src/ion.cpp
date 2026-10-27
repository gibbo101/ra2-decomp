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
#include "session.h"
#include "stringtable.h"
#include "voc.h"
#include "vox.h"
#include "swizzle.h"
#include "vector.h"
#include "hashtable.h"
#include "scheme.h"
#include "capmgr.h"
#include "lightcon.h"
#include <objidl.h>

extern Cell DAT_00A9F9F8;

struct ScreenEffect {
	virtual ~ScreenEffect(void);
};

extern ScreenEffect * DAT_00A9FA9C;
extern int DAT_00A9FA98;
extern int DAT_00A9FAB0;
void __fastcall FUN_0053AD00(int, int, int, bool);
void __fastcall FUN_0053C280(void);
void __fastcall FUN_0053AF40(void);
void __fastcall FUN_0053B560(void);
void __fastcall FUN_00539EB0(int, int, Cell, HouseClass *);
void __fastcall FUN_0053A300(Coord);
int __fastcall FUN_006D2120(int);
void __fastcall FUN_0065FA70(int, Cell);
DynamicVectorClass<ColorScheme *> * __fastcall GetPaletteSchemesFromIterator(HashIterator * it);
int __fastcall SchemeLibrary_Count(void);
extern int DAT_00A9FA84;
extern int DAT_00A9FA90;
extern Coord DAT_00A9FA30;
extern int DAT_007ED3D0[];
namespace Unsorted { extern Cell CellSpreadTable[]; extern int CurrentFrame; extern int MuteSWLaunches; }

class IonBlastClass
{
	public:
		IonBlastClass(Coord coord);

		unsigned char Before[0x10];
		BOOL DisableIonBeam;
};

class LightningStorm
{
	public:
		static Cell Coords;
		static DynamicVectorClass<AnimClass const *> CloudsManifesting;
		static DynamicVectorClass<AnimClass const *> CloudsPresent;
		static DynamicVectorClass<AnimClass const *> BoltsPresent;
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
DynamicVectorClass<AnimClass const *> LightningStorm::CloudsManifesting;

DynamicVectorClass<AnimClass const *> LightningStorm::CloudsPresent;
DynamicVectorClass<AnimClass const *> LightningStorm::BoltsPresent;

// SYNTHETIC: GAMEMD 0x00539560 SYMBOL
// _$E117
// SYNTHETIC: GAMEMD 0x00539570 SYMBOL
// _$E120
// SYNTHETIC: GAMEMD 0x00539580 SYMBOL
// _$E126
// SYNTHETIC: GAMEMD 0x005395C0 SYMBOL
// _$E123
// SYNTHETIC: GAMEMD 0x00539620 SYMBOL
// _$E132
// SYNTHETIC: GAMEMD 0x00539660 SYMBOL
// _$E129
// SYNTHETIC: GAMEMD 0x005396C0 SYMBOL
// _$E137
// SYNTHETIC: GAMEMD 0x00539700 SYMBOL
// _$E135

// FUNCTION: GAMEMD 0x00539760
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

// FUNCTION: GAMEMD 0x00539890
HRESULT __fastcall FUN_00539890(IStream * stream)
{
	int count;
	AnimClass const * item;
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

// FUNCTION: GAMEMD 0x00539AE0
HRESULT __fastcall FUN_00539AE0(IStream * stream)
{
	int count;
	AnimClass const * item;
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

// FUNCTION: GAMEMD 0x00539EA0
void __fastcall FUN_00539EA0(bool active)
{
	LightningStorm::Active = active;
}

// FUNCTION: GAMEMD 0x00539EB0
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
		Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_LIGHTNING_STORM", NULL, "D:\\ra2mdpost\\Ion.cpp", 0x213), scheme, TextPrintType(0x4046), 150, false);
	}
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: GAMEMD 0x0053A090
void FUN_0053A090(void)
{
	if (LightningStorm::Active) {
		LightningStorm::TimeToEnd = true;
	}
}

// FUNCTION: GAMEMD 0x0053A0B0
void FUN_0053A0B0(void)
{
	if (LightningStorm::Active) {
		LightningStorm::Active = false;
		LightningStorm::Owner = NULL;
		LightningStorm::Coords = DAT_00A9F9F8;
		FUN_0053C280();
	}
}

// FUNCTION: GAMEMD 0x0053A0E0
bool FUN_0053A0E0(void)
{
	return LightningStorm::Active || LightningStorm::Deferment > 0;
}

// FUNCTION: GAMEMD 0x0053A100
bool FUN_0053A100(void)
{
	return LightningStorm::Active;
}

// FUNCTION: GAMEMD 0x0053A110
bool FUN_0053A110(void)
{
	return NukeFlash::Status == 1;
}

// FUNCTION: GAMEMD 0x0053A120
bool FUN_0053A120(void)
{
	return NukeFlash::Status == 2;
}

// FUNCTION: GAMEMD 0x0053A130
bool FUN_0053A130(void)
{
	return false;
}

// FUNCTION: GAMEMD 0x0053A140
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
		LightningStorm::CloudsManifesting.Add(anim);
		LightningStorm::CloudsPresent.Add(anim);
	}
}

// FUNCTION: GAMEMD 0x0053A6C0
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
			AnimClass const * anim = LightningStorm::BoltsPresent[i];
			if (anim->Animation.Fetch_Stage() >= ((short const *)anim->Class_Of()->Get_Image_Data())[3] / 2) {
				LightningStorm::BoltsPresent.Delete_Index(i);
			}
		}
	}

	for (i = LightningStorm::CloudsManifesting.Count() - 1; i >= 0; i--) {
		AnimClass const * anim = LightningStorm::CloudsManifesting[i];
		if (anim->Animation.Fetch_Stage() > ((short const *)anim->Class_Of()->Get_Image_Data())[3] / 2) {
			FUN_0053A300(anim->Center_Coord());
			LightningStorm::CloudsManifesting.Delete_Index(i);
		}
	}

	if (LightningStorm::CloudsPresent.Count() > 0) {
		for (i = LightningStorm::CloudsPresent.Count() - 1; i >= 0; i--) {
			AnimClass const * anim = LightningStorm::CloudsPresent[i];
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
					Cell cc = LightningStorm::CloudsPresent[i]->Center_Coord().As_Cell();
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
			Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_LIGHTNING_STORM_APPROACHING", NULL, "D:\\ra2mdpost\\Ion.cpp", 0x3ce), scheme, TextPrintType(0x46), 150, false);
		}
	}
}

// FUNCTION: GAMEMD 0x0053AB50
void FUN_0053AB50(void)
{
	FUN_0053C280();
	PlayerPtr->RecalcRadar = true;
}

// FUNCTION: GAMEMD 0x0053AB70
void FUN_0053AB70(void)
{
	NukeFlash::Status = 1;
	NukeFlash::Duration = 30;
	NukeFlash::StartTime = Frame;
	Scen->AmbientChangeTimer = 1;
	Scen->DesiredAmbientLight = Scen->NukeAmbient;
	FUN_0053AD00(NORMAL_LIGHT * Scen->NukeLighting.RedTint / 100, NORMAL_LIGHT * Scen->NukeLighting.GreenTint / 100, NORMAL_LIGHT * Scen->NukeLighting.BlueTint / 100, 1);
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: GAMEMD 0x0053AC50
void FUN_0053AC50(void)
{
	NukeFlash::Status = 2;
	NukeFlash::Duration = 15;
	NukeFlash::StartTime = Unsorted::CurrentFrame;
	FUN_0053C280();
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: GAMEMD 0x0053AC80
void __fastcall FUN_0053AC80(int r, int g, int b, bool tint)
{
	HashIterator it = {0, 0, false};
	DynamicVectorClass<ColorScheme *> * schemes;
	while ((schemes = GetPaletteSchemesFromIterator(&it)) != NULL) {
		int count = SchemeLibrary_Count();
		for (int i = 0; i < count; i++) {
			(*schemes)[i]->Converter->Apply_Tint(r, g, b, tint);
		}
	}
}

// FUNCTION: GAMEMD 0x0053AD00
void __fastcall FUN_0053AD00(int r, int g, int b, bool tint)
{
	int i;
	for (i = 0; i < TileDrawers.Count(); i++) {
		TileDrawers[i]->Apply_Tint(r, g, b, tint);
	}
	for (i = 0; i < ColorSchemes.Count(); i++) {
		ColorScheme * scheme = ColorSchemes[i];
		if (scheme->IntensityLevels > 1) {
			scheme->Converter->Apply_Tint(r, g, b, tint);
		}
	}
	FUN_0053AC80(r, g, b, tint);
	Map.Update_Cell_Colors();
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: GAMEMD 0x0053AE00
void FUN_0053AE00(void)
{
	wchar_t const * text = StringTable::LoadString("Msg:LightningStormActive", NULL, "D:\\ra2mdpost\\Ion.cpp", 0x463);
	int scheme = 3;
	if (PlayerPtr) {
		scheme = PlayerPtr->Scheme;
	}
	Session.Messages.Add_Message(NULL, 0, text, scheme, TextPrintType(0x4046), 150, false);
}

// FUNCTION: GAMEMD 0x0053B400
bool FUN_0053B400(void)
{
	return PsyDom::Status ? true : false;
}

// FUNCTION: GAMEMD 0x0053B410
void FUN_0053B410(void)
{
	wchar_t const * text = StringTable::LoadString("Msg:DominatorActive", NULL, "D:\\ra2mdpost\\Ion.cpp", 0x53b);
	int scheme = 3;
	if (PlayerPtr) {
		scheme = PlayerPtr->Scheme;
	}
	Session.Messages.Add_Message(NULL, 0, text, scheme, TextPrintType(0x4046), 150, false);
}

// FUNCTION: GAMEMD 0x0053AE50
void __fastcall FUN_0053AE50(HouseClass * owner, Cell cell)
{
	if (Rule->DominatorFirstAnim == NULL || Rule->DominatorSecondAnim == NULL) {
		return;
	}
	PsyDom::Coords = cell;
	PsyDom::Owner = owner;
	AnimTypeClass const * type = Rule->DominatorFirstAnim;
	Coord coord = Map[PsyDom::Coords].Center_Coord();
	coord.Z += 750;
	PsyDom::Anim = new AnimClass(type, coord, 0, 1, ShapeFlags_Type(0x600), 0, false);
	PsyDom::Status = 1;
	Scen->AmbientChangeTimer = 1;
	FUN_0053C280();
}

static inline TechnoClass * Cast_Techno(ObjectClass * object)
{
	if (object == NULL) return NULL;
	return (object->AbstractFlags & 1) ? (TechnoClass *)object : NULL;
}

// FUNCTION: GAMEMD 0x0053B080
void FUN_0053B080(void)
{
	IonBlastClass * blast = new IonBlastClass(Map[PsyDom::Coords].Center_Coord());
	blast->DisableIonBeam = 1;
	AnimTypeClass const * type = Rule->DominatorSecondAnim;
	Coord coord = Map[PsyDom::Coords].Center_Coord();
	PsyDom::Anim = new AnimClass(type, coord, 0, 1, ShapeFlags_Type(0x600), 0, false);
	MapClass::DamageArea(coord, Rule->DominatorDamage, NULL, Rule->DominatorWarhead, true, LightningStorm::Owner);
	int count = DAT_007ED3D0[Rule->DominatorCaptureRange < 10 ? Rule->DominatorCaptureRange : 10];
	DynamicVectorClass<TechnoClass *> victims;
	for (int n = 0; n < count; n++) {
		Cell const * ptr = &Unsorted::CellSpreadTable[n];
		Cell entry = *ptr;
		TechnoClass * techno = Map[entry + PsyDom::Coords].Cell_Techno(Point2D(0, 0), false, NULL);
		while (techno != NULL) {
			if (techno->Fetch_RTTI() != RTTI_BUILDING && !techno->Techno_Type_Class()->ImmuneToPsionics && !techno->IsIronCurtained() && !techno->Techno_Type_Class()->BalloonHover && !techno->In_Air()) {
				if (techno->MindControlledBy != NULL) {
					techno->MindControlledBy->CaptureManager->FreeUnit(techno);
				}
				techno->Captured(PsyDom::Owner, true);
				techno->MindControlledByAUnit = true;
				int ring = techno->Techno_Type_Class()->MindControlRingOffset;
				Coord center = techno->Center_Coord() + Coord(0, 0, ring);
				AnimClass * anim = new AnimClass(Rule->PermaControlledAnimationType, center, 0, 1, ShapeFlags_Type(0x600), 0, false);
				techno->MindControlRingAnim = anim;
				if (anim != NULL) {
					anim->SetOwnerObject(techno);
				}
				victims.Add(techno);
			}
			if (techno->Next == NULL) {
				break;
			}
			techno = Cast_Techno(techno->Next);
		}
	}
	if (!PsyDom::Owner->FUN_0050B730()) {
		for (int i = victims.Count() - 1; i >= 0; i--) {
			victims[i]->QueueMission(MISSION_HUNT, false);
		}
	}
}

// FUNCTION: GAMEMD 0x0053B3F0
void FUN_0053B3F0(void)
{
}

#include "dsurface.h"
#include "geometry.h"

extern DSurface * DAT_00A9FA9C_Surface;
extern float DAT_00A9FAA0;
extern float DAT_00A9FAA4;

extern bool DAT_00A9FA94;
extern DSurface * DAT_00A9FAAC;
extern int DAT_00A9FAA8;
extern bool DAT_00A9FAD1;
extern AudioController DAT_00A9F9B8;
void __fastcall Hold_Scenario(int seconds);
void Release_Scenario(void);
void __fastcall FUN_007518C0(bool enabled);
void __fastcall FUN_00753580(void);
void __fastcall FUN_0053BBA0(bool forward);

// FUNCTION: GAMEMD 0x0053B560
void FUN_0053B560(void)
{
	switch (DAT_00A9FAB0) {
	case 0:
		goto done;
	case 1:
		FUN_0053BBA0(false);
		if (--DAT_00A9FA98 == 0) {
			DAT_00A9FAB0 = 2;
			DAT_00A9FA98 = 45;
		}
		break;
	case 2:
		DSurface::Composite->Fill_Rect(DSurface::ViewBounds, DSurface::ViewBounds, DSurface::Build_Hicolor_Pixel(255, 255, 255));
		DAT_00A9FA98--;
		if (DAT_00A9FA98 == 1) {
			Hold_Scenario(0);
		}
		if (DAT_00A9FA98 == 0) {
			Map.Flag_To_Redraw(GS_REDRAW_ALL);
			DAT_00A9FA9C_Surface->Fill(0);
			DAT_00A9FAAC = DSurface::Composite;
			DSurface::Composite = DAT_00A9FA9C_Surface;
			DAT_00A9FAB0 = 0;
			DAT_00A9FA94 = true;
			Map.Render();
			DAT_00A9FA94 = false;
			DSurface::Composite = DAT_00A9FAAC;
			DAT_00A9FA98 = DAT_00A9FAA8;
			DAT_00A9FAB0 = 3;
			FUN_007518C0(true);
			DAT_00A9F9B8.EndLooping();
			VocClass::PlayGlobal(Rule->LetsDoTheTimeWarpInAgain, 1.0f, 0x2000, &DAT_00A9F9B8);
			FUN_007518C0(false);
		}
		break;
	case 3:
		FUN_0053BBA0(true);
		if (--DAT_00A9FA98 == 0) {
			Release_Scenario();
			DAT_00A9F9B8.EndLooping();
			DAT_00A9F9B8.SetEvent(NULL, NULL);
			DAT_00A9F9B8.FUN_00405C00();
			FUN_007518C0(true);
			FUN_00753580();
			if (!DAT_00A9FAD1) {
				Unlock_Scenario_Input();
			}
			DAT_00A9FAB0 = 0;
			DSurface::Composite->Blit_From(*DAT_00A9FA9C_Surface, false, true);
			if (DAT_00A9FA9C_Surface != NULL) {
				delete DAT_00A9FA9C_Surface;
			}
			DAT_00A9FA9C_Surface = NULL;
			Map.Flag_To_Redraw(GS_REDRAW_ALL);
		}
		break;
	}
done:;
}

extern int DAT_007F5BFC;

static inline void Fade_556(unsigned short * dest, int width, int height, int pad, unsigned short alpha)
{
	int row;
	int col;
	unsigned short * source = dest;
	for (row = height; row > 0; row--) {
		for (col = width; col > 0; col--) {
			unsigned short pixel = *source;
			unsigned short r = pixel >> 11;
			unsigned short g = (pixel >> 6) & 0x1F;
			unsigned short b = pixel & 0x3F;
			r += ((0x1F - r) * alpha) >> 8;
			g += ((0x1F - g) * alpha) >> 8;
			b += ((0x3F - b) * alpha) >> 8;
			*dest++ = (r << 11) | (g << 6) | b;
			source++;
		}
		dest += pad;
		source += pad;
	}
}

static inline void Fade_565(unsigned short * dest, int width, int height, int pad, unsigned short alpha)
{
	int row;
	int col;
	unsigned short * source = dest;
	for (row = height; row > 0; row--) {
		for (col = width; col > 0; col--) {
			unsigned short pixel = *source;
			unsigned short r = pixel >> 11;
			unsigned short g = (pixel >> 5) & 0x3F;
			unsigned short b = pixel & 0x1F;
			r += ((0x1F - r) * alpha) >> 8;
			g += ((0x3F - g) * alpha) >> 8;
			b += ((0x1F - b) * alpha) >> 8;
			*dest++ = (r << 11) | (g << 5) | b;
			source++;
		}
		dest += pad;
		source += pad;
	}
}

static inline void Fade_555(unsigned short * dest, int width, int height, int pad, unsigned short alpha)
{
	int row;
	int col;
	unsigned short * source = dest;
	for (row = height; row > 0; row--) {
		for (col = width; col > 0; col--) {
			unsigned short pixel = *source;
			unsigned short r = (pixel >> 10) & 0x1F;
			unsigned short g = (pixel >> 5) & 0x1F;
			unsigned short b = pixel & 0x1F;
			r += ((0x1F - r) * alpha) >> 8;
			g += ((0x1F - g) * alpha) >> 8;
			b += ((0x1F - b) * alpha) >> 8;
			*dest++ = (r << 10) | (g << 5) | b;
			source++;
		}
		dest += pad;
		source += pad;
	}
}

// FUNCTION: GAMEMD 0x0053B780
void __fastcall FUN_0053B780(DSurface * surface)
{
	if (DAT_00A9FAB0 != 3) {
		return;
	}
	int width = surface->Get_Width();
	int height = surface->Get_Height();
	int pad = (surface->Stride() - width * 2) / 2;
	height -= DAT_007F5BFC;
	unsigned short * dest = (unsigned short *)surface->Lock(Point2D(0, 0));
	unsigned short alpha = DAT_00A9FA98 * 255 / DAT_00A9FAA8;
	switch (DSurface::Get_Primary_Color_Mode()) {
	case 1:
		Fade_556(dest, width, height, pad, alpha);
		break;
	case 2:
		Fade_565(dest, width, height, pad, alpha);
		break;
	default:
		Fade_555(dest, width, height, pad, alpha);
		break;
	}
	surface->Unlock();
}

// FUNCTION: GAMEMD 0x0053BAD0
bool FUN_0053BAD0(void)
{
	return DAT_00A9FAB0 != 0;
}

// FUNCTION: GAMEMD 0x0053BAE0
bool FUN_0053BAE0(void)
{
	return DAT_00A9FAB0 != 0;
}



// FUNCTION: GAMEMD 0x0053BAF0
void FUN_0053BAF0(void)
{
	if (DAT_00A9FA9C_Surface != NULL) {
		delete DAT_00A9FA9C_Surface;
		DAT_00A9FA9C_Surface = NULL;
	}
	DAT_00A9FA9C_Surface = new DSurface(DSurface::Composite->Get_Width(), DSurface::Composite->Get_Height(), true, false);
	DAT_00A9FA9C_Surface->Blit_From(*DSurface::Composite, false, true);
	DAT_00A9FAA0 = 0;
	DAT_00A9FAA4 = 0;
}

// FUNCTION: GAMEMD 0x0053BB80
void FUN_0053BB80(void)
{
	if (DAT_00A9FA9C_Surface != NULL) {
		delete DAT_00A9FA9C_Surface;
	}
	DAT_00A9FA9C_Surface = NULL;
}

// FUNCTION: GAMEMD 0x0053C280
void __fastcall FUN_0053C280(void)
{
	if (NukeFlash::Status != 1 && DAT_00A9FAB0 == 0) {
		if (LightningStorm::Active) {
			Scen->DesiredAmbientLight = Scen->IonAmbientLight;
			FUN_0053AD00(Scen->IonLighting.RedTint * 1000 / 100, Scen->IonLighting.GreenTint * 1000 / 100, Scen->IonLighting.BlueTint * 1000 / 100, true);
		} else if (PsyDom::Status != 0 && PsyDom::Status != 5) {
			Scen->DesiredAmbientLight = Scen->DominatorAmbient;
			FUN_0053AD00(Scen->DominatorLighting.RedTint * 1000 / 100, Scen->DominatorLighting.GreenTint * 1000 / 100, Scen->DominatorLighting.BlueTint * 1000 / 100, true);
		} else {
			Scen->DesiredAmbientLight = Scen->AmbientLight;
			FUN_0053AD00(-1, -1, -1, false);
		}
	} else {
		Scen->DesiredAmbientLight = Scen->NukeAmbient;
		FUN_0053AD00(Scen->NukeLighting.RedTint * 1000 / 100, Scen->NukeLighting.GreenTint * 1000 / 100, Scen->NukeLighting.BlueTint * 1000 / 100, true);
	}
}

// FUNCTION: GAMEMD 0x0053C450
bool TechnoClass::CanBePermaMindControlled(void) const
{
	if (this == NULL) {
		return false;
	}
	if (Fetch_RTTI() == RTTI_BUILDING) {
		return false;
	}
	if (Techno_Type_Class()->ImmuneToPsionics) {
		return false;
	}
	if (IsIronCurtained()) {
		return false;
	}
	if (Techno_Type_Class()->BalloonHover) {
		return false;
	}
	if (In_Air() != false) return false;
	return true;
}

// TEMPLATE: GAMEMD 0x0053C4C0
// ?Clear@?$DynamicVectorClass@PBVAnimClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0053C4F0
// ?ID@?$DynamicVectorClass@PBVAnimClass@@@@UAEHPBQBVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x0053C510
// ?GetItem@?$VectorClass@PBVAnimClass@@@@UBEPBVAnimClass@@H@Z
// TEMPLATE: GAMEMD 0x0053C520
// ??8?$VectorClass@PBVAnimClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0053C560
// ?Resize@?$VectorClass@PBVAnimClass@@@@UAE_NHPBQBVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x0053C610
// ?Clear@?$VectorClass@PBVAnimClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0053C640
// ?ID@?$VectorClass@PBVAnimClass@@@@UAEHABQBVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x0053C680
// ?ID@?$VectorClass@PBVAnimClass@@@@UAEHPBQBVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x0053C6A0
// ?Resize@?$DynamicVectorClass@PBVAnimClass@@@@UAE_NHPBQBVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x0053C750
// ?ID@?$DynamicVectorClass@PBVAnimClass@@@@UAEHABQBVAnimClass@@@Z
// SYNTHETIC: GAMEMD 0x0053C780
// ??_G?$VectorClass@PBVAnimClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0053C7D0
// ??_G?$DynamicVectorClass@PBVAnimClass@@@@UAEPAXI@Z
template class DynamicVectorClass<AnimClass const *>;
template class VectorClass<AnimClass const *>;

#include <math.h>


// SYNTHETIC: GAMEMD 0x0053C820 SYMBOL
// _$E158
// SYNTHETIC: GAMEMD 0x0053C850 SYMBOL
// _$E161
// SYNTHETIC: GAMEMD 0x0053C870 SYMBOL
// _$E164
// SYNTHETIC: GAMEMD 0x0053C890 SYMBOL
// _$E167
// SYNTHETIC: GAMEMD 0x0053C8B0 SYMBOL
// _$E170
// SYNTHETIC: GAMEMD 0x0053C8D0 SYMBOL
// _$E173
// SYNTHETIC: GAMEMD 0x0053C8F0 SYMBOL
// _$E176
// SYNTHETIC: GAMEMD 0x0053C930 SYMBOL
// _$E179
// SYNTHETIC: GAMEMD 0x0053C960 SYMBOL
// _$E182
// SYNTHETIC: GAMEMD 0x0053C990 SYMBOL
// _$E185
// SYNTHETIC: GAMEMD 0x0053C9C0 SYMBOL
// _$E188
// SYNTHETIC: GAMEMD 0x0053C9D0 SYMBOL
// _$E191
// SYNTHETIC: GAMEMD 0x0053C9F0 SYMBOL
// _$E194

// SYNTHETIC: GAMEMD 0x0053CA10 SYMBOL
// _$E197
const Rect RECT_NONE_2(0, 0, 0, 0);

