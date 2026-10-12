// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "anim.h"
#include "_bench.h"
#include "_convert.h"
#include "_map.h"
#include "_palette.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "animtype.h"
#include "bench.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "crc.h"
#include "draw.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "lightcon.h"
#include "notify.h"
#include "overlay.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "setpal.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tiberium.h"
#include "tracker.h"
#include "bench.hh"
#include "fastmath.h"
#include "coord.h"
#include <math.h>

extern DynamicVectorClass<AnimClass *> DAT_00A83E00;
extern const double DAT_007E3308;
extern const double DAT_007E3310;
extern double DAT_0089A158;
extern double DAT_0089A1B8;
extern double DAT_0089A1A0;
extern double DAT_0089A198;
extern double DAT_0089A168;
extern double DAT_0089A138;
extern int DAT_0089A1C0;
extern double DAT_0089A170;
extern double DAT_0089A188;
extern int DAT_0089A1B4;

// FUNCTION: 0x00421CB0



double DAT_0089A158 = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);

// FUNCTION: 0x00421CE0



double DAT_0089A1B8 = DAT_0089A158 / DAT_007E3308;

// FUNCTION: 0x00421D00



double DAT_0089A1A0 = 1.0 / DAT_0089A1B8;

// FUNCTION: 0x00421D20



double DAT_0089A198 = DAT_007E3310 * 45;

// FUNCTION: 0x00421D40



double DAT_0089A168 = DAT_007E3310 * 60;

// FUNCTION: 0x00421D60



double DAT_0089A138 = DAT_007E3310 * 90;

// FUNCTION: 0x00421D80



int DAT_0089A1C0 = (int)(fastmath::tan(DAT_0089A138 - DAT_0089A168) * DAT_0089A158 * 0.5);

// FUNCTION: 0x00421DC0



double DAT_0089A170 = fastmath::atan(DAT_0089A1C0 * (1 / (double)CELL_LEPTON_H));

// FUNCTION: 0x00421DF0



double DAT_0089A188 = fastmath::atan((DAT_0089A1C0 * 2) / DAT_0089A158);

// FUNCTION: 0x00421E20



int DAT_0089A1B4 = (int)((DAT_0089A1C0 * 4) + 0.5);

// FUNCTION: 0x00421E50



Cell const DAT_0089A150(0, 0);

// FUNCTION: 0x00421E60



Coord const DAT_0089A178(0, 0, 0);

// FUNCTION: 0x00421E80



Coord const DAT_0089A1A8(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0);

// FUNCTION: 0x00422720
AnimClass::AnimClass(void) :
	BASECLASS(),
	Animation(),
	Type(NULL),
	OwnerObject(NULL),
	unknown_D0(-1),
	LightConvert(NULL),
	LightConvertIndex(-1),
	TintColor(1000),
	ZAdjust(0),
	YSortAdjust(0),
	IsBuildingAnim(false),
	UnderTemporal(false),
	Paused(false),
	Unpaused(false),
	PausedAnimFrame(0),
	Reverse(false),
	Bounce(),
	TimeToDie(false),
	LoopDelay(0),
	Accum(0.0),
	HasExtras(false),
	RemainingIterations(1),
	UseCellLightConvert(false),
	DeleteOnMapCleanup(false),
	UnableToContinue(false),
	SkipProcessOnce(true),
	Invisible(false),
	PowerOff(false)
{
	Array.Add(this);
	StartSoundAudioController.FUN_00405BE0();
	StopSoundAudioController.FUN_00405BE0();
	IsActive = true;
}

