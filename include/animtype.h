// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"

#include "anim.hh"
#include "layer.hh"

struct AnimRandomRange
{
	int Min;
	int Max;

	AnimRandomRange(void) : Min(0), Max(0) {}
	AnimRandomRange(NoInitClass const &) {}
};

class WarheadTypeClass;
class OverlayTypeClass;

class AnimTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:
		AnimType HeapID;
		int MiddleFrameIndex;
		int MiddleFrameWidth;
		int MiddleFrameHeight;
		bool unknown_2A4;

		double Damage;
		int Rate;
		int Start;
		int LoopStart;
		int LoopEnd;
		int End;
		int LoopCount;
		AnimTypeClass * Next;
		int SpawnsParticle;
		int NumParticles;
		int DetailLevel;
		int TranslucencyDetailLevel;
		AnimRandomRange RandomLoopDelay;
		AnimRandomRange RandomRate;
		int Translucency;
		AnimTypeClass * Spawns;
		int SpawnCount;
		int Report;
		int StopSound;
		AnimTypeClass * BounceAnim;
		AnimTypeClass * ExpireAnim;
		AnimTypeClass * TrailerAnim;
		int TrailerSeperation;
		double Elasticity;
		double MinZVel;
		double MaxZVel;
		double MaxXYVel;
		WarheadTypeClass * Warhead;
		int DamageRadius;
		OverlayTypeClass * TiberiumSpawnType;
		int TiberiumSpreadRadius;
		int YSortAdjust;
		int YDrawOffset;
		int ZAdjust;
		int MakeInfantry;
		int RunningFrames;
		bool IsFlamingGuy;
		bool IsVeins;
		bool IsMeteor;
		bool TiberiumChainReaction;
		bool IsTiberium;
		bool HideIfNoOre;
		bool Bouncer;
		bool Tiled;
		bool ShouldUseCellDrawer;
		bool UseNormalLight;
		bool DemandLoad;
		bool FreeLoad;
		bool IsAnimatedTiberium;
		bool AltPalette;
		bool Normalized;

		LayerType Layer;

		bool DoubleThick;
		bool Flat;
		bool Translucent;
		bool Scorch;
		bool Flamer;
		bool Crater;
		bool ForceBigCraters;
		bool Sticky;
		bool PingPong;
		bool Reverse;
		bool Shadow;
		bool PsiWarning;
		bool ShouldFogRemove;

		AnimTypeClass(char const * ininame = NULL);
		AnimTypeClass(NoInitClass const & x);
		virtual ~AnimTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static void Init(TheaterType theater);
		static char const * Name_From(AnimType anim);
		static AnimType From_Name(char const * name);
		static AnimTypeClass * Find_Or_Make(char const * name);

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_ANIMTYPE);}
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));};
		virtual void Detach(AbstractClass const * target, bool all = true) override;

		virtual void Init_Theater(TheaterType theater) override;
		virtual bool Read_INI(CCINIClass & ini) override;

		virtual bool Create_And_Place(Cell const &, HouseClass * house = NULL) const override {return(false);};
		virtual ObjectClass * Create_One_Of(HouseClass *) const override {return(NULL);};
		virtual void const * Get_Image_Data(void) const override;

		virtual void Load_Image(TheaterType theater);
		void Free_Image(void);
};
