// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "partsys.h"
#include "_map.h"
#include "_rtti.h"
#include "ccrand.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "goptions.h"
#include "inline.h"
#include "laser.h"
#include "matrix3d.h"
#include "ovrlight.h"
#include "particle.h"
#include "psystype.h"
#include "ptype.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "techno.h"
#include "tracker.h"
#include "vector.h"
#include "wave.h"

static inline void _Add_System(ParticleSystemClass * system)
{
	ParticleSystems.Add(system);
}

// FUNCTION: 0x0062DC50
ParticleSystemClass::ParticleSystemClass(ParticleSystemTypeClass const * type, Coord const & coord, AbstractClass * target, AbstractClass * source, Coord const & spawn_coord, HouseClass * house) :
	BASECLASS(),
	Class((ParticleSystemTypeClass *)type),
	CoordOffset(0, 0, 0),
	SystemParticles(),
	SpawnCoord(COORD_NONE),
	Source(NULL),
	Target(NULL),
	SpawnFrames(Class->SpawnFrames),
	Lifetime(Class->Lifetime),
	SparkSpawnFrames(Class->SparkSpawnFrames),
	SparkRadius(29),
	IsMarkedForDeletion(false),
	IsRandomSparkDirection(false),
	OwnerHouse(NULL)
{
	Create_ID();
	_Add_System(this);

	SystemParticles.Clear();
	OwnerHouse = house;

	if (target != NULL) {
		SpawnCoord = target->Center_Coord();
		if (Map[target->Center_Coord()].IsUnderBridge && target->RTTI == RTTI_CELL) {
			SpawnCoord.Z += BRIDGE_LEPTON_HEIGHT;
		}
	} else {
		SpawnCoord = spawn_coord;
	}

	Coord spawn;
	spawn.X = coord.X;
	spawn.Y = coord.Y;
	spawn.Z = coord.Z;

	if (source != NULL) {
		Source = source;
		TechnoClass * techno = Dynamic_Cast<TechnoClass *>(source);
		if (techno != NULL) {
			Target = techno->TarCom;
		}
	} else {
		Source = NULL;
		Target = target;
	}

	Unlimbo(spawn);

	if (Source != NULL) {
		CoordOffset = Coord(Position) - Source->Center_Coord();
	}

	ObjectPtrTracker.Add(this);
}

// FUNCTION: 0x0062DF20
ParticleSystemClass::ParticleSystemClass(void) :
	BASECLASS(),
	Class(NULL),
	SystemParticles(),
	SpawnCoord(COORD_NONE),
	Source(NULL),
	Target(NULL),
	SpawnFrames(0),
	Lifetime(0),
	SparkSpawnFrames(0),
	SparkRadius(0),
	IsMarkedForDeletion(false),
	IsRandomSparkDirection(false)
{
	SystemParticles.Clear();
	ParticleSystems.Add(this);
	ObjectPtrTracker.Add(this);
}

// FUNCTION: 0x0062E070
ParticleSystemClass::~ParticleSystemClass(void)
{
	Detach_This_From_All(this);
	Limbo();

	while (SystemParticles.Count()) {
		delete SystemParticles[0];
		ObjectsToDelete.Delete(SystemParticles[0]);
		SystemParticles.Delete(SystemParticles[0]);
	}

	Class = NULL;

	ParticleSystems.Delete(this);
	AbstractTypePtrTracker.Delete(this);
	ObjectPtrTracker.Delete(this);
}

// FUNCTION: 0x0062E280
void ParticleSystemClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	if (Class->OneFrameLight) {
		if (Class->LightSize > 0 && SystemParticles.Count() > 0) {
			float cap = (float)SystemParticles.Count() / (float)Class->ParticleCap;
			float fullness = MAX(0.4, MIN(1, cap));
			int spotsize = (int)(fullness * Class->LightSize);
			SpotLightClass * spotlight = new SpotLightClass(PositionCoord, spotsize);
			spotlight->Set_Radius(SparkRadius);
			spotlight->Draw_It();
			delete spotlight;
		}
	}
}

// FUNCTION: 0x0062E380
ParticleClass * ParticleSystemClass::Spawn_Held_Particle(Coord const & coord1, Coord const & coord2)
{
	if (Class->HoldsWhat == PARTICLE_NONE) {
		return(NULL);
	}

	ParticleClass * particle = new ParticleClass(ParticleTypes[Class->HoldsWhat], coord1, coord2, this);
	if (particle != NULL) {
		SystemParticles.Add(particle);
		return(particle);
	}
	return(NULL);
}

