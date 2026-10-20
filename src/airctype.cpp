// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "airctype.h"
#include "_mixfile.h"
#include "_rules.h"
#include "aircraft.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "mixfile.h"
#include "rules.h"
#include "sun.h"
#include "tracker.h"
#include "animtype.h"
#include "swizzle.h"
#include "ccini.h"
#include "techtype.h"
#include <math.h>

extern char DAT_00889F64[];
extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E2928;

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

void const * AircraftTypeClass::LRotorData = NULL;
void const * AircraftTypeClass::RRotorData = NULL;

// FUNCTION: 0x0041C8B0
AircraftTypeClass::AircraftTypeClass(char const * ininame) :
	BASECLASS(ininame, SPEED_WINGED),
	IsLandable(false),
	IsRotorEquipped(false),
	IsRotorCustom(false),
	IsCarryall(false),
	HeapID(AIRCRAFT_NONE),
	IsFlyBy(false),
	IsFlyBack(false),
	IsAirportBound(false),
	IsFighter(false)
{
	Create_ID();

	AircraftTypes.Add(this);
	HeapID = AircraftType(AircraftTypes.ID(this));

	Parasiteable = true;
	ConsideredAircraft = true;
	IsMoveToShroud = false;
	Rotation = 32;
	Trailer = NULL;
	SpawnDelay = 3;
	ImmuneToPsionics = false;
	ImmuneToPsionicWeapons = false;
	ImmuneToPoison = false;
	Organic = false;
	Bunkerable = false;
}

// FUNCTION: 0x0041C9F0
AircraftTypeClass::AircraftTypeClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x0041CA20
AircraftTypeClass::~AircraftTypeClass(void)
{
	Detach_This_From_All(this, true);
	AircraftTypes.Delete(this);
}

// FUNCTION: 0x0041CAA0
AircraftType AircraftTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (AircraftType classid = AIRCRAFT_FIRST; classid < AircraftTypes.Count(); classid++) {
			if (stricmp(AircraftTypes[classid]->Name(), name) == 0) {
				return(classid);
			}
		}
	}
	return(AIRCRAFT_NONE);
}

// FUNCTION: 0x0041CAF0
void AircraftTypeClass::One_Time(void)
{
	LRotorData = FileSystem::LoadFile("LROTOR.SHP", false);
	RRotorData = FileSystem::LoadFile("RROTOR.SHP", false);
}

// FUNCTION: 0x0041CB20
ObjectClass * AircraftTypeClass::Create_One_Of(HouseClass * house) const
{
	return(new AircraftClass(this, house));
}

// FUNCTION: 0x0041CB50
bool AircraftTypeClass::CanUseWaypoint(void) const
{
	return(false);
}

// FUNCTION: 0x0041CB60
bool AircraftTypeClass::CanAttackMove(void) const
{
	return(false);
}

// FUNCTION: 0x0041CB70
Cell const * AircraftTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0,0), REFRESH_EOL};
	return(_list);
}

// FUNCTION: 0x0041CBD0
void AircraftTypeClass::Init(TheaterType) {}

// FUNCTION: 0x0041CBE0
bool AircraftTypeClass::Create_And_Place(Cell const &, HouseClass *) const
{
	return(false);
}

// FUNCTION: 0x0041CBF0
Point3D AircraftTypeClass::Lepton_Dimensions(void) const
{
	return(Point3D(CELL_LEPTON_W, CELL_LEPTON_H, LEVEL_LEPTON_H * 2));
}

// FUNCTION: 0x0041CC20
bool AircraftTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {

		IsLandable = ini.Get_Bool(Name(), "Landable", IsLandable);
		IsAirportBound = ini.Get_Bool(Name(), "AirportBound", IsAirportBound);
		IsFighter = ini.Get_Bool(Name(), "Fighter", IsFighter);
		IsCarryall = ini.Get_Bool(Name(), "Carryall", IsCarryall);
		IsRotorEquipped = ArtINI.Get_Bool(Graphic_Name(), "Rotors", IsRotorEquipped);
		IsRotorCustom = ArtINI.Get_Bool(Graphic_Name(), "CustomRotor", IsRotorCustom);
		char buffer[128];
		AnimTypeClass * trailer = Trailer;
		if (ArtINI.Get_String(Graphic_Name(), "Trailer", DAT_00889F64, buffer, sizeof(buffer))) {
			trailer = AnimTypeClass::Find_Or_Make(buffer);
		}
		Trailer = trailer;
		SpawnDelay = ArtINI.Get_Int(Graphic_Name(), "SpawnDelay", SpawnDelay);
		IsFlyBy = ini.Get_Bool(Name(), "FlyBy", IsFlyBy);
		IsFlyBack = ini.Get_Bool(Name(), "FlyBack", IsFlyBack);
		return(true);
	}
	return(false);
}


// FUNCTION: 0x0041CDB0
void AircraftTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(IsRotorEquipped);
	crc(IsRotorCustom);
	crc(IsLandable);
	crc(IsAirportBound);
	crc(IsFighter);
	crc(HeapID);
}

// FUNCTION: 0x0041CE20
HRESULT STDMETHODCALLTYPE AircraftTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) AircraftTypeClass(NoInitClass());

		Swizzle_Pointer(&Trailer);

		Fetch_Voxel_Image();
		Fetch_Normal_Image();

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0041CE90
HRESULT STDMETHODCALLTYPE AircraftTypeClass::Save(IStream *stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0041CEB0
HRESULT STDMETHODCALLTYPE AircraftTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AircraftTypeClass;
	return(S_OK);
}

// FUNCTION: 0x0041CEF0
AircraftTypeClass * AircraftTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<AircraftTypeClass>(name, AircraftTypes));
}

// FUNCTION: 0x0041CF80 ObjectTypeClass::Coord_Fixup

// FUNCTION: 0x0041CFA0 ObjectTypeClass::Get_Image_Data

// FUNCTION: 0x0041CFB0 AircraftTypeClass::Fetch_RTTI

// FUNCTION: 0x0041CFC0
int AircraftTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(AircraftTypeClass));
}

// FUNCTION: 0x0041CFD0 AircraftTypeClass::Fetch_Heap_ID

// FUNCTION: 0x0041CFE0 AircraftTypeClass::~AircraftTypeClass

// FUNCTION: 0x0041D070 $E36
double DAT_00889F88 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0041D0A0 $E39
double DAT_00889FF0 = DAT_00889F88 / DAT_007E2928;

// FUNCTION: 0x0041D0C0 $E42
double DAT_00889FD8 = DAT_007E1718 / DAT_00889FF0;
