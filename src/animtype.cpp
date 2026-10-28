// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "animtype.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "ccfile.h"
#include "data.h"
#include "dbgprint.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "mixfile.h"
#include "overtype.h"
#include "ptype.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "warhead.h"

// SYNTHETIC: GAMEMD 0x00427320 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00427350 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00427370 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00427390 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004273B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004273D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004273F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00427430 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00427460 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00427490 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004274C0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004274D0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004274F0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00427510 SYMBOL
// _$E41

extern char DAT_00889F64[];
namespace Unsorted { extern bool ArmageddonMode; }

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

void FUN_005F9710(char * name);
void FUN_005F96B0(char * name, TheaterType theater);

// FUNCTION: GAMEMD 0x00427530
AnimTypeClass::AnimTypeClass(char const * ininame) :
	BASECLASS(ininame),
	HeapID(ANIM_NONE),
	MiddleFrameIndex(0),
	MiddleFrameWidth(0),
	MiddleFrameHeight(0),
	unknown_2A4(false),
	Damage(0.0),
	Rate(1),
	Start(0),
	LoopStart(0),
	LoopEnd(0),
	End(0),
	LoopCount(0),
	Next(NULL),
	SpawnsParticle(-1),
	NumParticles(0),
	DetailLevel(0),
	TranslucencyDetailLevel(0),
	RandomLoopDelay(),
	RandomRate(),
	Translucency(0),
	Spawns(NULL),
	SpawnCount(0),
	Report(VOC_NONE),
	StopSound(VOC_NONE),
	BounceAnim(NULL),
	ExpireAnim(NULL),
	TrailerAnim(NULL),
	TrailerSeperation(0),
	Elasticity(0.8f),
	MinZVel(3.5),
	MaxZVel(3.5),
	MaxXYVel(15.0),
	Warhead(NULL),
	DamageRadius(0),
	TiberiumSpawnType(NULL),
	TiberiumSpreadRadius(0),
	YSortAdjust(0),
	YDrawOffset(0),
	ZAdjust(0),
	MakeInfantry(-1),
	RunningFrames(0),
	IsFlamingGuy(false),
	IsVeins(false),
	IsMeteor(false),
	TiberiumChainReaction(false),
	IsTiberium(false),
	HideIfNoOre(false),
	Bouncer(false),
	Tiled(false),
	ShouldUseCellDrawer(true),
	UseNormalLight(false),
	DemandLoad(false),
	FreeLoad(false),
	IsAnimatedTiberium(false),
	AltPalette(false),
	Normalized(false),
	Layer(LAYER_AIR),
	DoubleThick(false),
	Flat(false),
	Translucent(false),
	Scorch(false),
	Flamer(false),
	Crater(false),
	ForceBigCraters(false),
	Sticky(false),
	PingPong(false),
	Reverse(false),
	Shadow(false),
	PsiWarning(false),
	ShouldFogRemove(true)
{
	Create_ID();
	IsSentient = true;
	IsStealthy = true;
	IsSelectable = false;
	IsLegalTarget = false;
	IsInsignificant = true;
	IsImmune = true;
	IsFootprint = false;

	AnimTypes.Add(this);
	AbstractTypePtrTracker.Add(this);

	HeapID = (AnimType)AnimTypes.ID(this);
}

// FUNCTION: GAMEMD 0x00427850
AnimTypeClass::AnimTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	RandomLoopDelay(x),
	RandomRate(x)
{
}

