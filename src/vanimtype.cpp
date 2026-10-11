// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "vanimtype.h"
#include "animtype.h"
#include "ccfile.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "psystype.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "techtype.h"
#include "tracker.h"
#include "vector.h"
#include "warhead.h"

// FUNCTION: 0x0074AD80
VoxelAnimTypeClass::VoxelAnimTypeClass(char const * ininame) :
	BASECLASS(ininame),
	IsNormalized(false),
	IsTranslucent(false),
	IsSharesSourceData(false),
	VoxelIndex(0),
	Duration(2 * TICKS_PER_SECOND),
	Elasticity(0.8f),
	MinAngularVelocity(0),
	MaxAngularVelocity(10.0 * (3.1415 / 180.0)),
	MinZVel(3.5),
	MaxZVel(5),
	MaxXYVel(15),
	IsMeteor(false),
	Spawns(NULL),
	SpawnCount(0),
	StartSound(VOC_NONE),
	StopSound(VOC_NONE),
	BounceAnim(NULL),
	ExpireAnim(NULL),
	TrailerAnim(NULL),
	Damage(0),
	DamageRadius(0),
	Warhead(NULL),
	AttachedSystem(NULL),
	IsTiberium(false)
{
	Create_ID();
	IsLegalTarget = false;
	IsSentient = true;
	IsStealthy = true;
	IsSelectable = true;
	IsInsignificant = true;
	IsImmune = true;
	IsFootprint = false;
	AbstractTypePtrTracker.Add(this);
	VoxelAnimTypes.Add(this);
}

// FUNCTION: 0x0074AF70
// FUNCTION: 0x0074BA30
VoxelAnimTypeClass::~VoxelAnimTypeClass(void)
{
	Detach_This_From_All(this);
	AbstractTypePtrTracker.Delete(this);
	VoxelAnimTypes.Delete(this);
	if (IsSharesSourceData) {
		Voxel.VoxLib = NULL;
		Voxel.MotLib = NULL;
	}
}

// FUNCTION: 0x0074B620
char const * VoxelAnimTypeClass::Name_From(VoxelAnimType anim)
{
	if ((unsigned)anim > (unsigned)VoxelAnimTypes.Count()) return("");
	return(VoxelAnimTypes[anim]->IniName);
}

// FUNCTION: 0x0074B640
VoxelAnimType VoxelAnimTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = VANIM_FIRST; index < VoxelAnimTypes.Count(); index++) {
			if (stricmp(VoxelAnimTypes[index]->IniName, name) == 0) {
				return(VoxelAnimType(index));
			}
		}
	}
	return(VANIM_NONE);
}

// FUNCTION: 0x0074B690
void VoxelAnimTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(IsNormalized);
	crc(IsTranslucent);
	crc(IsSharesSourceData);
	crc(VoxelIndex);
	crc(Duration);
	crc(Elasticity);
	crc(MinAngularVelocity);
	crc(MaxAngularVelocity);
	crc(MinZVel);
	crc(MaxZVel);
	crc(MaxXYVel);
	crc(IsMeteor);
	crc(SpawnCount);
	crc(StartSound);
	crc(StopSound);
	crc(Damage);
	crc(DamageRadius);
	crc(IsTiberium);
}

// FUNCTION: 0x0074B7D0
HRESULT STDMETHODCALLTYPE VoxelAnimTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VoxelAnimTypeClass;
	return(S_OK);
}

// FUNCTION: 0x0074B8D0
HRESULT STDMETHODCALLTYPE VoxelAnimTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0074B8F0
void VoxelAnimTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (Warhead == target) {
		Warhead = NULL;
	}
	if (BounceAnim == target) {
		BounceAnim = NULL;
	}
	if (ExpireAnim == target) {
		ExpireAnim = NULL;
	}
	if (TrailerAnim == target) {
		TrailerAnim = NULL;
	}
	if (Spawns == target) {
		Spawns = NULL;
	}
	if (AttachedSystem == target) {
		AttachedSystem = NULL;
	}
}

// FUNCTION: 0x0074B960
VoxelAnimTypeClass * VoxelAnimTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<VoxelAnimTypeClass>(name, VoxelAnimTypes));
}
