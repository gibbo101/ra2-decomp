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
#include "stringtable.h"

extern Cell DAT_00B0C038[9];
extern wchar_t DAT_00887734[];
extern int DAT_008809A0;
extern int DAT_00B0C098;
extern int DAT_00B0C09C;
extern int DAT_00B0C0A0;
extern int DAT_00B0C0A4;

class LightningStorm
{
	public:
		static bool HasDeferment(void);
		static void PrintMessage(void);
};

class PsyDom
{
	public:
		static bool Active(void);
		static void PrintMessage(void);
};

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

// SYNTHETIC: GAMEMD 0x006CABF0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006CAC10 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006CAC40 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006CAC60 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006CAC80 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006CACA0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006CACC0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006CACE0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006CAD20 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006CAD50 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006CAD80 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006CADB0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006CADC0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006CADE0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x006CAE00
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

// FUNCTION: GAMEMD 0x006CAEC0
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

// FUNCTION: GAMEMD 0x006CAF90
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

// FUNCTION: GAMEMD 0x006CB120
// SuperClass::~SuperClass
SuperClass::~SuperClass(void)
{
	Release_Animation(this);
	ShowTimers.Delete(this);
	Array.Delete(this);
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
	PointerExpiredNotification::NotifyInvalidHouse.Delete(this);
}

// FUNCTION: GAMEMD 0x006CB2E0
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

// FUNCTION: GAMEMD 0x006CB3A0
void SuperClass::CreateChronoAnim(Coord coords)
{
	Release_Animation(this);
	AnimClass * anim = new AnimClass(Rule->ChronoPlacement, coords + Coord(0, 0, 5), 0, 1, (ShapeFlags_Type)0x600, 0, false);
	if (anim != NULL) {
		Animation = anim;
		PointerExpiredNotification::NotifyInvalidAnim.Add(this);
	}
}

// FUNCTION: GAMEMD 0x006CB4D0
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

// FUNCTION: GAMEMD 0x006CB560
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

// FUNCTION: GAMEMD 0x006CB7B0
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

// FUNCTION: GAMEMD 0x006CB820
void SuperClass::SetReadiness(bool ready)
{
	IsReady = ready;
}

// FUNCTION: GAMEMD 0x006CB830
char SuperClass::StopPreclickAnim(bool isPlayer)
{
	DetachAnimation();
	if (IsPresent && !IsReady && (!IsSuspended || Class->PreClick)) {
		OldStage = -1;
		Control.Start();
		Control = (CustomChargeTime == -1) ? Class->RechargeTime : CustomChargeTime;
		if (Class->UseChargeDrain) ChargeDrainState = CHARGING;
		return(1);
	}
	return(0);
}

static inline void Restart_Charge(SuperClass * super)
{
	Release_Animation(super);
	if (super->IsPresent && !super->IsReady && (!super->IsSuspended || super->Class->PreClick)) {
		super->OldStage = -1;
		super->Control.Start();
		super->Control = (super->CustomChargeTime == -1) ? super->Class->RechargeTime : super->CustomChargeTime;
		if (super->Class->UseChargeDrain) super->ChargeDrainState = SuperClass::CHARGING;
	}
}

static inline int Charge_Time(SuperClass const * super)
{
	return((super->CustomChargeTime != -1) ? super->CustomChargeTime : super->Class->RechargeTime);
}

