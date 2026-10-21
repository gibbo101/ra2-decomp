// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "anim.h"
#include "_bench.h"
#include "_convert.h"
#include "_map.h"
#include "_palette.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "animtype.h"
#include "bench.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "crc.h"
#include "draw.h"
#include "globals.h"
#include "goptions.h"
#include "bullet.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "lightcon.h"
#include "notify.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "setpal.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tiberium.h"
#include "tracker.h"
#include "bench.hh"
#include "fastmath.h"
#include "coord.h"
#include <math.h>

extern DynamicVectorClass<AnimClass *> DAT_00A83E00;
extern const double DAT_007E3308;
extern const double DAT_007E3310;
extern double DAT_0089A158;
extern double DAT_0089A1B8;
extern double DAT_0089A1A0;
extern double DAT_0089A198;
extern double DAT_0089A168;
extern double DAT_0089A138;
extern int DAT_0089A1C0;
extern double DAT_0089A170;
extern double DAT_0089A188;
extern int DAT_0089A1B4;
struct FileSystem
{
	static char TEMPERAT_PAL[];
};
struct Game
{
	static bool __cdecl CallBack(void);
};
void __fastcall FUN_00489280(Coord const & coord, int damage, TechnoClass * source, WarheadTypeClass const * warhead, bool affect_tiberium, HouseClass * house);
void __fastcall FUN_004A3C30(void * palette, int rate, bool (__cdecl * callback)(void));
extern const double DAT_007E3570;
class SHPStruct
{
	public:
		Rect GetFrameBounds(int frame) const;
};
extern ParticleSystemClass * DAT_00A8ED78;

// FUNCTION: GAMEMD 0x00421CB0



double DAT_0089A158 = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);

// FUNCTION: GAMEMD 0x00421CE0



double DAT_0089A1B8 = DAT_0089A158 / DAT_007E3308;

// FUNCTION: GAMEMD 0x00421D00



double DAT_0089A1A0 = 1.0 / DAT_0089A1B8;

// FUNCTION: GAMEMD 0x00421D20



double DAT_0089A198 = DAT_007E3310 * 45;

// FUNCTION: GAMEMD 0x00421D40



double DAT_0089A168 = DAT_007E3310 * 60;

// FUNCTION: GAMEMD 0x00421D60



double DAT_0089A138 = DAT_007E3310 * 90;

// FUNCTION: GAMEMD 0x00421D80



int DAT_0089A1C0 = (int)(fastmath::tan(DAT_0089A138 - DAT_0089A168) * DAT_0089A158 * 0.5);

// FUNCTION: GAMEMD 0x00421DC0



double DAT_0089A170 = fastmath::atan(DAT_0089A1C0 * (1 / (double)CELL_LEPTON_H));

// FUNCTION: GAMEMD 0x00421DF0



double DAT_0089A188 = fastmath::atan((DAT_0089A1C0 * 2) / DAT_0089A158);

// FUNCTION: GAMEMD 0x00421E20



int DAT_0089A1B4 = (int)((DAT_0089A1C0 * 4) + 0.5);

// FUNCTION: GAMEMD 0x00421E50



Cell const DAT_0089A150(0, 0);

// FUNCTION: GAMEMD 0x00421E60



Coord const DAT_0089A178(0, 0, 0);

// FUNCTION: GAMEMD 0x00421E80



Coord const DAT_0089A1A8(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0);

// FUNCTION: GAMEMD 0x00422720
AnimClass::AnimClass(void) :
	BASECLASS(),
	Animation(),
	Type(NULL),
	OwnerObject(NULL),
	unknown_D0(-1),
	LightConvert(NULL),
	LightConvertIndex(-1),
	TintColor(1000),
	ZAdjust(0),
	YSortAdjust(0),
	IsBuildingAnim(false),
	UnderTemporal(false),
	Paused(false),
	Unpaused(false),
	PausedAnimFrame(0),
	Reverse(false),
	Bounce(),
	TimeToDie(false),
	LoopDelay(0),
	Accum(0.0),
	HasExtras(false),
	RemainingIterations(1),
	UseCellLightConvert(false),
	DeleteOnMapCleanup(false),
	UnableToContinue(false),
	SkipProcessOnce(true),
	Invisible(false),
	PowerOff(false)
{
	Array.Add(this);
	StartSoundAudioController.FUN_00405BE0();
	StopSoundAudioController.FUN_00405BE0();
	IsActive = true;
}

