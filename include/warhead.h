// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "typelist.h"

#include "armor.hh"

class AnimTypeClass;
class ParticleSystemTypeClass;
class VoxelAnimTypeClass;

class WarheadTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		WarheadTypeClass(char const * ininame = NULL);
		WarheadTypeClass(NoInitClass const & x);
		virtual ~WarheadTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(bool oldsave) const override;
		virtual void Compute_CRC(CRCEngine &) const override;

		virtual bool Read_INI(CCINIClass & ini) override;

		static WarheadTypeClass *Find_Or_Make(const char *name);
		static WarheadTypeClass *From_Name(char const * name);

		double Deform;
		double Modifier[ARMOR_COUNT];
		double ProneDamage;
		int DeformThreshhold;
		TypeList<AnimTypeClass const *> ExplosionSet;
		int InfantryDeath;
		float SpreadFactor;
		float CellInset;
		float PercentAtMax;
		bool CausesDelayKill;
		int DelayKillFrames;
		float DelayKillAtMax;
		float CombatLightSize;
		ParticleSystemTypeClass *Particle;
		bool IsWallDestroyer;
		bool WallAbsoluteDestroyer;
		bool PenetratesBunker;
		bool IsWoodDestroyer;
		bool IsTiberiumDestroyer;
		bool IsOrganic;
		bool IsSparky;
		bool Sonic;
		bool IsFire;
		bool IsConventional;
		bool IsRocker;
		bool DirectRocker;
		bool IsBright;
		bool CLDisableRed;
		bool CLDisableGreen;
		bool CLDisableBlue;
		bool IsEMEffect;
		bool MindControl;
		bool Poison;
		bool IvanBomb;
		bool ElectricAssault;
		bool Parasite;
		bool Temporal;
		bool IsLocomotor;
		CLSID Locomotor;
		bool Airstrike;
		bool Psychedelic;
		bool BombDisarm;
		int Paralyzes;
		bool Culling;
		bool MakesDisguise;
		bool NukeMaker;
		bool Radiation;
		bool PsychicDamage;
		bool AffectsAllies;
		bool Bullets;
		bool IsVeinhole;
		int ShakeXlo;
		int ShakeXhi;
		int ShakeYlo;
		int ShakeYhi;
		TypeList<VoxelAnimTypeClass *> DebrisTypes;
		TypeList<int> DebrisMaximums;
		int MaxDebris;
		int MinDebris;
};
