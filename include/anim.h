// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "animtype.h"
#include "audioctl.h"
#include "bounce.h"
#include "coord.h"
#include "object.h"
#include "stage.h"
#include "vector.h"

#include "draw.hh"
#include "layer.hh"

class ConvertClass;
class LightConvertClass;
class BulletClass;
class HouseClass;

class AnimClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:
		StageClass Animation;
		AnimTypeClass * Type;
		ObjectClass * OwnerObject;
		unsigned int unknown_D0;
		LightConvertClass * LightConvert;
		int LightConvertIndex;
		char PaletteName[32];
		int TintColor;
		int ZAdjust;
		int YSortAdjust;
		Coord FlamingGuyCoords;
		int FlamingGuyRetries;
		bool IsBuildingAnim;
		bool UnderTemporal;
		bool Paused;
		bool Unpaused;
		int PausedAnimFrame;
		bool Reverse;

		BounceClass Bounce;

		char TranslucencyLevel;
		bool TimeToDie;
		BulletClass * AttachedBullet;
		HouseClass * Owner;
		int LoopDelay;
		double Accum;
		ShapeFlags_Type AnimFlags;
		bool HasExtras;
		unsigned char RemainingIterations;
		bool UseCellLightConvert;
		bool DeleteOnMapCleanup;
		bool IsInert;
		bool IsFogged;
		bool FlamingGuyExpire;
		bool UnableToContinue;
		bool SkipProcessOnce;
		bool Invisible;
		bool PowerOff;
		unsigned char unused_19F;

		AudioController StartSoundAudioController;
		AudioController StopSoundAudioController;

		static DynamicVectorClass<AnimClass *> Array;

		AnimClass(AnimTypeClass const * type, Coord const & coord, int timedelay = 0, int loop = 1, ShapeFlags_Type flags = ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_CENTER), int zadjust = 0, bool reverse = false);
		AnimClass(NoInitClass const & x) : BASECLASS(x), Animation(x), Bounce(x) {};
		AnimClass(void);
		virtual ~AnimClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static void Post_Load_Game(void);

		void SetOwnerObject(ObjectClass * owner);
		void Start(void);
		void Middle(void);
		void Make_Invisible(void) {Invisible = true;};
		void Make_Visible(void) {Invisible = false;};
		static void Do_Atom_Damage(HousesType ownerhouse, Cell const & cell);

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual bool Limbo(void) override;
		virtual void Delete_Me(void) override;
		virtual int Get_Height(void) const override;

		virtual void Set_Occupy_Bit(Coord const & coord) override;
		virtual void Clear_Occupy_Bit(Coord const & coord) override;
		virtual bool Mark(MarkType mark=MARK_CHANGE) override;
		virtual bool Render(Rect & rect, bool forced, bool extras_only = false) const override;
		virtual Coord Center_Coord(void) const override;
		virtual int Sort_Y(void) const override;
		virtual LayerType In_Which_Layer(void) const override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual Cell const * Occupy_List(bool = false) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual void AI(void) override;
		virtual void Detach(AbstractClass const * target, bool all) override;

		void Disable(void);
		void Enable(void);

		void Vein_Attack_AI(void);
		void Flaming_Guy_AI(void);

		Coord Next_Flaming_Guy_Coord(void);
		bool Is_Valid_Flaming_Guy_Cell(Cell const & cell);
		void FUN_00424C90(BulletClass * bullet);
		void FUN_00424CA0(HouseClass * house);

		virtual BounceResultType Bounce_AI(void);
		virtual int Stage_Count(void) const;
};

void Shorten_Attached_Anims(ObjectClass * obj);
AnimType Anim_From_Name(char const * name);
