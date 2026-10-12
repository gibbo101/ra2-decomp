// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "super.h"
#include "_map.h"
#include "_rules.h"
#include "_weapon.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "ccrand.h"
#include "cell.h"
#include "crc.h"
#include "data.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ionblast.h"
#include "language\language.h"
#include "anim.h"
#include "mouse.h"
#include "notify.h"
#include "rules.h"
#include "sun.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tracker.h"
#include "unit.h"
#include "vector.h"
#include "vox.h"
#include "weapon.h"

extern Cell DAT_00B0C038[9];
extern wchar_t DAT_00887734[];

static inline void Release_Animation(SuperClass * super)
{
	if (super->Animation != NULL) {
		super->Animation->RemainingIterations = 0;
		super->Animation = NULL;
		PointerExpiredNotification::NotifyInvalidAnim.Delete(super);
	}
	if (super->AnimationGotInvalid) {
		PointerExpiredNotification::NotifyInvalidAnim.Delete(super);
		super->AnimationGotInvalid = false;
	}
}

// FUNCTION: 0x006CABF0 $E2
// FUNCTION: 0x006CAC10 $E5
// FUNCTION: 0x006CAC40 $E8
// FUNCTION: 0x006CAC60 $E11
// FUNCTION: 0x006CAC80 $E14
// FUNCTION: 0x006CACA0 $E17
// FUNCTION: 0x006CACC0 $E20
// FUNCTION: 0x006CACE0 $E23
// FUNCTION: 0x006CAD20 $E26
// FUNCTION: 0x006CAD50 $E29
// FUNCTION: 0x006CAD80 $E32
// FUNCTION: 0x006CADB0 $E35
// FUNCTION: 0x006CADC0 $E38
// FUNCTION: 0x006CADE0 $E41

// FUNCTION: 0x006CAE00
void Init_006CAE00(void)
{
	DAT_00B0C038[0] = Cell(0, 0);
	DAT_00B0C038[1] = Cell(1, 0);
	DAT_00B0C038[2] = Cell(1, -1);
	DAT_00B0C038[3] = Cell(0, -1);
	DAT_00B0C038[4] = Cell(-1, -1);
	DAT_00B0C038[5] = Cell(-1, 0);
	DAT_00B0C038[6] = Cell(-1, 1);
	DAT_00B0C038[7] = Cell(0, 1);
	DAT_00B0C038[8] = Cell(1, 1);
}

// FUNCTION: 0x006CAEC0
SuperClass::SuperClass(void) :
	CustomChargeTime(-1),
	Class(NULL),
	House(NULL),
	Control(0),
	BlinkState(false),
	BlinkTimer(0),
	SpecialSoundDuration(-1),
	NeedsBuilding(true),
	ChronoMapCoords(CELL_NONE),
	Animation(NULL),
	AnimationGotInvalid(false),
	IsPresent(false),
	IsOneTime(false),
	IsReady(false),
	IsSuspended(false),
	ChargeDrainState(SUSPENDED)
{
	UIName = DAT_00887734;
	Array.Add(this);
}

// FUNCTION: 0x006CAF90
SuperClass::SuperClass(SuperWeaponTypeClass * type, HouseClass * owner) :
	CustomChargeTime(-1),
	Class(type),
	House(owner),
	Control(0),
	BlinkState(false),
	BlinkTimer(0),
	SpecialSoundDuration(-1),
	NeedsBuilding(true),
	ChronoMapCoords(CELL_NONE),
	Animation(NULL),
	AnimationGotInvalid(false),
	IsPresent(false),
	IsOneTime(false),
	IsReady(false),
	IsSuspended(false),
	ReadyFrame(-100),
	OldStage(-1),
	ChargeDrainState(SUSPENDED)
{
	Create_ID();
	Array.Add(this);
	PointerExpiredNotification::NotifyInvalidType.Add(this);
	PointerExpiredNotification::NotifyInvalidHouse.Add(this);
}

