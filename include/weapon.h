// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "rgb.h"
#include "typelist.h"

#include "armor.hh"
#include "mph.hh"
#include "threat.hh"
#include "weapon.hh"

class ParticleSystemTypeClass;
class AnimTypeClass;
class WarheadTypeClass;
class BulletTypeClass;

class WeaponTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		WeaponTypeClass(char const * ininame = NULL);
		WeaponTypeClass(NoInitClass const &);
		virtual ~WeaponTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static WeaponType From_Name(char const * name);

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_WEAPONTYPE);}
		virtual int Fetch_Object_Size(void) const override;

		virtual void Compute_CRC(CRCEngine &) const override;

		static WeaponTypeClass *Find_Or_Make(const char * name);

		char const * Name(void) const {return(IniName);}
		virtual bool Read_INI(CCINIClass & ini) override;
		ThreatType Allowed_Threats(void) const;
		bool Is_Wall_Destroyer(void) const;

		void Init_Max_Speed(void);

		int AmbientDamage;
		int Burst;
		BulletTypeClass const * Bullet;
		int Attack;
		MPHType MaxSpeed;
		WarheadTypeClass const * WarheadPtr;
		int ROF;
		LEPTON Range;
		LEPTON MinimumRange;
		TypeList<int> Sound;
		TypeList<int> DownReport;
		TypeList<AnimTypeClass const *> Anim;
		AnimTypeClass * OccupantAnim;
		AnimTypeClass * AssaultAnim;
		AnimTypeClass * OpenToppedAnim;
		ParticleSystemTypeClass *AttachedParticleSystem;
		RGBClass LaserInnerColor;
		RGBClass LaserOuterColor;
		RGBClass LaserOuterSpread;
		bool UseFireParticles;
		bool UseSparkParticles;
		bool OmniFire;
		bool DistributedWeaponFire;
		bool IsRailgun;
		bool IsLobber;
		bool IsBright;
		bool IsSonic;
		bool Spawner;
		bool LimboLaunch;
		bool DecloakToFire;
		bool CellRangefinding;
		bool FireOnce;
		bool NeverUse;
		bool RevealOnFire;
		bool TerrainFire;
		bool SabotageCursor;
		bool MigAttackCursor;
		bool DisguiseFireOnly;
		int DisguiseFakeBlinkTime;
		bool InfiniteMindControl;
		bool FireWhileMoving;
		bool DrainWeapon;
		bool FireInTransport;
		bool Suicide;
		bool IsTurboBoosted;
		bool IsSupressed;
		bool IsCamera;
		bool IsElectric;
		bool IsLaser;
		bool DiskLaser;
		bool IsLine;
		bool IsBigLaser;
		bool IsHouseColor;
		char LaserDuration;
		bool IsIonSensitive;
		bool AreaFire;
		bool IsElectricBolt;
		bool DrawBoltAsLaser;
		bool IsAlternateColor;
		bool IsRadBeam;
		bool IsRadEruption;
		int RadLevel;
		bool IsMagBeam;
};

ArmorType Armor_From_Name(char const * name);
