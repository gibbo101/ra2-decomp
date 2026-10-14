// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_infantr.h"
#include "techtype.h"

#include "infantry.hh"
#include "pip.hh"

class AnimTypeClass;

class InfantryTypeClass : public TechnoTypeClass
{
		typedef TechnoTypeClass BASECLASS;

	public:
		InfantryType HeapID;
		PipEnum Pip;
		PipEnum OccupyPip;
		YRWeaponDataStruct OccupyWeapon;
		YRWeaponDataStruct EliteOccupyWeapon;
		DoInfoStruct const * DoControls;
		int FireLaunch;
		int ProneLaunch;
		int SecondaryFire;
		int SecondaryProne;
		TypeList<AnimTypeClass *> DeadBodies;
		TypeList<AnimTypeClass *> DeathAnims;
		TypeList<int> VoiceComment;
		int EnterWaterSound;
		int LeaveWaterSound;
		bool IsCyborg;
		bool NotHuman;
		bool Ivan;
		int DirectionDistance;
		bool Occupier;
		bool Assaulter;
		int HarvestRate;
		bool IsFearless;
		bool IsCrawling;
		bool IsCapture;
		bool IsFraidyCat;
		bool IsTiberiumProof;
		bool IsCivilian;
		bool IsBomber;
		bool IsEngineer;
		bool IsAgent;
		bool IsThief;
		bool IsVehicleThief;
		bool IsDoggie;
		bool Deployer;
		bool DeployedCrushable;
		bool UseOwnName;
		bool JumpJetTurn;
		unsigned int : 32;

		InfantryTypeClass(NoInitClass const & x);
		InfantryTypeClass(char const * ininame = NULL);
		virtual ~InfantryTypeClass() override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override;

		static InfantryType From_Name(char const * name);
		static void Init(TheaterType);
		static InfantryTypeClass * Find_Or_Make(char const * name);

		void Read_Sequence_INI(void);
		virtual bool Read_INI(CCINIClass & ini) override;
		virtual Coord const Coord_Fixup(Coord const & coord) const override;
		virtual Point3D Lepton_Dimensions(void) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house = NULL) const override;
		virtual ObjectClass * Create_One_Of(HouseClass * house) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;
		virtual bool CanUseWaypoint(void) const override;
		virtual bool CanAttackMove(void) const override;
		virtual int Repair_Cost(void) const override;
		virtual int Repair_Step(void) const override;
};
