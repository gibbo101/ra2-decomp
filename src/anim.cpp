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
#include "msanim.h"
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
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x00421C90 SYMBOL
// _$E2

extern DynamicVectorClass<AnimClass *> DAT_00A83E00;
struct FileSystem
{
	static char TEMPERAT_PAL[];
	static ConvertClass * ANIM_PAL;
};
namespace Game { bool __cdecl CallBack(void); extern bool bDirect3DIsUseable; }
void __fastcall FUN_00489280(Coord const & coord, int damage, TechnoClass * source, WarheadTypeClass const * warhead, bool affect_tiberium, HouseClass * house);
void __fastcall FUN_004A3C30(void * palette, int rate, bool (__cdecl * callback)(void));
extern const double DAT_007E3570;

extern ParticleSystemClass * DAT_00A8ED78;
class AudioEvent;
AudioEvent * __fastcall FUN_00750D40(Coord const & coord, AudioController * controller);
bool __fastcall FUN_0043B4C0(HouseClass * house, CellClass * cell);
SideType FUN_006A46D0(char const * name);

// SYNTHETIC: GAMEMD 0x00421CB0 SYMBOL
// _$E106
// SYNTHETIC: GAMEMD 0x00421CE0 SYMBOL
// _$E109
// SYNTHETIC: GAMEMD 0x00421D00 SYMBOL
// _$E112
// SYNTHETIC: GAMEMD 0x00421D20 SYMBOL
// _$E115
// SYNTHETIC: GAMEMD 0x00421D40 SYMBOL
// _$E118
// SYNTHETIC: GAMEMD 0x00421D60 SYMBOL
// _$E121
// SYNTHETIC: GAMEMD 0x00421D80 SYMBOL
// _$E124
// SYNTHETIC: GAMEMD 0x00421DC0 SYMBOL
// _$E127
// SYNTHETIC: GAMEMD 0x00421DF0 SYMBOL
// _$E130
// SYNTHETIC: GAMEMD 0x00421E20 SYMBOL
// _$E133
// SYNTHETIC: GAMEMD 0x00421E50 SYMBOL
// _$E136
// SYNTHETIC: GAMEMD 0x00421E60 SYMBOL
// _$E139
// SYNTHETIC: GAMEMD 0x00421E80 SYMBOL
// _$E142

