// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"

#include "action.hh"
#include "overlay.hh"
#include "super.hh"
#include "vox.hh"

class BuildingTypeClass;
class SuperWeaponTypeClass;
class ShapeSet;
class WeaponTypeClass;

class SuperWeaponTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		SuperWeaponTypeClass(char const * ininame = NULL);
		SuperWeaponTypeClass(NoInitClass const & x);
		virtual ~SuperWeaponTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override;

		virtual bool Read_INI(CCINIClass & ini) override;

		virtual ActionType What_Action(Cell const & cell, ObjectClass * object);

		static SuperWeaponTypeClass * Find_Or_Make(const char * name);
		static SuperWeaponTypeClass * From_Action(ActionType action);
		static SuperWeaponType From_Name(char const * name);

	public:
		SuperWeaponType HeapID;
		WeaponTypeClass const * Weapon;
		VoxType VoxRecharge;
		VoxType VoxCharging;
		VoxType VoxImpatient;
		VoxType VoxSuspend;
		int RechargeTime;
		SuperWeaponType Type;
		ShapeSet const * CameoData;
		ActionType Action;
		int SpecialSound;
		int StartSound;
		BuildingTypeClass const * AuxBuilding;
		TStringID<24> SidebarImage;
		bool UseChargeDrain;
		bool IsPowered;
		bool DisableableFromShell;
		int FlashSidebarTabFrames;
		bool AIDefendAgainst;
		bool PreClick;
		bool PostClick;
		int PreDependent;
		bool ShowTimer;
		bool IsManualControl;
		float Range;
		int LineMultiplier;
};

SuperWeaponType Special_From_Name(char const * name);
