// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "waypoint.h"
#include "crc.h"
#include "noinit.h"
#include "sun.h"
#include "tracker.h"
#include "vector.h"

// SYNTHETIC: GAMEMD 0x00763380 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x007633B0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x007633D0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x007633F0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00763410 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00763430 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00763450 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00763490 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x007634C0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x007634F0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00763520 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00763530 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00763550 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00763570 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x00763590 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x007635D0 SYMBOL
// _$E51
DynamicVectorClass<WaypointPathClass *> WaypointPaths;

// FUNCTION: GAMEMD 0x00763610
const char *Waypoint_To_Name(WAYPOINT wp)
{
	static char _string[4];

	if (wp == -1) {
		_string[0] = '\0';
		return(_string);
	}

	const int num_chars = ('Z' - 'A') + 1;

	if (wp < num_chars) {
		wsprintf(_string, "%c", wp + 'A');
		return(_string);
	}

	wsprintf(_string, "%c%c", (wp / num_chars) + ('A' - 1), (wp % num_chars) + 'A');
	return(_string);
}

// FUNCTION: GAMEMD 0x00763690
WAYPOINT Waypoint_From_Name(const char *string)
{
	WAYPOINT wp = -1;

	const int num_chars = ('Z' - 'A') + 1;

	if (isalpha(string[0])) {
		wp = toupper(string[0]) - 'A';

		if (isalpha(string[1])) {
			wp = toupper(string[1]) + (wp * num_chars) - ('A' - num_chars);
		}
	}

	return(wp);
}

// FUNCTION: GAMEMD 0x007636F0
WaypointClass::WaypointClass(void) :
	Location(0,0,0)
{
}

// FUNCTION: GAMEMD 0x00763700
WaypointClass::WaypointClass(Coord const & coord) :
	Location(coord)
{
}

// FUNCTION: GAMEMD 0x00763720
WaypointClass::~WaypointClass(void)
{
}

// FUNCTION: GAMEMD 0x00763730
WaypointPathClass::WaypointPathClass(void) :
	BASECLASS(),
	CurrentWaypoint(-1),
	Waypoints()
{
	WaypointPaths.Add(this);
}

// FUNCTION: GAMEMD 0x007637D0
WaypointPathClass::WaypointPathClass(NoInitClass const & x) :
	BASECLASS(x),
	Waypoints(x)
{
}

// FUNCTION: GAMEMD 0x00763810
WaypointPathClass::WaypointPathClass(int index) :
	BASECLASS(),
	CurrentWaypoint(-1),
	Waypoints()
{
	if (index >= 0 && index < 12) {
		WaypointPaths.Add(this);
	}
}

// FUNCTION: GAMEMD 0x007638C0
WaypointPathClass::~WaypointPathClass(void)
{
	Detach_This_From_All(this);
	WaypointPaths.Delete(this);
}