// FUNCTION: GAMEMD 0x00427880
AnimTypeClass::~AnimTypeClass(void)
{
	AbstractTypePtrTracker.Delete(this);
	AnimTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x00427940
void AnimTypeClass::Init(TheaterType theater)
{
	for (int index = 0; index < AnimTypes.Count(); index++) {
		AnimTypeClass * anim = AnimTypes[index];

		char fullname[_MAX_FNAME+_MAX_EXT];
		if (!anim->DemandLoad) {
			if (anim->IsTheater) {
				if (anim->ImageData != NULL && anim->ImageAllocated) {
					delete (ShapeSet *)anim->ImageData;
					anim->ImageAllocated = false;
				}
				anim->ImageData = NULL;
				_makepath(fullname, NULL, NULL, anim->IniName, Theaters[theater].Suffix);
				anim->ImageData = FileSystem::LoadFile(fullname, true);
				if (anim->ImageData == NULL) {
					_makepath(fullname, NULL, NULL, anim->IniName, ".SHP");
					anim->ImageData = FileSystem::LoadFile(fullname, true);
				}
			} else if (anim->IsNewTheater) {
				anim->Load_Image(theater);
			}
		} else {
			if (anim->IsTheater || anim->IsNewTheater) {
				if (anim->ImageData != NULL && anim->ImageAllocated) {
					delete (ShapeSet *)anim->ImageData;
					anim->ImageAllocated = false;
				}
				anim->ImageData = NULL;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00427A80
void AnimTypeClass::Init_Theater(TheaterType theater)
{
	char fullname[_MAX_FNAME+_MAX_EXT];

	if (Unsorted::ArmageddonMode || theater != LastTheater) {
		if (IsTheater && !DemandLoad) {
			if (ImageData != NULL && ImageAllocated) {
				delete (ShapeSet *)ImageData;
				ImageAllocated = false;
			}
			ImageData = NULL;
			_makepath(fullname, NULL, NULL, IniName, Theaters[theater].Suffix);
			ImageData = FileSystem::LoadFile(fullname, true);
		} else if (IsNewTheater) {
			Load_Image(theater);
		}
	}
}

// FUNCTION: GAMEMD 0x00427B50
void AnimTypeClass::Load_Image(TheaterType theater)
{
	char fullname[_MAX_FNAME+_MAX_EXT];

	if (!DemandLoad) {
		if (IsTheater) {
			Fetch_Normal_Image();
		} else {
			if (ImageData != NULL && ImageAllocated) {
				delete (ShapeSet *)ImageData;
				ImageAllocated = false;
			}
			ImageData = NULL;
			_makepath(fullname, NULL, NULL, *GraphicName != '\0' ? Graphic_Name() : IniName, ".SHP");
			FUN_005F96B0(fullname, theater);
			ImageData = FileSystem::LoadFile(fullname, true);
			if (ImageData == NULL) {
				FUN_005F9710(fullname);
				ImageData = FileSystem::LoadFile(fullname, true);
			}
		}
	}

	ShapeSet const * shape = (ShapeSet const *)ImageData;
	if (shape != NULL) {
		unknown_2A4 = true;
		if (End == 0) {
			End = shape->Get_Count();
			if (Shadow) {
				End /= 2;
			}
		}
		if (LoopEnd == 0) {
			LoopEnd = End;
		}
		MiddleFrameIndex = shape->Get_Count() / 2;
		MiddleFrameWidth = -1;
		MiddleFrameHeight = -1;
	}
}

// FUNCTION: GAMEMD 0x00427C90
char const * AnimTypeClass::Name_From(AnimType anim)
{
	if ((unsigned)anim > (unsigned)AnimTypes.Count()) return(DAT_00889F64);

	return(AnimTypes[anim]->IniName);
}

// FUNCTION: GAMEMD 0x00427CB0
AnimType AnimTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < AnimTypes.Count(); index++) {
			if (stricmp(AnimTypes[index]->IniName, name) == 0) {
				return(AnimType)index;
			}
		}
	}
	return(ANIM_NONE);
}

// FUNCTION: GAMEMD 0x00427D00
bool AnimTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		bool shadow = ini.Get_Bool(Name(), "Shadow", Shadow);
		if (shadow != Shadow) {
			Shadow = shadow;
			if (Shadow) {
				End /= 2;
			} else {
				End *= 2;
			}
			if (LoopEnd > End) {
				LoopEnd = End;
			}
		}

		if (GraphicName[0] != '\0') {
			if (ImageData == NULL) {
				IsTheater = ArtINI.Get_Bool(Name(), "Theater", IsTheater);
				IsNewTheater = ArtINI.Get_Bool(Name(), "NewTheater", IsNewTheater);
			}
			Load_Image(THEATER_NONE);
		}

		Layer = ini.Get_LayerType(Name(), "Layer", Layer);
		AltPalette = ArtINI.Get_Bool(Name(), "AltPalette", AltPalette);
		DoubleThick = ini.Get_Bool(Name(), "DoubleThick", DoubleThick);
		Flat = ini.Get_Bool(Name(), "Flat", Flat);
		Flamer = ini.Get_Bool(Name(), "Flamer", Flamer);
		Normalized = ini.Get_Bool(Name(), "Normalized", Normalized);
		Translucent = ini.Get_Bool(Name(), "Translucent", Translucent);
		Scorch = ini.Get_Bool(Name(), "Scorch", Scorch);
		Crater = ini.Get_Bool(Name(), "Crater", Crater);
		ForceBigCraters = ini.Get_Bool(Name(), "ForceBigCraters", ForceBigCraters);
		Sticky = ini.Get_Bool(Name(), "Sticky", Sticky);
		PingPong = ini.Get_Bool(Name(), "PingPong", PingPong);
		Reverse = ini.Get_Bool(Name(), "Reverse", Reverse);
		PsiWarning = ini.Get_Bool(Name(), "PsiWarning", PsiWarning);
		TiberiumChainReaction = ini.Get_Bool(Name(), "TiberiumChainReaction", TiberiumChainReaction);

		int delay = ini.Get_Int(Name(), "Rate", -1);
		if (delay != -1) {
			Rate = (delay > 0) ? TICKS_PER_MINUTE / delay : 0;
		}

		Damage = ini.Get_Float(Name(), "Damage", Damage);
		Start = ini.Get_Int(Name(), "Start", Start);
		End = ini.Get_Int(Name(), "End", End);
		LoopStart = ini.Get_Int(Name(), "LoopStart", LoopStart);
		LoopEnd = ini.Get_Int(Name(), "LoopEnd", LoopEnd);
		LoopCount = ini.Get_Int(Name(), "LoopCount", LoopCount);
		Next = TGet_Class(ini, Name(), "Next", Next);

		DetailLevel = ini.Get_Int(Name(), "DetailLevel", DetailLevel);
		TranslucencyDetailLevel = ini.Get_Int(Name(), "TranslucencyDetailLevel", TranslucencyDetailLevel);

		Point2D loop_random_delay = ini.Get_Point(Name(), "RandomLoopDelay", Point2D(RandomLoopDelay.Min, RandomLoopDelay.Max));
		RandomLoopDelay.Min = loop_random_delay.X;
		RandomLoopDelay.Max = loop_random_delay.Y;

		Translucency = ini.Get_Int(Name(), "Translucency", Translucency);
		IsTiberium = ini.Get_Bool(Name(), "IsTiberium", IsTiberium);
		HideIfNoOre = ini.Get_Bool(Name(), "HideIfNoOre", HideIfNoOre);
		YSortAdjust = ini.Get_Int(Name(), "YSortAdjust", YSortAdjust);

		Elasticity = ini.Get_Float(Name(), "Elasticity", Elasticity);
		MaxXYVel = ini.Get_Float(Name(), "MaxXYVel", MaxXYVel);
		MinZVel = ini.Get_Float(Name(), "MinZVel", MinZVel);
		MakeInfantry = ini.Get_Int(Name(), "MakeInfantry", MakeInfantry);

		Spawns = TGet_Class(ini, Name(), "Spawns", Spawns);
		SpawnCount = ini.Get_Int(Name(), "SpawnCount", SpawnCount);
		IsMeteor = ini.Get_Bool(Name(), "IsMeteor", IsMeteor);
		IsVeins = ini.Get_Bool(Name(), "IsVeins", IsVeins);
		TiberiumSpreadRadius = ini.Get_Int(Name(), "TiberiumSpreadRadius", TiberiumSpreadRadius);
		TiberiumSpawnType = TGet_Class(ini, Name(), "TiberiumSpawnType", TiberiumSpawnType);

		IsAnimatedTiberium = ini.Get_Bool(Name(), "IsAnimatedTiberium", IsAnimatedTiberium);
		ShouldFogRemove = ini.Get_Bool(Name(), "ShouldFogRemove", ShouldFogRemove);
		IsFlamingGuy = ini.Get_Bool(Name(), "IsFlamingGuy", IsFlamingGuy);
		RunningFrames = ini.Get_Int(Name(), "RunningFrames", RunningFrames);
		YDrawOffset = ini.Get_Int(Name(), "YDrawOffset", YDrawOffset);
		ZAdjust = ini.Get_Int(Name(), "ZAdjust", ZAdjust);

		Report = ini.Get_VocType(Name(), "StartSound", Report);
		if (Report == VOC_NONE) {
			Report = ini.Get_VocType(Name(), "Report", VOC_NONE);
		}
		StopSound = ini.Get_VocType(Name(), "StopSound", StopSound);

		BounceAnim = TGet_Class(ini, Name(), "BounceAnim", BounceAnim);
		ExpireAnim = TGet_Class(ini, Name(), "ExpireAnim", ExpireAnim);
		TrailerAnim = TGet_Class(ini, Name(), "TrailerAnim", TrailerAnim);

		TrailerSeperation = ini.Get_Int(Name(), "TrailerSeperation", TrailerSeperation);
		DamageRadius = ini.Get_Int(Name(), "DamageRadius", DamageRadius);
		Warhead = TGet_Class(ini, Name(), "Warhead", Warhead);

		Bouncer = ini.Get_Bool(Name(), "Bouncer", Bouncer);
		Tiled = ini.Get_Bool(Name(), "Tiled", Tiled);
		ShouldUseCellDrawer = ini.Get_Bool(Name(), "ShouldUseCellDrawer", ShouldUseCellDrawer);
		UseNormalLight = ini.Get_Bool(Name(), "UseNormalLight", UseNormalLight);

		char particle[32];
		if (ini.Get_String(Name(), "SpawnsParticle", NULL, particle, sizeof(particle))) {
			SpawnsParticle = ParticleTypeClass::From_Name(particle);
		}
		NumParticles = ini.Get_Int(Name(), "NumParticles", NumParticles);

		Point2D random_rate = ini.Get_Point(Name(), "RandomRate", Point2D(-1, -1));
		if (random_rate.X != -1) {
			RandomRate.Min = (random_rate.X > 0) ? TICKS_PER_MINUTE / random_rate.X : 0;
		}
		if (random_rate.Y != -1) {
			RandomRate.Max = (random_rate.Y > 0) ? TICKS_PER_MINUTE / random_rate.Y : 0;
		}
		if (RandomRate.Max < 0) {
			RandomRate.Max = 0;
		}
		if (RandomRate.Min > RandomRate.Max) {
			RandomRate.Min = RandomRate.Max;
		}

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00428800
HRESULT STDMETHODCALLTYPE AnimTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) AnimTypeClass(NoInitClass());

		Fetch_Voxel_Image();
		Fetch_Normal_Image();

		Swizzle_Pointer(&Next);
		Swizzle_Pointer(&ExpireAnim);
		Swizzle_Pointer(&TrailerAnim);
		Swizzle_Pointer(&BounceAnim);
		Swizzle_Pointer(&Spawns);
		Swizzle_Pointer(&Warhead);
		Swizzle_Pointer(&TiberiumSpawnType);

		if (!DemandLoad) {
			if (IsTheater) {
				char fullname[_MAX_FNAME+_MAX_EXT];
				_makepath(fullname, NULL, NULL, IniName, Theaters[Scen->Theater].Suffix);
				ImageData = FileSystem::LoadFile(fullname, true);
				return(S_OK);
			} else if (IsNewTheater) {
				Load_Image(Scen->Theater);
			}
		}
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00428970
HRESULT STDMETHODCALLTYPE AnimTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}


