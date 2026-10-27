// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "object.h"
#include "_logic.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "assert.h"
#include "bomb.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "dsurface.h"
#include "foot.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "linetrail.h"
#include "logic.h"
#include "map.h"
#include "mono.h"
#include "objtype.h"
#include "rules.h"
#include "session.h"
#include "shapeset.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrtype.h"
#include "tag.h"
#include "tagtype.h"
#include "tracker.h"
#include "vector.h"
#include "voc.h"

// SYNTHETIC: GAMEMD 0x005F36F0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005F3720 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005F3740 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005F3760 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005F3780 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005F37A0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005F37C0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005F3800 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005F3830 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005F3860 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005F3890 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005F38A0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005F38C0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005F38E0 SYMBOL
// _$E47
namespace Unsorted { extern bool ArmageddonMode; }
namespace Game { extern bool IsActive; }
void __fastcall FUN_00731D00(int mode);
class SHPStruct
{
	public:
		short Type;
		short Width;
		short Height;
		short Count;
		Rect GetFrameBounds(int frame) const;
};
#define	GRAVITY	1.4

// FUNCTION: GAMEMD 0x005F3900
ObjectClass::ObjectClass(void) :
	BASECLASS(),
	Riser(0,0,0),
	Next(NULL),
	Tag(NULL),
	AttachedBomb(NULL),
	CustomSound(-1),
	BombVisible(false),
	Strength(255),
	EstimatedHealth(255),
	IsDown(false),
	IsToDamage(true),
	unknown_7C(0),
	IsToDisplay(false),
	IsInLimbo(true),
	InOpenToppedTransport(false),
	IsSelected(false),
	IsAnimAttached(false),
	Parachute(NULL),
	IsOnBridge(false),
	IsFalling(false),
	IsABomb(false),
	IsToExplode(false),
	IsActive(true),
	Layer(LAYER_NONE),
	IsSubmittedToLayer(false),
	IsVisible(true),
	Position(COORD_NONE),
	LineTrailer(NULL)
{
	AmbientSoundController.FUN_00405BE0();
	CustomSoundController.FUN_00405BE0();
	Objects.Add(this);
	ObjectPtrTracker.Add(this);
	AbstractTypePtrTracker.Add(this);
	TagPtrTracker.Add(this);
	AbstractFlags |= 2;
}

// FUNCTION: GAMEMD 0x005F3B50
ObjectClass::ObjectClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x005F3B80
ObjectClass::~ObjectClass(void)
{
	if (AttachedBomb != NULL) {
		AttachedBomb->Disarm();
	}
	ObjectsToDelete.Delete(this);
	Objects.Delete(this);
	ObjectPtrTracker.Delete(this);
	AbstractTypePtrTracker.Delete(this);
	TagPtrTracker.Delete(this);
	Next = NULL;
	IsActive = false;
	AmbientSoundController.Stop();
	CustomSoundController.Stop();
	AmbientSoundController.FUN_00405C00();
	CustomSoundController.FUN_00405C00();
	if (LineTrailer != NULL) {
		LineTrailer->~LineTrail();
		LineTrailer = NULL;
	}
	if (IsSubmittedToLayer) {
		Logic.Remove(this);
	}
}

// FUNCTION: GAMEMD 0x005F3D90
void ObjectClass::Clear_Line_Trail(void)
{
	if (LineTrailer != NULL) {
		LineTrailer->~LineTrail();
		LineTrailer = NULL;
	}
}

// FUNCTION: GAMEMD 0x005F3DB0
DirType AbstractClass::GetTargetDirection(AbstractClass const * target) const
{
	return(Direction(Center_Coord(), target->Center_Coord()));
}

// FUNCTION: GAMEMD 0x005F3E30
void const * ObjectClass::Get_Image_Data(void) const
{
	return(Class_Of()->Get_Image_Data());
}

// FUNCTION: GAMEMD 0x005F3E50
bool ObjectClass::Is_Type_Named(char const * name) const
{
	return(*Class_Of() == name);
}

// FUNCTION: GAMEMD 0x005F6060
void ObjectClass::Set_Height(int z)
{
	if (IsDown) {
		Mark(MARK_UP);
		Position.Z = z;
		Mark(MARK_DOWN);
	} else {
		Position.Z = z;
	}
}

// FUNCTION: GAMEMD 0x005F3E70
void ObjectClass::AI(void)
{
	if (!IsInLimbo) {
		if (Class_Of() != NULL && Class_Of()->AmbientSound != -1) {
			VocClass::PlayAt(Class_Of()->AmbientSound, Get_Coord(), &AmbientSoundController);
		}
		if (CustomSound != -1) {
			VocClass::PlayAt(CustomSound, Get_Coord(), &CustomSoundController);
		}
	}

	if (IsFalling) {
		LayerType layer = In_Which_Layer();

		Height += Riser.Z;
		if (HeightAGL <= 0) {
			HeightAGL = 0;
			IsFalling = false;
			Per_Cell_Process(PCP_END);

			if (Parachute != NULL) {
				Parachute->RemainingIterations = 0;
			}
		}

		if (!IsInLimbo) {
			if (IsAnimAttached) {
				Riser.Z -= 1;
				Riser.Z = MAX(Riser.Z, Rule->ParachuteMaxFallRate);
			} else {
				Riser.Z -= GRAVITY;
				Riser.Z = MAX(Riser.Z, Rule->NoParachuteMaxFallRate);
			}

			if (layer != In_Which_Layer()) {
				Map.Submit(this);
			}

			if (!IsFalling) {
				if (IsToExplode && Strength > 0) {
					int damage = Strength;
					Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
				}

				if (Fetch_RTTI() == RTTI_ANIM) {
					AnimClass * anim = (AnimClass *)this;
					if (anim->Type->IsFlamingGuy) {
						anim->FlamingGuyExpire = true;
						anim->Animation.Set_Rate(1);
						anim->Animation.Set_Stage(FACING_COUNT * anim->Type->RunningFrames + 1);
						if (Map[Get_Coord()].Land_Type() == LAND_WATER) {
							new AnimClass(Rule->SplashList[0], Get_Coord() + Coord(0, 0, 3));
						}
					}
				}
			}
		} else {
			Map.Remove(this);
		}
	}
}

