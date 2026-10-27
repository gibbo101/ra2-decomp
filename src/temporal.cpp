// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "brain.h"
#include "crc.h"
#include "ftimer.h"
#include "globals.h"
#include "noinit.h"
#define INCLUDE_COM
#include "always.h"
#include "temporal.h"
#include "crc.h"
#include "globals.h"
#include "noinit.h"
#include "swizzle.h"
#include "techno.h"
#include "_rules.h"
#include "anim.h"
#include "foot.h"
#include "building.h"
#include "house.h"
#include "rules.h"
#include "super.h"
#include "slavemgr.h"
#include "techtype.h"
#include "weapon.h"

// SYNTHETIC: GAMEMD 0x0071A1C0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0071A1F0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0071A210 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0071A230 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0071A250 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0071A270 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0071A290 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0071A2D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0071A300 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0071A330 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0071A360 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0071A370 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0071A390 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0071A3B0 SYMBOL
// _$E48

DynamicVectorClass<TemporalClass *> Temporals;

// SYNTHETIC: GAMEMD 0x0071A3D0 SYMBOL
// _$E99
// SYNTHETIC: GAMEMD 0x0071A410 SYMBOL
// _$E97

// FUNCTION: GAMEMD 0x0071A450
TemporalClass::TemporalClass(void) :
	BASECLASS(),
	LifeTimer()
{
	Temporals.Add(this);
}

// FUNCTION: GAMEMD 0x0071A4E0
TemporalClass::TemporalClass(TechnoClass * owner) :
	BASECLASS(),
	LifeTimer()
{
	Owner = owner;
	Target = NULL;
	LifeTimer = CDTimerClass<FrameTimerClass>();
	NextTemporal = NULL;
	PrevTemporal = NULL;
	Temporals.Add(this);
	WarpRemaining = 0;
	WarpPerStep = 0;
	unknown_pointer_38 = NULL;
	SourceSW = NULL;
}

// FUNCTION: GAMEMD 0x0071A5A0
TemporalClass::TemporalClass(NoInitClass const & x) :
	BASECLASS(x),
	LifeTimer(x)
{
}

// FUNCTION: GAMEMD 0x0071A5D0
TemporalClass::~TemporalClass(void)
{
	Temporals.Delete(this);
}

// FUNCTION: GAMEMD 0x0071A650
void TemporalClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: GAMEMD 0x0071A660
HRESULT STDMETHODCALLTYPE TemporalClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TemporalClass(NoInitClass());
		Swizzle_Pointer(&Owner);
		Swizzle_Pointer(&Target);
		Swizzle_Pointer(&NextTemporal);
		Swizzle_Pointer(&PrevTemporal);
		Swizzle_Pointer(&unknown_pointer_38);
		Swizzle_Pointer(&SourceSW);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0071A700
HRESULT STDMETHODCALLTYPE TemporalClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x0071A720
HRESULT STDMETHODCALLTYPE TemporalClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TemporalClass;
	return(S_OK);
}

static inline BuildingClass * Building_Cast(TechnoClass * techno)
{
	if (techno == NULL) {
		return(NULL);
	}
	return((techno->RTTI == RTTI_BUILDING) ? (BuildingClass *)techno : NULL);
}

// FUNCTION: GAMEMD 0x0071A760
void TemporalClass::AI(void)
{
	if (Target != NULL && Target->TemporalTargetingMe == this && NextTemporal != NULL) {
		Target->TemporalTargetingMe = NULL;
		Target->BeingWarpedOut = false;
		Detach();
		return;
	}

	if (Owner != NULL && Owner->InOpenToppedTransport && Target != NULL) {
		if (Owner->Center_Coord().Distance_To(Target->Center_Coord()) > (Rule->OpenToppedWarpDistance << 8)) {
			LetGo();
			return;
		}
	}

	WarpRemaining -= GetWarpPerStep();
	if (WarpRemaining > 0) {
		return;
	}

	if (Target == NULL) {
		Target = NULL;
		PrevTemporal = NULL;
		NextTemporal = NULL;
		SourceSW = NULL;
		unknown_pointer_38 = NULL;
		if (Owner != NULL) {
			Owner->Enter_Idle_Mode(false, true);
		}
	} else {
		new AnimClass(Rule->WarpAway, Coord(Target->Position), 0, 1, ShapeFlags_Type(0x600), 0, false);
	}

	if (Target != NULL) {
		if (Owner->Techno_Type_Class()->IsTrainable) {
			TechnoTypeClass const * target_type = Target->Techno_Type_Class();
			TechnoTypeClass const * owner_type = Owner->Techno_Type_Class();
			Owner->Veterancy.Made_A_Kill(owner_type->Cost_Of(Owner->House), target_type->Cost_Of(Target->House));
		}

		if (Target->RTTI == RTTI_BUILDING) {
			BuildingClass * building = (BuildingClass *)Target;
			if (building != NULL) {
				if (building->GetOccupantCount() > 0) {
					building->KillOccupants(NULL);
				}
				if (!ScenarioInit) {
					while (building->Cargo.Is_Something_Attached()) {
						delete building->Cargo.Detach_Object();
					}
				}
			}
			if (SourceSW != NULL) {
				SourceSW->SetOnHold(false);
				SourceSW = NULL;
			}
			if (Target->BunkerLinkedItem != NULL) {
				Building_Cast(Target)->UnloadBunker();
			}
			if (Target->SlaveManager != NULL) {
				Target->SlaveManager->Killed(Owner, NULL);
			}
			Target->Death_Announcement(Owner);
			Target->RegisterDestruction(Owner);
			Target->Delete_Me();
			building->House->Production_Status_Changed();
		} else {
			if (Target->BunkerLinkedItem != NULL) {
				Building_Cast(Target->BunkerLinkedItem)->ClearBunker();
			}
			if (Target->SlaveManager != NULL) {
				Target->SlaveManager->Killed(Owner, NULL);
			}
			Target->Death_Announcement(Owner);
			Target->RegisterDestruction(Owner);
			Target->Delete_Me();
		}
		if (Owner != NULL) {
			Owner->Enter_Idle_Mode(false, true);
		}
	}

	Target = NULL;
	PrevTemporal = NULL;
	NextTemporal = NULL;
	SourceSW = NULL;
	unknown_pointer_38 = NULL;
	if (Owner != NULL) {
		Owner->Enter_Idle_Mode(false, true);
	}
}

// FUNCTION: GAMEMD 0x0071AB10
int TemporalClass::GetWarpPerStep(int helpercount)
{
	int helpers = 0;
	if (PrevTemporal != NULL && helpercount <= 50) {
		helpers = PrevTemporal->GetWarpPerStep(helpercount + 1);
	}
	WarpPerStep = Owner->Get_Class_Weapon_Data(Owner->What_Weapon_Should_I_Use(NULL))->Weapon->Attack;
	return(WarpPerStep + helpers);
}