bool AnimClass::Limbo(void)
{
	if (Type != NULL && Type->IsVeins) {
		Map[(Coord const &)PositionCoord].IsAnimAttached = false;
	}

	if (BASECLASS::Limbo()) {
		Map.Remove(this);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004228E0
AnimClass::~AnimClass(void)
{
	Detach_This_From_All(this, true);
	if (GameActive) {
		if (OwnerObject != NULL) {
			ObjectClass * to = OwnerObject;

			int index;
			for (index = 0; index < Array.Count(); index++) {
				if (Array[index] != this && Array[index]->OwnerObject == OwnerObject) break;
			}

			if (index == Array.Count()) {
				to->Fire_Out();
				to->IsAnimAttached = false;
			}
			OwnerObject = NULL;
		}
		AnimClass::Limbo();
	}

	PointerExpiredNotification::NotifyInvalidType.Delete(this);

	if (Type != NULL) {
		Type->Free_Image();
	}

	StartSoundAudioController.EndLooping();
	StopSoundAudioController.EndLooping();
	StartSoundAudioController.FUN_00405C00();
	StopSoundAudioController.FUN_00405C00();

	OwnerObject = NULL;
	Type = NULL;

	if (Fetch_ID() == -2) {
		DAT_00A83E00.Delete(this);
	} else {
		Array.Delete(this);
	}
}

// FUNCTION: 0x00422B20
AnimType Anim_From_Name(char const * name)
{
	if (name == NULL) return(ANIM_NONE);

	if (strcmpi(name, "<none>") == 0) return(ANIM_NONE);

	for (AnimType anim = ANIM_FIRST; anim < AnimTypes.Count(); anim++) {
		if (stricmp(AnimTypes[anim]->IniName, name) == 0) {
			return(anim);
		}
	}
	return(ANIM_NONE);
}

// FUNCTION: 0x00422B80
void Shorten_Attached_Anims(ObjectClass * obj)
{
	if (obj != NULL) {
		for (int index = 0; index < AnimClass::Array.Count(); index++) {
			AnimClass * anim = AnimClass::Array[index];

			if (anim->OwnerObject == obj) {
				anim->RemainingIterations = 0;
			}
		}
	}
}

// FUNCTION: 0x00422BC0
int AnimClass::Sort_Y(void) const
{
	return(BASECLASS::Sort_Y() + YSortAdjust);
}

// FUNCTION: 0x00422BE0
Coord AnimClass::Center_Coord(void) const
{
	if (OwnerObject != NULL) {
		return(OwnerObject->Center_Coord() + BASECLASS::Center_Coord());
	}
	return(BASECLASS::Center_Coord());
}

// FUNCTION: 0x00422C70
bool AnimClass::Render(Rect & rect, bool forced, bool extras_only) const
{
	if (LoopDelay != 0) {
		return(false);
	}
	return(BASECLASS::Render(rect, forced, extras_only));
}

// FUNCTION: 0x004238B0
bool AnimClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004238D0
Cell const * AnimClass::Occupy_List(bool) const
{
	static Cell _simple[] = {REFRESH_EOL};

	return(_simple);
}

// FUNCTION: 0x00423920 $E141

// FUNCTION: 0x00423930
BounceResultType AnimClass::Bounce_AI(void)
{
	BounceClass & bounce = Bounce;
	BounceResultType bounce_result = bounce.AI();

	if (Type->IsMeteor) {
		bounce.Velocity.Z += bounce.Gravity;
	}

	switch (bounce_result) {
		case BOUNCE_IMPACT: {
			if (Type->BounceAnim != NULL) {
				new AnimClass(Type->BounceAnim, Center_Coord());
			}
			ObjectClass * optr = Map[bounce.Get_Bounce_Coord()].Cell_Occupier();
			while (optr != NULL) {
				Coord cdiff = bounce.Get_Bounce_Coord() - optr->Center_Coord();
				int rad = abs(cdiff.X) + abs(cdiff.Y);
				if (rad <= Type->DamageRadius) {
					int damage = (int)Type->Damage;
					optr->Take_Damage(damage, TacticalMap->Z_Lepton_To_Pixel(rad), Type->Warhead);
				}
				optr = optr->Next;
			}
			break;
		}
		case BOUNCE_SETTLED:
			Delete_Me();
			break;
	}

	Set_Coord(bounce.Get_Bounce_Coord());

	return(bounce_result);
}

// FUNCTION: 0x00424B50
void AnimClass::SetOwnerObject(ObjectClass * owner)
{
	if (OwnerObject != NULL) {
		ObjectClass * to = OwnerObject;

		bool down = IsDown;
		if (down) {
			Map.Remove(this);
		}

		int index;
		for (index = 0; index < Array.Count(); index++) {
			if (Array[index] != this && Array[index]->OwnerObject == OwnerObject) break;
		}

		if (index == Array.Count()) {
			to->Fire_Out();
			to->IsAnimAttached = false;
		}

		Coord center = Center_Coord();
		OwnerObject = NULL;

		Set_Coord(center);

		if (down) {
			Map.Submit(this);
		}
	}

	if (owner == NULL) return;

	Coord center = Center_Coord();
	Map.Remove(this);
	owner->IsAnimAttached = true;
	OwnerObject = owner;
	Set_Coord(center - owner->Center_Coord());
	Map.Submit(this);
}

// FUNCTION: 0x00424C90
void AnimClass::FUN_00424C90(BulletClass * bullet)
{
	AttachedBullet = bullet;
}

// FUNCTION: 0x00424CA0
void AnimClass::FUN_00424CA0(HouseClass * house)
{
	Owner = house;
}

// FUNCTION: 0x00426540
HRESULT STDMETHODCALLTYPE AnimClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AnimClass;
	return(S_OK);
}