// FUNCTION: GAMEMD 0x00421EA0
AnimClass::AnimClass(AnimTypeClass const * type, Coord const & coord, int timedelay, int loop, ShapeFlags_Type flags, int zadjust, bool reverse) :
	BASECLASS(),
	Animation(),
	Type((AnimTypeClass *)type),
	OwnerObject(NULL),
	unknown_D0(-1),
	LightConvert(NULL),
	LightConvertIndex(-1),
	TintColor(1000),
	ZAdjust(0),
	YSortAdjust(0),
	FlamingGuyCoords(COORD_NONE),
	FlamingGuyRetries(0),
	IsBuildingAnim(false),
	UnderTemporal(false),
	Paused(false),
	Unpaused(false),
	PausedAnimFrame(0),
	Reverse(false),
	Bounce(),
	TranslucencyLevel(0),
	TimeToDie(false),
	AttachedBullet(NULL),
	Owner(NULL),
	LoopDelay(timedelay),
	Accum(1.0),
	AnimFlags(flags),
	HasExtras(false),
	RemainingIterations(1),
	UseCellLightConvert(false),
	DeleteOnMapCleanup(false),
	IsInert(false),
	IsFogged(false),
	FlamingGuyExpire(false),
	UnableToContinue(false),
	SkipProcessOnce(true),
	Invisible(false),
	PowerOff(false)
{
	PaletteName[0] = 0;
	Create_ID();
	StartSoundAudioController.FUN_00405BE0();
	StopSoundAudioController.FUN_00405BE0();
	Array.Add(this);
	IsActive = true;

	if (Type == NULL) {
		Delete_Me();
		return;
	}

	YSortAdjust = Type->YSortAdjust;

	if (Type->End == -1) {
		Type->End = ((ShapeSet const *)Type->Get_Image_Data())->Get_Count();
		if (Type->Shadow) {
			Type->End /= 2;
		}
	}
	if (Type->LoopEnd == -1) {
		Type->LoopEnd = Type->End;
	}

	if (zadjust != 0) {
		ZAdjust = zadjust;
	} else {
		ZAdjust = Type->ZAdjust;
	}

	int delay = Type->Rate;
	if (Type->RandomRate.Min != 0 || Type->RandomRate.Max != 0) {
		if (Type->RandomRate.Min <= Type->RandomRate.Max) {
			delay = Random_Pick(Type->RandomRate.Min, Type->RandomRate.Max);
		}
	}
	if (Type->Normalized) {
		Animation.Set_Rate(Options.Normalize_Delay(delay));
	} else {
		Animation.Set_Rate(delay);
	}

	Animation.Set_Stage(0);

	if (Type->Layer == LAYER_AIR || Type->Layer == LAYER_TOP) {
		Height = Rule->FlightLevel;
	} else {
		HeightAGL = 0;
	}

	if (reverse) {
		Reverse = true;
	}
	if (Type->Reverse || Reverse) {
		Animation.Set_Stage(Type->LoopEnd - 1);
		Animation.Set_Step(-Animation.Fetch_Step());
	}

	if (!Type->Bouncer && !Type->IsMeteor) {
		BASECLASS::Unlimbo(coord, DIR_N);
	} else {
		HasExtras = true;
		if (Type->IsMeteor) {
			Vector3 velocity((abs(Scen->RandomNumber) % int(Type->MaxXYVel * 2)) - Type->MaxXYVel,
							(float)((abs(Scen->RandomNumber) % int(Type->MaxXYVel * 2)) - Type->MaxXYVel), Type->MinZVel);

			float negy = -velocity.Y;
			if (velocity.X < negy) {
				velocity.X = -velocity.X;
				velocity.Y = -velocity.Y;
			}

			int time = 70 - abs(Scen->RandomNumber() % 20);
			int x = coord.X - time * velocity.X;
			int y = coord.Y - time * velocity.Y;
			int z = coord.Z - time * velocity.Z;
			Coord ucoord(x, y, z);
			BASECLASS::Unlimbo(ucoord, DIR_N);

			Bounce.Init(Center_Coord(), Type->Elasticity, 1.4f, 0.0, velocity, 0.0);
		} else {
			BASECLASS::Unlimbo(coord, DIR_N);

			Coord center = Center_Coord() + Coord(0, 0, 10);
			int r1 = Scen->RandomNumber, r2 = Scen->RandomNumber, r3 = Scen->RandomNumber;
			float x = (abs(r3) % int(Type->MaxXYVel * 2)) - Type->MaxXYVel;
			float y = (abs(r2) % int(Type->MaxXYVel * 2)) - Type->MaxXYVel;
			float z = (abs(r1) % int(Type->MaxZVel - Type->MinZVel + 1.0)) + Type->MinZVel;

			Vector3 velocity(x, y, z);
			Bounce.Init(center, Type->Elasticity, 1.4f, 0.0, velocity, 0.0);
		}
	}

	if (Type == Rule->FlareAnim) {
		Map.RevealArea2((Coord *)&coord, Rule->DropZoneRadius / 256, PlayerPtr, 0, 0, 0, 1, 0);
		Map.RevealArea2((Coord *)&coord, Rule->DropZoneRadius / 256, PlayerPtr, 0, 0, 0, 1, 1);
	}

	loop = (loop > 1) ? loop : 1;
	RemainingIterations = loop * Type->LoopCount;
	RemainingIterations = (RemainingIterations > 1) ? RemainingIterations : 1;

	if (LoopDelay == 0) {
		Start();
	}
}

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
// _$E147

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

#include "houstype.h"
#include "infantry.h"
#include "infatype.h"