// FUNCTION: GAMEMD 0x006CB920
bool SuperClass::Discharged(bool player, Cell const & cell)
{
	if (Class->UseChargeDrain) {
		if (House->FUN_0050B730()) {
			if (ChargeDrainState == FIRESTORM_ON) {
				ChargeDrainState = READY;
				Control = Charge_Time(this) - Control.Value() / Rule->ChargeToDrainRatio;
				return(false);
			}
			if (ChargeDrainState == READY) {
				ChargeDrainState = FIRESTORM_ON;
				Control = (Charge_Time(this) - Control.Value()) * Rule->ChargeToDrainRatio;
				Launch(cell, player);
				return(false);
			}
		} else {
			if (!House->FirestormDefenseActivated) {
				Launch(cell, player);
			}
		}
		return(false);
	}

	if (!(Control.Is_Active() && IsPresent && IsReady)) {
		if (!Class->PostClick) return(false);
	}
	if (Class->Type == SUPER_LIGHTNING_STORM && LightningStorm::HasDeferment()) {
		if (player) LightningStorm::PrintMessage();
		return(false);
	}
	if (Class->Type == SUPER_PSYCHIC_DOMINATOR && PsyDom::Active()) {
		if (player) PsyDom::PrintMessage();
		return(false);
	}
	int chargeLeft = Control.Value();
	Launch(cell, player);
	if (!Class->PostClick) IsReady = false;
	if (IsOneTime) {
		IsOneTime = false;
		return(Lose());
	}
	if (Class->IsManualControl) {
		Control = (CustomChargeTime == -1) ? Class->RechargeTime : CustomChargeTime;
		OldStage = -1;
		Control.Stop();
		return(false);
	}
	if (!Class->PreClick && !Class->PostClick) {
		Restart_Charge(this);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006CBCA0
bool SuperClass::AI(bool player)
{
	if (SpecialSoundDuration > 0) SpecialSoundDuration--;
	if (SpecialSoundDuration == 0) {
		SpecialSoundDuration = -1;
		VocClass::PlayAt(Class->SpecialSound, *(Coord *)SpecialSoundLocation, NULL);
	}
	if (DAT_008809A0 != 4 && Animation != NULL) {
		Animation->Invisible = true;
		Animation->StartSoundAudioController.End();
	}
	if (IsPresent && (!IsReady || Class->UseChargeDrain) && !IsSuspended) {
		if (!Control.Is_Active()) {
			if (OldStage != -1) {
				OldStage = -1;
				return(true);
			}
		} else {
			if (Control == 0) {
				if (Class->UseChargeDrain) {
					if (ChargeDrainState == FIRESTORM_ON) {
						ChargeDrainState = CHARGING;
						Control = (CustomChargeTime == -1) ? Class->RechargeTime : CustomChargeTime;
					} else {
						ChargeDrainState = READY;
						IsReady = true;
					}
					return(true);
				}
				IsReady = true;
				if (player) {
					switch (Class->Type) {
					case SUPER_MULTI_MISSILE:
						VoxClass::Play("EVA_NuclearMissileReady");
						break;
					case SUPER_IRON_CURTAIN:
						VoxClass::Play("EVA_IronCurtainReady");
						break;
					case SUPER_FORCE_SHIELD:
						VoxClass::Play("EVA_ForceShieldReady");
						break;
					case SUPER_LIGHTNING_STORM:
						VoxClass::Play("EVA_LightningStormReady");
						break;
					case SUPER_PSYCHIC_DOMINATOR:
						VoxClass::Play("EVA_PsychicDominatorReady");
						break;
					case SUPER_CHRONO_SPHERE:
						VoxClass::Play("EVA_ChronosphereReady");
						break;
					case SUPER_PARA_DROP:
					case SUPER_AMER_PARA_DROP:
						VoxClass::Play("EVA_ReinforcementsReady");
						break;
					case SUPER_SPY_PLANE:
						VoxClass::Play("EVA_SpyPlaneReady");
						break;
					case SUPER_GENETIC_CONVERTER:
						VoxClass::Play("EVA_GeneticMutatorReady");
						break;
					case SUPER_PSYCHIC_REVEAL:
						VoxClass::Play("EVA_PsychicRevealReady");
						break;
					default:
						break;
			}
				}
				ReadyFrame = Frame;
				return(true);
			} else {
				if (Anim_Stage() != OldStage) {
					OldStage = Anim_Stage();
					return(true);
				}
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006CBEE0
int SuperClass::Anim_Stage(void) const
{
	if (IsPresent) {
		int stage;
		if (Class->UseChargeDrain) {
			if (ChargeDrainState == FIRESTORM_ON) {
				stage = ANIMATION_STAGES * (1 - double(Charge_Time(this) * Rule->ChargeToDrainRatio - Control.Value()) / (Charge_Time(this) * Rule->ChargeToDrainRatio));
			} else {
				stage = ANIMATION_STAGES * (double(Charge_Time(this) - Control.Value()) / Charge_Time(this));
			}
		} else {
			if (IsReady) {
				return(ANIMATION_STAGES);
			}
			stage = ANIMATION_STAGES * (double(Charge_Time(this) - Control.Value()) / Charge_Time(this));
		}
		stage = MIN(stage, ANIMATION_STAGES-1);
		return(stage);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x006CC070
void FUN_006CC070(void)
{
}

// FUNCTION: GAMEMD 0x006CC080
void SuperClass::Forced_Charge(bool player)
{
	if (IsPresent) {
		IsReady = true;
		int time = (CustomChargeTime == -1) ? Class->RechargeTime : CustomChargeTime;
		int remaining = time - int(time * 1.0f);
		if (remaining == 0) IsReady = true;
		Control.Start();
		Control = remaining;
		if (player) {
			switch (Class->Type) {
			case SUPER_MULTI_MISSILE:
				VoxClass::Play("EVA_NuclearMissileReady");
				break;
			case SUPER_IRON_CURTAIN:
				VoxClass::Play("EVA_IronCurtainReady");
				break;
			case SUPER_FORCE_SHIELD:
				VoxClass::Play("EVA_ForceShieldReady");
				break;
			case SUPER_LIGHTNING_STORM:
				VoxClass::Play("EVA_LightningStormReady");
				break;
			case SUPER_PSYCHIC_DOMINATOR:
				VoxClass::Play("EVA_PsychicDominatorReady");
				break;
			case SUPER_CHRONO_SPHERE:
				VoxClass::Play("EVA_ChronosphereReady");
				break;
			case SUPER_PARA_DROP:
			case SUPER_AMER_PARA_DROP:
				VoxClass::Play("EVA_ReinforcementsReady");
				break;
			case SUPER_SPY_PLANE:
				VoxClass::Play("EVA_SpyPlaneReady");
				break;
			case SUPER_GENETIC_CONVERTER:
				VoxClass::Play("EVA_GeneticMutatorReady");
				break;
			case SUPER_PSYCHIC_REVEAL:
				VoxClass::Play("EVA_PsychicRevealReady");
				break;
			default:
				break;
			}
		}
		ReadyFrame = Frame;
		if (Class->UseChargeDrain) ChargeDrainState = READY;
	}
}

// FUNCTION: GAMEMD 0x006CC1E0
void SuperClass::SetCharge(int percentage)
{
	if (IsPresent && percentage >= 0 && percentage <= 100) {
		int time = (CustomChargeTime == -1) ? Class->RechargeTime : CustomChargeTime;
		int remaining = time - int(time * (percentage * 0.01f));
		if (remaining == 0) IsReady = true;
		Control.Start();
		Control = remaining;
	}
}

// FUNCTION: GAMEMD 0x006CC260
int SuperClass::GetRechargeTime(void)
{
	int time = CustomChargeTime;
	if (time != -1) goto done;
	time = Class->RechargeTime;
done:
	return(time);
}

// FUNCTION: GAMEMD 0x006CC280
void SuperClass::SetRechargeTime(int time)
{
	CustomChargeTime = time;
}

// FUNCTION: GAMEMD 0x006CC290
void SuperClass::ResetRechargeTime(void)
{
	CustomChargeTime = -1;
}

// FUNCTION: GAMEMD 0x006CC2A0
bool SuperClass::Is_Powered(void) const
{
	return(Class->IsPowered);
}


// FUNCTION: GAMEMD 0x006CC2B0
wchar_t const * SuperClass::NameReadiness(void) const
{
	if (!IsSuspended) {
		if (Class->UseChargeDrain) {
			switch (ChargeDrainState) {
				case CHARGING:
#line 919 "D:\\ra2mdpost\\Super.CPP"
					return(StringTable::LoadString("TXT_CHARGING", NULL, __FILE__, __LINE__));
				case READY:
#line 922 "D:\\ra2mdpost\\Super.CPP"
					return(StringTable::LoadString("TXT_READY", NULL, __FILE__, __LINE__));
				case FIRESTORM_ON:
#line 925 "D:\\ra2mdpost\\Super.CPP"
					return(StringTable::LoadString("TXT_FIRESTORM_ON", NULL, __FILE__, __LINE__));
				default:
					return(NULL);
			}
		}
		if (IsReady) {
#line 944 "D:\\ra2mdpost\\Super.CPP"
			return(StringTable::LoadString("TXT_READY", NULL, __FILE__, __LINE__));
		}
		return(NULL);
	}
#line 950 "D:\\ra2mdpost\\Super.CPP"
	return(StringTable::LoadString("TXT_HOLD", NULL, __FILE__, __LINE__));
}

// FUNCTION: GAMEMD 0x006CC360
char SuperClass::CanFire(void) const
{
	if (!IsSuspended) {
		if (Class->UseChargeDrain) {
			bool b = ChargeDrainState != CHARGING;
			return(b);
		} else {
			return(IsReady);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006CDE80
void __stdcall FUN_006CDE80(int, int)
{
}

// FUNCTION: GAMEMD 0x006CDE90
bool SuperClass::ShouldDrawProgress(void) const
{
	if (Class->UseChargeDrain) return(true);
	return(!IsReady);
}

// FUNCTION: GAMEMD 0x006CDEB0
HRESULT STDMETHODCALLTYPE SuperClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SuperWeaponClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006CDEF0
HRESULT STDMETHODCALLTYPE SuperClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) SuperClass(NoInitClass());
		if (Animation != NULL) PointerExpiredNotification::NotifyInvalidAnim.Add(this);
		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&House);
		Swizzle_Pointer(&Animation);
		BlinkTimer = 0;
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006CDFD0
HRESULT STDMETHODCALLTYPE SuperClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006CDFF0
void SuperClass::Detach(AbstractClass const * target, bool all)
{
	if (target == House) House = NULL;
	if (target == Class) Class = NULL;
	if (target == Animation) {
		Animation = NULL;
		AnimationGotInvalid = true;
	}
}

// FUNCTION: GAMEMD 0x006CE020
void SuperClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(IsPresent);
	crc(IsOneTime);
	crc(IsReady);
	crc(OldStage);
	crc(ChargeDrainState);
	crc(NeedsBuilding);
	crc(AnimationGotInvalid);
	if (Animation != NULL) {
		crc(Animation->RemainingIterations);
		crc(Animation->Fetch_ID());
	}
}

// FUNCTION: GAMEMD 0x006CE0B0
void SuperClass::Reset(void)
{
	Release_Animation(this);
	IsReady = false;
	OldStage = -1;
	bool active = Control.Is_Active();
	Control = Charge_Time(this);
	if (!active) Control.Stop();
}

// FUNCTION: GAMEMD 0x006CE1A0
bool SuperClass::ShouldFlashTab(void) const
{
	if (IsSuspended) goto done;
	if (Class->UseChargeDrain) {
		if (ChargeDrainState == CHARGING) goto done;
	} else {
		if (!IsReady) goto done;
	}
	{
		int duration = Class->FlashSidebarTabFrames;
		if (duration == -1) return(true);
		if (duration != 0 && ReadyFrame + duration > Frame) return(true);
	}
done:
	return(false);
}

// FUNCTION: GAMEMD 0x006CE1F0
// SuperClass::What_Am_I
// FUNCTION: GAMEMD 0x006CE200
// SuperClass::Fetch_RTTI
// FUNCTION: GAMEMD 0x006CE210
// SuperClass::Fetch_Object_Size

// TEMPLATE: GAMEMD 0x006CE240
// ??H?$TPoint3D@H@@QBE?BV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x006CE280
// ?Stop@?$CDTimerClass@VFrameTimerClass@@@@QAEXXZ
// TEMPLATE: GAMEMD 0x006CE2C0
// ?Start@?$CDTimerClass@VFrameTimerClass@@@@QAEXXZ
// TEMPLATE: GAMEMD 0x006CE2D0
// ?Delete@?$DynamicVectorClass@PAVSuperClass@@@@QAE_NABQAVSuperClass@@@Z
// TEMPLATE: GAMEMD 0x006CE320
// ?Delete_Index@?$DynamicVectorClass@PAVSuperClass@@@@QAE_NH@Z
template class DynamicVectorClass<SuperClass *>;
template class TPoint3D<int>;

// FUNCTION: GAMEMD 0x006CE360
void FUN_006CE360(void)
{
	DAT_00B0C098 = 0;
	DAT_00B0C09C = 0;
	DAT_00B0C0A0 = 0;
	DAT_00B0C0A4 = 0;
}