// FUNCTION: GAMEMD 0x00428990
HRESULT STDMETHODCALLTYPE AnimTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AnimTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004289D0
void AnimTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Layer);
	crc(Normalized);
	crc(Flat);
	crc(Translucent);
	crc(Scorch);
	crc(Flamer);
	crc(Crater);
	crc(Sticky);
	crc(PingPong);
	crc(Reverse);
	crc(Shadow);
	crc(MiddleFrameIndex);
	crc(Damage);
	crc(TiberiumChainReaction);
	crc(Rate);
	crc(Start);
	crc(LoopStart);
	crc(LoopEnd);
	crc(End);
	crc(Report);
	crc(StopSound);
	crc(NumParticles);
	crc(DetailLevel);
	crc(TranslucencyDetailLevel);
	crc(RandomLoopDelay.Min);
	crc(RandomLoopDelay.Max);
	crc(Translucency);
	crc(Tiled);
}

// FUNCTION: GAMEMD 0x00428B80
AnimTypeClass * AnimTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<AnimTypeClass>(name, AnimTypes));
}

// FUNCTION: GAMEMD 0x00428C10
void AnimTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (Next == target) {
		Next = NULL;
	}
}

// FUNCTION: GAMEMD 0x00428C30
void const * AnimTypeClass::Get_Image_Data(void) const
{
	void const * data = ImageData;
	if (data == NULL && DemandLoad) {
		DebugString("Demand loading image for %s\n", Full_Name());

		char fullname[_MAX_FNAME+_MAX_EXT];
		_makepath(fullname, NULL, NULL, *GraphicName != '\0' ? Graphic_Name() : IniName, ".SHP");

		if (IsTheater) {
			_makepath(fullname, NULL, NULL, IniName, Theaters[Scen->Theater].Suffix);
		} else if (IsNewTheater) {
			FUN_005F96B0(fullname, Scen->Theater);
		}

		CCFileClass file(fullname);
		((void const *&)ImageData) = Load_Alloc_Data(file);
		if (ImageData != NULL) {
			((bool &)ImageAllocated) = true;
		} else {
			FUN_005F9710(fullname);
			CCFileClass file2(fullname);
			((void const *&)ImageData) = Load_Alloc_Data(file2);
			if (ImageData != NULL) {
				((bool &)ImageAllocated) = true;
			}
		}
		((AnimTypeClass *)this)->Load_Image(Scen->Theater);

		data = ImageData;
	}
	return(data);
}