// FUNCTION: 0x0062E430
ParticleClass * ParticleSystemClass::Spawn_Particle(ParticleTypeClass * type, Coord const & coord)
{
	ParticleClass * particle = new ParticleClass(type, coord, COORD_NONE, this);
	if (particle != NULL) {
		SystemParticles.Add(particle);
		return(particle);
	}
	return(NULL);
}

// FUNCTION: 0x0062E4C0
ParticleClass * ParticleSystemClass::Spawn_Held_Particle_Random(Coord const & coord1, Coord const & coord2, int v)
{
	if (Class->HoldsWhat == PARTICLE_NONE) {
		return(NULL);
	}

	ParticleClass * particle = new ParticleClass(ParticleTypes[Class->HoldsWhat], coord1, coord2, this);
	if (particle == NULL || v <= 0) {
		return(NULL);
	}

	SystemParticles.Add(particle);

	int active_count = SystemParticles.Count();
	ParticleClass **vector = &SystemParticles[0];

	int random_range = v;
	if (active_count < random_range) {
		random_range = active_count;
	}
	int random_offset = abs(Scen->RandomNumber()) % random_range;

	int source_index = active_count - 2;
	int stop_index = active_count - random_offset - 2;
	int insert_position = active_count - random_offset;

	if (source_index > stop_index) {
		ParticleClass **source = &vector[source_index];
		int move_count = source_index - stop_index;
		do {
			source[1] = *source;
			source--;
		} while (--move_count != 0);
	}
	vector[insert_position - 1] = particle;
	return(particle);
}