// FUNCTION: GAMEMD 0x00423AC0
void AnimClass::AI(void)
{
	if (!IsInert && Type->Report != -1) {
		FUN_00750D40(Center_Coord(), &StartSoundAudioController);
	}

	if (Type->IsFlamingGuy) {
		Flaming_Guy_AI();
		BASECLASS::AI();
	}

	if (Type->PsiWarning) {
		Invisible = !FUN_0043B4C0(Owner, &Map[Center_Coord()]);
	}

	if (Type == Rule->Behind) {
		if (!Options.ShowHidden) {
			Invisible = true;
		} else {
			Invisible = false;
		}
	}

	if (Type->HideIfNoOre) {
		CellClass * cellptr = Get_Cell_Ptr();
		if (cellptr != NULL && cellptr->Tiberium_Value() != 0) {
			Invisible = false;
		} else {
			Invisible = true;
		}
	}

	if (Type->MakeInfantry != -1) {
		Set_Occupy_Bit(PositionCoord);
	}

	if (Unpaused && PausedAnimFrame == Animation.Fetch_Stage()) {
		Unpaused = false;
	}

	if (HasExtras) {
		BounceResultType bounce_result = Bounce_AI();
		if (bounce_result == BOUNCE_SETTLED || bounce_result == BOUNCE_IMPACT) {
			bool water = Map[Get_Coord()].Land_Type() == LAND_WATER;
			bool bridge = PositionCoord.Z >= Map.Get_Height_GL(PositionCoord) + BRIDGE_LEPTON_HEIGHT;

			if (water && !bridge) {
				if (Type->IsMeteor) {
					new AnimClass(Rule->SplashList[Rule->SplashList.Count()-1], PositionCoord + Coord(0, 0, 3));
				} else {
					new AnimClass(Rule->Wake, PositionCoord);
					new AnimClass(Rule->SplashList[0], PositionCoord + Coord(0, 0, 3));
				}
			} else {
				if (Type->ExpireAnim != NULL) {
					Vector3 bouncecoord = Bounce.MyCoord;
					new AnimClass(Type->ExpireAnim, Coord(bouncecoord.X, bouncecoord.Y, bouncecoord.Z), 0, 1, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZGRAD), -30);
					FUN_00489280(Bounce.Get_Bounce_Coord(), Type->Damage, NULL, Type->Warhead, true, NULL);
					Combat_Lighting(Bounce.Get_Bounce_Coord(), Type->Damage, Type->Warhead);
				}
			}

			if (!water || bridge) {
				Coord coord = Bounce.Get_Bounce_Coord();
				if (Type->Spawns != NULL && Type->SpawnCount > 0) {
					int count = Random_Pick(0, Type->SpawnCount) + Random_Pick(0, Type->SpawnCount);
					for (int i = 0; i < count; i++) {
						new AnimClass(Type->Spawns, coord);
					}
				}

				Rect updaterect(0, 0, 0, 0);
				if (Type->IsTiberium && !bridge) {
					for (int x = -Type->TiberiumSpreadRadius; x <= Type->TiberiumSpreadRadius; x++) {
						for (int y = -Type->TiberiumSpreadRadius; y <= Type->TiberiumSpreadRadius; y++) {
							if ((int)fastmath::sqrt((double)x * (double)x + (double)y * (double)y) <= Type->TiberiumSpreadRadius) {
								CellClass * cellptr = &Map[Cell(AdjacentCell[x & 0x07] + Cell(coord))];
								if (cellptr->Can_Tiberium_Germinate(NULL) && Type->TiberiumSpawnType != NULL) {
									new OverlayClass(OverlayTypes[Type->TiberiumSpawnType->HeapID + Random_Pick(0, 3)], cellptr->Fetch_CellID());
									cellptr->OverlayData = Random_Pick(0, 2);
									Rect overlayrect = cellptr->Overlay_Render_Rect();
									overlayrect.Y -= TacticalRect.Y;
									updaterect = Union(updaterect, overlayrect);
								}
							}
						}
					}
					TacticalMap->Register_Dirty_Area(updaterect);
				}
			}

			Delete_Me();
			return;
		}
	}

	if (IsActive && !UnableToContinue) {
		if (Type->TrailerAnim != NULL && (Type->TrailerSeperation == 1 || (Frame % Type->TrailerSeperation == 0))) {
			new AnimClass(Type->TrailerAnim, Center_Coord(), 1);
		}
	}

	if (Type == Rule->FlareAnim && Map[Center_Coord()].Cell_Building()) {
		UnableToContinue = true;
	}

	if (UnableToContinue) {
		Delete_Me();
		return;
	}

	if (SkipProcessOnce) {
		SkipProcessOnce = false;
		return;
	}

	if (LoopDelay) {
		LoopDelay--;
		if (!LoopDelay) {
			Start();
		}
	} else if (IsActive) {
		if (Type->IsAnimatedTiberium) {
			OverlayType overlay = Map[Center_Coord() - Coord(CELL_LEPTON * 1.5, CELL_LEPTON * 1.5, 0)].Overlay;
			if (overlay == OVERLAY_NONE || OverlayTypes[overlay]->CellAnim != Type) {
				UnableToContinue = true;
			}
		}

		if (Type->End == -1) {
			Type->End = ((ShapeSet const *)Type->Get_Image_Data())->Get_Count();
			if (Type->Shadow) {
				Type->End /= 2;
			}
		}
		if (Type->LoopEnd == -1) {
			Type->LoopEnd = Type->End;
		}

		Mark(MARK_CHANGE);

		if (!PowerOff && !Paused && Animation.Graphic_Logic()) {
			int stage = Animation.Fetch_Stage();

			if (Type->Damage > 0 && !HasExtras) {
				if (OwnerObject != NULL && OwnerObject->Fetch_RTTI() == RTTI_TERRAIN) {
					Accum += Type->Damage * 5;
				} else {
					Accum += Type->Damage;
				}

				if (Accum >= 1 && !IsInert) {
					int damage = Accum;
					Accum -= damage;
					if (strcmp(Type->IniName, "INVISO") == 0) {
						FUN_00489280(Center_Coord(), damage, NULL, Rule->C4Warhead, true, NULL);
					} else {
						FUN_00489280(Center_Coord(), damage, NULL, Rule->FlameDamage2, true, NULL);
					}
					if (!IsActive) {
						return;
					}
				}
			}

			if (Type->MiddleFrameIndex && Type->Start+stage == Type->MiddleFrameIndex && !HasExtras) {
				Middle();
			}

			if (Type->PingPong) {
				if ((RemainingIterations <= 1 && (stage >= Type->End || stage == 0)) || (RemainingIterations > 1 && (stage >= Type->LoopEnd-Type->Start || stage == Type->Start))) {
					Animation.Set_Step(-Animation.Fetch_Step());
					return;
				}
			}

			if ((RemainingIterations <= 1 && stage >= Type->End) || (RemainingIterations > 1 && stage >= Type->LoopEnd-Type->Start) || (Type->Shadow && !Type->Reverse && !Reverse && stage >= Type->LoopEnd-Type->Start) || ((Type->Reverse || Reverse) && stage <= 0)) {
				if (RemainingIterations && RemainingIterations != UCHAR_MAX) RemainingIterations--;
				if (RemainingIterations) {
					if (!Type->Reverse && !Reverse) {
						Animation.Set_Stage(Type->LoopStart-Type->Start);
					} else {
						Animation.Set_Stage(Type->LoopEnd);
					}
					if (Type->RandomLoopDelay.Min != 0 || Type->RandomLoopDelay.Max != 0) {
						LoopDelay = Random_Pick(Type->RandomLoopDelay.Min, Type->RandomLoopDelay.Max);
					}
				} else {
					if (Type->Next != NULL) {
						Type = Type->Next;

						if (Type->End == -1) {
							Type->End = ((ShapeSet const *)Type->Get_Image_Data())->Get_Count();
							if (Type->Shadow) {
								Type->End /= 2;
							}
						}
						if (Type->LoopEnd == -1) {
							Type->LoopEnd = Type->End;
						}

						UnableToContinue = false;
						RemainingIterations = Type->LoopCount;
						Accum = 0;
						int delay = Type->Rate;
						if (Type->RandomRate.Min != 0 || Type->RandomRate.Max != 0) {
							delay = Random_Pick(Type->RandomRate.Min, Type->RandomRate.Max);
						}
						if (Type->Normalized) {
							Animation.Set_Rate(Options.Normalize_Delay(delay));
						} else {
							Animation.Set_Rate(delay);
						}
						Animation.Set_Stage(Type->Start);
						Start();
					} else if (Type->MakeInfantry != -1) {
						Clear_Occupy_Bit(PositionCoord);
						if (Type->MakeInfantry <= Rule->AnimToInfantry.Count()) {
							if (Owner == NULL || Owner->IsDefeated) {
								int count = Houses.Count();
								SideType side = FUN_006A46D0("Civilian");
								for (int i = 0; i < count; i++) {
									if (Houses[i]->Class->Side == side) {
										Owner = Houses[i];
										break;
									}
								}
							}
							if (Owner != NULL) {
								InfantryClass * infantry = (InfantryClass *)InfantryTypes[Rule->AnimToInfantry[Type->MakeInfantry]->HeapID]->Create_One_Of(Owner);
								if (infantry->Unlimbo(PositionCoord, DIR_SE)) {
									Cell cell = Center_Coord().As_Cell();
									CellClass * cellptr = &Map[cell];
									if (cellptr != NULL && cellptr->IsUnderBridge && PositionCoord.Z > cellptr->Center_Coord().Z) {
										infantry->Mark(MARK_UP);
										infantry->IsOnBridge = true;
										infantry->Mark(MARK_DOWN);
									}
									if (!Owner->IsHuman) {
										infantry->QueueMission(MISSION_HUNT, false);
									}
								} else {
									Animation.Set_Stage(Animation.Fetch_Stage() - 1);
									return;
								}
							}
						}
						TimeToDie = true;
						Delete_Me();
					} else {
						TimeToDie = true;
						Delete_Me();
					}
				}
			}
		}
	}
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
			Map.Update_Cell_Subzones(cptr->CellID);
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

