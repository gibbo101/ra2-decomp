// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "ptype.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "warhead.h"

extern char DAT_00889F64[];

// FUNCTION: 0x00644BE0
ParticleTypeClass::ParticleTypeClass(char const * ininame) :
	BASECLASS(ininame),
	NextParticleOffset(0, 0, 0),
	ColorSpeed(0),
	XVelocity(1),
	YVelocity(1),
	MinZVelocity(0),
	ZVelocityRange(1),
	ColorList(),
	StartColor1(),
	StartColor2(),
	Translucent25State(-1),
	Translucent50State(-1),
	MaxDC(0),
	MaxEC(1),
	Warhead(NULL),
	Damage(0),
	StartFrame(0),
	NumLoopFrames(1),
	Translucency(0),
	WindEffect(0),
	Velocity(0),
	Deacc(0),
	Radius(0),
	DeleteOnStateLimit(false),
	EndStateAI(0),
	StartStateAI(0),
	StateAIAdvance(4),
	FinalDamageState(0),
	IsNormalized(false),
	NextParticle(PARTICLE_NONE),
	BehavesLike(BEHAVIOR_NONE)
{
	ParticleTypes.Add(this);
	AbstractTypePtrTracker.Add(this);
	IsSentient = false;
}

// FUNCTION: 0x00644DD0
ParticleTypeClass::ParticleTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	StartColor1(x),
	StartColor2(x)
{

}

// FUNCTION: 0x00644E40
ParticleTypeClass::~ParticleTypeClass(void)
{
	Detach_This_From_All(this);
	AbstractTypePtrTracker.Delete(this);
	ParticleTypes.Delete(this);
}

// FUNCTION: 0x00644F50
bool ParticleTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		ColorList = ini.Get_RGBClass_List(IniName, "ColorList", TypeList<RGBClass>());

		MaxDC = ini.Get_Int(IniName, "MaxDC", MaxDC);
		MaxEC = ini.Get_Int(IniName, "MaxEC", MaxEC);

		Damage = ini.Get_Int(IniName, "Damage", Damage);
		WarheadTypeClass * oldwarhead = Warhead;
		char whname[128];
		Warhead = ini.Get_String(IniName, "Warhead", DAT_00889F64, whname, sizeof(whname)) ? WarheadTypeClass::Find_Or_Make(whname) : oldwarhead;

		StartFrame = ini.Get_Int(IniName, "StartFrame", StartFrame);
		NumLoopFrames = ini.Get_Int(IniName, "NumLoopFrames", NumLoopFrames);

		Translucency = ini.Get_Int(IniName, "Translucency", Translucency);
		WindEffect = ini.Get_Int(IniName, "WindEffect", WindEffect);

		Velocity = ini.Get_Float(IniName, "Velocity", Velocity);
		Deacc = ini.Get_Float(IniName, "Deacc", Deacc);

		Radius = ini.Get_Int(IniName, "Radius", Radius);
		DeleteOnStateLimit = ini.Get_Bool(IniName, "DeleteOnStateLimit", DeleteOnStateLimit);

		EndStateAI = ini.Get_Int(IniName, "EndStateAI", EndStateAI);
		StartStateAI = ini.Get_Int(IniName, "StartStateAI", StartStateAI);
		StateAIAdvance = ini.Get_Int(IniName, "StateAIAdvance", StateAIAdvance);

		Translucent50State = ini.Get_Int(IniName, "Translucent50State", Translucent50State);
		Translucent25State = ini.Get_Int(IniName, "Translucent25State", Translucent25State);

		IsNormalized = ini.Get_Bool(IniName, "Normalized", IsNormalized);
		ColorSpeed = ini.Get_Float(IniName, "ColorSpeed", ColorSpeed);

		XVelocity = ini.Get_Int(IniName, "XVelocity", XVelocity);
		YVelocity = ini.Get_Int(IniName, "YVelocity", YVelocity);
		MinZVelocity = ini.Get_Int(IniName, "MinZVelocity", MinZVelocity);
		ZVelocityRange = ini.Get_Int(IniName, "ZVelocityRange", ZVelocityRange);

		NextParticleOffset = ini.Get_Offset(IniName, "NextParticleOffset", NextParticleOffset);

		StartColor1 = ini.Get_RGBClass(IniName, "StartColor1", StartColor1);
		StartColor2 = ini.Get_RGBClass(IniName, "StartColor2", StartColor2);

		FinalDamageState = ini.Get_Int(IniName, "FinalDamageState", EndStateAI);

		char buffer[32];
		if (ini.Get_String(IniName, "NextParticle", NULL, buffer, sizeof(buffer))) {
			NextParticle = ParticleTypeClass::From_Name(buffer);
		}

		buffer[0] = '\0';
		ini.Get_String(IniName, "BehavesLike", NULL, buffer, sizeof(buffer));
		BehavesLike = Particle_Behavior_From_Name(buffer);

		int unused = ParticleTypes.Count();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00645430
