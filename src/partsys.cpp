// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x0062DA40 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0062DA70 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0062DA90 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0062DAB0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0062DAD0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0062DAF0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0062DB10 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0062DB50 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0062DB80 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0062DBB0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0062DBE0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0062DBF0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0062DC10 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0062DC30 SYMBOL
// _$E47

static inline void _Add_System(ParticleSystemClass * system)
{
	ParticleSystems.Add(system);
}

// FUNCTION: GAMEMD 0x0062DC50
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

// FUNCTION: GAMEMD 0x0062DF20
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

// FUNCTION: GAMEMD 0x0062E070
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

// FUNCTION: GAMEMD 0x0062E280
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

// FUNCTION: GAMEMD 0x0062E380
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

// FUNCTION: GAMEMD 0x0062E430
ParticleClass * ParticleSystemClass::Spawn_Particle(ParticleTypeClass * type, Coord const & coord)
{
	ParticleClass * particle = new ParticleClass(type, coord, COORD_NONE, this);
	if (particle != NULL) {
		SystemParticles.Add(particle);
		return(particle);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0062E4C0
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

// FUNCTION: GAMEMD 0x0062E5D0
bool ParticleSystemClass::Delete_Particle(int index)
{
	if (index < SystemParticles.Count()) {
		SystemParticles[index]->Delete_Me();
		SystemParticles.Delete(SystemParticles[index]);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0062E650
void ParticleSystemClass::Clear_System(void)
{
	for (int i = SystemParticles.Count() - 1; i >= 0; i--) {
		Delete_Particle(i);
	}
}

// FUNCTION: GAMEMD 0x0062E6D0
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

// FUNCTION: GAMEMD 0x0062E840
void ParticleSystemClass::Spark_AI(void)
{
	if (SparkSpawnFrames > 0) {

		if (SparkSpawnFrames == 1 || Random_Double(0.0, 1.0) <= Class->SpawnSparkPercentage) {

			int random = Scen->RandomNumber();
			int count = Class->ParticleCap / 2 + abs(random) % (Class->ParticleCap / 2);

			ParticleTypeClass * held = ParticleTypes[Class->HoldsWhat];

			Random2Class & rng = Scen->RandomNumber;
			int rand_a = rng();
			int rand_b = Scen->RandomNumber();

			Vector3 spark;
			float zvel = (float)(rng() % held->ZVelocityRange);
			spark.X = (float)(rand_b % held->XVelocity);
			spark.Y = (float)(rand_a % held->YVelocity);
			spark.Z = zvel;

			while (count > 0) {
				ParticleClass * particle = Spawn_Held_Particle(PositionCoord, PositionCoord);

				ParticleTypeClass const * ptype = particle->Class;
				Vector3 & dir = particle->MovementDirection;

				particle->MovementDirection.X = (float)(Scen->RandomNumber % ptype->XVelocity);
				particle->MovementDirection.Y = (float)(Scen->RandomNumber % ptype->YVelocity);
				particle->MovementDirection.Z = (float)(ptype->MinZVelocity + abs(Scen->RandomNumber) % ptype->ZVelocityRange);

				float speed = dir.Length();
				if (IsRandomSparkDirection) {
					dir = Vector3(spark.X + dir.X, spark.Y + dir.Y, spark.Z + dir.Z);
				} else {
					TPoint3D<float> const & spawndir = Class->SpawnDirection;
					dir = Vector3(spawndir.X + dir.X, spawndir.Y + dir.Y, spawndir.Z + dir.Z);
				}
				dir = Normalize(dir);
				dir = speed * dir;

				count--;
			}

			if (Options.DetailLevel == 2) {
				if (SparkSpawnFrames == Class->SparkSpawnFrames && Class->LightSize > 0 && !Class->OneFrameLight) {
					new SpotLightClass(PositionCoord, Class->LightSize);
				}
			}
		}

		SparkSpawnFrames--;
		if (SparkSpawnFrames <= 0) {
			IsMarkedForDeletion = true;
		}

		double chance = Random_Double(0.0, 1.0);
		if (chance < 0.3) {
			int radius;
			if (SparkRadius - 3 <= 17) {
				radius = 17;
			} else {
				radius = SparkRadius - 3;
			}
			SparkRadius = radius;
		} else if (chance < 0.6) {
			int radius = SparkRadius + 3;
			if (radius >= 41) {
				radius = 41;
			}
			SparkRadius = radius;
		}
	}

	for (int i = 0; i < SystemParticles.Count(); i++) {
		SystemParticles[i]->Behavior_AI();
	}

	for (int j = SystemParticles.Count() - 1; j >= 0; j--) {
		ParticleClass * particle = SystemParticles[j];
		if (particle->IsToDie) {
			particle->Delete_Me();
		}
	}
}

#include "cast.h"

static inline bool Is_Object(AbstractClass const * a)
{
	if (a == NULL) return(false);
	unsigned char flag = (a->AbstractFlags >> 1) & 1;
	return(flag != 0);
}

// FUNCTION: GAMEMD 0x0062ED40
void ParticleSystemClass::Smoke_AI(void)
{
	int i;

	if (Is_Object(Source) && Source->What_Am_I() != RTTI_BUILDING) {
		Set_Coord(Source->Center_Coord() + CoordOffset);
	}

	for (i = 0; i < SystemParticles.Count(); i++) {
		SystemParticles[i]->Behavior_AI();
	}

	for (i = SystemParticles.Count() - 1; i >= 0; i--) {
		ParticleClass *spart = SystemParticles[i];
		if (spart->IsToDie) {
			if (spart->Class->NextParticle != PARTICLE_NONE) {
				Coord coord = spart->Get_Coord();
				int xrand = Scen->RandomNumber % (spart->Class->Radius >> 3);
				if (xrand <= 0) {
					xrand = xrand - (spart->Class->Radius >> 3);
				} else {
					xrand = (spart->Class->Radius >> 3) + xrand;
				}
				int yrand = Scen->RandomNumber % (spart->Class->Radius >> 3);
				if (yrand <= 0) {
					yrand = yrand - (spart->Class->Radius >> 3);
				} else {
					yrand = (spart->Class->Radius >> 3) + yrand;
				}
				ParticleTypeClass *next = ParticleTypes[spart->Class->NextParticle];
				Point2D offset(xrand, yrand);
				ParticleClass *npart = Spawn_Particle(next, coord + offset);
				if (npart != NULL) {
					npart->Speed = spart->Speed;
					npart->Translucency = spart->Translucency + (Scen->RandomNumber % 6 != 0 ? 25 : 0);
				}
				ParticleClass *ppart = Spawn_Particle(next, coord - offset);
				if (ppart != NULL) {
					ppart->Speed = spart->Speed;
					ppart->Translucency = spart->Translucency + (Scen->RandomNumber % 6 != 0 ? 25 : 0);
				}
			}
			spart->Delete_Me();
		} else {
			spart->Motion_AI();
		}
	}

	if (!IsMarkedForDeletion && IsActive && (Frame % (int)SpawnFrames) == 0) {
		FootClass *foot = As_Foot(Source);
		if (foot == NULL || foot->CurrentTube < 0) {
			Coord scoord = Get_Coord() + Coord(Scen->RandomNumber % (Class->SpawnRadius + 1), Scen->RandomNumber % (Class->SpawnRadius + 1), 10);
			if (Class->HoldsWhat != PARTICLE_NONE) {
				ParticleClass *npart = new ParticleClass(ParticleTypes[Class->HoldsWhat], scoord, SpawnCoord, this);
				if (npart != NULL) {
					SystemParticles.Add(npart);
					if (SpawnFrames > Class->SpawnTranslucencyCutoff) {
						npart->Translucency += 25;
					}
					npart->Speed = npart->Speed - (SpawnFrames - Class->SpawnFrames) * 0.35f;
					if (npart->Speed < 2.0) {
						npart->Speed = 2.0f;
					}
				}
			}
		}
	}

	SpawnFrames = Class->Slowdown + SpawnFrames;
	if (SpawnFrames > Class->SpawnCutoff) {
		IsMarkedForDeletion = true;
	}
}

namespace Unsorted { extern int CurrentFrame; }
extern short SineTable[];

static int _FireStepX[FACING_COUNT] = { 1,  1,  0, -1, -1, -1,  0,  1 };
static int _FireStepY[FACING_COUNT] = { 0,  1,  1,  1,  0, -1, -1, -1 };

// FUNCTION: GAMEMD 0x0062F230
void ParticleSystemClass::Railgun_AI(void)
{
	int i;
	if (!IsMarkedForDeletion && SystemParticles.Count() == 0) {
		Point3D delta = SpawnCoord - Get_Coord();
		int dx = delta.X;
		int dy = delta.Y;
		int dz = delta.Z;
		int total_dist = (int)fastmath::sqrt(
			(double)dx * (double)dx +
			(double)dy * (double)dy +
			(double)dz * (double)dz);

		double dy_double = dy;
		double dx_double = dx;
		double horiz = dy_double * dy_double;
		int horiz_dist = (int)fastmath::sqrt(horiz + dx_double * dx_double);

		int clamped_z = dz;
		int clamped_x = dx;
		if (clamped_z >= total_dist) clamped_z = total_dist;
		if (clamped_z <= -total_dist) clamped_z = -total_dist;
		if (clamped_x >= horiz_dist) clamped_x = horiz_dist;
		if (clamped_x <= -horiz_dist) clamped_x = -horiz_dist;

		int num_particles = (int)((double)total_dist * Class->ParticlesPerCoord);
		float dist_f = (float)total_dist;
		float elevation = fastmath::asin((double)clamped_z / dist_f);
		float azimuth = fastmath::acos((double)clamped_x / horiz_dist);
		if (dy < 0) azimuth = -azimuth;

		Matrix3D mat;
		mat.Make_Identity();
		mat.Rotate_Z(azimuth);
		mat.Rotate_X(elevation);

		float velocity_accum = 0.0f;
		i = 0;
		if (num_particles > 0) {
			float num_particles_f = (float)num_particles;
			do {
				float frac = (float)i / num_particles_f;
				double angle = dist_f;
				angle *= frac;
				angle *= Class->SpiralDeltaPerCoord;
				double sine = fastmath::sin(angle);
				double cosine = fastmath::cos(angle);

				Vector3 spiral(0.0f, cosine, sine);
				spiral = mat * spiral;
				Vector3 scaled = (float)Class->SpiralRadius * spiral;

				Vector3 tmp(
					((Random_Pick(0, 0x7ffffffe) * 4.656612877414201e-10) - 0.5)
						* Class->PositionPerturbationCoefficient,
					((Random_Pick(0, 0x7ffffffe) * 4.656612877414201e-10) - 0.5)
						* Class->PositionPerturbationCoefficient,
					((Random_Pick(0, 0x7ffffffe) * 4.656612877414201e-10) - 0.5)
						* Class->PositionPerturbationCoefficient);
				scaled += tmp;

				Coord particle_pos;
				particle_pos = Point3D((int)scaled.X, (int)scaled.Y, (int)scaled.Z)
					+ Lerp(Get_Coord(), SpawnCoord, frac);
				ParticleClass *particle =
					Spawn_Held_Particle(particle_pos, particle_pos);

				particle->MovementDirection = spiral;
				Vector3 move(
					((Random_Pick(0, 0x7ffffffe) * 4.656612877414201e-10) - 0.5)
						* Class->MovementPerturbationCoefficient,
					((Random_Pick(0, 0x7ffffffe) * 4.656612877414201e-10) - 0.5)
						* Class->MovementPerturbationCoefficient,
					((Random_Pick(0, 0x7ffffffe) * 4.656612877414201e-10) - 0.5)
						* Class->MovementPerturbationCoefficient);
				particle->MovementDirection += move;
				particle->MovementDirection =
					Normalize(particle->MovementDirection);

				float vp =
					((Random_Pick(0, 0x7ffffffe) * 4.656612877414201e-10)
						+ velocity_accum - 0.5)
					* (Class->VelocityPerturbationCoefficient * 0.5);

				velocity_accum = (float)MAX(MIN(vp, Class->VelocityPerturbationCoefficient),
					-Class->MovementPerturbationCoefficient);

				particle->Speed = velocity_accum + particle->Class->Velocity;
				i++;
			} while (i < num_particles);
		}

		if (Class->IsLaser) {
			new LaserDrawClass(
				Get_Coord(), SpawnCoord, 0, true, Class->LaserColor,
				RGBClass(0, 0, 0), RGBClass(0, 0, 0),
				10, false, true, 0.5f, 0.0f);
		}
		IsMarkedForDeletion = true;
	}

	for (i = 0; i < SystemParticles.Count(); i++) {
		SystemParticles[i]->Behavior_AI();
	}
	for (int j = SystemParticles.Count() - 1; j >= 0; j--) {
		ParticleClass *particle = SystemParticles[j];
		if (particle->IsToDie) particle->Delete_Me();
	}
}

// FUNCTION: GAMEMD 0x0062F9A0
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

// FUNCTION: GAMEMD 0x0062FD60
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

// FUNCTION: GAMEMD 0x0062FE60
bool ParticleSystemClass::Is_Inactive(void) const
{
	return(IsMarkedForDeletion && SystemParticles.Count() == 0);
}

// FUNCTION: GAMEMD 0x0062FE80
LayerType ParticleSystemClass::In_Which_Layer(void) const
{
	return(LAYER_GROUND);
}

// FUNCTION: GAMEMD 0x0062FE90
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

// FUNCTION: GAMEMD 0x0062FF20
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

// FUNCTION: GAMEMD 0x00630090
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

// FUNCTION: GAMEMD 0x00630100
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

// FUNCTION: GAMEMD 0x006301A0
HRESULT STDMETHODCALLTYPE ParticleSystemClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleSystemClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006301E0
// ParticleSystemClass::Delete_Me

// FUNCTION: GAMEMD 0x006301F0
void ParticleSystemClass::FUN_006301F0(int lifetime)
{
	Lifetime = lifetime;
}

// FUNCTION: GAMEMD 0x00630200
int ParticleSystemClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00630210
RTTIType ParticleSystemClass::Fetch_RTTI(void) const
{
	return(RTTI_PARTICLESYSTEM);
}

// FUNCTION: GAMEMD 0x00630220
ObjectTypeClass const * ParticleSystemClass::Class_Of(void) const
{
	return(Class);
}

// SYNTHETIC: GAMEMD 0x00630230
// ??_GParticleSystemClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00630250
// ?Add@?$DynamicVectorClass@PAVParticleClass@@@@QAE_NABQAVParticleClass@@@Z

// TEMPLATE: GAMEMD 0x006302A0
// ??0?$VectorClass@PAVParticleClass@@@@QAE@HPBQAVParticleClass@@@Z
template class DynamicVectorClass<ParticleClass *>;