// FUNCTION: GAMEMD 0x005F4160
void ObjectClass::Fall_From_Height(void)
{
	IsFalling = true;
	IsToExplode = true;

	Mark(MARK_UP);
	Map.Remove(this);
	IsOnBridge = false;
	Map.Submit(this);
	Mark(MARK_DOWN);

	if (Is_Foot()) {
		FootClass * foot = (FootClass *)this;
		if (!foot->Locomotion->Is_Moving()) {
			foot->Clear_Occupy_Bit(Get_Coord());
		} else {
			foot->Locomotion->Mark_All_Occupation_Bits(MARK_UP);
		}
	}
}

// FUNCTION: GAMEMD 0x005F4240
ActionType ObjectClass::What_Action(ObjectClass const *, bool) const
{
	return(ACTION_NONE);
}

// FUNCTION: GAMEMD 0x005F4250
ActionType ObjectClass::What_Action(Cell const &, bool, bool) const
{
	return(ACTION_NONE);
}

// FUNCTION: GAMEMD 0x005F4260
LayerType ObjectClass::In_Which_Layer(void) const
{
	if (!In_Air()) {
		return(LAYER_GROUND);
	}
	if (HeightAGL < Rule->JumpjetCruiseHeight) {
		return(LAYER_AIR);
	}
	return(LAYER_TOP);
}

// FUNCTION: GAMEMD 0x005F42A0
int ObjectClass::Get_Ownable(void) const
{
	return(0x7FFFFFFF);
}