// FUNCTION: 0x0062E5D0
bool ParticleSystemClass::Delete_Particle(int index)
{
	if (index < SystemParticles.Count()) {
		SystemParticles[index]->Delete_Me();
		SystemParticles.Delete(SystemParticles[index]);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0062E650
void ParticleSystemClass::Clear_System(void)
{
	for (int i = SystemParticles.Count() - 1; i >= 0; i--) {
		Delete_Particle(i);
	}
}

// FUNCTION: 0x0062E6D0
void ParticleSystemClass::Gas_AI(void)
{
	int i;

	for (i = SystemParticles.Count()-1; i >= 0; i--) {
		SystemParticles[i]->Behavior_AI();
	}

	for (i = SystemParticles.Count()-1; i >= 0; i--) {
		ParticleClass *spart = SystemParticles[i];
		if (spart->IsToDie) {
			ParticleType nt = spart->Class->NextParticle;
			if (nt != PARTICLE_NONE) {
				Coord const & coord = spart->PositionCoord;
				ParticleTypeClass *next = ParticleTypes[nt];
				Coord ncoord = coord + spart->Class->NextParticleOffset;
				ParticleClass *npart = new ParticleClass(next, ncoord, COORD_NONE, this);
				if (npart != NULL) {
					SystemParticles.Add(npart);
					npart->Speed = spart->Speed;
					npart->GasDrift = spart->GasDrift;
				}
			}
			spart->Delete_Me();
		} else {
			spart->Motion_AI();
		}
	}
}

namespace Unsorted { extern int CurrentFrame; }
extern short SineTable[];

static int _FireStepX[FACING_COUNT] = { 1,  1,  0, -1, -1, -1,  0,  1 };
static int _FireStepY[FACING_COUNT] = { 0,  1,  1,  1,  0, -1, -1, -1 };

// FUNCTION: 0x0062F9A0
void ParticleSystemClass::Fire_AI(void)
{
	int i;

	for (i = SystemParticles.Count()-1; i >= 0; i--) {
		ParticleClass *spart = SystemParticles[i];
		spart->Behavior_AI();
		spart->Motion_AI();
	}

	for (i = SystemParticles.Count()-1; i >= 0; i--) {
		ParticleClass *spart = SystemParticles[i];
		if (spart->IsToDie) {
			spart->Delete_Me();
		}
	}

	bool reaimed = false;
	TechnoClass *source = Dynamic_Cast<TechnoClass *>(Source);
	if (source != NULL) {
		if (source->IsActive) {
			if (source->TarCom != NULL && source->PrimaryFacing.Is_Rotating()) {
				Coord cc = source->TarCom->Center_Coord();
				Coord delta = cc - source->Get_Coord();
				int dist = (int)fastmath::sqrt((double)delta.X * (double)delta.X + (double)delta.Y * (double)delta.Y + (double)delta.Z * (double)delta.Z);
				Coord aim = Move_Coord(source->Get_Coord(), source->PrimaryFacing.Current(), dist);
				SpawnCoord = aim;
				Set_Coord(source->Fire_Coord(0, Coord(0, 0, 0)));
				reaimed = true;
			}
		}
		if (!source->IsActive) {
			Delete_Me();
		}
	} else {
		Delete_Me();
	}

	if (!IsMarkedForDeletion && (!(Unsorted::CurrentFrame % Class->SpawnFrames) || (!(Unsorted::CurrentFrame % 3) && reaimed))) {
		int index = Unsorted::CurrentFrame % 500;

		Coord delta = Get_Coord() - SpawnCoord;
		int distsq = delta.Dot_Product(delta);

		int divisor;
		if ((float)fastmath::sqrt((double)distsq) < 200.0f) {
			divisor = 3;
		} else {
			divisor = 1;
		}

		int offset = (int)((double)(12 * SineTable[index]) / ((float)divisor * 3.0f));

		FacingType face = Facing_Between_Points(Get_Coord(), SpawnCoord);

		Spawn_Held_Particle_Random(Get_Coord() + Coord(0, 0, 1), SpawnCoord + Coord(offset * _FireStepX[face], offset * _FireStepY[face], 0), 4);
	}
}

// FUNCTION: 0x0062FD60
void ParticleSystemClass::AI(void)
{
	switch (Class->BehavesLike) {
		case PSYS_BEHAVIOR_SMOKE:
			Smoke_AI();
			break;
		case PSYS_BEHAVIOR_GAS:
			Gas_AI();
			break;
		case PSYS_BEHAVIOR_FIRE:
			Fire_AI();
			break;
		case PSYS_BEHAVIOR_SPARK:
			Spark_AI();
			break;
		case PSYS_BEHAVIOR_RAILGUN:
			Railgun_AI();
			break;
	}

	Lifetime--;
	if (Lifetime == 0) {
		Delete_Me();
	}
	if (IsActive && (IsMarkedForDeletion && SystemParticles.Count() == 0)) {
		Limbo();
		IsActive = false;
		ObjectsToDelete.Add(this);
	}
}

// FUNCTION: 0x0062FE60
bool ParticleSystemClass::Is_Inactive(void) const
{
	return(IsMarkedForDeletion && SystemParticles.Count() == 0);
}

// FUNCTION: 0x0062FE80
LayerType ParticleSystemClass::In_Which_Layer(void) const
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x0062FE90
void ParticleSystemClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	SystemParticles.Delete((ParticleClass *)target);
	if (target == Class) {
		Class = NULL;
	}
	if (target == Target) {
		Target = NULL;
	}
	if (target == Source) {
		IsMarkedForDeletion = true;
		Source = NULL;
	}
}

// FUNCTION: 0x0062FF20
HRESULT STDMETHODCALLTYPE ParticleSystemClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) ParticleSystemClass(NoInitClass());

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&Target);
		Swizzle_Pointer(&Source);

		int count = 0;
		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		while (count--) {
			ULONG pcbRead;
			ParticleClass *pptr = NULL;
			result = stream->Read(&pptr, sizeof(pptr), &pcbRead);
			if (FAILED(result)) {
				return(result);
			}
			SystemParticles.Add(pptr);
		}

		for (int index = 0; index < SystemParticles.Count(); index++) {
			Swizzle_Pointer(&SystemParticles[index]);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00630090
HRESULT STDMETHODCALLTYPE ParticleSystemClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int count = SystemParticles.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (int index = 0; index < count; index++) {
			result = stream->Write(&SystemParticles[index], sizeof(SystemParticles[index]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}
	}
	return(result);
}

// FUNCTION: 0x00630100
void ParticleSystemClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	for (int i = 0; i < SystemParticles.Count(); i++) {
		crc(SystemParticles[i]->Fetch_ID());
	}
	if (Source != NULL) {
		crc(Source->Fetch_ID());
	}
	if (Target != NULL) {
		crc(Target->Fetch_ID());
	}
	crc(SpawnFrames);
	crc(Lifetime);
}

// FUNCTION: 0x006301A0
HRESULT STDMETHODCALLTYPE ParticleSystemClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleSystemClass;
	return(S_OK);
}

// FUNCTION: 0x006301E0 ParticleSystemClass::Delete_Me

// FUNCTION: 0x006301F0
void ParticleSystemClass::FUN_006301F0(int lifetime)
{
	Lifetime = lifetime;
}

// FUNCTION: 0x00630200
int ParticleSystemClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00630210
RTTIType ParticleSystemClass::Fetch_RTTI(void) const
{
	return(RTTI_PARTICLESYSTEM);
}

// FUNCTION: 0x00630220
ObjectTypeClass const * ParticleSystemClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: 0x00630230 ParticleSystemClass::~ParticleSystemClass

// FUNCTION: 0x00630250 ?$DynamicVectorClass@PAVParticleClass@@::Add

// FUNCTION: 0x006302A0 ?$VectorClass@PAVParticleClass@@::?$VectorClass@PAVParticleClass@@
template class DynamicVectorClass<ParticleClass *>;

// FUNCTION: 0x006302F0 $E2