static inline DirType _Direction(ObjectClass * object, Coord const & to)
{
	return(DirType().Direction(object->Center_Coord(), to));
}

static inline TPoint3D<int> _Flat_Diff(Coord b, Coord a)
{
	return(TPoint3D<int>(a.X - b.X, a.Y - b.Y, 0));
}

static inline int _Flat_Distance(Coord b, Coord a)
{
	return(_Flat_Diff(b, a).Length());
}

static inline Coord _Move_Coord(Coord const & start, DirType const & dir, int distance)
{
	double radians = dir.As_Radian();
	int y = start.Y - fastmath::sin(radians) * distance;
	int x = start.X + fastmath::cos(radians) * distance;
	Coord c;
	c.X = x;
	c.Y = y;
	c.Z = start.Z;
	return(c);
}

// FUNCTION: GAMEMD 0x00425670
void AnimClass::Flaming_Guy_AI(void)
{
	static const int _max_distance = 18;
	static const int _max_tries = 7;
	static const int _frame_divisor = 3;
	static const int _min_stage = 0;

	if (!FlamingGuyExpire) {
		Animation.Set_Rate(0);
	}

	if (FlamingGuyCoords != COORD_NONE && !FlamingGuyExpire) {
		if (_Flat_Distance(FlamingGuyCoords, PositionCoord) <= _max_distance) {
			if (!IsFalling) {
				Coord nextcoord = Next_Flaming_Guy_Coord();
				bool sink = (Map[FlamingGuyCoords].Land_Type() == LAND_WATER || Map[FlamingGuyCoords].Land_Type() == LAND_BEACH) && HeightAGL <= LEVEL_LEPTON_H;
				if (nextcoord != COORD_NONE && FlamingGuyRetries < _max_tries && !sink) {
					FlamingGuyRetries++;
					int z = PositionCoord.Z;
					Coord oldcoord = FlamingGuyCoords;
					if (z <= Map.Get_Height_GL(oldcoord) + 2 * LEVEL_LEPTON_H) {
						oldcoord.Z = Map.Get_Height_GL(oldcoord);
					} else {
						oldcoord.Z = Map.Get_Height_GL(oldcoord) + BRIDGE_LEPTON_HEIGHT;
					}
					PositionCoord = oldcoord;
					FlamingGuyCoords = nextcoord;
				} else {
					FlamingGuyCoords = COORD_NONE;
					FlamingGuyExpire = true;
					Animation.Set_Rate(1);
					Animation.Set_Stage(Type->RunningFrames * FACING_COUNT + 1);
					return;
				}
			}
		} else {
			int z = PositionCoord.Z;
			DirType direction = _Direction(this, FlamingGuyCoords);
			Coord newcoord = _Move_Coord(PositionCoord, direction, _max_distance);
			if (z <= Map.Get_Height_GL(newcoord) + 2 * LEVEL_LEPTON_H) {
				newcoord.Z = Map.Get_Height_GL(newcoord);
			} else {
				newcoord.Z = Map.Get_Height_GL(newcoord) + BRIDGE_LEPTON_HEIGHT;
			}
			PositionCoord = newcoord;
		}

		if (!IsFalling) {
			Cell position = PositionCoord.As_Cell();
			if (PositionCoord.Z >= Map.Get_Height_GL(PositionCoord) + BRIDGE_LEPTON_HEIGHT) {
				if (!Map[Get_Coord()].IsUnderBridge && !Map[Adjacent_Cell(position, FACING_W)].IsUnderBridge && !Map[Adjacent_Cell(position, FACING_N)].IsUnderBridge) {
					IsFalling = true;
				}
			}
		}
	} else {
		if (!IsFalling) {
			if (!FlamingGuyExpire) {
				Coord nextcoord = Next_Flaming_Guy_Coord();
				if (nextcoord != COORD_NONE) {
					FlamingGuyCoords = nextcoord;
				} else {
					FlamingGuyCoords = COORD_NONE;
					FlamingGuyExpire = true;
					Animation.Set_Rate(1);
					Animation.Set_Stage(Type->RunningFrames * FACING_COUNT + 1);
				}
			}
		}
	}

	if (!FlamingGuyExpire) {
		int stage = _min_stage;
		if (FlamingGuyCoords != COORD_NONE) {
			stage = Type->RunningFrames * (Facing_Sub(-1, ::Direction(Center_Coord(), FlamingGuyCoords).As_Dir8()));
		}
		Animation.Set_Stage(stage + Frame / _frame_divisor % Type->RunningFrames);
		return;
	} else {
		int stage = ((ShapeSet const *)Get_Image_Data())->Get_Count() / 2 - 1;
		if (Animation.Fetch_Stage() >= stage) {
			Delete_Me();
		}
	}
}