ParticleType ParticleTypeClass::From_Name(char const * name)
{
	if (stricmp(name, "<none>")) {
		for (int index = PARTICLE_FIRST; index < ParticleTypes.Count(); index++) {
			if (stricmp(name, ParticleTypes[index]->Name()) == 0) {
				return(ParticleType(index));
			}
		}
		ParticleTypeClass *ptr = new ParticleTypeClass(name);
		return(ParticleType(ParticleTypes.ID(ptr)));

	}
	return(PARTICLE_NONE);
}

// FUNCTION: 0x006454C0
const char * ParticleTypeClass::Name_From(ParticleType particle)
{
	if ((unsigned)particle < (unsigned)ParticleTypes.Count()) {
		return(ParticleTypes[particle]->Name());
	}
	return(NULL);
}

// FUNCTION: 0x006454E0
void ParticleTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(MaxDC);
	crc(MaxEC);
	crc(Damage);
	crc(StartFrame);
	crc(NumLoopFrames);
	crc(Translucency);
	crc(WindEffect);
	crc(Velocity);
	crc(Deacc);
	crc(Radius);
	crc(DeleteOnStateLimit);
	crc(EndStateAI);
	crc(StartStateAI);
	crc(StateAIAdvance);
	crc(Translucent25State);
	crc(Translucent50State);
	crc(IsNormalized);
	crc(NextParticle);
	crc(BehavesLike);
	crc(ColorSpeed);
}

// FUNCTION: 0x00645620
HRESULT STDMETHODCALLTYPE ParticleTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParticleTypeClass;
	return(S_OK);
}

// FUNCTION: 0x00645660
HRESULT STDMETHODCALLTYPE ParticleTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) ParticleTypeClass(NoInitClass());

		Swizzle_Pointer(&Warhead);

		int count = 0;

		HRESULT hr = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(hr);
		}

		for (int index = 0; index < count; index++) {
			RGBClass obj;
			hr = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(hr)) {
				return(hr);
			}
			ColorList.Add(obj);
		}

		Fetch_Normal_Image();

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006457A0
HRESULT STDMETHODCALLTYPE ParticleTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;
		int count = ColorList.Count();

		HRESULT hr = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(hr);
		}

		for (index = 0; index < count; index++) {
			hr = stream->Write(&ColorList[index], sizeof(ColorList[index]), NULL);
			if (FAILED(hr)) {
				return(S_OK);
			}
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00645820
ParticleTypeClass * ParticleTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<ParticleTypeClass>(name, ParticleTypes));
}

// FUNCTION: 0x006458B0
void ParticleTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (Warhead == target) {
		Warhead = NULL;
	}
}

// FUNCTION: 0x006458D0
ParticleBehaviorType Particle_Behavior_From_Name(char const * name)
{
	static char const * _BehaviorNames[BEHAVIOR_COUNT] = {
		"Gas",
		"Smoke",
		"Fire",
		"Spark",
		"Railgun",
		"Web",
		"WeakGas"
	};

	for (ParticleBehaviorType behavior = BEHAVIOR_FIRST; behavior < BEHAVIOR_COUNT; behavior++) {
		if (!strcmpi(name, _BehaviorNames[behavior])) {
			return(behavior);
		}
	}
	return(BEHAVIOR_NONE);
}

// FUNCTION: 0x00645910
int ParticleTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00645920
RTTIType ParticleTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_PARTICLETYPE);
}

// FUNCTION: 0x00645930
bool ParticleTypeClass::Create_And_Place(Cell const & , HouseClass * house) const
{
	return(false);
}

// FUNCTION: 0x00645940
ObjectClass * ParticleTypeClass::Create_One_Of(HouseClass *) const
{
	return(NULL);
}

// FUNCTION: 0x00645950 ParticleTypeClass::~ParticleTypeClass
