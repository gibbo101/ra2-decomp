// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "facing.h"
#include "fuse.h"
#include "object.h"
#include "velocity.h"

#include "draw.hh"

class BulletTypeClass;
class TechnoClass;
class WeaponTypeClass;
class AnimClass;
struct VoxelDataStruct;
class Matrix3D;

class BulletClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

	public:

		BulletTypeClass * Class;

	private:

		TechnoClass * Payback;

	public:

		BulletClass(void);
		BulletClass(NoInitClass const & x) : BASECLASS(x), Fuse(x) {};
		virtual ~BulletClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int Fetch_Object_Size(void) const override;
		virtual RTTIType Fetch_RTTI(void) const override;

		void Construct(BulletTypeClass const *type, AbstractClass *target, TechnoClass *payback, int strength, WarheadTypeClass const *warhead, int max_speed, bool bright);
		void LoseTarget(void);
		bool Is_Forced_To_Explode(Coord & coord) const;
		void Set_Weapon_Type(WeaponTypeClass * weapon);
		void Bullet_Explodes(bool forced);
		virtual int Shape_Number(void) const;
		virtual void Assign_Target(AbstractClass * target);
		virtual bool Unlimbo(Coord const & coord, TVelocity3D<double> const & velocity);
		virtual LayerType In_Which_Layer(void) const override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual bool Mark(MarkType mark=MARK_CHANGE) override;
		virtual void AI(void) override;
		virtual Cell const * Occupy_List(bool = false) const override;
		bool Is_Homing(void) const;
		void Draw_Voxel(VoxelDataStruct const & voxeldata, Matrix3D const & transform, Point2D const & drawpoint, Rect const & cliprect, int frame, ShapeFlags_Type flags, int brightness) const;
		void Detonate(Coord const & coord);
		void InitScalable(void);

		bool IsInaccurate;

	private:

		FuseClass Fuse;

		bool IsBright;

		int field_E4;

	public:

		TVelocity3D<double> Velocity;

		int BounceCount;

		bool field_A4;

		bool IsLaunching;

		int CourseLockCounter;

		AbstractClass * TarCom;

		int MaxSpeed;

		int InheritedColor;

		int ClosureSamples;

		int field_11C;

		double SmoothedClosure;

		WarheadTypeClass * Warhead;

		unsigned char AnimFrame;

		unsigned char AnimRate;

		WeaponTypeClass * WeaponType;

		Coord SourceCoords;

		Coord TargetCoords;

		Cell LastMapCoords;

		int DamageMultiplier;

		AnimClass * NextAnim;

		bool SpawnNextAnim;

		int Range;

		static DynamicVectorClass<BulletClass *> Array;
		static DynamicVectorClass<BulletClass *> ScalableBullets;

	public:
		friend BulletClass *Create_Bullet(BulletTypeClass const *type, TechnoClass *target, TechnoClass *payback, int strength, WarheadTypeClass const *warhead, int max_speed, int range, bool bright);
};

BulletClass * Create_Bullet(BulletTypeClass const *type, AbstractClass *target, TechnoClass *payback, int strength, WarheadTypeClass const *warhead, int max_speed, int range, bool bright);
