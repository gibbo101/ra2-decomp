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

// FUNCTION: 0x006301A0
HRESULT STDMETHODCALLTYPE ParticleSystemClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleSystemClass;
	return(S_OK);
}

// FUNCTION: 0x006301F0
void ParticleSystemClass::FUN_006301F0(int lifetime)
{
	Lifetime = lifetime;
}
