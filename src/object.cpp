// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
#include "tag.h"
#include "tagtype.h"
#include "tracker.h"
#include "vector.h"
#include "voc.h"
#define	GRAVITY	1.4

// FUNCTION: 0x005F3900
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

// FUNCTION: 0x005F3B50
ObjectClass::ObjectClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x005F3B80
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

// FUNCTION: 0x005F3D90
void ObjectClass::Clear_Line_Trail(void)
{
	if (LineTrailer != NULL) {
		LineTrailer->~LineTrail();
		LineTrailer = NULL;
	}
}

// FUNCTION: 0x005F3DB0
DirType AbstractClass::GetTargetDirection(AbstractClass const * target) const
{
	return(Direction(Center_Coord(), target->Center_Coord()));
}

// FUNCTION: 0x005F3E30
void const * ObjectClass::Get_Image_Data(void) const
{
	return(Class_Of()->Get_Image_Data());
}

// FUNCTION: 0x005F3E50
bool ObjectClass::Is_Type_Named(char const * name) const
{
	return(*Class_Of() == name);
}

inline void ObjectClass::Set_Height(int z)
{
	if (IsDown) {
		Mark(MARK_UP);
		Position.Z = z;
		Mark(MARK_DOWN);
	} else {
		Position.Z = z;
	}
}

// FUNCTION: 0x005F3E70
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

// FUNCTION: 0x005F4160
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

// FUNCTION: 0x005F4240
ActionType ObjectClass::What_Action(ObjectClass const *, bool) const
{
	return(ACTION_NONE);
}

// FUNCTION: 0x005F4250
ActionType ObjectClass::What_Action(Cell const &, bool, bool) const
{
	return(ACTION_NONE);
}

// FUNCTION: 0x005F4260
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

// FUNCTION: 0x005F42A0
int ObjectClass::Get_Ownable(void) const
{
	return(0x7FFFFFFF);
}

// FUNCTION: 0x005F42B0
bool ObjectClass::Can_Repair(void) const
{
	return(false);
}

// FUNCTION: 0x005F42C0
bool ObjectClass::Can_Demolish(void) const
{
	return(false);
}

// FUNCTION: 0x005F42D0
bool ObjectClass::Can_Player_Fire(void) const
{
	return(false);
}

// FUNCTION: 0x005F42E0
bool ObjectClass::Can_Player_Move(void) const
{
	return(false);
}

// FUNCTION: 0x005F42F0
void ObjectClass::RegisterDestruction(TechnoClass *)
{
}

// FUNCTION: 0x005F4300
void ObjectClass::Record_The_Kill(TechnoClass *)
{
}

// FUNCTION: 0x005F4310
void ObjectClass::Do_Shimmer(void)
{
}

// FUNCTION: 0x005F4320
int ObjectClass::Exit_Object(TechnoClass *, Cell)
{
	return(0);
}

// FUNCTION: 0x005F4330
void ObjectClass::Hidden(void)
{
}

// FUNCTION: 0x005F4340
void ObjectClass::Look(bool, bool)
{
}

// FUNCTION: 0x005F4350
bool ObjectClass::Active_Click_With(ActionType, ObjectClass *, bool)
{
	return(false);
}

// FUNCTION: 0x005F4360
bool ObjectClass::Active_Click_With(ActionType, Cell const &, Cell const &, bool)
{
	return(false);
}

// FUNCTION: 0x005F4370
void ObjectClass::Clicked_As_Target(int)
{
}

// FUNCTION: 0x005F4380
bool ObjectClass::In_Range(Coord const &, int) const
{
	return(false);
}

// FUNCTION: 0x005F4390
int ObjectClass::Weapon_Range(int) const
{
	return(0);
}

// FUNCTION: 0x005F43A0
void ObjectClass::Scatter(Coord const &, bool, bool)
{
}

// FUNCTION: 0x005F43B0
bool ObjectClass::Catch_Fire(void)
{
	return(false);
}

// FUNCTION: 0x005F43C0
void ObjectClass::Fire_Out(void)
{
}

// FUNCTION: 0x005F43D0
int ObjectClass::Value(void) const
{
	return(0);
}

// FUNCTION: 0x005F43E0
MissionType ObjectClass::Get_Mission(void) const
{
	return(MISSION_NONE);
}

// FUNCTION: 0x005F43F0
void ObjectClass::Repair(int)
{
}

// FUNCTION: 0x005F4400
void ObjectClass::Sell_Back(int)
{
}

// FUNCTION: 0x005F4410
void ObjectClass::Move(FacingType facing)
{
	Mark(MARK_UP);
	Coord coord = Adjacent_Cell(PositionCoord, facing);
	if (Can_Enter_Cell(&Map[coord], facing) == MOVE_OK) {
		PositionCoord = coord;
	}
	Mark(MARK_DOWN);
}

// FUNCTION: 0x005F44A0
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

// FUNCTION: 0x005F61A0
VoxelDataStruct::~VoxelDataStruct(void)
{
	delete VoxLib;
	VoxLib = NULL;
	delete MotLib;
	MotLib = NULL;
}

// FUNCTION: 0x005F61F0
bool ObjectClass::operator < (ObjectClass const & object) const
{
	return(Sort_Y() < object.Sort_Y());
}

// FUNCTION: 0x005F6220
bool ObjectClass::operator > (ObjectClass const & object) const
{
	return((Sort_Y() > object.Sort_Y()) ? true : false);
}

// FUNCTION: 0x005F6C80
Coord ObjectClass::Docking_Coord(TechnoClass * ) const
{
	return(Center_Coord());
}