// FUNCTION: 0x006CB120 SuperClass::~SuperClass
SuperClass::~SuperClass(void)
{
	Release_Animation(this);
	ShowTimers.Delete(this);
	Array.Delete(this);
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
	PointerExpiredNotification::NotifyInvalidHouse.Delete(this);
}

// FUNCTION: 0x006CB2E0
void SuperClass::DetachAnimation(void)
{
	if (Animation != NULL) {
		Animation->RemainingIterations = 0;
		Animation = NULL;
		PointerExpiredNotification::NotifyInvalidAnim.Delete(this);
	}
	if (AnimationGotInvalid) {
		PointerExpiredNotification::NotifyInvalidAnim.Delete(this);
		AnimationGotInvalid = false;
	}
}

// FUNCTION: 0x006CB3A0
void SuperClass::CreateChronoAnim(Coord coords)
{
	Release_Animation(this);
	AnimClass * anim = new AnimClass(Rule->ChronoPlacement, coords + Coord(0, 0, 5), 0, 1, (ShapeFlags_Type)0x600, 0, false);
	if (anim != NULL) {
		Animation = anim;
		PointerExpiredNotification::NotifyInvalidAnim.Add(this);
	}
}

// FUNCTION: 0x006CB4D0
bool SuperClass::SetOnHold(bool on)
{
	if (IsPresent && !IsOneTime && on != IsSuspended && NeedsBuilding) {
		if (!on && !Class->IsManualControl) {
			Control.Start();
		} else {
			Control.Stop();
		}
		IsSuspended = on;
		return(true);
	}
	return(false);
}

static inline bool Begin_Charge(SuperClass * super)
{
	Release_Animation(super);
	if (super->IsPresent && !super->IsReady && (!super->IsSuspended || super->Class->PreClick)) {
		super->OldStage = -1;
		super->Control.Start();
		super->Control = super->GetRechargeTime();
		if (super->Class->UseChargeDrain) super->ChargeDrainState = SuperClass::CHARGING;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006CB560
bool SuperClass::Grant(bool oneTime, bool announce, bool onHold)
{
	if (!IsPresent) {
		IsPresent = true;
		IsOneTime = oneTime;
		BlinkState = false;
		bool charged = false;
		UIName = Class->UIName;
		if (!oneTime) SetOnHold(false);
		if (Class->ShowTimer && !House->Class->IsMultiplayPassive) {
			ShowTimers.Add(this);
		}
		if (Class->IsManualControl) {
			OldStage = -1;
			Control = (CustomChargeTime == -1) ? Class->RechargeTime : CustomChargeTime;
			Control.Stop();
		} else {
			charged = Begin_Charge(this);
		}
		if (onHold) SetOnHold(true);
		if (IsOneTime) Forced_Charge(announce);
		return(charged);
	}
	return(false);
}

// FUNCTION: 0x006CB7B0
bool SuperClass::Lose(void)
{
	if (IsPresent) {
		IsReady = false;
		IsPresent = false;
		ShowTimers.Delete(this);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006CB820
void SuperClass::SetReadiness(bool ready)
{
	IsReady = ready;
}

// FUNCTION: 0x006CB830
char SuperClass::StopPreclickAnim(bool isPlayer)
{
	Release_Animation(this);
	if (IsPresent && !IsReady && (!IsSuspended || Class->PreClick)) {
		OldStage = -1;
		Control.Start();
		Control = (CustomChargeTime == -1) ? Class->RechargeTime : CustomChargeTime;
		if (Class->UseChargeDrain) ChargeDrainState = CHARGING;
		return(1);
	}
	return(0);
}

// FUNCTION: 0x006CDEB0
HRESULT STDMETHODCALLTYPE SuperClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SuperWeaponClass;
	return(S_OK);
}

// FUNCTION: 0x006CDFD0
HRESULT STDMETHODCALLTYPE SuperClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