// FUNCTION: GAMEMD 0x005F42B0
bool ObjectClass::Can_Repair(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F42C0
bool ObjectClass::Can_Demolish(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F42D0
bool ObjectClass::Can_Player_Fire(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F42E0
bool ObjectClass::Can_Player_Move(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F42F0
void ObjectClass::RegisterDestruction(TechnoClass *)
{
}

// FUNCTION: GAMEMD 0x005F4300
void ObjectClass::Record_The_Kill(HouseClass *)
{
}

// FUNCTION: GAMEMD 0x005F4310
void ObjectClass::Do_Shimmer(void)
{
}

// FUNCTION: GAMEMD 0x005F4320
int ObjectClass::Exit_Object(TechnoClass *, Cell)
{
	return(0);
}

// FUNCTION: GAMEMD 0x005F4330
void ObjectClass::Hidden(void)
{
}

// FUNCTION: GAMEMD 0x005F4340
void ObjectClass::Look(bool, bool)
{
}

// FUNCTION: GAMEMD 0x005F4350
bool ObjectClass::Active_Click_With(ActionType, ObjectClass *, bool)
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F4360
bool ObjectClass::Active_Click_With(ActionType, Cell const &, Cell const &, bool)
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F4370
void ObjectClass::Clicked_As_Target(int)
{
}

// FUNCTION: GAMEMD 0x005F4380
bool ObjectClass::In_Range(Coord const &, int) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F4390
int ObjectClass::Weapon_Range(int) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x005F43A0
void ObjectClass::Scatter(Coord const &, bool, bool)
{
}

// FUNCTION: GAMEMD 0x005F43B0
bool ObjectClass::Catch_Fire(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x005F43C0
void ObjectClass::Fire_Out(void)
{
}

// FUNCTION: GAMEMD 0x005F43D0
int ObjectClass::Value(void) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x005F43E0
MissionType ObjectClass::Get_Mission(void) const
{
	return(MISSION_NONE);
}

// FUNCTION: GAMEMD 0x005F43F0
void ObjectClass::Repair(int)
{
}

// FUNCTION: GAMEMD 0x005F4400
void ObjectClass::Sell_Back(int)
{
}

// FUNCTION: GAMEMD 0x005F4410
void ObjectClass::Move(FacingType facing)
{
	Mark(MARK_UP);
	Coord coord = Adjacent_Cell(PositionCoord, facing);
	if (Can_Enter_Cell(&Map[coord], facing) == MOVE_OK) {
		PositionCoord = coord;
	}
	Mark(MARK_DOWN);
}

// FUNCTION: GAMEMD 0x005F44A0
void ObjectClass::Unselect(void)
{
	if (IsSelected) {
		CurrentObject.Delete(this);
		IsSelected = false;
		if (Map.Object_To_Follow() == this) {
			Map.Set_To_Follow(NULL);
		}
	}
}

// FUNCTION: GAMEMD 0x005F4520
bool ObjectClass::Select(void)
{
	if (!Unsorted::ArmageddonMode && (IsInLimbo || IsSelected || !CanBeSelected())) {
		return(false);
	}

	TechnoClass * techno = Dynamic_Cast<TechnoClass *>(this);
	if (!Unsorted::ArmageddonMode && Can_Player_Move() && techno && techno->IsALoaner) {
		return(false);
	}
	if (techno && techno->IsBeingWarpedOut()) {
		return(false);
	}

	if (Map.PendingObject) {
		return(false);
	}

	if (CurrentObject.Count() > 0) {
		HouseClass * tryhptr = Owner_HouseClass();
		HouseClass * oldhptr = CurrentObject[0]->Owner_HouseClass();
		if (oldhptr->Is_Player_Control() != tryhptr->Is_Player_Control() || !oldhptr->Is_Player_Control()) {
			Unselect_All();
		}
	}

	if (Is_Techno() && ((TechnoTypeClass *)Class_Of())->IsLeader) {
		CurrentObject.Add_Head(this);
	} else {
		CurrentObject.Add(this);
	}

	FUN_00731D00(0);
	IsSelected = true;

	return(true);
}

// FUNCTION: GAMEMD 0x005F4730
Rect ObjectClass::Get_Visual_Rect(void) const
{
	Point2D point;
	TacticalMap->Coord_To_Pixel(Render_Coord(), point);

	SHPStruct const * shape = (SHPStruct const *)Get_Image_Data();
	if (shape == NULL) {
		return(RECT_NONE);
	}

	Rect shape_rect = shape->GetFrameBounds(0);
	Rect visual_rect(
		point.X + shape_rect.X - shape->Width/2,
		point.Y + shape_rect.Y - shape->Height/2,
		shape_rect.Width,
		shape_rect.Height);

	if (DSurface::ViewBounds.Is_Overlapping(visual_rect + DSurface::ViewBounds.TopLeft)) {
		return(visual_rect);
	}

	return(RECT_NONE);
}

// FUNCTION: GAMEMD 0x005F4870
Rect ObjectClass::Get_Render_Rect(void)
{
	Point2D drawpoint;
	TacticalMap->Coord_To_Pixel(Render_Coord(), drawpoint);

	SHPStruct const * sdata = (SHPStruct const *)Class_Of()->Get_Image_Data();
	if (sdata == NULL) {
		return(RECT_NONE);
	}
	int frame = 0;
	BuildingClass * bptr = (BuildingClass *)this;
	if (RTTI == RTTI_BUILDING && bptr->Class->IsFirestormWall) {
		frame = 15;
	}

	Rect rect1 = sdata->GetFrameBounds(frame);
	Rect rect2 = sdata->GetFrameBounds(frame + (sdata->Count / 2));

	int width = sdata->Width;
	int height = sdata->Height;

	Rect a = Union(rect1, rect2);

	if (RTTI == RTTI_BUILDING && bptr->Class->BibShape) {
		Rect rect2 = ((SHPStruct const *)bptr->Class->BibShape)->GetFrameBounds(0);
		a = Union(a, rect2);
	}

	Rect result(drawpoint.X + a.X - width / 2, drawpoint.Y + a.Y - height / 2, a.Width, a.Height);
	return(result);
}

// FUNCTION: GAMEMD 0x005F4B10
bool ObjectClass::Render(Rect & cliprect, bool forced, bool extras_only) const
{
	Point2D point;

	if (Unsorted::ArmageddonMode || !MainWindow || (forced || IsToDisplay) && !IsInLimbo) {
		IsToDisplay = false;

		if (TacticalMap->Coord_To_Pixel(Render_Coord(), point) || RTTI == RTTI_PARTICLESYSTEM) {

			cliprect = Intersect(cliprect, DSurface::ViewBounds);

			if (cliprect.X > DSurface::ViewBounds.X) {
				point.X += DSurface::ViewBounds.X - cliprect.X;
			}

			if (cliprect.Y > DSurface::ViewBounds.Y) {
				point.Y += DSurface::ViewBounds.Y - cliprect.Y;
			}

			Draw_It(point, cliprect);

			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F4D10
void ObjectClass::Mark_For_Redraw(void)
{
	if (!IsToDisplay) {
		IsToDisplay = true;
		Map.Flag_To_Redraw();
	}
}

// FUNCTION: GAMEMD 0x005F4D30
bool ObjectClass::Limbo(void)
{
	if (Game::IsActive && !IsInLimbo) {

		Unselect();

		Detach_All();

		Mark(MARK_UP);

		Map.Remove(this);

		AmbientSoundController.Stop();
		CustomSoundController.End();

		if (Class_Of() != NULL && Class_Of()->IsSentient && (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Fetch_ID() != -2)) {
			Logic.Remove(this);
		}

		if (Class_Of() != NULL && Class_Of()->AlphaImageData != NULL) {

			Point2D point;
			Coord coord = Center_Coord();
			TacticalMap->Coord_To_Pixel(coord, point);

			SHPStruct const * alpha = (SHPStruct const *)Class_Of()->AlphaImageData;
			point -= Point2D(alpha->Width/2, alpha->Height/2);

			TacticalMap->Register_Dirty_Area(Rect(point, alpha->Width, alpha->Height), true);
		}

		Hidden();
		IsInLimbo = true;
		IsToDisplay = false;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F4EC0
bool ObjectClass::Unlimbo(Coord const & coord, Dir256 )
{
	if (coord == COORD_NONE) {
		return(false);
	}

	if (GameActive && IsInLimbo && !IsDown) {
		if (ScenarioInit || Can_Enter_Cell(&Map[coord], FACING_NONE, -1, NULL, false) == MOVE_OK) {
			IsInLimbo = false;
			IsToDisplay = false;

			ObjectTypeClass const * objclass = Class_Of();
			Coord ucoord = coord;
			if (objclass != NULL) {
				ucoord = objclass->Coord_Fixup(coord);
			}
			Set_Coord(ucoord);

			if (Mark(MARK_DOWN)) {
				if (IsActive) {
					if (In_Which_Layer() != LAYER_NONE) {
						Map.Submit(this);
					}

					if (objclass != NULL) {

						if (objclass->IsSentient && (RTTI != RTTI_TERRAIN || (RTTI == RTTI_TERRAIN && ((TerrainTypeClass const *)objclass)->IsTiberiumSpawn)) && (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH || Fetch_ID() != -2)) {
							Logic.Submit(this);
						}

						if (objclass->AlphaImageData != NULL) {

							Point2D point;
							Coord center = Center_Coord();
							TacticalMap->Coord_To_Pixel(center, point);
							point += Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);

							SHPStruct const * alpha = (SHPStruct const *)Class_Of()->AlphaImageData;
							point -= Point2D(alpha->Width/2, alpha->Height/2);

							new AlphaShapeClass(this, point.X, point.Y);

							if (!ScenarioInit) {
								Point2D dpoint = point - Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY);
								TacticalMap->Register_Dirty_Area(Rect(dpoint, alpha->Width, alpha->Height), true);
							}
						}
					}

					if (Class_Of()->UseLineTrail) {
						LineTrailer = new LineTrail();
						if (LineTrailer) {
							RGBClass color = Rule->LineTrailColorOverride;
							if (color.Get_Red() == 0 && color.Get_Green() == 0 && color.Get_Blue() == 0) {
								RGBClass c = Class_Of()->LineTrailColor;
								LineTrailer->Color = c;
							} else {
								RGBClass c = Rule->LineTrailColorOverride;
								LineTrailer->Color = c;
							}
							LineTrailer->Set_Decrement(Class_Of()->LineTrailColorDecrement);
							LineTrailer->Owner = this;
						}
					}
				}
				return(true);
			} else {
				IsInLimbo = true;
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F5230
void ObjectClass::Detach(AbstractClass const * target, bool all)
{
	if (Tag == target) {
		if (Tag) {
			Tag->AttachCount--;
			Tag = NULL;
		}
	}

	if (all) {
		if (target == Next) {
			if (target) {
				Next = Next->Next;
			}
		}
	}

	if (target == Parachute) {
		Parachute = NULL;
	}
}

// FUNCTION: GAMEMD 0x005F5280
void ObjectClass::Detach_All(bool all)
{
	if (LineTrailer != NULL) {
		LineTrailer->~LineTrail();
		LineTrailer = NULL;
	}

	HouseClass * owner = Owner_HouseClass();
	bool sensed = false;
	bool unselect = false;

	if (all) {
		unselect = true;
	} else {
		if (Is_Techno()) {
			if (((TechnoClass *)this)->Is_Sensed_By_Player()) {
				sensed = true;
			}
		}
		if (!(owner != NULL && (owner->Is_Player_Control() || sensed))) {
			unselect = true;
		}
	}

	if (unselect) {
		Unselect();
	}

	if (Map.Object_To_Follow() == this) {
		Map.Set_To_Follow(NULL);
	}

	Detach_This_From_All(this, all);
}

// FUNCTION: GAMEMD 0x005F5320
RadioMessageType ObjectClass::Receive_Message(RadioClass *, RadioMessageType message, int & )
{
	switch (message) {
		case RADIO_REDRAW:
			Mark(MARK_CHANGE);
			return(RADIO_ROGER);

		case RADIO_NEED_REPAIR:
			if ((double)Strength / Class_Of()->MaxStrength >= Rule->ConditionGreen) {
				return(RADIO_NEGATIVE);
			}
			return(RADIO_ROGER);

		default:
			break;
	}
	return(RADIO_STATIC);
}

// FUNCTION: GAMEMD 0x005F5390
ResultType ObjectClass::Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced, bool, HouseClass * killer)
{
	ResultType result = RESULT_NONE;
	int oldstrength = Strength;

	if (oldstrength > 0 && damage != 0 && (forced || !Class_Of()->IsImmune)) {
		int maxstrength = Class_Of()->MaxStrength;

		if (!forced) {
			damage = Modify_Damage(damage, warhead, Class_Of()->Armor, distance);
		}
		if (this && RTTI == RTTI_BUILDING && !((RTTI == RTTI_BUILDING) ? (BuildingClass *)this : (BuildingClass *)NULL)->Class->CanC4) {
			int clamped = damage;
			if (clamped < 1) {
				clamped = 1;
			}
			damage = clamped;
		}
		if (damage == 0) return(RESULT_NONE);

		if (damage < 0) {
			int oldstrength = Strength;
			Strength -= damage;
			if (Strength > maxstrength) {
				Strength = maxstrength;
			}
			if (oldstrength != Strength) {
				Clicked_As_Target(7);
			}
			return(RESULT_NONE);
		}

		result = RESULT_LIGHT;

		if (oldstrength > damage) {

			if (oldstrength >= (maxstrength >> 1) && (oldstrength-damage) < (maxstrength >> 1)) {
				result = RESULT_HALF;
			}
		} else {
			damage = oldstrength;
		}

		if (oldstrength > maxstrength * Rule->ConditionRed && (oldstrength - damage) < maxstrength * Rule->ConditionRed) {
			result = RESULT_MAJOR;
		}

		Strength = oldstrength - damage;

		if (Strength <= 0 && RTTI == RTTI_INFANTRY && !forced) {
			InfantryClass * inf = (InfantryClass*)this;
			if (inf->Class->IsCyborg && !inf->IsProne) {
				new AnimClass(Rule->InfantryExplode, Get_Coord(), 0, 1, ShapeFlags_Type(0x600), 0, false);
				int strength = inf->Class->MaxStrength;
				strength *= 0.25;
				Strength = strength;
				if (strength <= 1)	{
					strength = 1;
				}
				Strength = strength;
				inf->IsProne = true;
				inf->Do_Action(DO_CRAWL, true);
				result = RESULT_MAJOR;
			}
		}

		int pretag_strength = Strength;

		if (result == RESULT_HALF) {
			if (source && Tag != NULL) Tag->Spring(TEVENT_ENTER_YELLOW, this);

			if (IsActive) {
				if (Tag != NULL) Tag->Spring(TEVENT_ENTER_YELLOW_ANY, this);
			} else {
				return(RESULT_ALREADY_DESTROYED);
			}
		}

		if (IsActive) {
			if (result == RESULT_MAJOR) {
				if (source && Tag != NULL) Tag->Spring(TEVENT_ENTER_RED, this);

				if (IsActive) {
					if (Tag != NULL) Tag->Spring(TEVENT_ENTER_RED_ANY, this);
				} else {
					return(RESULT_ALREADY_DESTROYED);
				}
			}
		}

		if (IsActive) {
			if (Strength != oldstrength && Class_Of() != NULL && oldstrength == Class_Of()->MaxStrength) {
				if (source && Tag != NULL) Tag->Spring(TEVENT_FIRST_DAMAGED, this);
				if (Tag != NULL && IsActive) Tag->Spring(TEVENT_FIRST_DAMAGED_ANY, this);

				if (Tag != NULL) {
					if (IsActive) {
						if (source) Tag->Spring(TEVENT_FIRST_DAMAGED_ANY, this, CELL_NONE, false, source);
					} else {
						return(RESULT_ALREADY_DESTROYED);
					}
				}
			}
		} else {
			return(RESULT_ALREADY_DESTROYED);
		}

		if (IsActive && (pretag_strength <= 0 || Strength > 0)) {

			if (IsActive) {
				HouseClass * source_house = NULL;
				if (Strength == 0) {
					if (source) {
						source_house = source->House;
					}
					if (killer != NULL && (source == NULL || killer != source_house)) {
						Record_The_Kill(killer);
					} else {
						RegisterDestruction(source);
					}
					result = RESULT_DESTROYED;
					Detach_All(true);
				}
			}

			if (IsActive) {
				if (source) {
					if (Tag && result != RESULT_DESTROYED) {
						Tag->Spring(TEVENT_ATTACKED, this, CELL_NONE, 0, source);
					}
				}
			}

			if (IsActive) {
				if (source) {
					if (Tag && result != RESULT_DESTROYED) {
						Tag->Spring(TEVENT_ATTACKED_BY, this, CELL_NONE, 0, source);
					}
				}
			}

			if (IsActive) {
				if (result != RESULT_NONE && IsSelected) {
					Mark(MARK_CHANGE);
				}
			}
		} else {
			return(RESULT_ALREADY_DESTROYED);
		}
	}

	return(result);
}

// FUNCTION: GAMEMD 0x005F5850
bool ObjectClass::Mark(MarkType mark)
{
	if (!IsInLimbo) {

		if (mark == MARK_CHANGE) {
			if (IsToDisplay) return(false);
			if (IsDown) {
				Mark_For_Redraw();
				return(true);
			}
			return(false);
		}

		if (RTTI != RTTI_BUILDING || !((BuildingClass *)this)->Class->ToTile || mark != MARK_UP) {
			TechnoClass * tech = Dynamic_Cast<TechnoClass *>(this);
			if (tech != NULL) {
				int threat = tech->Risk();
				HousesType house = tech->Owner();
				Cell cell = Get_Cell();
			}
		}

		if ((mark == MARK_DOWN || mark == MARK_DOWN_FORCED) && !IsDown) {
			IsDown = true;
			Mark_For_Redraw();
			return(true);
		}

		if (mark == MARK_UP && IsDown) {
			IsDown = false;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F5930
bool ObjectClass::Revealed(HouseClass * house)
{
	return(house != NULL);
}

// FUNCTION: GAMEMD 0x005F5940
bool ObjectClass::Paradrop(Coord const & coord)
{
	CellClass * cellptr;
	AnimClass * anim;

	if (Map.In_Local_Radar(coord)) {
		IsFalling = true;
		cellptr = &Map[coord];
		if (cellptr != NULL) {
			bool ok = true;
			if (cellptr->IsUnderBridge) {
				IsOnBridge = true;
				if (!cellptr->IsBridgeTraversable) {
					ok = false;
				}
			}
			if (ok) {
				if (Is_Techno()) {
					MZoneType mz = Techno_Type_Class()->MZone;
					Cell cell = coord.As_Cell();
					ok = cellptr->Is_Clear_To_Move(Techno_Type_Class()->Speed, false, false, Map.Get_Cell_Zone(cell, mz, IsOnBridge), mz, -1, true);
				}
				if (ok && Unlimbo(coord, (Dir256)0x80)) {
					Set_Coord(coord);

					if (Fetch_RTTI() == RTTI_BULLET) {
						anim = new AnimClass(Rule->BombParachute, coord, 0, 1, (ShapeFlags_Type)0x600, 0, false);
					} else {
						Coord drop = coord;
						drop.Z += 75;
						anim = new AnimClass(Rule->Parachute, drop, 0, 1, (ShapeFlags_Type)0x600, 0, false);
						Parachute = anim;
					}
					if (anim != NULL) {
						anim->SetOwnerObject(this);
						if (Fetch_RTTI() != RTTI_BULLET) {
							anim->LightConvert = (LightConvertClass *)GetRemapColour();
							anim->TintColor = Get_Cell_Ptr()->Brightness;
						}
					}
					return(true);
				}
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F5B50
bool ObjectClass::Attach_Tag(TagClass * tag)
{
	if (Tag != NULL) {
		TagClass * tptr = Tag;
		tptr->AttachCount--;
		Tag = NULL;
	}

	if (tag) {
		Tag = tag;
		tag->AttachCount++;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F5B90
Cell const * ObjectClass::Occupy_List(bool placement) const
{
	static Cell const _list[] = {REFRESH_EOL};

	if (Class_Of() == NULL) {
		return(_list);
	}

	return(Class_Of()->Occupy_List(placement));
}

// SYNTHETIC: GAMEMD 0x005F5C10 SYMBOL
// _$E165

// FUNCTION: GAMEMD 0x005F5C20
BuildingClass * ObjectClass::Who_Can_Build_Me(bool intheory, bool legal) const
{
	return(Class_Of()->Who_Can_Build_Me(intheory, legal, false, Owner_HouseClass()));
}

// FUNCTION: GAMEMD 0x005F5C60
double ObjectClass::Get_Health_Ratio(void) const
{
	return((double)Strength / Class_Of()->MaxStrength);
}

// FUNCTION: GAMEMD 0x005F5C80
void ObjectClass::Set_Health_Ratio(double health)
{
	if (health > 0) {
		Strength = (int)(Class_Of()->MaxStrength * health);
		if (Strength == 0) Strength = 1;
	} else {
		Strength = 0;
	}
}

// FUNCTION: GAMEMD 0x005F5CD0
bool ObjectClass::Is_Red_HP(void) const
{
	return(HealthRatio <= Rule->ConditionRed && Strength > 0);
}

// FUNCTION: GAMEMD 0x005F5D20
bool ObjectClass::Is_Yellow_HP(void) const
{
	return(HealthRatio > Rule->ConditionRed && HealthRatio <= Rule->ConditionYellow);
}

// FUNCTION: GAMEMD 0x005F5D90
bool ObjectClass::Is_Green_HP(void) const
{
	return(HealthRatio > Rule->ConditionYellow);
}

// FUNCTION: GAMEMD 0x005F5DD0
HealthState ObjectClass::GetHealthStatus(void) const
{
	if (Is_Red_HP()) {
		return(HEALTH_RED);
	}
	if (Is_Yellow_HP()) {
		return(HEALTH_YELLOW);
	}
	return(HEALTH_GREEN);
}

// FUNCTION: GAMEMD 0x005F5E80
HRESULT STDMETHODCALLTYPE ObjectClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		Swizzle_Pointer(&Next);
		Swizzle_Pointer(&Tag);
		Swizzle_Pointer(&AttachedBomb);
		Swizzle_Pointer((void **)&unknown_18);
		Swizzle_Pointer(&Parachute);
		AmbientSoundController.FUN_00405BE0();
		CustomSoundController.FUN_00405BE0();
		LineTrailer = NULL;
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005F5F00
int ObjectClass::Get_Cell_Height(void) const
{
	CellClass * cell = Get_Cell_Ptr();
	return(cell->Height + (IsOnBridge ? BRIDGE_CELL_HEIGHT : 0));
}

// FUNCTION: GAMEMD 0x005F5F30
int ObjectClass::Get_Height(void) const
{
	return(Position.Z);
}

// FUNCTION: GAMEMD 0x005F5F40
int ObjectClass::Get_Height_AGL(void) const
{
	int height = Position.Z - Map.Get_Height_GL(PositionCoord);
	if (IsOnBridge) {
		height -= BRIDGE_LEPTON_HEIGHT;
	}
	return(height);
}

// FUNCTION: GAMEMD 0x005F5FA0
void ObjectClass::Set_Height_AGL(int height)
{
	if (IsOnBridge) {
		height += BRIDGE_LEPTON_HEIGHT;
	}
	if (IsDown) {
		Mark(MARK_UP);
		Position.Z = height + Map.Get_Height_GL(PositionCoord);
		Mark(MARK_DOWN);
	} else {
		Position.Z = height + Map.Get_Height_GL(PositionCoord);
	}
}

// FUNCTION: GAMEMD 0x005F60A0
void ObjectClass::Set_Occupy_Bit(Coord const & coord)
{
	if (Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT <= coord.Z && Map[coord].IsUnderBridge) {
		Map[coord].BridgeFlag.Occupy.Monolith = true;
	} else {
		Map[coord].Flag.Occupy.Monolith = true;
	}
}

// FUNCTION: GAMEMD 0x005F6120
void ObjectClass::Clear_Occupy_Bit(Coord const & coord)
{
	if (Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT <= coord.Z && Map[coord].IsUnderBridge) {
		Map[coord].BridgeFlag.Occupy.Monolith = false;
	} else {
		Map[coord].Flag.Occupy.Monolith = false;
	}
}

// FUNCTION: GAMEMD 0x005F61A0
VoxelDataStruct::~VoxelDataStruct(void)
{
	delete VoxLib;
	VoxLib = NULL;
	delete MotLib;
	MotLib = NULL;
}

// FUNCTION: GAMEMD 0x005F61F0
bool ObjectClass::operator < (ObjectClass const & object) const
{
	return(Sort_Y() < object.Sort_Y());
}

// FUNCTION: GAMEMD 0x005F6220
bool ObjectClass::operator > (ObjectClass const & object) const
{
	return((Sort_Y() > object.Sort_Y()) ? true : false);
}

// FUNCTION: GAMEMD 0x005F6250
void ObjectClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	if (Next != NULL) {
		crc(Next->Fetch_ID());
	}

	if (Tag != NULL) {
		crc(Tag->Fetch_ID());
	}

	crc(Strength);
	crc(IsDown);
	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
		crc(IsToDisplay);
		crc(IsSelected);
	}
	crc(IsInLimbo);
	crc(IsAnimAttached);
	crc(IsOnBridge);
	crc(IsFalling);
	crc(IsToExplode);
	crc(IsActive);
	crc(Position.X);
	crc(Position.Y);
	crc(Position.Z);
}

// FUNCTION: GAMEMD 0x005F6360
int ObjectClass::Distance(AbstractClass const * target) const
{
	if (target == NULL) {
		return(0);
	}
	int dist = Center_Coord().Distance_To(target->Center_Coord());
	BuildingClass * obj = target->RTTI == RTTI_BUILDING ? (BuildingClass *)target : NULL;
	if (obj) {
		dist -= ((obj->Class->Width() + obj->Class->Height()) * (CELL_LEPTON / 4));
		if (dist < 0) dist = 0;
	}
	return(dist);
}

// FUNCTION: GAMEMD 0x005F6440
int ObjectClass::Planar_Distance(AbstractClass const * target) const
{
	if (target == NULL) {
		return(0);
	}
	int dist = Point2D(Center_Coord()).Distance_To(Point2D(target->Center_Coord()));
	BuildingClass * obj = target->RTTI == RTTI_BUILDING ? (BuildingClass *)target : NULL;
	if (obj) {
		dist -= ((obj->Class->Width() + obj->Class->Height()) * (CELL_LEPTON / 4));
		if (dist < 0) dist = 0;
	}
	return(dist);
}

// FUNCTION: GAMEMD 0x005F6500
int ObjectClass::Relative_Distance(AbstractClass const * target) const
{
	if (target == NULL) {
		return(0);
	}

	Coord center = Center_Coord();
	Coord target_center = target->Center_Coord();
	Point2D pt(center.X - target_center.X, center.Y - target_center.Y);
	return((pt.X * pt.X) + (pt.Y * pt.Y));
}

// FUNCTION: GAMEMD 0x005F6560
int ObjectClass::Relative_Distance(Coord const & coord) const
{
	Coord center = Center_Coord();
	Point2D pt(center.X - coord.X, center.Y - coord.Y);
	return((pt.X * pt.X) + (pt.Y * pt.Y));
}

// FUNCTION: GAMEMD 0x005F65A0
Coord ObjectClass::Center_Coord(void) const
{
	return(PositionCoord);
}

// FUNCTION: GAMEMD 0x005F65D0
void ObjectClass::Editor_Draw_It(Point2D const & point, Rect const & cliprect) const
{
	Draw_It(point, cliprect);
}

// FUNCTION: GAMEMD 0x005F65F0
void ObjectClass::Delete_Me(void)
{
	if (AttachedBomb != NULL) {
		AttachedBomb->Disarm();
	}
	if (this != NULL && (AbstractFlags & 1)) {
		((TechnoClass *)this)->Kill_Cargo(NULL);
	}
	Detach_This_From_All(this, true);
	Limbo();
	IsActive = false;
	ObjectsToDelete.Add(this);
}

// FUNCTION: GAMEMD 0x005F6690
bool ObjectClass::Is_Inactive(void) const
{
	return(IsActive==false);
}

// FUNCTION: GAMEMD 0x005F66A0
Rect Vector_Rect(DynamicVectorClass<ObjectClass *> const & list)
{
	Rect bounds(0, 0, 0, 0);

	if (list.Count() > 0) {
		bounds.X = list[0]->PositionCoord.X;
		bounds.Y = list[0]->PositionCoord.Y;

		for (int i = list.Count() - 1; i >= 0; i--) {
			Coord coord = list[i]->PositionCoord;
			int x = coord.X;
			int y = coord.Y;

			if (x < bounds.X) {
				bounds.Width += bounds.X - x;
				bounds.X = x;
			}
			if (y < bounds.Y) {
				bounds.Height += bounds.Y - y;
				bounds.Y = y;
			}
			if (x > bounds.X + bounds.Width) {
				bounds.Width = x - bounds.X;
			}
			if (y > bounds.Y + bounds.Height) {
				bounds.Height = y - bounds.Y;
			}
		}
	}
	return(bounds);
}

// FUNCTION: GAMEMD 0x005F6790
Coord Vector_Center(DynamicVectorClass<ObjectClass *> const & list)
{
	Coord center = Coord(0, 0, 0);
	int count = list.Count();
	for (int i = count - 1; i >= 0; i--) {
		center += list[i]->PositionCoord;
	}
	center.X /= count;
	center.Y /= count;
	center.Z /= count;
	return(center);
}

// FUNCTION: GAMEMD 0x005F6810
ObjectClass * Vector_Closest_Object(DynamicVectorClass<ObjectClass *> const & list, Coord const & coord)
{
	ObjectClass * closest = NULL;
	int mindist = 99999999;

	for (int i = list.Count() - 1; i >= 0; i--) {
		if (closest == NULL) {
			closest = list[i];
			mindist = coord.Distance_To(closest->PositionCoord);
		} else {
			int dist = Distance(coord, list[i]->PositionCoord);
			if (dist < mindist) {
				closest = list[i];
				mindist = dist;
			}
		}
	}
	return(closest);
}

// FUNCTION: GAMEMD 0x005F6940
void ObjectClass::Set_Coord(Coord const & coord)
{
	Position = coord;
}

// FUNCTION: GAMEMD 0x005F6960
CellClass * ObjectClass::Get_Cell_Ptr(void) const
{
	Map[Get_Coord()];
	return(&Map[Get_Coord()]);
}

// FUNCTION: GAMEMD 0x005F69C0
Cell ObjectClass::Get_Target_Cell(void) const
{
	return(Destination_Coord().As_Cell());
}

// FUNCTION: GAMEMD 0x005F6A10
CellClass * ObjectClass::Get_Target_Cell_Ptr(void) const
{
	return(&Map[Destination_Coord().As_Cell()]);
}

// FUNCTION: GAMEMD 0x005F6A70
bool ObjectClass::Is_Moving_Onto_Bridge(TechnoClass * docker) const
{
	bool bridge = IsOnBridge;

	Coord destination = Destination_Coord(docker);
	int dest_height = Map.Get_Height_GL(destination);
	int current_height = Map.Get_Height_GL(PositionCoord);

	if (!IsOnBridge) {
		if (current_height - dest_height > 3 * LEVEL_LEPTON_H) {
			if (Map[destination].IsUnderBridge) {
				return(true);
			}
		}
	}

	if (IsOnBridge) {
		if (dest_height - current_height > 3 * LEVEL_LEPTON_H) {
			return(false);
		}
	}
	return(bridge);
}

// FUNCTION: GAMEMD 0x005F6B50
void ObjectClass::Set_Waypoint_Path(PathType path, char index)
{
}

// FUNCTION: GAMEMD 0x005F6B60
bool ObjectClass::On_Ground(void) const
{
	if (IsDown) {
		if (Get_Height_AGL() < LEVEL_LEPTON_H * 2) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F6B90
bool ObjectClass::In_Air(void) const
{
	if (IsDown) {
		if (Get_Height_AGL() >= LEVEL_LEPTON_H * 2) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F6BC0
TechnoTypeClass const * ObjectClass::Techno_Type_Class(void) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x005F6BD0
int ObjectClass::Sort_Y(void) const
{
	return(Render_Coord().Y + Render_Coord().X);
}

// FUNCTION: GAMEMD 0x005F6C10
bool ObjectClass::Considered_Vehicle(void) const
{
	return(Get_Height_AGL() > -20);
}

// FUNCTION: GAMEMD 0x005F6C30
bool ObjectClass::CanBeSelected(void) const
{
	if (IsDisguisedAs(PlayerPtr)) {
		if (GetDisguiseHouse(true) == NULL) {
			return(false);
		}
	}
	return(Class_Of()->IsSelectable);
}

// FUNCTION: GAMEMD 0x005F6C70
bool ObjectClass::CanBeSelectedNow(void) const
{
	return(CanBeSelected());
}

// FUNCTION: GAMEMD 0x005F6C80
Coord ObjectClass::Docking_Coord(TechnoClass * ) const
{
	return(Center_Coord());
}

// FUNCTION: GAMEMD 0x005F6CB0
void ObjectClass::FUN_005F6CB0(int sound)
{
	CustomSound = sound;
	if (sound == -1) {
		CustomSoundController.End();
	}
}

// FUNCTION: GAMEMD 0x005F6CD0
bool ObjectClass::IsCrushable(TechnoClass * crusher) const
{
	if (crusher != NULL && crusher->Techno_Type_Class()->OmniCrusher && this != NULL && (AbstractFlags & 1) && !Techno_Type_Class()->OmniCrushResistant && Fetch_RTTI() != RTTI_BUILDING && !crusher->House->Is_Ally(this) && !IsIronCurtained()) {
		return(true);
	}
	if (Class_Of()->IsCrushable && this != NULL && (AbstractFlags & 1) && !((TechnoClass *)this)->Uncrushable && !crusher->House->Is_Ally(this) && !IsIronCurtained()) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F6DA0
void ObjectClass::AnimPointerExpired(AnimClass * anim)
{
	if (anim == Parachute) {
		Parachute = 0;
	}
}

// SYNTHETIC: GAMEMD 0x005F6DC0
// ??_GObjectClass@@UAEPAXI@Z