// FUNCTION: GAMEMD 0x00425D10
Coord AnimClass::Next_Flaming_Guy_Coord(void)
{
	bool any = false;

	Cell cell = PositionCoord.As_Cell();
	int mindist = 10000;
	Cell mincell = CELL_NONE;

	for (int x = cell.X - 5; x < cell.X + 5; x++) {
		any = true;
		for (int y = cell.Y - 5; y < cell.Y + 5; y++) {
			if (Map.In_Local_Radar(Cell(x, y))) {
				CellClass * cellptr = &Map[Cell(x, y)];
				if (cellptr->Land_Type() == LAND_WATER && !cellptr->IsUnderBridge && !cellptr->Adjacent_Cell(FACING_W).IsUnderBridge && !cellptr->Adjacent_Cell(FACING_N).IsUnderBridge) {
					int dist = cellptr->CellID != cell ? Point2D(x - cell.X, y - cell.Y).Length() : 0;
					if (x > cell.X || y > cell.Y) {
						dist -= 3;
					}
					if (dist < mindist) {
						mindist = dist;
						mincell = Cell(x, y);
					}
				}
			}
		}
	}

	if (any && mincell != CELL_NONE) {
		int dx = mincell.X - cell.X;
		int dy = mincell.Y - cell.Y;
		Point2D step(0, 0);
		if (dx < 0) {
			step.X = -1;
		} else if (dx > 0) {
			step.X = 1;
		}
		if (dy < 0) {
			step.Y = -1;
		} else if (dy > 0) {
			step.Y = 1;
		}

		if (Is_Valid_Flaming_Guy_Cell(cell + Cell(step.X, step.Y))) {
			return(Coord(Cell(cell.X + step.X, cell.Y + step.Y)));
		}

		if (Is_Valid_Flaming_Guy_Cell(cell + Cell(step.X, 0))) {
			return(Cell(cell.X + step.X, cell.Y).As_Coord());
		}

		if (Is_Valid_Flaming_Guy_Cell(cell + Cell(0, step.Y))) {
			return(Cell(cell.X, cell.Y + step.Y).As_Coord());
		}
	}

	if (CELL_NONE == Cell(0, 0)) {
		FacingType dir = Random_Pick(FACING_FIRST, FacingType(FACING_COUNT - 1));
		for (int i = 0; i < FACING_COUNT; i++) {
			FacingType facing = FacingType((dir + i) % FACING_COUNT);
			Cell const & offset = AdjacentCell[(int)facing & 7];
			Cell newcell = cell + offset;
			if (Is_Valid_Flaming_Guy_Cell(newcell)) {
				return(newcell.As_Coord());
			}
		}
	}

	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x004260F0
bool AnimClass::Is_Valid_Flaming_Guy_Cell(Cell const & cell)
{
	CellClass * cptr = &Map[cell];

	if (cptr->IsUnderBridge && Get_Coord().Z - LEVEL_LEPTON_H * cptr->Height > 2 * LEVEL_LEPTON_H) {
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
		if ((cptr->Flag.Occupy.Building || cptr->Flag.Occupy.Monolith || cptr->Flag.Occupy.Vehicle) || LEVEL_LEPTON_H * cptr->Height - Get_Coord().Z > 2 * LEVEL_LEPTON_H) {
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
	if (Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT <= coord.Z && cell.IsUnderBridge) {
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

	if (height + BRIDGE_LEPTON_HEIGHT <= coord.Z) {
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
LightConvertClass * ObjectClass::GetRemapColour(void) const
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

// FUNCTION: GAMEMD 0x004265B0
// DirType::Direction
template<class T>
bool TPoint2D<T>::operator == (TPoint2D<T> const & rvalue) const
{
	return(X==rvalue.X && Y==rvalue.Y);
}

// TEMPLATE: GAMEMD 0x00426620
// ??0?$CDTimerClass@VFrameTimerClass@@@@QAE@ABVNoInitClass@@@Z

// TEMPLATE: GAMEMD 0x00426630
// ?Value@?$CDTimerClass@VFrameTimerClass@@@@QBEHXZ
template class CDTimerClass<FrameTimerClass>;

#include <d3d.h>
#include "bound.h"
#include "zbuffer.h"
#include "dsurface.h"
#include "building.h"
namespace FPSCounter { extern unsigned CurrentFrameRate; }
namespace Detail { unsigned GetMinFrameRate(void); }
struct Triangle3D {
	void Set_Color(int a, int b, int c);
	void Set_Vertex(int index, float x, float y, float z, float u, float v);
	D3DTLVERTEX Vertex[3];
};
class Texture3DCache {
public:
	Texture3DCache(void);
	void FUN_004A3640(void);
	void FUN_004A3650(void);
	HRESULT Flush(LPDIRECTDRAWSURFACE target);
	int Add(Triangle3D const & triangle);
	Triangle3D List[1024];
	int Count;
};
extern Texture3DCache * DAT_008A0E10;

// FUNCTION: GAMEMD 0x00422CA0
void AnimClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	if (Game::bDirect3DIsUseable && DepthBuffer != NULL && Is_Type_Named("RING1")) {
		int elapsed = Animation.Fetch_Stage() * Animation.Fetch_Rate() - Animation.Timer.Value();
		int rate = Animation.Fetch_Rate();
		elapsed += rate;
		int duration = Stage_Count() * rate;
		int radius = elapsed + 8;
		Point2D top_left(point.X - (radius << 1), point.Y - radius);
		Point2D bottom_right(point.X + (radius << 1), point.Y + radius);
		int remaining = (duration - elapsed) << 8;
		int red = remaining / duration;
		red = Bound(red, 0, 255);
		int first_third = duration / 3;
		int green = elapsed < first_third ? (elapsed << 8) / first_third : remaining / (duration - first_third);
		green = Bound(green, 0, 255) * 2 / 3;
		int last_third = duration * 2 / 3;
		int blue = elapsed < last_third ? (elapsed << 8) / last_third : remaining / (duration - last_third);
		blue = Bound(blue, 0, 255);
		int zadjust = ZAdjust + Type->YDrawOffset - TacticalMap->Z_Lepton_To_Pixel(Height) - 2;
		unsigned depth = (unsigned short)(DepthBuffer->Get_Scroll() + DepthBuffer->Bounds.Y - point.Y - TacticalRect.Y);
		zadjust += depth;
		float upper_z = (float)((zadjust + radius) & 0xFFFF) / 65535.0f;
		float lower_z = (float)((zadjust - radius) & 0xFFFF) / 65535.0f;
		Triangle3D tri[2];
		tri[0].Set_Color(red, green, blue);
		tri[1].Set_Color(red, green, blue);
		tri[0].Set_Vertex(0, top_left.X, top_left.Y, upper_z, 0.0f, 0.0f);
		tri[0].Set_Vertex(1, top_left.X, bottom_right.Y, lower_z, 0.0f, 1.0f);
		tri[0].Set_Vertex(2, bottom_right.X, bottom_right.Y, lower_z, 1.0f, 1.0f);
		tri[1].Set_Vertex(0, top_left.X, top_left.Y, upper_z, 0.0f, 0.0f);
		tri[1].Set_Vertex(1, bottom_right.X, bottom_right.Y, lower_z, 1.0f, 1.0f);
		tri[1].Set_Vertex(2, bottom_right.X, top_left.Y, upper_z, 1.0f, 0.0f);
		DAT_008A0E10->Add(tri[0]);
		DAT_008A0E10->Add(tri[1]);
		return;
	}
	if (FPSCounter::CurrentFrameRate < Detail::GetMinFrameRate() && Type->DetailLevel > 1) {
		return;
	}
	if (!Invisible && Type->DetailLevel <= Options.DetailLevel && (!IsFogged || !Type->ShouldFogRemove)) {
		ShapeSet const * shapefile = (ShapeSet const *)Get_Image_Data();
		if (shapefile != NULL) {
			int flags = AnimFlags;
			int shapenum = Type->Start + Animation.Fetch_Stage();
			if (UnderTemporal) {
				if (Type->DoubleThick) {
					flags |= SHAPE_TRANSLUCENT75;
				}
				else {
					flags |= SHAPE_TRANSLUCENT50;
				}
			}

			if (Type->TranslucencyDetailLevel <= Options.DetailLevel) {
				if (Type->Translucent) {
					if (TranslucencyLevel >= 15) {
						return;
					}
					if (Animation.Fetch_Stage() > Type->End * 0.6) {
						flags |= SHAPE_TRANSLUCENT75;
					} else if (Animation.Fetch_Stage() > Type->End * 0.4) {
						flags |= SHAPE_TRANSLUCENT50;
					} else if (Animation.Fetch_Stage() > Type->End * 0.2) {
						flags |= SHAPE_TRANSLUCENT25;
					}
				} else if (Type->Translucency > 0) {
					if (TranslucencyLevel >= 15) {
						return;
					}
					if (Type->Translucency == 25) {
						flags |= SHAPE_TRANSLUCENT25;
					} else if (Type->Translucency == 50) {
						flags |= SHAPE_TRANSLUCENT50;
					} else if (Type->Translucency == 75) {
						flags |= SHAPE_TRANSLUCENT75;
					}
				} else if (TranslucencyLevel != 0) {
					if (TranslucencyLevel > 15) {
						return;
					}
					if (TranslucencyLevel > 10) {
						flags |= SHAPE_TRANSLUCENT50;
					} else if (TranslucencyLevel > 5) {
						flags |= SHAPE_TRANSLUCENT50;
					} else {
						flags |= SHAPE_TRANSLUCENT25;
					}
				}
			}

			if ((flags & SHAPE_DARKEN) == 0) {
				flags |= SHAPE_ALPHA;
			}

			int height = HeightAGL;
			int brightness = 1000;
			ConvertClass * convert;

			if (Type->IsVeins) {
				convert = (ConvertClass *)ColorSchemes[PlayerPtr->Scheme]->Converter;
				if (!Type->UseNormalLight) {
					brightness = Map[Render_Coord()].TileBrightness;
				}
			} else if (UseCellLightConvert) {
				CellClass * cellptr = &Map[Cell(Render_Coord())];
				if (cellptr->Drawer == NULL) {
					cellptr->Init_Drawer(0, 0x10000, 0, 1000, 1000, 1000);
				}
				convert = (ConvertClass *)cellptr->Drawer;
				if (!Type->UseNormalLight) {
					brightness = cellptr->TileBrightness;
				}
			} else if (LightConvert != NULL) {
				convert = (ConvertClass *)LightConvert;
				if (!Type->UseNormalLight) {
					brightness = TintColor;
				}
			} else {
				convert = FileSystem::ANIM_PAL;
				if (Type->AltPalette) {
					convert = (ConvertClass *)ColorSchemes[0]->Converter;
				}
				if (!Type->UseNormalLight) {
					brightness = Map[Cell(Render_Coord())].Brightness;
				}
			}

			if (HasExtras) {
				Draw_Shape(*LogicalSurface, *convert, shapefile, shapenum, point + Point2D(0, Type->YDrawOffset + TacticalMap->Z_Lepton_To_Pixel(height)), cliprect, ShapeFlags_Type(SHAPE_ZGRAD | SHAPE_WIN_REL | SHAPE_CENTER | SHAPE_DARKEN), 0, Type->YDrawOffset - TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_GROUND, 1000, 0, 0, 0, Point2D(0, 0));
			}

			WORD tint = 0;
			if (IsBuildingAnim) {
				BuildingClass * building = Map[Center_Coord()].Cell_Building();
				if (building != NULL) {
					if (building->Airstrike != NULL) {
						BYTE red = Rule->ColorAdd[Rule->LaserTargetColor].Get_Red();
						BYTE green = Rule->ColorAdd[Rule->LaserTargetColor].Get_Green();
						BYTE blue = Rule->ColorAdd[Rule->LaserTargetColor].Get_Blue();
						switch (DSurface::Get_Primary_Color_Mode()) {
						case 2: tint = (((red << 6) | green) << 5) | blue;
							// Fall through to accumulate the packed colors.
						case 1: tint |= (((green >> 1) | (red << 5)) << 6) | blue;
							// Fall through.
						default: tint |= (((green >> 1) | (red << 5)) << 5) | blue;
						}
					}
					if (building->IsIronCurtained() && building->ForceShielded == 1) {
						BYTE red = Rule->ColorAdd[Rule->ForceShieldColor].Get_Red();
						BYTE green = Rule->ColorAdd[Rule->ForceShieldColor].Get_Green();
						BYTE blue = Rule->ColorAdd[Rule->ForceShieldColor].Get_Blue();
						switch (DSurface::Get_Primary_Color_Mode()) {
						case 2: tint |= (((red << 6) | green) << 5) | blue;
							// Fall through to accumulate the packed colors.
						case 1: tint |= (((green >> 1) | (red << 5)) << 6) | blue;
							// Fall through.
						default: tint |= (((green >> 1) | (red << 5)) << 5) | blue;
						}
					}
				}
			}
			if (Map[Cell(Center_Coord())].Is_Shrouded()) {
				tint = 0;
			}

			if (Type->Tiled) {
				int frameheight = ((SHPStruct const *)shapefile)->GetFrameBounds(0).Height;
				Point2D origin = point;
				Point2D drawpoint = origin - Point2D(0, frameheight / 2);
				bool done = false;
				int height_offset = ZAdjust + Type->YDrawOffset - TacticalMap->Z_Lepton_To_Pixel(Height) - 50;
				while (!done) {
					Draw_Shape(*LogicalSurface, *FileSystem::ANIM_PAL, shapefile, shapenum, Point2D(origin.X, drawpoint.Y + Type->YDrawOffset), TacticalRect, ShapeFlags_Type(flags | SHAPE_ZGRAD), 0, height_offset, ZGRAD_90DEG, brightness, tint, 0, 0, Point2D(0, 0));
					if (drawpoint.Y < 0) {
						done = true;
					}
					drawpoint.Y -= frameheight;
					height_offset -= frameheight + frameheight / 2;
				}
			} else if (Type->Flat) {
				Draw_Shape(*LogicalSurface, *convert, shapefile, shapenum, point + Point2D(0, Type->YDrawOffset), cliprect, ShapeFlags_Type(flags | SHAPE_ZGRAD), 0, ZAdjust + Type->YDrawOffset - TacticalMap->Z_Lepton_To_Pixel(Height) - 3, ZGRAD_GROUND, brightness, tint, 0, 0, Point2D(0, 0));
			} else {
				Draw_Shape(*LogicalSurface, *convert, shapefile, shapenum, point + Point2D(0, Type->YDrawOffset), cliprect, ShapeFlags_Type(flags | SHAPE_ZGRAD), 0, ZAdjust + Type->YDrawOffset - TacticalMap->Z_Lepton_To_Pixel(Height) - 2, ZGRAD_90DEG, brightness, tint, 0, 0, Point2D(0, 0));
				if (Type->Shadow) {
					Draw_Shape(*LogicalSurface, *convert, shapefile, shapenum + shapefile->Get_Count() / 2, point, cliprect, ShapeFlags_Type((flags & ~SHAPE_TRANSLUCENT75) | SHAPE_WIN_REL | SHAPE_CENTER | SHAPE_DARKEN), 0, -2 - TacticalMap->Z_Lepton_To_Pixel(Height), ZGRAD_GROUND, 1000, 0, 0, 0, Point2D(0, 0));
				}
			}
		}
	}
}