// FUNCTION: GAMEMD 0x00428DE0
void AnimTypeClass::Free_Image(void)
{
	if (DemandLoad && ImageData != NULL && FreeLoad) {
		DebugString("Freeing loaded image for %s\n", Full_Name());
		delete (char *)ImageData;
		ImageData = NULL;
		ImageAllocated = false;
	}
}

// FUNCTION: GAMEMD 0x00428E40
// ObjectTypeClass::Get_Ownable

// FUNCTION: GAMEMD 0x00428E50
// AnimTypeClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x00428E60
// AnimTypeClass::Fetch_Heap_ID

// FUNCTION: GAMEMD 0x00428E70
// AnimTypeClass::Fetch_Object_Size

// FUNCTION: GAMEMD 0x00428E80
// AnimTypeClass::Create_And_Place

// FUNCTION: GAMEMD 0x00428E90
// AnimTypeClass::Create_One_Of

// SYNTHETIC: GAMEMD 0x00428EA0
// ??_GAnimTypeClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00428F70
// ?TFind_Or_Make@@YIPAVAnimTypeClass@@PBDABV?$DynamicVectorClass@PAVAnimTypeClass@@@@@Z
template AnimTypeClass * TFind_Or_Make<AnimTypeClass>(char const *, DynamicVectorClass<AnimTypeClass *> const &);
