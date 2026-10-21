// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "psystype.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "ptype.h"
#include "sun.h"
#include "tracker.h"
#include "warhead.h"

// FUNCTION: GAMEMD 0x006440A0
ParticleSystemTypeClass::ParticleSystemTypeClass(char const * ininame) :
	BASECLASS(ininame),
	HoldsWhat(PARTICLE_NONE),
	BehavesLike(PSYS_BEHAVIOR_NONE),
	Lifetime(-1),
	SpawnDirection(0, 0, 0),
	Spawns(false),
	SpawnFrames(1),
	Slowdown(0.0),
	ParticleCap(50),
	SpawnRadius(0),
	SpawnCutoff(0.0),
	SpawnTranslucencyCutoff(0.0),
	ParticlesPerCoord(.1),
	SpiralDeltaPerCoord(.025),
	SpiralRadius(25.0),
	PositionPerturbationCoefficient(0.0),
	MovementPerturbationCoefficient(0.0),
	VelocityPerturbationCoefficient(0.0),
	SpawnSparkPercentage(0.0),
	SparkSpawnFrames(0),
	LightSize(0),
	LaserColor(),
	IsLaser(false),
	OneFrameLight(false)
{
	Create_ID();
	ParticleSystemTypes.Add(this);
	IsSentient = true;
}

// FUNCTION: GAMEMD 0x00644220
ParticleSystemTypeClass::ParticleSystemTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	LaserColor(x)
{
}

// FUNCTION: GAMEMD 0x00644250
ParticleSystemTypeClass::~ParticleSystemTypeClass(void)
{
	Detach_This_From_All(this);
	ParticleSystemTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x006442D0
bool ParticleSystemTypeClass::Read_INI(CCINIClass & ini)
{
	char buffer[64];

	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {

		ini.Get_String(IniName, "HoldsWhat", "", buffer, sizeof(buffer));
		HoldsWhat = ParticleTypeClass::From_Name(buffer);

		Spawns = ini.Get_Bool(IniName, "Spawns", Spawns);
		SpawnFrames = ini.Get_Int(IniName, "SpawnFrames", SpawnFrames);
		ParticleCap = ini.Get_Int(IniName, "ParticleCap", ParticleCap);
		SpawnRadius = ini.Get_Int(IniName, "SpawnRadius", SpawnRadius);
		Slowdown = ini.Get_Float(IniName, "Slowdown", Slowdown);
		SpawnCutoff = ini.Get_Float(IniName, "SpawnCutoff", SpawnCutoff);
		SpawnTranslucencyCutoff = ini.Get_Float(IniName, "SpawnTranslucencyCutoff", SpawnTranslucencyCutoff);
		Lifetime = ini.Get_Int(IniName, "Lifetime", Lifetime);

		ini.Get_String(IniName, "BehavesLike", "", buffer, sizeof(buffer));
		BehavesLike = Particle_System_Behavior_From_Name(buffer);

		SpawnDirection = ini.Get_Vector(IniName, "SpawnDirection", SpawnDirection);
		ParticlesPerCoord = ini.Get_Float(IniName, "ParticlesPerCoord", ParticlesPerCoord);
		SpiralDeltaPerCoord = ini.Get_Float(IniName, "SpiralDeltaPerCoord", SpiralDeltaPerCoord);
		SpiralRadius = ini.Get_Float(IniName, "SpiralRadius", SpiralRadius);
		PositionPerturbationCoefficient = ini.Get_Float(IniName, "PositionPerturbationCoefficient", PositionPerturbationCoefficient);
		MovementPerturbationCoefficient = ini.Get_Float(IniName, "MovementPerturbationCoefficient", MovementPerturbationCoefficient);
		VelocityPerturbationCoefficient = ini.Get_Float(IniName, "VelocityPerturbationCoefficient", VelocityPerturbationCoefficient);
		IsLaser = ini.Get_Bool(IniName, "Laser", IsLaser);
		LaserColor = ini.Get_RGBClass(IniName, "LaserColor", LaserColor);
		SparkSpawnFrames = ini.Get_Int(IniName, "SparkSpawnFrames", SparkSpawnFrames);
		LightSize = ini.Get_Int(IniName, "LightSize", LightSize);
		OneFrameLight = ini.Get_Bool(IniName, "OneFrameLight", OneFrameLight);
		SpawnSparkPercentage = ini.Get_Float(IniName, "SpawnSparkPercentage", SpawnSparkPercentage);

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00644630
ParticleSystemType ParticleSystemTypeClass::From_Name(char const * name)
{
	if (stricmp(name, "<none>") && strlen(name)) {
		for (int index = PARTSYS_FIRST; index < ParticleSystemTypes.Count(); index++) {
			if (stricmp(name, ParticleSystemTypes[index]->Name()) == 0) {
				return(ParticleSystemType(index));
			}
		}
		ParticleSystemTypeClass *ptr = new ParticleSystemTypeClass(name);
		return(ParticleSystemType(ParticleSystemTypes.ID(ptr)));

	}
	return(PARTSYS_NONE);
}

// FUNCTION: GAMEMD 0x006446E0
const char * ParticleSystemTypeClass::Name_From(ParticleSystemType partsys)
{
	if ((unsigned)partsys < (unsigned)ParticleSystemTypes.Count()) {
		return(ParticleSystemTypes[partsys]->Name());
	}
	return("<none>");
}

// FUNCTION: GAMEMD 0x00644700
void ParticleSystemTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(HoldsWhat);
	crc(Spawns);
	crc(SpawnFrames);
	crc(Slowdown);
	crc(ParticleCap);
	crc(SpawnRadius);
	crc(SpawnCutoff);
	crc(SpawnTranslucencyCutoff);
	crc(BehavesLike);
	crc(Lifetime);
}

// FUNCTION: GAMEMD 0x006447A0
HRESULT STDMETHODCALLTYPE ParticleSystemTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleSystemTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006447E0
HRESULT STDMETHODCALLTYPE ParticleSystemTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) ParticleSystemTypeClass(NoInitClass());
	}
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00644830
HRESULT STDMETHODCALLTYPE ParticleSystemTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00644850
ParticleSystemBehaviorType Particle_System_Behavior_From_Name(char const * name)
{
	static char const * _BehaviorNames[PSYS_BEHAVIOR_COUNT] = {
		"Smoke",
		"Gas",
		"Fire",
		"Spark",
		"Railgun"
	};

	for (ParticleSystemBehaviorType behavior = PSYS_BEHAVIOR_FIRST; behavior < PSYS_BEHAVIOR_COUNT; behavior++) {
		if (!strcmpi(name, _BehaviorNames[behavior])) {
			return(behavior);
		}
	}
	return(PSYS_BEHAVIOR_NONE);
}

// FUNCTION: GAMEMD 0x00644890
ParticleSystemTypeClass * ParticleSystemTypeClass::FindOrAllocate(char const * name)
{
	if (stricmp("<none>", name) && stricmp("none", name)) {
		for (int index = 0; index < ParticleSystemTypes.Count(); index++) {
			if (stricmp(ParticleSystemTypes[index]->Name(), name) == 0) {
				return(ParticleSystemTypes[index]);
			}
		}
		return(new ParticleSystemTypeClass(name));
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x00644920
int ParticleSystemTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00644930
RTTIType ParticleSystemTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_PARTICLESYSTEMTYPE);
}

// FUNCTION: GAMEMD 0x00644940
bool ParticleSystemTypeClass::Create_And_Place(Cell const & , HouseClass * house) const
{
	return(false);
}
