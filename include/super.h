// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "coord.h"
#include "vector.h"
#include "anim.h"
#include "notify.h"
#include "ftimer.h"
#include "timer.h"

class SuperWeaponTypeClass;
class HouseClass;
class AnimClass;

class SuperClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		SuperClass(void);
		SuperClass(SuperWeaponTypeClass * type, HouseClass * owner);
		SuperClass(NoInitClass const & x) : Control(x) {};
		virtual ~SuperClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int STDMETHODCALLTYPE What_Am_I(void) const override {return(SuperClass::Fetch_RTTI());};

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_SUPERWEAPON);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		virtual void Compute_CRC(CRCEngine & crc) const override;

		void Detach(AbstractClass const * target, bool all);

		bool SetOnHold(bool on);
		bool Grant(bool oneTime, bool announce, bool onHold);
		void Forced_Charge(bool player=false);
		void Place(Cell const & cell, bool player);
		void Drop_Pods(Cell const & cell) const;
		void Deactivate_Firestorm(int, bool player) const;
		bool AI(bool player=false);
		bool Lose(void);
		void SetReadiness(bool ready);
		char StopPreclickAnim(bool isPlayer);
		int GetRechargeTime(void);
		void DetachAnimation(void);
		void CreateChronoAnim(Coord coords);
		void Impatient_Click(void) const;
		int Anim_Stage(void) const;
		bool Discharged(bool player, Cell const & cell);
		bool Can_Place(void) const;
		bool Is_Charging(void) const;
		bool Is_Powered(void) const;
		bool Is_Ready(void) const {return(IsReady);}
		bool Is_Present(void) const {return(IsPresent);}
		bool Is_One_Time(void) const {return(IsOneTime && IsPresent);}

		char const * State_String(void) const;

	public:
		static DynamicVectorClass<SuperClass *> Array;
		static DynamicVectorClass<SuperClass *> ShowTimers;

		int CustomChargeTime;

		SuperWeaponTypeClass *Class;

		bool Recharge(bool player=false);

		HouseClass * House;

		CDTimerClass<FrameTimerClass> Control;

		wchar_t const * UIName;

		bool BlinkState;

		unsigned char unused_41[7];

		__int64 BlinkTimer;

		int SpecialSoundDuration;

		int SpecialSoundLocation[3];

		bool NeedsBuilding;

		unsigned char unused_61;

		Cell ChronoMapCoords;

		unsigned char unused_66[2];

		AnimClass * Animation;

		bool AnimationGotInvalid;

	public:
		bool IsPresent;
		bool IsOneTime;
		bool IsReady;
		bool IsSuspended;

		unsigned char unused_71[3];

		int ReadyFrame;

		int OldStage;

		enum ChargeDrainType {
			SUSPENDED = -1,
			CHARGING,
			READY,
			FIRESTORM_ON
		};

		ChargeDrainType ChargeDrainState;

		enum {
			ANIMATION_STAGES=54
		};
};
