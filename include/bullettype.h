// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"

#include "bullet.hh"
#include "scheme.hh"

class WeaponTypeClass;
class AnimTypeClass;
class HouseClass;
class Cell;

class BulletTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:

		bool IsAirburst;
		bool IsFloater;
		bool SubjectToCliffs;
		bool SubjectToElevation;
		bool SubjectToWalls;
		bool IsVeryHigh;
		bool IsShadow;
		bool IsArcing;
		bool IsDropping;
		bool Level;
		bool IsInvisible;
		bool IsProximityArmed;
		bool IsFueled;
		bool NoRotate;
		bool IsInaccurate;
		bool FlakScatter;
		bool IsAntiAircraft;
		bool IsAntiGround;
		bool IsDegenerate;
		bool IsBouncy;
		bool IsAnimPalette;
		bool FirersPalette;
		int Cluster;
		WeaponTypeClass *AirburstWeapon;
		WeaponTypeClass *ShrapnelWeapon;
		int ShrapnelCount;
		int DetonationAltitude;
		bool Vertical;
		double Elasticity;
		int Acceleration;
		int Color;
		AnimTypeClass *Trailer;
		int ROT;
		int CourseLockDuration;
		int SpawnDelay;
		int ScaledSpawnDelay;
		bool Scalable;
		int Arming;
		unsigned char AnimLow;
		unsigned char AnimHigh;
		unsigned char AnimRate;
		bool Flat;

		BulletTypeClass(NoInitClass const & x);
		BulletTypeClass(char const * name = NULL);
		virtual ~BulletTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual void Detach(AbstractClass const * target, bool all = true) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;

		virtual void Compute_CRC(CRCEngine & crc) const override;

		static BulletTypeClass *Find_Or_Make(const char * name);

		static BulletType From_Name(char const * name);
		static const char * Name_From(BulletType bullet);

		static void Init(TheaterType ) {};
		static void One_Time(void) {BASECLASS::One_Time();}

		virtual bool Read_INI(CCINIClass & ini) override;

		virtual Coord const Coord_Fixup(Coord const & coord) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house) const override {return(false);};
		virtual ObjectClass * Create_One_Of(HouseClass * house) const override {return(NULL);};
};