// FUNCTION: GAMEMD 0x00425530
bool AnimClass::Limbo(void)
{
	if (Type != NULL && Type->IsVeins) {
		Map[(Coord const &)PositionCoord].IsAnimAttached = false;
	}

	if (BASECLASS::Limbo()) {
		Map.Remove(this);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004228E0
AnimClass::~AnimClass(void)
{
	Detach_This_From_All(this, true);
	if (GameActive) {
		if (OwnerObject != NULL) {
			ObjectClass * to = OwnerObject;

			int index;
			for (index = 0; index < Array.Count(); index++) {
				if (Array[index] != this && Array[index]->OwnerObject == OwnerObject) break;
			}

			if (index == Array.Count()) {
				to->Fire_Out();
				to->IsAnimAttached = false;
			}
			OwnerObject = NULL;
		}
		AnimClass::Limbo();
	}

	PointerExpiredNotification::NotifyInvalidType.Delete(this);

	if (Type != NULL) {
		Type->Free_Image();
	}

	StartSoundAudioController.EndLooping();
	StopSoundAudioController.EndLooping();
	StartSoundAudioController.FUN_00405C00();
	StopSoundAudioController.FUN_00405C00();

	OwnerObject = NULL;
	Type = NULL;

	if (Fetch_ID() == -2) {
		DAT_00A83E00.Delete(this);
	} else {
		Array.Delete(this);
	}
}

// FUNCTION: GAMEMD 0x00422B20
AnimType Anim_From_Name(char const * name)
{
	if (name == NULL) return(ANIM_NONE);

	if (strcmpi(name, "<none>") == 0) return(ANIM_NONE);

	for (AnimType anim = ANIM_FIRST; anim < AnimTypes.Count(); anim++) {
		if (stricmp(AnimTypes[anim]->IniName, name) == 0) {
			return(anim);
		}
	}
	return(ANIM_NONE);
}

// FUNCTION: GAMEMD 0x00422B80
void Shorten_Attached_Anims(ObjectClass * obj)
{
	if (obj != NULL) {
		for (int index = 0; index < AnimClass::Array.Count(); index++) {
			AnimClass * anim = AnimClass::Array[index];

			if (anim->OwnerObject == obj) {
				anim->RemainingIterations = 0;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00422BC0
int AnimClass::Sort_Y(void) const
{
	return(BASECLASS::Sort_Y() + YSortAdjust);
}

// FUNCTION: GAMEMD 0x00422BE0
Coord AnimClass::Center_Coord(void) const
{
	if (OwnerObject != NULL) {
		return(OwnerObject->Center_Coord() + BASECLASS::Center_Coord());
	}
	return(BASECLASS::Center_Coord());
}

// FUNCTION: GAMEMD 0x00422C70
bool AnimClass::Render(Rect & rect, bool forced, bool extras_only) const
{
	if (LoopDelay != 0) {
		return(false);
	}
	return(BASECLASS::Render(rect, forced, extras_only));
}

// FUNCTION: GAMEMD 0x004238B0
bool AnimClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004238D0
Cell const * AnimClass::Occupy_List(bool) const
{
	static Cell _simple[] = {REFRESH_EOL};

	return(_simple);
}

// SYNTHETIC: GAMEMD 0x00423920 SYMBOL
// _$E145

// FUNCTION: GAMEMD 0x00423930
BounceResultType AnimClass::Bounce_AI(void)
{
	BounceClass & bounce = Bounce;
	BounceResultType bounce_result = bounce.AI();

	if (Type->IsMeteor) {
		bounce.Velocity.Z += bounce.Gravity;
	}

	switch (bounce_result) {
		case BOUNCE_IMPACT: {
			if (Type->BounceAnim != NULL) {
				new AnimClass(Type->BounceAnim, Center_Coord());
			}
			ObjectClass * optr = Map[bounce.Get_Bounce_Coord()].Cell_Occupier();
			while (optr != NULL) {
				Coord cdiff = bounce.Get_Bounce_Coord() - optr->Center_Coord();
				int rad = abs(cdiff.X) + abs(cdiff.Y);
				if (rad <= Type->DamageRadius) {
					int damage = (int)Type->Damage;
					optr->Take_Damage(damage, TacticalMap->Z_Lepton_To_Pixel(rad), Type->Warhead);
				}
				optr = optr->Next;
			}
			break;
		}
		case BOUNCE_SETTLED:
			Delete_Me();
			break;
	}

	Set_Coord(bounce.Get_Bounce_Coord());

	return(bounce_result);
}

// FUNCTION: GAMEMD 0x00424B50
void AnimClass::SetOwnerObject(ObjectClass * owner)
{
	if (OwnerObject != NULL) {
		ObjectClass * to = OwnerObject;

		bool down = IsDown;
		if (down) {
			Map.Remove(this);
		}

		int index;
		for (index = 0; index < Array.Count(); index++) {
			if (Array[index] != this && Array[index]->OwnerObject == OwnerObject) break;
		}

		if (index == Array.Count()) {
			to->Fire_Out();
			to->IsAnimAttached = false;
		}

		Coord center = Center_Coord();
		OwnerObject = NULL;

		Set_Coord(center);

		if (down) {
			Map.Submit(this);
		}
	}

	if (owner == NULL) return;

	Coord center = Center_Coord();
	Map.Remove(this);
	owner->IsAnimAttached = true;
	OwnerObject = owner;
	Set_Coord(center - owner->Center_Coord());
	Map.Submit(this);
}

// FUNCTION: GAMEMD 0x00424C90
void AnimClass::FUN_00424C90(BulletClass * bullet)
{
	AttachedBullet = bullet;
}

// FUNCTION: GAMEMD 0x00424CA0
void AnimClass::FUN_00424CA0(HouseClass * house)
{
	Owner = house;
}

// FUNCTION: GAMEMD 0x00424CB0
LayerType AnimClass::In_Which_Layer(void) const
{
	if (OwnerObject != NULL) {
		return(LAYER_GROUND);
	}
	if (Type != NULL) {
		return(Type->Layer);
	}
	return(LAYER_AIR);
}

// FUNCTION: GAMEMD 0x00424CE0
void AnimClass::Start(void)
{
	Mark(MARK_CHANGE);

	if (!IsInert && Type->Report != -1) {
		VocClass::PlayAt(Type->Report, Center_Coord(), &StartSoundAudioController);
	} else {
		StartSoundAudioController.Stop();
	}
	StopSoundAudioController.Stop();

	if (Type->MiddleFrameIndex == 0) {
		Middle();
	}

	if (!IsInert && Type->TiberiumChainReaction) {
		CellClass * cptr = &Map[Center_Coord()];
		TiberiumType tib = cptr->Tiberium_Type_Here();

		if (tib != TIBERIUM_NONE) {
			TiberiumClass * tiberium = Tiberiums[tib];
			cptr->Reduce_Tiberium(cptr->OverlayData + 1);

			if (tiberium->Debris.Count() > 0 && (abs(Scen->RandomNumber) % 3) == 0) {
				AnimClass * debris = new AnimClass(tiberium->Debris[Random_Pick(0, tiberium->Debris.Count()-1)], Center_Coord() + Coord(0, 0, 10));
				debris->LightConvert = ColorSchemes[tiberium->Color]->Converter;
				debris->TintColor = cptr->Brightness;
			}

			FUN_00489280(PositionCoord, Rule->TiberiumExplosionDamage, NULL, Rule->C4Warhead, false, NULL);

			cptr->Recalc_Attributes();
			Map.ResetZones(cptr->CellID);
			Map.RecalculateSubZones(cptr->CellID);
		}
	}
}

// FUNCTION: GAMEMD 0x00424F00
void AnimClass::Middle(void)
{
	Cell cell = Center_Coord().As_Cell();
	CellClass * cellptr = &Map[cell];

	int width = 30;
	int height = 30;

	SHPStruct const * shape = (SHPStruct const *)Get_Image_Data();
	if (shape != NULL) {
		if (Type->MiddleFrameWidth == -1) {
			Type->MiddleFrameWidth = shape->GetFrameBounds(Type->MiddleFrameIndex).Width;
		}
		if (Type->MiddleFrameHeight == -1) {
			Type->MiddleFrameHeight = shape->GetFrameBounds(Type->MiddleFrameIndex).Height;
		}
		width = Type->MiddleFrameWidth;
		height = Type->MiddleFrameHeight;
	}

	if (Type->SpawnsParticle != -1) {
		for (int i = 0; i < Type->NumParticles; i++) {
			Coord pos = PositionCoord;
			DAT_00A8ED78->Spawn_Particle(ParticleTypes[Type->SpawnsParticle], pos);
		}
	}

	if (HeightAGL < 30) {
		if (Type->Scorch && (!Type->Crater || Scen->RandomNumber(0, 0x7FFFFFFE) * DAT_007E3570 < 0.5)) {
			SmudgeTypeClass::Scorch_The_Ground(Center_Coord(), width, height, false);
		} else if (Type->Crater) {
			cellptr->Reduce_Tiberium(6);
			if (Type->ForceBigCraters) {
				SmudgeTypeClass::Crater_The_Ground(Center_Coord(), 300, 300, true);
			} else {
				SmudgeTypeClass::Crater_The_Ground(Center_Coord(), width, height, false);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00425150
void AnimClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	if (OwnerObject == target && target != NULL) {
		Map.Remove(this);
		OwnerObject->AnimPointerExpired(this);
		OwnerObject = NULL;
		UnableToContinue = true;
		Mark(MARK_UP);
	}
	if (Type == target) {
		Type = NULL;
	}
	if (AttachedBullet == target) {
		AttachedBullet = NULL;
		Delete_Me();
	}
	if (Owner == target) {
		AttachedBullet = NULL;
		Delete_Me();
	}
}

// FUNCTION: GAMEMD 0x004251F0
void AnimClass::Do_Atom_Damage(HousesType ownerhouse, Cell const & cell)
{
	Coord coord(cell.X * 256 + 128, cell.Y * 256 + 128, 0);
	FUN_00489280(coord, Rule->AtomDamage, NULL, Rule->NukeWarhead, false, NULL);
	if (Session.Type == GAME_NORMAL) {
		FUN_004A3C30(&FileSystem::TEMPERAT_PAL, 30, Game::CallBack);
	}
}

// FUNCTION: GAMEMD 0x00425260
void AnimClass::Disable(void)
{
	PowerOff = true;
}

// FUNCTION: GAMEMD 0x00425270
void AnimClass::Enable(void)
{
	PowerOff = false;
}

// FUNCTION: GAMEMD 0x00425280
HRESULT STDMETHODCALLTYPE AnimClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) AnimClass(NoInitClass());
		StartSoundAudioController.FUN_00405BE0();
		StopSoundAudioController.FUN_00405BE0();

		Swizzle_Pointer(&Type);
		Swizzle_Pointer(&OwnerObject);
		Swizzle_Pointer(&AttachedBullet);
		Swizzle_Pointer(&Owner);

		if (LightConvertIndex > -1) {
			if (LightConvertIndex < ColorSchemes.Count()) {
				LightConvert = ColorSchemes[LightConvertIndex]->Converter;
				return(result);
			}
		} else {
			if (PaletteName[0] != '\0') {
				DynamicVectorClass<ColorScheme *> * palette = ColorScheme::GeneratePalette(PaletteName);
				if (palette != NULL) {
					ColorScheme * scheme = palette->GetItem(0);
					if (scheme != NULL) {
						LightConvert = scheme->Converter;
						return(result);
					}
				}
			}
			LightConvert = NULL;
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004253B0
HRESULT STDMETHODCALLTYPE AnimClass::Save(IStream * stream, BOOL cleardirty)
{
	if (LightConvert != NULL) {
		for (int index = ColorSchemes.Count() - 1; index >= 0; index--) {
			if (LightConvert == ColorSchemes[index]->Converter) {
				LightConvertIndex = index;
				goto done;
			}
		}
	}
	LightConvertIndex = -1;
done:
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x00425410
void AnimClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	if (OwnerObject != NULL) {
		crc(OwnerObject->Fetch_ID());
	}
	crc((int)unknown_D0);
	crc(TintColor);
	crc(ZAdjust);
	crc(YSortAdjust);
	crc(LoopDelay);
	crc(Accum);
	crc(AnimFlags);
	crc(HasExtras);
	crc((int)RemainingIterations);
	crc(UseCellLightConvert);
	crc(DeleteOnMapCleanup);
	crc(UnableToContinue);
	crc(SkipProcessOnce);
	crc(PowerOff);
}

// FUNCTION: GAMEMD 0x00425510
int AnimClass::Stage_Count(void) const
{
	return(Type->End);
}

// FUNCTION: GAMEMD 0x00425520
ObjectTypeClass const * AnimClass::Class_Of(void) const
{
	return(Type);
}

// FUNCTION: GAMEMD 0x004255B0
void AnimClass::Delete_Me(void)
{
	if (OwnerObject != NULL) {
		OwnerObject->AnimPointerExpired(this);
	}
	SetOwnerObject(NULL);
	StartSoundAudioController.EndLooping();
	if (!IsInert && Type != NULL && Type->StopSound != -1) {
		VocClass::PlayAt(Type->StopSound, Center_Coord(), &StopSoundAudioController);
	}
	BASECLASS::Delete_Me();
}

// FUNCTION: GAMEMD 0x00425630
int AnimClass::Get_Height(void) const
{
	int z = Get_Coord().Z;
	if (OwnerObject != NULL) {
		z += OwnerObject->Get_Coord().Z;
	}
	return(z);
}

// FUNCTION: GAMEMD 0x004260F0
bool AnimClass::Is_Valid_Flaming_Guy_Cell(Cell const & cell)
{
	CellClass * cptr = &Map[cell];

	if (cptr->IsUnderBridge && Get_Coord().Z - DAT_0089A1C0 * cptr->Height > 2 * DAT_0089A1C0) {
		if ((cptr->BridgeFlag.Occupy.Building || cptr->BridgeFlag.Occupy.Monolith || cptr->BridgeFlag.Occupy.Vehicle)) {
			return(false);
		}
	} else {
		if (cptr->Land_Type() == LAND_ROCK || cptr->Land_Type() == LAND_TUNNEL || cptr->Land_Type() == LAND_WALL) {
			return(false);
		}
		if (cptr->Overlay != OVERLAY_NONE && OverlayTypes[cptr->Overlay]->IsWall) {
			return(false);
		}
		if ((cptr->Flag.Occupy.Building || cptr->Flag.Occupy.Monolith || cptr->Flag.Occupy.Vehicle) || DAT_0089A1C0 * cptr->Height - Get_Coord().Z > 2 * DAT_0089A1C0) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004261D0
void AnimClass::Post_Load_Game(void)
{
	for (int i = 0; i < Array.Count(); i++) {
		AnimClass * anim = Array[i];
		if (anim->Type->IsAnimatedTiberium) {
			CellClass * cptr = &Map[anim->Get_Coord()];
			TiberiumType tib = cptr->Tiberium_Type_Here();
			if (tib != TIBERIUM_NONE) {
				TiberiumClass * tptr = Tiberiums[tib];
				if (tptr != NULL) {
					anim->LightConvert = ColorSchemes[tptr->Color]->Converter;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00426270
void AnimClass::Set_Occupy_Bit(Coord const & coord)
{
	int spot_index = CellClass::Spot_Index(coord);
	CellClass & cell = Map[coord];
	if (Map.Get_Height_GL(coord) + DAT_0089A1B4 <= coord.Z && cell.IsUnderBridge) {
		cell.BridgeFlag.Composite |= (1 << spot_index);
		cell.BridgeInfType = ((AbstractClass *)this)->Owner();
	} else {
		cell.Flag.Composite |= (1 << spot_index);
		cell.InfType = ((AbstractClass *)this)->Owner();
	}
}

// FUNCTION: GAMEMD 0x00426300
void AnimClass::Clear_Occupy_Bit(Coord const & coord)
{
	int spot_index = CellClass::Spot_Index(coord);
	CellClass & cell = Map[coord];
	int height = Map.Get_Height_GL(coord);

	if (height + DAT_0089A1B4 <= coord.Z) {
		cell.BridgeFlag.Composite &= ~(1 << spot_index);
		if (!(cell.BridgeFlag.Composite & 0x1C)) {
			cell.BridgeInfType = HOUSE_NONE;
		}
	} else {
		cell.Flag.Composite &= ~(1 << spot_index);
		if (!(cell.Flag.Composite & 0x1C)) {
			cell.InfType = HOUSE_NONE;
		}
	}
}

// FUNCTION: GAMEMD 0x00426390
// ObjectClass::Is_Players_Army

// FUNCTION: GAMEMD 0x004263A0
// ObjectClass::Visual_Character

// FUNCTION: GAMEMD 0x004263B0
bool ObjectClass::IsStrange(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x004263C0
// ObjectClass::Full_Name

// FUNCTION: GAMEMD 0x004263D0
// ObjectClass::Fire_Coord

// FUNCTION: GAMEMD 0x00426410
// ObjectClass::Occupies_Cells

// FUNCTION: GAMEMD 0x00426420
// ObjectClass::IsDisguised

// FUNCTION: GAMEMD 0x00426430
// ObjectClass::IsDisguisedAs

// TEMPLATE: GAMEMD 0x00426440
// ?Draw_Pre_Render@ObjectClass@@UBEXABVPoint2D@@ABV?$TRect@H@@@Z

// TEMPLATE: GAMEMD 0x00426450
// ?Draw_Post_Render@ObjectClass@@UBEXABVPoint2D@@ABV?$TRect@H@@@Z

// FUNCTION: GAMEMD 0x00426460
// ObjectClass::IronCurtain

// FUNCTION: GAMEMD 0x00426470
// ObjectClass::StopAirstrikeTimer

// FUNCTION: GAMEMD 0x00426480
// ObjectClass::StartAirstrikeTimer

// FUNCTION: GAMEMD 0x00426490
// ObjectClass::IsIronCurtained

// FUNCTION: GAMEMD 0x004264A0
// ObjectClass::Destroy

// FUNCTION: GAMEMD 0x004264B0
// ObjectClass::Per_Cell_Process

// FUNCTION: GAMEMD 0x004264C0
// ObjectClass::Can_Enter_Cell

// FUNCTION: GAMEMD 0x004264D0
// ObjectClass::Can_Reach

// FUNCTION: GAMEMD 0x004264E0
bool ObjectClass::IsBeingWarpedOut(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x004264F0
bool ObjectClass::IsWarpingIn(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x00426500
bool ObjectClass::IsWarpingSomethingOut(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x00426510
bool ObjectClass::IsNotWarping(void) const
{
	return(true);
}

// FUNCTION: GAMEMD 0x00426520
void * ObjectClass::GetRemapColour(void) const
{
	return(NULL);
}

// FUNCTION: GAMEMD 0x00426530
int AnimClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00426540
HRESULT STDMETHODCALLTYPE AnimClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AnimClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00426580
RTTIType AnimClass::Fetch_RTTI(void) const
{
	return(RTTI_ANIM);
}

// SYNTHETIC: GAMEMD 0x00426590
// ??_GAnimClass@@UAEPAXI@Z


// TEMPLATE: GAMEMD 0x00426620
// ??0?$CDTimerClass@VFrameTimerClass@@@@QAE@ABVNoInitClass@@@Z

// TEMPLATE: GAMEMD 0x00426630
// ?Value@?$CDTimerClass@VFrameTimerClass@@@@QBEHXZ
template class CDTimerClass<FrameTimerClass>;

extern int DAT_0089A1C8;
extern int DAT_0089A1CC;
extern int DAT_0089A1D0;
extern int DAT_0089A1D4;

// FUNCTION: GAMEMD 0x00426650
void FUN_00426650(void)
{
	DAT_0089A1C8 = 0;
	DAT_0089A1CC = 0;
	DAT_0089A1D0 = 0;
	DAT_0089A1D4 = 0;
}