// FUNCTION: GAMEMD 0x00763980
WaypointClass * WaypointPathClass::Get_Waypoint(int index)
{
	if (index >= 0 && index < Waypoints.Count()) {
		return(&Waypoints[index]);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x007639A0
bool WaypointPathClass::Add_Waypoint(Coord const & coord)
{
	Coord snapped = coord.As_Cell().As_Coord();
	snapped.Z = coord.Z;
	if (CurrentWaypoint == -1) {
		Waypoints.Add(WaypointClass(snapped));
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00763A50
bool WaypointPathClass::Select_Waypoint(Coord const & coord)
{
	for (int i = 0; i < Waypoints.Count(); i++) {
		if (Waypoints[i].Location.As_Cell() == coord.As_Cell()) {
			CurrentWaypoint = i;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00763AF0
void WaypointPathClass::Replace_Waypoint(int index, Coord const & coord)
{
	WaypointClass * wp = Get_Waypoint(index);
	if (wp != NULL) {
		wp->Location = coord;
	}
}

// FUNCTION: GAMEMD 0x00763B30
void WaypointPathClass::Delete_Waypoint(int index)
{
	if (index >= 0 && index < Waypoints.Count()) {
		if (index == CurrentWaypoint || index == Waypoints.Count() - 1) {
			CurrentWaypoint = -1;
		}
		Waypoints.Delete_Index(index);
	}
}

// FUNCTION: GAMEMD 0x00763BA0
WaypointClass * WaypointPathClass::Get_Next_Waypoint(WaypointClass * wp)
{
	int index = Waypoints.ID(wp) + 1;
	if (CurrentWaypoint != -1 && index == Waypoints.Count()) {
		index = CurrentWaypoint;
	}
	return(Get_Waypoint(index));
}

// FUNCTION: GAMEMD 0x00763BE0
void WaypointPathClass::Clear(void)
{
	Waypoints.Clear();
	CurrentWaypoint = -1;
}

// FUNCTION: GAMEMD 0x00763C00
void WaypointPathClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(CurrentWaypoint);
	crc(Waypoints.Count());
}

// FUNCTION: GAMEMD 0x00763C30
HRESULT STDMETHODCALLTYPE WaypointPathClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WaypointPath;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00763C70
HRESULT STDMETHODCALLTYPE WaypointPathClass::Load(IStream *stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) WaypointPathClass(NoInitClass());

		int count;

		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		new (&Waypoints) DynamicVectorClass<WaypointClass>;

		for (int index = 0; index < count; index++) {
			WaypointClass obj;
			result = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(result)) {
				return(result);
			}
			Waypoints.Add(obj);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00763D90
HRESULT STDMETHODCALLTYPE WaypointPathClass::Save(IStream *stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;

		int count = Waypoints.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			result = stream->Write(&Waypoints[index], sizeof(Waypoints[index]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00763E00
int WaypointPathClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00763E10
RTTIType WaypointPathClass::Fetch_RTTI(void) const
{
	return(RTTI_WAYPOINT);
}

// SYNTHETIC: GAMEMD 0x00763E20
// ??_GWaypointPathClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00763EF0
// ?Clear@?$DynamicVectorClass@VWaypointClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00763F30
// ?ID@?$DynamicVectorClass@VWaypointClass@@@@UAEHPBVWaypointClass@@@Z
// TEMPLATE: GAMEMD 0x00763F60
// ?GetItem@?$VectorClass@VWaypointClass@@@@UBE?AVWaypointClass@@H@Z
template class DynamicVectorClass<WaypointClass>;

// TEMPLATE: GAMEMD 0x00763F90
// ?Clear@?$DynamicVectorClass@PAVWaypointPathClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00763FC0
// ?ID@?$DynamicVectorClass@PAVWaypointPathClass@@@@UAEHPBQAVWaypointPathClass@@@Z
// TEMPLATE: GAMEMD 0x00763FE0
// ?GetItem@?$VectorClass@PAVWaypointPathClass@@@@UBEPAVWaypointPathClass@@H@Z
// SYNTHETIC: GAMEMD 0x00763FF0
// ??_G?$DynamicVectorClass@VWaypointClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00764050
// ??_G?$VectorClass@VWaypointClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x007640B0
// ??8?$VectorClass@VWaypointClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00764110
// ?Resize@?$VectorClass@VWaypointClass@@@@UAE_NHPBVWaypointClass@@@Z
// TEMPLATE: GAMEMD 0x00764220
// ?Clear@?$VectorClass@VWaypointClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00764260
// ?ID@?$VectorClass@VWaypointClass@@@@UAEHABVWaypointClass@@@Z
// TEMPLATE: GAMEMD 0x007642B0
// ?ID@?$VectorClass@VWaypointClass@@@@UAEHPBVWaypointClass@@@Z
// TEMPLATE: GAMEMD 0x007642E0
// ?Resize@?$DynamicVectorClass@VWaypointClass@@@@UAE_NHPBVWaypointClass@@@Z
// TEMPLATE: GAMEMD 0x00764420
// ?ID@?$DynamicVectorClass@VWaypointClass@@@@UAEHABVWaypointClass@@@Z
// TEMPLATE: GAMEMD 0x00764460
// ??8?$VectorClass@PAVWaypointPathClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x007644A0
// ?Resize@?$VectorClass@PAVWaypointPathClass@@@@UAE_NHPBQAVWaypointPathClass@@@Z
// TEMPLATE: GAMEMD 0x00764550
// ?Clear@?$VectorClass@PAVWaypointPathClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00764580
// ?ID@?$VectorClass@PAVWaypointPathClass@@@@UAEHABQAVWaypointPathClass@@@Z
// TEMPLATE: GAMEMD 0x007645C0
// ?ID@?$VectorClass@PAVWaypointPathClass@@@@UAEHPBQAVWaypointPathClass@@@Z
// TEMPLATE: GAMEMD 0x007645E0
// ?Resize@?$DynamicVectorClass@PAVWaypointPathClass@@@@UAE_NHPBQAVWaypointPathClass@@@Z
// TEMPLATE: GAMEMD 0x00764690
// ?ID@?$DynamicVectorClass@PAVWaypointPathClass@@@@UAEHABQAVWaypointPathClass@@@Z
// SYNTHETIC: GAMEMD 0x007646C0
// ??_G?$VectorClass@PAVWaypointPathClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00764710
// ??_G?$DynamicVectorClass@PAVWaypointPathClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00764760
// ??0?$VectorClass@VWaypointClass@@@@QAE@HPBVWaypointClass@@@Z
template class DynamicVectorClass<WaypointPathClass *>;
template class VectorClass<WaypointPathClass *>;
template class VectorClass<WaypointClass>;
